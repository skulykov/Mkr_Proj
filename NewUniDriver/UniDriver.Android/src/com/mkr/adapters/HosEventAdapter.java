package com.mkr.adapters;

import java.util.Date;
import java.util.List;

import com.mkr.models.proto.CommPackets.PTimeLogRow;
import com.mkr.unidriver.R;
import com.mkr.unidriver.hos.HosHelper;
import com.mkr.unidriver.hos.dailylogs.HosDailyLogsPagerActivity;
import com.mkr.utils.DateUtil;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class HosEventAdapter extends ArrayAdapter<PTimeLogRow> 
{
	HosDailyLogsPagerActivity ctx;
	int resource=0;
	public HosEventAdapter(Context context, int resource, List objects) {
		super(context, resource,  objects);
		this.ctx=(HosDailyLogsPagerActivity)context;
		this.resource=resource;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View row = convertView;
		PTimeLogRow tlr = getItem(position);
		
		if (row == null) {
			LayoutInflater inflater = ctx.getLayoutInflater();
			row = inflater.inflate(this.resource, parent,false);
		}
		
		TextView tl = (TextView) row.findViewById(R.id.txt_her_time);
		tl.setText(DateUtil.getTS(tlr.getLogTime()));
		TextView tev = (TextView) row.findViewById(R.id.txt_her_event);
		tev.setText(HosHelper.GetEventStr(tlr.getEvent()));
		
		TextView tv = (TextView) row.findViewById(R.id.txt_her_location);
		tv.setText(tlr.getAddr());
		
		tv = (TextView) row.findViewById(R.id.txt_her_comment);
		tv.setText(tlr.getComment());
		
		
		ViewGroup rla = (ViewGroup) row.findViewById(R.id.rel_la_hos_events_id);
		if(tlr.getType()==1)  // do not show auto type
		{
			
			rla.setVisibility(View.GONE);
		}
		else
			rla.setVisibility(View.VISIBLE);
		return row;
	}

}
