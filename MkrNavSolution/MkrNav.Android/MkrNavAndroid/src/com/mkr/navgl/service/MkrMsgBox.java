/**  
 * MkrMsgBox - Provides interface for the native message box presentation  
 
 */
package com.mkr.navgl.service;

import com.mkr.navgl.nintf.MkrNavMainAct;
import com.mkr.navgl.nintf.R;
import com.mkr.navgl.nintf.R.id;
import com.mkr.navgl.nintf.R.layout;
import com.mkr.navgl.nintf.R.style;

import android.app.ActionBar.LayoutParams;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Handler;
import android.text.util.Linkify;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

public final class MkrMsgBox 
{
    /*************************************************************************************************
     *================================= Public interface section =================================
     */
    /*************************************************************************************************
     * InitNativeLayer - Initialization of the core level for the message box  
     * At this stage the library should be loaded
     * @return MkrMsgBox instance 
     */
	static void InitNativeLayer()
	{
		MkrMsgBox instance = getInstance();
		instance.InitMsgBoxNTV();		
	}
	
    /*************************************************************************************************
     * Create - Msg box instance getter  
     * @return MkrMsgBox instance 
     */
	static MkrMsgBox getInstance()
	{
		if ( mInstance == null )
		{
			mInstance = new MkrMsgBox();
		}
		return mInstance;
	}
	
	/*************************************************************************************************
     * setBlocking - sets the messagebox to be blocking  
     * @param aValue - true message box blocks the native thread  
     */
	void setBlocking( boolean aValue )
	{
		mInstance.mIsBlocking = aValue;
	}
	
    /*************************************************************************************************
     * Show - Shows native message box  (for use from the library side) 
     * @param aTitle - title at the top of the box
     * @param aMessage - message at the body of the box
     * @param aLabelOk - label of the ok button
     * @param aLabelCancel - label of the cancel button
     * @param aCbContext - the context of the buttons callback 
     * @return void 
     */
	public void Show( final byte[] aTitle, final byte[] aMessage, final byte[] aLabelOk, final byte[] aLabelCancel, final long aCbContext )
	{
		final MkRlNativeManager mgr = MkRlAppService.getNativeManager();
		final MkrNavMainAct activity = MkRlAppService.getMainActivity();
		/*
		if ( activity == null || mgr == null )
		{
			Log.e( "Mkr", "Could not call Show" );
			return;
		}*/
        /*
		if ( mgr.IsNativeThread() )
		{
			final Runnable runMsgBox = new Runnable() {
				public void run() {					
					ShowRun( aTitle, aMessage, aLabelOk, aLabelCancel, aCbContext );
				}
			};
			activity.runOnUiThread( runMsgBox );
			// Wait on native thread 
			if ( mIsBlocking )
			{
				try 
				{
		    		synchronized( mInstance ) {
		    			mInstance.wait();
		    			mIsBlocking = false;
		    		}				
					Log.w( "Mkr", "Continue running the native thread" );
				}
				catch ( Exception ex ) {
					Log.e("Mkr", "Error waiting for the message to finish" );
				}
			}

		}
		else  */
		{
			Log.w( "Mkr", "ShowRun ..." );
			show_2(aTitle, aMessage);
			//ShowRun( aTitle, aMessage, aLabelOk, aLabelCancel, aCbContext );
		}
	}
	 /*************************************************************************************************
     * Show - Shows native message box   
     * @param aTitle - title at the top of the box
     * @param aMessage - message at the body of the box
     * @param aLabelOk - label of the ok button
     * @param aLabelCancel - label of the cancel button
     * @param aOkCb - callback for Ok button
     * @param aCancelCb - callback for the cancel button
     * @return void 
     */
	public void Show( final String aTitle, final String aMessage, final String aLabelOk, final String aLabelCancel, 
			final DialogInterface.OnClickListener aOkCb, final DialogInterface.OnClickListener aCancelCb )
	{
		final MkRlNativeManager mgr = MkRlAppService.getNativeManager();
		final MkrNavMainAct activity = MkRlAppService.getMainActivity();
		
		Log.w( "Mkr", "Show_1 ..." );
		
		if ( activity == null )
		{
			Log.e( "Mkr", "Show_1 ...return" );
			return;
		}
		/*
		if ( mgr != null && mgr.IsNativeThread() )
		{
			final Runnable runMsgBox = new Runnable() {
				public void run() {					
					ShowRun( aTitle, aMessage, aLabelOk, aLabelCancel, aOkCb, aCancelCb );					
				}
			};
			activity.runOnUiThread( runMsgBox );
			// Wait on native thread 
			if ( mIsBlocking )
			{
				try 
				{
		    		synchronized( mInstance ) {
		    			mInstance.wait();
		    			mIsBlocking = false;
		    		}				
					Log.w( "Mkr", "Continue running the native thread" );
				}
				catch ( Exception ex ) {
					Log.e("MkrErr", "Error waiting for the message to finish" );
				}
			}
	
		}
		else */
		{
			ShowRun( aTitle, aMessage, aLabelOk, aLabelCancel, aOkCb, aCancelCb );
		}
	}
	
	 /*************************************************************************************************
     * Notify - notifies the message box to stop blocking the thread    
     * 
     */
	 public static void Notify()
	 {
			try 
			{
	    		synchronized( mInstance ) {
	    			mInstance.notify();
	    		}				
			}
			catch ( Exception ex ) {
				Log.w("Mkr", "Error notifying the message box: " + ex.getMessage() );
			}
	 }
	 
