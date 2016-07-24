package com.mkr.utils;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import com.mkr.service.MkrApp;

import android.content.Context;
import android.text.format.DateFormat;

public class DateUtil {

	public static Date addDays(Date date, int days)
    {
        Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        cal.add(Calendar.DATE, days); //minus number would decrement the days
        return cal.getTime();
    }

	public static String GetDateStr(Date dt)
	{
						
		String timeStamp = new SimpleDateFormat("yyyy-MM-dd").format(dt);
		return timeStamp;
	}
	
	public static Date parseDate(String dtStart)
	{
		Date date = null;
		//String dtStart = "2010-10-15T09:27:37Z";  
		SimpleDateFormat  format = new SimpleDateFormat("yyyy-MM-dd");  
		    try {
				date = format.parse(dtStart);
			} catch (java.text.ParseException e) {
				e.printStackTrace();
			}  
		return date;
	}
	public static Date GetDateDate(Date dt)
	{
		return parseDate(GetDateStr(dt));
	}
	//========================Formats===========================
	public static String GetLongTimeStr(Date dt)
	{
		//String timeStamp = new SimpleDateFormat("yyyy-MM-dd").format(dt);
		//return timeStamp;
		/*
		Context ctx=MkrApp.getAppContext();
		java.text.DateFormat df=DateFormat.getDateFormat(ctx);
			
		
		*/
		SimpleDateFormat df = new SimpleDateFormat("EEE MMM dd,yyyy");//new SimpleDateFormat("MMM dd,yyyy  hh:mm a");
		return df.format(dt);
	}
	public static String getTS(/*Date dt*/)
	{
		Date dt=new Date();
		SimpleDateFormat df = new SimpleDateFormat("EEE MMM dd, yyyy hh:mm");//https://developer.android.com/reference/java/text/SimpleDateFormat.html
		return df.format(dt);
	}
	public static String getTimeStamp(long ts)
	{
		//String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		String timeStamp = new SimpleDateFormat("HH:mm:ss").format(new Date(ts));
		return timeStamp;
	}
	public Date GetMidnight(Date dt)
	{
		Long time = dt.getTime();
		Date date = new Date(time - time % (24 * 60 * 60 * 1000));
		return date;
	}
	public Date AddDays(Date dt,int days)
	{
		Date date = new Date(dt.getTime() + days*24 * 60 * 60 * 1000);
		return date;
	}
}
