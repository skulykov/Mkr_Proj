package com.mkr.unidriver.inspection;

import android.app.Activity;
import android.app.Fragment;
import android.app.TabActivity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.TabHost;
import android.widget.TabHost.OnTabChangeListener;
import android.widget.TabHost.TabSpec;
import android.widget.TextView;

import com.mkr.service.MkrApp;
import com.mkr.unidriver.MainDrawerActivity;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.HosActivity;
import com.mkr.unidriver.hos.dailylogs.HosDailyLogsPagerActivity;
import com.mkr.utils.GConst;

public class InspectionReportActivity extends TabActivity
{
	TabHost tabHost=null;
	 @Override
	  public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.inspection_reports_tabs);
	        tabHost = (TabHost)findViewById(android.R.id.tabhost);
	        if(tabHost!=null)
	        {
	    	CreateTab(GConst.REQUEST_PreTripIR,/*PreTInspectionActivity.class,*/ "T1", "Pre Trip", R.drawable.pre_trip);
			CreateTab(GConst.REQUEST_PostTripIR,/*PostTInspectionActivity.class,*/ "T2", "Post Trip", R.drawable.post_trip);
	        UpdateTitles();
	        }
	       /* tabHost.setOnTabChangedListener(new OnTabChangeListener(){
                @Override
                public void onTabChanged(String tabId) {
                	int vv=GConst.REQUEST_PreTripIR;
                   if(tabId.equals("T2")){
                	   vv=GConst.REQUEST_PostTripIR;  
                    }
                   InspectionActivity.Update(vv);
                 }
           });*/

	 }
	 private void CreateTab(/*Class<?> activityType,*/int type, String tag, String label, int drawableId )
		{
		    Intent intent = new Intent(this, /*activityType*/InspectionActivity.class);
		    intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.putExtra(GConst.EXTRA_REG_INSPECTION, /*GConst.REQUEST_PreTripIR*/type);
		   // TabHost th=new TabHost(this);
		    TabSpec spec = tabHost.newTabSpec(tag);
		    //var drawableIcon = Resources.GetDrawable(drawableId);
			LayoutInflater inflator = (LayoutInflater)getSystemService (Context.LAYOUT_INFLATER_SERVICE);
			View view = inflator.inflate (R.layout.my_insp_tab, null, true);
			TextView tv=((TextView)view.findViewById(R.id.tab_text));
			tv.setText(label);
			tv .setTypeface(null, Typeface.BOLD);

			((ImageView)view.findViewById(R.id.tab_icon)).setImageResource(drawableId);
		    spec.setIndicator(/*label, drawableIcon*/view);
		    spec.setContent(intent);

		    tabHost.addTab(spec);
		}
	 void UpdateTitles()
	 {
		 MainDrawerActivity act = (MainDrawerActivity)MkrApp.GetMainActivity();
			if( act!= null)
			{
				act.SetMainTitles("Inspection Reports");
			} 
	 }
	
	 /*
	 public void StartIRActivity (int flag)
	 {
			Intent intent=new Intent (this, InspectionActivity.class);
			intent.putExtra(GConst.EXTRA_REG_INSPECTION,Integer.toString(flag));
			startActivityForResult(intent, flag);
	 }*/
	 
}
