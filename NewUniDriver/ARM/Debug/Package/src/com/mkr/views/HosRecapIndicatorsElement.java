package com.mkr.views;

import android.content.Context;
import android.graphics.Canvas;

public class HosRecapIndicatorsElement extends HosRecapIndicatorView {
	Context ctx;
	int mWidth = 0,	mHeight = 0;
	public HosRecapIndicatorsElement(Context ctx) {
		super(ctx);
		this.ctx=ctx;
	}
	 @Override
	    protected void onSizeChanged(int w, int h, int wOld, int hOld)
	    {
	        super.onSizeChanged(w, h, wOld, hOld);
	        mWidth = w;
			mHeight = h;
	    }
	 @Override
	    protected void onDraw(Canvas canvas) {
		 float w = mWidth, h = mHeight;
			float x0 = w / 4;//bounds.GetMidX ();
			float y0 = h / 4;//bounds.GetMidY ();
			float radius = 80;
			SetRecapSummary();
			for (int i = 0; i < 4; i++) {

				switch (i) {
				case 0:
					_Draw(canvas,x0, y0,radius, 1);
				break;
				case 1:
					_Draw(canvas,x0 + w / 2, y0,radius, 2);
				break;
				case 2:
					_Draw(canvas,x0, y0 + h / 2,radius, 3);
				break;
				case 3:
					_Draw(canvas,x0 + w / 2, y0 + h / 2,radius, 4);
				break;
				}


			}
	 	}

}
