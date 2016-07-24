package com.mkr.unidriver;

import com.mkr.service.MkrApp;
import android.app.ActionBar;
import android.app.Activity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.TextView;



public class BaseDrawerActivity extends  Activity//AppCompatActivity
{
	 @Override
	    public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	       // requestFeature(FEATURE_SUPPORT_ACTION_BAR);
	 }
	 
	@Override
	    public void onResume() {
	        super.onResume();
	        MkrApp.SetCurrentOnTopActivity( this );
	 }
	
	public void SetMainActionBarView(String title,CallBack _cb)
	{

		//=======Actionbar==============
		ActionBar mActionBar=  getActionBar();//getSupportActionBar();
		//View v=    mActionBar.getCustomView();

		mActionBar.setDisplayHomeAsUpEnabled(true);
		
		LayoutInflater mInflater = this.getLayoutInflater();
		View mCustomView = mInflater.inflate(R.layout.action_bar_title_image, null);
		
		TextView mTitleTextView = (TextView) mCustomView.findViewById(R.id.abar_title_id);
		mTitleTextView.setText(title);
		ImageButton menuImageButton = (ImageButton) mCustomView.findViewById(R.id.imageButton);
		
		//menuImageButton.Click+= cb;
		final CallBack cb=_cb;
		menuImageButton.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				if(cb!=null)
					cb.ImageButtonMenuClick();
				
			}
			
		});

		
		mActionBar.setCustomView(mCustomView);
		mActionBar.setDisplayShowCustomEnabled(true);
	
	}
	
	public void SetMainTitles(String title/*,String title1,String title2*/)
	{
		ActionBar ab=getActionBar();
		View v=    ab.getCustomView();
		
		TextView tv = (TextView) v.findViewById(R.id.abar_title_id);
		tv.setText(title);
/*
		tv = (TextView) v.findViewById(R.id.abar_title1_id);
		tv.setText(title1);

		tv = (TextView) v.findViewById(R.id.abar_title2_id);
		tv.setText(title2); 
*/
		v.invalidate();  // update 
	}
	@Override
	public void onBackPressed() {
		// TODO Auto-generated method stub
		
	}
	
}
