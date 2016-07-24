package com.mkr.views;

import java.util.Date;
import java.util.List;

import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.hos.HosActivity;
import com.mkr.utils.DateUtil;
import com.mkr.utils.Util;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.view.View;

public class HosGraphView extends View 
{
	Context ctx;
	final String TAG="HGV";
	//added recently
	public int selectedTimeLogIndex=-1;
	public Date plottingDate=new Date();

	/*public*/ List<PTimeLogRow> timeLogList=null;
    public float _gridunit=0;
	private int LeftOffset = 24;//48;
	private int RightOffset = 30;//64;
	private int TopOffset = 30;//48;
	private int BottomOffset = 2;
	private float gh = 0;
	private float gw = 0;
	private float gLeft = 0;
	private float gTop = 0;
	private float segmentHeight = 0;
	private int mWidth = 500;
	private int mHeight = 350;
	private int dutyStatusCount = 4;
	private Paint gridPaint;
	private Paint headerPaint;
	private Paint barPaint/*,vertPaint*/;
	//private Paint highlightPaint;
	private String[] header = null;
	private String[] gutter = null;

	float screenX ,screenY,offsetX;
	
	public HosGraphView(Context context)
	{
		 super(context);
		ctx=context;
		initialize ();
	}
	void initialize()
	{
		header=new String[] { " M", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", " N", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11" };
		gutter=new String[] { "OF", "SB", "D ", "ON" };

		gridPaint = new Paint();
		gridPaint.setAntiAlias( true); 
		gridPaint.setColor(Color.BLACK);
		gridPaint.setStyle(Paint.Style.STROKE);
		gridPaint.setStrokeWidth(1);

		headerPaint = new Paint();
		headerPaint.setAntiAlias( true); 
		//headerPaint.Dither = true;
		headerPaint.setColor(Color.BLACK);
		//headerPaint.SubpixelText = true;
		headerPaint.setTextSize ( 12f);
		headerPaint.setFakeBoldText(true);

		barPaint = new Paint();
		barPaint.setAntiAlias( true);
		barPaint.setColor(Color.BLUE);
		barPaint.setStyle(Paint.Style.STROKE);
		barPaint.setStrokeWidth(4);
		
		dutyStatusCount = 4;
		segmentHeight = this.gh/this.dutyStatusCount;
	}
	void onSizeChanged(int w, int h) {
		
		mWidth = w;
		mHeight = h;
		gh = (float)(mHeight - (TopOffset+BottomOffset));
		gw = (float)(mWidth- (LeftOffset+RightOffset));
		segmentHeight = gh/dutyStatusCount;
	}
	 @Override
	    protected void onSizeChanged(int w, int h, int wOld, int hOld)
	    {
	        super.onSizeChanged(w, h, wOld, hOld);
	        onSizeChanged(w,h);
	    }
     public List<PTimeLogRow> getList()
     {
       return timeLogList;
     }
	 public void updateDrawArray( Date date,boolean flag) {
			try {
			    plottingDate=date;
			    
				timeLogList=  MkrNLib.Get().GetTLRList(date.getTime(),0,1);//TimeLogDB.Get().GetAllForDate(date);
				  this.invalidate();
			}
			catch(Exception e) {
				//Console.WriteLine ( "HOSGraphExc:" + e.Message);
			}
		}
		public void updateDrawArray(Date date,List<PTimeLogRow> list,boolean flag) {
			try {
				plottingDate=date;
			   	timeLogList= list;
				if(flag)
				  this.invalidate();
			}
			catch(Exception e) {
				//Console.WriteLine ( "HOSGraphExc:" + e.Message);
			}
		}
		public List<PTimeLogRow>  GetTimeLogList(){return timeLogList;}
		public void initOnDraw()
		{
			gw = (float)(mWidth - (LeftOffset+RightOffset));
			gh = (float)(mHeight- (TopOffset+BottomOffset));
			gLeft = /*(float)rect.Left +*/ LeftOffset;
			gTop = /*(float)rect.Top +*/ TopOffset;
			segmentHeight = gh/dutyStatusCount;
		}
		 @Override
		    protected void onDraw(Canvas canvas) {
				initOnDraw();
				DrawGridBackground (canvas,null);
				/*
				List<TimeLogRow> prepLogData = null;
				
				if(timeLogList==null||timeLogList.Count==0)
					prepLogData=HosEvents.TlrListForTheDayWithNoHosEvents (plottingDate);
				else
					prepLogData=HosEvents.PrepareLogData (timeLogList,false);*/
				DrawLog (timeLogList/*prepLogData*/, canvas);
				DrawTotalHours (canvas);
		 }
		
