package net.flcnav.data;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import net.flcnav.Algoritms;
import net.flcnav.osm.Entity;
import net.flcnav.osm.LatLon;
import net.flcnav.osm.MapUtils;
import net.flcnav.osm.Node;
import net.flcnav.osm.Way;
import net.flcnav.osm.OSMSettings.OSMTagKey;


public class Street extends MapObject {
	
	private List<Building> buildings = new ArrayList<Building>(); 
	private List<Way> wayNodes = null;
	private List<Street> intersectedStreets = null;
	private final City city;
	private int indexInCity = -1;

	public Street(City city, String name){
		this.city = city;
		this.name = name;
	}
	
	public Street(City city) {
		this.city = city;
	}
	public Building registerBuilding(Entity e){
		return registerBuilding(e, e.getTag(OSMTagKey.ADDR_HOUSE_NUMBER));
	}
	
	public Building registerBuilding(Entity e, String ref){
		if(ref == null){
			return null;
		}
		Building building = new Building(e);
		building.setName(ref);
		buildings.add(building);
		return building;
	}
	
	public void registerBuilding(Building building){
		buildings.add(building);
	}
	
	public List<Street> getIntersectedStreets() {
		if(intersectedStreets == null){
			return Collections.emptyList();
		}
		return intersectedStreets;
	}
	
	public void addIntersectedStreet(Street s){
		if(intersectedStreets == null) {
			intersectedStreets = new ArrayList<Street>();
		}
		intersectedStreets.add(s);
	}
	
	public void registerBuildingCheckById(Building building){
		for(Building b : buildings) {
			if(b.getId().longValue() == building.getId().longValue()){
				return;
			}
		}
		buildings.add(building);
	}
	
	public List<Building> getBuildings() {
		return buildings;
	}
	
	protected void calculateCenter(){
		List<Node> nodes = new ArrayList<Node>();
		if (wayNodes != null) {
			for (Way w : wayNodes) {
				nodes.addAll(w.getNodes());
			}
		}
		
		LatLon c = MapUtils.getWeightCenterForNodes(nodes);
		double dist = Double.POSITIVE_INFINITY;
		for(Node n : nodes){
			if (n != null) {
				double nd = MapUtils.getDistance(n, c);
				if (nd < dist) {
					dist = nd;
					location = n.getLatLon();
				}
			}
		}
	}
	
	public boolean isRegisteredInCity(){
		return city != null && city.getStreet(getName()) == this;
	}
	
	@Override
	public void setName(String name) {
		if (name.equals(getName())) {
			return;
		}
		if (city != null && city.getStreet(getName()) == this) {
			city.unregisterStreet(getName());
			super.setName(name);
			Street s = city.registerStreet(this);
			if(s != this){
				// that case means that street unregistered
//				city = null;
			}
		} else {
			super.setName(name);
		}
	}
	
	
	public List<Way> getWayNodes() {
		if(wayNodes == null){
			wayNodes = new ArrayList<Way>();
		}
		return wayNodes;
	}
	
	public City getCity() {
		return city;
	}
	
	public void sortBuildings(){
		Collections.sort(buildings, new Comparator<Building>(){
			@Override
			public int compare(Building o1, Building o2) {
				int i1 = Algoritms.extractFirstIntegerNumber(o1.getName());
				int i2 = Algoritms.extractFirstIntegerNumber(o2.getName());
				return i1 - i2;
			}
		});
	}

	@Override
	public void doDataPreparation() {
		sortBuildings();
		calculateCenter();
		if(location == null){
			List<LatLon> nodes = new ArrayList<LatLon>();
			for(Building b : buildings){
				nodes.add(b.getLocation());
			}
			location = MapUtils.getWeightCenter(nodes);
		}
		if(location == null || ((wayNodes == null || wayNodes.isEmpty()) && buildings.isEmpty())){
			city.unregisterStreet(name);
		}
		if (wayNodes != null && wayNodes.size() > 0) {
			this.id = wayNodes.get(0).getId();
		} else if(buildings.size() > 0){
			this.id = buildings.get(0).getId();
		}
		
	}
	
	public int getIndexInCity() {
		return indexInCity;
	}
	
	public void setIndexInCity(int indexInCity) {
		this.indexInCity = indexInCity;
	}

}
