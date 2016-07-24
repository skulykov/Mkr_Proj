package com.mkr.unidriver;

import com.mkr.service.MkrNLib;

import android.app.Activity;
import android.app.Application;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;

//http://stackoverflow.com/questions/19897628/need-to-handle-uncaught-exception-and-send-log-file

public class MyApplication extends Application
{
	 public void onCreate ()
	  {
	    // Setup handler for uncaught exceptions.
	    /*
		 Thread.setDefaultUncaughtExceptionHandler (new Thread.UncaughtExceptionHandler()
	    {
	      @Override
	      public void uncaughtException (Thread thread, Throwable e)
	      {
	        handleUncaughtException (thread, e);
	      }
	    });*/
		 
		 //http://stackoverflow.com/questions/6745797/how-to-set-entire-application-in-portrait-mode-only/9784269#9784269
		// register to be informed of activities starting up
	        registerActivityLifecycleCallbacks(new ActivityLifecycleCallbacks() {

				@Override
				public void onActivityCreated(Activity activity,
						Bundle savedInstanceState) {
					 // new activity created; force its orientation to portrait
	                activity.setRequestedOrientation(
	                    ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
					}

				@Override
				public void onActivityDestroyed(Activity activity) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onActivityPaused(Activity activity) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onActivityResumed(Activity activity) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onActivitySaveInstanceState(Activity activity,
						Bundle outState) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onActivityStarted(Activity activity) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onActivityStopped(Activity activity) {
					// TODO Auto-generated method stub
					
				}

	          

	        });

	  }

	  public void handleUncaughtException (Thread thread, Throwable e)
	  {
	    
		e.printStackTrace(); // not all Android versions will print the stack trace automatically
		/*
	    Intent intent = new Intent ();
	    intent.setAction ("com.mydomain.SEND_LOG"); // see step 5.
	    intent.setFlags (Intent.FLAG_ACTIVITY_NEW_TASK); // required when starting from Application
	    startActivity (intent);
        */
		
		MkrNLib.Get().write2Log(getExceptionStr(e));
		//MkrNLib.Get().WriteExc2Log(e.get);
	    System.exit(1); // kill off the crashed app
	  }
	  String getExceptionStr(Throwable e)
	  {
		  StackTraceElement[] stack = e.getStackTrace();
		  String theTrace = "";
		  for(StackTraceElement line : stack)
		  {
		     theTrace += line.toString();
		  }
		  return theTrace;
	  }

}
