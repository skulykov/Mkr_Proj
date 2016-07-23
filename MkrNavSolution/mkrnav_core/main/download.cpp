/*
 *
 */
#include <jni.h>

#include "sk_net.h"
#include "sk_httpcopy_async.h"
#include "filemng.h"

const char *DlMapFullName="/mnt/sdcard/fcnavgl/fname.bin";
int DlMapTotalSize=0,DlMapProgress=0;
SK_FILE DlFile;
static void *DlContext = NULL;

static void dlmap_close (int success) {

	if (SK_FILE_IS_VALID (DlFile)) {
		sk_file_close (DlFile);
		DlFile = SK_INVALID_FILE;
		if (success) {
			/*ssd_progress_msg_dialog_show (sk_lang_get ("Updating Map Tiles..."));
			if (!sk_screen_refresh ()) {
				sk_screen_redraw();
			}
			sk_main_set_periodic (100, dlmap_update_tiles);*/
		} else {
			sk_file_remove (NULL, DlMapFullName);
		}
	}	
	//dlmap_finalize ();
	DlContext = NULL;
}
static int  DlMapCallbackSize	(void *context, size_t size) {

	DlMapTotalSize = size;
	
	//TODO: check free file space
	
	DlFile = sk_file_open (DlMapFullName, "w");
	if (!SK_FILE_IS_VALID (DlFile)) {
		sk_log (SK_ERROR, "Cannot open file %s for writing", DlMapFullName);
		//sk_messagebox ("Error", "Map Download Failed");
		return 0;
	}

	DlMapProgress = 0;
	LOGI("DlMapCallbackSize..called");
	return 1;	 	
}
void DlMapCallbackProgress (void *context, char *data, size_t size) {

	if (SK_FILE_IS_VALID (DlFile)) {
		size_t written = sk_file_write (DlFile, data, size);
		if (written != size) {
			sk_log (SK_ERROR, "Writing failed to file %s", DlMapFullName);
			//sk_messagebox ("Error", "Map Download Failed -- Not Enough Space");
			dlmap_close (0);
		}
		DlMapProgress += 10;//written;

		//if (DlMapProgress * 10 / DlMapTotalSize > (DlMapProgress - (int)written) * 10 / DlMapTotalSize) {
//			sk_screen_refresh ();
//		}
		LOGI("DlMapCallbackProgress.. written=%d size=%d",written,size);
	}
		
}

void DlMapCallbackError (void *context, int connection_failure, const char *format, ...) {

   va_list ap;
   char err_string[1024];

   va_start (ap, format);
   vsnprintf (err_string, 1024, format, ap);
   va_end (ap);
	sk_log (SK_ERROR, err_string);

	/*dlmap_close (0);
	sk_messagebox ("Error", "Map Download Failed");*/
}


void DlMapCallbackDone (void *context, char *last_modified, const char *format, ... ) {

	LOGI("DlMapCallbackDone..called");
	dlmap_close (1);	
}

void sk_map_download_region (const char *region_code, int fips) {

	//char url[512];
	const char *url="http://www.wilinxdemexico.net/logfile/test.bin";
	static Sk_HttpAsyncCallbacks cbs = { DlMapCallbackSize, DlMapCallbackProgress, DlMapCallbackError, DlMapCallbackDone };
	
	
	/*snprintf (url, sizeof(url), "%s/%s/%s", 
				 sk_config_get (&DlMapSourceConf),
				 region_code, DlMapFileName);*/
				 
	//dlmap_initialize ();
	DlContext = sk_http_async_copy (&cbs, NULL, url, 0);
}


//jint
//Java_com_example_twolibs_TwoLibs_add( JNIEnv*  env,
//                                      jobject  _this,
//                                      jint     x,
//                                      jint     y )
//{
//    sk_map_download_region ("canada", 11);
//	return 1;
//}
