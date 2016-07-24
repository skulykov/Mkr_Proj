package com.mkr.utils;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Random;



import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.ParseException;
import android.os.BatteryManager;
import android.view.inputmethod.InputMethodManager;

public class Util {
	private static Random random=new Random();
	
	public static int getRandomInteger(int aStart, int aEnd) {
		if (aStart > aEnd) {
			throw new IllegalArgumentException("Start cannot exceed End.");
		}
		long range = (long) aEnd - (long) aStart + 1;
		
		long fraction = (long) (range * random.nextDouble());
		int randomNumber = (int) (fraction + aStart);
		return randomNumber;
	}
	
	
	//**************************DateTime*******************************************
	public static String getTimeStamp(long ts)
	{
		//String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		String timeStamp = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date(ts));
		return timeStamp;
	}
	public static String getTimeStamp()
	{
		//String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		String timeStamp = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date());
		return timeStamp;
	}
	public static String GetLongTimeStr(Date dt)
	{
		String timeStamp = new SimpleDateFormat("yyyy-MM-dd").format(dt);
		return timeStamp;
	}
	public static String ToShortDateString(Date dt)
	{
		String timeStamp = new SimpleDateFormat("yyyy-MM-dd").format(dt);
		return timeStamp;
	}
	
	public static String getTS()
	{
		//String timeStamp = new SimpleDateFormat("HH:mm:ss").format(new Date());
		//return timeStamp;
		Date d=new Date();
		return d.toString();
	}
	public static String getUtcTS()
	{
		long ts=System.currentTimeMillis();
		String timeStamp = new SimpleDateFormat("HH:mm:ss").format(new Date(ts));
		return timeStamp;
	}
	public static Date getUtcDate()
	{
		long ts=System.currentTimeMillis();
		
		return new Date(ts);
	}
	
	public static long DiffInSec(Date start,Date end)
	{
		 long diffInSeconds = (end.getTime() - start.getTime()) / 1000;
		 return diffInSeconds;
		 /*
		   long diff[] = new long[] { 0, 0, 0, 0 };
    diff[3] = (diffInSeconds >= 60 ? diffInSeconds % 60 : diffInSeconds);                         // sec 
     diff[2] = (diffInSeconds = (diffInSeconds / 60)) >= 60 ? diffInSeconds % 60 : diffInSeconds; //min 
    diff[1] = (diffInSeconds = (diffInSeconds / 60)) >= 24 ? diffInSeconds % 24 : diffInSeconds; //hours 
     diff[0] = (diffInSeconds = (diffInSeconds / 24));                                           //days 

		  */

	}
	public static long DiffInMin(long end,long start)
	{
		 long diffInSeconds = (end - start) / 1000;
		 return (diffInSeconds/60);
	
	}
	public static int GetMinutesFromMidNight(long millis)
	{
		Date dt=new Date(millis);
		int year=dt.getYear(),month=dt.getMonth(),day=dt.getDate();
		Date dt0 = new Date(year,month,day);
		 long diffInSeconds = (dt.getTime() - dt0.getTime()) / 1000;
		
		return (int)(diffInSeconds/60);
	}
	
	
	public static Date parseIso(String dtStart)
	{
		Date date = null;
		//String dtStart = "2010-10-15T09:27:37Z";  
		SimpleDateFormat  format = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'");  
		
		    try {
				date = format.parse(dtStart);
			} catch (java.text.ParseException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}  
		     
		
		return date;
	}
	public static Date parseDateTime(String dtStart)
	{
		Date date = null;
		//String dtStart = "2010-10-15T09:27:37Z";  
		SimpleDateFormat  format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");  
		    try {
				date = format.parse(dtStart);
			} catch (java.text.ParseException e) {
				e.printStackTrace();
			}  
		return date;
	}
	
