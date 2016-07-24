package com.mkr.adapters;

import java.util.List;

import com.mkr.models.proto.CommPackets.PInspectionItem;
import com.mkr.models.proto.CommPackets.PInspectionRow;
import com.mkr.unidriver.R;
import com.mkr.unidriver.inspection.InspHelper;
import com.mkr.utils.GConst;
import com.mkr.utils.Util;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class FormInspectionItemAdapter extends ArrayAdapter<PInspectionItem> 
{
	Context ctx ;
    int resId=0;

	public FormInspectionItemAdapter(Context context, int textViewResourceId,
			List<PInspectionItem> objects) {
		super(context, textViewResourceId, objects);
		this.ctx=context;
		resId=textViewResourceId;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View row = convertView;
		if (row == null) {
			LayoutInflater inflater = ((Activity) ctx).getLayoutInflater();
			row = inflater.inflate(this.resId, parent,false);
		}
		PInspectionItem item = getItem(position);
		//If hasChild show a forward icon on the left side of the row
		if (item.getHasChildren()==1) {
			ImageView iv=(ImageView)row.findViewById (R.id.img_forward_icon);
			iv.setVisibility(View.VISIBLE);
		}
		
		TextView rowAttTextView =(TextView) row.findViewById (R.id.txt_ii_att_id);
		rowAttTextView.setText(item.getAttID());

		TextView rowBodyTextView =(TextView) row.findViewById (R.id.inspection_item_body);
		rowBodyTextView.setText(item.getDefect());
		
		if(item.getDefLevel() == 3/*DefLevels.MajorDefect*/)
			rowBodyTextView.setTypeface(null,  Typeface.BOLD_ITALIC);

		TextView rowIdTextView = (TextView)row.findViewById (R.id.txt_insp_item_id);
		rowIdTextView.setText(" "+ item.getIID());
		
		//If the listView is being generated for globalReport set textview colors based on defects list of report
		
			   int rv=InspHelper.getIIColor(item.getIID());
			   int color=Color.BLACK;
			   if(rv>0)
				   color=Color.RED;
				rowBodyTextView.setTextColor (color);

			//If there is attachments show attachment icon
			ImageView iv1=(ImageView)row.findViewById (R.id.img_ic_media_attachment);
		    if (/*item.getMediaFilesList()!=null&&item.getMediaFilesCount()>0*/rv == 3)
		    {
		    	//String str=String.format("rv==3   iid=%d  pos=%d desc=%s",item.getIID(),position,item.getDefect() );
		    	// Log.d("IIA",str);
		    	
				iv1.setVisibility(View.VISIBLE);
		    }else
		    	iv1.setVisibility(View.GONE);
		
		
		
		return row;
	}
	/*
	int getColor(PInspectionItem item)
	{
		int color=Color.BLACK;

		//if (editable) {
		//	bool rv=InspHelper.processItemDefectsInSubCats(this.item);
		//	if(rv)
		//		color = Theme.Color_Defect;
		//} else 
		{
			boolean changed = false,cleared=true;
			List<PInspectionItem> arDef =GConst.g_IR.getDefectsList();
			int CategoryID = item.getCID();

				boolean rv1=InspHelper.processItemDefectsInSubCats(item,arDef,CategoryID);
				if (rv1) {
					changed = true;
					if (item.getClrd()==""||item.getClrd() == "0")
						cleared = false;
				}

			if (changed&&cleared)
				color = Color.MAGENTA;
			else
				if(changed)
					color= Color.RED;
		}
		return color;
	}*/
}
