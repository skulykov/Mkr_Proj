package com.girish.cameraLibrary;

import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

public class CamPreview extends SurfaceView implements SurfaceHolder.Callback{

	private SurfaceHolder mHolder;
	public Camera mCamera;
	private Context mContext;
	
	private final String LOG_TAG = "Camera Lib : MyPreview";
	public static boolean front_camera = false;
	
	private List<Camera.Size> mSupportedPreviewSizes;
    private Camera.Size mPreviewSize;

	
	public CamPreview(Context mContext){
		super(mContext);
		this.mContext = mContext;
		
		mHolder = getHolder();
		mHolder.addCallback(this);
		// Added for devices before 3.0 else it crashes 
		mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		//CreateCamera();
	}
	/*public static*/ void setCameraDisplayOrientation(Activity activity,
	         int cameraId, android.hardware.Camera camera) {
	     android.hardware.Camera.CameraInfo info =
	             new android.hardware.Camera.CameraInfo();
	     android.hardware.Camera.getCameraInfo(cameraId, info);
	     int rotation = activity.getWindowManager().getDefaultDisplay()
	             .getRotation();
	     int degrees = 0;
	     switch (rotation) {
	         case Surface.ROTATION_0: degrees = 0; break;
	         case Surface.ROTATION_90: degrees = 90; break;
	         case Surface.ROTATION_180: degrees = 180; break;
	         case Surface.ROTATION_270: degrees = 270; break;
	     }

	     int result;
	     if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
	         result = (info.orientation + degrees) % 360;
	         result = (360 - result) % 360;  // compensate the mirror
	     } else {  // back-facing
	         result = (info.orientation - degrees + 360) % 360;
	     }
	     camera.setDisplayOrientation(result);
	 }

	void CreateCamera(){
		
		try{
			if(mCamera == null)
				if(front_camera){
					Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
				    int cameraCount = Camera.getNumberOfCameras();
				    for ( int camIdx = 0; camIdx < cameraCount; camIdx++ ) {
				        Camera.getCameraInfo( camIdx, cameraInfo );
				        if ( cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT  ) {
				            try {
				            	mCamera = Camera.open( camIdx );
				            } catch (RuntimeException e) {
				                Log.e(LOG_TAG, "Camera failed to open: " + e.getLocalizedMessage());
				            }
				        }
				    }
				}else{
					mCamera = android.hardware.Camera.open();
				}
			
				mCamera.setPreviewDisplay(mHolder);
				/*
				Camera.Parameters parameters = mCamera.getParameters();
	            List<Size> sizes = parameters.getSupportedPictureSizes();
	            parameters.setPictureSize(sizes.get(0).width, sizes.get(0).height); // mac dinh solution 0
	            parameters.set("orientation","portrait");
	            List<Size> size = parameters.getSupportedPreviewSizes();
	            parameters.setPreviewSize(size.get(0).width,size.get(0).height);
	            mCamera.setParameters(parameters);
	        	//mCamera.setPreviewCallback(null);
	            */
			
	            mCamera.setDisplayOrientation(90);
				//setCameraDisplayOrientation((Activity)mContext,mCamera);
	            
	            // supported preview sizes
	            mSupportedPreviewSizes = mCamera.getParameters().getSupportedPreviewSizes();
	            for(Camera.Size str: mSupportedPreviewSizes)
	                    Log.e(LOG_TAG, str.width + "/" + str.height);
	            
		}catch(Exception e){
			Toast.makeText(mContext, e.getMessage(), 35).show();
			Log.e(LOG_TAG, e.getMessage()+"");
			e.printStackTrace();
			System.exit(0);
		}
		
		
		// // empty. surfaceChanged will take care of stuff

	}
	
	
	@Override
	public void surfaceCreated(SurfaceHolder surfaceHolder){
		CreateCamera();
		/*
		try{
			if(mCamera == null)
				if(front_camera){
					Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
				    int cameraCount = Camera.getNumberOfCameras();
				    for ( int camIdx = 0; camIdx < cameraCount; camIdx++ ) {
				        Camera.getCameraInfo( camIdx, cameraInfo );
				        if ( cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT  ) {
				            try {
				            	mCamera = Camera.open( camIdx );
				            } catch (RuntimeException e) {
				                Log.e(LOG_TAG, "Camera failed to open: " + e.getLocalizedMessage());
				            }
				        }
				    }
				}else{
					mCamera = android.hardware.Camera.open();
				}
			
				mCamera.setPreviewDisplay(mHolder);
				
				Camera.Parameters parameters = mCamera.getParameters();
	            List<Size> sizes = parameters.getSupportedPictureSizes();
	            parameters.setPictureSize(sizes.get(0).width, sizes.get(0).height); // mac dinh solution 0
	            parameters.set("orientation","portrait");
	            List<Size> size = parameters.getSupportedPreviewSizes();
	            parameters.setPreviewSize(size.get(0).width,size.get(0).height);
	            mCamera.setParameters(parameters);
	        	//mCamera.setPreviewCallback(null);
	          
			
	            mCamera.setDisplayOrientation(90);
	            
	            // supported preview sizes
	            mSupportedPreviewSizes = mCamera.getParameters().getSupportedPreviewSizes();
	            for(Camera.Size str: mSupportedPreviewSizes)
	                    Log.e(LOG_TAG, str.width + "/" + str.height);
	            
		}catch(Exception e){
			Toast.makeText(mContext, e.getMessage(), 35).show();
			Log.e(LOG_TAG, e.getMessage()+"");
			e.printStackTrace();
			System.exit(0);
		}
		*/
		
		
		// // empty. surfaceChanged will take care of stuff

	}
	
	@Override
	public void surfaceDestroyed(SurfaceHolder surfaceHolder){
		if(mCamera != null) {
			mCamera.stopPreview();
			mCamera.setPreviewCallback(null);
			mCamera.release();
			mCamera = null;
		}
	}
	
	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int w , int h){
		/*Camera.Parameters params = mCamera.getParameters();
		mCamera.setParameters(params);
		mCamera.startPreview();
		return;*/
		
		 Log.e(LOG_TAG, "surfaceChanged => w=" + w + ", h=" + h);
	        // If your preview can change or rotate, take care of those events here.
	        // Make sure to stop the preview before resizing or reformatting it.
	        if (mHolder.getSurface() == null){
	            // preview surface does not exist
	            return;
	        }

	        // stop preview before making changes
	        try {
	            mCamera.stopPreview();
	        } catch (Exception e){
	            // ignore: tried to stop a non-existent preview
	        }

	        // set preview size and make any resize, rotate or reformatting changes here
	        // start preview with new settings
	        try {
	            Camera.Parameters parameters = mCamera.getParameters();
	            parameters.setPreviewSize(mPreviewSize.width, mPreviewSize.height);
	            mCamera.setParameters(parameters);
	           // mCamera.setDisplayOrientation(90);
	            mCamera.setPreviewDisplay(mHolder);
	            mCamera.startPreview();

	        } catch (Exception e){
	            Log.d(LOG_TAG, "Error starting camera preview: " + e.getMessage());
	        }
          
	}
	
	@Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		
		int parentWidth = MeasureSpec.getSize(widthMeasureSpec);
        int parentHeight = MeasureSpec.getSize(heightMeasureSpec);

      
	//	int w=resolveSize(getSuggestedMinimumWidth(), widthMeasureSpec);
        final int width = parentWidth;
        final int height = parentHeight/2;//resolveSize(getSuggestedMinimumHeight(), heightMeasureSpec);

        if (mSupportedPreviewSizes != null) {
            mPreviewSize = getOptimalPreviewSize(mSupportedPreviewSizes, width, height);
        }

        float ratio=1;
        if(mPreviewSize!=null)
        {
	        if(mPreviewSize.height >= mPreviewSize.width)
	            ratio = (float) mPreviewSize.height / (float) mPreviewSize.width;
	        else
	            ratio = (float) mPreviewSize.width / (float) mPreviewSize.height;
        }
      
        // One of these methods should be used, second method squishes preview slightly
        setMeasuredDimension(width, (int) (width * ratio));
//        setMeasuredDimension((int) (width * ratio), height);
    }
	private Camera.Size getOptimalPreviewSize(List<Camera.Size> sizes, int w, int h) {
        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) h / w;

        if (sizes == null)
            return null;

        Camera.Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;

        int targetHeight = h;

        for (Camera.Size size : sizes) {
            double ratio = (double) size.height / size.width;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE)
                continue;

            if (Math.abs(size.height - targetHeight) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.height - targetHeight);
            }
        }

        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Camera.Size size : sizes) {
                if (Math.abs(size.height - targetHeight) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.height - targetHeight);
                }
            }
        }

        return optimalSize;
    }


	
	public void clear(){
		if(mCamera != null) {
			mCamera.stopPreview();
			mCamera.setPreviewCallback(null);
			mCamera.release();
			mCamera = null;
		}
	}
	
	
}
