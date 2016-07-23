package net.fcnav.plus.activities;


import java.io.File;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import net.fcnav.AndroidUtils;
import net.fcnav.CallbackWithObject;
import net.fcnav.FavouritePoint;
import net.fcnav.GPXUtilities;
import net.fcnav.GPXUtilities.GPXFile;
import net.flcnav.LogUtil;
import net.fcnav.access.AccessibleAlertBuilder;
import net.fcnav.access.AccessibleToast;
import net.flcnav.data.Amenity;
import net.flcnav.map.ITileSource;
import net.flcnav.osm.LatLon;
import net.flcnav.osm.MapUtils;
import net.fcnav.plus.AmenityIndexRepositoryOdb;
import net.fcnav.plus.ContextMenuAdapter;
import net.fcnav.plus.ContextMenuAdapter.OnContextMenuClick;
import net.fcnav.plus.FavouritesDbHelper;
import net.fcnav.plus.OptionsMenuHelper;
import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.FCNavPlugin;
import net.fcnav.plus.FCNavSettings;
import net.fcnav.plus.R;
import net.fcnav.plus.ResourceManager;
import net.fcnav.plus.activities.search.SearchActivity;
import net.fcnav.plus.routing.RouteAnimation;
import net.fcnav.plus.routing.RouteProvider.GPXRouteParams;
import net.fcnav.plus.routing.RoutingHelper;
import net.fcnav.plus.views.BaseMapLayer;
import net.fcnav.plus.views.MapTileLayer;
import net.fcnav.plus.views.FCNavMapTileView;
import net.fcnav.plus.views.RouteInfoLayer;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.DialogInterface.OnMultiChoiceClickListener;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.Resources;
import android.graphics.Rect;
import android.graphics.RectF;
import android.location.Location;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.Settings.Secure;
import android.text.ClipboardManager;
import android.text.Html;
import android.util.FloatMath;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class MapActivityActions implements DialogProvider {
	
//	private static final String GPS_STATUS_COMPONENT = "com.eclipsim.gpsstatus2"; //$NON-NLS-1$
//	private static final String GPS_STATUS_ACTIVITY = "com.eclipsim.gpsstatus2.GPSStatus"; //$NON-NLS-1$
	
	private static final String KEY_LONGITUDE = "longitude";
	private static final String KEY_LATITUDE = "latitude";
	private static final String KEY_NAME = "name";
	private static final String KEY_FAVORITE = "favorite";
	private static final String KEY_ZOOM = "zoom";

	private static final int DIALOG_ADD_FAVORITE = 100;
	private static final int DIALOG_REPLACE_FAVORITE = 101;
	private static final int DIALOG_ADD_WAYPOINT = 102;
	private static final int DIALOG_RELOAD_TITLE = 103;
	private static final int DIALOG_SHARE_LOCATION = 104;
	private static final int DIALOG_SAVE_DIRECTIONS = 106;
	private Bundle dialogBundle = new Bundle();
	
	private final MapActivity mapActivity;
	private FCNavSettings settings;
	private RoutingHelper routingHelper;
	private RouteAnimation routeAnimation = new RouteAnimation();

	public MapActivityActions(MapActivity mapActivity){
		this.mapActivity = mapActivity;
		settings = mapActivity.getMyApplication().getSettings();
		routingHelper = mapActivity.getMyApplication().getRoutingHelper();
	}

	protected void addFavouritePoint(final double latitude, final double longitude){
		String name = mapActivity.getMapLayers().getContextMenuLayer().getSelectedObjectName();
		enhance(dialogBundle,latitude,longitude, name);
		mapActivity.showDialog(DIALOG_ADD_FAVORITE);
	}
	
	private Bundle enhance(Bundle aBundle, double latitude, double longitude, String name) {
		aBundle.putDouble(KEY_LATITUDE, latitude);
		aBundle.putDouble(KEY_LONGITUDE, longitude);
		aBundle.putString(KEY_NAME, name);
		return aBundle;
	}
	
	private Bundle enhance(Bundle bundle, double latitude, double longitude, final int zoom) {
		bundle.putDouble(KEY_LATITUDE, latitude);
		bundle.putDouble(KEY_LONGITUDE, longitude);
		bundle.putInt(KEY_ZOOM, zoom);
		return bundle;
	}

	protected void prepareAddFavouriteDialog(Dialog dialog, Bundle args) {
		final Resources resources = mapActivity.getResources();
		final double latitude = args.getDouble(KEY_LATITUDE);
		final double longitude = args.getDouble(KEY_LONGITUDE);
		String name = resources.getString(R.string.add_favorite_dialog_default_favourite_name);
		if(args.getString(KEY_NAME) != null) {
			name = args.getString(KEY_NAME);
		}
		final FavouritePoint point = new FavouritePoint(latitude, longitude, name,
				resources.getString(R.string.favorite_default_category));
		args.putSerializable(KEY_FAVORITE, point);
		final EditText editText =  (EditText) dialog.findViewById(R.id.Name);
		editText.setText(point.getName());
		editText.selectAll();
		editText.requestFocus();
		final AutoCompleteTextView cat =  (AutoCompleteTextView) dialog.findViewById(R.id.Category);
		cat.setText(point.getCategory());
		AndroidUtils.softKeyboardDelayed(editText);
	}
	
	protected Dialog createAddFavouriteDialog(final Bundle args) {
    	Builder builder = new AlertDialog.Builder(mapActivity);
		builder.setTitle(R.string.favourites_context_menu_edit);
		final View v = mapActivity.getLayoutInflater().inflate(R.layout.favourite_edit_dialog, null, false);
		final FavouritesDbHelper helper = getMyApplication().getFavorites();
		builder.setView(v);
		final EditText editText =  (EditText) v.findViewById(R.id.Name);
		final AutoCompleteTextView cat =  (AutoCompleteTextView) v.findViewById(R.id.Category);
		cat.setAdapter(new ArrayAdapter<String>(mapActivity, R.layout.list_textview, helper.getFavoriteGroups().keySet().toArray(new String[] {})));
		
		builder.setNegativeButton(R.string.default_buttons_cancel, null);
		builder.setNeutralButton(R.string.update_existing, new DialogInterface.OnClickListener(){

			@Override
			public void onClick(DialogInterface dialog, int which) {
				mapActivity.showDialog(DIALOG_REPLACE_FAVORITE);
			}
			
		});
		builder.setPositiveButton(R.string.default_buttons_add, new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				FavouritePoint point = (FavouritePoint) args.getSerializable(KEY_FAVORITE);
				final FavouritesDbHelper helper = mapActivity.getMyApplication().getFavorites();
				point.setName(editText.getText().toString());
				point.setCategory(cat.getText().toString());
				boolean added = helper.addFavourite(point);
				if (added) {
					AccessibleToast.makeText(mapActivity, MessageFormat.format(getString(R.string.add_favorite_dialog_favourite_added_template), point.getName()), Toast.LENGTH_SHORT)
							.show();
				}
				mapActivity.getMapView().refreshMap(true);
			}
		});
		return builder.create();
    }

	protected Dialog createReplaceFavouriteDialog(final Bundle args) {
		final FavouritesDbHelper helper = getMyApplication().getFavorites();
		final Collection<FavouritePoint> points = helper.getFavouritePoints();
		final String[] names = new String[points.size()];
		if(names.length == 0){
			AccessibleToast.makeText(mapActivity, getString(R.string.fav_points_not_exist), Toast.LENGTH_SHORT).show();
			helper.close();
			return null;
		}
			
		Builder b = new AlertDialog.Builder(mapActivity);
		final FavouritePoint[] favs = new FavouritePoint[points.size()];
		Iterator<FavouritePoint> it = points.iterator();
		int i=0;
		while(it.hasNext()){
			FavouritePoint fp = it.next();
			// filter gpx points
			if(fp.isStored()){
				favs[i] = fp;
				names[i] = fp.getName();
				i++;
			}
		}
		b.setItems(names, new DialogInterface.OnClickListener(){
			@Override
			public void onClick(DialogInterface dialog, int which) {
				FavouritePoint fv = favs[which];
				FavouritePoint point = (FavouritePoint) args.getSerializable(KEY_FAVORITE);
				if(helper.editFavourite(fv, point.getLatitude(), point.getLongitude())){
					AccessibleToast.makeText(mapActivity, getString(R.string.fav_points_edited), Toast.LENGTH_SHORT).show();
				}
				mapActivity.getMapView().refreshMap();
			}
		});
		return b.create();
	}
	
    public void addWaypoint(final double latitude, final double longitude){
    	String name = mapActivity.getMapLayers().getContextMenuLayer().getSelectedObjectName();
    	enhance(dialogBundle,latitude,longitude, name);
    	mapActivity.showDialog(DIALOG_ADD_WAYPOINT);
    }
    
    private Dialog createAddWaypointDialog(final Bundle args) {
    	Builder builder = new AlertDialog.Builder(mapActivity);
		builder.setTitle(R.string.add_waypoint_dialog_title);
		FrameLayout parent = new FrameLayout(mapActivity);
		final EditText editText = new EditText(mapActivity);
		editText.setId(R.id.TextView);
		parent.setPadding(15, 0, 15, 0);
		parent.addView(editText);
		builder.setView(parent);
		builder.setNegativeButton(R.string.default_buttons_cancel, null);
		builder.setPositiveButton(R.string.default_buttons_add, new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				double latitude = args.getDouble(KEY_LATITUDE);
				double longitude = args.getDouble(KEY_LONGITUDE);
				String name = editText.getText().toString();
				mapActivity.getSavingTrackHelper().insertPointData(latitude, longitude, System.currentTimeMillis(), name);
				if(settings.SHOW_CURRENT_GPX_TRACK.get()) {
					getMyApplication().getFavorites().addFavoritePointToGPXFile(new FavouritePoint(latitude, longitude, name, ""));
				}
				AccessibleToast.makeText(mapActivity, MessageFormat.format(getString(R.string.add_waypoint_dialog_added), name), Toast.LENGTH_SHORT)
							.show();
				dialog.dismiss();
			}
		});
		return builder.create();
    }
    
    public void reloadTile(final int zoom, final double latitude, final double longitude){
    	enhance(dialogBundle,latitude,longitude,zoom);
    	mapActivity.showDialog(DIALOG_RELOAD_TITLE);
    }

    
    
    
    protected String getString(int res){
    	return mapActivity.getString(res);
    }
    
    protected void updatePoiDb(int zoom, double latitude, double longitude){
    	if(zoom < 15){
    		AccessibleToast.makeText(mapActivity, getString(R.string.update_poi_is_not_available_for_zoom), Toast.LENGTH_SHORT).show();
    		return;
    	}
    	final AmenityIndexRepositoryOdb repo = ((FCNavApplication) mapActivity.getApplication()).
    								getResourceManager().getUpdatablePoiDb();
    	if(repo == null){
    		AccessibleToast.makeText(mapActivity, getString(R.string.update_poi_no_offline_poi_index), Toast.LENGTH_LONG).show();
    		return;
    	} else {
    		AccessibleToast.makeText(mapActivity, getString(R.string.update_poi_does_not_change_indexes), Toast.LENGTH_LONG).show();
    	}
    	final FCNavMapTileView mapView = mapActivity.getMapView();
    	Rect pixRect = new Rect(-mapView.getWidth()/2, -mapView.getHeight()/2, 3*mapView.getWidth()/2, 3*mapView.getHeight()/2);
    	RectF tileRect = new RectF();
    	mapView.calculateTileRectangle(pixRect, mapView.getCenterPointX(), mapView.getCenterPointY(), 
    			mapView.getXTile(), mapView.getYTile(), tileRect);
    	final double leftLon = MapUtils.getLongitudeFromTile(zoom, tileRect.left); 
    	final double topLat = MapUtils.getLatitudeFromTile(zoom, tileRect.top);
		final double rightLon = MapUtils.getLongitudeFromTile(zoom, tileRect.right);
		final double bottomLat = MapUtils.getLatitudeFromTile(zoom, tileRect.bottom);
    	
		ProgressDialog progressDlg = ProgressDialog.show(mapActivity, getString(R.string.loading), getString(R.string.loading_data));
		mapActivity.setProgressDlg(progressDlg);
    	new Thread(new Runnable(){
			@Override
			public void run() {
				try {
					List<Amenity> amenities = new ArrayList<Amenity>();
					boolean loadingPOIs = AmenityIndexRepositoryOdb.loadingPOIs(amenities, leftLon, topLat, rightLon, bottomLat);
					if(!loadingPOIs){
						showToast(getString(R.string.update_poi_error_loading));
					} else {
						repo.updateAmenities(amenities, leftLon, topLat, rightLon, bottomLat);
						showToast(MessageFormat.format(getString(R.string.update_poi_success), amenities.size()));
						mapView.refreshMap();
					}
				} catch(Exception e) {
					Log.e(LogUtil.TAG, "Error updating local data", e); //$NON-NLS-1$
					showToast(getString(R.string.update_poi_error_local));
				} finally {
					Dialog prog = mapActivity.getProgressDlg();
					if(prog !=null){
						prog.dismiss();
						mapActivity.setProgressDlg(prog);
					}
				}
			}
    	}, "LoadingPOI").start(); //$NON-NLS-1$
    	
    }
    
    protected void showToast(final String msg){
    	mapActivity.runOnUiThread(new Runnable(){
			@Override
			public void run() {
				AccessibleToast.makeText(mapActivity, msg, Toast.LENGTH_LONG).show();
			}
    	});
    }
    
    protected void shareLocation(final double latitude, final double longitude, int zoom){
    	enhance(dialogBundle,latitude,longitude,zoom);
    	mapActivity.showDialog(DIALOG_SHARE_LOCATION);
    }
    
    private Dialog createShareLocationDialog(final Bundle args) {
		AlertDialog.Builder builder = new Builder(mapActivity);
		builder.setTitle(R.string.send_location_way_choose_title);
		builder.setItems(new String[]{
				"Email", "SMS", "Clipboard"
		}, new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				final double latitude = args.getDouble(KEY_LATITUDE);
				final double longitude = args.getDouble(KEY_LONGITUDE);
				final int zoom = args.getInt(KEY_ZOOM);

				final String shortOsmUrl = MapUtils.buildShortOsmUrl(latitude, longitude, zoom);
				// final String simpleGeo = "geo:"+((float) latitude)+","+((float)longitude) +"?z="+zoom;
				final String appLink = "http://download.flcnav.net/go?lat="+((float) latitude)+"&lon="+((float)longitude) +"&z="+zoom;
				if(which == 0){
					String email = mapActivity.getString(R.string.send_location_email_pattern, shortOsmUrl, appLink);
					Intent intent = new Intent(Intent.ACTION_SEND);
					intent.setType("vnd.android.cursor.dir/email"); //$NON-NLS-1$
					intent.putExtra(Intent.EXTRA_SUBJECT, "Location"); //$NON-NLS-1$
					intent.putExtra(Intent.EXTRA_TEXT, Html.fromHtml(email));
					intent.setType("text/html");
					mapActivity.startActivity(Intent.createChooser(intent, getString(R.string.send_location)));
				} else {
					String sms = mapActivity.getString(R.string.send_location_sms_pattern, shortOsmUrl, appLink);
					if(which == 1){
						Intent sendIntent = new Intent(Intent.ACTION_VIEW);
						sendIntent.putExtra("sms_body", sms); 
						sendIntent.setType("vnd.android-dir/mms-sms");
						mapActivity.startActivity(sendIntent);   
					} else if (which == 2){
						ClipboardManager clipboard = (ClipboardManager) mapActivity.getSystemService(Activity.CLIPBOARD_SERVICE);
						clipboard.setText(sms);
					}
				}
				
			}
		});
    	return builder.create();
    }
    
    protected void aboutRoute() {
    	Intent intent = new Intent(mapActivity, ShowRouteInfoActivity.class);
		intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		mapActivity.startActivity(intent);
    }
    
    
    private boolean checkPointToNavigate(){
    	MapActivityLayers mapLayers = mapActivity.getMapLayers();
    	if(mapLayers.getNavigationLayer().getPointToNavigate() == null){
			// svk SpeechRecognition invocation problem 
    		//AccessibleToast.makeText(mapActivity, R.string.mark_final_location_first, Toast.LENGTH_LONG).show();
			return false;
		}
    	return true;
    }
    
    //// svk -> do  not show appl mode dialog (car,truck,bicycle..)
    // go directly to navigation
	protected void getDirections(final Location from /*, boolean followEnabled*/) {

		final RoutingHelper routingHelper = mapActivity.getRoutingHelper();
        /*
		Builder builder = new AlertDialog.Builder(mapActivity);

		View view = mapActivity.getLayoutInflater().inflate(R.layout.calculate_route, null);
		final ToggleButton[] buttons = new ToggleButton[ApplicationMode.values().length];
		buttons[ApplicationMode.CAR.ordinal()] = (ToggleButton) view.findViewById(R.id.CarButton);
		buttons[ApplicationMode.BICYCLE.ordinal()] = (ToggleButton) view.findViewById(R.id.BicycleButton);
		buttons[ApplicationMode.TRUCK.ordinal()] = (ToggleButton) view.findViewById(R.id.TruckButton);
		ApplicationMode appMode = settings.getApplicationMode();
		for (int i = 0; i < buttons.length; i++) {
			if (buttons[i] != null) {
				final int ind = i;
				ToggleButton b = buttons[i];
				b.setChecked(appMode == ApplicationMode.values()[i]);
				b.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
					@Override
					public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
						if (isChecked) {
							for (int j = 0; j < buttons.length; j++) {
								if (buttons[j] != null) {
									if (buttons[j].isChecked() != (ind == j)) {
										buttons[j].setChecked(ind == j);
									}
								}
							}
						} else {
							// revert state
							boolean revert = true;
							for (int j = 0; j < buttons.length; j++) {
								if (buttons[j] != null) {
									if (buttons[j].isChecked()) {
										revert = false;
										break;
									}
								}
							}
							if (revert) {
								buttons[ind].setChecked(true);
							}
						}
					}
				});
			}
		}

		DialogInterface.OnClickListener onlyShowCall = new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				ApplicationMode mode = getAppMode(buttons, settings);
				if (!checkPointToNavigate()) {
					return;
				}
				if (from == null) {
					AccessibleToast.makeText(mapActivity, R.string.unknown_from_location, Toast.LENGTH_LONG).show();
					return;
				}
				routingHelper.setAppMode(mode);
				settings.PREV_APPLICATION_MODE.set(settings.APPLICATION_MODE.get());
				settings.FOLLOW_THE_ROUTE.set(false);
				settings.FOLLOW_THE_GPX_ROUTE.set(null);
				routingHelper.setFollowingMode(false);
				routingHelper.setFinalAndCurrentLocation(mapActivity.getPointToNavigate(), from);
			}
		};

		DialogInterface.OnClickListener followCall = new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				if (!checkPointToNavigate()) {
					return;
				}
				boolean msg = true;
				Location current = from;
				if (!mapActivity.isPointAccurateForRouting(from)) {
					current = null;
				}
				Location lastKnownLocation = mapActivity.getLastKnownLocation();
				if (mapActivity.isPointAccurateForRouting(lastKnownLocation)) {
					current = lastKnownLocation;
					msg = false;
				}
				if (msg) {
					AccessibleToast.makeText(mapActivity, R.string.route_updated_loc_found, Toast.LENGTH_LONG).show();
				}
				ApplicationMode mode = getAppMode(buttons, settings);
				// change global settings
				settings.PREV_APPLICATION_MODE.set(settings.APPLICATION_MODE.get());
				boolean changed = settings.APPLICATION_MODE.set(mode);
				if (changed) {
					mapActivity.updateApplicationModeSettings();
					mapActivity.getMapView().refreshMap(true);
				}

				routingHelper.setAppMode(mode);
				settings.FOLLOW_THE_ROUTE.set(true);
				settings.FOLLOW_THE_GPX_ROUTE.set(null);
				routingHelper.setFollowingMode(true);
				routingHelper.setFinalAndCurrentLocation(mapActivity.getPointToNavigate(), current);
				dialog.dismiss();
				getMyApplication().showDialogInitializingCommandPlayer(mapActivity);
			}
		};

		DialogInterface.OnClickListener useGpxNavigation = new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				ApplicationMode mode = getAppMode(buttons, settings);
				navigateUsingGPX(mode);
			}
		};

		builder.setView(view);
		builder.setTitle(R.string.get_directions);
		if (followEnabled) {
			builder.setPositiveButton(R.string.follow, followCall);
			builder.setNeutralButton(R.string.gpx_navigation, useGpxNavigation);
			builder.setNegativeButton(R.string.only_show, onlyShowCall);
		} else {
			// view.findViewById(R.id.TextView).setVisibility(View.GONE);
			builder.setPositiveButton(R.string.show_gpx_route, onlyShowCall);
			builder.setNegativeButton(R.string.default_buttons_cancel, null);
		}
		builder.show();
		*/
		
		
		//svk-------------------------------------------------------------------------------
		//if(!speechRecog)
		{
			if (!checkPointToNavigate()) {
				return;
			}
		}
		boolean msg = true;
		Location current = from;
		if (!mapActivity.isPointAccurateForRouting(from)) {
			current = null;
		}
		Location lastKnownLocation = mapActivity.getLastKnownLocation();
		if (mapActivity.isPointAccurateForRouting(lastKnownLocation)) {
			current = lastKnownLocation;
			msg = false;
		}
		if (msg) {
			AccessibleToast.makeText(mapActivity, R.string.route_updated_loc_found, Toast.LENGTH_LONG).show();
		}
		
		ApplicationMode mode = settings.getApplicationMode();//getAppMode(buttons, settings);
		routingHelper.setAppMode(mode);
		settings.FOLLOW_THE_ROUTE.set(true);
		settings.FOLLOW_THE_GPX_ROUTE.set(null);
		routingHelper.setFollowingMode(true);
		
		RouteInfoLayer routeInfoLayer=mapActivity.getMapLayers().getRouteInfoLayer();
		routeInfoLayer.setVisible(true);    // svk explicitly set route navigation control visible
		
		routingHelper.setFinalAndCurrentLocation(mapActivity.getPointToNavigate(), current);
		//LatLon ll=new LatLon(from.getLatitude(),from.getLongitude());
		//routingHelper.setFinalAndCurrentLocation(ll, current);
		
		getMyApplication().showDialogInitializingCommandPlayer(mapActivity);
	}
    
    protected FCNavApplication getMyApplication() {
		return mapActivity.getMyApplication();
	}

    public void navigateUsingGPX(final ApplicationMode appMode) {
		final LatLon endForRouting = mapActivity.getPointToNavigate();
		final MapActivityLayers mapLayers = mapActivity.getMapLayers();
    	final RoutingHelper routingHelper = mapActivity.getRoutingHelper();
		mapLayers.selectGPXFileLayer(new CallbackWithObject<GPXFile>() {
			
			@Override
			public boolean processResult(final GPXFile result) {
				Builder builder = new AlertDialog.Builder(mapActivity);
				final boolean[] props = new boolean[]{false, false, false};
				builder.setMultiChoiceItems(new String[] { getString(R.string.gpx_option_reverse_route),
						getString(R.string.gpx_option_destination_point), getString(R.string.gpx_option_from_start_point) }, props,
						new OnMultiChoiceClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which, boolean isChecked) {
								props[which] = isChecked;
							}
						});
				builder.setPositiveButton(R.string.default_buttons_apply, new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						boolean reverse = props[0];
						boolean passWholeWay = props[2];
						boolean useDestination = props[1];
						GPXRouteParams gpxRoute = new GPXRouteParams(result, reverse, settings);
						
						Location loc = mapActivity.getLastKnownLocation();
						if(passWholeWay && loc != null){
							gpxRoute.setStartPoint(loc);
						}
						
						Location startForRouting = mapActivity.getLastKnownLocation();
						if(startForRouting == null){
							startForRouting = gpxRoute.getStartPointForRoute();
						}
						
						LatLon endPoint = endForRouting;
						if(endPoint == null || !useDestination){
							LatLon point = gpxRoute.getLastPoint();
							if(point != null){
								endPoint = point;
							}
							if(endPoint != null) {
								settings.setPointToNavigate(point.getLatitude(), point.getLongitude(), null);
								mapLayers.getNavigationLayer().setPointToNavigate(point);
							}
						}
						// change global settings
						boolean changed = settings.APPLICATION_MODE.set(appMode);
						if (changed) {
							mapActivity.updateApplicationModeSettings();	
						}
						mapActivity.getMapView().refreshMap(changed);
						if(endPoint != null){
							settings.FOLLOW_THE_ROUTE.set(true);
							settings.FOLLOW_THE_GPX_ROUTE.set(result.path);
							routingHelper.setFollowingMode(true);
							routingHelper.setFinalAndCurrentLocation(endPoint, startForRouting, gpxRoute);
							getMyApplication().showDialogInitializingCommandPlayer(mapActivity);
						}
					}
				});
				builder.setNegativeButton(R.string.default_buttons_cancel, null);
				builder.show();
				return true;
			}
		}, false, false);
	}
  /*  
    private ApplicationMode getAppMode(ToggleButton[] buttons, FCNavSettings settings){
    	for(int i=0; i<buttons.length; i++){
    		if(buttons[i] != null && buttons[i].isChecked() && i < ApplicationMode.values().length){
    			return ApplicationMode.values()[i];
    		}
    	}
    	return settings.getApplicationMode();
    }
     */
	public void saveDirections() {
		mapActivity.showDialog(DIALOG_SAVE_DIRECTIONS);
	}
	
	public static  Dialog createSaveDirections(Activity activity) {
		final FCNavApplication app = ((FCNavApplication) activity.getApplication());
		FCNavSettings settings = ((FCNavApplication) activity.getApplication()).getSettings();
		final File fileDir = settings.extendFlcnavPath(ResourceManager.GPX_PATH);
		final Dialog dlg = new Dialog(activity);
		dlg.setTitle(R.string.save_route_dialog_title);
		dlg.setContentView(R.layout.save_directions_dialog);
		final EditText edit = (EditText) dlg.findViewById(R.id.FileNameEdit);
		
		edit.setText("_" + MessageFormat.format("{0,date,yyyy-MM-dd}", new Date()) + "_");
		((Button) dlg.findViewById(R.id.Save)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				String name = edit.getText().toString();
				fileDir.mkdirs();
				File toSave = fileDir;
				if(name.length() > 0) {
					if(!name.endsWith(".gpx")){
						name += ".gpx";
					}
					toSave = new File(fileDir, name);
				}
				if(toSave.exists()){
					dlg.findViewById(R.id.DuplicateFileName).setVisibility(View.VISIBLE);					
				} else {
					dlg.dismiss();
					new SaveDirectionsAsyncTask(app).execute(toSave);
				}
			}
		});
		
		((Button) dlg.findViewById(R.id.Cancel)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				dlg.dismiss();
			}
		});
		
		
		return dlg;
	}
	
	
	private static class SaveDirectionsAsyncTask extends AsyncTask<File, Void, String> {
		
		private final FCNavApplication app;

		public SaveDirectionsAsyncTask(FCNavApplication app) {
			this.app = app;
		}

		@Override
		protected String doInBackground(File... params) {
			if (params.length > 0) {
				File file = params[0];
				GPXFile gpx = app.getRoutingHelper().generateGPXFileWithRoute();
				GPXUtilities.writeGpxFile(file, gpx, app);
				return app.getString(R.string.route_successfully_saved_at, file.getName());
			}
			return null;
		}
		
		@Override
		protected void onPostExecute(String result) {
			if(result != null){
				AccessibleToast.makeText(app, result, Toast.LENGTH_LONG).show();
			}
		}
		
	}
	
	public void contextMenuPoint(final double latitude, final double longitude, final ContextMenuAdapter iadapter, Object selectedObj) {
		final ContextMenuAdapter adapter = iadapter == null ? new ContextMenuAdapter(mapActivity) : iadapter;
		Builder builder = new AlertDialog.Builder(mapActivity);

		adapter.registerItem(R.string.context_menu_item_navigate_point, R.drawable.list_view_set_destination);
		adapter.registerItem(R.string.context_menu_item_directions, R.drawable.list_view_directions_to_here);
		adapter.registerItem(R.string.context_menu_item_show_route, R.drawable.list_view_show_route_from_here);
		adapter.registerItem(R.string.context_menu_item_search, R.drawable.list_view_search_near_here);
		adapter.registerItem(R.string.context_menu_item_share_location, R.drawable.list_view_share_location);
		adapter.registerItem(R.string.context_menu_item_add_favorite, R.drawable.list_activities_favorites);

		FCNavPlugin.registerMapContextMenu(mapActivity, latitude, longitude, adapter, selectedObj);

		ListAdapter listadapter = new ArrayAdapter<String>(mapActivity, R.layout.layers_list_activity_item, R.id.title,
				adapter.getItemNames()) {
			@Override
			public View getView(int position, View convertView, ViewGroup parent) {
				// User super class to create the View
				View v = super.getView(position, convertView, parent);
				TextView tv = (TextView) v.findViewById(R.id.title);
				tv.setText(adapter.getItemName(position));

				// Put the image on the TextView
				if (adapter.getImageId(position) != 0) {
					tv.setCompoundDrawablesWithIntrinsicBounds(adapter.getImageId(position), 0, 0, 0);
				} else {
					tv.setCompoundDrawablesWithIntrinsicBounds(R.drawable.list_activities_transparent, 0, 0, 0);
				}

				final CheckBox ch = ((CheckBox) v.findViewById(R.id.check_item));
				ch.setVisibility(View.GONE);
				return v;
			}
		};

		builder.setAdapter(listadapter, new DialogInterface.OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {
				int standardId = adapter.getItemId(which);
				OnContextMenuClick click = adapter.getClickAdapter(which);
				if (click != null) {
					click.onContextMenuClick(standardId, which, false, dialog);
				} else if (standardId == R.string.context_menu_item_search) {
					Intent intent = new Intent(mapActivity, FCNavIntents.getSearchActivity());
					intent.putExtra(SearchActivity.SEARCH_LAT, latitude);
					intent.putExtra(SearchActivity.SEARCH_LON, longitude);
					intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
					mapActivity.startActivity(intent);
				} else if (standardId == R.string.context_menu_item_navigate_point) {
					mapActivity.navigateToPoint(new LatLon(latitude, longitude));
				} else if (standardId == R.string.context_menu_item_directions) {
					Location loc = mapActivity.getLastKnownLocation();
					mapActivity.navigateToPoint(new LatLon(latitude, longitude));
					// always enable and follow and let calculate it (GPS is not accessible in garage)
					getDirections(loc/*, true*/);
				} else if (standardId == R.string.context_menu_item_show_route) {
					if (checkPointToNavigate()) {
						Location loc = new Location("map");
						loc.setLatitude(latitude);
						loc.setLongitude(longitude);
						getDirections(loc/*, true*/);        // svk -> do  not show appl mode dialog (car,truck,bicycle..)
					}
				} else if (standardId == R.string.context_menu_item_share_location) {
					shareLocation(latitude, longitude, mapActivity.getMapView().getZoom());
				} else if (standardId == R.string.context_menu_item_add_favorite) {
					addFavouritePoint(latitude, longitude);
				}
			}
		});
		builder.create().show();
	}
	
	public void contextMenuPoint(final double latitude, final double longitude){
		contextMenuPoint(latitude, longitude, null, null);
	}
	
	private Dialog createReloadTitleDialog(final Bundle args) {
    	Builder builder = new AccessibleAlertBuilder(mapActivity);
    	builder.setMessage(R.string.context_menu_item_update_map_confirm);
    	builder.setNegativeButton(R.string.default_buttons_cancel, null);
    	final FCNavMapTileView mapView = mapActivity.getMapView();
    	builder.setPositiveButton(R.string.context_menu_item_update_map, new DialogInterface.OnClickListener(){
			@Override
			public void onClick(DialogInterface dialog, int which) {
				int zoom = args.getInt(KEY_ZOOM);
				BaseMapLayer mainLayer = mapView.getMainLayer();
				if(!(mainLayer instanceof MapTileLayer) || !((MapTileLayer) mainLayer).isVisible()){
					AccessibleToast.makeText(mapActivity, R.string.maps_could_not_be_downloaded, Toast.LENGTH_SHORT).show();
					return;
				}
				final ITileSource mapSource = ((MapTileLayer) mainLayer).getMap();
				if(mapSource == null || !mapSource.couldBeDownloadedFromInternet()){
					AccessibleToast.makeText(mapActivity, R.string.maps_could_not_be_downloaded, Toast.LENGTH_SHORT).show();
					return;
				}
				Rect pixRect = new Rect(0, 0, mapView.getWidth(), mapView.getHeight());
		    	RectF tilesRect = new RectF();
		    	mapView.calculateTileRectangle(pixRect, mapView.getCenterPointX(), mapView.getCenterPointY(), 
		    			mapView.getXTile(), mapView.getYTile(), tilesRect);
		    	int left = (int) FloatMath.floor(tilesRect.left);
				int top = (int) FloatMath.floor(tilesRect.top);
				int width = (int) (FloatMath.ceil(tilesRect.right) - left);
				int height = (int) (FloatMath.ceil(tilesRect.bottom) - top);
				for (int i = 0; i <width; i++) {
					for (int j = 0; j< height; j++) {
						((FCNavApplication)mapActivity.getApplication()).getResourceManager().
								clearTileImageForMap(null, mapSource, i + left, j + top, zoom);	
					}
				}
				
				
				mapView.refreshMap();
			}
    	});
    	builder.setNeutralButton(R.string.context_menu_item_update_poi, new DialogInterface.OnClickListener(){
			@Override
			public void onClick(DialogInterface dialog, int which) {
				updatePoiDb(args.getInt(KEY_ZOOM), args.getDouble(KEY_LATITUDE), args.getDouble(KEY_LONGITUDE));
			}
    	});
		return builder.create();
    }
	
	

	@Override
	public Dialog onCreateDialog(int id) {
		Bundle args = dialogBundle;
		switch (id) {
			case DIALOG_ADD_FAVORITE:
				return createAddFavouriteDialog(args);
			case DIALOG_REPLACE_FAVORITE:
				return createReplaceFavouriteDialog(args);
			case DIALOG_ADD_WAYPOINT:
				return createAddWaypointDialog(args);
			case DIALOG_RELOAD_TITLE:
				return createReloadTitleDialog(args);
			case DIALOG_SHARE_LOCATION:
				return createShareLocationDialog(args);
			case DIALOG_SAVE_DIRECTIONS:
				return createSaveDirections(mapActivity);
		}
		return null;
	}

	@Override
	public void onPrepareDialog(int id, Dialog dialog) {
		Bundle args = dialogBundle;
		switch (id) {
		case DIALOG_ADD_FAVORITE:
			prepareAddFavouriteDialog(dialog, args);
			break;
		case DIALOG_ADD_WAYPOINT:
			EditText v = (EditText) dialog.getWindow().findViewById(R.id.TextView);
			v.setPadding(5, 0, 5, 0);
			if(args.getString(KEY_NAME) != null) {
				v.setText(args.getString(KEY_NAME));
			} else {
				v.setText("");
			}
			break;
		}
	}
	
	public boolean onCreateOptionsMenu(Menu menu) {
//		NOTE: delete not a "menu.xml" because all id-resources are generated to R-class from there
		
		OptionsMenuHelper helper = new OptionsMenuHelper(menu);
		helper.registerOptionsMenuItem(R.id.map_where_am_i, R.string.where_am_i, android.R.drawable.ic_menu_mylocation);
		helper.registerOptionsMenuItem(R.id.map_layers, R.string.menu_layers, android.R.drawable.ic_menu_mapmode);		
		helper.registerOptionsMenuItem(R.id.map_show_settings, R.string.settings_Button, android.R.drawable.ic_menu_preferences);		
		helper.registerOptionsMenuItem(R.id.map_navigate_to_point, R.string.stop_navigation, android.R.drawable.ic_menu_close_clear_cancel, false);		
		helper.registerOptionsMenuItem(R.id.map_mute, R.string.menu_mute_off, false);		
		//helper.registerOptionsMenuItem(R.id.map_get_directions, R.string.get_directions, android.R.drawable.ic_menu_directions);
		helper.registerOptionsMenuItem(R.id.map_animate_route, R.string.animate_route, false);
		helper.registerOptionsMenuItem(R.id.map_specify_point, R.string.search_button, android.R.drawable.ic_menu_search);
		//helper.registerOptionsMenuItem(R.id.map_show_gps_status, R.string.show_gps_status, android.R.drawable.ic_menu_compass);
		//helper.registerOptionsMenuItem(R.id.map_show_point_options, R.string.show_point_options);		
		FCNavPlugin.registerOptionsMenu(mapActivity, helper);
		
		return true;
	}
	
	public void onPrepareOptionsMenu(Menu menu) {
		
		MenuItem navigateToPointMenu = menu.findItem(R.id.map_navigate_to_point);
		if (navigateToPointMenu != null) {
			if (settings.getPointToNavigate() != null) {
				navigateToPointMenu.setTitle((routingHelper.isRouteCalculated() || routingHelper.isFollowingMode() || 
						routingHelper.isRouteBeingCalculated()) ? R.string.stop_routing : R.string.stop_navigation);
				navigateToPointMenu.setVisible(true);
			} else {
				navigateToPointMenu.setVisible(false);
			}
		}
		MenuItem muteMenu = menu.findItem(R.id.map_mute); 
		if(muteMenu != null){
			if (routingHelper.getFinalLocation() != null && routingHelper.isFollowingMode()) {
				boolean mute = routingHelper.getVoiceRouter().isMute();
				muteMenu.setTitle(mute ? R.string.menu_mute_on : R.string.menu_mute_off);
				muteMenu.setIcon(mute ? R.drawable.ic_menu_soundoff : R.drawable.ic_menu_soundon);
				muteMenu.setVisible(true);
			} else {
				muteMenu.setVisible(false);
			}
		}
		
		/*  svk we show route from toolbar
		MenuItem directions = menu.findItem(R.id.map_get_directions);
		if(routingHelper.isRouteCalculated()){
			directions.setTitle(R.string.show_route);
		} else {
			directions.setTitle(R.string.get_directions);
		}
		*/
		MenuItem animateMenu = menu.findItem(R.id.map_animate_route);
		
		if (animateMenu != null) {
			if(settings.TEST_ANIMATE_ROUTING.get()){
				animateMenu.setTitle(routeAnimation.isRouteAnimating() ? R.string.animate_route_off
					: R.string.animate_route);
				animateMenu.setVisible("1".equals(Secure.getString(
					mapActivity.getContentResolver(), Secure.ALLOW_MOCK_LOCATION))
					&& settings.getPointToNavigate() != null
					&& routingHelper.isRouteCalculated());
				animateMenu.setVisible(true);
			} else {
				animateMenu.setVisible(false);
			}
		}
		
		FCNavPlugin.registerOnPrepareOptionsMenu(mapActivity, menu);
		
	}
	
	public boolean onOptionsItemSelected(MenuItem item) {
		
		final int itemId = item.getItemId();
		FCNavMapTileView mapView = mapActivity.getMapView();
		if (itemId == R.id.map_show_settings) {
			final Intent intentSettings = new Intent(mapActivity, FCNavIntents.getSettingsActivity());
			mapActivity.startActivity(intentSettings);
			return true;
		} else if (itemId == R.id.map_where_am_i) {
			if (getMyApplication().accessibilityEnabled()) {
				whereAmIDialog();
			} else {
				mapActivity.backToLocationImpl();
			}
			return true;
		} else 
		if (false/*itemId == R.id.map_show_gps_status*/) {
			//startGpsStatusIntent();
			return true;
		} else if (itemId == R.id.map_specify_point) {
			Intent newIntent = new Intent(mapActivity, FCNavIntents.getSearchActivity());
			// causes wrong position caching:  newIntent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
			LatLon loc = mapActivity.getMapLocation();
			newIntent.putExtra(SearchActivity.SEARCH_LAT, loc.getLatitude());
			newIntent.putExtra(SearchActivity.SEARCH_LON, loc.getLongitude());
			newIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			mapActivity.startActivity(newIntent);
			return true;
		} else {
			if (itemId == R.id.map_get_directions) {
				if (routingHelper.isRouteCalculated()) {
					aboutRoute();
				} else {
					Location loc = mapActivity.getLastKnownLocation();
					getDirections(loc/*, true*/);
				}
				return true;
			} else if (itemId == R.id.map_layers) {
				mapActivity.getMapLayers().openLayerSelectionDialog(mapView);
				return true;
			} else if (itemId == R.id.map_mute) {
				routingHelper.getVoiceRouter().setMute(!routingHelper.getVoiceRouter().isMute());
				return true;
			} else if (itemId == R.id.map_navigate_to_point) {
				if (mapActivity.getMapLayers().getNavigationLayer().getPointToNavigate() != null) {
					if (routingHelper.isRouteCalculated() || routingHelper.isFollowingMode() || routingHelper.isRouteBeingCalculated()) {
						routingHelper.setFinalAndCurrentLocation(null, routingHelper.getCurrentLocation(), routingHelper.getCurrentGPXRoute());
						// restore default mode
						boolean changed = settings.APPLICATION_MODE.set(settings.PREV_APPLICATION_MODE.get());
						mapActivity.updateApplicationModeSettings();
						mapView.refreshMap(changed);
					} else {
						mapActivity.navigateToPoint(null);
					}
				} else {
					mapActivity.navigateToPoint(new LatLon(mapView.getLatitude(), mapView.getLongitude()));
				}
				mapView.refreshMap();
				return true;
			} else if (false/*itemId == R.id.map_show_point_options*/) {
				//contextMenuPoint(mapView.getLatitude(), mapView.getLongitude());
				return true;
			} else if (itemId == R.id.map_animate_route) {
				// animate moving on route
				routeAnimation.startStopRouteAnimation(routingHelper, mapActivity);
				return true;
			} else {
				return FCNavPlugin.registerOnOptionsMenuItemSelected(mapActivity, itemId);
			}
		}
		
		
	}
    
	/*
	private void startGpsStatusIntent() {
		Intent intent = new Intent();
		intent.setComponent(new ComponentName(GPS_STATUS_COMPONENT,
				GPS_STATUS_ACTIVITY));
		ResolveInfo resolved = mapActivity.getPackageManager().resolveActivity(intent,
				PackageManager.MATCH_DEFAULT_ONLY);
		if (resolved != null) {
			mapActivity.startActivity(intent);
		} else {
			AlertDialog.Builder builder = new AccessibleAlertBuilder(mapActivity);
			builder.setMessage(getString(R.string.gps_status_app_not_found));
			builder.setPositiveButton(getString(R.string.default_buttons_yes), new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("market://search?q=pname:" + GPS_STATUS_COMPONENT));
					try {
						mapActivity.startActivity(intent);
					} catch (ActivityNotFoundException e) {
					}
				}
			});
			builder.setNegativeButton(
					getString(R.string.default_buttons_no), null);
			builder.show();
		}
	}
    */
    private void whereAmIDialog() {
        final List<String> items = new ArrayList<String>();
        items.add(getString(R.string.show_location));
        items.add(getString(R.string.show_details));
        AlertDialog.Builder menu = new AlertDialog.Builder(mapActivity);
        menu.setItems(items.toArray(new String[items.size()]),
                      new DialogInterface.OnClickListener() {
                          @Override
                          public void onClick(DialogInterface dialog, int item) {
                              dialog.dismiss();
                              switch (item) {
                              case 0:
                                  mapActivity.backToLocationImpl();
                                  break;
                              case 1:
                                  mapActivity.getNavigationInfo().show(settings.getPointToNavigate(), mapActivity.getMapLayers().
                                		  getLocationLayer().getHeading());
                                  break;
                              default:
                                  break;
                              }
                          }
                      });
        menu.show();
    }
    //------------------------Speech Recognition stuff------------------------------------------
    public void DirectionsFromSpeechCmd()
    {
    	
    	Location loc=new Location("myRoute");
    	double latitude=43.5678, longitude=-79.5555;
    	loc.setLatitude(latitude);
    	loc.setLongitude(longitude);
    	mapActivity.navigateToPoint(new LatLon(latitude, longitude));
		getDirections(loc/*, true*/);
    }
    public void CancelRoute()
    {
    	mapActivity.navigateToPoint(null);
    
    }
}