		 public void DrawGridBackground(Canvas canvas,PTimeLogRow tlr)
			{
				// paint the interior of the Box
				for (int i = 0; i <= 4; i++)
				{
					canvas.drawLine (gLeft, gTop + (float)(i * segmentHeight), gLeft+gw, gTop + (float)(i * segmentHeight), gridPaint);
				}
				if(tlr!=null)
				{
				   //darw background shadow during tlr edit
					
					float gridunit = gw/1440,x1=gLeft,x2,x3=gLeft+gw;
					
					x2 = gLeft+(Util.GetMinutesFromMidNight(tlr.getLogTime())* gridunit);
					
						drawBkgndRect(canvas,(int)x1, (int)x2, Color.LTGRAY);
						drawBkgndRect(canvas,(int)x2, (int)x3, Color.GRAY);
					
				}
				// draw the tick lines
				float tickXdelta = gw / 96;
				for (int i = 0; i <= 96; i++)
				{
					float tickx = gLeft + (i * tickXdelta);
					float tickheight = segmentHeight / 4;
					float delta = 0;
					if (i % 2 == 0) 
						tickheight = segmentHeight / 2;
					if (i % 4 == 0) {
						tickheight = segmentHeight;
						delta = 4;
						// draw header text
						int j = i / 4;
						if(j<header.length)
							canvas.drawText (header [j], tickx-LeftOffset/3, gTop-TopOffset/3, headerPaint);
					}

					canvas.drawLine (tickx, gTop-delta, tickx, gTop + tickheight, gridPaint);
					canvas.drawLine (tickx, gTop+segmentHeight, tickx, gTop + segmentHeight + tickheight, gridPaint);
					canvas.drawLine (tickx, gTop+segmentHeight*3, tickx, gTop + segmentHeight*3 - tickheight, gridPaint);
					canvas.drawLine (tickx, gTop+segmentHeight*4+delta, tickx, gTop + segmentHeight*4 - tickheight, gridPaint);
				}

				// draw lables
				canvas.drawText (gutter [0], gLeft-LeftOffset+2, gTop+BottomOffset + segmentHeight / 2, headerPaint);
				canvas.drawText (gutter [1], gLeft-LeftOffset+2, gTop+BottomOffset + segmentHeight*3 / 2, headerPaint);
				canvas.drawText (gutter [2], gLeft-LeftOffset+2, gTop+BottomOffset + segmentHeight*5 / 2, headerPaint);
				canvas.drawText (gutter [3], gLeft-LeftOffset+2, gTop+BottomOffset + segmentHeight*7 / 2, headerPaint);
				canvas.drawText (" TOT", gw+LeftOffset, gTop-TopOffset/3, headerPaint);
			}
		 long totaloff = 0,totalon=0,totaldrv=0,totalslp=0;
		 public void initTotalHours()
		 {
			 totaloff = 0;
			 totalon=0;
			 totaldrv=0;
			 totalslp=0;
		 }
		 long get_extend_current_date(long ts)
		 {
			 long extend_current_date=0;
			 Date d=new Date();
				long cur=d.getTime();
				if(cur>ts)
				{
					Date cur_dd=DateUtil.GetDateDate(d);
					Date ts_dd=DateUtil.GetDateDate(new Date(ts));
					if(cur_dd.getTime()==ts_dd.getTime())
						extend_current_date=cur;
					else{
					  ts_dd=DateUtil.addDays(ts_dd, 1);
					  extend_current_date=ts_dd.getTime();
					}
				}
				return extend_current_date; 
		 }
		 public void DrawLog(List<PTimeLogRow> logdata, Canvas canvas)
			{
				if (logdata == null || logdata.isEmpty()) 
					return;
				initTotalHours();
				//grid pixel size per minute gWidth/(24*60)
				float gridunit = gw/1440;
				_gridunit = 1440/gw;

				float x1, x2, y1, y2;
				PTimeLogRow tlr0=logdata.get(0);
				x1 = gLeft+(Util.GetMinutesFromMidNight(tlr0.getLogTime())* gridunit);
				y1 = gTop + (tlr0.getEvent()-101) * segmentHeight + segmentHeight / 2;
                int size=logdata.size();
                //boolean extend_current_date=false;
                long ts=0,ts0=0,extend_current_date=0;
                if(size==1)
                {
                	extend_current_date=get_extend_current_date(tlr0.getLogTime());
                }
                else
				for (int i = 1; i < size; i++)
				{
					PTimeLogRow tlr=logdata.get(i);
					long TotalMinutes = 0;
					ts=tlr.getLogTime();
					ts0=tlr0.getLogTime();
					if(i==(size-1))
					{
						extend_current_date=get_extend_current_date(ts);
					}
					TotalMinutes=Util.DiffInMin(ts,ts0 );
					UpdateTotalHours(ts-ts0,tlr0.getEvent());

					x2 = x1 + (float)(TotalMinutes * gridunit);
					y2 = gTop + (tlr.getEvent()-101) * segmentHeight + segmentHeight / 2;

					//horizantal log line
					barPaint.setStrokeWidth(4);
					canvas.drawLine(x1, y1, x2, y1, barPaint);
					//vertical log line
					barPaint.setStrokeWidth(2);
					canvas.drawLine(x2, y1, x2, y2, barPaint);
					/*
					if (edit_flag&&i==1) 
					{
						DrawDriverStatus.drawEdit (g,(int)x2, (int)y1, 16, UIColor.Red);
					}*/
					x1 = x2;
					y1 = y2;
					tlr0=tlr;
				}
				//last correction
				if(extend_current_date>0)
				{
					ts0=tlr0.getLogTime();
					long TotalMinutes=Util.DiffInMin(extend_current_date, ts0);
					UpdateTotalHours(extend_current_date-ts0,tlr0.getEvent());
					x2 = x1 + (float)(TotalMinutes * gridunit);
					y2 = gTop + (tlr0.getEvent()-101) * segmentHeight + segmentHeight / 2;
					//horizantal log line
					barPaint.setStrokeWidth(4);
					canvas.drawLine(x1, y1, x2, y1, barPaint);
				}
			}
		 public void UpdateTotalHours(long ts,int ev)
			{
					switch (ev)
					{
					case 101/*LOGSTATUS.OffDuty*/:
						totaloff +=ts;
						break;
					case 104/*LOGSTATUS.OnDuty*/:
						totalon +=ts;
						break;
					case 103/*LOGSTATUS.Driving*/:
						totaldrv += ts;
						break;
					case 102/*LOGSTATUS.Sleeping*/:
						totalslp += ts;
						break;
					}
			}
		 public void DrawTotalHours( Canvas canvas)
			{
				
				float x = gw + LeftOffset + 1;
				canvas.drawText (toStr(totaloff), x, gTop + BottomOffset + segmentHeight / 2, headerPaint);
				canvas.drawText (toStr(totalslp), x, gTop + BottomOffset + segmentHeight * 3 / 2, headerPaint);
				canvas.drawText (toStr(totaldrv), x, gTop + BottomOffset + segmentHeight * 5 / 2, headerPaint);
				canvas.drawText (toStr(totalon), x, gTop + BottomOffset + segmentHeight * 7 / 2, headerPaint);
			}
			String toStr(long ts)
			{
				int m = (int)(ts/60000);
			//	int h=(int)(ts/(3600*1000));
			//	int sec=(int)(ts/1000);
				if (/*ts.TotalDays == 1  &&*/ m == 1440) {
					return "24:00";
				}


				/*if (sec >= 30) {
					//m++;
					return Util.ConvertMinutesToHoursandMinutes(m);
				}*/
				return Util.ConvertMinutesToHoursandMinutes(m);
			}
			

