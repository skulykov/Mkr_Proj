package com.mkr.unidriver.hos;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

import android.app.Activity;
import android.app.DatePickerDialog;
import android.app.DatePickerDialog.OnDateSetListener;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.CheckBox;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import com.mkr.service.MkrMsgBox;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.DateUtil;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;

public class EmailActivity  extends OnResultBaseActivity implements OnClickListener{

	private Date fromDate = new Date();
	private Date toDate = new Date();
	private Date oldestDate = DateUtil.addDays(fromDate,-14);
	private Date newestDate = DateUtil.addDays(fromDate,1);

	//private ImageButton btnSendEmail;
	private TextView txtFromDate, txtToDate;
	EditText txtEmail;
	
	CheckBox chkLogSheet,chkPreTrip,chkPostTrip;
	
	DatePickerDialog fromDatePickerDialog = null,toDatePickerDialog=null; 
	//private SimpleDateFormat dateFormatter;
	Activity _this=null;
	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        _this=this;
	        setContentView(R.layout.email_send_layout);
	        
	      //  dateFormatter = new SimpleDateFormat("dd-MM-yyyy", Locale.US);
	        SetMainTitles("Hos Email",R.layout.action_bar_title);
	        
	        txtFromDate = (EditText)findViewById (R.id.txt_email_from_date);
			txtFromDate.setText (DateUtil.getDateTS(fromDate));
			
			txtToDate = (EditText)findViewById (R.id.txt_email_to_date);
			txtToDate.setText( DateUtil.getDateTS(toDate));
			
			setDateTimeField();   // sets the listeners
			
			
			txtEmail =(EditText) findViewById (R.id.txt_email_address);
			txtEmail.setOnEditorActionListener(new OnEditorActionListener() {
			   
				@Override
				public boolean onEditorAction(TextView v, int actionId,	KeyEvent ev) {
					boolean rv=false;
					int result = actionId & EditorInfo.IME_MASK_ACTION;
					if (result==EditorInfo.IME_ACTION_DONE) {
						//this hides the keyboard
						Util.hideSoftKeyboard(_this);
			            rv=true;
			        }
					
					return rv;
				}
			});
			
