package com.mkr.navgl.service;

import java.util.ArrayList;
import java.util.Timer;

import com.mkr.navgl.nintf.MkrNavLib;

import android.content.res.AssetManager;
import android.os.Debug;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;

import android.view.MotionEvent;


public final class MkRlNativeManager 
{
	 // Represents the UI events
    public enum MkRlUIEvent
    {
        UI_EVENT_START, // Start application message
        UI_EVENT_FORCE_NEW_CANVAS, // Force new canvas
        UI_EVENT_KEY_DOWN, // Key down event
        UI_EVENT_TOUCH, // Touch event
        UI_EVENT_STARTUP_NOSDCARD, // Starting up with no sdcard
        UI_EVENT_STARTUP_GPUERROR, // Error access GPU
        UI_EVENT_LOW_MEMORY, // Low memory warning
        UI_EVENT_SCREENSHOT, // Screen shot manager request
        UI_EVENT_NATIVE,
        UI_PRIORITY_EVENT_NATIVE; 

        // Get the UI event
        public static MkRlUIEvent FromInt( int aCode )
        {
            return MkRlUIEvent.values()[aCode];
        }

        // Get the code for the UI event
        public static int ToInt( MkRlUIEvent aEvent )
        {
            return aEvent.ordinal();
        }

    }
	
    
    static public MkRlNativeManager Start()
    {
    	
    	
    	if (mNativeManager != null)
            return mNativeManager;

        // Creating the singleton instance
        mNativeManager = new MkRlNativeManager();
	    // Location management
        mNativeManager.InitGPS();
        
        AssetManager assetMgr = MkRlAppService.getAppContext().getAssets();
        createAssetManager(assetMgr);  //initialize native AssetManager reference
        
        // Starting the context thread
        mNativeThread = mNativeManager.new NativeThread("NVThread");
        mNativeThread.start();

        while (!mNativeManager.mAppLooperReady);            
        
        mNativeManager.PostUIMessage(MkRlUIEvent.UI_EVENT_START);

        if ( TEMPERATURE_PROFILER_ENABLED )
        	mNativeManager.InitTemperatureProfiler();
        
        MkrNavLib.inititializeOnStart();             // init AndroidFileIO 
        return mNativeManager;
    }

   

	private MkRlNativeManager()
    {
       // mUITouchEventQueue = new ArrayList<MotionEvent>();
       // mUIKeyEventQueue = new ArrayList<KeyEvent>();
        mPriorityEventQueue = new ArrayList<Message>();
    }
	
    //************************************************************************************************
    /*************************************************************************************************
     * Interface function for the async native messages to be posted to the
     * current thread. Adds the message to the mNativeMsgDispatcher queue
     * 
     * @param aMsgId
     *            - Message id of the posted message
     * 
     */
    public void PostNativeMessage( int aMsgId )
    {
        Message msg = Message.obtain( mUIMsgDispatcher, 
                            MkRlUIEvent.ToInt(MkRlUIEvent.UI_EVENT_NATIVE), aMsgId, 0 );
        msg.sendToTarget();
    }
    
    /*************************************************************************************************
     * Interface function for the async native messages to be posted to the
     * current thread. Adds the message to the mNativeMsgDispatcher queue 
     * 
     * @param aMsgId - Message id of the posted message
     * @param aObjParam - object parameter reference
     * 
     */
    public void PostNativeMessage( int aMsgId, IMkRlMessageParam aObjParam )
    {
        Message msg = Message.obtain( mUIMsgDispatcher, 
                            MkRlUIEvent.ToInt(MkRlUIEvent.UI_EVENT_NATIVE), aMsgId, 0 );
        msg.obj = aObjParam;
        msg.sendToTarget();
    }

    /*************************************************************************************************
     * Interface function for the async native messages to be posted to the
     * current thread. Adds the message to the priority queue 
     * 
     * @param aMsgId - Message id of the posted message
     * @param aObjParam - object parameter reference
     * 
     */
    public void PostPriorityNativeMessage( int aMsgId, IMkRlMessageParam aObjParam )
    {
        Message msg = Message.obtain( mUIMsgDispatcher, 
                            MkRlUIEvent.ToInt(MkRlUIEvent.UI_PRIORITY_EVENT_NATIVE), aMsgId, 0 );
        msg.obj = aObjParam;
        
        synchronized (mPriorityEventQueue) {	
        	mPriorityEventQueue.add(msg);
        }
        
        msg.sendToTarget();
    }
    
