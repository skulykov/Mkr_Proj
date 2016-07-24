package com.mkr.adapters;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import com.mkr.models.HosRecapMenuItem;
import com.mkr.models.proto.CommPackets.PRecapRow;
import com.mkr.models.proto.CommPackets.PRecapRowList;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.R;
import com.mkr.views.HosRecapIndicatorsElement;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;


public class RightPaneHosRecapAdapter extends BaseAdapter  {

	Context ctx=null;
	private LayoutInflater inflater=null;
	private List<HosRecapMenuItem> items = null;
	
	void populateMenuItems()
	{
		
		for (int i = 0; i < 2; i++) {
			HosRecapMenuItem mi = new HosRecapMenuItem ();
			mi.pos = i;
			mi.count = i;
			items.add (mi);
		}
	}
	
	public RightPaneHosRecapAdapter(Context ctx)
	{
	  this.ctx=ctx;
	  items = new ArrayList<HosRecapMenuItem>();
	  inflater =(LayoutInflater)this.ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	  populateMenuItems();
	}
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return items.size();
	}

	@Override
	public /*Object*/HosRecapMenuItem getItem(int pos) {
		// TODO Auto-generated method stub
		return items.get(pos);
	}

	@Override
	public long getItemId(int pos) {
		// TODO Auto-generated method stub
		return pos;
	}

	@Override
	public View getView(int pos, View v, ViewGroup parent) {
		return getCustomView (pos, v, parent);
	}

	private  View getCustomView (int position, View convertView, ViewGroup parent)
	{
		View row = convertView;
		if (row == null) {
			if (position != 0) {
				row = inflater.inflate(R.layout.rdi_hos_recap, null);
			} else {
				row = inflater.inflate(R.layout.rdi_hos_times, null);
				LinearLayout v1 = (LinearLayout)row.findViewById (R.id.hos_recap_times_view);
				HosRecapIndicatorsElement hgv = new HosRecapIndicatorsElement (ctx);
				v1.addView (hgv);
			}
		}

		if (position == 0) {
			
			return row;
		}
		if (row != null) {
			//this might redraw every scroll movement -> not sure if this olace is right for the code
			ScrollView _scroller = (ScrollView) row.findViewById (R.id.hos_recap_scroller);
			ViewGroup _track = (ViewGroup) row.findViewById (R.id.hos_recap_tracks);
			if (_track != null) {
				
				_track.removeAllViews ();
				int _insertPos = 0;
				PRecapRowList rlist = MkrNLib.Get().GetRecapRowList();
				if (rlist != null && rlist.getListCount() > 0) {
					for (PRecapRow rr : rlist.getListList()) {
						View container = inflater.inflate (R.layout.rdi_hos_recap_item, null);

						if (container != null) {
							TextView text = (TextView) container.findViewById (R.id.hos_r_date);
							text.setText(new Date(rr.getDate()).toString());// = TimeUtil.GetLongTimeStr(rr.Date);

							text = (TextView) container.findViewById (R.id.hos_r_title1);
							text.setText("On Duty: " + convertTT (rr.getToday())+"--"+"Total: " + convertTT (rr.getCycleTotal()));
							text = (TextView) container.findViewById(R.id.hos_r_title2);
							text.setText("Avail: "+convertTT (rr.getCycleAvailable())+"--"+rr.getCycle());
							_track.addView (container, _insertPos++);
						}
					}
				}
				
			}
		}
		return row;
	}
	String convertTT(int v)
	{
		int h = v / 60;
		int min = v - (h * 60);
		String str = String.format ("%02d:%02d", h, min);
		return str;
	}
	
}
