package com.mkr.unidriver.workorders;

import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.mkr.models.proto.CommPackets.PWorkOrderItem;
import com.mkr.unidriver.MainDrawerActivity;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;


public class WorkOrderDetailsActivity extends OnResultBaseActivity{

	PWorkOrderItem woi=null;
	Button sts_btn=null;
	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.jobs_item_details_layout);
	        woi=GConst.wo_item;
	        setEditText(R.id.tName,woi.getName());
			setEditText(R.id.et_tid,Util.int2str(woi.getID()));
			setEditText(R.id.et_caller,woi.getCaller());
			setEditText(R.id.et_ref,woi.getReference());
			String addr = woi.getStreet()+", "+woi.getCity()+", "+woi.getCountry()+", "+woi.getPostalCode();
			setEditText(R.id.et_addr,addr);
			setEditText(R.id.et_instr,woi.getInstructions()+"\r\n"+woi.getInstructions()+"\r\n"+woi.getInstructions());
			
			sts_btn=(Button)findViewById(R.id.job_sts_btn);
			SetMainTitles(/*woi.getName()*/"Work order details",R.layout.custom_ab_help);
	       
			RefreshJobStatus (woi.getStatus()-1);
	 }
	 public void ABBackBtn(View v)
		{
		  finish();
		}
	 public void StsBtn(View v)
	 {
		 ProcessJobOptionsMenu (v);
	 }
	 public void ChatBtn(View v)
	 {
		 InvokeDispatcherChat();
	 }
	 void setEditText(int id,String txt)
	 {
			TextView ed=(TextView)findViewById(id);
			//ed.setKeyListener(null);
			ed.setText(txt);
	 }
	//================processCustomOptionsmenu=========================
			public PWorkOrderItem GetWorkOrderItem(){return woi;}
			public void RefreshJobStatus (int sts)
			{
				if(sts>=0 && sts<5)
				{
				int id=JobStatusPopUp.Sts_Names_Ids[sts];
				String str=(String) this.getResources().getText(id);
				sts_btn.setText ( /*Util.int2str(sts)*/str);
				}
			}
			JobStatusPopUp job_menuqa=null;
			void ProcessJobOptionsMenu (Object sender)
			{

				if (job_menuqa == null)
					job_menuqa = new JobStatusPopUp (this);
				job_menuqa.Show ((View)sender);

			}


			//===================Add WorkOrder Chat=====================
			void InvokeDispatcherChat()
			{
				Fragment frag=MainDrawerActivity.current_fragment;
				frag.startActivityForResult(new Intent (this, WODetailsChatActivity.class), GConst.REQUEST_WORK_ORDER_CHAT);
			}
}
