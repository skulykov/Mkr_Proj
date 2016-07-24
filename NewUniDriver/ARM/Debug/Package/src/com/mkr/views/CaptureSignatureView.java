package com.mkr.views;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.view.MotionEvent;
import android.view.View;

public class CaptureSignatureView extends View 
{

	private Bitmap _Bitmap;
	private Canvas _Canvas;
	
	public int _BackColor;
	Path _Path;
	Paint _BitmapPaint,_paint;
	List<Point> _SinglePoints=null,datas=null;
	private boolean drawFlag = false;

	public CaptureSignatureView(Context context) {
		super(context);
		initialize();
		this.setOnTouchListener(new OnTouchListener() {
	       
			@Override
			public boolean onTouch(View v, MotionEvent e) {
				return _onTouch(e);
				
			}
	    });

	}

	void initialize ()
	{
		_Path = new Path();
		_SinglePoints = new ArrayList<Point> ();
		_BitmapPaint = new Paint(Paint.DITHER_FLAG);
		_paint = new Paint();
		
		_paint.setAntiAlias( true);
		_paint.setDither(true);
		_paint.setColor(Color.argb(255, 0, 0, 0));
		
		_paint.setStyle(Paint.Style.STROKE);
		_paint.setStrokeJoin(Paint.Join.ROUND);
		_paint.setStrokeCap( Paint.Cap.ROUND);
		_paint.setStrokeWidth( 6);

		datas = new ArrayList<Point>();
	}
	public List<SignatureDataPoint> GetSignatureDataPoints()
	{
		List<SignatureDataPoint> data = new ArrayList<SignatureDataPoint>();
		for (int i = 1; i < datas.size(); i++)
		{
			if (datas.get(i - 1).x < 0) 
				continue;
			SignatureDataPoint sdp = new SignatureDataPoint();
			sdp.x1 = datas.get(i - 1).x;
			sdp.y1 = datas.get(i - 1).y;
			sdp.x2 = datas.get(i).x;
			sdp.y2 = datas.get(i).y;
			data.add(sdp);
		}
		return data;
	}
	/* Integer to String
	 * There are multiple ways:
	•String.valueOf(number) (my preference)
	•"" + number (I don't know how the compiler handles it, perhaps it is as efficient as the above)
	•Integer.toString(number)
*/
	
	public String GetSignatureString()
	{
		//Find the smallest X value and shift the sign using that value
		int Xshift = 10000000;
		for (int i = 0; i < datas.size(); i++)
		{
			if (datas.get (i).x > -1 && datas.get (i).x < Xshift)
				Xshift = datas.get (i).x;
		}
		Xshift = Xshift - 5; //Left Pad 5 pixles

		String rtnSign = "";
		for (int i = 1; i < datas.size(); i++)
		{
			if (datas.get(i-1).x < 0) 
				continue;

			if (i > 1)
				rtnSign += ";" + (datas.get(i-1).x-Xshift) + "," + datas.get(i-1).y + ";" + (datas.get(i).x-Xshift) + "," + datas.get(i).y;
			else
				rtnSign += (datas.get(i-1).x-Xshift) + "," + datas.get(i-1).y + ";" + (datas.get(i).x-Xshift) + "," + datas.get(i).y;
		}
		return rtnSign;
	}

	

	public void Clear() {
		initialize ();
		invalidate();
		
	}

	public void ClearCanvas() {
		if (_Canvas != null) {
			_Canvas.drawColor (_BackColor);
			invalidate ();
		}
		
	}

	public void SetSignatureString(String signature)
	{
		String[] signPoints = signature.split (";");

		if (signPoints.length > 1) {
			datas = new ArrayList<Point> ();
			String[] curPoint;
			for (int i = 1; i < signPoints.length; i=i+2) {
				if (signPoints[i - 1].length() > 0) {
					curPoint = signPoints [i - 1].split (",");
					datas.add (new Point (Integer.parseInt (curPoint [0]), Integer.parseInt (curPoint [1])));
				}
				if (signPoints [i].length() > 0) {
					curPoint = signPoints [i].split (",");
					datas.add (new Point (Integer.parseInt (curPoint [0]), Integer.parseInt (curPoint [1])));
				}
			}
			DrawSignature ();
			invalidate();
		}
	}

