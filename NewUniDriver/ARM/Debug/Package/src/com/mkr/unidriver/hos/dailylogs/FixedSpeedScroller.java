package com.mkr.unidriver.hos.dailylogs;

import android.content.Context;
import android.view.animation.Interpolator;
import android.widget.Scroller;

public class FixedSpeedScroller extends Scroller 
{
	 private int mDuration = 1000;

		 public FixedSpeedScroller(Context context,int duration){
			 super(context);
			 mDuration =duration;
		 }
		
		public FixedSpeedScroller(Context context, Interpolator interpolator) {
			 super(context,interpolator);
		 }
		
		public FixedSpeedScroller(Context context, Interpolator interpolator, boolean flywheel) {
			super(context, interpolator, flywheel);   
		 }
		@Override
	 public  void startScroll (int startX, int startY, int dx, int dy, int duration)
		{
			super.startScroll (startX, startY, dx, dy, mDuration);
		}
		@Override
		public  void startScroll (int startX, int startY, int dx, int dy)
		{
			super.startScroll (startX, startY, dx, dy, mDuration);
		}
}
