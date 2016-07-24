package com.mkr.unidriver.hos.dailylogs;

import java.lang.reflect.Field;

import android.content.Context;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.Scroller;

public class MyViewPager extends ViewPager
{
	
	public MyViewPager(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		
	}
	public <IAttributeSet> MyViewPager (Context ctx,AttributeSet attr)
	{
		super(ctx, attr);
	}
	@Override
	public  boolean onInterceptTouchEvent (MotionEvent ev)
	{
		//return base.OnInterceptTouchEvent (ev);
		return false;// never allow swiping to switch between pages
	}
	@Override
	public boolean onTouchEvent (MotionEvent e)
	{
		//return base.OnTouchEvent (e);
		return false;// never allow swiping to switch between pages
	}

	//-------------------Custom scroller---------------
	//https://github.com/Trinea/android-auto-scroll-view-pager
		/*	Scroller _customScroller=null;
	    public Scroller getCustomScroller() {
	       
	            return _customScroller;
	        }*/

	        void setCustomScroller(Scroller scroller)
	        {
	        	try {
	        		Field scrollerField = ViewPager.class.getDeclaredField("mScroller"); 
	        		scrollerField.setAccessible(true); 
	        		scrollerField.set(this, scroller);
	        	//	_customScroller=scroller;
	            } catch (Exception e) {
	                //e.printStackTrace();
	            }
	        }

}
