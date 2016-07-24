package com.mkr.views;

import com.mkr.unidriver.hos.HosActivity;
import com.mkr.utils.Screen;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.Shader;
import android.graphics.Typeface;
import android.view.MotionEvent;
import android.view.View;
import android.widget.LinearLayout;

public class HosStatusCustomView extends View
{
	Context ctx;
	LinearLayout ll;
	float _viewX;
	int mEvt=0;
	int driverState=101;
	public HosStatusCustomView(Context context,LinearLayout v)
	{
		   super(context);
		   ctx=context;
		   ll=v;
		   setOnTouchListener(new View.OnTouchListener()
		   {
				@Override
				public boolean onTouch(View v, MotionEvent e) {
					switch (e.getAction())
					{
					case MotionEvent.ACTION_DOWN:
						_viewX = e.getX ();
						mEvt = 1;
						v.invalidate ();
						break;
					case MotionEvent.ACTION_MOVE:
						int left = (int) (e.getRawX() - _viewX);
						int right = (int) (left + v.getWidth());
						//v.Layout(left, v.Top, right, v.Bottom);
						break;
					case MotionEvent.ACTION_UP:
						mEvt = 0;
						v.invalidate ();
						HosActivity act = (HosActivity)ctx;
						act.createStatusDialog (v);
						break;
					}
					return true;
				}
			});
	}
	
	public void refreshDriverStatusGui(int state)
	{
		driverState = state;
		this.invalidate ();
	}
	
	//=================================================================
	void _drawText(String str,int x,int y,Canvas canvas,int c,int size,boolean bold)
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		size = Screen.toPx (ctx,size);

		Paint p2 = new Paint(); 
		p2.setColor(c); 
		p2.setTextSize(size); 
		p2.setAntiAlias(true);
		if(bold)
			p2.setTypeface(Typeface.DEFAULT_BOLD);


		Rect rect = new Rect();
		p2.getTextBounds(str, 0, 1, rect);

