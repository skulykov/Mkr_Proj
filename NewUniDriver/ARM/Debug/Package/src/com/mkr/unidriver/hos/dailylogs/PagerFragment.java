package com.mkr.unidriver.hos.dailylogs;

import java.util.Date;
import java.util.List;

import com.mkr.adapters.HosEventAdapter;
import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.HosActivity;
import com.mkr.utils.DateUtil;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;
import com.mkr.views.HosGraphView;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

public class PagerFragment extends Fragment 
{
    
	int pageNumber=0;
    View view=null;
    HosDailyLogsPagerActivity ctx =null;
    
	public PagerFragment(int position) {
		this.pageNumber=position;
	}
     @Override 
	  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
      {
			view = inflater.inflate(R.layout.hos_daily_logs_fragment_layout, container, false);
			ctx = (HosDailyLogsPagerActivity) MkrApp.GetCurrentOnTopActivity () ;
			if(ctx!=null)
			{
				processView ();
			}
          return view;
      }
   //===============Helper Functions=================================
   		HosGraphView hgv=null;
   		ListView listHosEvents=null;
   		private Date currentGraphDate = Util.getUtcDate();
   		private ImageButton btnRowEdit = null;
   //		private ImageButton btnSign,btnAddHosEvent, btnCancel;
   		TextView lblHosDate;
   		//ImageButton btnPrev=null,btnNext=null;
   		void processView ()
   		{
   			currentGraphDate=DateUtil.addDays(currentGraphDate,0-pageNumber);
   			lblHosDate = (TextView) view.findViewById (R.id.lbl_hos_insp_date);
   			lblHosDate.setText(DateUtil.GetLongTimeStr(currentGraphDate));//currentGraphDate.DayOfWeek.ToString()+" "+currentGraphDate.Month+"/"+currentGraphDate.Day;
   			TextView lblHosCycle = (TextView)view.findViewById (R.id.lbl_hos_insp_cycle);
   			lblHosCycle.setText(MkrNLib.Get().GetStringFromCurrentHosCycle () );


   			LinearLayout v1 = (LinearLayout)view.findViewById (R.id.hos_insp_graph_view);
   			hgv = new HosGraphView (ctx);
   			v1.addView (hgv);

   			listHosEvents =(ListView) view.findViewById (R.id.list_hos_insp_events);
   			
   			   			
   			List<PTimeLogRow> timeLogList=  MkrNLib.Get().GetTLRList(currentGraphDate.getTime(),0,1);//TimeLogDB.Get().GetAllForDate(date);
			this.hgv.updateDrawArray (currentGraphDate,timeLogList,true);
   			
			
   			listHosEvents.setAdapter(new HosEventAdapter (ctx, R.layout.hos_event_list_row, timeLogList));
   			listHosEvents.setOnItemClickListener(adapter_listener);

   		}
   		public HosGraphView getHgv()
   		{
   		 return hgv;
   		}
   	 OnClickListener btn_listener=new OnClickListener(){

			@Override
			public void onClick(View v) {
				if(v==btnRowEdit) 
					BtnRowEditClicked();
			}
			
		};
   		void AddHosEvent()
   		{
   				//Start activity to edit this HOS TimeLogRow
   			Intent serverIntent = new Intent(ctx, HosEventEditActivity.class);
   			serverIntent.putExtra (GConst.EXTRA_HOS_EVENT_EDIT_ID, "-1");
   			serverIntent.putExtra (GConst.EXTRA_HOS_GRAPH_DATE, DateUtil.GetDateStr(currentGraphDate));
			startActivityForResult(serverIntent, GConst.REQUEST_HOS_EVENT_EDIT);
   		}

   		
   		OnItemClickListener adapter_listener=new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position,
					long id) {
				processItemClick(view ,position);
				GConst.selected_tlr = (PTimeLogRow ) listHosEvents.getItemAtPosition(position);//   getSelectedItem();
			}

            
        };
   int selectedTLID=-1;
   	void processItemClick(View v,int pos)
   	{

   		//Set selected row
   		listHosEvents.setSelection(pos);
   		//selectedPos=pos;
   		//Highlight the selected TimeLog region on graph
   		//hgv.selectedTimeLogIndex = e.Position;
   		hgv.invalidate();

   		//Hide the previous selected row's edit button if we had set it to Visible here; this could have Invisible ViewState due to TimeLogType.Auto
   		if(btnRowEdit != null /*&& btnRowEdit.getVisibility() == View.VISIBLE*/){
   			btnRowEdit.setVisibility(View.GONE);
   		}

   //		currentGraphDate = Util.parseStrDate(Util.ToShortDateString(currentGraphDate)+" "+((TextView) view.findViewById (R.id.txt_her_time)).getText());
   		List<PTimeLogRow> list=this.hgv.GetTimeLogList();
   		if(list!=null&&pos<list.size())
   		{
   			PTimeLogRow tlr=list.get(pos);
   			if(tlr!=null)
   			{
   				currentGraphDate =	new Date(tlr.getLogTime());
   				selectedTLID=tlr.getTLID();
   			}
   		}
   		
   		btnRowEdit =(ImageButton) v.findViewById (R.id.btn_her_edit);
   		if(btnRowEdit != null /*&& btnRowEdit.getVisibility() != View.INVISIBLE*/){
   			btnRowEdit.setVisibility(View.VISIBLE);
   			
   			btnRowEdit.setOnClickListener(btn_listener);
   			
   		}

   	}//processItemClick
   	
   	void BtnRowEditClicked()
   	{ //     int pos=listHosEvents.getSelectedItemPosition();// always return -1
   			//int pos=selectedPos;
   			Intent serverIntent = new Intent(ctx, HosEventEditActivity.class);
			serverIntent.putExtra (GConst.EXTRA_HOS_GRAPH_DATE, DateUtil.GetDateStr(currentGraphDate));
			serverIntent.putExtra (GConst.EXTRA_HOS_EVENT_EDIT_ID, Integer.toString(/*pos*/selectedTLID));

			startActivityForResult(serverIntent, GConst.REQUEST_HOS_EVENT_EDIT);
		
   	}

   	//------------------
   
        @Override
   		public  void onActivityResult (int requestCode, int resultCode, Intent data)
   		{
   			super.onActivityResult (requestCode, resultCode, data);
   			Log.d ("HDL", "onActivityResult " + resultCode);

   			switch(requestCode)
   			{

   			case GConst.REQUEST_HOS_EVENT_EDIT:
   				// When HosEventEditActivity returns with Android.App.Result.Ok
   				if( resultCode == GConst.G_RESULT_OK){
   					//If Edited TimeLog is deleted then unselect the graph
   					if (data != null && data.getExtras().getString (GConst.EXTRA_HOS_EVENT_EDIT_ID).equals ("-1"))
   						hgv.selectedTimeLogIndex = -1;

   					hgv.updateDrawArray(currentGraphDate,true);
   					HosEventAdapter adapter=new HosEventAdapter (ctx, R.layout.hos_event_list_row, hgv.GetTimeLogList());
   					listHosEvents.setAdapter(adapter); 

   					//We might have added a new TimeLog event let's check and view sign button
   					ctx.SetSignBtnVisibility ();
   				}
   				break;
   			case GConst.REQUEST_SIGNATURE_ADD:
   				// When MyProfileActivity returns with Android.App.Result.Ok check for signature and sign LogSheet
   				if( resultCode == GConst.G_RESULT_OK){
   					/*
   					if(GlobalInstance.currentEmployee.Signature != null && GlobalInstance.currentEmployee.Signature.Length > 1){
   						foreach(TimeLogRow TLrow in hgv.timeLogList)
   						{
   							if (!TLrow.Signed) {
   								TLrow.Signed = true;
   								TLrow.haveSend = false;
   								TimeLogDB.Get().SaveTimeLog (TLrow);
   							}
   						}
   						Global.runTimerCallBackNow();
   						//Check to see if all timelogs are signed hide signBtn
   						SetSignBtnVisibility ();
   					}*/
   				}
   				break;

   			}

   		}
       
}
