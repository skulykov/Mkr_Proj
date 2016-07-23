package net.fcnav.plus.activities;

import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.R;
import android.app.ListActivity;
import android.widget.ListView;

public abstract class FCNavListActivity extends ListActivity {

	@Override
	protected void onResume() {
		super.onResume();
		
		ListView view = getListView();
		view.setCacheColorHint(getResources().getColor(R.color.activity_background));
		view.setDivider(getResources().getDrawable(R.drawable.tab_text_separator));
	}
	
	protected FCNavApplication getMyApplication() {
		return (FCNavApplication) getApplication();
	}
}
