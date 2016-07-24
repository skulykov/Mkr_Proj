package com.mkr.unidriver.hos;

import android.os.Bundle;
import android.view.View;

import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;

public class CoWorkerLoginActivity extends OnResultBaseActivity {

	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.login_cow);
	        
	        SetMainTitles("Coworker login",R.layout.action_bar_title);
	 }
	 
	 public void CancelBtn(View v)
	 {
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 public void LoginBtn(View v)
	 {
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 
	 
}
