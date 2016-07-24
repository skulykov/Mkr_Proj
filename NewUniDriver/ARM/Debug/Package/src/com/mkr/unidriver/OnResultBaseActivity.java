package com.mkr.unidriver;

import com.mkr.service.MkrApp;
import com.mkr.utils.GConst;

import android.app.ActionBar;
import android.app.Activity;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.view.View;
import android.widget.TextView;

public class OnResultBaseActivity extends FragmentActivity 
{
	 @Override
	    public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	 }
	 
	 @Override
	    public void onResume() {
	        super.onResume();
	        MkrApp.SetCurrentOnTopActivity( this );
	 }
	 @Override
		public void onBackPressed() {
			// TODO Auto-generated method stub
			
		}
		public void BackButtonClick()
		{
			Finish_Activity (GConst.G_RESULT_OK);
		}
		public void Finish_Activity(int res)
		{
			this.setResult(res);
			this.finish();
			overridePendingTransition(R.anim.pull_in_left,R.anim.push_out_right);
		}
		
		public void SetTitle(String t)
		{
			MainDrawerActivity act = (MainDrawerActivity)MkrApp.GetMainActivity();
			if( act!= null)
			{
				act.SetMainTitles(t);
			}
		}
		//==========================
		public void SetMainTitles(String title,int res_id)
		{
			ActionBar ab=getActionBar();
			ab.setDisplayOptions(/*ActionBar.DISPLAY_SHOW_HOME | ActionBar.DISPLAY_SHOW_TITLE | */ActionBar.DISPLAY_SHOW_CUSTOM);
			ab.setCustomView(res_id);
			TextView tv = (TextView) ab.getCustomView().findViewById(R.id.bar_title_id);
            tv.setText(title);
			//ab.setTitle(title); 
		}
		
}
