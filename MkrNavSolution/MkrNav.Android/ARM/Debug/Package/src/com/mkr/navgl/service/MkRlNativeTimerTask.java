package com.mkr.navgl.service;
import java.util.TimerTask;

public final class MkRlNativeTimerTask extends TimerTask implements IMkRlMessageParam 
{

		/*************************************************************************************************
	     *================================= Public interface section =================================
	     * 
	     */
	    MkRlNativeTimerTask(int aTaskId, int aTaskNativeMsg,
	            int interval, MkRlNativeManager aNativeManager)
	    {
	        mNativeManager = aNativeManager;
	        mTaskId = aTaskId;
	        mInterval = interval;
	        mTaskNativeMsg = aTaskNativeMsg;
	        mIsActive = true;
	    }

	    /*************************************************************************************************
	     * Message parameter active inspector (interface override)
	     */
	    public boolean IsActive()
	    {
	        return mIsActive;
	    }
	    
	    /*************************************************************************************************
	     * Posting the message to the activity thread upon timeout
	     * 
	     */
	    @Override
	    synchronized public void run()
	    {
	        // Log.i("TimerTask Timeout", "Now: " + String.valueOf( (new
	        // Date()).getTime() ) + " Id: " + String.valueOf( mTaskId ) );
	    	if (mInterval < 100) {
	    		mNativeManager.PostPriorityNativeMessage( mTaskNativeMsg, this );
	    	} else {
	    		mNativeManager.PostNativeMessage( mTaskNativeMsg, this );
	    	}
	    }

	    /*************************************************************************************************
	     * Task id modifier
	     * 
	     */
	    public void setTaskId( int aTaskId )
	    {
	        mTaskId = aTaskId;
	    }

	    /*************************************************************************************************
	     * Task id inspector
	     * 
	     */
	    public int getTaskId()
	    {
	        return mTaskId;
	    }
	    /*************************************************************************************************
	     * task cancel routine override
	     * 
	     */
	    public boolean cancel()
	    {
	        mIsActive = false;
	        return super.cancel();
	    }

	    /*************************************************************************************************
	     * Synchronized task cancel routine
	     * 
	     */
	    synchronized public void CancelSync()
	    {
	        this.cancel();
	    }

	    /*************************************************************************************************
	     *================================= Private interface section
	     * =================================
	     */

	    /*************************************************************************************************
	     *================================= Data members section
	     * =================================
	     * 
	     */
	    private int mTaskId; // The id of the task
		private int mTaskNativeMsg; // The message to be posted to the native layer
	    // on timer expiration
	    private int mInterval; // timer interval
	    private MkRlNativeManager mNativeManager;
	    
	    private boolean mIsActive = false;
	    
	    
}