	//===================================================================================
	public static boolean isValidEmailAddress(String email) {
        String ePattern = "^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@((\\[[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\])|(([a-zA-Z\\-0-9]+\\.)+[a-zA-Z]{2,}))$";
        java.util.regex.Pattern p = java.util.regex.Pattern.compile(ePattern);
        java.util.regex.Matcher m = p.matcher(email);
        return m.matches();
 }
	//********************************************File Image******************************
	static String getLocation()
	{
	  /* int lat=(int)(Global.Lat*1000.0);
	   int lon=(int)(Global.Lon*1000.0);
	   String rv=String.format("%s_%s_", Integer.toString(lat),Integer.toString(lon)); */
	   return "";//rv;
	}
	public static String createCameraFileName(int v)  {
		// Create an image file name
		String s = Util.getTimeStamp();
		String fn = getLocation();
		if(v==0)
			fn = fn+s + ".jpg";
		else
			fn = fn+s + ".mp4";
		
		return fn;
	}
    
    public static String getFullPath(String imageFileName)
    {
    	 // Build the path
        String fullPath ="";//new String(Global.mImageFolder);
        fullPath += String.valueOf("/");
        fullPath += imageFileName;
        return fullPath; 
    }
	//************************************************************************************
		
	public static void hideSoftKeyboard(Activity activity) {

        try
        {
		InputMethodManager inputMethodManager = (InputMethodManager)  activity.getSystemService(Activity.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(activity.getCurrentFocus().getWindowToken(), 0);
        }
        catch(Exception e){}
    }
	
	public static void Notify(Context ctx,/*String title, String msg*/int title_id,int msg_id)
	{
		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(ctx);
 
			// set title
			alertDialogBuilder.setTitle(title_id);
 
			// set dialog message
			alertDialogBuilder
				.setMessage(msg_id)
				.setCancelable(false)
				/*.setPositiveButton("Yes",new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog,int id) {
						dialog.cancel();
					}
				  }) */
				.setNegativeButton("OK",new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog,int id) {
						dialog.cancel();
					}
				});
 
				// create alert dialog
				AlertDialog alertDialog = alertDialogBuilder.create();
 
				// show it
				alertDialog.show();
			}
	
	//*******************************Network*******************************************
	/*
	public boolean isNetworkAvailable(Context ctx) {
	    ConnectivityManager connectivityManager 
	          = (ConnectivityManager) ctx.getSystemService(Context.CONNECTIVITY_SERVICE);
	    NetworkInfo activeNetworkInfo = connectivityManager.getActiveNetworkInfo();
	    return activeNetworkInfo != null && activeNetworkInfo.isConnected();
	}*/
	//http://stackoverflow.com/questions/4238921/android-detect-whether-there-is-an-internet-connection-available
	/*
	 * return values:  1 - wifi only, 2 - cellullar only , 3 -both
	 */
	public static int /*boolean*/ haveNetworkConnection(Context ctx) {
	    //boolean haveConnectedWifi = false;
	    //boolean haveConnectedMobile = false;
		int rv=0;

	    ConnectivityManager cm = (ConnectivityManager) ctx.getSystemService(Context.CONNECTIVITY_SERVICE);
	    NetworkInfo[] netInfo = cm.getAllNetworkInfo();
	    for (NetworkInfo ni : netInfo) {
	        if (ni.getTypeName().equalsIgnoreCase("WIFI"))
	            if (ni.isConnected())
	                rv|=1;//haveConnectedWifi = true;
	        if (ni.getTypeName().equalsIgnoreCase("MOBILE"))
	            if (ni.isConnected())
	                rv|=2;//haveConnectedMobile = true;
	    }
	    return rv;//haveConnectedWifi || haveConnectedMobile;
	}
	
	//http://stackoverflow.com/questions/15746709/get-battery-level-only-once-using-android-sdk
	public static float getBatteryLevel(Context ctx) {
	    Intent batteryIntent = ctx.registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
	    int level = batteryIntent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
	    int scale = batteryIntent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);

	    // Error checking that probably isn't needed but I added just in case.
	    if(level == -1 || scale == -1) {
	        return 50.0f;
	    }

	    return ((float)level / (float)scale) * 100.0f; 
	}

	public static String int2str(long val) {
		return (""+val);
	}
	public static int str2int(String s)
	{
		//Integer.toString(lon)
		int rv=-1;
		 try {
				rv=Integer.parseInt(s);
			} catch (Exception e) {
				
			}
		 return rv;
	}
	public static String ConvertMinutesToHoursandMinutes(int v)
	{
		int h=v/60;
		int m=v % 60;
		return String.format("%02d:%02d", h,m);
	}
}



