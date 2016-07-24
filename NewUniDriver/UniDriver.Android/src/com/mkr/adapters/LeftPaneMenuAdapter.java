package com.mkr.adapters;

import com.mkr.models.MenuItem;
import com.mkr.unidriver.R;
import com.mkr.unidriver.R.drawable;
import com.mkr.unidriver.R.id;
import com.mkr.unidriver.R.layout;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

public class LeftPaneMenuAdapter extends BaseAdapter
{
	
	final String[] titles=
		{
			"Work Orders",
			"Inspections",
			"HOS",
			"Navigation",
			"Messaging",
			"Settings",
			"Select Vehicle",
			"Help",
			"Logout"
		
		};
	/* icons must match LeftPane_Menu_Array from Arrays.xml  */
	final int[] menuIcons = {
		R.drawable.work_order,
		R.drawable.inspection,
		R.drawable.hos,
		R.drawable.navigation,
		R.drawable.messaging,
		R.drawable.settings,
		R.drawable.vehicle,
		R.drawable.help,
		R.drawable.logout
	};
	 
	 
	 public static final int  WorkOrders=0,
			Inspection=1,	
			HosFragment=2,
			Navigation=3,
			Messaging=4,	
			Settings=5,
			SwitchAsset=6,
			Help=7,
			Logout=8;
		
	
	private LayoutInflater inflater=null;
	Context ctx;
	private int selectedItem=0;
	public LeftPaneMenuAdapter( Context ctx)
	{
		this.ctx=ctx;
		inflater =(LayoutInflater)this.ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	}
	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return titles.length;
	}

	@Override
	public Object getItem(int arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public long getItemId(int arg0) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View v=convertView;
	    if(v==null)
		    v = inflater.inflate(R.layout.left_drawer_li, null);
	    String title=titles[position];
        int ic=menuIcons[position];
        
        TextView desc = (TextView) v.findViewById(R.id.left_menu_title);
		desc.setText(title);

		ImageView icon=(ImageView)v.findViewById(R.id.left_menu_image);
		icon.setImageResource(ic);
        
		if (position == selectedItem) {
			v.setBackgroundResource (R.drawable.menu_item_selected_background);
			
			// for focus on it
			int top = (v == null) ? 0 : v.getTop();

			((ListView)parent).setSelectionFromTop (position, top);
		}
		else { //<color name="left_pane_color">#FFBFE7F5</color>
			v.setBackgroundColor (/*Color.argb (0xFF,0xdc,0xef,0xf5)*/ctx.getResources().getColor(R.color.left_pane_color));
		}
		
		
		return v;
	}
	
	
	//http://stackoverflow.com/questions/3888015/androidto-set-an-item-as-selected-when-the-listview-opens
	public void setSelectedItem(int position) {
		selectedItem = position;
	}
	public int getSelectedItem() {
		return selectedItem;
	}
	

}
