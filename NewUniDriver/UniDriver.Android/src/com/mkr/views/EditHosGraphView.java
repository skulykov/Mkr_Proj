package com.mkr.views;

import java.util.Date;
import java.util.List;

import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.models.proto.CommPackets.PWorkOrderItem.Builder;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.hos.dailylogs.HosEventEditActivity;
import com.mkr.utils.DateUtil;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

public class EditHosGraphView extends HosGraphView{

	float screenX ,screenY,offsetX;
	HosEventEditActivity act=null;
	PTimeLogRow tlr=null;
	List<PTimeLogRow> tlrList=null;
	Date date;
	long prev_ts=0,next_ts=0;
	int TLID=-1;
	public EditHosGraphView(Context context,Date _date,int _TLID) {
		super(context);
		act=(HosEventEditActivity)context;
		date=_date;
		this.TLID=_TLID;
		updateDrawArray (_date,true);
		this.setOnTouchListener(new OnTouchListener() {
		       
			@Override
			public boolean onTouch(View v, MotionEvent e) {
				return _onTouch(e);
				
			}
	    });
		if(TLID==-1)
		{
			tlrList =  MkrNLib.Get().GetTLRList(date.getTime(),0,2);  // 2 - new event at noon
		
			if(tlrList!=null&&tlrList.size()==1)
			{
				tlr=tlrList.get(0);
				
			}
		}
		else{
			tlrList=getList();
			if(tlrList!=null&&tlrList.size()>0)
			{
				for(PTimeLogRow tl : tlrList)
				{
					if(tl.getTLID()==TLID)
					{
						tlr=tl;
						break;
					}
				}
				
			}
		}
		if(tlr!=null)
		{
			prev_ts=tlr.getLogTime();
			next_ts=prev_ts+12*60*60000;
			prev_ts-=12*60*60000;
			
			//next_ts-=60000; // 1 minute correction
			//prev_ts+=60000;
		}
	}
	public PTimeLogRow getEditTLR()
	{
		return tlr;
	}
	@Override
    protected void onDraw(Canvas canvas) {
		initOnDraw();
		DrawGridBackground (canvas,tlr);
		/*
		List<TimeLogRow> prepLogData = null;
		
		if(timeLogList==null||timeLogList.Count==0)
			prepLogData=HosEvents.TlrListForTheDayWithNoHosEvents (plottingDate);
		else
			prepLogData=HosEvents.PrepareLogData (timeLogList,false);*/
		DrawLog (timeLogList/*prepLogData*/, canvas);
		DrawTotalHours ( canvas);
		
		if(tlr!=null)
			DrawLog4Edit(tlr, canvas);
	}
	
	public String UpdateTS(float v)
	{
		String currentTimeStr="";
	try{
        
			long ts=tlr.getLogTime();
			long off=(long)(v*60000);
			ts+=off;
	       if(v<0)
	       {
				if(ts<prev_ts)
					ts=prev_ts;
			}
			else{
				if(ts>next_ts)
					ts=next_ts;
			}

	       	com.mkr.models.proto.CommPackets.PTimeLogRow.Builder bldr=tlr.toBuilder(); 
			 bldr.setLogTime(ts);
			 tlr=bldr.build();
			 
			currentTimeStr=DateUtil.getTimeStamp (this.tlr.getLogTime());
			this.invalidate ();
			
		}
		catch(Exception ex){}
		return currentTimeStr;
	}
	 boolean _onTouch( MotionEvent e)
	 {
		
			float x = e.getX();
			float y = e.getY();

			switch (e.getAction())
			{
				case MotionEvent.ACTION_DOWN://MotionEventActions.Down:
					screenX = x;
					screenY = y;
					offsetX = 0;
				break;
				case MotionEvent.ACTION_MOVE:
					
					offsetX -= (screenX - x);
					if (Math.abs (offsetX) > 2) {
						//if (this.parent != null)
						//	currentTimeStr=this.parent.UpdateTS (offsetX*_gridunit);
						//lock (thisLock) {
							act.UpdateSelectedTime(UpdateTS (offsetX * _gridunit));
							//v.Invalidate ();
							offsetX = 0;
						//}
					}
					screenX = x;
					screenY = y;
				break;
				case MotionEvent.ACTION_UP:
				
				break;
			}
			return true;
	 }
}
