package com.mkr.unidriver.inspection;

import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.inputmethod.EditorInfo;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import com.mkr.adapters.FormCategoryAdapter;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRow;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRowList;
import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.models.proto.CommPackets.PInspectionRow;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrMsgBox;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.MainDrawerActivity;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;

import com.mkr.utils.GConst;
import com.mkr.utils.Util;
import com.mkr.views.InspectionReportView;

public class InspCatFormActivity extends OnResultBaseActivity{

	int is_new=0,insID=0,inspType=0;
	PInspectionRow ir=null;
	//PInspectionCategoryRowList cat_list=null;
	int catListSize=0;
	FormCategoryAdapter formCatgAdapter = null; 
	ListView fLV = null;
	 int odo=0;
	 EditText odo_te=null;
	static InspCatFormActivity _this=null;
	@Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        _this=this;
	        Intent i=getIntent();
	        is_new=i.getIntExtra(GConst.INSP_FORM_NEW,-1);
	        insID=i.getIntExtra(GConst.INSP_FORM_ID,-1);
	        inspType=i.getIntExtra(GConst.INSP_FORM_TYPE,0);
	        String ttl="Inspection ";
	       
	        setContentView (R.layout.inspection_cat_form_layout);
	        LinearLayout insp_form_tb_ll = (LinearLayout)findViewById (R.id.insp_form_tb_id); 
	        TextView itv = (TextView)findViewById (R.id.insp_emp_name_id);
			TextView itv1 = (TextView)findViewById(R.id.insp_time_id);
	        if(is_new==1)
	        {
	        	//new inspection form
//	        	GConst.g_IR_Builder = PInspectionRow.newBuilder();
	        	ttl=ttl+"Form";
	        	 GConst.g_IR=null; 
	        	 itv.setText( GConst.CurrentDriverName);
					itv1.setText(Util.getTS());
					insp_form_tb_ll.setVisibility(View.VISIBLE);
	        }else{
	        	ir=GConst.g_IR;
	        	ttl=ttl+"Report";
	        	itv.setText( GConst.g_IR.getDrName());
				itv1.setText( Util.getTimeStamp(GConst.g_IR.getInsTime()) );
				insp_form_tb_ll.setVisibility(View.GONE);
	        }
	        odo_te =(EditText) findViewById (R.id.insp_odo_te_id);
			if (is_new == 1) {
				odo_te.setOnEditorActionListener(new OnEditorActionListener(){
					@Override
					public boolean onEditorAction(TextView arg0, int actionId,KeyEvent event) {
						if (actionId == EditorInfo.IME_ACTION_DONE)
						{
							Util.hideSoftKeyboard(_this);
							odo = Util.str2int (odo_te.getText().toString());
						}
						return true;
					}
				});
			} else {
				odo_te.setText(""+GConst.g_IR.getOdo());
				odo_te.setEnabled(false);
			}
	        SetMainTitles(ttl, R.layout.custom_ab_help);
	        
			fLV = (ListView)findViewById (R.id.list_category_form);
			//int inspType=GConst.
			/*cat_list*/catListSize= InspHelper.createInspection(is_new==1?-1:ir.getInsRID(),is_new,is_new==1?GConst.BoxID:ir.getBoxID(),inspType);                 //MkrNLib.Get().GetCategories(GConst.BoxID);
			//if(cat_list==null)
			//	return;
			
