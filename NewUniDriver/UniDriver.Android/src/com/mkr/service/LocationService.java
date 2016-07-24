package com.mkr.service;

import java.io.IOException;
import java.util.List;

import com.mkr.models.MyLocation;

import android.content.Context;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;

public class LocationService implements LocationListener {

	LocationManager _locMgr;
	String _locationProvider;
	MyLocation mloc=null;
	
	public LocationService()
	{
		start();
		mloc=new MyLocation();
	}
	void start()
	{
		Context ctx=MkrApp.getAppContext();
		_locMgr = (LocationManager)ctx.getSystemService(Context.LOCATION_SERVICE);
		Criteria criteria = new Criteria();
		
		criteria.setAccuracy( Criteria.ACCURACY_FINE );
		criteria.setAltitudeRequired(false);
        criteria.setBearingRequired(true);
        criteria.setCostAllowed(true);

        _locationProvider = _locMgr.getBestProvider(criteria, true);

		if(_locationProvider!=null)
			_locMgr.requestLocationUpdates(_locationProvider, 2000, 50, this);
	}
    public MyLocation GetMLoc(){return mloc;}
	@Override
	public void onLocationChanged(Location location) {
		mloc.Lat = (float)location.getLatitude();
		mloc.Lon = (float)location.getLongitude();
		mloc.Speed = location.getSpeed();
		mloc.Cog = location.getBearing();
		mloc.gpsFix=location.hasAccuracy(); //??
	}

	@Override
	public void onProviderDisabled(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onProviderEnabled(String arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onStatusChanged(String arg0, int arg1, Bundle arg2) {
		// TODO Auto-generated method stub
		
	}
	
	//=======================================================
	public Address getAddress( float lat,float lon) 
	{
			   
			   final Context context = MkrApp.getAppContext();
			    final Geocoder geocoder = new Geocoder(context);
			    if(Geocoder.isPresent ()) 
			    {
				    final List<Address> addresses;
				    try { 
				      addresses = geocoder.getFromLocation(lat,lon, 1);
				    } catch (IOException e) {
				      return null; 
				    } 
				    if (addresses != null && !addresses.isEmpty())
				      return addresses.get(0);
				    else 
				      return null;
			    }
			    return null;
	} 
	public String getStrAddress(float lat,float lon)
	{
		String addr="";
		 Log.d("LLL","getStrAddress...");
		Address ad=getAddress( lat,lon) ;
		
		if (ad != null) {
			 Log.d("LLL","getStrAddress_1...");
			String building = ad.getSubThoroughfare() == null ? "" : ad.getSubThoroughfare();
			String street = ad.getThoroughfare() == null ? "" : ad.getThoroughfare();
			String city = ad.getSubLocality() == null ? "" : ad.getSubLocality();
			String country = ad.getCountryName() == null ? "" : ad.getCountryName();
			String province = ad.getAdminArea() == null ? "" : ad.getAdminArea();
			String zip = ad.getPostalCode() == null ? "" : ad.getPostalCode();
			addr = String.format ("%s %s, %s, %s, %s, %s", building, street, city, province, country, zip); 
			 Log.d("LLL","getStrAddress...="+addr);
		}
		return addr;
	}
	
	public String getCurrentAddress()
	{
		if(mloc==null)
			return "";
		return getStrAddress( this.mloc.Lat,this.mloc.Lon) ;
	}
	
	
}
