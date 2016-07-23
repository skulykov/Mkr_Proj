package com.mkr.navgl.service;


import java.util.Timer;
import android.util.Log;

public class MkRlNativeTimerManager 
{
	 public MkRlNativeTimerManager(MkRlNativeManager aNativeManager)
	    {

	        // Native manager context
	        mNativeManager = aNativeManager;

	        // Timer allocation
	        mTimer = mNativeManager.getTimer();

	        // Allocate the array list
	        mTasks = new MkRlNativeTimerTask[MKR_MAX_TASK_COUNT];

	        // At this stage the native library has to be loaded
	        InitTimerManagerNTV();

	    }

	    /*************************************************************************************************
	     * Adding the timer task to the scheduler
	     * 
	     * @param aTaskId
	     *            - the task id
	     * @param aTaskNativeMsg
	     *            - the message to be posted to the activity to upon timer's
	     *            timeout
	     * @param aInterval
	     *            - the timeout period
	     */
	    public void AddTask( int aTaskId, int aTaskNativeMsg, int aInterval )
	    {
	        // Log.i("AddTask"," Index: " + String.valueOf( aTaskId ) +
	        // " Interval: " + String.valueOf( aInterval ) + ". Now: " +
	        // String.valueOf( (new Date()).getTime() ) );      
	        // Check bounds
	        
	        // Don't handle the delayed requests while shutting down
	        if (  mNativeManager.isShuttingDown() ) 
	            return;
	        
	        if (aTaskId > MKR_MAX_TASK_COUNT - 1 || aTaskId < 0)
	        {
	            Log.w( LOG_TAG , "Task ID is illegal: " + aTaskId );
	            return;
	        }
	        // Interval 0 and lower is illegal too ...
	        if (aInterval <= 0)
	        {
	            Log.w( LOG_TAG , "Interval is illegal: " + aInterval );
	        }

	        // Be careful here to not enter to the block
	        synchronized (mTasks)
	        {
	            // Check if there is an active task at this index
	            if (mTasks[aTaskId] != null)
	            {
	                // Log.d( "AddTimer", "There is an active task at index" +
	                // String.valueOf( aTaskId ) );
	                // Canceling the task
	                RemoveTask(aTaskId);
	            }
	            // Create and initialize the new timer and its task
	            mTasks[aTaskId] = new MkRlNativeTimerTask( aTaskId,
	                    aTaskNativeMsg, aInterval, mNativeManager );
	        }
	        // Run it ....
	        StartTask( aTaskId, aInterval );
	    }

	    /*************************************************************************************************
	     * Task removal according to its id
	     * 
	     * @param aTaskId
	     *            - the task id
	     * 
	     * 
	     */
	    public void RemoveTask( int aTaskId )
	    {
	        // Log.i("RemoveTask", "Id : " + String.valueOf( aTaskId ) + ". Now: " +
	        // String.valueOf( (new Date()).getTime() ) );
	        
	        // Don't handle the delayed requests while shutting down
	        if (  mNativeManager.isShuttingDown() ) 
	            return;
	        
	        if (aTaskId >= 0 && aTaskId < MKR_MAX_TASK_COUNT)
	        {
	            // Cancel the task
	            synchronized (mTasks)
	            {
	                mTasks[aTaskId].cancel();
	                mTasks[aTaskId] = null;
	            }
	        }
	    }

	    /*************************************************************************************************
	     * Starting the task according to its id (if exists) with the given period
	     * 
	     * @param aTaskId
	     *            - the task id
	     * @param aInterval
	     *            - the timeout period
	     */
	    public void StartTask( int aTaskId, long aInterval )
	    {
	        // Schedule the timer
	        if ( mTasks[aTaskId] != null )
	        {
	            try
	            {
	                mTimer.scheduleAtFixedRate( mTasks[aTaskId], aInterval,
	                        aInterval );
	            }
	            catch (Exception ex)
	            {
	                Log.w( "Start Task Error!  TaskId: " + aTaskId
	                        + "Interval: " + aInterval, ex );
	            }
	        }
	        else
	        {
	            Log.i( LOG_TAG, "StartTask. Task: " + String.valueOf(aTaskId)
	                    + " is not active" );
	        }
	    }

	    /*************************************************************************************************
	     * Closing the tasks Closing the timer - its impossible to schedule new task
	     * 
	     */
	    public void ShutDown()
	    {
	        Log.i(LOG_TAG, "Shutting down the timer manager" );
	        synchronized (mTasks)
	        {
	            // Pass through the timers and close the active ones
	            for (int lTask = 0; lTask < MKR_MAX_TASK_COUNT; ++lTask)
	            {
	                if (mTasks[lTask] != null)
	                {
	                    mTasks[lTask].cancel();
	                }
	            }
	            mTimer.cancel();
	        }
	    }

	    /*************************************************************************************************
	     * Returns true if there is active tasks
	     * 
	     */
	    public boolean ActiveTasksExist()
	    {
	        return (ActiveTasksCount() > 0);
	    }

	    /*************************************************************************************************
	     * Counts the active timers and returns the result
	     * 
	     */
	    public int ActiveTasksCount()
	    {
	        int lCount = 0;
	        synchronized (mTasks)
	        {
	            // Pass through the timers and close the active ones
	            for (int lTask = 0; lTask < MKR_MAX_TASK_COUNT; ++lTask)
	            {
	                if (mTasks[lTask] != null)
	                {
	                    lCount++;
	                }
	            }
	        }
	        return lCount;
	    }

	    /*************************************************************************************************
	     *================================= Native methods section ================================= 
	     * These methods are implemented in the
	     * native side and should be called after!!! the shared library is loaded
	     */
	    private native void InitTimerManagerNTV();

	    /*************************************************************************************************
	     *================================= Private interface section =================================
	     */

	    /*************************************************************************************************
	     *================================= Data members section
	     * =================================
	     * 
	     */
	    private final static int       MKR_MAX_TASK_COUNT = 32;
	    private MkRlNativeTimerTask mTasks[];
	    private Timer                  mTimer;
	    private MkRlNativeManager   mNativeManager;
	    private final String           LOG_TAG               = "TimerManager";
}
