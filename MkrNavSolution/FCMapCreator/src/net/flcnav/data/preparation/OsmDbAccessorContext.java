package net.flcnav.data.preparation;

import java.sql.SQLException;

import net.flcnav.osm.Relation;
import net.flcnav.osm.Way;

public interface OsmDbAccessorContext {
	
	/**
	 * Load way with points (with tags) and tags 
	 */
	public void loadEntityWay(Way e) throws SQLException;
	
	/**
	 * Load one level relation members :
	 * ways - loaded with tags and points, nodes with tags, relations with members and tags
	 */
	public void loadEntityRelation(Relation e) throws SQLException;
}