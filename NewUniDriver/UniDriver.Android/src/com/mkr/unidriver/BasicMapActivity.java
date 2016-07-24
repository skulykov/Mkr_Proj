
package com.mkr.unidriver;



import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;



import org.apache.http.NameValuePair;
import org.apache.http.message.BasicNameValuePair;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;



//import com.wl.fleet.util.CXml;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;

import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.GoogleMap.InfoWindowAdapter;
import com.google.android.gms.maps.GoogleMap.OnInfoWindowClickListener;
import com.google.android.gms.maps.GoogleMap.OnMarkerClickListener;

import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;


import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.renderscript.Type;
import android.support.v4.app.FragmentActivity;
import android.text.SpannableString;
import android.text.style.ForegroundColorSpan;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.view.animation.BounceInterpolator;
import android.view.animation.Interpolator;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;


public class BasicMapActivity extends /*FragmentActivity*/OnResultBaseActivity 
implements OnMarkerClickListener, OnInfoWindowClickListener {

	private static final LatLng SOUTH_WEST = new LatLng(-20.47093, -179.0235);
	private static final LatLng NORTH_EAST = new LatLng(70.81319, -40.96298);
/** Demonstrates customizing the info window and/or its contents. */
class CustomInfoWindowAdapter implements InfoWindowAdapter {
   //private final RadioGroup mOptions;

   // These a both viewgroups containing an ImageView with id "badge" and two TextViews with id
   // "title" and "snippet".
   private final View mWindow;
//   private final View mContents;

   CustomInfoWindowAdapter() {
       mWindow = getLayoutInflater().inflate(R.layout.custom_info_window, null);
       //mContents = getLayoutInflater().inflate(R.layout.custom_info_contents, null);
       //mOptions = (RadioGroup) findViewById(R.id.custom_info_window_options);
   }

   @Override
   public View getInfoWindow(Marker marker) {
       /*
	   if (mOptions.getCheckedRadioButtonId() != R.id.custom_info_window) {
           // This means that getInfoContents will be called.
           return null;
       }*/
       render(marker, mWindow);
       return mWindow;
   }

   @Override
   public View getInfoContents(Marker marker) {
       if (true/*mOptions.getCheckedRadioButtonId() != R.id.custom_info_contents*/) {
           // This means that the default info contents will be used.
           return null;
       }
      // render(marker, mContents);
       return null;//mContents;
   }
//http://stackoverflow.com/questions/2471935/how-to-load-an-imageview-by-url-in-android
   private void render(Marker marker, View view) {
      
      
   }
}

    private GoogleMap mMap;
   
    private TextView mUserName,numberOfAssets;
    private TextView txtSelectedAsset;
  
    int timeout=1;

    boolean bctFlag=false;
    @Override
	public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.marker_demo);
        mUserName = (TextView) findViewById(R.id.username_id);
        numberOfAssets = (TextView) findViewById(R.id.number_assets_id);
        txtSelectedAsset = (TextView) findViewById(R.id.selected_asset_id);
        setUpMapIfNeeded();
     
