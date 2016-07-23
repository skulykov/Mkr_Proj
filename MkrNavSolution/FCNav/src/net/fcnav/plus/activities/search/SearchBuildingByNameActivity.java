package net.fcnav.plus.activities.search;

import java.util.Comparator;
import java.util.List;

import net.flcnav.Algoritms;
import net.flcnav.ResultMatcher;
import net.flcnav.data.Building;
import net.flcnav.data.City;
import net.flcnav.data.Street;
import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.R;
import net.fcnav.plus.RegionAddressRepository;
import android.os.AsyncTask;
import android.view.View;
import android.widget.TextView;

public class SearchBuildingByNameActivity extends SearchByNameAbstractActivity<Building> {
	private RegionAddressRepository region;
	private City city;
	private Street street;
	
	@Override
	protected Comparator<? super Building> createComparator() {
		return new Comparator<Building>() {
			@Override
			public int compare(Building o1, Building o2) {
				int i1 = Algoritms.extractFirstIntegerNumber(o1.getName());
				int i2 = Algoritms.extractFirstIntegerNumber(o2.getName());
				return i1 - i2;
			}
		};
	}
	
	@Override
	public AsyncTask<Object, ?, ?> getInitializeTask() {
		return new AsyncTask<Object, Void, List<Building>>(){
			@Override
			protected void onPostExecute(List<Building> result) {
				((TextView)findViewById(R.id.Label)).setText(R.string.incremental_search_building);
				progress.setVisibility(View.INVISIBLE);
				finishInitializing(result);
			}
			
			@Override
			protected void onPreExecute() {
				((TextView)findViewById(R.id.Label)).setText(R.string.loading_streets_buildings);
				progress.setVisibility(View.VISIBLE);
			}
			@Override
			protected List<Building> doInBackground(Object... params) {
				region = ((FCNavApplication)getApplication()).getResourceManager().getRegionRepository(settings.getLastSearchedRegion());
				if(region != null){
					city = region.getCityById(settings.getLastSearchedCity(), settings.getLastSearchedCityName());
					street = region.getStreetByName(city, settings.getLastSearchedStreet());
				}
				if(street != null){
					// preload here to avoid concurrent modification
					region.preloadBuildings(street, new ResultMatcher<Building>() {
						@Override
						public boolean isCancelled() {
							return false;
						}

						@Override
						public boolean publish(Building object) {
							addObjectToInitialList(object);
							return true;
						}
					});
					return street.getBuildings();
				}
				return null;
			}
		};
	}
	
	@Override
	public String getText(Building obj) {
		return obj.getName(region.useEnglishNames());
	}
	
	@Override
	public void itemSelected(Building obj) {
		settings.setLastSearchedBuilding(obj.getName(region.useEnglishNames()), obj.getLocation());
		finish();
		
	}



}
