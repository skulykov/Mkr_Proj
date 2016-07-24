package com.mkr.adapters;

import java.util.List;

import com.mkr.models.proto.CommPackets.PInspectionRow;
import com.mkr.utils.Util;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class InspectionReportAdapter extends ArrayAdapter<PInspectionRow> {

	Context ctx ;
    int resId=0;
	public InspectionReportAdapter(Context context, int textViewResourceId,
			List objects) {
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
		PInspectionRow item = getItem(position);
		//TextView tl = (TextView) row.findViewById(com.mkr.unidriver.R.id.txt_ir_id);
		//tl.setText(item.getInsRID());
		TextView tev = (TextView) row.findViewById(com.mkr.unidriver.R.id.txt_ir_date);
		tev.setText(Util.getTimeStamp(item.getInsTime()));
		
		TextView tt = (TextView) row.findViewById(com.mkr.unidriver.R.id.txt_ir_inspector);
		tt.setText(item.getDrName());
		
		
		return row;
	}
	
}