		//buffer.drawText(this.myText, canvasWidht >> 1, (canvasHeight + bounds.height()) >> 1, paint);
		canvas.drawText(str, x, y, p2); 
	}
	void triagle(Canvas canvas,int x, int y, int w,int h,boolean up)
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		w = Screen.toPx (ctx,w);
		h = Screen.toPx (ctx,h);

		Paint paint = new Paint();
		paint.setColor(Color.BLACK);
		paint.setAntiAlias( true ); 
		paint.setStyle(Paint.Style.FILL_AND_STROKE);

		Path path = new Path();
		path.setFillType(Path.FillType.EVEN_ODD);
		path.moveTo(x,y);
		if (up) {
			path.lineTo (x - w / 2, y + h);
			path.lineTo (x + w / 2, y + h);
		} else {
			path.lineTo (x - w / 2, y - h);
			path.lineTo (x + w / 2, y - h);
		}
		path.lineTo(x,y);
		path.close();

		canvas.drawPath(path, paint);


	}
	void drawCircle (Canvas canvas, int x, int y, int radius,int strokeW, int c)
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		radius = Screen.toPx (ctx,radius);
		strokeW = Screen.toPx (ctx,strokeW);


		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.STROKE);
		p1.setColor(c);
		p1.setStrokeWidth(strokeW);
		p1.setAntiAlias(true);
		canvas.drawCircle (x, y, radius, p1);
	}
	void drawSemiCircleGrad (Canvas canvas, int x, int y, int radius,int strokeW,boolean pressed)
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		radius = Screen.toPx (ctx,radius);
		strokeW = Screen.toPx (ctx,strokeW);


		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.FILL);
		//p1.Color = c;
		p1.setStrokeWidth(strokeW);
		p1.setAntiAlias(true);
		//canvas.DrawCircle (x, y, radius, p1);
		float zz=radius+strokeW;
		RectF rect=new RectF(strokeW,-zz,2*zz,zz);

		//SweepGradient gradient1 = new SweepGradient(100, 100, Color.Red, Color.Blue);
		/*
		RadialGradient gradient1 = new RadialGradient(zz, zz, zz, 
			new int[] {Color.Red.ToArgb(),Color.Blue.ToArgb(),Color.Red.ToArgb()},
        new float[] {0.0f, 0.4f, 0.8f}, 
        Android.Graphics.Shader.TileMode.Clamp); 
        */
		int c1=Color.parseColor("#FFCCDDCC");
		int c2=Color.parseColor("#FFCCEECC");
		int c0=Color.LTGRAY;
		if(pressed)
		{
			c2=Color.parseColor("#FFFF55FF");
			c1=Color.parseColor("#FFFF33FF");
			c0=Color.MAGENTA;
		}
		int colors[]={c0,c1,c2,c1,c0};
		LinearGradient gradient1=new LinearGradient(zz/2, 0+strokeW, zz/2,zz-strokeW,
			/*Color.LTGRAY,c1,*/colors,null,
            Shader.TileMode.CLAMP); 
		p1.setShader(gradient1);



		canvas.drawArc(rect,0,180,true,p1);
	}
	void drawSemiCircle (Canvas canvas, int x, int y, int radius,int strokeW, boolean pressed )
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		radius = Screen.toPx (ctx,radius);
		strokeW = Screen.toPx (ctx,strokeW);


		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.STROKE);
		p1.setColor(/*pressed?Color.MAGENTA:*/Color.BLUE);
		p1.setStrokeWidth(strokeW);
		p1.setAntiAlias(true);
		//canvas.DrawCircle (x, y, radius, p1);
		float zz=radius+strokeW;
		RectF rect=new RectF(strokeW,-zz,2*zz,zz);
				
		canvas.drawArc(rect,0,180,true,p1);
	}
	void drawOffDuty (Canvas canvas, int x, int y, int radius,int strokeW, int c)
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		radius = Screen.toPx (ctx,radius);
		strokeW = Screen.toPx (ctx,strokeW);

		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.STROKE);
		p1.setColor(c);
		p1.setStrokeWidth(strokeW);
		p1.setAntiAlias(true);

		canvas.drawLine (x - radius, y - radius, x + radius, y + radius,p1);
		canvas.drawLine (x - radius, y + radius, x + radius, y - radius,p1);
			//canvas.DrawCircle (x, y, radius, p1);
	}
	void drawSleeping (Canvas canvas, int x, int y, int radius,int strokeW, int c)
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		radius = Screen.toPx (ctx,radius);
		strokeW = Screen.toPx (ctx,strokeW);

		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.STROKE);
		p1.setColor(c);
		p1.setStrokeWidth(strokeW);
		p1.setAntiAlias(true);

		Rect r = new Rect ();
		r.left = x - radius;
		r.top = y - radius;
		r.right = x + radius;
		r.bottom = y + radius;
		canvas.drawRect(r,p1);

		//canvas.DrawCircle (x, y, radius, p1);
	}
	void drawDriving (Canvas canvas, int x, int y, int radius,int strokeW, int c)
	{
		x = Screen.toPx (ctx,x);
		y = Screen.toPx (ctx,y);
		radius = Screen.toPx (ctx,radius);
		strokeW = Screen.toPx (ctx,strokeW);

		Paint p1 = new Paint();
		p1.setStyle(Paint.Style.STROKE);
		p1.setColor(c);
		p1.setStrokeWidth(strokeW);
		p1.setAntiAlias( true);

		Path path = new Path();
		path.setFillType(Path.FillType.EVEN_ODD);
		path.moveTo(x-radius/2,y-radius);
		path.lineTo (x-radius/2,y+radius);
		path.lineTo (x + radius, y );

		path.lineTo(x-radius/2,y-radius);
		path.close();

		canvas.drawPath(path, p1);
	}
	//=================================================================
	
	 @Override
	    protected void onDraw(Canvas canvas) {
	        super.onDraw(canvas);
	        int x0=100,y0=0;//int x0=100,y0=100;
			int radius=100;//x0;

			//if(mEvt==1)
			//	this.SetBackgroundResource(Resource.Drawable.oval_status_background_pressed);
			//else
			//	this.SetBackgroundResource(Resource.Drawable.oval_status_background);
			boolean pressed=mEvt==1;
			drawSemiCircleGrad (canvas, x0, y0, radius-20,10, pressed);
			drawSemiCircle (canvas, x0, y0, radius-20,10, pressed);
		//	drawSemiCircle (canvas, x0, y0, radius-15,10, Color.Gray);

			
		//	_drawText ("Remaining", x0-40, y0-40, canvas, Color.Black, 18, true);

			//_drawText ("11:23", x0-36, y0-10, canvas, Color.Green, 28, true);


			// display driver status
			int x1=x0-40,y1=y0+30;
			int x2=40,y2=22;
			//TestAudit test = TestAudit.Get ();
			//int t_remaining=0;
			switch (driverState) 
			{
			case 101://(int)HOSAgent.DUTY_STATUS_OFFDUTY:  //0
				_drawText ("OFF DUTY", x1, y1, canvas, Color.BLACK, 22, true);
				drawOffDuty (canvas, x2, y2, 8, 4, Color.BLACK);
				break;
			case 102://(int)HOSAgent.DUTY_STATUS_SLEEPER_BERTH: //1
				_drawText ("SLEEPING", x1, y1, canvas, Color.BLACK, 22, true);
				drawSleeping (canvas, x2, y2, 8, 4, Color.BLACK);
				break;
			case 103://(int)HOSAgent.DUTY_STATUS_DRIVING: //2
				_drawText ("DRIVING", x1, y1, canvas, Color.BLACK, 22, true);
				drawDriving (canvas, x2, y2, 8, 4, Color.BLACK);
				//t_remaining = test.getAvailDrivingTime ();
				break;
			case 104://(int)HOSAgent.DUTY_STATUS_ON_DUTY:
				_drawText ("ON DUTY", x1, y1, canvas, Color.BLACK, 22, true);
				drawCircle (canvas, x2, y2, 8, 4, Color.RED);
				//t_remaining = test.getAvailOnDutyTime ();
				break;
			}

			//remaining time
			/*
			if (t_remaining < 0)
				t_remaining = 0;
			string	timeStr = TimeUtil.ConvertMinutesToHoursandMinutes (t_remaining);
			_drawText (timeStr, x0-36, y0-10, canvas, Color.Blue, 28, true);
			*/
			triagle (canvas, x0, y0+40, 20, 10,true); // Up
			triagle (canvas, x0, y0+65, 20, 10,false); // Down

	 }
}
