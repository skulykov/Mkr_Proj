package com.mkr.unidriver.inspection;

import java.io.File;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;

import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;
import com.mkr.utils.MediaFile;

public class ViewMediaActivity extends OnResultBaseActivity{

	@Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView (R.layout.view_media);
	       // _this=this;
	        Intent i=getIntent();
	       
	        String ttl=i.getStringExtra(GConst.EXTRA_MEDIA_FILES);
	        if(ttl!="")
	        {
	        	if (ttl.toLowerCase ().endsWith (".jpg")) {
					String imageFilePath = MediaFile.GetPictureFullPath( ttl);
					File _file = new File(imageFilePath);
					if (_file.exists()) {
						Bitmap tmpBitmap = new BitmapDrawable(getResources(), imageFilePath).getBitmap();
						int nh = (int) ( tmpBitmap.getHeight() * (512.0 / tmpBitmap.getWidth()) );
						Bitmap bm = Bitmap.createScaledBitmap(tmpBitmap, 512, nh, true);
						Bitmap bitmap = MediaFile.rotateImageIfRequired (/*this,*/ bm,/* Uri.fromFile (_file), imageFilePath*/_file);
						ImageView _imageView =(ImageView) findViewById(R.id.img_view);
						_imageView.setImageBitmap (bitmap);
						_imageView.setVisibility( View.VISIBLE);

						tmpBitmap.recycle();
						tmpBitmap=null;
						
					}
						
				}
	        }
	        
	      
	        SetMainTitles(ttl, R.layout.custom_ab_help);
	}
	
	 public void ABBackBtn(View v)
	 {
		 setResult (GConst.G_RESULT_OK);
		 finish();
		 overridePendingTransition(R.anim.pull_in_left,R.anim.push_out_right);
	 }
}
