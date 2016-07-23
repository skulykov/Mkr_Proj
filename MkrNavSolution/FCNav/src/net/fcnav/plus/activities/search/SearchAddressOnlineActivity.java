package net.fcnav.plus.activities.search;

import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Locale;
import java.util.regex.Pattern;

import net.flcnav.Algoritms;
import net.flcnav.LogUtil;
import net.flcnav.ResultMatcher;
import net.fcnav.FCNavFormatter;
import net.fcnav.Version;
import net.flcnav.binary.BinaryMapIndexReader;
import net.flcnav.binary.BinaryMapIndexReader.SearchRequest;
import net.flcnav.data.Building;
import net.flcnav.data.City;
import net.flcnav.data.Street;
import net.flcnav.osm.LatLon;
import net.flcnav.osm.MapUtils;
import net.fcnav.access.AccessibleToast;
import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.FCNavSettings;
import net.fcnav.plus.R;
import net.fcnav.plus.RegionAddressRepository;
import net.fcnav.plus.ResourceManager;
import net.fcnav.plus.activities.MapActivity;
import net.fcnav.plus.activities.FCNavListActivity;
import net.fcnav.plus.activities.search.SearchActivity.SearchActivityChild;
import net.fcnav.plus.activities.search.SearchActivity.SearchActivityNotify;

import org.apache.commons.logging.Log;
import org.xmlpull.v1.XmlPullParser;

import android.app.ProgressDialog;
import android.content.Intent;
import android.location.Location;
import android.os.Bundle;
import android.util.Xml;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class SearchAddressOnlineActivity extends FCNavListActivity implements SearchActivityChild , SearchActivityNotify {
	
	private LatLon location;
	private ProgressDialog progressDlg;
	private final static Log log = LogUtil.getLog(SearchAddressOnlineActivity.class);

	private static PlacesAdapter lastResult = null;
	private FCNavSettings settings;
	
	SearchAddressOnlineActivity _this_activity=null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		_this_activity=this;
		setContentView(R.layout.search_address_online);
		Button searchOffline = (Button) findViewById(R.id.SearchOffline);
		if (getParent() instanceof SearchActivity) {
			searchOffline.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View v) {
					((SearchActivity) getParent()).startSearchAddressOffline();
				}
			});
		} else {
			searchOffline.setVisibility(View.INVISIBLE);
		}
		
		settings = ((FCNavApplication) getApplication()).getSettings();
		
		final EditText searchText = (EditText) findViewById(R.id.SearchText);
		
		Button clearSearch = (Button) findViewById(R.id.ClearSearch);
		clearSearch.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				searchText.setText("");
				lastResult = null;
				setListAdapter(null);
			}
		});
		
		Button searchButton = (Button) findViewById(R.id.SearchButton);
		searchButton.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				InputMethodManager inputMethodManager = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
				inputMethodManager.hideSoftInputFromWindow(searchText.getWindowToken(), 0); // Remove keyboard

				//searchPlacesOnLine(searchText.getText().toString());
				//searchPlacesOffLine(searchText.getText().toString());
				SearchCommon.searchPlacesOffLine((FCNavApplication)getApplication(),_this_activity, searchText.getText().toString(),SearchAddressOnlineActivity.this);
			}
		});
		location = settings.getLastKnownMapLocation();
		
		if (lastResult != null) {
			setListAdapter(lastResult);
		}
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		Intent intent = getIntent();
		if(intent != null){
			double lat = intent.getDoubleExtra(SearchActivity.SEARCH_LAT, 0);
			double lon = intent.getDoubleExtra(SearchActivity.SEARCH_LON, 0);
			if(lat != 0 || lon != 0){
				location = new LatLon(lat, lon);
			}
		}
		if (location == null && getParent() instanceof SearchActivity) {
			location = ((SearchActivity) getParent()).getSearchPoint();
		}
		if (location == null) {
			location = settings.getLastKnownMapLocation();
		}
	}
	
	@Override
	public void locationUpdate(LatLon l) {
		location = l;
		if(lastResult != null){
			lastResult.notifyDataSetInvalidated();
		}
	}
