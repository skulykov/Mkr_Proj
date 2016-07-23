package net.fcnav.plus.activities;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.FCNavPlugin;
import net.fcnav.plus.R;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class PluginsActivity extends FCNavListActivity {
	
	private List<FCNavPlugin> availablePlugins;
	private Set<String> enabledPlugins = new LinkedHashSet<String>();
	private Set<String> restartPlugins = new LinkedHashSet<String>();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		CustomTitleBar titleBar = new CustomTitleBar(this, R.string.plugins_screen, R.drawable.tab_plugin_screen_icon);
		setContentView(R.layout.plugins);
		
		availablePlugins = FCNavPlugin.getAvailablePlugins();
		List<FCNavPlugin> enabledPlugins = FCNavPlugin.getEnabledPlugins();
		for(FCNavPlugin p : enabledPlugins) {
			restartPlugins.add(p.getId());
			this.enabledPlugins.add(p.getId());
		}
		
		titleBar.afterSetContentView();
		setListAdapter(new FCNavPluginsAdapter(availablePlugins));
	}
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		super.onListItemClick(l, v, position, id);
		FCNavPlugin item = getListAdapter().getItem(position);
		boolean enable = !restartPlugins.contains(item.getId());

		boolean ok = FCNavPlugin.enablePlugin(((FCNavApplication) getApplication()), item, enable);
		if (ok) {
			if (!enable) {
				restartPlugins.remove(item.getId());
			} else {
				restartPlugins.add(item.getId());
			}
		}
		getListAdapter().notifyDataSetChanged();
	}
	
	@Override
	public FCNavPluginsAdapter getListAdapter() {
		return (FCNavPluginsAdapter) super.getListAdapter();
	}
	
	protected class FCNavPluginsAdapter extends ArrayAdapter<FCNavPlugin> {
		
		public FCNavPluginsAdapter(List<FCNavPlugin> plugins) {
			super(PluginsActivity.this, R.layout.plugins_list_item, plugins);
		}

		@Override
		public View getView(final int position, View convertView, ViewGroup parent) {
			View v = convertView;
			if (v == null) {
				LayoutInflater inflater = getLayoutInflater();
				v = inflater.inflate(net.fcnav.plus.R.layout.plugins_list_item, parent, false);
			}
			final View row = v;
			/*
			FCNavPlugin plugin = getItem(position);
			TextView nameView = (TextView) row.findViewById(R.id.plugin_name);
			nameView.setText(plugin.getName());
			
			TextView description = (TextView) row.findViewById(R.id.plugin_descr);
			description.setText(plugin.getDescription());
			boolean toBeEnabled = restartPlugins.contains(plugin.getId());
			int right = toBeEnabled ? R.drawable.list_activities_dot_marker2_pressed : R.drawable.list_activities_dot_marker1_pressed;
			nameView.setCompoundDrawablesWithIntrinsicBounds(getResources().getDrawable(R.drawable.list_activities_plugin_menu_symbol), null, getResources().getDrawable(right), null);
            */
			return row;
		}
		
	}
	

}