	 /*************************************************************************************************
     * ShowOk - Shows native message box with Ok button only. Static wrapper   
     * 
     */
	public static void ShowOk( final String aTitle, final String aMessage, final String aLabelOk, final DialogInterface.OnClickListener aOkCb )
	{
		mInstance.Show( aTitle, aMessage, aLabelOk, null, aOkCb, null );
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
	
    /*************************************************************************************************
     * ShowRun - Thread independent body of the show function  
     * @param see Show(...)
     * @return void 
     */
	/*
	private void ShowRun( byte[] aTitle, byte[] aMessage, byte[] aLabelOk, byte[] aLabelCancel, long aCbContext )
	{
        AlertDialog dlg = builder().create();
        dlg.setCancelable( false );
        
        if ( aLabelOk != null )
        {
        	dlg.setButton( new String( aLabelOk ), new MsgBoxOnClick( Mkr_MSG_BOX_RES_OK, aCbContext ) );
        }
        if ( aLabelCancel != null )
        {
        	dlg.setButton2( new String( aLabelCancel ), new MsgBoxOnClick( Mkr_MSG_BOX_RES_CANCEL, aCbContext ) );
        }
        dlg.setTitle( new String( aTitle ) );
        dlg.setMessage( new String( aMessage ) );
        
        dlg.show();
        
        // Makes http links clickable       
        TextView msgView = (TextView) dlg.findViewById(android.R.id.message);
        if ( msgView != null )
        {
        	Linkify.addLinks( msgView, Linkify.WEB_URLS );
        }
	}
	*/
	private void ShowRun( byte[] aTitle, byte[] aMessage, byte[] aLabelOk, byte[] aLabelCancel, long aCbContext )
	{
		MkrNavMainAct activity = MkRlAppService.getMainActivity();
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
        	dlg.setButton( new String( aLabelOk ), new MsgBoxOnClick( Mkr_MSG_BOX_RES_OK, aCbContext ) );
        }
        if ( aLabelCancel != null )
        {
        	dlg.setButton2( new String( aLabelCancel ), new MsgBoxOnClick( Mkr_MSG_BOX_RES_CANCEL, aCbContext ) );
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
		MkrNavMainAct ctx = MkRlAppService.getMainActivity(); 
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
	
	private void show_2(byte[] aTitle, byte[] aMessage)
	{
		MkrNavMainAct ctx = MkRlAppService.getMainActivity(); 
		final Dialog d = new Dialog(ctx,/*R.style.PauseDialog*/R.style.DialogSlideAnim); // with animation
		//d.requestWindowFeature(Window.FEATURE_NO_TITLE);
		//http://stackoverflow.com/questions/4757558/android-dialog-box-without-shadow-around-it
		//d.getWindow().clearFlags(android.view.WindowManager.LayoutParams.FLAG_DIM_BEHIND);
		
		Window window = d.getWindow();
		window.setBackgroundDrawableResource(android.R.color.transparent);
		window.requestFeature(window.FEATURE_NO_TITLE);
		
		d.setContentView(R.layout.my_dialog_layout);
		TextView title=(TextView)d.findViewById(R.id.mDlgTextTitle);
		title.setText(new String( aTitle )/*"Title I want to show"*/);
		
		TextView msg=(TextView)d.findViewById(R.id.mDlgTextMsg);
		msg.setText(new String(aMessage));
		
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
		MkrNavMainAct activity = MkRlAppService.getMainActivity();
		return ( new AlertDialog.Builder( activity, AlertDialog.THEME_HOLO_LIGHT ) );
	}
	
    /*************************************************************************************************
     *================================= Native methods section ================================= 
     * These methods are implemented in the native side and should be called after!!! the shared library is loaded
     */
	private native void InitMsgBoxNTV();									// Object initializer
	private native void MsgBoxCallbackNTV( int aRes, long aCbContext );		// Callback wrapper on the native layer
	
	/*************************************************************************************************
     *================================= Private interface section =================================
     * 
     */
    /*************************************************************************************************
     * MkrMsgBox - private constructor  
     * @return void 
     */
	private MkrMsgBox()
	{		
	}
	
    /*************************************************************************************************
     * class MsgBoxOnClick -click interface   
     */
	private class MsgBoxOnClick implements DialogInterface.OnClickListener
	{
		public MsgBoxOnClick( int aRes, long aCbContext )
		{
			mCbRes = aRes;
			mCbContext = aCbContext;
		}
    	public void onClick( DialogInterface dialog, int which )
    	{   
    		/* Cancel the dialog in any case */
    		dialog.cancel();
    		
    		MkRlNativeManager mgr = MkRlAppService.getNativeManager();
    		Runnable clickRun = new Runnable() 
    		{
				public void run() {

					MsgBoxCallbackNTV( mCbRes, mCbContext );
				}
			};
			/*
    		mgr.PostRunnable( clickRun );
    		if ( mIsBlocking )
    		{
	    		synchronized( mInstance ) {
	    			mInstance.notify();
	    		}
    		} */
    	}
    	private volatile long mCbContext;
    	private volatile int mCbRes;
	}
	
	
	/*************************************************************************************************
     *================================= Members section =================================
     */
	static private MkrMsgBox mInstance = null;					// Instance reference	
	private volatile boolean mIsBlocking = false;
	
	/*************************************************************************************************
     *================================= Constants section =================================
     */
	public static final int Mkr_MSG_BOX_RES_CANCEL = 0;
	public static final int Mkr_MSG_BOX_RES_OK = 1;
	
}
