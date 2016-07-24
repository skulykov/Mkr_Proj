package com.mkr.utils;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import android.app.Application;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.MemoryFile;
import android.provider.MediaStore.Images.Media;
import android.support.v7.widget.LinearLayoutCompat.OrientationMode;
import android.widget.ImageView;

public class MediaFile {

	public static String GenerateMediaName(String pref,String ext)
	{
		long ts=System.currentTimeMillis();
		String rv = "_"+ts;
		return (pref+rv+ext);
	}
	
	static File CreateDirectoryForMedia(String dir)
	{
		File _file = new File(dir);
		if (!_file.exists())
		{
			_file.mkdirs();
		}
		return _file;
	}
	static String getDocDir(String dirName)
	{
		//var documentsDirectory = Android.OS.Environment.GetExternalStoragePublicDirectory(Android.OS.Environment.DirectoryPictures);//Environment.GetFolderPath(Environment.SpecialFolder.Personal);
		String docPath="/sdcard/unidriver";//documentsDirectory.Path;
		String dir=docPath+"/" +dirName;
		CreateDirectoryForMedia(dir);
		return dir;
	}
	//File f = new File(Environment.getExternalStorageDirectory(), "a directory");
	public static String GetPictureFullPath(String filename)
	{
		String dir=getDocDir("pictures");
		String jpgFilename = dir+"/"+ filename;
		return jpgFilename;
	}
	public static Bitmap LoadAndResizeBitmap(String fileName, int height, int width)
	{
		Bitmap resizedBitmap = null;
		try{
			// First we get the the dimensions of the file on disk
			BitmapFactory.Options options = new BitmapFactory.Options();
			options.inJustDecodeBounds = true;
			BitmapFactory.decodeFile(fileName, options);

			// Next we calculate the ratio that we need to resize the image by
			// in order to fit the requested dimensions.
			int outHeight = options.outHeight;
			int outWidth = options.outWidth;
			int inSampleSize = 1;

			if (outHeight > height || outWidth > width)
				inSampleSize = outWidth > outHeight ? outHeight / height : outWidth / width;

			// Now we will load the image and have BitmapFactory resize it for us.
			options.inSampleSize = inSampleSize;
			options.inJustDecodeBounds = false;
			resizedBitmap = BitmapFactory.decodeFile(fileName, options);
		} catch(Exception ex){
			//throw(ex);
		}

		return resizedBitmap;
	}
	/* http://developer.android.com/training/camera/photobasics.html
	 * 
	private void setPicInIV(String mCurrentPhotoPath,ImageView mImageView) {
	    // Get the dimensions of the View
	    int targetW = mImageView.getWidth();
	    int targetH = mImageView.getHeight();

	    // Get the dimensions of the bitmap
	    BitmapFactory.Options bmOptions = new BitmapFactory.Options();
	    bmOptions.inJustDecodeBounds = true;
	    BitmapFactory.decodeFile(mCurrentPhotoPath, bmOptions);
	    int photoW = bmOptions.outWidth;
	    int photoH = bmOptions.outHeight;

	    // Determine how much to scale down the image
	    int scaleFactor = Math.min(photoW/targetW, photoH/targetH);

	    // Decode the image file into a Bitmap sized to fill the View
	    bmOptions.inJustDecodeBounds = false;
	    bmOptions.inSampleSize = scaleFactor;
	    bmOptions.inPurgeable = true;

	    Bitmap bitmap = BitmapFactory.decodeFile(mCurrentPhotoPath, bmOptions);
	    mImageView.setImageBitmap(bitmap);
	}*/
	public static int getCameraPhotoOrientation(/*Context context, Uri imageUri, String imagePath*/File imageFile){
		int rotate = 0;
		try {
		//	context.getContentResolver().notifyChange(imageUri, null);
		//	File imageFile = new File(imagePath);
			ExifInterface exif = new ExifInterface(imageFile.getAbsolutePath());
			int orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);

			switch (orientation) {
			case (int)/*Android.Media.Orientation.Rotate270*/ExifInterface.ORIENTATION_ROTATE_270:
				rotate = 270;
				break;
			case (int)ExifInterface.ORIENTATION_ROTATE_180:
				rotate = 180;
				break;
			case (int)ExifInterface.ORIENTATION_ROTATE_90:
				rotate = 90;
				break;
			}
		} catch (Exception e) {
			//throw(e);
		}
		return rotate;
	}
	static Bitmap rotate(Bitmap img,int rotation)
	{
		if(rotation!=0){
			Matrix matrix = new Matrix();
			matrix.postRotate(rotation);
			Bitmap rotatedImg = Bitmap.createBitmap(img, 0, 0, img.getWidth(), img.getHeight(), matrix, true);
			img.recycle();
			return rotatedImg;
		}else{
			return img;
		}
	}
	public static Bitmap rotateImageIfRequired(/*Context context,*/Bitmap img,/* Uri selectedImage, String imagePath*/File file) {
		int rotation=getCameraPhotoOrientation(/*context, selectedImage, imagePath*/file);
		return rotate(img,rotation);
	}
	public static Bitmap ReadPictureThumbNail (/*Context ctx,*/File file/*,String imageFilePath*/)
	{
		Bitmap bitmap=null;
		if (file.exists ()) {
			//using (var stream = new System.IO.MemoryStream ()) 
			OutputStream stream=new ByteArrayOutputStream();
			{

				//BitmapFactory.Options options = new BitmapFactory.Options { InJustDecodeBounds = true };
				//Bitmap tmpBitmap=BitmapFactory.DecodeFile(imageFilePath, options);
				Bitmap tmpBitmap = LoadAndResizeBitmap (/*imageFilePath*/file.getAbsolutePath(), 100, 160);

				int nh = (int) ( tmpBitmap.getHeight() * (160.0 / tmpBitmap.getWidth()) );  //512.0
				Bitmap bm = Bitmap.createScaledBitmap(tmpBitmap, 160, nh, true); //512

				bitmap = rotateImageIfRequired (/*ctx ,*/ bm,/* Uri.fromFile (file), imageFilePath*/file);

				bitmap.compress (Bitmap.CompressFormat.JPEG, 100, stream);

			}//using
		}
		return bitmap;
	}
	public static /*boolean*/Bitmap ResizeFileAndSave(File existing_file, String new_file_name,boolean rotate_flag)
	{
		//boolean rv=true;
		FileInputStream fis = null;
		FileOutputStream fo = null;
		File f = new File(new_file_name);
		try {
			f.createNewFile();
		} catch (IOException e2) {/*rv=false;*/return null;}
		try {
			    fis = new FileInputStream(existing_file);
			    fo = new FileOutputStream(f);
		    } 
			catch (FileNotFoundException e) {/*rv=false;*/return null;}
		Bitmap bm = BitmapFactory.decodeStream(fis);   //decodeFile(filename);
		int w=bm.getWidth(),h=bm.getHeight();
		int dstWidth=320,dstHeight=480;
		if(!rotate_flag)
		{
			if(w>h)
			{
				dstWidth=480;
				dstHeight=320;
			}
			bm = Bitmap.createScaledBitmap(bm, dstWidth, dstHeight, false);
		}
		else{
			float h1=(float) 480.0;
			float scale=(float) (h1/(h*1.0));
			float w1=w*scale;
			
			bm = Bitmap.createScaledBitmap(bm,(int) w1,(int) h1, false);
		}
		
		
		if(rotate_flag)
			bm=rotate(bm,90);
		else
			bm = rotateImageIfRequired (/*ctx ,*/ bm,/* Uri.fromFile (file), imageFilePath*/existing_file);
		ByteArrayOutputStream stream = new ByteArrayOutputStream();
		bm.compress(Bitmap.CompressFormat.JPEG, 100, stream);
		
		try {
			fo.write(stream.toByteArray());
			fo.close();
			
			//delete old file
			//existing_file.delete();
			
		} catch (IOException e) {/*rv=false;*/bm=null;}
		return bm;
	}
	
}