    /*************************************************************************************************
     * Interface function for the async UI messages to be posted to the current
     * thread. Adds the message to the mUIMsgDispatcher queue
     * 
     * @param aMsgId
     *            - Message id of the posted message
     * 
     */
    public void PostUIMessage( MkRlUIEvent aEvent )
    {
        // Log.i( "Posting message", "Thread ID: " + String.valueOf(
        // Thread.currentThread().getId() ) + " Posting message: " +
        // String.valueOf( MkRlUIEvent.ToInt( aEvent ) ) );
        // Posting a message
        Message.obtain( mUIMsgDispatcher, MkRlUIEvent.ToInt(aEvent) ).sendToTarget();
    }
    /*************************************************************************************************
     * Interface function for the async UI messages to be posted to the current
     * thread. Adds the message to the mUIMsgDispatcher queue
     * 
     * @param aMsgId
     *            - Message id of the posted message
     * @param aParam            
     * 			  - Additional message parameter
     */
    public void PostUIMessage( MkRlUIEvent aEvent, int aParam )
    {
        // Log.i( "Posting message", "Thread ID: " + String.valueOf(
        // Thread.currentThread().getId() ) + " Posting message: " +
        // String.valueOf( MkRlUIEvent.ToInt( aEvent ) ) );
        // Posting a message
        Message msg = Message.obtain( mUIMsgDispatcher, MkRlUIEvent.ToInt(aEvent) );
        msg.arg1 = aParam;
        msg.sendToTarget();
    }
    /*************************************************************************************************
     * async UI messages creator
     * thread. Creates message that can be sent to the native thread
     * 
     * @param aMsgId
     *            - Message id of the posted message
     * @param arg0 - message param #1           
     * @param arg1 - message param #2
     */
    public Message GetUIMessage( MkRlUIEvent aEvent )
    {
        // Log.i( "Posting message", "Thread ID: " + String.valueOf(
        // Thread.currentThread().getId() ) + " Posting message: " +
        // String.valueOf( MkRlUIEvent.ToInt( aEvent ) ) );
        // Posting a message
        return Message.obtain( mUIMsgDispatcher, MkRlUIEvent.ToInt(aEvent) );
    }

    
    /*************************************************************************************************
     * Post
     * Posts the runnable on the main dispatcher queue ( at the end )
     */
    public static void Post( Runnable aMsg )
    {    	
    	if ( mNativeManager != null )
    	{
    		mNativeManager.PostRunnable( aMsg );
    	}    	
    }
    /*************************************************************************************************
     * PostRunnable
     * Posts the runnable on the main dispatcher queue ( at the end )
     */
    public void PostRunnable( Runnable aMsg )
    {    	
    	if ( mNativeThread.isAlive() )
    		mUIMsgDispatcher.post( aMsg );
    }
    
    
    /*************************************************************************************************
     * This class implements the message handler loop for the current thread
     * 
     */
    public final class NativeThread extends HandlerThread
    {
        public NativeThread(String aThreadName)
        {
            super(aThreadName, NATIVE_THREAD_PRIORITY);
            // this.setPriority( MAX_PRIORITY );
        }

        @Override
        protected void onLooperPrepared()
        {
            // UI message dispatcher
            mUIMsgDispatcher = new MkRlUIMsgDispatcher();

//            Log.i(LOG_TAG, "Starting native thread. Id: "
//                    + String.valueOf(this.getId()));
            mAppLooperReady = true;
        }

    }
    /*************************************************************************************************
     * This class implements the UI message handler loop for the current thread
     * 
     */
    public class MkRlUIMsgDispatcher extends Handler
    {

