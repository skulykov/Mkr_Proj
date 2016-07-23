package net.flcnav.osm.io;

import net.flcnav.osm.Entity;
import net.flcnav.osm.Entity.EntityId;

public interface IOsmStorageFilter {
	
	public boolean acceptEntityToLoad(OsmBaseStorage storage, EntityId entityId, Entity entity);

}
