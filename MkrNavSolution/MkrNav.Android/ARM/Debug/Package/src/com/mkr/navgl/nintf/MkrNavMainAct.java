
package com.mkr.navgl.nintf;


import com.mkr.navgl.service.IMkRlTtsManager;
import com.mkr.navgl.service.MkRlAppService;
import com.mkr.navgl.service.MkRlNativeManager;
import com.mkr.navgl.utils.MsgBox;
import com.mkr.navgl.utils.UriFileDownloader;
import com.mkr.navgl.nintf.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.gesture.GestureOverlayView;
import android.gesture.GestureOverlayView.OnGestureListener;
import android.graphics.PointF;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.media.AudioManager;
import android.os.Build;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.TextView;
import android.widget.Toast;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.inputmethod.InputMethodManager;
import android.util.Log;


public class MkrNavMainAct extends Activity implements SurfaceHolder.Callback
{
	
	 LocationManager locationManager=null;
	 LocationListener locationListener=null;

	 private MultiTouchSupport multiTouchSupport;
	 private GestureDetector gestureDetector;
	    
    private static String TAG = "N_Egl";

    private boolean flag3D=false,voiceOn=true;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG, "onCreate()");

        MkRlAppService.StartApp( this, null );   //====================?????? 
		MkRlAppService.setAppActivity(this);
        
        nativeOnCreate();
        
        setContentView(R.layout.main);
        SurfaceView surfaceView = (SurfaceView)findViewById(R.id.surfaceview);
        surfaceView.getHolder().addCallback(this);
        surfaceView.setOnClickListener(new OnClickListener() {
                public void onClick(View view) {
                    Toast toast = Toast.makeText(MkrNavMainAct.this,
                                                 "FCNav renderer",
                                                 Toast.LENGTH_LONG);
                    toast.show();
                }});
        
        surfaceView.setOnTouchListener(onTouchListener);
        initLocationService();
        
        initZoomButtons(this);
        
        gestureDetector = new GestureDetector(getBaseContext(), new MapTileViewOnGestureListener());
		multiTouchSupport = new MultiTouchSupport(getBaseContext(), new MapTileViewMultiTouchZoomListener());
		
		// initialize native audio system
        //createEngine();
        //createBufferQueueAudioPlayer();
        
         
        MkRlAppService.initTtsAndSoundService();
        
        //http://stackoverflow.com/questions/2539264/volume-control-in-android-application?rq=1
        /*
         don't override the onKeyDown and onKeyUp buttons.!!!!!!!!!!!!!!!!!
         * It's much better to simply use this one line setVolumeControlStream(AudioManager.STREAM_MUSIC); 
         * in your onCreate() method. That tells the OS that the volume buttons should affect the "media" volume 
         * when your application is visible, and that's the volume it uses for your application.
         */
        setVolumeControlStream(AudioManager.STREAM_MUSIC); 
    }
    OnTouchListener onTouchListener = new OnTouchListener() {

		@Override
		public boolean onTouch(View v, MotionEvent event) {
			 int action = event.getAction();
			 //Log.i(TAG, "onTouch..");
	            switch (action) {

	            case MotionEvent.ACTION_DOWN:
	            	MkrNavLib.keyUpDown(event.getX(), event.getY(), 1);
	             break;
	            case MotionEvent.ACTION_UP:
	            	MkrNavLib.keyUpDown(event.getX(), event.getY(), 2);
		             break; 
	            default:
	                //return false;
	            }
	            if (!multiTouchSupport.onTouchEvent(event)) {
	    			gestureDetector.onTouchEvent(event);
	    		}
			return true;
		}
    };