        @Override
        public void handleMessage( Message aMsg )
        {
            if (  isShuttingDown() ) // Don't handle the message during the shutdown
                return;
            
            // First handle all the proirity events
            handlePriorityEvents();
            
            MkRlUIEvent lEvent = MkRlUIEvent.FromInt(aMsg.what);
            long startTime = System.currentTimeMillis();
            long delta;
            String eventName = "";
            
            switch (lEvent)
            {
                case UI_EVENT_START:
                {
                    AppStart();
                    break;
                }
                case UI_EVENT_STARTUP_GPUERROR:
                {
            		//AppLayerShutDown();                	
            		break;
                }
                case UI_EVENT_STARTUP_NOSDCARD:
                {
                	/*if ( CheckSDCardAvailable() )
                	{
                		AppStart();
                	}*/
                    break;
                }
                case UI_EVENT_SCREENSHOT:
                {
                	//( new WazeScreenShotManager() ).Capture( getMainView(), mNativeCanvas.getGL() );
                    break;
                }

                case UI_EVENT_FORCE_NEW_CANVAS:
                {
                    break;
                }
                case UI_EVENT_LOW_MEMORY:
                {
                	String memory_usage = new String( "Memory usage native heap. Used: " + Debug.getNativeHeapAllocatedSize() +
    		                ". Free: " + Debug.getNativeHeapFreeSize() +
    		                ". Total: " + Debug.getNativeHeapSize() );
                	Log.w(LOG_TAG, new String( "Android system reported low memory !!! " ) + memory_usage );
                	break;
                }
                	
                case UI_EVENT_KEY_DOWN:
                case UI_EVENT_TOUCH:
                case UI_PRIORITY_EVENT_NATIVE:
                	// These are handled in the priority handler
                    break;
                    
                case UI_EVENT_NATIVE:
                {
                    // 1. If the object parameter is not passed - the assumption is that the message 
                    //    is active
                    // 2. If the object parameter is passed - check if the object is active
                    boolean isActive = true;
                    IMkRlMessageParam prm = (IMkRlMessageParam) aMsg.obj;
                    if ( prm != null )
                    {
                        isActive = prm.IsActive();
                        eventName = "Timer Event";
                    }
                    else
                    {
                    	eventName = "IO event";
                    }
                    if ( isActive )
                    {
                        NativeMsgDispatcherNTV( aMsg.arg1 );
                    }
                    break;
                }
            }
            delta = System.currentTimeMillis() - startTime;
            
            if ( delta > 500 && getInitializedStatus() )
            {
                 Log.w( LOG_TAG, "EXCEPTIONAL TIME FOR " + eventName + " HANDLING TIME: " + delta );
            }
        }

		
		private void handlePriorityEvents() {
			while (true) {
		
				// touch events
				MotionEvent lTouchEvent = null;
				/*
				synchronized (mUITouchEventQueue) {
					if (!mUITouchEventQueue.isEmpty()) {
						lTouchEvent = mUITouchEventQueue.remove(0);
					}
				}
				
				if (lTouchEvent != null) {
					//Handle the event
					mNativeCanvas.OnTouchEventHandler(lTouchEvent);
					continue;
				}

                KeyEvent lKeyEvent = null;
                // Take the element from the queue head
                synchronized (mUIKeyEventQueue)
                {
                	if (!mUIKeyEventQueue.isEmpty()) {
                		lKeyEvent = mUIKeyEventQueue.remove(0);
                	}
                }
                
                if (lKeyEvent != null) {
                	// Handle the event
                	mNativeCanvas.OnKeyDownHandler(lKeyEvent);
                    //eventName = "Key Down Event";
                	continue;
                }
				*/
                Message msg = null;
                synchronized (mPriorityEventQueue) {
					if (!mPriorityEventQueue.isEmpty()) {
						msg = mPriorityEventQueue.remove(0);
					}
				}
                
                if (msg != null) {
                	MkRlUIEvent lEvent = MkRlUIEvent.FromInt(msg.what);
                	long startTime = System.currentTimeMillis();
                	long delta;
                	String eventName = "";
                
                	switch (lEvent)
                	{
                		case UI_PRIORITY_EVENT_NATIVE:
                		{
                			// 1. If the object parameter is not passed - the assumption is that the message 
                			//    is active
                			// 2. If the object parameter is passed - check if the object is active
                			boolean isActive = true;
                			IMkRlMessageParam prm = (IMkRlMessageParam) msg.obj;
                			if ( prm != null )
                			{
                				isActive = prm.IsActive();
                				eventName = "Timer Event";
                			}
                			else
                			{
                				eventName = "IO event";
                			}
                			if ( isActive )
                			{
                				NativeMsgDispatcherNTV( msg.arg1 );
                			}
                		}
                		break;
                		
                		default:
                			Log.e(LOG_TAG,"Unknown priority event - " + lEvent);
                	}
                		
                    delta = System.currentTimeMillis() - startTime;
                        
                    if ( delta > 500 )
                    {
                         Log.w( LOG_TAG, "EXCEPTIONAL TIME FOR " + eventName + " HANDLING TIME: " + delta );
                    }                		
                	
                	continue;
                }
                
                // no events
                break;
			}	
		}
    }//public class MkRlUIMsgDispatcher extends Handler
    
       
    //------------------------------------------------------------------------------
    private void AppStart() 
    {
    	Log.w(LOG_TAG, "Starting the application!!!" );
    	InitNativeManager();
    	
    	AppStartNTV( /*FreeMapAppService.getUrl(), FreeMapAppService.getAppMode()*/ "",0 );
    	mAppStarted = true;	
	}
    private void InitTemperatureProfiler() {
		// TODO Auto-generated method stub
		
	}

