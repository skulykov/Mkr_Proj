package com.mkr.adapters;

import java.util.Date;
import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.mkr.models.proto.CommPackets.PWorkOrderItem;
import com.mkr.models.proto.CommPackets.PWorkOrderList;
import com.mkr.unidriver.workorders.WorkOrdersActivity;
import com.mkr.utils.DateUtil;
import com.mkr.utils.Util;

public class WorkOrdersAdapter extends BaseAdapter
{
    WorkOrdersActivity act=null;
    int resId=0;
    PWorkOrderList aList=null;
	public WorkOrdersAdapter(Context ctx, int textViewResourceId,PWorkOrderList aList) {
		act=(WorkOrdersActivity )ctx;
		resId=textViewResourceId;
		this.aList=aList;
	}
	public void SetAList(PWorkOrderList list)
	{
		aList=list;
	}
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View row = convertView;
		if (row == null) {
			LayoutInflater inflater = act.getLayoutInflater();
			row = inflater.inflate(this.resId, parent,false);
		}
		PWorkOrderItem item = (PWorkOrderItem) getItem(position);
		TextView tl = (TextView) row.findViewById(com.mkr.unidriver.R.id.txtRptDescription);
		tl.setText(item.getName());
		TextView tev = (TextView) row.findViewById(com.mkr.unidriver.R.id.txtRptTimeStamp);
		tev.setText(DateUtil.getTS(item.getStartTime()));
		return row;
	}
	@Override
	public int getCount() {
		return  aList==null?0:aList.getWoItemsCount();
	}
	@Override
	public Object getItem(int pos) {
		return aList==null?null:aList.getWoItems(pos);
	}
	@Override
	public long getItemId(int position) {
		return position;
	}
}
