package com.mkr.unidriver.route;

import android.os.Bundle;
import android.widget.ListView;
import android.widget.TextView;

import com.mkr.unidriver.OnResultBaseActivity;
import com.mkr.unidriver.R;


public class RouteInfoActivity extends  OnResultBaseActivity{

	private TextView header;
	RouteListAdapter ra=null;
	ListView lv=null;
	
	 @Override
	 public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.sf_route_info_layout);
	        
	        lv =  (ListView)findViewById(R.id.route_listView);
			header = (TextView)findViewById (R.id.txt_generalRouteInfo);
	 }
	 
}
