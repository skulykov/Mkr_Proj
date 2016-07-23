package com.mkr.navgl.service;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

import com.mkr.navgl.utils.MkRlResManager;


import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;


public class MkRlSoundManager 
{
	//AudioManager a_mgr = null;
	//int max_volume=15;
    public MkRlSoundManager(MkRlNativeManager aNativeManager)
    {
        mNativeManager = aNativeManager;

        // At this stage the native library has to be loaded
        InitSoundManagerNTV(); 

        mPendingPlayersList = new ArrayList<String>();
        
        Context appContext = MkRlAppService.getAppContext();
    	//a_mgr = (AudioManager) appContext.getSystemService(Context.AUDIO_SERVICE);
    	//max_volume=a_mgr.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        
    }

    //http://stackoverflow.com/questions/16229648/set-volume-to-max-in-android
    //http://stackoverflow.com/questions/2539519/set-the-volume-of-an-android-application?rq=1
    void setVolume(int val)
    {
    	
       // int valuess = 9;//range(0-15)
         //a_mgr.getStreamVolume(AudioManager.STREAM_MUSIC);
    	 //a_mgr.setStreamVolume(AudioManager.STREAM_MUSIC, val, 0);
    }
    /*************************************************************************************************
     * Init the sound pool. Build the hash map
     * 
     * @param aDataDir
     *            - the full path to the directory containing only the sound files 
     */
    public void LoadSoundData( byte[] aDataDir )
    {
    }

    /*************************************************************************************************
     * Add file to the list and try to play
     * 
     * @param aFileName
     *            - the full path to the file
     */
    public void PlayFile( byte aFileName[] )
    {
        String fileName = new String( aFileName, 0, aFileName.length );
        File file = new File( fileName );
    	mPendingPlayersList.add( fileName );

    	PlayNext();
    }

    /*************************************************************************************************
     * Add file to the list and try to play
     * 
     * @param aFileName
     *            - the full path to the file
     */
    public void PlayBuffer( byte aData[] )
    {
    	try 
    	{
	    	File file = File.createTempFile( "sound", null, new File( MkRlResManager.mAppDir + MkRlResManager.mSoundDir ) );
	    	FileOutputStream fos = new FileOutputStream( file );
	    	fos.write( aData );
	    	fos.close();
	    	
	    	mPendingPlayersList.add( file.getAbsolutePath() );
	    	PlayNext();
    	}
    	catch( Exception ex )
    	{
    		Log.e(TAG, "Error playing sound buffer", ex );
    		ex.printStackTrace();
    	}
    }
    /*************************************************************************************************
     * Playing the sound file for the SDK supported formats. The overloaded
     * function for the more convenient call from the native layer
     * 
     * @param aFileName
     *            - the full path to the file
     */

    /*************************************************************************************************
     *================================= Private interface section =================================
     */

    /*************************************************************************************************
     * PlayNext() - Main logic of the manager. Plays the file and tries to buffer the next file in the list
     * 
     * @param void
     * @return void           
     */
    private void PlayNext()
    {
		if ( mPlaying ) 
		{
			// If already playing - try to buffer the next			
			BufferNext();
		}
		else
		{
			// If not playing - play the buffered file or play the new one
			// If sound buffering is disabled - will be null always
			if ( mBufferedPlayer == null )
			{
				if ( mPendingPlayersList.size() > 0 )
				{
					String nextFile = mPendingPlayersList.remove( 0 );
					mCurrentPlayer = new MkrAudioPlayer( nextFile ); 
					mCurrentPlayer.Play();
				}
			}
			else
			{
				mCurrentPlayer = mBufferedPlayer;
				mCurrentPlayer.Play();
				
				mBufferedPlayer = null;
			}
			// Try to buffer next file
			BufferNext();
		}
    }
    
    /*************************************************************************************************
     * BufferNext() - Creates thread with buffering request. Will be played later 
     * 
     * @param void
     * @return void           
     */
    private void BufferNext()
    {
		if ( SOUND_BUFFERING_ENABLED )
		{
			if ( mBufferedPlayer == null )
			{
				if ( mPendingPlayersList.size() > 0 )
				{
					String nextFile = mPendingPlayersList.get( 0 );
					
					/*
					 * Avoid audio lock on playing the same file
					 */
					if ( mCurrentPlayer != null && nextFile.equals( mCurrentPlayer.getFileName() ) )
					{
						return;
					}
	
					mPendingPlayersList.remove( 0 );
					mBufferedPlayer = new MkrAudioPlayer( nextFile );
					mBufferedPlayer.Buffer();
				}
			}
		}
    }
    
