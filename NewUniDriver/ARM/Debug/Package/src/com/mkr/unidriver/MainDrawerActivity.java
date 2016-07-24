package com.mkr.unidriver;

import com.mkr.adapters.LeftPaneMenuAdapter;
import com.mkr.adapters.RightPaneHosRecapAdapter;
import com.mkr.fragments.HosViewFragment;
import com.mkr.fragments.InspectionFragment;
import com.mkr.fragments.MessagingFragment;
import com.mkr.fragments.SettingsFragment;
import com.mkr.fragments.WorkOrdersFragment;
import com.mkr.service.MkrApp;
import com.mkr.service.MkrNLib;
import com.mkr.unidriver.inspection.InspectionReportActivity;
import com.mkr.utils.GConst;
import com.mkr.utils.Screen;

import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.ActionBarDrawerToggle;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.widget.DrawerLayout;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;

interface CallBack {
    void ImageButtonMenuClick();
}


public class MainDrawerActivity extends BaseDrawerActivity implements CallBack
{
	private DrawerLayout _drawer=null;
	private ListView left_drawerList,right_drawerList;
	LeftPaneMenuAdapter _menuAdapter = null;
	RightPaneHosRecapAdapter hos_recap_adapter=null;
	public static Fragment current_fragment = null;
	
	ActionBarDrawerToggle actionBarDrawerToggle=null;
	
		
	 @Override
	    public void onCreate(Bundle bundle) {
	        super.onCreate(bundle);
	        setContentView(R.layout.main_drawer_layout);
	        LayoutInflater mInflater = this.getLayoutInflater();
	        _drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
			left_drawerList =(ListView) findViewById(R.id.left_drawer);
			right_drawerList =(ListView) findViewById(R.id.right_drawer);
	        
			float screenW=Screen.getScreenWidth (this);
			ViewGroup.LayoutParams pars=right_drawerList.getLayoutParams();
			pars.width=Screen.toPx(this,screenW*7/8);
			right_drawerList.setLayoutParams(pars);
			
			_drawer.setDrawerShadow(R.drawable.drawer_shadow_dark,(int) Gravity.START);
			_drawer.setDrawerShadow(R.drawable.drawer_shadow_dark,(int) Gravity.END);
			
			_menuAdapter = new LeftPaneMenuAdapter (this);
			left_drawerList.setAdapter( _menuAdapter );
			
			hos_recap_adapter=new RightPaneHosRecapAdapter (this);
			right_drawerList.setAdapter(hos_recap_adapter);
			
			left_drawerList.setOnItemClickListener (listener);
			MkrApp.SetMainActivity(this);
			
			 actionBarDrawerToggle = new ActionBarDrawerToggle(
		                this,
		                _drawer,
		                R.drawable.ic_drawer_light,// toolbar,
		                R.string.draweropen,
		                R.string.drawerclose
		        )

		        {
		            public void onDrawerClosed(View view)
		            {
		                super.onDrawerClosed(view);
		                invalidateOptionsMenu();
		                syncState();
		            }

		            public void onDrawerOpened(View drawerView)
		            {
		                super.onDrawerOpened(drawerView);
		                invalidateOptionsMenu();
		                syncState();
		            }
		           
		      };
		      _drawer.setDrawerListener(actionBarDrawerToggle);
		      
		      if (bundle == null)
					SelectItem(0);
			
		      SetMainActionBarView ("UniDriver",this);
	 }
	 OnItemClickListener listener = new OnItemClickListener (){
		@Override
		public void onItemClick(AdapterView<?> parent, View v, int position,long id) {
			SelectItem(position);
			
		}

	 };
	 
	 public void ImageButtonMenuClick(/*object o, EventArgs arg*/)
		{
			//ProcessCustomOptionsMenu (sender);
			if(_drawer.isDrawerOpen(right_drawerList))
				_drawer.closeDrawer(right_drawerList);
			else{
				_drawer.openDrawer(right_drawerList);
				right_drawerList.invalidateViews(); // try to refresh GUI content

			}
		}
	 
