package com.mkr.unidriver;

import java.io.File;
import java.io.FileOutputStream;

import com.girish.cameraLibrary.CamPreview;
import com.girish.cameraLibrary.CameraClsActivity;
import com.girish.cameraLibrary.OnPictureTaken;
import com.mkr.models.proto.CommPackets.PBoxIdVehicleRow;
import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.models.proto.CommPackets.PTimeLogRow.Builder;
import com.mkr.service.MkrNLib;
import com.mkr.utils.GConst;
import com.mkr.utils.MediaFile;
import com.mkr.utils.Util;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;

public class CreateVehicleActivity extends OnResultBaseActivity implements OnPictureTaken 
{
	//private CustomCamera mCustomCamera;
	private ImageView imgCapture;
	
	EditText vname,vdesc,VL,VLP,VW;
	String img_file_name="";
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//mLog.w("WL","ON Create..MainActivity..");
		
		setContentView(R.layout.vehicle_add_layout);
		
		//mCustomCamera = new CustomCamera(CreateVehicleActivity.this);
		//mCustomCamera.setPictureTakenListner(this);
		
		vname=(EditText)findViewById(R.id.veh_name_id);
		vdesc=(EditText)findViewById(R.id.veh_desc_id);
		VL=(EditText)findViewById(R.id.veh_lic_plate_id);
		VLP=(EditText)findViewById(R.id.veh_lic_prov_id);
		VW=(EditText)findViewById(R.id.veh_weight_id);
		
		imgCapture=(ImageView)findViewById(R.id.veh_img_id);
	}
	
	public void TakeVehPicture(View v)
	{
		//mCustomCamera.startCamera();
		CameraClsActivity.useDefaultCamera = false;
		CamPreview.front_camera = false;
		
		CameraClsActivity.setOnPictureTakenListner(this);
		startActivity(new Intent(this,CameraClsActivity.class));
	}
	public void CancelBtn(View v)
	{
		setResult(GConst.ActivityCanceled);
		finish ();
	}
	public void DoneBtn(View v)
	{
		
		com.mkr.models.proto.CommPackets.PBoxIdVehicleRow.Builder b=PBoxIdVehicleRow.newBuilder();
		
		b.setDesc(vdesc.getText().toString());
		b.setCid(0);
		b.setID(-1);
		b.setFlag(0);
		b.setVL(VL.getText().toString());
		b.setVLP(VLP.getText().toString());
		b.setMTS(0);
		b.setWeight(Util.str2int(VW.getText().toString()));
		b.setImg(img_file_name);
		
		byte[] ba = b.build().toByteArray();
		int rv=MkrNLib.Get().saveVehicle(ba);
		
		
		setResult(GConst.ActivityCanceled);
		finish ();
	}

	@Override
	public void pictureTaken(byte[] data, Camera camera) {
		
		FileOutputStream fos ;
		
		try {
			
			String fname=MediaFile.GenerateMediaName("tmp",".jpg");
			String ImagePath=MediaFile.GetPictureFullPath(fname);
			File f = new File(ImagePath);
			fos = new FileOutputStream(f);
			fos.write(data);
			fos.close();
			
			
			String new_file_name=MediaFile.GenerateMediaName("veh",".jpg");
			String new_file_name_path=MediaFile.GetPictureFullPath(new_file_name);
			Bitmap bm=MediaFile.ResizeFileAndSave(f, new_file_name_path,true);
			
			//Bitmap bm = BitmapFactory.decodeFile(new File(ImagePath/*new_file_name_path*/).getAbsolutePath());
			if(bm!=null)
			{
			imgCapture.setImageBitmap(bm);
			
			//img_file_name=new_file_name;
			}
	
		} catch (Exception e) {
			Log.e("CCC", e.getMessage());
			e.printStackTrace();
			throw new RuntimeException(e.getMessage());
		}
		
	}
}
