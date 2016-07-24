package com.mkr.service;


import android.app.Activity;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class MkrApp {

	static LocationService loc_service=null;
	
	    public static void StartApp( Context aContext, Runnable aResumeEvent )
	    {
	    	MkrNLib.Get().initMQTT();
	    	loc_service=new LocationService();
	    }
	    public static void StopApp( Context aContext, Runnable aResumeEvent )
	    {
	    	
	    }
	   
	   
	    /*************************************************************************************************
	     * Returns the application context reference
	     * 
	     * @param void
	     * @return Context object of the process           
	     */
	    public static Context getAppContext()
	    {
	    	//if ( mInstance != null )
	    	//	return mInstance.getApplicationContext();
	    	
	    	if ( currentOnTopAct != null )
	    		return currentOnTopAct.getApplicationContext();
	    	
			return null;
	    }
	    public static LocationService GetLocationService()
	    {
	    	return loc_service;
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
 
	  //  private static MkrApp    mInstance  = null;
	    private final static String         LOG_TAG  = "MKRSrv";
    


}
