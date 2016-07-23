package com.mkr.navgl.utils;

import android.util.Log; 
import java.io.File; 
import java.io.FileInputStream; 
import java.io.FileOutputStream; 
import java.util.zip.ZipEntry; 
import java.util.zip.ZipInputStream;

public class Decompress {
	
	private String _zipFile;  
	private String _location;    
	public Decompress(String zipFile, String location) 
	{     
		_zipFile = zipFile;
		_location = location;     
		_dirChecker("");   
	}
	private void _dirChecker(String dir) 
	{     
		File f = new File(_location + dir);
		if(!f.isDirectory()) {       
			f.mkdirs();     
		}   
	}
	
	
	public void unzip() 
	{     
		try  
		{  
			FileInputStream fin = new FileInputStream("/mnt/sdcard"+_location +"/"+_zipFile);
			ZipInputStream zin = new ZipInputStream(fin);
			ZipEntry ze = null;
			while ((ze = zin.getNextEntry()) != null) 
			{
				Log.v("Decompress", "Unzipping " + ze.getName());
				if(ze.isDirectory()) {
					_dirChecker(ze.getName());
				} else {
					FileOutputStream fout = new FileOutputStream("/mnt/sdcard"+_location +"/" + ze.getName());
					/*
					for (int c = zin.read(); c != -1; c = zin.read()) {
						fout.write(c); 
					}*/
					
					byte[] buffer = new byte[32*1024];
					 int length;
					 while ((length = zin.read(buffer))!=-1) {
						 fout.write(buffer, 0, length);
					}
					//http://stackoverflow.com/questions/17814707/unzipping-large-files-on-android?rq=1
					Log.v("Decompress", "Unzipping DONE.. " + ze.getName());
					zin.closeEntry();
					fout.close();         
				}                
			}       
			zin.close();     
		} 
		catch(Exception e) {
			Log.e("Decompress", "unzip", e);     
		}    
	} 
	
	
	
}
