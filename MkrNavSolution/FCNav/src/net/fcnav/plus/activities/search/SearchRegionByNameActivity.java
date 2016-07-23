package net.fcnav.plus.activities.search;

import java.text.Collator;
import java.util.ArrayList;
import java.util.Comparator;

import net.fcnav.access.AccessibleToast;
import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.R;
import net.fcnav.plus.RegionAddressRepository;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;


public class SearchRegionByNameActivity extends SearchByNameAbstractActivity<RegionAddressRepository> {
	
	@Override
	protected Comparator<? super RegionAddressRepository> createComparator() {
		return new Comparator<RegionAddressRepository>() {
			Collator col = Collator.getInstance();
			@Override
			public int compare(RegionAddressRepository lhs,
					RegionAddressRepository rhs) {
				return col.compare(lhs.getName(), rhs.getName());
			}
		};
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		((TextView)findViewById(R.id.Label)).setText(R.string.choose_available_region);
		if(((FCNavApplication)getApplication()).getResourceManager().getAddressRepositories().isEmpty()){
			AccessibleToast.makeText(this, R.string.none_region_found, Toast.LENGTH_LONG).show();
		}
		initialListToFilter = new ArrayList<RegionAddressRepository>(((FCNavApplication)getApplication()).getResourceManager().getAddressRepositories());
		NamesAdapter namesAdapter = new NamesAdapter(new ArrayList<RegionAddressRepository>(initialListToFilter),createComparator()); //$NON-NLS-1$
		setListAdapter(namesAdapter);
	}
	
	
	@Override
	public String getText(RegionAddressRepository obj) {
		return obj.getName();
	}

	@Override
	public void itemSelected(RegionAddressRepository obj) {
		((FCNavApplication) getApplication()).getSettings().setLastSearchedRegion(obj.getName(), obj.getEstimatedRegionCenter());
		finish();
	}
	
}