//        setSelectedAsset(Global.gHdr.getAssets().get(0));
        SetMainTitles("Navigation",R.layout.custom_ab_help);
    }
    public void ABBackBtn(View v)
	{
	  finish();
	}

    @Override
	public void onResume() {
        super.onResume();
        setUpMapIfNeeded();
    }

   
	
    private void setUpMapIfNeeded() {
        // Do a null check to confirm that we have not already instantiated the map.
        if (mMap == null) {
            // Try to obtain the map from the SupportMapFragment.
            mMap = ((SupportMapFragment) getSupportFragmentManager().findFragmentById(R.id.map))
                    .getMap();
            // Check if we were successful in obtaining the map.
            if (mMap != null) {
                setUpMap();
            }
        }
    }

    static final LatLng HAMBURG = new LatLng(53.558, 9.927);
    static final LatLng KIEL = new LatLng(53.551, 9.993);

    private void setUpMap() {
        // Hide the zoom controls as the button panel will cover it.
        mMap.getUiSettings().setZoomControlsEnabled(true);

        // Add lots of markers to the map.
        addMarkersToMap();

        // Setting an info window adapter allows us to change the both the contents and look of the
        // info window.
        mMap.setInfoWindowAdapter(new CustomInfoWindowAdapter());

        // Set listeners for marker events.  See the bottom of this class for their behavior.
        mMap.setOnMarkerClickListener(this);
        mMap.setOnInfoWindowClickListener(this);
       
        // Pan to see all markers in view.
        // Cannot zoom to bounds until the map has a size.
        final View mapView = getSupportFragmentManager().findFragmentById(R.id.map).getView();
        if (mapView.getViewTreeObserver().isAlive()) {
            mapView.getViewTreeObserver().addOnGlobalLayoutListener(new OnGlobalLayoutListener() {
                @SuppressWarnings("deprecation") // We use the new method when supported
                @SuppressLint("NewApi") // We check which build version we are using.
                @Override
                public void onGlobalLayout() {
                    
                    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.FROYO) {
                      mapView.getViewTreeObserver().removeGlobalOnLayoutListener(this);
                    } else {
                      mapView.getViewTreeObserver().removeGlobalOnLayoutListener(this);
                    }
                    onMapFinishedLoading();
                   
                }
            });
        }
    }
    
    void onMapFinishedLoading()
    {
    /*	int len=Global.gHdr.getAssetsSize();
    	if(len>0)
    	{
    		fixZoom(Global.gHdr.getAssetsPoints());
    	}
    	else*/
    	{
    		 LatLngBounds bounds = new LatLngBounds.Builder()
             .include(SOUTH_WEST)
             .include(NORTH_EAST)
             .build();
             mMap.moveCamera(CameraUpdateFactory.newLatLngBounds(bounds, 50));
    	}
    }

    private void addMarkersToMap() {
        // Uses a colored icon.
        /*
    	List<Asset> _assets=Global.gHdr.getAssets();
    	
    	int len=_assets.size();
    	numberOfAssets.setText("Assets: "+Integer.toString(len));
    	
    	//http://stackoverflow.com/questions/5114710/android-setting-zoom-level-in-google-maps-to-include-all-marker-points
    	for(int i=0;i<len;i++)
    	{
 
    		Asset a=_assets.get(i);
    		
    		LatLng loc  = new LatLng(a.getLat(), a.getLon());
    		Marker m = mMap.addMarker(new MarkerOptions()
	                .position(loc)
	                .title(a.getDesc())
	                .snippet("0")      // use it as unique identifier
	                .anchor(0.5f, 0.5f) //Simple set anchor point for your marker to 0.5 and 0.5 (middle of your icon).
	                //.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_AZURE)));
	                .icon(BitmapDescriptorFactory.fromResource(R.drawable.normal_car32x32)));
	    	a.setMarker(m);
	    	
	    	 
    	}
    	*/
    }

    private boolean checkReady() {
        if (mMap == null) {
            Toast.makeText(this, R.string.map_not_ready, Toast.LENGTH_SHORT).show();
            return false;
        }
        return true;
    }

    /** Called when the Clear button is clicked. */
    public void onClearMap(View view) {
        if (!checkReady()) {
            return;
        }
        mMap.clear();
    }

    /** Called when the Reset button is clicked. */
    public void onResetMap(View view) {
        if (!checkReady()) {
            return;
        }
        // Clear the map because we don't want duplicates of the markers.
        mMap.clear();
        addMarkersToMap();
    }
    
    private void fixZoom(List<LatLng> points) {
       
        LatLngBounds.Builder bc = new LatLngBounds.Builder();

        for (LatLng item : points) {
            bc.include(item);
        }

        mMap.moveCamera(CameraUpdateFactory.newLatLngBounds(bc.build(), 100));
    }  
    
    //---------------------------------------------------------------------------------
    
    void showAssetTree()
    {
    	    	
    	//MsgBox box=new MsgBox(this);
    	//box.show_assets("Assets","");
    }
    void showSettings()
    {
    	   	
    	//MsgBox box=new MsgBox(this);
    	//	box.show_settings("Settings","");
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
     // getMenuInflater().inflate(R.menu.activity_main, menu);
      return true;
    }
    @Override
   	public boolean onOptionsItemSelected(MenuItem item) {
   		/*
    	switch (item.getItemId()) {
   		case R.id.menu_settings:
   			showSettings();
   			break;
   		case R.id.add:
   			fixZoom(Global.gHdr.getAssetsPoints());
   			showAssetTree();
   			return (true);
   		case R.id.exitN_id:
   			//http://stackoverflow.com/questions/13330415/android-how-to-properly-exit-application-when-inconsistent-condition-is-unavoid
   			finish();   // Exit Application
   			break;
   		case R.id.BreadcrumbTrail_id:
   			if(selectedAsset==null)
   	    	{
   	    		 Util.Notify(BasicMapActivity.this, R.string.str_attention, R.string.str_sel_asset);
   	       	     return true;
   	    	}
   			if(!bctFlag&&selectedAsset!=null)
   	        {
   	        	getBreadcrumbTrail();
   	        	item.setTitle(getResources().getString(R.string.str_clear));
   	        	//item.setIcon(R.drawable.normal_car32x32);
   	        	bctFlag=true;
   	        }
   	        else
   	        {
   	           if(bct!=null)
   	           {
   	        	   for(BCTData d:bct)
   	        	   {
   	        		   Marker m=d.getMarker();
   	        		   if(m!=null)
   	        			   m.remove();
   	        	   }
   	        	   bct.clear();
   	           }
   	           item.setTitle("Trail");
	           //item.setIcon(R.drawable.normal_car32x32);
   	           bctFlag=false;
   	        }
   			return (true);

   		case R.id.LocationReport_id:
   			//Toast.makeText(this, "Action Bar Menu Example", Toast.LENGTH_LONG)
   			//		.show();
   			startReportAct();
   			return (true);
   		}

   		return (super.onOptionsItemSelected(item));
   		*/
    	return true;
   	}

    void startReportAct()
    {
    	/*if(selectedAsset==null)
	    {
	    	 Util.Notify(BasicMapActivity.this, R.string.str_attention, R.string.str_sel_asset);
	         return;
	    }
    	String str=selectedAsset.getDid();
    	if(str==null || str=="")
    	{
    		Util.Notify(BasicMapActivity.this, R.string.str_attention, R.string.str_sel_asset);
	        return;
    	}
    	
    	Intent intent = new Intent(BasicMapActivity.this, LocationReport.class);
		Bundle b = new Bundle(); 
   		b.putString("selDid",str); //Your deviceid
   		intent.putExtras(b); //Put your id to your next Intent 
		startActivity(intent);*/
    }
    //------------------------------------------------------------------------------
    //
    // Marker related listeners.
    //

    @Override
    public boolean onMarkerClick(final Marker marker) {
        /*
    	String s=marker.getSnippet();
    	if(s.equalsIgnoreCase("0"))
    	{
    	   return false;  // this is an assset marker
    	}
    	return true;
    	*/
    	
    	
    	/*
    	if (marker.equals(mPerth)) {
            // This causes the marker at Perth to bounce into position when it is clicked.
            final Handler handler = new Handler();
            final long start = SystemClock.uptimeMillis();
            final long duration = 1500;

            final Interpolator interpolator = new BounceInterpolator();

            handler.post(new Runnable() {
                @Override
                public void run() {
                    long elapsed = SystemClock.uptimeMillis() - start;
                    float t = Math.max(1 - interpolator
                            .getInterpolation((float) elapsed / duration), 0);
                    marker.setAnchor(0.5f, 1.0f + 2 * t);

                    if (t > 0.0) {
                        // Post again 16ms later.
                        handler.postDelayed(this, 16);
                    }
                }
            });
        } else if (marker.equals(mAdelaide)) {
            // This causes the marker at Adelaide to change color.
            marker.setIcon(BitmapDescriptorFactory.defaultMarker(new Random().nextFloat() * 360));
        }
        */
        // We return false to indicate that we have not consumed the event and that we wish
        // for the default behavior to occur (which is for the camera to move such that the
        // marker is centered and for the marker's info window to open, if it has one).
        return false;
        
        /*
         * return true;
			from onMarkerClick to disable default behavior of showing info window and centering on the marker.

         */
    }

    @Override
    public void onInfoWindowClick(Marker marker) {
        Toast.makeText(getBaseContext(), "Click Info Window", Toast.LENGTH_SHORT).show();
    }

 }
/*
 * marker.showInfoWindow();
 http://stackoverflow.com/questions/15899619/opening-infowindow-automatically-when-adding-marker-google-maps-v2-android
 
 */ 
