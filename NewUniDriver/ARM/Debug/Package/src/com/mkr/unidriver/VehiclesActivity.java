package com.mkr.unidriver;

//import com.google.zxing.client.android.CaptureActivity;


import com.mkr.utils.GConst;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;


//http://stackoverflow.com/questions/4782543/integration-zxing-library-directly-into-my-android-application
public class VehiclesActivity extends Activity
{
	public int SCANNER_REQUEST_CODE = 123;
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//mLog.w("WL","ON Create..MainActivity..");
		
		setContentView(R.layout.vehicles_layout);
		
		
	}
	public void CancelThisAct(View v)
	{
		setResult(GConst.ActivityCanceled);
		finish ();
	}
	public void CreateNewVeh(View v)
	{
		// Launch the AddDeffectActivity to require comment and confirm addition to InspectionReport or clear the inspection
			Intent serverIntent = new Intent(this, CreateVehicleActivity.class);
			this.startActivityForResult(serverIntent, GConst.REQUEST_DEFECT_ADD);

	}
	
	public void scanQRCode(View v) {
	   // IntentIntegrator integrator = new IntentIntegrator(VehiclesActivity.this);
	    //integrator.initiateScan(IntentIntegrator.QR_CODE_TYPES);
		
	/*	Intent intent = new Intent(VehiclesActivity.this, CaptureActivity.class);
		intent.setPackage("com.google.zxing.client.android"); 
		intent.putExtra("SCAN_MODE", "SCAN_MODE");
		startActivityForResult(intent, SCANNER_REQUEST_CODE); */
	}
/*
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent intent) {
	    IntentResult result = 
	        IntentIntegrator.parseActivityResult(requestCode, resultCode, intent);
	    if (result != null) {
	        String contents = result.getContents();
	        if (contents != null) {
	            showDialog(R.string.result_succeeded, result.toString());
	        } else {
	            showDialog(R.string.result_failed,
	                getString(R.string.result_failed_why));
	        }
	    }
	}
*/
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent intent) {

		if (requestCode == SCANNER_REQUEST_CODE) {
			// Handle scan intent
			if (resultCode == Activity.RESULT_OK) {
				// Handle successful scan
				String contents = intent.getStringExtra("SCAN_RESULT");
				String formatName = intent.getStringExtra("SCAN_RESULT_FORMAT");
				byte[] rawBytes = intent.getByteArrayExtra("SCAN_RESULT_BYTES");
				int intentOrientation = intent.getIntExtra("SCAN_RESULT_ORIENTATION", Integer.MIN_VALUE);
				Integer orientation = (intentOrientation == Integer.MIN_VALUE) ? null : intentOrientation;
				String errorCorrectionLevel = intent.getStringExtra("SCAN_RESULT_ERROR_CORRECTION_LEVEL");

				//tvScanResults.setText(contents + "\n\n" + formatName);
				showDialog(R.string.result_succeeded, contents);

			} else if (resultCode == Activity.RESULT_CANCELED) {
				// Handle cancel
				 getString(R.string.result_failed_why);
			}
		} else {
			// Handle other intents
		}

	}
	private void showDialog(int title, CharSequence message) {
	    AlertDialog.Builder builder = new AlertDialog.Builder(this);
	    builder.setTitle(title);
	    builder.setMessage(message);
	    builder.setPositiveButton(R.string.ok_button, null);
	    builder.show();
	}
}
