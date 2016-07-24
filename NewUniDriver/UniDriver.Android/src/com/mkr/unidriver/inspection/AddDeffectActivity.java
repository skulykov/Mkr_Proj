package com.mkr.unidriver.inspection;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.LinearLayout;

import com.mkr.adapters.ImageListAdaptor;
import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.models.proto.CommPackets.PInspectionItem.Builder;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;
import com.mkr.utils.MediaFile;
import com.mkr.utils.Util;

public class AddDeffectActivity extends OnResultBaseActivity {
	private static final String Intent = null;
	boolean is_defect=false,ro_flag=false;
	Activity _this;
	GridView horiListView=null;
	List<String> fileList = null;
	ImageListAdaptor ImgListAdapter  = null;
	EditText textComment=null;
	File _curFile=null;
	PInspectionItem defect_ii=null;
	@Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        _this=this;
	       /* Intent i=getIntent();
	        is_defect=i.getBooleanExtra(GConst.EXTRA_HAS_DEFECT,false);
	        ro_flag=i.getBooleanExtra(GConst.EXTRA_IS_READONLY,false);*/
	        String ttl= GConst.currentInspectionItem.getDefect();//"Edit defect ";
	        
	        setContentView (R.layout.inspection_add_deffect);
	        SetMainTitles(ttl, R.layout.custom_ab_help);
	        
	        textComment = (EditText)findViewById (R.id.txt_deffect_comment);
	        textComment.setOnKeyListener(new OnKeyListener(){
	            
				@Override
				public boolean onKey(View arg0, int keyCode, KeyEvent event) {
					if(keyCode == event.KEYCODE_ENTER){
						Util.hideSoftKeyboard(_this);
	                }
					return true;
				}
	        });
	       
	        fileList = new ArrayList<String> ();
	        defect_ii=InspHelper.GetDefect(GConst.currentInspectionItem.getIID());
	        if(defect_ii!=null)
	        {
	        	is_defect=true;
	        	textComment.setText(defect_ii.getComments());
	        	if(defect_ii.getMediaFilesCount()>0)
	        	{
	        		for(String str : defect_ii.getMediaFilesList())
	        		{
	        			fileList.add (str);
	        		}
	        	}
	        }
	        ro_flag=InspHelper.isEditable();
	        LinearLayout toolbar = (LinearLayout)findViewById (R.id.ll_defect_tb);
	        if(!ro_flag)
	        {
	        	toolbar.setVisibility(View.GONE);
	        }
			
			horiListView = (GridView) findViewById(R.id.gridview);
			ImgListAdapter  = new ImageListAdaptor (this,fileList);
			horiListView.setAdapter(ImgListAdapter);
			
			horiListView.setOnItemClickListener(listener) ;
			
	}
	
	OnItemClickListener listener=new OnItemClickListener(){
		@Override
		public void onItemClick(AdapterView<?> arg0, View v, int pos,long arg3) {
			if(fileList != null && fileList.size() > pos){
				//Launch the ViewMediaActivity to view media
				Intent serverIntent = new Intent(_this, ViewMediaActivity.class);
				serverIntent.putExtra (GConst.EXTRA_MEDIA_FILES, fileList.get(pos));
				startActivity(serverIntent);
				overridePendingTransition(R.anim.pull_in_right,R.anim.push_out_left);
			}
			
		}
	
	};
	
	
	public void BtnSaveDefect(View v)
	{
		PInspectionItem.Builder bb= PInspectionItem.newBuilder(GConst.currentInspectionItem);
		boolean flag=false;
		
		if(fileList.size()>0)
		{
			
			for(int i = 0; i < fileList.size(); i++){
				//item.media_files.Add( fileList.get(i));
				bb.addMediaFiles( fileList.get(i));
			}
			flag=true;
		}
		if(textComment!=null&&textComment.getText().toString() !="")
		{
			bb.setComments(textComment.getText().toString());				
			flag=true;
		}
		if(flag)
		{
			InspHelper.addDefect(bb.build().toByteArray());
		}
		Finish_Activity(GConst.G_RESULT_OK);
	}
	public void BtnClearDefect(View v)
	{
	   
	}
	public void BtnGetPicture(View v)
	{
		int imgLimit=3;
		if(IsThereAnAppToTakePictures()){
			if (fileList.size() < imgLimit) {
				Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
				_curFile =new File(MediaFile.GetPictureFullPath(MediaFile.GenerateMediaName("a_img",".jpg")));
				intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(_curFile));
				startActivityForResult(intent, GConst.REQUEST_TAKE_PICTURE);
			}else{
				//Toast.MakeText(this, Resources.GetString(Resource.String.camera_click_toast), ToastLength.Long).Show();
			}
		}
	}
	private boolean IsThereAnAppToTakePictures()
	{
		final PackageManager packageManager = getPackageManager();
		Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
		List<ResolveInfo> availableActivities = packageManager.queryIntentActivities(intent,0/* PackageInfo.*/);
		return availableActivities != null && availableActivities.size() > 0;
	}
	 public void ABBackBtn(View v)
	 {
		 setResult (GConst.G_RESULT_OK);
		 finish();
		 overridePendingTransition(R.anim.pull_in_left,R.anim.push_out_right);
	 }
	 @Override
	 public  void onActivityResult (int requestCode, int resultCode, Intent data)
	 {
		 super.onActivityResult (requestCode, resultCode, data);
		 switch(requestCode)
			{

			case GConst.REQUEST_TAKE_PICTURE:
				if( resultCode == -1/*GConst.G_RESULT_OK*/){
					if (_curFile != null) {
						//fileList.Add (_curFile.Path);
						//the file size is big _> need to resize
						String new_file_name=MediaFile.GenerateMediaName("a_img",".jpg");
						String new_file_name_path=MediaFile.GetPictureFullPath(new_file_name);
						Bitmap bm=MediaFile.ResizeFileAndSave(_curFile, new_file_name_path,false);
						if(bm!=null)
						{
							fileList.add (/*_curFile.getName()*/new_file_name);
							ImgListAdapter.notifyDataSetChanged ();
						}
						_curFile=null;
					}
				}
				break;
			}
	 }
}
