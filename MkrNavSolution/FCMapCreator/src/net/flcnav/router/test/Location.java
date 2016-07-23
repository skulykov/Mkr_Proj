package net.flcnav.router.test;

public class Location {
	
	private  double longitude;
	private  double latitude;
	String str;
	public Location(String _str)
	{
		str=_str;
	}
	public Location(double lat,double lon)
	{
		str="";
		latitude=lat;
		longitude=lon;
	}
	void setLatitude(double v)
	{
		latitude=v;
	}
	void setLongitude(double v)
	{
		longitude=v;
	}
	public double getLongitude()
	{
		return longitude;
	}
	public double getLatitude()
	{
		return latitude;
	}
	public float distanceTo(Location loc)
	{
	  return (float)1.0;
	}
	public float bearingTo(Location loc)
	{
	  return (float)1.0;
	}
	
	public boolean hasSpeed(){return false;}
	public float getSpeed()
	{
		return (float)2.2;
	}
	public static void distanceBetween(double latitude2, double longitude2,
			double latitude3, double longitude3, float[] compute) {
		// TODO Auto-generated method stub
		compute[0]=(float)2.3;
		compute[1]=(float)2.4;
	}
	
}
