package com.mkr.unidriver;

import com.mkr.utils.GConst;

import android.os.Bundle;
import android.view.View;

public class CreateVehicleActivity extends OnResultBaseActivity 
{
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//mLog.w("WL","ON Create..MainActivity..");
		
		setContentView(R.layout.vehicle_add_layout);
	}
	
	public void CancelBtn(View v)
	{
		setResult(GConst.ActivityCanceled);
		finish ();
	}
	public void DoneBtn(View v)
	{
		setResult(GConst.ActivityCanceled);
		finish ();
	}
}
