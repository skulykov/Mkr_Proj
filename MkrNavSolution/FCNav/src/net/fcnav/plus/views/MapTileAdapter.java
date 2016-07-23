package net.fcnav.plus.views;

import android.graphics.Canvas;
import android.graphics.RectF;

/**
 * This class is designed to represent adapter for specific map sources
 * that requires additional computation or updates
 */
public abstract class MapTileAdapter {
	
	protected MapTileLayer layer;
	protected FCNavMapTileView view;

	public void initLayerAdapter(MapTileLayer layer, FCNavMapTileView view){
		this.layer = layer;
		this.view = view;
	}
	
	public abstract void onDraw(Canvas canvas, RectF latlonRect, RectF tilesRect, boolean nightMode);
	
	public abstract void onClear(); 

	public abstract void onInit();
}
