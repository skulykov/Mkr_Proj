package com.mkr.service;


import android.app.Activity;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class MkrAppService extends Service {

	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}
	 @Override
	    public void onCreate()
	    {
	    	super.onCreate();
	    //	Log.w( LOG_TAG, "Service Created. Instance: " + this );
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
	        	// Start the native manager
		     //   mNativeManager = MkrNVMng.Start();
	        	MkrNLib.Get().initOnStart();       
		        mResumeEvent = aResumeEvent;
		        
		        // Draw splash background ( ApplicationContext should be available. Can be run from onCreate of Activity however
		        // in this case another functions (IsHd) should be aware of the ApplicationContext somehow )
		        if ( !MkrAppService.IsInitialized() )
	            {
	            	Intent intent = new Intent( mStartContext, MkrAppService.class );
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
	    	
	    	if ( currentOnTopAct != null )
	    		return currentOnTopAct.getApplicationContext();
	    	
	    	if ( mStartContext != null )
	    		return mStartContext.getApplicationContext();
	    	
			return null;
	    }
	    /*************************************************************************************************
	     * The activity being used as a main activity
	     */
	   
	    public static void SetCurrentOnTopActivity( Activity act )
	    {
	        currentOnTopAct = act;
	    }
	    public static Activity GetCurrentOnTopActivity()
	    {
	        return currentOnTopAct;
	    }
	    public static void SetMainActivity(Activity act)
		{
			main_act=act;
		}

		public static Activity GetMainActivity ()
		{
			return main_act;
		} 
	    
	   	  
	    
	    public final static int APP_MODE_NORMAL = 0;	
	    
	    private static Activity currentOnTopAct;                // current on top activity
	    private static Activity main_act;
	    
	 
	    private static Context mStartContext;			// Context passed to the service during startup
	    private static Runnable mResumeEvent = null;
	    private static MkrAppService    mInstance               = null;
	    private final static String         LOG_TAG                 = "MKRSrv";
	    
	    private static boolean		mIsStarting = false;
	    


}
