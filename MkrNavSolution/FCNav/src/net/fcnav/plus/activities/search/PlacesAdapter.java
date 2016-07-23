package net.fcnav.plus.activities.search;

import java.util.List;

import net.fcnav.FCNavFormatter;
import net.fcnav.plus.activities.search.SearchAddressOnlineActivity.Place;
import net.flcnav.osm.LatLon;
import net.flcnav.osm.MapUtils;
import net.fcnav.plus.R;
import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class PlacesAdapter extends ArrayAdapter<Place> {

	Context context=null;
	int item_id=0;
	private LatLon location;
	public PlacesAdapter(Context ctx,int id,List<Place> places,LatLon _location) {
		
		super(ctx, id, places);
		context=ctx;
		item_id=id;
		location=_location;
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View row = convertView;
		if (row == null) {
			LayoutInflater inflater = ((Activity)context).getLayoutInflater();
			row = inflater.inflate(item_id, parent, false);
		}
		Place model = getItem(position);
		TextView label = (TextView) row.findViewById(R.id.label);
		TextView distanceLabel = (TextView) row.findViewById(R.id.distance_label);
		if(location != null){
			int dist = (int) (MapUtils.getDistance(location, model.lat, model.lon));
			distanceLabel.setText(FCNavFormatter.getFormattedDistance(dist, context));
		} else {
			distanceLabel.setText(""); //$NON-NLS-1$
		}
		label.setText(model.displayName);
		return row;
	}
	
}

