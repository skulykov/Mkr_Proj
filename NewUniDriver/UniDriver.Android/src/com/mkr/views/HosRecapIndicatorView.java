package com.mkr.views;

import com.mkr.models.RecapSummary;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.hos.HosHelper;
import com.mkr.utils.Util;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.RectF;
import android.graphics.Shader;
import android.view.View;

public class HosRecapIndicatorView extends View{
	String timeStr="00:00";
	String descStr="";

	float endAngle = 0;

	private Paint headerPaint;
	Canvas canvas=null;
	Context context=null;  
	RecapSummary rs=null;
	public HosRecapIndicatorView(Context ctx)
	{
		super(ctx);
	    this.context=ctx;
	    headerPaint = new Paint();
		headerPaint.setAntiAlias(true); 
		//headerPaint.Dither = true;
		headerPaint.setColor(Color.BLACK);
		//headerPaint.SubpixelText = true;
		headerPaint.setTextSize(24f);
		headerPaint.setFakeBoldText(true);
		
		
	}
	public void SetRecapSummary()
	{
		String str=MkrNLib.Get().GetHosRecapSummary();
		rs=HosHelper.parseRecapSummary(str);
	}
	void init (float x0,float y0,float radius,int index)
	{
		float w = radius*2;//100, h = 100;
		switch (index) {
		case 1:
			descStr="Driving";
			{
				int t1 = rs.avail_driving_min;
				if (t1 < 0)
					t1 = 0;
				timeStr = Util.ConvertMinutesToHoursandMinutes (t1);
				int mt=rs.max_driving * 60;    // MaxDriving in hours
				/*int dx = mt - t1;
				if (dx < 0)
					dx = 0;*/
				endAngle=(t1*2.0f * (float)Math.PI)/mt;
			}
			break;
		case 2:
			descStr = "On Duty";
			{

				int t1 = rs.avail_onduty_min;
				if (t1 < 0)
					t1 = 0;
				timeStr = Util.ConvertMinutesToHoursandMinutes (t1);
				int mt=rs.max_onduty * 60;
				//int dx = mt - t1;
				//if (dx < 0)
				//	dx = 0;
				endAngle=(t1*2.0f * (float)Math.PI)/mt;
			}
			break;
		case 3:     // 30 min break;
			descStr = "Break";
			{
				timeStr = "00:30";
				int t1 = 30;
				int mt = 30;
                /*
				if (HOSAgent.is30MinBreak ()) {
					TimeSpan ts = HOSAgent.get30MinRemaining ();
					t1 = ts.Minutes;
					if (t1 < 0)
						t1 = 0;
					if (t1 > mt)
						t1 = mt;
					timeStr = string.Format ("{0}", ts.ToString (@"hh\:mm"));//"11:18";
				
				}
                */
				//int dx = mt - t1;
				//if (dx < 0)
				//	dx = 0;
				endAngle=(t1*2.0f * (float)Math.PI)/mt;
			}
			break;
		case 4:
			descStr = "Cycle";
			{
				int t1 = rs.avail_cycle;
				if (t1 < 0)
					t1 = 0;

				timeStr = Util.ConvertMinutesToHoursandMinutes (t1);
				int mt = rs.MaxCycle;
				endAngle=(t1*2.0f * (float)Math.PI)/mt;
			}
			break;
		}
	}
	void drawMainCircle(float x0,float y0,float radius)
	{

		Path path = new Path ();
		//path.AddArc (x0, y0, radius, 0, 2.0f * (float)Math.PI, true);
		RectF rect=new RectF(x0-radius,y0-radius,x0+radius,y0+radius);
		path.addArc (rect ,0, 360);
        int colors[]=new int[]{Color.GRAY,Color.WHITE,Color.GRAY};
        float positions[]=new float[] {0,  0.5f, 1 };
			LinearGradient lg = new LinearGradient(x0,y0-radius,x0,y0+radius,
				colors,
				positions,
				Shader.TileMode.REPEAT);

		Paint p = new Paint();
		p.setAntiAlias(true); 
		p.setFilterBitmap(true);
		p.setShader(lg);
		if(canvas!=null)canvas.drawPath(path,p);
	}
	void drawStatusCircle(float x0,float y0,float radius)
	{
		
		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.STROKE);
		p1.setColor(Color.BLUE);
		p1.setStrokeWidth(16);
		p1.setAntiAlias(true);

		//2.0f * (float)Math.PI)
		endAngle=(float)(endAngle*360.0/(2.0*Math.PI));  // degree !!!
		Path path = new Path ();
		radius=radius-p1.getStrokeWidth()/2;
		RectF rect=new RectF(x0-radius,y0-radius,x0+radius,y0+radius);
		//path.AddArc (rect, 0, endAngle);
		//canvas.DrawPath (path,p1);
		canvas.drawArc(rect, 0, endAngle,false,p1);
	}
	void drawStatusCircle1(float x0,float y0,float radius)
	{
		
		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.STROKE);
		p1.setColor(Color.RED);
		p1.setStrokeWidth(4);
		p1.setAntiAlias(true);


		Path path = new Path ();
		radius=radius-p1.getStrokeWidth()/2;
		RectF rect=new RectF(x0-radius,y0-radius,x0+radius,y0+radius);
		canvas.drawArc(rect, 0, 360,false,p1);
	}
	void showCircle(float x0,float y0,float radius)
	{
		drawMainCircle(x0,y0,radius);
		drawStatusCircle1(x0,y0,radius+4);
		drawStatusCircle1(x0,y0,radius-16);
		drawStatusCircle (x0, y0,radius);
	}
	void showText (float x, float y, String str, String str1, int ind)
	{
		canvas.drawText (str, x - 30, y - 26+10, headerPaint);
		switch (ind) {
			case 1:
				canvas.drawText (str1, x-30-5, y+26, headerPaint);
				break;
			case 2:
				canvas.drawText (str1, x-30-9, y+26, headerPaint);
			break;
			default:
				canvas.drawText (str1, x-30, y+26, headerPaint);
			break;
		}
	}
	public void _Draw (Canvas canvas,float x0,float y0,float radius,int ind)
	{
		//base.Draw (rect);
		this.canvas=canvas;
		init (x0,y0,radius,ind);
			
			showCircle(x0,y0,radius);
			showText(x0,y0,timeStr,descStr,ind);

			/*
		Paint p1 = new Paint();
		p1.SetStyle(Paint.Style.Stroke);
		p1.Color = Color.Blue;
		p1.StrokeWidth = 10;
		p1.AntiAlias = true;
		canvas.DrawCircle (x0, y0, radius, p1);
		*/
	}
}
