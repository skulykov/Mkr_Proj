package com.mkr.unidriver;

//import com.google.zxing.client.android.CaptureActivity;


import com.mkr.adapters.VehicleArrayAdapter;
import com.mkr.models.proto.CommPackets.PBoxIdVehicleRow;
import com.mkr.models.proto.CommPackets.PBoxIdVehicleRowList;
import com.mkr.service.MkrNLib;
import com.mkr.utils.GConst;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;


//http://stackoverflow.com/questions/4782543/integration-zxing-library-directly-into-my-android-application
public class VehiclesActivity extends Activity
{
	public int SCANNER_REQUEST_CODE = 123;
	VehicleArrayAdapter boxIdVehiclesArrayAdapter = null; 
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//mLog.w("WL","ON Create..MainActivity..");
		
		setContentView(R.layout.vehicles_layout);
		
		RadioGroup rg=(RadioGroup) findViewById(R.id.radio_group_search_types);
		RadioButton rb= (RadioButton) rg.getChildAt(0);
		rb.setChecked(true);
		
		final ListView lv=(ListView)findViewById(R.id.list_view_vehicles);
		
		PBoxIdVehicleRowList vList=MkrNLib.Get().GetVehicles();
		if(vList!=null&&vList.getVitemList()!=null&&vList.getVitemList().size()>0)
		{
			boxIdVehiclesArrayAdapter = new VehicleArrayAdapter(this, R.layout.vehicle_item_layout,vList.getVitemList());
			lv.setAdapter(boxIdVehiclesArrayAdapter);
			lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
				  @Override
				  public void onItemClick(AdapterView<?> arg0, View v, int position, long arg3) {

					  	PBoxIdVehicleRow ii = (PBoxIdVehicleRow )lv.getItemAtPosition(position);
					    if(ii!=null)
					    {
					    	
					    }
				  }
				});				
		}
		
		final EditText etSearchV = (EditText)findViewById (R.id.txt_search_veh_name);
		etSearchV.addTextChangedListener(new TextWatcher() {

			   @Override
			   public void afterTextChanged(Editable s) {}

			   @Override    
			   public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

			   @Override    
			   public void onTextChanged(CharSequence s, int start,int before, int count) {
			      if(s.length() != 0)
			    	  etSearchV.setText("");
			   }
			  });

		
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
