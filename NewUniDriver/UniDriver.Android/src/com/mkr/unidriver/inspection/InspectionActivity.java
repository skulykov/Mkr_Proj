package com.mkr.unidriver.inspection;

import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;

import com.mkr.service.MkrApp;
import com.mkr.unidriver.BaseDrawerActivity;
import com.mkr.unidriver.MainDrawerActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;

import com.mkr.views.InspectionReportView;

public class InspectionActivity extends  BaseDrawerActivity {
	
	/*static*/ InspectionReportView inspView = null;
	/*static*/ int inspection_type=0;
	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        Intent i=getIntent();
	        inspection_type=i.getIntExtra(GConst.EXTRA_REG_INSPECTION,-1);
	        //setContentView(R.layout.inspection_reports_layout);
	        LayoutInflater inflator = (LayoutInflater)getSystemService (Context.LAYOUT_INFLATER_SERVICE);
			View view = inflator.inflate (R.layout.inspection_reports_layout, null, true);
			Fragment fragment = MainDrawerActivity.current_fragment;
			//Global.currentInspectionType = SentinelMobile.Shared.Data.InspectionType.PostTrip;
			inspView = new InspectionReportView (MkrApp.GetMainActivity(), fragment, view);
			setContentView(view);
	       
			
	      //  SetMainTitles("Pre Trip Inspection", R.layout.custom_ab_help);
	 }
	 @Override
	 public void onResume() 
	 {
		 super.onResume ();
		inspView.UpdateReportList (inspection_type); // must be on Demand !!
	 }
	 /*
	 public static void Update(int ir_type)
	 {
		 inspView.UpdateReportList (ir_type); 
	 }*/
	 public void SyncReport(View v)
	 {
		 
	 }
	 public void NewReport(View v)
	 {
		 inspView.StartInspectionCatFormWithSlide(1,-1);
	 }
	 public void IRCancelBtn(View v)
	 {
		finish();
	 }
	
}
