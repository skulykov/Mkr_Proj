package com.mkr.unidriver.hos.dailylogs;

import android.app.ActionBar;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.view.PagerAdapter;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.TextView;

import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.views.HosGraphView;

public class HosDailyLogsPagerActivity extends OnResultBaseActivity 
{
	private MyViewPager viewPager;
	private HosDailyLogsPagerAdapter pagerAdapter;
	ImageButton btnPrev=null,btnNext=null;//,backButton=null;
	private ImageButton btnSign,btnAddHosEvent, btnCancel;
	
	int index_item=0;
	 @Override
	    public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.hos_daily_logs_container_layout);
	        
	        //SetMainTitles("Hos Email",R.layout.action_bar_title);
	        ShowCustomActionBarView4DailyLogs("HOS Daily Logs");
            FragmentManager fm =getSupportFragmentManager();
			pagerAdapter = new HosDailyLogsPagerAdapter(fm);
			
	        viewPager = (MyViewPager)this.findViewById(R.id.pager);
	        
	        FixedSpeedScroller scroller=new FixedSpeedScroller(this,1400);
			viewPager.setCustomScroller(scroller);
			
			viewPager.setAdapter(pagerAdapter);
			//this.SetPageTransformer ();
			
			btnSign = (ImageButton) findViewById (R.id.btn_hos_insp_sign);
   			btnSign.setOnClickListener(btn_listener);

   			//Let check to see if any of the TimeLogs of this LogSheet is not signed then show sign button
   			SetSignBtnVisibility ();

   			btnAddHosEvent = (ImageButton)findViewById (R.id.btn_hos_insp_add);
   			btnAddHosEvent.setOnClickListener(btn_listener); 

   			btnCancel =(ImageButton) findViewById (R.id.btn_hos_dlog_cancel);
   			btnCancel.setOnClickListener(btn_listener);
	 }
	 OnClickListener btn_listener=new OnClickListener(){

			@Override
			public void onClick(View v) {
				if(v==btnCancel)
				{
					finish();
				}else if(v==btnAddHosEvent)
				{
					PagerFragment pf=pagerAdapter.getFragment(viewPager.getCurrentItem());
					if(pf!=null)
						pf.AddHosEvent();
				}else if(v==btnSign)
				{
					SignTimeLog () ;
				}
				
			}
			
		};
		void SignTimeLog () 
   		{ /*
   				ctx.RunOnUiThread (() => {
   					AlertDialog alertMessage = new AlertDialog.Builder(ctx).Create();
   					alertMessage.SetTitle(this.Resources.GetString(Resource.String.alert_title));
   					alertMessage.SetMessage(this.Resources.GetString(Resource.String.sign_alert_q));
   					alertMessage.SetButton(this.Resources.GetString(Resource.String.Yes), delegate {
   						//Check to see if currentEmployee has signature, if not, take them to MyProfile add signature
   					EmployeeRow  prof = Profile.GetCurrentProfile();
   					if(prof != null && !string.IsNullOrEmpty(prof.Signature)){
   							foreach(TimeLogRow TLrow in hgv.GetTimeLogList())
   							{
   								if (!TLrow.Signed) {
   									TLrow.Signed = true;
   									TLrow.SetHaveSend(false);
   									TimeLogDB.Get().SaveTimeLog (TLrow);
   								}
   							}
   							//If no timelogs and trying to sign add one off duty to this day
   							if(hgv.GetTimeLogList().Count == 0){
   								TimeLogRow tempLog = new TimeLogRow();
   								//SentinelMobile.Shared.Data.Utils.LocalizeTimeLog(ref tempLog);
   								tempLog.Type = TimeLogType.Auto;
   								tempLog.LogTime = currentGraphDate.Date;
   								tempLog.Signed = true;
   								TimeLogDB.Get().SaveTimeLog (tempLog);
   							}
   							//Global.runTimerCallBackNow();   ---> send immediately
   							SetSignBtnVisibility();
   						} else{
   							AlertDialog alertMessage2 = new AlertDialog.Builder(ctx).Create();
   							alertMessage2.SetTitle(this.Resources.GetString(Resource.String.alert_title));
   							alertMessage2.SetMessage(this.Resources.GetString(Resource.String.sign_alert_warning));
   							alertMessage2.SetButton(this.Resources.GetString(Resource.String.OK), delegate {
   								var sIntent = new Intent(ctx, typeof(MyProfileActivity));
   								
   								StartActivityForResult(sIntent, REQUEST_SIGNATURE_ADD);
   							});
   							alertMessage2.SetButton2(this.Resources.GetString(Resource.String.button_scan_cancel), delegate {
   							});
   							alertMessage2.Show();
   							alertMessage.SetCanceledOnTouchOutside(false);
   						}
   					});
   					alertMessage.SetButton2(this.Resources.GetString(Resource.String.No), delegate {
   					});
   					alertMessage.Show();
   					alertMessage.SetCanceledOnTouchOutside(false);
   				});
   				*/
   			}

		public void SetSignBtnVisibility()
	   	{
	   		boolean logSheetSigned = true;
	   		PagerFragment pf=pagerAdapter.getFragment(viewPager.getCurrentItem());
			if(pf==null) return;
	   		HosGraphView hgv=pf.getHgv();
	   		for (PTimeLogRow logrow : hgv.GetTimeLogList()) {
	   			if (!logrow.getSigned()) {
	   				logSheetSigned = false;
	   				break;
	   			}
	   		}
	   		if (logSheetSigned) {
	   			btnSign.setImageResource (R.drawable.ic_signed);
	   			btnSign.setEnabled(false);
	   		} else {
	   			btnSign.setImageResource (R.drawable.ic_signature);
	   			btnSign.setEnabled(true);
	   		}
	   	}
		public void ShowCustomActionBarView4DailyLogs(String title/*,EventHandler cb=null*/)
		{
			ActionBar mActionBar = this.getActionBar();
			LayoutInflater mInflater = getLayoutInflater();
			View mCustomView = mInflater.inflate(R.layout.custom_action_bar_hos_daily_logs, null);
			TextView mTitleTextView = (TextView) mCustomView.findViewById(R.id.hos_bar_title_id);
			mTitleTextView.setText(title);

			/*backButton = (ImageButton) mCustomView.findViewById(R.id.back_btn_id);
			backButton.setOnClickListener(listener); */
			btnPrev = (ImageButton)mCustomView.findViewById(R.id.btn_hos_insp_prev);
			btnNext = (ImageButton)mCustomView.findViewById (R.id.btn_hos_insp_next);
			btnNext.setOnClickListener(listener);

			btnPrev.setOnClickListener(listener);

			mActionBar.setDisplayShowHomeEnabled(false);
			mActionBar.setDisplayShowTitleEnabled(false);
			mActionBar.setCustomView(mCustomView);
			mActionBar.setDisplayShowCustomEnabled(true);


		//	backButton.setVisibility(View.GONE);
		}
		public ImageButton getPrevBtn(){return btnPrev;}
		public ImageButton getNextBtn(){return btnNext;}
		public void SetItem (int index)
		{
			viewPager.setCurrentItem(index,true);

		}
		OnClickListener listener=new OnClickListener(){

			@Override
			public void onClick(View v) {
				/*if(v==backButton)
				{
					BackButtonClick();
				}else*/ 
				if(v==btnPrev)
				{
					doPrev();
				}else if(v==btnNext)
				{
					doNext();
				}
				
			}
			
		};

		void doPrev ()
		{
			index_item++;
			if (index_item == pagerAdapter.getCount()) {
				btnPrev.setVisibility(View.INVISIBLE);
				index_item--;
			} else {
				SetItem (index_item);
			}
			btnNext.setVisibility(View.VISIBLE);
		     
		}
		void doNext ()
		{
			index_item--;
			if (index_item <= 0) {
				if(index_item==0)
						SetItem (index_item);
				index_item = 0;
				btnNext.setVisibility(View.INVISIBLE);
			} else {
				btnPrev.setVisibility(View.VISIBLE);
				SetItem (index_item);
			}

		      
		}
}
