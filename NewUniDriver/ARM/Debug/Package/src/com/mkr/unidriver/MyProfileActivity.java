package com.mkr.unidriver;

import com.mkr.models.proto.CommPackets.PEmployeeRow;
import com.mkr.models.proto.CommPackets.PRegister;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.views.CaptureSignatureView;
import com.mkr.views.SignatureDialog;

import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;


public class MyProfileActivity extends OnResultBaseActivity 
{
	LinearLayout LLcurrentSignature = null;
	CaptureSignatureView mSignatureView = null;
	PEmployeeRow prof=null;
	
	 @Override
	    public void onCreate(Bundle savedInstanceState) {
	        super.onCreate(savedInstanceState);
	        setContentView(R.layout.my_profile_layout);
	        
	        prof=MkrNLib.Get().GetEmployee();
	        if(prof==null)
	        	return;
	        TextView txtName = (TextView)findViewById (R.id.prof_emp_name_id);
			txtName.setText(prof.getDriverName());
			txtName = (TextView)findViewById (R.id.prof_emp_id_id);
			txtName.setText( prof.getID());


			txtName = (TextView)findViewById (R.id.prof_un_id);
			txtName.setText( prof.getUserName());

			txtName = (TextView)findViewById (R.id.prof_email_id);
			txtName.setText(prof.getEmail());

			txtName = (TextView)findViewById (R.id.prof_h_addr_id);
			txtName.setText(prof.getHomeAddress());

			txtName = (TextView)findViewById (R.id.prof_license_id);
			txtName.setText(prof.getLicense());

			txtName = (TextView)findViewById (R.id.prof_cycle_id);
			txtName.setText(MkrNLib.Get().GetStringFromCurrentHosCycle ());

			txtName = (TextView)findViewById (R.id.prof_un_id);
			txtName.setText( prof.getUserName());

			txtName = (TextView)findViewById (R.id.prof_tz_id);
			float vv=prof.getTZ();
			String tz=vv>=0?"+":"-";
			txtName.setText(/*prof.getTimeZone()*/"UTC"+tz+vv);


			///////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////SignatureCapture//////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////
			LLcurrentSignature = (LinearLayout)findViewById (R.id.ll_cur_signature);
			mSignatureView = new CaptureSignatureView (this/*, null*/);

			//Button btnChangeSign =(Button) findViewById(R.id.btn_change_sign_id);
			if (!prof.getSignature().isEmpty()) {
				showSignature(prof.getSignature());
			}
	        
	        SetTitle("Driver Profile");
	 }
	 public void ChangeSignature(View v)
	 {
		 SignatureDialog dlg=new SignatureDialog(this);
			dlg.showSignatureDlg();
	 }
	 void showSignature(String sig)
		{
			    LLcurrentSignature.removeAllViews();
				mSignatureView.Clear();
				mSignatureView._BackColor = Color.WHITE;
				mSignatureView.SetSignatureString (sig);
				LLcurrentSignature.addView(mSignatureView);
		}
	 public void UpdateSignature (String strSign)
		{
		//	prof.setSignature(strSign);
		//	prof.haveSend=false;  // force to send this profile to the server
		//	EmployeeDB.Get().SaveEmployee (prof);
			showSignature(strSign);
			SaveThisEmployee();
		}
	 void SaveThisEmployee()
	 {
		 	PEmployeeRow.Builder bb=prof.toBuilder();   //PEmployeeRow.newBuilder();
	    	//bb.setUserName("");
	    	//bb.setPassword("");
	    	//bb.setLicense("");
	    	//bb.setEmail("");
	    	
	    	String sig=mSignatureView.GetSignatureString ();
	    	bb.setSignature(sig);
	    	
	    	byte[] b=bb.build().toByteArray();
	    	
	    	MkrNLib.Get().saveEmployeeRow(b);
	 }
	 
	 
}