/*
	static String city_req_name="";
	static String street_req_name="";
	static String building_req_name="0";
	static String req_province="";
	
	protected void searchPlacesOffLine(final String search) {
		if(Algoritms.isEmpty(search)){
			return;
		}
		String [] temp = search.split(",");
		if(temp.length<3)
			return;
		req_province=temp[2].trim();
		city_req_name=temp[1].trim();
		temp=temp[0].split(" ");
		if(temp.length>2)
		{
			building_req_name=temp[0].trim();
			street_req_name=temp[1].trim()+" "+temp[2].trim();
		}
		else
		if(temp.length>1)
		{
			building_req_name=temp[0].trim();
			street_req_name=temp[1].trim();
		}
		else
		{
			street_req_name=temp[0].trim();
			building_req_name="0";
		}
			
		
		progressDlg = ProgressDialog.show(this, getString(R.string.searching), getString(R.string.searching_address));
		new Thread(new Runnable(){
			@Override
			public void run() {
				try {
					final List<Place> places = new ArrayList<Place>();
					
					ResourceManager rm=((FCNavApplication)getApplication()).getResourceManager();
					if(rm!=null)
					{
						Collection<RegionAddressRepository> rep= rm.getAddressRepositories();
						if(rep!=null)
						{
							String name="Canada_ontario";
							RegionAddressRepository region=rm.getRegionRepository(name);
							region.preloadCities(new ResultMatcher<City>() {
								 
								@Override
								public boolean publish(City c) {
									String name=c.getEnName();
									//if(name.contains(city_req_name))//if(collator.equals (name,street_req_name))
									if(Pattern.compile(Pattern.quote(city_req_name), Pattern.CASE_INSENSITIVE).matcher(name).find()) 

										return true;
									else
									return false;
								}
								@Override
								public boolean isCancelled() {
									return false;
								}
							});
							List<City> list=region.getLoadedCities();
							if(list!=null && !list.isEmpty())
							{
								for(City c : list){
									
									//street_req.nameQuery = "Toronto";
									
									region.preloadStreets(c, new ResultMatcher<Street>() {
										 
										@Override
										public boolean publish(Street c) {
											String name=c.getEnName();
											//if(name.contains(street_req_name))//if(collator.equals (name,street_req_name))
											if(Pattern.compile(Pattern.quote(street_req_name), Pattern.CASE_INSENSITIVE).matcher(name).find())
												return true;
											else
											return false;
										}
										@Override
										public boolean isCancelled() {
											return false;
										}
									});
									Collection<Street> streets=c.getStreets();
									for(Street s : streets){
									     region.preloadBuildings(s,null);
									     int building=Integer.parseInt(building_req_name);
									    // boolean found=false;
									     LatLon loc=s.getLocation();
									     String bName="";
									     //int prev=0;
									     for (Building b : s.getBuildings()) {
												bName = b.getName(true);
												//if (bName.equals(name)) {
												//	return b;
												//}
												int val=0;
												try
												{
													val=Integer.parseInt(bName.trim());
												}
												catch(Exception e){}
												
												if(val>building)
												{
													loc=b.getLocation();
													break;
												}
												//else
												//	prev=val;
											}
									     //buildings = s.getBuildings().size();
									     Place p = new Place();
									     p.lat = loc.getLatitude();
										 p.lon = loc.getLongitude();
										 p.displayName = bName+" "+s.getEnName()+", "+c.getEnName()+", "+req_province ;
										 places.add(p);
									}
									
								}
							}
						}
					}
										
					
					if(places.isEmpty()){
						showResult(R.string.search_nothing_found, null);
					} else {
						showResult(0, places);
					}
				} catch(Exception e){
					log.error("Error searching address", e); //$NON-NLS-1$
					showResult(R.string.error_io_error, null);
				} finally {
					if(progressDlg != null){
						progressDlg.dismiss();
						progressDlg = null;
					}
				}
			}
			
		}, "SearchingAddress").start(); //$NON-NLS-1$
	}
	*/
	protected void searchPlacesOnLine(final String search) {
		if(Algoritms.isEmpty(search)){
			return;
		}
		
		progressDlg = ProgressDialog.show(this, getString(R.string.searching), getString(R.string.searching_address));
		new Thread(new Runnable(){
			@Override
			public void run() {
				try {
					final List<Place> places = new ArrayList<Place>();
					StringBuilder b = new StringBuilder();
					b.append("http://nominatim.openstreetmap.org/search"); //$NON-NLS-1$
					b.append("?format=xml&addressdetails=0&accept-language=").append(Locale.getDefault().getLanguage()); //$NON-NLS-1$
					b.append("&q=").append(URLEncoder.encode(search)); //$NON-NLS-1$
					
					log.info("Searching address at : " + b.toString()); //$NON-NLS-1$
					URL url = new URL(b.toString());
					URLConnection conn = url.openConnection();
					conn.setDoInput(true);
					conn.setRequestProperty("User-Agent", Version.getFullVersion(SearchAddressOnlineActivity.this)); //$NON-NLS-1$
					conn.connect();
					InputStream is = conn.getInputStream();
					XmlPullParser parser = Xml.newPullParser();
					parser.setInput(is, "UTF-8"); //$NON-NLS-1$
					int ev;
					while ((ev = parser.next()) != XmlPullParser.END_DOCUMENT) {
						if(ev == XmlPullParser.START_TAG){
							if(parser.getName().equals("place")){ //$NON-NLS-1$
								String lat = parser.getAttributeValue("", "lat"); //$NON-NLS-1$ //$NON-NLS-2$
								String lon = parser.getAttributeValue("", "lon");  //$NON-NLS-1$//$NON-NLS-2$
								String displayName = parser.getAttributeValue("", "display_name"); //$NON-NLS-1$ //$NON-NLS-2$
								if(lat != null && lon != null && displayName != null){
									Place p = new Place();
									p.lat = Double.parseDouble(lat);
									p.lon = Double.parseDouble(lon);
									p.displayName = displayName;
									places.add(p);
								}
							}
						}

					}
					is.close();
					if(places.isEmpty()){
						showResult(R.string.search_nothing_found, null);
					} else {
						showResult(0, places);
					}
				} catch(Exception e){
					log.error("Error searching address", e); //$NON-NLS-1$
					showResult(R.string.error_io_error, null);
				} finally {
					if(progressDlg != null){
						progressDlg.dismiss();
						progressDlg = null;
					}
				}
			}
			
		}, "SearchingAddress").start(); //$NON-NLS-1$
	}
	
	private void showResult(final int warning, final List<Place> places) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if(places == null){
					AccessibleToast.makeText(SearchAddressOnlineActivity.this, getString(warning), Toast.LENGTH_LONG).show();
				} else {
					ListView lv=getListView();//svk
					lv.clearChoices();   //svk  clear previous choices!!!
					
					lastResult = new PlacesAdapter(SearchAddressOnlineActivity.this, R.layout.search_address_online_list_item,places,location);
					setListAdapter(lastResult);
				}
			}
		});
	}
	
	
	@Override
	protected void onStop() {
		if(progressDlg != null){
			progressDlg.dismiss();
			progressDlg = null;
		}
		super.onStop();
	}
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		super.onListItemClick(l, v, position, id);
		Place item = ((PlacesAdapter) getListAdapter()).getItem(position);
		settings.setMapLocationToShow(item.lat, item.lon,
				Math.max(15, settings.getLastKnownMapZoom()), getString(R.string.address)+ " : " + item.displayName); //$NON-NLS-1$
		MapActivity.launchMapActivityMoveToTop(this);
	}
	
	public static class Place {
		public double lat;
		public double lon;
		public String displayName;
	}
	/*
	class PlacesAdapter extends ArrayAdapter<Place> {

		public PlacesAdapter(List<Place> places) {
			super(SearchAddressOnlineActivity.this, R.layout.search_address_online_list_item, places);
		}
		
		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			View row = convertView;
			if (row == null) {
				LayoutInflater inflater = getLayoutInflater();
				row = inflater.inflate(R.layout.search_address_online_list_item, parent, false);
			}
			Place model = getItem(position);
			TextView label = (TextView) row.findViewById(R.id.label);
			TextView distanceLabel = (TextView) row.findViewById(R.id.distance_label);
			if(location != null){
				int dist = (int) (MapUtils.getDistance(location, model.lat, model.lon));
				distanceLabel.setText(FCNavFormatter.getFormattedDistance(dist, SearchAddressOnlineActivity.this));
			} else {
				distanceLabel.setText(""); //$NON-NLS-1$
			}
			label.setText(model.displayName);
			return row;
		}
		
	}
     */
	@Override
	public void notifyOnSearch(int flag, List<Place> places) {
		showResult(flag, places);
		
	}
	

}
