package com.mkr.unidriver.hos.dailylogs;

import java.util.Date;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.service.MkrApp;
import com.mkr.unidriver.MainDrawerActivity;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.HosHelper;
import com.mkr.utils.DateUtil;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;
import com.mkr.views.EditHosGraphView;
import com.mkr.views.HosEditStatusPU;
import com.mkr.views.HosGraphView;
import com.mkr.views.HosMenuQA;
import com.mkr.views.HosStatusCustomView;

public class HosEventEditActivity extends OnResultBaseActivity 
{
	Button sts_btn=null;
	Date currentGraphDate=null;
	int timeLogId=-1;
	EditHosGraphView hgv;
	EditText txtStartTime=null,txtComments=null;
	int is_new=1,selectedSts=101;
	 @Override
	    public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.hos_event_edit_layout);
	        Intent intent = getIntent();
	        String _str=intent.getStringExtra (GConst.EXTRA_HOS_GRAPH_DATE);
	        currentGraphDate = DateUtil.parseDate(_str);
	        _str=intent.getStringExtra (GConst.EXTRA_HOS_EVENT_EDIT_ID);
			timeLogId = Integer.parseInt(_str);	
	       
			LinearLayout v1 = (LinearLayout)findViewById (R.id.hos_graph_view_id);
			hgv = new EditHosGraphView (this,currentGraphDate,timeLogId);
			v1.addView (hgv);
			
			String title="Add new event";		
			if(timeLogId!=-1)
			{
				title="Edit event ";
				String title1=GConst.selected_tlr==null?"":""+GConst.selected_tlr.getTLID();
				title=title+title1;
				is_new=0;
			}
			SetMainTitles(title,R.layout.action_bar_title);
			
			txtComments = (EditText)findViewById (R.id.txt_comments_id);
			txtStartTime = (EditText)findViewById (R.id.txt_hea_start_time_id);
			
			sts_btn=(Button)findViewById(R.id.hea_sts_id);
			if(is_new==1)
			{
				updateStsBtn(0);
			}
			else{
				PTimeLogRow tlr=hgv.getEditTLR();
				if(tlr!=null)
					updateStsBtn(tlr.getEvent()-101);
			}
	 }
	 @Override
	 public void onResume()
	 {
	   super.onResume();
	   UpdateSelectedTime(hgv.UpdateTS(0));
	 }
	 public void LessBtn(View v)
	 {
		 UpdateSelectedTime(hgv.UpdateTS(-15));
	 }
	 public void MoreBtn(View v)
	 {
		 UpdateSelectedTime(hgv.UpdateTS(15));
	 }
	 public void UpdateSelectedTime(String str)
	{
		 txtStartTime.setText(str);
		 txtStartTime.invalidate();
	}
	 public void CancelBtn(View v)
	 {
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 public void SaveBtn(View v)
	 {
		 PTimeLogRow tlr=hgv.getEditTLR();
		 if(tlr==null) return;
		 String s1=txtComments.getText().toString();
		 s1=tlr.getComment()+"|"+s1;
		 
		if(is_new==1)
		{
			int rv=HosHelper.CreateDriverStatus(selectedSts-101,-1,s1,0,1,tlr.getLogTime());
		}
		else{
			int rv=HosHelper.CreateDriverStatus(selectedSts-101,tlr.getTLID(),s1,2,1,tlr.getLogTime());
		}
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 HosEditStatusPU sts_menuqa=null;
	 public	void StatusBtnClick (View sender)
	{
			
				if (sts_menuqa == null)
					sts_menuqa = new HosEditStatusPU (this);
				sts_menuqa.Show ((View)sender);
			
	}
	 void updateStsBtn(int ind)
	 {
		 selectedSts=101+ind;
		 String str=HosHelper.GetEventStr(selectedSts);
			if (sts_btn != null) {
				sts_btn.setText(str);
				sts_btn.invalidate();
			}
	 }
	public void selectDriverStatus(int ind)
	{
			
			/*int rv=HosHelper.CreateDriverStatus(ind);
			if(rv>0)
			{
				 Log.d("HHH","rv="+Integer.toString(rv));
				 if (sts_btn != null) {
						sts_btn.setText("GGGG");
				}
				
			}*/
			updateStsBtn(ind);
	}
}
