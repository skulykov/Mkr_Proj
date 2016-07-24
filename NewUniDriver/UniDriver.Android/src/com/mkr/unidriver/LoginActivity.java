package com.mkr.unidriver;


import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;


import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;


public class LoginActivity extends OnResultBaseActivity  {
	 EditText /*Client,*/Phone,Pin;
		TextView error;
	    Button ok;
	    CheckBox cb=null; 
	    int rv=0;
	    Activity _this=null;
	    ProgressDialog progressDialog = null;
	    boolean after_logout=false;
	    //==============================================================================
	    static
	    {
	    	MkrNLib.Get().initLibs();
	    	MkrNLib.Get().initOnStart();
	    }
	    
	    
	    /** Called when the activity is first created. */
	    @Override
	    public void onCreate(Bundle savedInstanceState) {
	        super.onCreate(savedInstanceState);
	        _this=this;
	        Intent i=getIntent();
	        int tmp=i.getIntExtra(GConst.EXTRA_LOGOUT,-1);
	        if(tmp==GConst.REQUEST_LOGOUT)
	        	after_logout=true;
	        setContentView(R.layout.login);
	       // Client=(EditText)findViewById(R.id.et_client);
	        Phone=(EditText)findViewById(R.id.userName);
	        Pin=(EditText)findViewById(R.id.password);
	        ok=(Button)findViewById(R.id.log_in);
	        error=(TextView)findViewById(R.id.txt_login_error);
	        //Global.domainURL = "http://www.wilinxdemexico.net/nReq.ashx";//"http://www.mikerel.com/";
	        ok.setOnClickListener(new View.OnClickListener() {

	            @Override
	            public void onClick(View v) {
	            	progressDialog =ProgressDialog.show(_this, "", _this.getResources().getString(R.string.str_please_wait));
	            	//fake it
	            	//startMainAct();
					MkrNLib.Get().loginReq(Phone.getText().toString(),Pin.getText().toString(),1000);
	            	
	            	return;
	            	
	            }
	        });
	        SetMainTitles("UniDriver Login",R.layout.action_bar_title); 
	    }
	    private void CheckIfLoginIsNeeded() {
			
			int rv=MkrNLib.Get().checkIfLoginIsNeeded();
			if(rv==0)
			{
				MkrApp.StartApp( this, null );   //====================?????? 	
				startNewActAfterLogin ();
			}
		}
	    void startNewActAfterLogin ()
		{
			
			Intent intd = new Intent (this, MainDrawerActivity.class);
			//act.PutExtra ("mkey", Username+"|"+Password);
			//startActivityForResult (act, GConst.REQUEST_REGISTER_ADD);
			startActivity(intd);

		}
		@Override
	    public void onResume()
	    {
	    	super.onResume();
	    	 CheckIfLoginIsNeeded (); // do we need to login??
	    }
        public void RegisterBtn(View v)
        {
        	Intent serverIntent = new Intent(this, RegisterActivity.class);
			this.startActivityForResult(serverIntent, GConst.REQUEST_REGISTER_ADD);
        }
		 public void login_resp(int v) {
			 if(progressDialog != null && progressDialog.isShowing())
					progressDialog.dismiss ();
			if(v==2)
			{
				MkrApp.StartApp( this, null );   //====================?????? 	
				
			  Intent intent = new Intent(LoginActivity.this, MainDrawerActivity.class/*VehiclesActivity.class*/);
			  startActivity(intent);
			}
			
		}
    
		 @Override
		 protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		     // Check which request we're responding to
		     if (requestCode == GConst.REQUEST_REGISTER_ADD) {
		         // Make sure the request was successful
		         if (resultCode == RESULT_OK) {
		        	
		 	        String _str=data.getStringExtra (GConst.EXTRA_REG_USER);
		 	        Phone.setText(_str);
		 	        _str=data.getStringExtra (GConst.EXTRA_REG_PASS);
		 			Pin.setText(_str);	
		         }
		     }
		 }

}
