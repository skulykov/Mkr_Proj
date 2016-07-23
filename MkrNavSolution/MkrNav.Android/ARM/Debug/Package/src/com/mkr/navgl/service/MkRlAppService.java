package com.mkr.navgl.service;


import com.mkr.navgl.nintf.MkrNavMainAct;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.os.IBinder;
import android.util.Log;

public class MkRlAppService extends Service 
{

	 @Override
	    public void onCreate()
	    {
	    	super.onCreate();
	    //	Log.w( LOG_TAG, "Service Created. Instance: " + this );
	    }
	 
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}

    @Override
    public void onStart( Intent aIntent, int aStartId )
    {
        super.onStart(aIntent, aStartId);
        // Initialize the service instance
        mInstance = this;
        
       
        // Reduce the possibility to be killed 
        // setForeground( true ); // @deprecated
        
        if ( mResumeEvent != null )
        {
        	mResumeEvent.run();
        }

    //    mConnEventReceiver = new ConnEventReceiver();
    	// Register on the intent providing the battery level inspection
    //	getAppContext().registerReceiver( mConnEventReceiver, new IntentFilter( ConnectivityManager.CONNECTIVITY_ACTION ) );
    	
        Log.w( LOG_TAG, "Service started. Instance: " + mInstance );
        
        mIsStarting = false;
        
        mStartContext = null; // Allow disposing the startup  context upon service creation
    }

    @Override
    public void onDestroy()
    {
       /* if ( mNativeManager != null )
        {
            mNativeManager.RestoreSystemSettings();
//            unregisterReceiver( mStandbyManager );
        }*/
        Log.w( LOG_TAG, "Service destroy." );
    }
    public static void StartApp( Context aContext, Runnable aResumeEvent )
    {
    	StartApp( aContext, aResumeEvent, APP_MODE_NORMAL );
    }
    public static void StartApp( Context aContext, Runnable aResumeEvent, int aAppMode )
    {
        try
        {
        	mIsStarting = true;
        	
        	mStartContext = aContext;
        	
        	//mAppMode = aAppMode;
        	
        	 // Start the native manager
	        mNativeManager = MkRlNativeManager.Start();
	       	        
	        mResumeEvent = aResumeEvent;
	        
	        // Draw splash background ( ApplicationContext should be available. Can be run from onCreate of Activity however
	        // in this case another functions (IsHd) should be aware of the ApplicationContext somehow )
	        if ( !MkRlAppService.IsInitialized() )
            {
            	Intent intent = new Intent( mStartContext, MkRlAppService.class );
            	/*
            	 * Pay attention that onStart event of the service comes later than 
            	 * onResume of activity. In the future the start flow should be in the onStart
            	 * of the service 
            	 */
            	aContext.startService( intent );
            }
	        
        }
        catch( Exception ex )
        {
        	Log.e( "MKR", "Error starting the application " + ex.getMessage() );
        	ex.printStackTrace();
        }

    }
    
    public static boolean IsInitialized()
    {
        return ( mInstance != null );
    }
    /*************************************************************************************************
     * Returns the application context reference
     * 
     * @param void
     * @return Context object of the process           
     */
    public static Context getAppContext()
    {
    	if ( mInstance != null )
    		return mInstance.getApplicationContext();
    	
    	if ( mAppActivity != null )
    		return mAppActivity.getApplicationContext();
    	
    	if ( mStartContext != null )
    		return mStartContext.getApplicationContext();
    	
		return null;
    }
    /*************************************************************************************************
     * The activity being used as a main activity
     */
    public static void initTtsAndSoundService()
    {
    	if(mNativeManager!=null)
    	{
    		mNativeManager.InitTtsManager();
    		mNativeManager.initSoundManager();
    	}
    }
    public static void setAppActivity( MkrNavMainAct aAppActivity )
    {
        mAppActivity = aAppActivity;
    }
    public static MkrNavMainAct getMainActivity()
    {
        return mAppActivity;
    }
    public static MkRlNativeManager getNativeManager()
    {
        return mNativeManager;
    }
    
    public final static int APP_MODE_NORMAL = 0;	
    
    private static MkrNavMainAct mAppActivity;
    private static MkRlNativeManager mNativeManager = null; 
    private static Context mStartContext;			// Context passed to the service during startup
    private static Runnable mResumeEvent = null;
    private static MkRlAppService    mInstance               = null;
    private final static String         LOG_TAG                 = "MKR Service";
    
    private static boolean		mIsStarting = false;
    
}