	private void InitGPS() {
		// TODO Auto-generated method stub
		
	}
	void InitTtsManager()
	{
		mNativeManager.mTtsManager = new MkRlTtsManager();
		mNativeManager.mTtsManager.InitTtsMngNativeLayer();            
	}
	void initSoundManager()
	{
		mSoundManager=new MkRlSoundManager(this);
	}
	public void InitNativeManager()
    {
    	// Initialize the  timer
        mTimer = new Timer("FreeMap Timer");
        
        /*
        // Load the system settings
        SaveSystemSettings();
        
        // Backlight management
        mBackLightManager = new FreeMapBackLightManager( this );
        mBackLightManager.StartWakeMonitor( FreeMapBackLightManager.WAKE_REFRESH_PERIOD_DEFAULT );
        
        // Extracting the resources if necessary
    	WazeResManager.Prepare();
    	
        // Loading the native library
        LoadNativeLib( WazeResManager.mPkgDir + WazeResManager.mLibFile);
        
        mResManager = WazeResManager.Create();
        */
        final int sdkBuildVersion = Integer.parseInt( android.os.Build.VERSION.SDK );
        
    	// Initialize the library
    	InitNativeManagerNTV( /*WazeResManager.mAppDir*/"", /*sdkBuildVersion*/1, android.os.Build.DEVICE, 
        android.os.Build.MODEL, /*manufacturer*/"" );

        // Message box engine
        MkrMsgBox.InitNativeLayer(); 
        
        
        mAppInitializedFlag = true;
    }
    
    
	 public boolean getInitializedStatus()
	    {
	        return mAppInitializedFlag;
	    }
	
	public Timer getTimer() {
		return mTimer;
	}
	 public boolean isShuttingDown()
	    {
	        return false;//mAppShutDownFlag;
	    }
	 /*************************************************************************************************
	     * TTS manager accessor
	     */
	    public IMkRlTtsManager getTtsManager()
	    {
	    	return mTtsManager;
	    }
	
	//=================================================================
	 /*================================= Native methods section ================================= 
		     * These methods are implemented in the
		     * native side and should be called after!!! the shared library is loaded
		     */
		    
		    private native boolean UrlHandlerNTV( String aUrl );	// Handles custom urls ( true - if handled )
		    
		    private native void NativeMsgDispatcherNTV( int aMsgId );

		    private native void InitNativeManagerNTV( String aLibPath, int aBuildSdkVersion, String aDeviceName,
		    		String aDeviceModel, String aDeviceManufacturer );

		    private native void AppStartNTV( String aUrl, int aAppMode ); // Starts the application
		    
		    private native void AppShutDownNTV(); // Shutdowns the application
		    
		    private native void ShowGpsDisabledWarningNTV(); // Shows the user the warning in case of the disabled GPS

		    private native void BackLightMonitorResetNTV();	// Resets the backlight monitor
		    
		    private native void SetUpgradeRunNTV( byte value );	// Sets if this run is the first(upgrade or installation) run
		    
		    private native void SetBackgroundRunNTV( int value );	// Sets true if the application is currently at the background
		    
		    private native void EditBoxCallbackNTV( int res, String aText, long aCbContext );	// Returns text and action result
		    
		    private native void TakePictureCallbackNTV( int res );	// Returns action result

		    private native boolean EditBoxCheckTypingLockNTV();	// Checks whether the typing is locked 
		    
		    private native void ConnectivityChangedNTV( boolean aConnected, int aConnType, String aConnTypeStr ); // Connectivity changed event callback 
		    
		    private static native void createAssetManager(AssetManager assetMgr);
		    
		    /*************************************************************************************************
		     *================================= Data members section =================================
		     * 
		     */
			
	private static NativeThread mNativeThread;
	private static MkRlNativeManager mNativeManager         = null;
	private Timer                mTimer;                 // Application timer
	
	private static final int NATIVE_THREAD_PRIORITY = -18;
	private static final boolean TEMPERATURE_PROFILER_ENABLED = false;;
	private boolean mAppLooperReady = false;
	private boolean mAppStarted=false;
	private MkRlUIMsgDispatcher mUIMsgDispatcher; // The UI messages dispatcher
	private MkRlNativeTimerManager mTimerManager=null; // Application timers - will be used natively
	private ArrayList<Message> mPriorityEventQueue; // The event queue of the key events
	
	private boolean mAppInitializedFlag=false;
	
	
	private IMkRlTtsManager mTtsManager = null;
	
	private MkRlSoundManager mSoundManager=null; // Sound manager - will be used natively
	
	private final String           LOG_TAG               = "NVManager";
	
}
