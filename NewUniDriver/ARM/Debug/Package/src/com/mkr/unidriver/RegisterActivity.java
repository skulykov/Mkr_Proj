package com.mkr.unidriver;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;


import com.mkr.models.proto.CommPackets.PRegister;
import com.mkr.models.proto.CommPackets.PRegister.Builder;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrMsgBox;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;


public class RegisterActivity extends OnResultBaseActivity  {
	 EditText username,fullname,email,password;
	 TextView error;
	
	    int rv=0;
	    	    
	    /** Called when the activity is first created. */
	    @Override
	    public void onCreate(Bundle savedInstanceState) {
	        super.onCreate(savedInstanceState);
	        setContentView(R.layout.login_register);
	       // Client=(EditText)findViewById(R.id.et_client);
	      
	        username=(EditText)findViewById(R.id.userName_id);
	        fullname=(EditText)findViewById(R.id.full_name_id);
	        email=(EditText)findViewById(R.id.email_id);
	        password=(EditText)findViewById(R.id.password_id);
	       
	        error=(TextView)findViewById(R.id.txt_login_error);
  
	        SetMainTitles("UniDriver Registration",R.layout.custom_ab_help);    
	    }
	    public void ABBackBtn(View v)
		{
		  finish();
		}
	    public void RegisterBtnClick(View v)
	    {
	    	String user=username.getText().toString();
	    	String pass=password.getText().toString(), fname=fullname.getText().toString(),eml=email.getText().toString();
	    	if(pass.isEmpty()||user.isEmpty()||fname.isEmpty()||eml.isEmpty())
	    	{
	    		MkrMsgBox bb=new MkrMsgBox();
	    		bb.Show( "Error","Fill out all fields");
	    		return;
	    	}
	    	if(!Util.isValidEmailAddress(eml))
	    	{
	    		MkrMsgBox bb=new MkrMsgBox();
	    		bb.Show( "Error","Email is invalid");
	    		return;
	    	}
	    	
	    	PRegister.Builder bb=PRegister.newBuilder();
	    	bb.setUser(user);
	    	bb.setPass(pass);
	    	bb.setFname(fname);
	    	bb.setEmail(eml);
	    	byte[] b=bb.build().toByteArray();
	    	
	    	MkrNLib.Get().registerReq(b,1);
	    }
	    
		 public void register_resp(int v) {
			if(v==1)
			{
				String user=username.getText().toString(),pass=password.getText().toString();
				
				Intent intent = new Intent();
				Bundle b = new Bundle(); 
		   		b.putString(GConst.EXTRA_REG_USER,user); //Your id
		   		b.putString(GConst.EXTRA_REG_PASS,pass); //Your id
		   		intent.putExtras(b); //Put your id to your next Intent 
				
				setResult(RESULT_OK);
			    
			}
			else
			{
				MkrMsgBox bb=new MkrMsgBox();
	    		bb.Show( "Error","We could not register you. Try again later");
			}
			 
			finish();
		}
    
		

}