	 void SelectItem(int position)
	 {
		 Fragment fragment = null;
			switch(position)
			{	
				case (int)LeftPaneMenuAdapter.WorkOrders:
					fragment=new WorkOrdersFragment();
					break;
				case (int)LeftPaneMenuAdapter.Inspection:
				{
					/* Intent vehicleIntent = new Intent(this, InspectionReportActivity.class);
					startActivityForResult(vehicleIntent, GConst.REQUEST_InspectionReport_ID);
					_set_adapter_(position); */
					fragment = new InspectionFragment ();
				}
					break;
				case (int)LeftPaneMenuAdapter.HosFragment:
					fragment=new HosViewFragment();
					break;
				case (int)LeftPaneMenuAdapter.Messaging:
					fragment = new MessagingFragment ();
					break;
				case (int)LeftPaneMenuAdapter.Navigation:
				{
					Intent vehicleIntent = new Intent(this, BasicMapActivity.class);
					startActivityForResult(vehicleIntent, GConst.REQUEST_VEH_BOX_ID);
					_set_adapter_(position);
				}
					break;
				case (int)LeftPaneMenuAdapter.Settings:
					fragment = new SettingsFragment ();
					break;
				case (int)LeftPaneMenuAdapter.SwitchAsset:
				{
					Intent vehicleIntent = new Intent(this, VehiclesActivity.class);
					startActivityForResult(vehicleIntent, GConst.REQUEST_VEH_BOX_ID);
					_set_adapter_(position);
				}
					break;
				case (int)LeftPaneMenuAdapter.Help:
				{
					Intent vehicleIntent = new Intent(this, HelpActivity.class);
					//startActivityForResult(vehicleIntent, GConst.REQUEST_HELP_ID);
					startActivity(vehicleIntent);
					_set_adapter_(position);
				}
					break;
				case (int)LeftPaneMenuAdapter.Logout:
					processLogout();
					break;
			}
			
			if (fragment != null) {
				current_fragment=fragment;
				_menuAdapter.setSelectedItem(position);

				FragmentManager fm = getFragmentManager();
				android.app.FragmentTransaction ft = fm.beginTransaction();
				ft.replace(R.id.content_frame, fragment);
				ft.commit();

				left_drawerList.setItemChecked (position, true);
				//ActionBar.Title = _title = _menuAdapter.getTitle (position);//_fragmentTitles [position];
				_drawer.closeDrawer (left_drawerList);
			}
	 }
	 void _set_adapter_(int position)
	 {
		 _menuAdapter.setSelectedItem(position);
			left_drawerList.setItemChecked (position, true);
			_drawer.closeDrawer (left_drawerList);
	 }
	
	 @Override
	 public boolean onCreateOptionsMenu(Menu menu) {
		 getMenuInflater().inflate(R.menu.drawer_main, menu);
		 //setTitle("");
	 return true;
	 }

	 @Override
	 public boolean onOptionsItemSelected(MenuItem item) {
		  super.onOptionsItemSelected(item);
		  switch(item.getItemId())
		  {
	
			 case android.R.id.home:
				 actionBarDrawerToggle.onOptionsItemSelected(item);
					if(_drawer.isDrawerOpen(right_drawerList))
						_drawer.closeDrawer(right_drawerList); 
			 break;
	
	
		  }
		  return true;   
	  }
	 //-------------------------------Logout-------------------
	 void processLogout()
	 {
		 	MkrNLib.Get().processLogout();
		 	MkrApp.StopApp( this, null ); 
		 	
		    Intent i = new Intent(this, LoginActivity.class);
		    i.putExtra(GConst.EXTRA_LOGOUT, GConst.REQUEST_LOGOUT);
			startActivityForResult(i, GConst.REQUEST_LOGOUT);
	 }
}
