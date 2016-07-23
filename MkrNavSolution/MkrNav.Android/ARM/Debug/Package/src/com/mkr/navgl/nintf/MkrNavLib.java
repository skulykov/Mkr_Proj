/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.mkr.navgl.nintf;



import org.apache.commons.logging.Log;

import com.mkr.navgl.utils.LogUtil;

// Wrapper for native library

public class MkrNavLib {
	static boolean initialized=false;
	public static final Log log = LogUtil.getLog(MkrNavLib.class);
    
	public static void initLibs() {
    	if(!initialized)
    	{
    	try
    	 {
    		 //log.debug("Loading native Libs stlport_shared...");
    		 //System.loadLibrary("stlport_shared");
    		 System.loadLibrary("MkrNavGL");
    		 initialized=true;
    	 }
    	 catch (Throwable e) {log.debug("EXCEPTION Loading native libs...");}
    	}
     }
     
    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void inititializeOnStart();
 
     public static native void drag(float dx,float dy,int flag);
     public static native void keyUpDown(float x,float y,int flag);
     public static native void zoom(int zoom);
     public static native int rSearchAddress(String s);//request searchAddress
     public static native void animate();
     public static native void setProj(int flag);
     public static native void setVoiceOn(boolean flag);
     public static native void locationChanged(float lat,float lon,int speed,int cog);
     public static native void setCurrentLocation();
   //  public static native boolean isRenderingDone();
     public static native void routeOption(float x,float y,int flag);
}
