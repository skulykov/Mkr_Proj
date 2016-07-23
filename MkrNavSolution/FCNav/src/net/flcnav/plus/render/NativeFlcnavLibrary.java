package net.flcnav.plus.render;


import net.flcnav.LogUtil;
import net.flcnav.NativeLibrary;
import net.flcnav.plus.render.FlcnavRenderer.RenderingContext;
import net.flcnav.render.RenderingRuleSearchRequest;
import net.flcnav.render.RenderingRulesStorage;

import org.apache.commons.logging.Log;

import android.graphics.Bitmap;

public class NativeFlcnavLibrary extends NativeLibrary {
	private static final Log log = LogUtil.getLog(NativeFlcnavLibrary.class);
	
	private static NativeFlcnavLibrary library;
	private static Boolean isNativeSupported = null;
	
	public static NativeFlcnavLibrary getLoadedLibrary(){
		synchronized (NativeFlcnavLibrary.class) {
			return library;
		}
	}

	public static NativeFlcnavLibrary getLibrary(RenderingRulesStorage storage) {
		if (!isLoaded()) {
			synchronized (NativeFlcnavLibrary.class) {
				if (!isLoaded()) {
					try {
						log.debug("Loading native stlport_shared..."); //$NON-NLS-1$
						System.loadLibrary("stlport_shared");
						log.debug("Loading native cpufeatures_proxy..."); //$NON-NLS-1$
						System.loadLibrary("cpufeatures_proxy");
						if(android.os.Build.VERSION.SDK_INT >= 8) {
							log.debug("Loading jnigraphics, since Android >= 2.2 ..."); //$NON-NLS-1$
							System.loadLibrary("jnigraphics");
						}
						if(!cpuHasNeonSupport()) {
							log.debug("Loading native flcnav..."); //$NON-NLS-1$
							System.loadLibrary("flcnav");
						} else {
							log.debug("Loading native flcnav with NEON..."); //$NON-NLS-1$
							System.loadLibrary("flcnav_neon");
						}
						log.debug("Creating NativeFlcnavLibrary instance..."); //$NON-NLS-1$
						library = new NativeFlcnavLibrary();
						//library.NativeLibTest(12);
						log.debug("Initializing rendering rules storage..."); //$NON-NLS-1$
						NativeFlcnavLibrary.initRenderingRulesStorage(storage);
						isNativeSupported = true;
						log.debug("Native library loaded successfully"); //$NON-NLS-1$
					} catch (Throwable e) {
						log.error("Failed to load native library", e); //$NON-NLS-1$
						isNativeSupported = false;
					}
				}
			}
		}
		return library;
	}
	
	public static boolean isSupported()
	{
		return isNativeSupported != null && isNativeSupported;
	}
	
	public static boolean isLoaded() {
		return isNativeSupported != null;  
	}
	
	public static boolean isNativeSupported(RenderingRulesStorage storage) {
		if(storage != null) {
			getLibrary(storage);
		}
		return isSupported();
	}
	
	public boolean useDirectRendering(){
		return android.os.Build.VERSION.SDK_INT >= 8;
	}

	public RenderingGenerationResult generateRendering(RenderingContext rc, NativeSearchResult searchResultHandler,
			Bitmap bitmap, boolean isTransparent, RenderingRuleSearchRequest render) {
		if (searchResultHandler == null) {
			log.error("Error searchresult = null"); //$NON-NLS-1$
			return new RenderingGenerationResult(null);
		}
		
		// Android 2.2+
		if(android.os.Build.VERSION.SDK_INT >= 8) { 
			return generateRenderingDirect(rc, searchResultHandler.nativeHandler, bitmap, render);
		} else {
			return generateRenderingIndirect(rc, searchResultHandler.nativeHandler, isTransparent, render, false);
		}
	}

	
	private static native RenderingGenerationResult generateRenderingDirect(RenderingContext rc, long searchResultHandler,
			Bitmap bitmap, RenderingRuleSearchRequest render);
			
	public static native int getCpuCount();
	public static native boolean cpuHasNeonSupport();
}