    public void ShutDown()
    {
    	/*
    	 * Explicitly release the resources     	 
    	 */
    }
   
    
    /*************************************************************************************************
     * This class implements the logic of the player thread 
     
     */
    private final class MkrAudioPlayer extends Thread
    {
    	MkrAudioPlayer( String aFileName )
    	{
    		mFileName = aFileName;
    	}
    	public void Play()
    	{
    		if ( mBuffering && !mBuffered ) // In the buffer process
    			return;
    		
    		mPlaying = true;
    		if ( mBuffered )
    		{
    			try
                {
	    			synchronized( this ) {
	    				notify();
	    			}
                }
                catch( Exception ex )
                {
                	Log.e(TAG, "Audio Player. Error notifying the thread", ex );
                }
    		}
    		else
    		{  
    			start();
    		}
    	}
    	public void Buffer()
    	{
    		mBuffering = true;
    		start();
    	}
    	public void run()
    	{
            try
            {
            	/*
            	 * Always buffer first
            	 */
            	if ( !mBuffered )
            		BufferInternal();
            	
            	/*
            	 * if buffering request - wait for the play request
            	 */
            	if ( mBuffering )
            	{
	    			synchronized( this ) {
	    				wait();
	    			}
            	}
            	/*
            	 * Start playing
            	 */
	            mMP.start();
            }
            catch( Exception ex )
            {
            	Log.e(TAG, "Audio Player. Error playing the file " + mFileName, ex );
            	ex.printStackTrace();
            	if ( mMP != null )
            	{
            		finalizePlay( mMP );
            	}
            }
    	}
    	public String getFileName()
    	{
    		return mFileName;
    	}
    	private void BufferInternal() throws IOException
    	{
    		
    		mMP = new MediaPlayer();
    		mMP.setOnCompletionListener( new CompletionListener() );
    		mMP.setOnErrorListener( new ErrorListener() );
            // Load the file
            final FileInputStream fileInStream = new FileInputStream( mFileName );
            // Prepare the media player
            mMP.setDataSource( fileInStream.getFD() );
            // Set the volume to the maximum. Adjustment is performed through
            // the general media volume setting
            mMP.setVolume(1, 1);                     
            /*
             * Prepare the data file
             */
            mMP.prepare();
            
            mBuffered = true;	            
    	}
    	
    	/*
    	 * Completion  listener - indicate the next file can be played. Post request for the next file playing 
    	 */
    	private final class CompletionListener implements OnCompletionListener
    	{
            public void onCompletion( MediaPlayer aMP )
            {            	
            	finalizePlay( aMP );
            }            
    	}
    	private final class ErrorListener implements OnErrorListener
    	{
    		public boolean onError( MediaPlayer aMP, int what, int extra )
    		{
    			finalizePlay( aMP );
    			return true;
    		}
    	}
    	
    	private void finalizePlay( MediaPlayer aMP )
    	{
    		aMP.release();            	
        	mPlaying = false;
        	
        	Runnable playNext = new Runnable() {
				public void run() {
					PlayNext();
				}
			};
			mNativeManager.PostRunnable( playNext );
    	}
    	private String mFileName;
    	private boolean mBuffered = false;			// Indicates whether the prepare is finished
    	private boolean mBuffering = false;			// Indicates whether the buffering is requested  
    	private MediaPlayer mMP = null;
    }
    
    
    
    /*************************************************************************************************
     *================================= Native methods section ================================= These methods are implemented in the
     * native side and should be called after!!! the shared library is loaded
     */
    private native void InitSoundManagerNTV();

    /*************************************************************************************************
     *================================= Data members section =================================
     * 
     */
    private MkRlNativeManager mNativeManager;
	
    private ArrayList<String> mPendingPlayersList;
    private volatile boolean mPlaying = false;
    private MkrAudioPlayer mBufferedPlayer = null;
    private MkrAudioPlayer mCurrentPlayer = null;
    
    private final static boolean SOUND_BUFFERING_ENABLED = false;
//    private final static int  MAX_PLAYERS_NUMBER = 5;
    
    private final String           TAG               = "SoundManager";
}
