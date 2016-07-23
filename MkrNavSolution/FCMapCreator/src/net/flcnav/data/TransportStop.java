package net.flcnav.data;

import net.flcnav.osm.Entity;

public class TransportStop extends MapObject {
	int[] referencesToRoutes = null;
	
	public TransportStop(Entity e){
		super(e);
	}
	
	public TransportStop(){
	}
	
	public int[] getReferencesToRoutes() {
		return referencesToRoutes;
	}
	
	public void setReferencesToRoutes(int[] referencesToRoutes) {
		this.referencesToRoutes = referencesToRoutes;
	}

}
