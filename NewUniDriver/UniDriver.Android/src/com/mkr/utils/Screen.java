package com.mkr.utils;

import android.content.Context;
import android.util.DisplayMetrics;

public class Screen 
{
	public static int toPx(Context ctx,float dp)
	{
		float scale = ctx.getResources().getDisplayMetrics().density;
		int pixels =(int) Math.round (dp * scale);//(int) (dp * scale + 0.5f); 
		return pixels;
	}
	public static float getScreenWidth (Context ctx)
	{
		DisplayMetrics metrics = ctx.getResources().getDisplayMetrics();
	    float widthInDp = metrics.widthPixels/metrics.density;
	    //var heightInDp = ConvertPixelsToDp(metrics.HeightPixels);
		return widthInDp;
	}
}
