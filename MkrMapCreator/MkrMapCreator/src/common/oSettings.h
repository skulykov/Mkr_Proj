#ifndef OSMSETTINGS_H
#define OSMSETTINGS_H

//http://stackoverflow.com/questions/1965249/how-to-write-java-like-enums-in-c

#define   OSMTagKey_NAME  "name"   
#define   OSMTagKey_NAME_EN  "name:en"   
   // ways
#define   OSMTagKey_HIGHWAY  "highway"   
#define   OSMTagKey_BUILDING  "building"   
#define   OSMTagKey_BOUNDARY  "boundary"   
#define   OSMTagKey_POSTAL_CODE  "postal_code"   
#define   OSMTagKey_RAILWAY  "railway"   
#define   OSMTagKey_ONEWAY  "oneway"   
#define   OSMTagKey_LAYER  "layer"   
#define   OSMTagKey_BRIDGE  "bridge"   
#define   OSMTagKey_TUNNEL  "tunnel"   
#define   OSMTagKey_TOLL  "toll"   
#define   OSMTagKey_JUNCTION  "junction"   
   // transport
#define   OSMTagKey_ROUTE  "route"   
#define   OSMTagKey_ROUTE_MASTER  "route_master"   
#define   OSMTagKey_OPERATOR  "operator"   
#define   OSMTagKey_REF  "ref"   
   // address
#define   OSMTagKey_PLACE  "place"   
#define   OSMTagKey_ADDR_HOUSE_NUMBER  "addr:housenumber"   
#define   OSMTagKey_ADDR_STREET  "addr:street"   
#define   OSMTagKey_ADDR_STREET2  "addr:street2"   
#define   OSMTagKey_ADDR_CITY  "addr:city"   
#define   OSMTagKey_ADDR_POSTCODE  "addr:postcode"   
#define   OSMTagKey_ADDR_INTERPOLATION  "addr:interpolation"   
#define   OSMTagKey_ADDRESS_TYPE  "address:type"   
#define   OSMTagKey_ADDRESS_HOUSE  "address:house"   
#define   OSMTagKey_TYPE  "type"   
#define   OSMTagKey_IS_IN  "is_in"   
   // POI
#define   OSMTagKey_AMENITY  "amenity"   
#define   OSMTagKey_SHOP  "shop"   
#define   OSMTagKey_LEISURE  "leisure"    
#define   OSMTagKey_TOURISM  "tourism"   
#define   OSMTagKey_SPORT  "sport"    
#define   OSMTagKey_HISTORIC  "historic"   
#define   OSMTagKey_NATURAL  "natural"   
#define   OSMTagKey_INTERNET_ACCESS  "internet_access"   
#define   OSMTagKey_CONTACT_WEBSITE  "contact:website"   
#define   OSMTagKey_CONTACT_PHONE  "contact:phone"   
   
#define   OSMTagKey_OPENING_HOURS  "opening_hours"    
#define   OSMTagKey_PHONE  "phone"   
#define   OSMTagKey_DESCRIPTION  "description"   
#define   OSMTagKey_WEBSITE  "website"   
#define   OSMTagKey_URL  "url"   
#define   OSMTagKey_WIKIPEDIA  "wikipedia"   
  
#define   OSMTagKey_ADMIN_LEVEL  "admin_level"   

	
	
	enum OSMHighwayTypes {
		TRUNK, MOTORWAY, PRIMARY, SECONDARY, RESIDENTIAL, TERTIARY, SERVICE, TRACK,
   
		// TODO is link needed?
		TRUNK_LINK, MOTORWAY_LINK, PRIMARY_LINK, SECONDARY_LINK, RESIDENTIAL_LINK, TERTIARY_LINK, SERVICE_LINK, TRACK_LINK, 
   
	};

#endif

	