			chkLogSheet = (CheckBox)findViewById (R.id.chk_email_log_sheet);
			chkPreTrip = (CheckBox)findViewById (R.id.chk_email_pre_trip_insp);
			chkPostTrip = (CheckBox)findViewById (R.id.chk_email_post_trip_insp);
			
			
	 }
	 
	 private void setDateTimeField() {
		 txtFromDate.setOnClickListener(this);
		 txtToDate.setOnClickListener(this);
	        
	        Calendar newCalendar = Calendar.getInstance();
	        fromDatePickerDialog = new DatePickerDialog(this, new OnDateSetListener() {
	 
	            public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth) {
	                Calendar newDate = Calendar.getInstance();
	                newDate.set(year, monthOfYear, dayOfMonth);
	                fromDate=newDate.getTime();
	                txtFromDate.setText(DateUtil.getDateTS(fromDate));
	            }
	 
	        },newCalendar.get(Calendar.YEAR), newCalendar.get(Calendar.MONTH), newCalendar.get(Calendar.DAY_OF_MONTH));
	        
	        toDatePickerDialog = new DatePickerDialog(this, new OnDateSetListener() {
	 
	            public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth) {
	                Calendar newDate = Calendar.getInstance();
	                newDate.set(year, monthOfYear, dayOfMonth);
	                toDate=newDate.getTime();
	                txtToDate.setText(DateUtil.getDateTS(toDate));
	            }
	 
	        },newCalendar.get(Calendar.YEAR), newCalendar.get(Calendar.MONTH), newCalendar.get(Calendar.DAY_OF_MONTH));
	    }
	 
	 @Override
	 public void onClick(View view) {
	        if(view == txtFromDate) {
	            fromDatePickerDialog.show();
	        } else if(view == txtToDate) {
	            toDatePickerDialog.show();
	        }        
	    }


	 public void FromLessBtn(View v)
	 {
		 Date newDateTime = DateUtil.addDays(fromDate,-1);
		 long ts=newDateTime.getTime();
			if (ts >= oldestDate.getTime() && ts <= newestDate.getTime() && ts <= toDate.getTime()) { 
				fromDate = newDateTime;
				txtFromDate.setText( DateUtil.getDateTS(fromDate));
			} 
	 }
	 public void FromMoreBtn(View v)
	 {
		 Date newDateTime = DateUtil.addDays(fromDate,1);
		 long ts=newDateTime.getTime();
			if (ts >= oldestDate.getTime() && ts <= newestDate.getTime() && ts <= toDate.getTime()) { 
				fromDate = newDateTime;
				txtFromDate.setText( DateUtil.getDateTS(fromDate));
			}
	 }
	 public void ToLessBtn(View v)
	 {
		 Date newDateTime = DateUtil.addDays(toDate,-1);
		 long ts=newDateTime.getTime();
			if (ts >= oldestDate.getTime() && ts <= newestDate.getTime() && ts >= fromDate.getTime()) { 
				toDate = newDateTime;
				txtToDate.setText( DateUtil.getDateTS(toDate));
			}
	 }
	 public void ToMoreBtn(View v)
	 {
		 Date newDateTime = DateUtil.addDays(toDate,1);
		 long ts=newDateTime.getTime();
		 if (ts >= oldestDate.getTime() && ts <= newestDate.getTime() && ts >= fromDate.getTime())
		 {
			 toDate = newDateTime;
			 txtToDate.setText( DateUtil.getDateTS(toDate));
		 }
	 }
	 
	 
	 public void CancelBtn(View v)
	 {
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 public void SaveBtn(View v)
	 {
		 String eaddr=txtEmail.getText().toString();
		 Date from=fromDate,to=toDate;
		 if(eaddr.isEmpty())
		 {
			 MkrMsgBox bb=new MkrMsgBox();
	    		bb.Show( "Email address", "You need to enter email address");
				return;
		 }
		 	String fromStr=DateUtil.getDateTS(from);
			String toStr=DateUtil.getDateTS(to);
			String types=(chkLogSheet.isChecked() ? "0,":"") + (chkPreTrip.isChecked() ? "1,":"") + (chkPostTrip.isChecked() ? "2,":"");

			String[] s1=eaddr.split(";");
			int len=s1.length;
			if(len>0)
			{
				for(int i=0;i<len;i++)
				{
					if(!s1[i].isEmpty())
					if(!Util.isValidEmailAddress(s1[i]))
					{
						 MkrMsgBox bb=new MkrMsgBox();
				    	 bb.Show( "Verify email address", "Please make sure you entered a valid email address");
						return;
					}
				}
			}
			
			if(!validateSignTimeLog(from,to))
			{
				MkrMsgBox bb=new MkrMsgBox();
		    	 bb.Show( "Verify TimeLog Sign", "Please make sure you signed all timelogs");
				return;
			}
			
		 String str=eaddr+"|"+fromStr+"|"+toStr+"|"+types;
		 MkrNLib.Get().sendEmail(str.getBytes(),str.length());  //string.getBytes(Charset.forName("UTF-8"));
		 Finish_Activity(GConst.G_RESULT_OK);
	 }
	 boolean validateSignTimeLog(Date from,Date to)
		{
			boolean rv = true;
			/*EventStates evts = null; 
			if(from.Date==to.Date)
				evts=HosEvents.getEventstates (to);
			else
				evts=HosEvents.getEventstates (from.Date, to.Date);
			if (evts!=null&&evts.getCount () > 0) {
				List<TimeLogRow> list = evts.getList ();
				foreach (TimeLogRow tlr in list) {
					if (!tlr.Signed) {
						rv = false;
						break;
					}
				}
			}*/
			return rv;
		}
}
