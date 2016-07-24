package com.mkr.views;

import com.google.protobuf.InvalidProtocolBufferException;
import com.mkr.adapters.InspectionReportAdapter;
import com.mkr.adapters.SeparatedListAdapter;
import com.mkr.models.proto.CommPackets.PInspectionRow;
import com.mkr.models.proto.CommPackets.PInspectionRowList;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.R;
import com.mkr.unidriver.inspection.InspCatFormActivity;
import com.mkr.utils.GConst;

import android.app.Activity;
import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;

public class InspectionReportView extends View{

	ListView irLV=null;
	Activity act;
	Fragment fragment;
	View view;
	SeparatedListAdapter adapter=null;
	int report_type=0;
	public InspectionReportView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}

	public InspectionReportView(Activity act, Fragment fragment,View view) {
		super(act);
		this.act=act;
		this.view=view;
		this.fragment=fragment;
		initV();
		
	}
	void initV()
	{
		irLV =(ListView) this.view.findViewById (R.id.list_inspection_reports);
		irLV.setOnItemClickListener(new AdapterView.OnItemClickListener() {
			  @Override
			  public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

			    PInspectionRow o = (PInspectionRow )irLV.getItemAtPosition(position);
			    if(o!=null)
			    {
			    	//Log.i("MM",o.getDrName()+"--"+o.getInsType());
			    	GConst.g_IR=o;
			    	StartInspectionCatFormWithSlide(0,o.getInsRID());
			    }
			     
			  }
			});
	}
	public void StartInspectionCatFormWithSlide(int is_new,int insRID)
	{
		Intent inte = new Intent (act, InspCatFormActivity.class);
		inte.putExtra (GConst.INSP_FORM_NEW, is_new);
		inte.putExtra (GConst.INSP_FORM_ID,insRID );
		inte.putExtra (GConst.INSP_FORM_TYPE,this.report_type );
		
		act.startActivityForResult(inte, GConst.INSP_FORM_ACTIVITY_RESULT_ID);
		//slide from right to left
	    act.overridePendingTransition(R.anim.pull_in_right,R.anim.push_out_left);
	}
	void PopulateInspectionReportList()
	{
		int bid=GConst.BoxID;
		PInspectionRowList recent_r=MkrNLib.Get().GetInspections(bid,report_type,0);
		PInspectionRowList old_r=MkrNLib.Get().GetInspections(bid,report_type,1);
		boolean f1=recent_r!=null&&recent_r.getListCount()>0;
		boolean f2=old_r!=null && old_r.getListCount()>0;
		
		if(f1||f2)
		{
			adapter = new SeparatedListAdapter(act);
			if(f1)
			{
				InspectionReportAdapter r_adapter = new InspectionReportAdapter (act, R.layout.inspection_report_list_row, recent_r.getListList());
				adapter.AddSection (this.act.getResources().getString(R.string.str_recent_insp), r_adapter);
			}
			if(f2)
			{
				InspectionReportAdapter r_adapter = new InspectionReportAdapter (act, R.layout.inspection_report_list_row, old_r.getListList());
				adapter.AddSection (this.act.getResources().getString(R.string.str_old_insp), r_adapter);
			}
			irLV.setAdapter(adapter);
			adapter.notifyDataSetChanged ();
		}
		
	}
	public void UpdateReportList(int report_type) {
		adapter=null;
		this.report_type=report_type==GConst.REQUEST_PreTripIR?0:1;
		PopulateInspectionReportList();
	 	
	}
	
}
