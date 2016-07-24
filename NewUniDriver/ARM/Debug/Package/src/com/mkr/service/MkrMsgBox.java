package com.mkr.service;


import com.mkr.unidriver.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Handler;
import android.text.util.Linkify;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;



public  class MkrMsgBox 
{
	   
		public void Show( final String aTitle, final String aMessage, final String aLabelOk, final String aLabelCancel, 
				final DialogInterface.OnClickListener aOkCb, final DialogInterface.OnClickListener aCancelCb )
		{
		//	final MkrNVMng mgr = MkrAppService.getNativeManager();
			final Activity activity = MkrApp.GetCurrentOnTopActivity();
			
			Log.w( "Mkr", "Show_1 ..." );
			
			if ( activity == null )
			{
				Log.e( "Mkr", "Show_1 ...return" );
				return;
			}
			
			{
				ShowRun( aTitle, aMessage, aLabelOk, aLabelCancel, aOkCb, aCancelCb );
			}
		}
		
				 
		 /*************************************************************************************************
	     * ShowOk - Shows native message box with Ok button only. Static wrapper   
	     * 
	     */
		public void ShowOk( final String aTitle, final String aMessage, final String aLabelOk, final DialogInterface.OnClickListener aOkCb )
		{
			Show( aTitle, aMessage, aLabelOk, null, aOkCb, null );
		}

		 /*************************************************************************************************
	     * ShowRun - Shows native message box   
	     * Auxiliary - see matching Show call
	     */
		public void ShowRun( final String aTitle, final String aMessage, final String aLabelOk, final String aLabelCancel, 
				final DialogInterface.OnClickListener aOkCb, final DialogInterface.OnClickListener aCancelCb )
		{
			AlertDialog dlg = builder().create();
	        dlg.setCancelable( false );
	        
	        if ( ( aLabelOk != null ) && ( aOkCb != null ) )
	        {
	        	dlg.setButton( new String( aLabelOk ), aOkCb );
	        }
	        if ( ( aLabelCancel != null ) && ( aCancelCb != null ) )
	        {
	        	dlg.setButton2( new String( aLabelCancel ), aCancelCb );
	        }

	        dlg.setTitle( aTitle );
	        dlg.setMessage( aMessage );
	        
	        dlg.show();
	        
	        // Makes http links clickable
	        TextView msgView = (TextView) dlg.findViewById(android.R.id.message);
	        if ( msgView != null )
	        {
	        	Linkify.addLinks( msgView, Linkify.WEB_URLS );
	        }
		}
		
	   
		private void ShowRun( byte[] aTitle, byte[] aMessage, byte[] aLabelOk, byte[] aLabelCancel, long aCbContext )
		{
			Activity activity = MkrApp.GetCurrentOnTopActivity();
			AlertDialog.Builder builder = builder();
			//Then I do what I need with the builder (except for setTitle();
			LayoutInflater inflater = activity.getLayoutInflater();
			View view=inflater.inflate(R.layout.custom_dialog_title, null);
			TextView title = (TextView)view.findViewById(R.id.myDlgTitle);
			title.setText(new String( aTitle )/*"Title I want to show"*/);
			builder.setCustomTitle(view);

			
			final AlertDialog dlg = builder.create();
	        dlg.setCancelable( false );
	        
	        if ( aLabelOk != null )
	        {
	        	//dlg.setButton( new String( aLabelOk ), new MsgBoxOnClick( Mkr_MSG_BOX_RES_OK, aCbContext ) );
	        }
	        if ( aLabelCancel != null )
	        {
	        	//dlg.setButton2( new String( aLabelCancel ), new MsgBoxOnClick( Mkr_MSG_BOX_RES_CANCEL, aCbContext ) );
	        }
	       // dlg.setTitle( new String( aTitle ) );
	        dlg.setMessage( new String( aMessage ) );
	        
	        //Full Screen AlertDialog -> comment out if needed
	        WindowManager.LayoutParams lp = new WindowManager.LayoutParams();
	        lp.copyFrom(dlg.getWindow().getAttributes());
	        lp.width = WindowManager.LayoutParams.FILL_PARENT;
	        lp.height = WindowManager.LayoutParams.FILL_PARENT;

	        
	        dlg.show();
	        
	        dlg.getWindow().setAttributes(lp); //Full Screen AlertDialog -> comment out if needed
	        
	        // Makes http links clickable       
	       /*
	        TextView msgView = (TextView) dlg.findViewById(android.R.id.message);
	        if ( msgView != null )
	        {
	        	Linkify.addLinks( msgView, Linkify.WEB_URLS );
	        }*/
	        
	        Handler handler = null;
	        handler = new Handler(); 
	        handler.postDelayed(new Runnable(){ 
	             public void run(){
	                 dlg.cancel();
	                 dlg.dismiss();
	             }
	        }, 2500);

	        
		}
		private void show_1()
		{
			Activity ctx = MkrApp.GetCurrentOnTopActivity(); 
			AlertDialog.Builder builder = new AlertDialog.Builder(ctx);
		         // Creates textview for centre title
		   TextView myMsg = new TextView(ctx);
		   myMsg.setText("Succesfully send!");
		   myMsg.setGravity(Gravity.CENTER_HORIZONTAL);
		   myMsg.setTextSize(20); 
		   myMsg.setTextColor(Color.WHITE);
		   //set custom title
		         builder.setCustomTitle(myMsg);
		         builder.setMessage("Check your email and verify your username and password...");
		         builder.setPositiveButton("OK", null);
		         
		         AlertDialog dialog = builder.create();
		         
		         dialog.show();
		         
		         //dialog.getWindow().getDecorView().setBackgroundColor(0xaabbcc);
		         
		         //Create custom message
		         TextView messageText = (TextView)dialog.findViewById(android.R.id.message);
		         messageText.setGravity(Gravity.CENTER);
		}
		
