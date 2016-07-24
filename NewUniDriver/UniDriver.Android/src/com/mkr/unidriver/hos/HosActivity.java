package com.mkr.unidriver.hos;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import com.mkr.models.MyLocation;
import com.mkr.models.RecapSummary;

import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.models.proto.CommPackets.PTimeLogRow.Builder;
import com.mkr.service.LocationService;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.MainDrawerActivity;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.dailylogs.HosDailyLogsPagerActivity;
import com.mkr.utils.DateUtil;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;
import com.mkr.views.HosGraphView;
import com.mkr.views.HosMenuQA;
import com.mkr.views.HosStatusCustomView;
import com.mkr.views.HosStatusPopup;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class HosActivity extends OnResultBaseActivity
{
	HosStatusCustomView scv=null;
	HosGraphView hgv = null; 
	TextView hos_cycle_v,hos_ts_v,avail_h_onduty_v , h_addr_v ,avaiol_h_dr_v,tz_v ;
	RecapSummary rs=null;
	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.hos_main_layout);
	        
	        LinearLayout v = (LinearLayout)findViewById (R.id.hos_status_view);
			scv = new HosStatusCustomView (this, v);
			v.addView (scv);
			
			LinearLayout v1 = (LinearLayout)findViewById (R.id.hos_graph_view);
			hgv = new HosGraphView (this);
			v1.addView (hgv);
			
			hos_cycle_v = (TextView)findViewById (R.id.hos_cycle_id);
			Button menuButton = (Button)findViewById (R.id.hos_opt_btn);
			menuButton.setOnClickListener(new View.OnClickListener() {
	            @Override
	            public void onClick(View v) {
	            	ProcessCustomOptionsMenu (v);
	            }
	        });
						
			hos_ts_v = (TextView)findViewById (R.id.hos_ts_id);
			hos_ts_v.setText(DateUtil.getTS(null));//TimeUtil.GetLongTimeStr (DateTime.Now);

			
			avail_h_onduty_v = (TextView)findViewById (R.id.hos_avail_on_d_id);
			avaiol_h_dr_v = (TextView)findViewById (R.id.hos_avail_dr_id);

			h_addr_v = (TextView)findViewById (R.id.hos_h_addr_id);
			tz_v = (TextView)findViewById (R.id.hos_tz_id);

			
		//	RefreshDashboardView ();
						
			SetTitle("Hours of service");
	 }
	 @Override
	 public void onResume()
	 {
	   super.onResume();
	   RefreshDashboardView ();
	 }
	 void debugDumpTLR()
	 {
			List<PTimeLogRow> list=this.hgv.getList();
			if(list!=null&&!list.isEmpty())
			{
			  for(PTimeLogRow tlr : list)
			  {
				  String s=DateUtil.getTS(tlr.getLogTime());
				  String str=s+"  ev="+Integer.toString(tlr.getEvent());
				  Log.d("HHH",str);
			  }
			}
	 }
	 /*
	 void debugDumpRecap()
	 {
			List<RecapRow> list=MkrNLib.Get().getRecapList();
			if(list!=null&&!list.isEmpty())
			{
			  for(RecapRow r : list)
			  {
				  Date dt=new Date(r.date);
				  String str=dt.toString()+"  cl:"+r.cycle+"  today="+Integer.toString(r.today);
				  Log.d("HHH",str);
			  }
			}
	 }*/
	 boolean refreshTlr(PTimeLogRow tlr)
	 {
		 boolean tlr_updated=false;
		 if(tlr!=null&&scv != null)
 		{
 				scv.refreshDriverStatusGui (tlr.getEvent());
 				tlr_updated=true;
 		}
		 return tlr_updated;
	 }
	 public void RefreshDashboardView ()
		{
         		hos_cycle_v.setText(MkrNLib.Get().GetStringFromCurrentHosCycle () );               //clc.ToString();
				hos_cycle_v.invalidate();
				
		 
		 //debugDumpRecap();
			this.hgv.updateDrawArray (Util.getUtcDate(),true);
			//debugDumpTLR();
			boolean tlr_updated=false;
			List<PTimeLogRow> list=this.hgv.getList();
            if(list!=null)
            {
            	int len=list.size();
            	if(len>0)
            	{
            		tlr_updated=refreshTlr(list.get(len-1));
            	}
            }
            if(!tlr_updated){
            	refreshTlr(MkrNLib.Get().GetLastDriverStatus());
            	
            }
            
            this.hgv.invalidate(); // redraw Hos Graph
            /*
			cow_et.Text=Global.CoDriver;
			*/
		}
	 public void UpdateFromHosEngine(String str)
	 {
		 rs=HosHelper.parseRecapSummary(str);
		 runOnUiThread(new Runnable() {
		        @Override
		        public void run() {
		        	 avaiol_h_dr_v.setText(Util.ConvertMinutesToHoursandMinutes(rs.avail_driving_min));
		    		 avail_h_onduty_v.setText(Util.ConvertMinutesToHoursandMinutes(rs.avail_onduty_min));
		    		 
		    		 avail_h_onduty_v.invalidate();
		    		 avaiol_h_dr_v.invalidate();
		    		 
		    		 hos_ts_v.setText(DateUtil.getTS(null));
					 hos_ts_v.invalidate();
		    		 
		    		 
		    		 hgv.invalidate(); // redraw Hos Graph
		        }
		    });
	 }

	 HosMenuQA hos_menuqa=null;
		void ProcessCustomOptionsMenu (View sender)
		{
			
				if (hos_menuqa == null)
					hos_menuqa = new HosMenuQA (this);
				hos_menuqa.Show ((View)sender);
			
		}
	
	 public void createStatusDialog (View v)
		{
			HosStatusPopup hos_status_pu=new HosStatusPopup (this);
			if(hos_status_pu!=null)
				hos_status_pu.Show(v); 
		}
	 
		public void selectDriverStatus(int ind)
		{
			
			int rv=HosHelper.CreateDriverStatus(ind,-1,"",0,0,0);
			if(rv>0)
			{
				 Log.d("HHH","rv="+Integer.toString(rv));
				 if (scv != null) {
						scv.refreshDriverStatusGui (rv);
				}
				 RefreshDashboardView ();
			}
			
		}
		void resetExcStatuses()
		{
			/*HOSAgent.set30MinBreak (false);
			HOSAgent.set_personal_driving (false);
			HOSAgent.set_emergency_driving (false);*/
		}
		
		//---------------------------------------------------------------------
		
		public void StartHosDailyLogsActivity ()
		{
			//java.lang.IllegalStateException: Fragment HosViewFragment{421f5aa8} not attached to Activity
			//Fragment frag=MainDrawerActivity.current_fragment;
			//Intent intent=new Intent (HosActivity.this, HosDailyLogsPagerActivity.class);
			//frag.startActivityForResult(intent, GConst.REQUEST_HOS_DAILY_LOGS);
			
			Intent intent=new Intent (HosActivity.this, HosDailyLogsPagerActivity.class);
			startActivityForResult(intent, GConst.REQUEST_HOS_DAILY_LOGS);
		}
		public void StartHosRulesActivity ()
		{
			Fragment frag=MainDrawerActivity.current_fragment;
			Intent intent=new Intent (HosActivity.this, RulesActivity.class);
			frag.startActivityForResult(intent, GConst.REQUEST_HOS_RULES);
		}
		
		public void StartCoWorkerLoginActivity ()
		{
			Fragment frag=MainDrawerActivity.current_fragment;
			Intent intent=new Intent (HosActivity.this, CoWorkerLoginActivity.class);
			frag.startActivityForResult(intent, GConst.REQUEST_HOS_RULES);
		}
		public void StartCoWorkerLogoutActivity ()
		{
			Fragment frag=MainDrawerActivity.current_fragment;
			Intent intent=new Intent (HosActivity.this, CoWorkerLogoutActivity.class);
			frag.startActivityForResult(intent, GConst.REQUEST_HOS_RULES);
		}
		public void StartEmailActivity ()
		{
			Fragment frag=MainDrawerActivity.current_fragment;
			Intent intent=new Intent (HosActivity.this, EmailActivity.class);
			frag.startActivityForResult(intent, GConst.REQUEST_HOS_RULES);
		}
		
}