/*
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        int action = event.getAction();
        int keyCode = event.getKeyCode();
        switch (keyCode) {
            case KeyEvent.KEYCODE_VOLUME_UP:
                if (action == KeyEvent.ACTION_DOWN) {
                    //TODO
                }
                return true;
            case KeyEvent.KEYCODE_VOLUME_DOWN:
                if (action == KeyEvent.ACTION_DOWN) {
                    //TODO
                }
                return true;
            default:
                return super.dispatchKeyEvent(event);
        }
    }
*/
    private void initLocationService() {
    	locationManager = (LocationManager)getSystemService(LOCATION_SERVICE);
    	locationListener = new LocationListener() 
    	{
			@Override
			public void onLocationChanged(Location location) {
				
				float lon = (float) (location.getLongitude()/* * 1E6*/);				
				float lat = (float) (location.getLatitude() /* * 1E6 */);								
				float speed = 0;
				if(location.hasSpeed())
					speed=location.getSpeed();
				float cog = 0;
				if(location.hasBearing())
					cog=location.getBearing();
				
				// FCNavLib.log.debug("onLocationChanged...lat="+lat);
				MkrNavLib.locationChanged(lat, lon,(int) speed,(int) cog);
				 
			}

			@Override
			public void onProviderDisabled(String provider) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void onProviderEnabled(String provider) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void onStatusChanged(String provider, int status,
					Bundle extras) {
				// TODO Auto-generated method stub
				
			}
    	};
    	locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,1000,30,locationListener);	// 1 sec 50meters	
    	locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER,60*1000,100,locationListener); // 1 min 100 meters
    
    }
    
    //===============Optional menu=========================================
    
    
    private void initZoomButtons(MkrNavMainAct ctx) {
    	LinearLayout ll = new LinearLayout(ctx);

    	Button b2 = new Button(ctx);
		b2.setBackgroundResource(R.drawable.map_zoom_out);
    	b2.setContentDescription(ctx.getString(R.string.zoomOut));
    	b2.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
            	MkrNavLib.zoom(-1);
            }
        });
    	ll.addView(b2);
    	
    	Button b1 = new Button(ctx);
		b1.setBackgroundResource(R.drawable.map_zoom_in);
    	b1.setContentDescription(ctx.getString(R.string.zoomIn));
    	
    	b1.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
            	MkrNavLib.zoom(1);
            }
        });
    	    	
    	ll.addView(b1);
    	
    	
    	ll.setGravity(Gravity.BOTTOM | Gravity.RIGHT);
    	LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT);
    	params.setMargins(10,10, 10,10);
    	
    	ctx.addContentView(ll, params);
		
	}
    @Override
    protected void onActivityResult( int requestCode, int resultCode, Intent data )
    {
    	MkRlNativeManager nativeManager = MkRlAppService.getNativeManager();
    	
    	
    	if ( requestCode == TTS_DATA_CHECK_CODE ) {
        	final IMkRlTtsManager ttsManager = nativeManager.getTtsManager();
        	if ( ttsManager != null )
        		ttsManager.onDataCheckResult( resultCode, data );	        	
        }

    	if ( requestCode == TTS_DATA_INSTALL_CODE ) {
        	final IMkRlTtsManager ttsManager = nativeManager.getTtsManager();
        	if ( ttsManager != null )
        		ttsManager.onDataInstallResult( resultCode, data );	        	
        }
    	
        super.onActivityResult(requestCode, resultCode, data);
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
      getMenuInflater().inflate(R.menu.mainmenu, menu);
      return true;
    }
    @Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.search_address_id:
		case R.id.search_address_id_1:
			find_address();
			return (true);

		case R.id.map_mode_id:
			if(!flag3D)	
			{
				item.setTitle(" 3D");
				//item.setIcon(R.drawable.normal_car32x32);
				flag3D=true;
				MkrNavLib.setProj(1);
			}else
			{
				item.setTitle(" 2D");
				flag3D=false;
				MkrNavLib.setProj(0);
			}
			return (true);
		case R.id.turn_voice_id:
			if(voiceOn)	
			{
				item.setTitle("Turn voice on");
				item.setIcon(android.R.drawable.ic_lock_silent_mode_off);
				voiceOn=false;
				MkrNavLib.setVoiceOn(false);
			}else
			{
				item.setTitle("Turn voice off");
				item.setIcon(android.R.drawable.ic_lock_silent_mode);
				voiceOn=true;
				MkrNavLib.setVoiceOn(true);
			}
			return (true);
		case R.id.my_location_id:
			MkrNavLib.setCurrentLocation();
			return (true);
			
		case R.id.about:
			//Toast.makeText(this, "Action Bar Menu Example", Toast.LENGTH_LONG)
			//		.show();
			MkrNavLib.animate();
			return (true);
		case R.id.download_map_id:
			if(UriFileDownloader.isDownloadManagerAvailable(MkrNavMainAct.this))
        	{
        		UriFileDownloader.setFileLocation("1.zip","/fcnavgl/maps");
        		UriFileDownloader.downloadFile(MkrNavMainAct.this, "http://www.wilinxdemexico.net/mapfiles/ontario.zip");
        	}
			return (true);
		case R.id.exitN_id:
   			//http://stackoverflow.com/questions/13330415/android-how-to-properly-exit-application-when-inconsistent-condition-is-unavoid
			System.runFinalizersOnExit(true); 
			System.exit(1);//finish();   // Exit Application
   			break;
		}

		return (super.onOptionsItemSelected(item));
	}
    private void find_address() {
		final View addView = getLayoutInflater().inflate(R.layout.find_address, null);

		new AlertDialog.Builder(this).setTitle("Find Address").setView(addView)
				.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						TextView tv=((TextView) addView.findViewById(R.id.txtToSearch));
						String s= tv.getText().toString();
						MkrNavLib.rSearchAddress(s);
					}
				}).setNegativeButton("Cancel", null).show();
	}

	

	private TextView.OnEditorActionListener onSearch = new TextView.OnEditorActionListener() {
		
		@Override
		public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
			if (event == null || event.getAction() == KeyEvent.ACTION_UP) {
				
				InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);

				imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
			}

			return (true);
		}
	};

    //==========================================================================
    
    @Override
    protected void onResume() {
        super.onResume();
        Log.i(TAG, "onResume()");
        nativeOnResume();
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        Log.i(TAG, "onPause()");
        nativeOnPause();
    }

    @Override
    protected void onStop() {
        super.onDestroy();
        Log.i(TAG, "onStop()");
        nativeOnStop();

		//shutdownAudio();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        nativeSetSurface(holder.getSurface());
    }

    public void surfaceCreated(SurfaceHolder holder) {
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        nativeSetSurface(null);
    }

    
    //=================================================================================
    //============MultiTouch Support===================================================
    private class MapTileViewMultiTouchZoomListener implements com.mkr.navgl.nintf.MultiTouchSupport.MultiTouchZoomListener {

		@Override
		public void onZoomStarted(float distance, PointF centerPoint) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onZooming(float distance, float relativeToStart) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onZoomEnded(float distance, float relativeToStart) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onGestureInit(float x1, float y1, float x2, float y2) {
			// TODO Auto-generated method stub
			
		}

	}
	
	int lastProgress=10;
	private class MapTileViewOnGestureListener implements OnGestureListener, android.view.GestureDetector.OnGestureListener {

		@Override
		public boolean onDown(MotionEvent e) {
			//Log.w(TAG, "onDown.."+e.getX()+"   "+ e.getY());
			/*
			lastProgress+=10;
            if(lastProgress>=100)
				lastProgress=100;
			 int attenuation = 100 - lastProgress;
                int millibel = attenuation * -50;
                setVolumeAudioPlayer(millibel);
			selectClip(3, 1);
			*/
			return false;
		}

		@Override
		public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
				float velocityY) {
			// TODO Auto-generated method stub
			return false;
		}

		@Override
		public void onLongPress(MotionEvent e) {
			MsgBox box=new MsgBox(MkrNavMainAct.this);
			
			box.show_route_to("Options",e.getX(),e.getY());
			
		}

		@Override
		public boolean onScroll(MotionEvent e1, MotionEvent e2,
				float distanceX, float distanceY) {
						
			MkrNavLib.drag(distanceX,distanceY,1);
			return true;
			
		}

		@Override
		public void onShowPress(MotionEvent e) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public boolean onSingleTapUp(MotionEvent e) {
			// TODO Auto-generated method stub
			return false;
		}

		@Override
		public void onGesture(GestureOverlayView overlay, MotionEvent event) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onGestureCancelled(GestureOverlayView overlay,
				MotionEvent event) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onGestureEnded(GestureOverlayView overlay, MotionEvent event) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onGestureStarted(GestureOverlayView overlay,
				MotionEvent event) {
			// TODO Auto-generated method stub
			
		}

		
		
	}

   

    public static native void nativeOnCreate();
    public static native void nativeOnResume();
    public static native void nativeOnPause();
    public static native void nativeOnStop();
    public static native void nativeSetSurface(Surface surface);

	public static native void createEngine();
    public static native void createBufferQueueAudioPlayer();
	public static native void setVolumeAudioPlayer(int millibel);
    public static native boolean selectClip(int which, int count);
	public static native void shutdownAudio();

    static {
        MkrNavLib.initLibs();
    }

    public static final int SPEECHTT_EXTERNAL_REQUEST_CODE               =    0x00001000;
   	public static final int TTS_DATA_CHECK_CODE               			 =    0x00002000;
   	public static final int TTS_DATA_INSTALL_CODE               	     =    0x00004000;
   	
}