			//******************************************************************************
			public void drawHosEdit (Canvas canvas, int x1, int c)
			{
				Paint p1 = new Paint();
				p1.setStyle(Paint.Style.FILL);
				p1.setColor( c);
				p1.setAntiAlias(true);
				int radius=20;
				Rect r = new Rect ();
				r.left = x1-2;//(int)gLeft;
				r.top = (int)gTop+radius;
				r.right = x1+2;
				r.bottom =(int) (gTop+gh)-radius;
				canvas.drawRect(r,p1);

				float x=x1,y=gTop;
				Path path = new Path ();
				path.moveTo (x,y);
				path.lineTo (x-radius/2,y+radius);
				path.lineTo (x+radius/2,y+radius);
				path.lineTo (x,y);

				path.close();
				canvas.drawPath (path,p1);

				y=(gTop+gh);
				path = new Path ();
				path.moveTo (x,y);
				path.lineTo (x-radius/2,y-radius);
				path.lineTo (x+radius/2,y-radius);
				path.lineTo (x,y);

				path.close();
				canvas.drawPath (path,p1);

			}
			
			public void drawBkgndRect(Canvas canvas, int x1,int x2, int c)
			{
				Paint p1 = new Paint();
				p1.setStyle(Paint.Style.FILL);
				p1.setColor(c);
				p1.setAntiAlias(true);

				Rect r = new Rect ();
				r.left = x1;//(int)gLeft;
				r.top = (int)gTop;
				r.right = x2;
				r.bottom =(int) (gTop+gh);
				canvas.drawRect(r,p1);
			}

			
			//--------------------------------------------------------
			 public void DrawLog4Edit(PTimeLogRow tlr, Canvas canvas)
				{
					//grid pixel size per minute gWidth/(24*60)
					float gridunit = gw/1440;
					
					float x1;
					
					x1 = gLeft+(Util.GetMinutesFromMidNight(tlr.getLogTime())* gridunit);
					
		       
						//horizantal log line
						barPaint.setStrokeWidth(4);
						//canvas.drawLine(x1, y1, x2, y1, barPaint);
						
						drawHosEdit (canvas,(int)x1, Color.RED);
					
				}
		 
}
