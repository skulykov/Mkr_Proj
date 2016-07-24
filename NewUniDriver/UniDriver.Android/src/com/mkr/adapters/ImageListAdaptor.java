package com.mkr.adapters;

import java.io.File;
import java.util.List;

import com.mkr.unidriver.R;
import com.mkr.utils.MediaFile;

import android.content.Context;
import android.graphics.Bitmap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;

public class ImageListAdaptor extends BaseAdapter {

	List<String> items=null;
	Context ctx;
	public ImageListAdaptor(Context ctx,List<String> fileList) {
		items=fileList;
		this.ctx=ctx;
	}

	@Override
	public int getCount() {
		return items.size();
	}

	@Override
	public Object getItem(int pos) {
		String str=items.get(pos);
		return str;
	}

	@Override
	public long getItemId(int pos) {
		return pos;
	}

	@Override
	public View getView(int pos, View view, ViewGroup parent) {
		View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.horiz_list_img_item, null);
		if (items != null && items.size() > pos) {
			if (items.get(pos).toLowerCase ().endsWith (".jpg")) {
				String imageFilePath = MediaFile.GetPictureFullPath( items.get(pos));
				File _file = new File(imageFilePath);
				if (_file.exists()) {
					Bitmap bitmap = MediaFile.ReadPictureThumbNail(/*ctx,*/_file/*,imageFilePath*/);
					//using (/*LoadAndResizeBitmap (imageFilePath, 160, 100)*/) {
					    if(bitmap!=null)
					    {
					    	ImageView iv=(ImageView )	v.findViewById (R.id.horizImageItem);
					    	iv.setImageBitmap (bitmap);
					    }
					
				} else {
					ImageView iv=(ImageView )	v.findViewById (R.id.horizImageItem);
					iv.setImageResource (R.drawable.ic_cancel);
				}
			} /*else if (_items [position].ToLower ().EndsWith (".mp4")) {
				retval.FindViewById<ImageView> (Resource.Id.horizVideoThumNail).SetImageBitmap (Android.Media.ThumbnailUtils.CreateVideoThumbnail (_items[position], Android.Provider.ThumbnailKind.MiniKind));
				//Set the video play button water mark on the recorded video thumbNail
				FrameLayout videoFL = retval.FindViewById<FrameLayout> (Resource.Id.horizVideoFrameLayout);
				videoFL.Foreground = parent.Context.Resources.GetDrawable (Resource.Drawable.ic_play);
				videoFL.SetForegroundGravity (GravityFlags.Center);
			}*/
		}
		return v;
	}

}
