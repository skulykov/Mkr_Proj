package com.girish.cameraLibrary;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.sql.Timestamp;
import java.util.Date;

import com.mkr.unidriver.R;
import com.mkr.utils.MediaFile;


import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.LinearLayout.LayoutParams;

public class CameraClsActivity extends Activity{
	//Default Camera
	private static final int CAMERA_PIC_REQUEST = 11111; 
	private static OnPictureTaken mOnPictureTaken=null;
	public static boolean useDefaultCamera = false;
	//Custom Camera
	private CamPreview mPreview;
	private RelativeLayout cameraFrame;
	private ImageView iconCapture;
	private boolean shouldGoBack = true;
//	private Bitmap CaptureImage = null;

	
	
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	
		if(useDefaultCamera){
			Intent cameraIntent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE);
			startActivityForResult(cameraIntent, CAMERA_PIC_REQUEST);
		} else {
			
	        requestWindowFeature(Window.FEATURE_NO_TITLE);
	       
	        //getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
			
			//setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
			setContentView(R.layout.custom_camera_layout);
			CreateView_1();
	        mPreview = new CamPreview(/*getApplicationContext()*/this);
	        cameraFrame.addView(mPreview);

	        iconCapture.setOnClickListener(new OnClickListener() {
				
				@Override
				public void onClick(View v) {
					mPreview.mCamera.takePicture(new ShutterCallback() {
						
						@Override
						public void onShutter() {
							
							cameraFrame.setVisibility(View.GONE);
							iconCapture.setVisibility(View.GONE);
							//Imagebackground.setVisibility(View.GONE);
						}
					}, null, new PictureCallback() {
						
						@Override
						public void onPictureTaken(byte[] data, Camera camera) {
							makeCameraVisible(false);
						  /*
							Bitmap bitmap =getBitmapFromView(cameraFrame);
							String fname=MediaFile.GenerateMediaName("veh__",".jpg");
							String ImagePath=MediaFile.GetPictureFullPath(fname);
							File pictureFile = new File(ImagePath);
							try {
						        pictureFile.createNewFile();
						        FileOutputStream oStream = new FileOutputStream(pictureFile);
						        bitmap.compress(CompressFormat.PNG, 100, oStream);
						        oStream.flush();
						        oStream.close();
						    } catch (IOException e) {
						        e.printStackTrace();
						        Log.i("TAG", "There was an issue saving the image.");
						    }  
							*/
							
							if(mOnPictureTaken!=null)
							   mOnPictureTaken.pictureTaken(data, camera);
							
						   finish();
						}
					});
				}
			});
		}
	}
	private Bitmap getBitmapFromView(View view) {
	    //Define a bitmap with the same size as the view
	    Bitmap returnedBitmap = Bitmap.createBitmap(view.getWidth(), view.getHeight(),Bitmap.Config.ARGB_8888);
	    //Bind a canvas to it
	    Canvas canvas = new Canvas(returnedBitmap);
	    //Get the view's background
	    Drawable bgDrawable =view.getBackground();
	    if (bgDrawable!=null) {
	        //has background drawable, then draw it on the canvas
	        bgDrawable.draw(canvas);
	    }   else{
	        //does not have background drawable, then draw white background on the canvas
	        canvas.drawColor(Color.WHITE);
	    }
	    // draw the view on the canvas
	    view.draw(canvas);
	    //return the bitmap
	    return returnedBitmap;
	}

	public static void setOnPictureTakenListner(OnPictureTaken activity){
		mOnPictureTaken = activity;
	}
	
	public void CreateView_1(){
		cameraFrame=(RelativeLayout)findViewById(R.id.camera_ll_id);
		iconCapture=(ImageView)findViewById(R.id.capture_imgv1);
		iconCapture.setImageResource(R.drawable.capture_icon);
	}
	
	public void makeCameraVisible(boolean cameraVisible){
		if(cameraVisible){
			shouldGoBack = true;
			cameraFrame.setVisibility(View.VISIBLE);
			iconCapture.setVisibility(View.VISIBLE);
			//Imagebackground.setVisibility(View.GONE);
		//	CaptureImage.recycle();
		}else{
			shouldGoBack = false;
			cameraFrame.setVisibility(View.GONE);
			iconCapture.setVisibility(View.GONE);
			//Imagebackground.setVisibility(View.VISIBLE);
		}
		
	}
	
	
}
