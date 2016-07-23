package net.fcnav.plus;


import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import org.apache.commons.logging.Log;

import android.preference.PreferenceScreen;
import android.view.Menu;

import net.flcnav.LogUtil;
import net.fcnav.access.AccessibilityPlugin;
import net.fcnav.plus.activities.MapActivity;
import net.fcnav.plus.activities.SettingsActivity;
import net.fcnav.plus.background.FlcnavBackgroundServicePlugin;
import net.fcnav.plus.views.FCNavMapTileView;




public abstract class FCNavPlugin {
	
	private static List<FCNavPlugin> installedPlugins = new ArrayList<FCNavPlugin>();  
	private static List<FCNavPlugin> activePlugins = new ArrayList<FCNavPlugin>();
	private static final Log LOG = LogUtil.getLog(FCNavPlugin.class);
	
	
	public abstract String getId();
	
	public abstract String getDescription();
	
	public abstract String getName();
	
	public String getVersion() {
		return "";
	}
	
	/**
	 * Initialize plugin runs just after creation
	 */
	public abstract boolean init(FCNavApplication app);
	
	public void disable(FCNavApplication app) {};
	
	
	public static void initPlugins(FCNavApplication app) {
		FCNavSettings settings = app.getSettings();
		
	
		installedPlugins.add(new FlcnavBackgroundServicePlugin(app));
	
		installedPlugins.add(new AccessibilityPlugin(app));

		
		Set<String> enabledPlugins = settings.getEnabledPlugins();
		for (FCNavPlugin plugin : installedPlugins) {
			if (enabledPlugins.contains(plugin.getId())) {
				try {
					if (plugin.init(app)) {
						activePlugins.add(plugin);
					}
				} catch (Exception e) {
					LOG.error("Plugin initialization failed " + plugin.getId(), e);
				}
			}
		}
	}
	
	public static boolean enablePlugin(FCNavApplication app, FCNavPlugin plugin, boolean enable) {
		if (enable) {
			if (!plugin.init(app)) {
				return false;
			}
			activePlugins.add(plugin);
		} else {
			plugin.disable(app);
			activePlugins.remove(plugin);
		}
		app.getSettings().enablePlugin(plugin.getId(), enable);
		return true;
	}
	
	public void updateLayers(FCNavMapTileView mapView, MapActivity activity) {};
	
	public abstract void registerLayers(MapActivity activity);

	public void mapActivityCreate(MapActivity activity) { }
	
	public void mapActivityResume(MapActivity activity) { }
	
	public void mapActivityPause(MapActivity activity) { }
	
	public void mapActivityDestroy(MapActivity activity) { }
	
	public void settingsActivityCreate(SettingsActivity activity, PreferenceScreen screen) {}
	
	public void settingsActivityDestroy(final SettingsActivity activity){}
	
	public void settingsActivityUpdate(final SettingsActivity activity){}
	
	public void registerLayerContextMenuActions(FCNavMapTileView mapView, ContextMenuAdapter adapter, MapActivity mapActivity) {}
	
	public void registerMapContextMenuActions(MapActivity mapActivity, double latitude, double longitude, ContextMenuAdapter adapter, Object selectedObj) {}
	
	public void registerOptionsMenuItems(MapActivity mapActivity, OptionsMenuHelper helper) {}
	
	public void prepareOptionsMenuItems(MapActivity mapActivity, Menu menu) {}

	public boolean onOptionsItemSelected(MapActivity mapActivity, int itemId) { return false; }

	public static void refreshLayers(FCNavMapTileView mapView, MapActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.updateLayers(mapView, activity);
		}
	}
	
	public static List<FCNavPlugin> getAvailablePlugins(){
		return installedPlugins;
	}
	
	public static List<FCNavPlugin> getEnabledPlugins(){
		return activePlugins;
	}
	
	@SuppressWarnings("unchecked")
	public static <T extends FCNavPlugin> T getEnabledPlugin(Class<T> clz) {
		for(FCNavPlugin lr : activePlugins) {
			if(clz.isInstance(lr)){
				return (T) lr;
			}
		}
		return null;
	}
	
	public static void onMapActivityCreate(MapActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.mapActivityCreate(activity);
		}
	}
	
	public static void onMapActivityResume(MapActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.mapActivityResume(activity);
		}
	}
	
	public static void onMapActivityPause(MapActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.mapActivityPause(activity);
		}
	}
	
	public static void onMapActivityDestroy(MapActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.mapActivityDestroy(activity);
		}
	}
	
	
	public static void onSettingsActivityCreate(SettingsActivity activity, PreferenceScreen screen) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.settingsActivityCreate(activity, screen);
		}
	}
	
	public static void onSettingsActivityDestroy(SettingsActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.settingsActivityDestroy(activity);
		}
	}
	
	public static void onSettingsActivityUpdate(SettingsActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.settingsActivityUpdate(activity);
		}
	}
	

	public static void createLayers(FCNavMapTileView mapView, MapActivity activity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.registerLayers(activity);
		}
	}
	
	public static void registerMapContextMenu(MapActivity map, double latitude, double longitude, ContextMenuAdapter adapter, Object selectedObj) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.registerMapContextMenuActions(map, latitude, longitude, adapter, selectedObj);
		}
	}

	public static void registerLayerContextMenu(FCNavMapTileView mapView, ContextMenuAdapter adapter, MapActivity mapActivity) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.registerLayerContextMenuActions(mapView, adapter, mapActivity);
		}
	}
	
	public static void registerOptionsMenu(MapActivity map, OptionsMenuHelper helper) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.registerOptionsMenuItems(map, helper);
		}
	}
	public static void registerOnPrepareOptionsMenu(MapActivity mapActivity, Menu menu) {
		for (FCNavPlugin plugin : activePlugins) {
			plugin.prepareOptionsMenuItems(mapActivity, menu);
		}
	}

	public static boolean registerOnOptionsMenuItemSelected(MapActivity mapActivity, int itemId) {
		for (FCNavPlugin plugin : activePlugins) {
			if (plugin.onOptionsItemSelected(mapActivity, itemId)) {
				return true;
			}
		}
		return false;
	}
	
}
