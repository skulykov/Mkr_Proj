package net.fcnav.plus.views;

import java.util.ArrayList;
import java.util.List;

import net.londatiga.android.ActionItem;
import net.londatiga.android.QuickAction;
import net.fcnav.access.AccessibleToast;
import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.FCNavSettings;
import net.fcnav.plus.R;
import net.fcnav.plus.activities.MapActivity;
import net.fcnav.plus.activities.ShowRouteInfoActivity;

import net.fcnav.plus.routing.RouteCalculationResult;
import net.fcnav.plus.routing.RouteDirectionInfo;
import net.fcnav.plus.routing.RoutingHelper;
import net.flcnav.osm.MapUtils;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.Intent;
import android.content.DialogInterface.OnClickListener;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.location.Location;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class mRouteInfoControl {
	
	protected boolean isScreenLocked;
//	private View transparentLockView;
	private Drawable lockEnabled;
	private Drawable lockDisabled;
	MapActivity mapAct=null;
	private List<mRouteInfoControlActions> lockActions = new ArrayList<mRouteInfoControl.mRouteInfoControlActions>();
	
	public interface mRouteInfoControlActions {
		
		public void addLockActions(QuickAction qa, mRouteInfoControl li, FCNavMapTileView view);
	}
	
	public void addLockActions(mRouteInfoControlActions la){
		lockActions.add(la);
	}
	
	
	public List<mRouteInfoControlActions> getLockActions() {
		return lockActions;
	}
	
	public ImageView createLockScreenWidget(final MapActivity _map,final FCNavMapTileView view) {
		final ImageView lockView = new ImageView(view.getContext());
		mapAct=_map;
		lockEnabled = view.getResources().getDrawable(R.drawable.list_activities_route);
		lockDisabled = view.getResources().getDrawable(R.drawable.list_activities_gpx_tracks);
		updateLockIcon(view, lockView);
		lockView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				showBgServiceQAction(lockView, view);
				
				//Intent intent = new Intent(view.getContext(), ShowRouteInfoActivity.class);
				//intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
				//view.getContext().startActivity(intent);
			}
		});
		return lockView;
	}

	private void updateLockIcon(final FCNavMapTileView view, final ImageView lockView) {
		if (isScreenLocked) {
			lockView.setBackgroundDrawable(lockEnabled);
		} else {
			lockView.setBackgroundDrawable(lockDisabled);
		}
	}

	private void showBgServiceQAction(final ImageView lockView, final FCNavMapTileView view) {	
		
		RoutingHelper helper=view.getApplication().getRoutingHelper();
		
		if(helper.isRouteCalculated())
		{
			RouteCalculationResult route=helper.getRoute();
			List<RouteDirectionInfo> ll=route.getDirections();
			
			/* this sets first point to center of the map
			RouteDirectionInfo item = ll.get(0);
			Location loc = helper.getLocationFromRouteDirection(item);
			
			mapAct.setMapLocation(loc.getLatitude(), loc.getLongitude());
			*/
			
			RectF box=new RectF();
			box.bottom=(float)90.0;
			box.top=(float)0.0;
			box.left=(float)0.0;
			box.right=(float)-180.0;
			float lat,lon;
			for(RouteDirectionInfo item : ll)
			{
				Location loc = helper.getLocationFromRouteDirection(item);
				lat=(float)loc.getLatitude();
				lon=(float)loc.getLongitude();
				if(lat>box.top)
					box.top=lat;
				if(lat<box.bottom)
					box.bottom=lat;
				
				if(lon<box.left)
					box.left=lon;
				if(lon>box.right)
					box.right=lon;
			}
			lat=(box.bottom+box.top)/2;
			lon=(box.left+box.right)/2;
			
			FCNavMapTileView mapView=mapAct.getMapView();
			float mZoom=mapView.getZoom();
			//int tileSize = mapView.getSourceTileSize();
			
			float mStX = (float) ((MapUtils.getTileNumberX(mZoom, box.left) - MapUtils.getTileNumberX(mZoom, box.right))/* * tileSize*/);
			float mStY = (float) ((MapUtils.getTileNumberY(mZoom, box.top) - MapUtils.getTileNumberY(mZoom, box.bottom))/* * tileSize*/);
		
			boolean dx=true;
			if(Math.abs(mStX)<4.0 && Math.abs(mStY) < 3.0)
			  dx=false;
			while (/*Math.abs(mStX)>4 && Math.abs(mStY) > 3*/true) {
				if(dx)
				{
					if(Math.abs(mStX) < 4.0 && Math.abs(mStY) < 3.0)
						break;
					mZoom--;
				}
				else
				{
					if(Math.abs(mStX) >= 4.0 && Math.abs(mStY) >= 3.0)
						break;
					mZoom++;
				}
				if(mZoom <= 4 || mZoom>=20){
					break;//skipAnimation = true;
				}
				mStX = (float) ((MapUtils.getTileNumberX(mZoom, box.left) - MapUtils.getTileNumberX(mZoom, box.right)));
				mStY = (float) ((MapUtils.getTileNumberY(mZoom, box.top) - MapUtils.getTileNumberY(mZoom, box.bottom)));
			}
			final float moveZoom = mZoom;
			
			mapAct.setMapLocation(lat, lon);
			mapAct.changeZoom(moveZoom);
		}
		
		
		
	}
	
	
	
	
}
