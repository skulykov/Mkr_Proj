package com.mkr.navgl.utils;

import java.util.List;

import com.mkr.navgl.nintf.R;

import android.app.Activity;
import android.app.DownloadManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

//http://stackoverflow.com/questions/3028306/download-a-file-with-android-and-showing-the-progress-in-a-progressdialog
public class UriFileDownloader {

	static int state=0;
	static int prev=0;
	static String zipFile="", unzipLocation="";
	/**
	 * @param context used to check the device version and DownloadManager information
	 * @return true if the download manager is available
	 */
	public static boolean isDownloadManagerAvailable(Context context) {
	    try {
	        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.GINGERBREAD) {
	            return false;
	        }
	        Intent intent = new Intent(Intent.ACTION_MAIN);
	        intent.addCategory(Intent.CATEGORY_LAUNCHER);
	        intent.setClassName("com.android.providers.downloads.ui", "com.android.providers.downloads.ui.DownloadList");
	        List<ResolveInfo> list = context.getPackageManager().queryIntentActivities(intent,
	                PackageManager.MATCH_DEFAULT_ONLY);
	        return list.size() > 0;
	    } catch (Exception e) {
	        return false;
	    }
	}
	public static void setFileLocation(String _zipFile,String _unzipLocation)
	{
		zipFile=_zipFile; 
		unzipLocation=_unzipLocation;
	}
	public static void downloadFile(Context context,String url)
	{
		//String url = "url you want to download";
		DownloadManager.Request request = new DownloadManager.Request(Uri.parse(url));
		request.setDescription("Map data download");
		request.setTitle("Ontario");
		// in order for this to run, you must use the android 3.2 to compile your app
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
		    request.allowScanningByMediaScanner();
		    request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED); 
		}
		try
		{
		    request.setDestinationInExternalPublicDir(/*Environment.DIRECTORY_DOWNLOADS */  unzipLocation  , zipFile);
		}
		catch(Exception e)
		{
		  e.printStackTrace();
		}
		//request.setDestinationInExternalFilesDir(context, "/mnt/sdcard/fcnavgl"  , "1.obf");

		// get download service and enqueue file
		final DownloadManager manager = (DownloadManager) context.getSystemService(Context.DOWNLOAD_SERVICE);
		final long downloadId =manager.enqueue(request);
		
		//http://stackoverflow.com/questions/15795872/show-download-progress-inside-activity-using-downloadmanager
		
		final Activity act=(Activity)context;
		final ProgressBar pb = (ProgressBar) act.findViewById(R.id.progressBar1);
		final LinearLayout la = (LinearLayout) act.findViewById(R.id.myHeaderProgress);
		final TextView tv=(TextView) act.findViewById(R.id.tvDownloadProgress);
        state=0;
        prev=0;
        pb.setProgress(0);
        new Thread(new Runnable() {

            @Override
            public void run() {

                boolean downloading = true;
                
                
                while (downloading) {

                    DownloadManager.Query q = new DownloadManager.Query();
                    q.setFilterById(downloadId);

                    Cursor cursor = manager.query(q);
                    cursor.moveToFirst();
                    final long bytes_downloaded = cursor.getInt(cursor
                            .getColumnIndex(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR));
                   final long bytes_total = cursor.getInt(cursor.getColumnIndex(DownloadManager.COLUMN_TOTAL_SIZE_BYTES));

                    if (cursor.getInt(cursor.getColumnIndex(DownloadManager.COLUMN_STATUS)) == DownloadManager.STATUS_SUCCESSFUL) {
                        downloading = false;
                        state=2;
                    }

                    final double dl_progress = (bytes_downloaded *100.0)/ bytes_total;

                    act.runOnUiThread(new Runnable() {

                        @Override
                        public void run() {

                            switch(state)
                            {
                            case 0:
                            	la.setVisibility(View.VISIBLE);
                            	state=1;
                            	break;
                            case 1:
                            	if(prev!=dl_progress)
                            	{
                            		pb.setProgress((int) dl_progress);
                            		String s="Ontario Canada  "+Integer.toString((int)dl_progress)+"%";
                            		//String s=String.format("%d  %d from %d",dl_progress,bytes_downloaded,bytes_total);
                            		tv.setText(s);
                            		prev=(int)dl_progress;
                            	}
                            	break;
                            case 2:
                            	 la.setVisibility(View.GONE);
                            	break;
                            }
                        	
                        	
                        }
                    });

                   // Log.d(Constants.MAIN_VIEW_ACTIVITY, statusMessage(cursor));
                    cursor.close();
                    try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						
					}
                }
                //we finished downloading at this point -> decompress if needed 
                
                
                
                Decompress d = new Decompress(zipFile, unzipLocation); 
                d.unzip(); 

            }
        }).start();
		
	}
	
}