		public void Show( String aTitle, String aMessage )
		{
			Activity ctx = MkrApp.GetCurrentOnTopActivity(); 
			final Dialog d = new Dialog(ctx,/*R.style.PauseDialog*/R.style.DialogSlideAnim); // with animation
			//d.requestWindowFeature(Window.FEATURE_NO_TITLE);
			//http://stackoverflow.com/questions/4757558/android-dialog-box-without-shadow-around-it
			//d.getWindow().clearFlags(android.view.WindowManager.LayoutParams.FLAG_DIM_BEHIND);
			
			Window window = d.getWindow();
			window.setBackgroundDrawableResource(android.R.color.transparent);
			window.requestFeature(window.FEATURE_NO_TITLE);
			
			d.setContentView(R.layout.my_dialog_layout);
			TextView title=(TextView)d.findViewById(R.id.mDlgTextTitle);
			title.setText(aTitle );
			
			TextView msg=(TextView)d.findViewById(R.id.mDlgTextMsg);
			msg.setText(aMessage);
			
			final TextView to=(TextView)d.findViewById(R.id.timeoutText_id);
			
			Button btn=(Button)d.findViewById(R.id.dlg_button_ok);
			btn.setOnClickListener(new OnClickListener() {
			    public void onClick(View v) {
			    	 d.cancel();
	                 d.dismiss();
			    }
			});
			d.show();
			 
			 final Handler handler = new Handler(); 
		        handler.postDelayed(new Runnable(){ 
		             public void run(){
		            	 
		            	 int timeout=Integer.parseInt((String) to.getText());
		            	 timeout--;
		            	 if(timeout==0)
		            	 {
		            		 d.cancel();
		            		 d.dismiss();
		            	 }
		            	 else
		            	 {
		            		 to.setText(Integer.toString(timeout));
		            		 handler.postDelayed(this, 1000); // Run this again in one second
		            	 }

		             }
		        }, 1000);

		}
		
	    /*************************************************************************************************
	     * builder() - builder creator  
	     * @param void
	     * @return dialog builder 
	     */
		private AlertDialog.Builder builder()
		{
			Activity activity = MkrApp.GetCurrentOnTopActivity();
			return ( new AlertDialog.Builder( activity, AlertDialog.THEME_HOLO_LIGHT ) );
		}
		
		public void ShowLogout(final Activity ctx)
		{
			new AlertDialog.Builder(ctx)
	        .setIcon(android.R.drawable.ic_dialog_alert)
	        .setTitle("Logout")
	        .setMessage("Do you want to logout?")
	        .setPositiveButton("Yes", new DialogInterface.OnClickListener()
		    {
		        @Override
		        public void onClick(DialogInterface dialog, int which) {
		            
		        	ctx.finish();    
		        }
		
		    })
		    .setNegativeButton("No", null)
		    .show();

		}
	   
	    /*************************************************************************************************
	     * MkrMsgBox - private constructor  
	     * @return void 
	     */
		public MkrMsgBox()
		{		
		}
		
	  
		
		/*************************************************************************************************
	     *================================= Constants section =================================
	     */
		public static final int Mkr_MSG_BOX_RES_CANCEL = 0;
		public static final int Mkr_MSG_BOX_RES_OK = 1;

}
