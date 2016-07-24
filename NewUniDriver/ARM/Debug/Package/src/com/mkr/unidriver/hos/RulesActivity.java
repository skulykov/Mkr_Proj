package com.mkr.unidriver.hos;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.mkr.service.MkrNLib;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;
import com.mkr.views.HosCyclePopUp;

public class RulesActivity extends OnResultBaseActivity
{
	
	Button cycle_btn=null;
	
	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.hos_rules);
	        
	        SetMainTitles("Hos Rules",R.layout.action_bar_title);
	        
	        cycle_btn=(Button)findViewById(R.id.rule_cycle_btn);
	        
	        RefreshCycleStatus (MkrNLib.Get().GetStringFromCurrentHosCycle ());
	 }
	 
	 public void CancelBtn(View v)
	 {
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 public void SaveBtn(View v)
	 {
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 public void CycleBtn(View v)
	 {
			ProcessCycleOptionsMenu (v);
	 }
	 public void RefreshCycleStatus (String sts)
	 {
			//HOSCYCLE status = (HOSCYCLE)sts;//JobsHelper.GetJobsStatusSelectedIndexRow ();
			cycle_btn.setText(sts);

	 }
		HosCyclePopUp c_menuqa=null;
		void ProcessCycleOptionsMenu (Object sender)
		{

			if (c_menuqa == null)
				c_menuqa = new HosCyclePopUp (this);
			c_menuqa.Show ((View)sender);

		}
}
