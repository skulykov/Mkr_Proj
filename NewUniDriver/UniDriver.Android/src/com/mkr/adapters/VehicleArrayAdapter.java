package com.mkr.adapters;

import java.util.List;

import com.koushikdutta.urlimageviewhelper.UrlImageViewCallback;
import com.koushikdutta.urlimageviewhelper.UrlImageViewHelper;
import com.mkr.models.proto.CommPackets.PBoxIdVehicleRow;
import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.unidriver.R;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.OvershootInterpolator;
import android.view.animation.ScaleAnimation;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;



public class VehicleArrayAdapter extends ArrayAdapter<PBoxIdVehicleRow>{

	Context ctx ;
    int resId=0;
	public VehicleArrayAdapter(Context context, int textViewResourceId, List<PBoxIdVehicleRow> objects) {
		super(context, textViewResourceId, objects);
		ctx=context;
		resId=textViewResourceId;
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View v = convertView;
		if (v == null) {
			LayoutInflater inflater = ((Activity) ctx).getLayoutInflater();
			v = inflater.inflate(this.resId, parent,false);
		}
		PBoxIdVehicleRow item = getItem(position);
		
		TextView idTextView = (TextView)v.findViewById (R.id.veh_desc_id);
		idTextView.setText(item.getDesc() + " [" + item.getID() + "]");
		if (!item.getImg().isEmpty()) {
			final ImageView imgV = (ImageView)v.findViewById (R.id.veh_img_id);
			String url = "http://www.mikerel.com/mkr_files/common/v_img/" + item.getImg();//@"http://www.mikerel.com/fmportal/common/v_img/toyta.jpg";
			//https://github.com/koush/UrlImageViewHelper/blob/master/UrlImageViewHelperSample/src/com/koushikdutta/urlimageviewhelper/sample/UrlImageViewHelperSample.java
			//imgV.setUrlDrawable (url/*,Resource.Drawable.DefaultUrlImage*/);
			 UrlImageViewHelper.setUrlDrawable(imgV, url, R.drawable.loading, new UrlImageViewCallback() { 

								@Override
								public void onLoaded(ImageView imageView,
										Bitmap loadedBitmap, String url,
										boolean loadedFromCache) {
									  ScaleAnimation scale = new ScaleAnimation(0, 1, 0, 1, ScaleAnimation.RELATIVE_TO_SELF, .5f, ScaleAnimation.RELATIVE_TO_SELF, .5f); 
				                        scale.setDuration(300); 
				                        scale.setInterpolator(new OvershootInterpolator()); 
				                        imageView.startAnimation(scale); 
									
								} 
			}); 

		}
		
		return v;
	}

}