	public void SetSignatureDataPoints(List<SignatureDataPoint> data)
	{
		if (data.size() > 0) {
			datas = new ArrayList<Point> ();
			for (SignatureDataPoint sdp : data) {
				datas.add (new Point (sdp.x1, sdp.y1));
				datas.add (new Point (sdp.x2, sdp.y2));
			}
			DrawSignature ();
			invalidate ();
		}
	}
	private void DrawSignature()
	{
		Path tmpPath = new Path ();
		if (datas.size() > 0) {
			_Path.reset ();
			tmpPath.moveTo (datas.get(0).x, datas.get(0).y);
		}
		boolean hasMoved = true;
		for (int i =1; i < datas.size(); i++){
			if (datas .get(i).x < 0) {
				if (!tmpPath.isEmpty()) {
					_Path.addPath (tmpPath);
					tmpPath.reset ();
				} else {
					_SinglePoints.add(new Point(datas.get (i - 1).x, datas.get (i - 1).y));
				}
				hasMoved = false;
			} else {
				if ( hasMoved ) {
					tmpPath.lineTo (datas.get (i).x,datas.get (i).y);
				} else {
					tmpPath.moveTo (datas.get(i).x, datas.get(i).y);
					hasMoved = true;
				}
			}
		}
	}

	 @Override
	    protected void onSizeChanged(int w, int h, int wOld, int hOld)
	    {
	        super.onSizeChanged(w, h, wOld, hOld);
	        _Bitmap = Bitmap.createBitmap(w, (h > 0 ? h : ((View)this.getParent()).getHeight()), Bitmap.Config.ARGB_8888);
			_Canvas = new Canvas(_Bitmap);
	    }
	 
	 @Override
	    protected void onDraw(Canvas canvas) {
		 canvas.drawColor(_BackColor);
			canvas.drawBitmap(_Bitmap, 0, 0, _BitmapPaint);
			canvas.drawPath(_Path, _paint);

			if (_SinglePoints != null) {
				for (int i=0; i<_SinglePoints.size(); i++) {
					if (_SinglePoints .get(i).x > 0 && _SinglePoints.get (i).y > 0)
						canvas.drawPoint (_SinglePoints.get (i).x, _SinglePoints.get (i).y, _paint);
				}
			}
	 }
	 
	 private float _mX, _mY;
		private final float TouchTolerance = 4;

		private void TouchStart(float x, float y)
		{
			_Path.reset();
			_Path.moveTo(x, y);
			_mX = x;
			_mY = y;

			datas.add(new Point((int)x, (int)y));

			// process if currently drawing signature
			if (!drawFlag)
			{
				// start collecting points
				drawFlag = true;
			}
		}

		private void TouchMove(float x, float y)
		{
			//We don't want to collect the points that our outside our draw canves
			if (y < 0)
				return;

			float dx = Math.abs(x - _mX);
			float dy = Math.abs(y - _mY);

			if (dx >= TouchTolerance || dy >= TouchTolerance)
			{
				_Path.quadTo(_mX, _mY, (x + _mX) / 2, (y + _mY) / 2);

				// process if drawing signature
				if (drawFlag)
					datas.add(new Point((int)x, (int)y));

				_mX = x;
				_mY = y;
			}
		}

		private void TouchUp()
		{
			if (!_Path.isEmpty())
			{
				_Path.lineTo(_mX, _mY);
				_Canvas.drawPath(_Path, _paint);
			}
			else
			{
				_Canvas.drawPoint(_mX, _mY, _paint);
			}


			// process if drawing signature
			if (drawFlag)
			{
				// stop collecting points
				drawFlag = false;
				datas.add(new Point(-1, -1));
			}

			_Path.reset();
		}

	 
	 boolean _onTouch( MotionEvent e)
	 {
		 if (_BackColor==Color.WHITE)
				return true;

			float x = e.getX();
			float y = e.getY();

			switch (e.getAction())
			{
				case MotionEvent.ACTION_DOWN://MotionEventActions.Down:
				TouchStart(x, y);
				invalidate();
				break;
				case MotionEvent.ACTION_MOVE:
				TouchMove(x, y);
				invalidate();
				break;
				case MotionEvent.ACTION_UP:
				TouchUp();
				invalidate();
				break;
			}
			return true;
	 }

	//*******============== ===========================================================
	class SignatureDataPoint
	{
		public int x1, x2, y1, y2;
		public String ToString()
		{
			return String.format("%d,%d;%d,%d", x1, y1, x2, y2);
		}
		SignatureDataPoint Clone()
		{
			SignatureDataPoint sp = new SignatureDataPoint();
			sp.x1 = x1;
			sp.x2 = x2;
			sp.y1 = y1;
			sp.y2 = y2;
			return sp;
		}
	}
	
	
}