			formCatgAdapter = new FormCategoryAdapter (this,  R.layout.insp_category_list_row,/*null,*/ /*cat_list*//*.getListList()*/catListSize);
			fLV.setAdapter(formCatgAdapter);
			formCatgAdapter.notifyDataSetChanged ();
			fLV.setOnItemClickListener(adapter_listener);
	 }
	public static InspCatFormActivity Get(){return _this;}
	AdapterView.OnItemClickListener adapter_listener=new AdapterView.OnItemClickListener() {
		  @Override
		  public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

		    PInspectionCategoryRow row = InspHelper.GetCatRow(position);//(PInspectionCategoryRow )fLV.getItemAtPosition(position);
		    if(row!=null)
		    {
		    	int catId=row.getCID();
		    	GConst.insp_items_array = MkrNLib.Get ().GetInspectionItemsByCategoryId (catId);
              if(GConst.insp_items_array!=null)
              {
					if (GConst.insp_items_array.getListCount() == 1 && GConst.insp_items_array.getList(0).getHasChildren() == 0) { // => Category only has one InspectionItem with no children; let's lunch AddDefect screen
						GConst.currentInspectionItem = GConst.insp_items_array.getList(0);
						OpenDefectEditScreen (GConst.currentInspectionItem);
					}
					else
						StartActivityWithSlide (row.getDesc());
              }
				
		    }
		     
		  }
		};
	
	public boolean IsNew(){return is_new==1;}
	private void OpenDefectEditScreen(PInspectionItem ii_to_open)
	{
		Intent serverIntent = new Intent(this, AddDeffectActivity.class);
		this.startActivityForResult(serverIntent, GConst.REQUEST_DEFECT_ADD);
				//slide from right to left
		 overridePendingTransition(R.anim.pull_in_right,R.anim.push_out_left);
	}
	void StartActivityWithSlide (String desc)
	{
		Intent inte = new Intent (this, InspItemsActivity.class);
			inte.putExtra (GConst.IS_ATTACHMENT, true);
			inte.putExtra (GConst.INSP_PARENT_DESC,desc);
			startActivityForResult(inte, GConst.REQUEST_ATTACHMENT);
			//slide from right to left
			overridePendingTransition(R.anim.pull_in_right,R.anim.push_out_left);
	}
	 @Override
	 public void onResume() 
	 {
		 super.onResume ();
		
			        boolean rv=InspHelper.isContainsDefects();
					if (rv && formCatgAdapter != null)
						formCatgAdapter.notifyDataSetChanged ();
	 }
	
	 boolean cb_all=true;
	 public void CheckAllBtn(View v)
	 {
		 ImageButton chk_all_btn=(ImageButton)v;
		/* PInspectionCategoryRowList.Builder ll=PInspectionCategoryRowList.newBuilder(cat_list);
		 int size=ll.getListCount();
		 for (int i=0;i<size;i++) {
			 PInspectionCategoryRow row = ll.getList(i);
			 PInspectionCategoryRow.Builder b=row.toBuilder();
				if(cb_all)
					b.setIsChecked((int) CheckType.ManualChecked.ordinal());
				else
					b.setIsChecked ( CheckType.NotChecked.ordinal() );
				row=b.build();
				int xx=row.getIsChecked();
				if(xx>0)
				{
					
				}
				if(xx==0)
				{
					b.setIsChecked(1);
				}
				ll.setList(i, row);
			}
		 cat_list=ll.build();
		 formCatgAdapter.SetItems(cat_list);
		 formCatgAdapter.notifyDataSetChanged ();*/
			//BitmapDescriptor image = BitmapDescriptorFactory.FromResource(Resource.Drawable.polarbear);
		 int rv=InspHelper.setAllChecked(cb_all?1:0); //0,1 or 2
			if(rv==1)
			{
				 formCatgAdapter.notifyDataSetChanged ();
			}
			
			if (cb_all) {
				chk_all_btn.setImageResource(R.drawable.cb_off);
				cb_all=false;
			} else {
				chk_all_btn.setImageResource (R.drawable.cb_on);
				cb_all=true;
			}
			
	 }
	 public void CatScanBtn(View v){
		 
	 }
	 public void SaveIRBtn(View v)
	 {
		
		if (odo == 0) {
			MkrMsgBox bb=new MkrMsgBox();
    		bb.Show( "Odometer value", "You need to enter odometer value");
				return;
		}
		boolean all_checked=InspHelper.getAllChecked(odo)==1;
		if (!all_checked) {
			MkrMsgBox bb=new MkrMsgBox();
    		bb.Show( "Attention", "Make sure you have checked in all categories");
			return;
		}
		//InspHelper.saveInspection ();  if getAllChecked= return 1 -> inspection saved !!!!
		
		if(all_checked)
		{
			//HOSProcess.Get().ResetTowRule();           // attachment might be changed !!! this will affect Tow rule
			Finish_Activity (GConst.G_RESULT_OK);
		}
	 }
	 
	 public void ABBackBtn(View v)
	 {
		 InspCatFormActivity act = (InspCatFormActivity) MkrApp.GetCurrentOnTopActivity () ;
			if (act != null) {
				this.setResult (GConst.G_RESULT_OK);
				this.finish ();            //return to inspection Reports activity
				overridePendingTransition(R.anim.pull_in_left,R.anim.push_out_right);
			} else {
				//StartActivityWithSlide ();
			}
	 }
	 
}
