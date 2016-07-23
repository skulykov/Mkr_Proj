package net.fcnav.plus.activities.search;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.regex.Pattern;

import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.R;
import net.fcnav.plus.RegionAddressRepository;
import net.fcnav.plus.ResourceManager;
import net.fcnav.plus.activities.search.SearchActivity.SearchActivityChild;
import net.fcnav.plus.activities.search.SearchActivity.SearchActivityNotify;
import net.fcnav.plus.activities.search.SearchAddressOnlineActivity.Place;
//import net.fcnav.plus.activities.search.SearchAddressOnlineActivity.Place;
import net.flcnav.Algoritms;
import net.flcnav.ResultMatcher;
import net.flcnav.data.Building;
import net.flcnav.data.City;
import net.flcnav.data.Street;
import net.flcnav.osm.LatLon;
import android.app.ProgressDialog;
import android.content.Context;

public class SearchCommon {

	static FCNavApplication application;
	static String city_req_name="";
	static String street_req_name="";
	static String building_req_name="0";
	static String req_province="";
	
	public static void searchPlacesOffLine(FCNavApplication appl,Context ctx,final String search,final SearchActivityNotify notif) {
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
		
		application=appl;
		//Context ctx=application.getBaseContext();
		final ProgressDialog progressDlg = ProgressDialog.show(ctx, ctx.getString(R.string.searching), ctx.getString(R.string.searching_address));
		new Thread(new Runnable(){
			@Override
			public void run() {
				try {
					final List<Place> places = new ArrayList<Place>();
					
					ResourceManager rm=application.getResourceManager();
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
												/*
												if(Math.abs(building-val)<100)
												{
													//found=true;
													loc=b.getLocation();
													break;
												}*/
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
						notif.notifyOnSearch(R.string.search_nothing_found, null);//showResult(R.string.search_nothing_found, null);
					} else {
						notif.notifyOnSearch(0, places);//showResult(0, places);
					}
				} catch(Exception e){
					//log.error("Error searching address", e); //$NON-NLS-1$
					notif.notifyOnSearch(R.string.error_io_error, null); //showResult(R.string.error_io_error, null);
				} finally {
					if(progressDlg != null){
						progressDlg.dismiss();
						//progressDlg = null;
					}
				}
			}
			
		}, "SearchingAddress").start(); //$NON-NLS-1$
	}
}
