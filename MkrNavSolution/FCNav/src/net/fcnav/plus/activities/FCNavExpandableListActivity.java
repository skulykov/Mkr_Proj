package net.fcnav.plus.activities;

import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.R;
import android.app.ExpandableListActivity;
import android.widget.ExpandableListView;

public abstract class FCNavExpandableListActivity extends
		ExpandableListActivity {

	@Override
	protected void onResume() {
		super.onResume();
		
		ExpandableListView view = getExpandableListView();
		view.setCacheColorHint(getResources().getColor(R.color.activity_background));
		view.setDivider(getResources().getDrawable(R.drawable.tab_text_separator));
	}

	protected FCNavApplication getMyApplication() {
		return (FCNavApplication)getApplication();
	}
}
