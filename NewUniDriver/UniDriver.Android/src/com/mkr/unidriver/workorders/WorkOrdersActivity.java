package com.mkr.unidriver.workorders;

import java.util.ArrayList;
import java.util.List;

import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import com.mkr.adapters.WorkOrdersAdapter;
import com.mkr.chat.ChatUser;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRow;
import com.mkr.models.proto.CommPackets.PWorkOrderItem;
import com.mkr.models.proto.CommPackets.PWorkOrderList;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.MainDrawerActivity;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.unidriver.inspection.InspHelper;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;
import com.mkr.views.HosGraphView;
import com.mkr.views.HosStatusCustomView;

public class WorkOrdersActivity extends OnResultBaseActivity 
{
	ListView jobsLV=null;
	TextView title,title1,title2;
	PWorkOrderList aList=null;
	WorkOrdersActivity _this=null;
	WorkOrdersAdapter adapter = null;
	 @Override
	    public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.jobs_main_layout);
	        _this=this;
	        jobsLV = (ListView)findViewById (R.id.list_jobs_id);
			
	        title=(TextView)findViewById (R.id.wo_title_id);
			title1=(TextView)findViewById (R.id.wo_title1_id);
			title2=(TextView)findViewById (R.id.wo_title2_id);
	       			
			adapter = new WorkOrdersAdapter (this,R.layout.jobs_item_layout, null);
			jobsLV.setAdapter(adapter);
			jobsLV.setOnItemClickListener(adapter_listener);
			UpdateTitles();
			request4WOs();
	 }
	 AdapterView.OnItemClickListener adapter_listener=new AdapterView.OnItemClickListener() {
		  @Override
		  public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

			  PWorkOrderItem woi= aList.getWoItems(position);
			  if(woi!=null)
				{
					GConst.wo_item=woi;
					UpdateTitles ();
					Fragment frag=MainDrawerActivity.current_fragment;
					frag.startActivityForResult(new Intent (_this, WorkOrderDetailsActivity.class), GConst.REQUEST_WORK_ORDER_DETAIL);
				}
		  }
		};
	 void UpdateTitles()
	 {
			SetTitle("Work Orders");
			
	 }
	 void request4WOs()
	 {
		 MkrNLib lib=MkrNLib.Get();
		 lib.sendRequest(lib.GetActivities);
	 }
	 
	 public void Response4WOs(int par)
	 {
	 	aList= MkrNLib.Get().GetWOList();
		 if(aList!=null&&aList.getWoItemsCount()>0)
		 {
			 runOnUiThread(new Runnable() {
			        @Override
			        public void run() {
			        	adapter.SetAList(aList);
						adapter.notifyDataSetChanged();
			        }
			    });
			 
		 }
	 }
	 
}
