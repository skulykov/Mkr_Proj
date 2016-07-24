package com.mkr.unidriver.inspection;

import java.util.List;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import com.mkr.adapters.FormInspectionItemAdapter;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRow;
import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.models.proto.CommPackets.PInspectionItemList;
import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;
import com.mkr.utils.GConst;

public class InspItemsActivity extends OnResultBaseActivity {
	boolean is_att=false;
	String desc="";
	ListView iiLV=null;
	PInspectionItemList curInspItemArray=null;
	FormInspectionItemAdapter formInspItemAdapter = null;
	@Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        Intent i=getIntent();
	        is_att=i.getBooleanExtra(GConst.IS_ATTACHMENT,false);
	        desc=i.getStringExtra(GConst.INSP_PARENT_DESC);
	        	        
	        setContentView (R.layout.inspection_items_form_layout);
	        iiLV=(ListView)findViewById(R.id.list_insp_form);
	        GConst.currentInspectionItem = null;
	        curInspItemArray = GConst.insp_items_array;
	        if (curInspItemArray != null && curInspItemArray.getListCount() > 0) {
				if (curInspItemArray.getListCount() == 1 && curInspItemArray.getList(0).getHasChildren() == 0) { // => Category only has one InspectionItem with no children; let's lunch AddDefect screen
					GConst.currentInspectionItem = curInspItemArray.getList(0);
					OpenDefectEditScreen (GConst.currentInspectionItem);
				} else {
				    formInspItemAdapter = new FormInspectionItemAdapter (this, R.layout.inspection_item_list_row, curInspItemArray.getListList());
					iiLV.setAdapter(formInspItemAdapter);
					//formInspItemAdapter.notifyDataSetChanged();
					iiLV.setOnItemClickListener(new AdapterView.OnItemClickListener() {
						  @Override
						  public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

						    PInspectionItem ii = (PInspectionItem )iiLV.getItemAtPosition(position);
						    if(ii!=null&&ii.getHasChildren()==1)
						    {
						    	int inspId=ii.getIID();
						    	GConst.insp_items_array =  null;//InspectionItemDB.Get().GetInspectionItemChildren(inspId).ToArray<InspectionItem>();
								String desc = ii.getDefect();
								StartActivityWithSlide (desc);
						    }else{
									GConst.currentInspectionItem = ii;
									OpenDefectEditScreen(GConst.currentInspectionItem);
								}
						     
						  }
						});				
					}
				
	        }
	        SetMainTitles(desc , R.layout.custom_ab_help);
	}
	@Override
	 public void onResume() {
	        super.onResume();
	        if(GConst.currentInspectionItem!=null)
	        {
		        boolean rv=InspHelper.processItemDefectsInSubCats(GConst.currentInspectionItem.getIID());
				if (rv && formInspItemAdapter != null)
					formInspItemAdapter.notifyDataSetChanged ();
	        }
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
	 public void ABBackBtn(View v)
	 {
		 setResult (GConst.G_RESULT_OK);
		 finish();
		 overridePendingTransition(R.anim.pull_in_left,R.anim.push_out_right);
	 }
	 
	private void OpenDefectEditScreen(PInspectionItem ii_to_open) {
		
		Intent serverIntent = new Intent(this, AddDeffectActivity.class);
		this.startActivityForResult(serverIntent, GConst.REQUEST_DEFECT_ADD);
				//slide from right to left
		 overridePendingTransition(R.anim.pull_in_right,R.anim.push_out_left);
	}
	
}
