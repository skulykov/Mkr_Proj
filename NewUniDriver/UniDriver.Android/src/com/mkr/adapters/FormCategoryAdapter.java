package com.mkr.adapters;

import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;

import com.mkr.models.proto.CommPackets.PInspectionCategoryRow;
import com.mkr.models.proto.CommPackets.PInspectionCategoryRowList;
import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.models.proto.CommPackets.PInspectionItemList;
import com.mkr.models.proto.CommPackets.PInspectionRow;
import com.mkr.unidriver.R;
import com.mkr.unidriver.inspection.CheckType;
import com.mkr.unidriver.inspection.InspCatFormActivity;
import com.mkr.unidriver.inspection.InspHelper;
import com.mkr.utils.GConst;

public class FormCategoryAdapter extends  BaseAdapter  //ArrayAdapter<PInspectionCategoryRow>
{
	/*List<PInspectionCategoryRow>*//*PInspectionCategoryRowList items=null;*/ 
	/*Context*/InspCatFormActivity act ;
    int resId=0;
    int count=0;
	public FormCategoryAdapter(Context context, int textViewResourceId,
			/*List<PInspectionCategoryRow>*//*PInspectionCategoryRowList objects*/int size) {
		//super(context, textViewResourceId, objects);
		//items=objects;
		count=size;
		this.act=(InspCatFormActivity)context;
		resId=textViewResourceId;
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View row = convertView;
		if (row == null) {
			LayoutInflater inflater = ((Activity) act).getLayoutInflater();
			row = inflater.inflate(this.resId, parent,false);
		}
		PInspectionCategoryRow icr =InspHelper.GetCatRow(position);// (PInspectionCategoryRow) getItem(position);
		if(icr==null)
			return row;
		
		TextView rowAttTextView =(TextView) row.findViewById (R.id.category_id);
		rowAttTextView.setText(""+icr.getCID());

		TextView rowBodyTextView =(TextView) row.findViewById (R.id.category_body);
		rowBodyTextView.setText(icr.getDesc());
		
		ImageView qrCodeImage = (ImageView)row.findViewById (R.id.img_qr);
		TextView barcodeTextView = (TextView)row.findViewById (R.id.txt_barcode);

		if (icr.getBarCodeID()!=-1) {
			qrCodeImage.setVisibility(View.VISIBLE);
			barcodeTextView.setText(""+icr.getBarCodeID());
		} else {
			qrCodeImage.setVisibility(View.GONE);
			barcodeTextView.setText( "");
		}
		
		CheckBox chkGroup =(CheckBox) row.findViewById (R.id.chk_category);		
		if (icr.getIsChecked() ==CheckType.ManualChecked.ordinal()) {
			chkGroup.setChecked(true);
		} else if(icr.getIsChecked() ==CheckType.ScannedChecked.ordinal()){
			chkGroup.setChecked(true);
			//Reset the isChecked type back to ScannedChecked as the above line causes the ChekcedChange event handler to execut and result in manualchecked
			//icr.toBuilder().setIsChecked( CheckType.ScannedChecked.ordinal());
			if(qrCodeImage.getVisibility() == View.VISIBLE)
				qrCodeImage.setImageDrawable (act.getResources().getDrawable (R.drawable.ic_qr_checked));
		} else {
			chkGroup.setChecked(false);
		}
		
		
		//If the listView is being generated for globalReport set textview colors based on defects list of report
		
			   int c=icr.getColor();//InspHelper.getCatColor(icr.getCID(),-1);  //getColor(icr,"");
			   rowBodyTextView.setTextColor (c);

		
		return row;
	}
	/*
	int getColor(PInspectionCategoryRow row,String attID )
	{
		int color=Color.BLACK;

		//if (editable) {
		//	bool rv=InspHelper.processItemDefectsInSubCats(this.item);
		//	if(rv)
		//		color = Theme.Color_Defect;
		//} else 
		{
			boolean changed = false,cleared=true;
			List<PInspectionItem> arDef =null;
			if (!act.IsNew()&&GConst.g_IR != null)
				arDef=GConst.g_IR.getDefectsList();
			else if (act.IsNew()&&GConst.g_IR_Builder != null)
				arDef=GConst.g_IR_Builder.getDefectsList();
			int CategoryID = row.getCID();
			
			PInspectionItemList iiL=row.getItems();
			int count=iiL.getListCount();
			for (int i=0;i<count;i++) 
			{
				PInspectionItem item = iiL.getList(i);
				boolean rv=InspHelper.processItemDefectsInCats(item,arDef,CategoryID,attID);
				if (rv) {
					changed = true;
					if (item.getClrd()==""||item.getClrd() == "0")
						cleared = false;
				}
			}
				

			if (changed&&cleared)
				color = Color.MAGENTA;
			else
				if(changed)
					color= Color.RED;
		}
		return color;
	}*/

	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return count;//items==null?0:items.getListCount();
	}

	@Override
	public Object getItem(int pos) {
		return null;//items==null?null:items.getList(pos);
	}

	@Override
	public long getItemId(int pos) {
		return pos;
	}
	
}
