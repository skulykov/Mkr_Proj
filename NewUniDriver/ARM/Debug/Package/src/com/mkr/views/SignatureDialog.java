package com.mkr.views;

import com.mkr.unidriver.MyProfileActivity;
import com.mkr.unidriver.R;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;

public class SignatureDialog {

	Context ctx;
	CaptureSignatureView mSignatureView=null;
	public SignatureDialog (Context _ctx)
	{
		ctx = _ctx;
	}
	public void showSignatureDlg ()
	{
		final MyProfileActivity act = (MyProfileActivity)ctx;
		//final CaptureSignatureView mSignatureView=null;
		final Dialog d = new Dialog (ctx, R.style.DialogSlideAnim); // with animation

		Window window = d.getWindow();//d.GetWindow();
		//window.SetSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
		window.setSoftInputMode (WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN); // hide KB  that did not work//A_Util.HideKB((Activity)ctx,at_email);
		window.setBackgroundDrawableResource (android.R.color.transparent);
		window.requestFeature (window.FEATURE_NO_TITLE);

		d.setContentView (R.layout.my_prof_signature_layout);
		 

		Button btn = (Button)d.findViewById (R.id.btn_done_sign_id);
		btn.setOnClickListener(new OnClickListener() {
		    public void onClick(View v) {
		    	String strSign=mSignatureView.GetSignatureString();
				if(strSign!=null&&!strSign.isEmpty())
				{
					act.UpdateSignature (strSign);
				}
		    	 d.cancel();
                 d.dismiss();
		    }
		});
		
		Button btn_cancel = (Button)d.findViewById (R.id.btn_cancel_sign_id);
		btn_cancel.setOnClickListener(new OnClickListener() {
		    public void onClick(View v) {
		    	mSignatureView.Clear();
		    	 d.cancel();
                 d.dismiss();
		    }
		});
		
		mSignatureView = new CaptureSignatureView (ctx/*, null*/);
		LinearLayout LLsignCap = (LinearLayout)d.findViewById (R.id.ll_sign_capture);
		mSignatureView.Clear();
		mSignatureView._BackColor = Color.GREEN;
		mSignatureView.ClearCanvas();
		LLsignCap.addView(mSignatureView);
		LLsignCap.setVisibility(View.VISIBLE);
		LLsignCap.requestFocus();
		
		d.show();


	}
	
}
