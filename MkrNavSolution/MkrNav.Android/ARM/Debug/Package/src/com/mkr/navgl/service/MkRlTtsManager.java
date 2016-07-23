package com.mkr.navgl.service;

import java.util.HashMap;
import java.util.Locale;

import com.mkr.navgl.nintf.MkrNavMainAct;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.speech.tts.TextToSpeech;
import android.util.Log;


public class MkRlTtsManager implements TextToSpeech.OnInitListener, IMkRlTtsManager
{
	MkRlTtsManager(){}
	/*************************************************************************************************
	 * InitNativeLayer - Initialization of the   
	 * At this stage the library should be loaded
	 * @return void 
	 */
	public void InitTtsMngNativeLayer()
	{
		InitTtsManagerNTV();	
	}
	/*************************************************************************************************
	 * Submit - Main entry point  
	 * Submits the tts request
	 * @param aText - text to synthesize
	 * @param aCbContext - context to pass upon completion
	 */
	public void Submit( final String aText, final String aFullPath, final long aCbContext )
	{		
		if ( mTtsInitilized )
		{
			// AGA DEBUG
			//Log.w( MTAG, "Received request for " + aText + ". Path: " + aFullPath );

			HashMap<String,String> params = new HashMap<String,String>();
			params.put( TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, aText );
			int result = mTts.synthesizeToFile( aText, params, aFullPath );
			
			if ( result == TextToSpeech.ERROR )
				TtsManagerCallbackNTV( aCbContext, TTS_RES_STATUS_ERROR );
			else
				mRequestMap.put( aText, new Long( aCbContext ) );
	
		}
		
	}
	public void SpeakText( final String aText )
	{		
		if ( mTtsInitilized )
		{
			// AGA DEBUG
			//Log.w( MTAG, "Received request for " + aText + ". Path: " + aFullPath );
			mTts.speak(aText, TextToSpeech.QUEUE_FLUSH, null);
			 
		}
		
	}
	
	/*************************************************************************************************
	 * Prepare
	 * Prepares the provider to the requests
	 * @param void
	 */
	public void Prepare()
	{
		Log.w( MTAG, "Tts Prepare()..." );
		checkData();
	}	
	
	public void onDataCheckResult( int resultCode, Intent data )
	{
		//Log.w( MTAG, "TTS onDataCheckResult.." + Integer.toString(resultCode));
		if ( resultCode == TextToSpeech.Engine.CHECK_VOICE_DATA_PASS ) 
		{
			final Runnable ttsCreateEvent = new Runnable() {
				public void run() {
					Context ctx = MkRlAppService.getAppContext();
		            // success, create the TTS instance
		            mTts = new TextToSpeech( ctx, MkRlTtsManager.this );
				}
			};
			MkRlNativeManager.Post( ttsCreateEvent );
        } 
		else 
		{
			//Log.w( MTAG, "TTS Data doesn't present - installing" );
			Activity activity = MkRlAppService.getMainActivity();
			if ( activity != null )
			{
			    // Missing data, install it
	            Intent installIntent = new Intent();
	            installIntent.setAction( TextToSpeech.Engine.ACTION_INSTALL_TTS_DATA );
	            activity.startActivityForResult( installIntent, MkrNavMainAct.TTS_DATA_INSTALL_CODE );
			}
        } 		
	}
	
	public void onDataInstallResult( int resultCode, Intent data )
	{
		checkData();
	}
	
	public void onInit( int status )
	{
		//Log.w( MTAG, "TTS onInit.." );
		if ( status == TextToSpeech.SUCCESS )
		{
			int result = mTts.setLanguage(Locale.US);
			mTtsInitilized = true;
			mRequestMap = new HashMap<String, Long>();
			mTts.setOnUtteranceCompletedListener( new UtteranceCompletedListener() );
		}
		else
		{
			//Log.e( MTAG, "TTS onInit..ERROR" );
		}
	}
	/*************************************************************************************************
     *================================= Private interface section =================================
     * 
     */
	private final class UtteranceCompletedListener implements TextToSpeech.OnUtteranceCompletedListener
	{
		public void onUtteranceCompleted( String aUtteranceId )
		{
			// AGA DEBUG
			//Log.w( MTAG, "Request completed for " + aUtteranceId );
			Long cbContext = mRequestMap.get( aUtteranceId );
			if ( cbContext == null )
			{
				Log.e(MTAG, "WazeTttManager Error. There is no request for " + aUtteranceId );
			}
			// Todo : Check thread
			TtsManagerCallbackNTV( cbContext.longValue(), TTS_RES_STATUS_SUCCESS );
		}
	}

	private void checkData()
	{
		Activity activity = MkRlAppService.getMainActivity();
		if ( activity != null )
		{
			Intent checkIntent = new Intent();
			checkIntent.setAction( TextToSpeech.Engine.ACTION_CHECK_TTS_DATA );
			activity.startActivityForResult( checkIntent,MkrNavMainAct.TTS_DATA_CHECK_CODE );
		}
		else
		{
			//Log.w( MTAG, "Tts checkData() ...( activity != null )" );
		}
	}

	/*************************************************************************************************
     *================================= Native methods section ================================= 
     * These methods are implemented in the
     * native side and should be called after!!! the shared library is loaded
     */
	 private native void InitTtsManagerNTV();
	 private native void TtsManagerCallbackNTV( final long aCbContext, final int aResStatus );
	 
	/*************************************************************************************************
     *================================= Members section =================================
     */
	
	private volatile boolean mTtsInitilized = false;
	private TextToSpeech mTts = null;
	private HashMap<String,Long> mRequestMap;
	/*************************************************************************************************
     *================================= Constants section =================================
     */
	
	private final int TTS_RES_STATUS_ERROR                     =   0x00000001;
	private final int TTS_RES_STATUS_PARTIAL_SUCCESS           =   0x00000002;
	private final int TTS_RES_STATUS_SUCCESS                   =   0x00000004;
	private final int TTS_RES_STATUS_NO_RESULTS                =   0x00000008;
	
	private final String           MTAG               = "MkrTtsManager";
}
