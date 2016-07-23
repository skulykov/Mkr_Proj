package net.fcnav.plus.activities;

import java.io.File;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import net.flcnav.ResultMatcher;
import net.fcnav.Version;
import net.flcnav.map.TileSourceManager;
import net.flcnav.map.TileSourceManager.TileSourceTemplate;
import net.fcnav.access.AccessibleToast;
import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.FCNavPlugin;
import net.fcnav.plus.FCNavSettings;
import net.fcnav.plus.FCNavSettings.CommonPreference;
import net.fcnav.plus.FCNavSettings.DayNightMode;
import net.fcnav.plus.FCNavSettings.MetricsConstants;
import net.fcnav.plus.FCNavSettings.FlcnavPreference;
import net.fcnav.plus.routing.RouteProvider.RouteService;
import net.fcnav.plus.views.SeekBarPreference;
import net.fcnav.plus.ProgressDialogImplementation;
import net.fcnav.plus.R;
import net.fcnav.plus.ResourceManager;
import net.flcnav.plus.render.NativeFlcnavLibrary;
import net.flcnav.render.RenderingRuleProperty;
import net.flcnav.render.RenderingRulesStorage;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.Preference.OnPreferenceClickListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.widget.Toast;

public class SettingsActivity extends PreferenceActivity implements OnPreferenceChangeListener, OnPreferenceClickListener {
	
	public static final String INTENT_KEY_SETTINGS_SCREEN = "INTENT_KEY_SETTINGS_SCREEN";
	public static final int SCREEN_GENERAL_SETTINGS = 1;
	public static final int SCREEN_NAVIGATION_SETTINGS = 2;
	public static final int SCREEN_MONITORING_SETTINGS = 3;
	
	public static final String SCREEN_ID_GENERAL_SETTINGS = "general_settings";
	public static final String SCREEN_ID_NAVIGATION_SETTINGS = "routing_settings";
	public static final String SCREEN_ID_MONITORING_SETTINGS = "monitor_settings";
	public static final String MORE_VALUE = "MORE_VALUE";
	
//	private Preference bidforfix;
	private Preference plugins;

	private EditTextPreference applicationDir;
	private ListPreference applicationModePreference;

	private ListPreference dayNightModePreference;
	private ListPreference routerServicePreference;

	public ProgressDialog progressDlg;
	
	private FCNavSettings flcnavSettings;
	
	private Map<String, Preference> screenPreferences = new LinkedHashMap<String, Preference>();
	private Map<String, FlcnavPreference<Boolean>> booleanPreferences = new LinkedHashMap<String, FlcnavPreference<Boolean>>();
	private Map<String, FlcnavPreference<?>> listPreferences = new LinkedHashMap<String, FlcnavPreference<?>>();
	private Map<String, FlcnavPreference<String>> editTextPreferences = new LinkedHashMap<String, FlcnavPreference<String>>();
	private Map<String, FlcnavPreference<Integer>> seekBarPreferences = new LinkedHashMap<String, FlcnavPreference<Integer>>();
	
	private Map<String, Map<String, ?>> listPrefValues = new LinkedHashMap<String, Map<String, ?>>();
	
	public CheckBoxPreference registerBooleanPreference(FlcnavPreference<Boolean> b, PreferenceScreen screen){
		CheckBoxPreference p = (CheckBoxPreference) screen.findPreference(b.getId());
		p.setOnPreferenceChangeListener(this);
		screenPreferences.put(b.getId(), p);
		booleanPreferences.put(b.getId(), b);
		return p;
	}
	
	public CheckBoxPreference createCheckBoxPreference(FlcnavPreference<Boolean> b, int title, int summary){
		CheckBoxPreference p = new CheckBoxPreference(this);
		p.setTitle(title);
		p.setKey(b.getId());
		p.setSummary(summary);
		p.setOnPreferenceChangeListener(this);
		screenPreferences.put(b.getId(), p);
		booleanPreferences.put(b.getId(), b);
		return p;
	}
	
	public void registerSeekBarPreference(FlcnavPreference<Integer> b, PreferenceScreen screen){
		SeekBarPreference p = (SeekBarPreference) screen.findPreference(b.getId());
		p.setOnPreferenceChangeListener(this);
		screenPreferences.put(b.getId(), p);
		seekBarPreferences.put(b.getId(), b);
	}
	
	public String getStringPropertyName(String propertyName, String defValue) {
		try {
			Field f = R.string.class.getField("rendering_attr_"+propertyName+"_name");
			if(f != null) {
				Integer in = (Integer) f.get(null);
				return getString(in);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return defValue;
	}
	
	public String getStringPropertyDescription(String propertyName, String defValue) {
		try {
			Field f = R.string.class.getField("rendering_attr_"+propertyName+"_description");
			if(f != null) {
				Integer in = (Integer) f.get(null);
				return getString(in);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return defValue;
	}
	
	public SeekBarPreference createSeekBarPreference(FlcnavPreference<Integer> b, int title, int summary, int dialogTextId,
			int defValue, int maxValue){
		SeekBarPreference p = new SeekBarPreference(this, dialogTextId, defValue, maxValue);
		p.setTitle(title);
		p.setKey(b.getId());
		p.setSummary(summary);
		p.setOnPreferenceChangeListener(this);
		screenPreferences.put(b.getId(), p);
		seekBarPreferences.put(b.getId(), b);
		return p;
	}
	
	public <T> void registerListPreference(FlcnavPreference<T> b, PreferenceScreen screen, String[] names, T[] values){
		ListPreference p = (ListPreference) screen.findPreference(b.getId());
		prepareListPreference(b, names, values, p);
	}
	
	public <T> ListPreference createListPreference(FlcnavPreference<T> b, String[] names, T[] values, int title, int summary){
		ListPreference p = new ListPreference(this);
		p.setTitle(title);
		p.setKey(b.getId());
		p.setDialogTitle(title);
		p.setSummary(summary);
		prepareListPreference(b, names, values, p);
		return p;
	}

	private <T> void prepareListPreference(FlcnavPreference<T> b, String[] names, T[] values, ListPreference p) {
		p.setOnPreferenceChangeListener(this);
		LinkedHashMap<String, Object> vals = new LinkedHashMap<String, Object>();
		screenPreferences.put(b.getId(), p);
		listPreferences.put(b.getId(), b);
		listPrefValues.put(b.getId(), vals);
		assert names.length == values.length;
		for(int i=0; i<names.length; i++){
			vals.put(names[i], values[i]);
		}
	}
	
	public void registerEditTextPreference(FlcnavPreference<String> b, PreferenceScreen screen){
		EditTextPreference p = (EditTextPreference) screen.findPreference(b.getId());
		p.setOnPreferenceChangeListener(this);
		screenPreferences.put(b.getId(), p);
		editTextPreferences.put(b.getId(), b);
	}
	
	public EditTextPreference createEditTextPreference(FlcnavPreference<String> b, int title, int summary){
		EditTextPreference p = new EditTextPreference(this);
		p.setTitle(title);
		p.setKey(b.getId());
		p.setDialogTitle(title);
		p.setSummary(summary);
		p.setOnPreferenceChangeListener(this);
		screenPreferences.put(b.getId(), p);
		editTextPreferences.put(b.getId(), b);
		return p;
	}
	
	
	public void registerTimeListPreference(FlcnavPreference<Integer> b, PreferenceScreen screen, int[] seconds, int[] minutes, int coeff){
		int minutesLength = minutes == null? 0 : minutes.length;
    	int secondsLength = seconds == null? 0 : seconds.length;
    	Integer[] ints = new Integer[secondsLength + minutesLength];
		String[] intDescriptions = new String[ints.length];
		for (int i = 0; i < secondsLength; i++) {
			ints[i] = seconds[i] * coeff;
			intDescriptions[i] = seconds[i] + " " + getString(R.string.int_seconds); //$NON-NLS-1$
		}
		for (int i = 0; i < minutesLength; i++) {
			ints[secondsLength + i] = (minutes[i] * 60) * coeff;
			intDescriptions[secondsLength + i] = minutes[i] + " " + getString(R.string.int_min); //$NON-NLS-1$
		}
		registerListPreference(b, screen, intDescriptions, ints);
	}
	
	public ListPreference createTimeListPreference(FlcnavPreference<Integer> b, int[] seconds, int[] minutes, int coeff, int title, int summary){
		int minutesLength = minutes == null? 0 : minutes.length;
    	int secondsLength = seconds == null? 0 : seconds.length;
    	Integer[] ints = new Integer[secondsLength + minutesLength];
		String[] intDescriptions = new String[ints.length];
		for (int i = 0; i < secondsLength; i++) {
			ints[i] = seconds[i] * coeff;
			intDescriptions[i] = seconds[i] + " " + getString(R.string.int_seconds); //$NON-NLS-1$
		}
		for (int i = 0; i < minutesLength; i++) {
			ints[secondsLength + i] = (minutes[i] * 60) * coeff;
			intDescriptions[secondsLength + i] = minutes[i] + " " + getString(R.string.int_min); //$NON-NLS-1$
		}
		return createListPreference(b, intDescriptions, ints, title, summary);
	}
	
	private Set<String> getVoiceFiles(){
		// read available voice data
		File extStorage = flcnavSettings.extendFlcnavPath(ResourceManager.VOICE_PATH);
		Set<String> setFiles = new LinkedHashSet<String>();
		if (extStorage.exists()) {
			for (File f : extStorage.listFiles()) {
				if (f.isDirectory()) {
					setFiles.add(f.getName());
				}
			}
		}
		return setFiles;
	}

    @Override
    public void onCreate(Bundle savedInstanceState) {
    	CustomTitleBar titleBar = new CustomTitleBar(this, R.string.settings_activity, R.drawable.tab_settings_screen_icon, R.style.CustomTitleTheme_Preference);
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.settings_pref);
		titleBar.afterSetContentView();
		
		String[] entries;
		String[] entrieValues;
		PreferenceScreen screen = getPreferenceScreen();
		flcnavSettings = getMyApplication().getSettings();

		FCNavPlugin.onSettingsActivityCreate(this, screen);
		
		registerBooleanPreference(flcnavSettings.SHOW_VIEW_ANGLE,screen); 
		registerBooleanPreference(flcnavSettings.USE_ENGLISH_NAMES,screen); 
		registerBooleanPreference(flcnavSettings.AUTO_ZOOM_MAP,screen); 
		registerBooleanPreference(flcnavSettings.FAST_ROUTE_MODE,screen);

//		registerBooleanPreference(flcnavSettings.LEFT_SIDE_NAVIGATION,screen);
		
		
//		registerBooleanPreference(flcnavSettings.SHOW_ALTITUDE_INFO,screen);
		
		CheckBoxPreference nativeCheckbox = registerBooleanPreference(flcnavSettings.NATIVE_RENDERING,screen);
		//disable the checkbox if the library cannot be used
		if ((NativeFlcnavLibrary.isLoaded() && !NativeFlcnavLibrary.isSupported()) || 
				flcnavSettings.NATIVE_RENDERING_FAILED.get()) {
			nativeCheckbox.setEnabled(false);
		}
		
		// List preferences
		registerListPreference(flcnavSettings.ROTATE_MAP, screen, 
				new String[]{getString(R.string.rotate_map_none_opt), getString(R.string.rotate_map_bearing_opt), getString(R.string.rotate_map_compass_opt)},
				new Integer[]{FCNavSettings.ROTATE_MAP_NONE, FCNavSettings.ROTATE_MAP_BEARING, FCNavSettings.ROTATE_MAP_COMPASS});
		
		registerListPreference(flcnavSettings.MAP_SCREEN_ORIENTATION, screen, 
				new String[] {getString(R.string.map_orientation_portrait), getString(R.string.map_orientation_landscape), getString(R.string.map_orientation_default)},
				new Integer[] {ActivityInfo.SCREEN_ORIENTATION_PORTRAIT, ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE, ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED});
		
		
		entries = new String[DayNightMode.values().length];
		for(int i=0; i<entries.length; i++){
			entries[i] = DayNightMode.values()[i].toHumanString(this);
		}
		registerListPreference(flcnavSettings.DAYNIGHT_MODE, screen, entries, DayNightMode.values());
		
		MetricsConstants[] mvls  = new MetricsConstants[] {MetricsConstants.KILOMETERS_AND_METERS, MetricsConstants.MILES_AND_FOOTS}; //MetricsConstants.values();
		entries = new String[mvls.length];
		for(int i=0; i<entries.length; i++){
			entries[i] = mvls[i].toHumanString(this);
		}
		registerListPreference(flcnavSettings.METRIC_SYSTEM, screen, entries, mvls);
		
		//getResources().getAssets().getLocales();
		entrieValues = new String[] { "",
				"en", "fr",	"es" };
		entries = new String[] { getString(R.string.system_locale), 
				"English","French",	"Spanish"};
		registerListPreference(flcnavSettings.PREFERRED_LOCALE, screen, entries, entrieValues);

		
		
		
		Integer[] intValues = new Integer[] { 0, 5, 10, 15, 20, 25, 30, 45, 60, 90};
		entries = new String[intValues.length];
		entries[0] = getString(R.string.auto_follow_route_never);
		for (int i = 1; i < intValues.length; i++) {
			entries[i] = (int) intValues[i] + " " + getString(R.string.int_seconds);
		}
		registerListPreference(flcnavSettings.AUTO_FOLLOW_ROUTE, screen, entries, intValues);
		
		Float[] floatValues = new Float[] {0.6f, 0.8f, 1.0f, 1.2f, 1.5f};
		entries = new String[floatValues.length];
		for (int i = 0; i < floatValues.length; i++) {
			entries[i] = (int) (floatValues[i] * 100) +" %";
		}
		registerListPreference(flcnavSettings.MAP_TEXT_SIZE, screen, entries, floatValues);

		entries = new String[RouteService.values().length];
		for(int i=0; i<entries.length; i++){
			entries[i] = RouteService.values()[i].getName();
		}
		registerListPreference(flcnavSettings.ROUTER_SERVICE, screen, entries, RouteService.values());
		
		
		
		entries = new String[ApplicationMode.values().length];
		for(int i=0; i<entries.length; i++){
			entries[i] = ApplicationMode.values()[i].toHumanString(this);
		}
		registerListPreference(flcnavSettings.APPLICATION_MODE, screen, entries, ApplicationMode.values());
		
		Collection<String> rendererNames = getMyApplication().getRendererRegistry().getRendererNames();
		entries = (String[]) rendererNames.toArray(new String[rendererNames.size()]);
		registerListPreference(flcnavSettings.RENDERER, screen, entries, entries);
		
		createCustomRenderingProperties(false);
		
		applicationModePreference = (ListPreference) screen.findPreference(flcnavSettings.APPLICATION_MODE.getId());
		applicationModePreference.setOnPreferenceChangeListener(this);

		dayNightModePreference = (ListPreference) screen.findPreference(flcnavSettings.DAYNIGHT_MODE.getId());
		dayNightModePreference.setOnPreferenceChangeListener(this);
		routerServicePreference = (ListPreference) screen.findPreference(flcnavSettings.ROUTER_SERVICE.getId());
		routerServicePreference.setOnPreferenceChangeListener(this);

		Preference localIndexes =(Preference) screen.findPreference(FCNavSettings.LOCAL_INDEXES);
		localIndexes.setOnPreferenceClickListener(this);
		applicationDir = (EditTextPreference) screen.findPreference(FCNavSettings.EXTERNAL_STORAGE_DIR);
		applicationDir.setOnPreferenceChangeListener(this);
//		bidforfix = (Preference) screen.findPreference("bidforfix");
//		bidforfix.setOnPreferenceClickListener(this);

//svk Plugin manager can be enabled if necessary
//		plugins = (Preference) screen.findPreference("plugins");
//		plugins.setOnPreferenceClickListener(this);
		
		
		
		
		Intent intent = getIntent();
		if(intent != null && intent.getIntExtra(INTENT_KEY_SETTINGS_SCREEN, 0) != 0){
			int s = intent.getIntExtra(INTENT_KEY_SETTINGS_SCREEN, 0);
			String pref = null;
			if(s == SCREEN_GENERAL_SETTINGS){
				pref = SCREEN_ID_GENERAL_SETTINGS;
			} else if(s == SCREEN_NAVIGATION_SETTINGS){
				pref = SCREEN_ID_NAVIGATION_SETTINGS;
			} else if(s == SCREEN_MONITORING_SETTINGS){
				pref = SCREEN_ID_MONITORING_SETTINGS;
			} 
			if(pref != null){
				Preference toOpen = screen.findPreference(pref);
				if(toOpen instanceof PreferenceScreen){
					setPreferenceScreen((PreferenceScreen) toOpen);
				}
			}
		}
    }

	private void createCustomRenderingProperties(boolean update) {
		RenderingRulesStorage renderer = getMyApplication().getRendererRegistry().getCurrentSelectedRenderer();
		PreferenceCategory cat = (PreferenceCategory) findPreference("custom_vector_rendering");
		if (cat != null) {
			cat.removeAll();
			if (renderer != null) {
				for (RenderingRuleProperty p : renderer.PROPS.getCustomRules()) {
					CommonPreference<String> custom = getMyApplication().getSettings().getCustomRenderProperty(p.getAttrName());
					ListPreference lp = new ListPreference(this);
					lp.setOnPreferenceChangeListener(this);
					lp.setKey(custom.getId());
					lp.setTitle(getStringPropertyName(p.getAttrName(), p.getName()));
					lp.setSummary(getStringPropertyDescription(p.getAttrName(), p.getDescription()));
					cat.addPreference(lp);

					LinkedHashMap<String, Object> vals = new LinkedHashMap<String, Object>();
					screenPreferences.put(custom.getId(), lp);
					listPreferences.put(custom.getId(), custom);
					listPrefValues.put(custom.getId(), vals);
					String[] names = p.getPossibleValues();
					for (int i = 0; i < names.length; i++) {
						vals.put(names[i], names[i]);
					}

				}
				if (update) {
					updateAllSettings();
				}
			}
		}
		
	}

	private void reloadVoiceListPreference(PreferenceScreen screen) {
		String[] entries;
		String[] entrieValues;
		Set<String> voiceFiles = getVoiceFiles();
		entries = new String[voiceFiles.size() + 2];
		entrieValues = new String[voiceFiles.size() + 2];
		int k = 0;
//		entries[k++] = getString(R.string.voice_not_specified);
		entrieValues[k] = FCNavSettings.VOICE_PROVIDER_NOT_USE;
		entries[k++] = getString(R.string.voice_not_use);
		for (String s : voiceFiles) {
			entries[k] = s;
			entrieValues[k] = s;
			k++;
		}
		entrieValues[k] = MORE_VALUE;
		entries[k] = getString(R.string.install_more);
		registerListPreference(flcnavSettings.VOICE_PROVIDER, screen, entries, entrieValues);
	}

	private void updateApplicationDirTextAndSummary() {
		String storageDir = flcnavSettings.getExternalStorageDirectory().getAbsolutePath();
		applicationDir.setText(storageDir);
		applicationDir.setSummary(storageDir);
	}
    
    @Override
    protected void onResume() {
		super.onResume();
		updateAllSettings();
	}
    
    @Override
    protected void onDestroy() {
    	FCNavPlugin.onSettingsActivityDestroy(this);
    	super.onDestroy();
    }
    
    public void updateAllSettings(){
    	for(FlcnavPreference<Boolean> b : booleanPreferences.values()){
    		CheckBoxPreference pref = (CheckBoxPreference) screenPreferences.get(b.getId());
    		pref.setChecked(b.get());
    	}
    	
    	for(FlcnavPreference<Integer> b : seekBarPreferences.values()){
    		SeekBarPreference pref = (SeekBarPreference) screenPreferences.get(b.getId());
    		pref.setValue(b.get());
    	}
    	
    	reloadVoiceListPreference(getPreferenceScreen());
    	
    	for(FlcnavPreference<?> p : listPreferences.values()){
    		ListPreference listPref = (ListPreference) screenPreferences.get(p.getId());
    		Map<String, ?> prefValues = listPrefValues.get(p.getId());
    		String[] entryValues = new String[prefValues.size()];
    		String[] entries = new String[prefValues.size()];
    		int i = 0;
    		for(Entry<String, ?> e : prefValues.entrySet()){
    			entries[i] = e.getKey();
				entryValues[i] = e.getValue() + ""; // case of null
    			i++;
    		}
    		listPref.setEntries(entries);
    		listPref.setEntryValues(entryValues);
			listPref.setValue(p.get() + "");
    	}
    	
    	for(FlcnavPreference<String> s : editTextPreferences.values()){
    		EditTextPreference pref = (EditTextPreference) screenPreferences.get(s.getId());
    		pref.setText(s.get());
    	}
    	
    	FCNavPlugin.onSettingsActivityUpdate(this);
		
		updateApplicationDirTextAndSummary();

		applicationModePreference.setTitle(getString(R.string.settings_preset) + "  [" + flcnavSettings.APPLICATION_MODE.get().toHumanString(this) + "]");
		dayNightModePreference.setSummary(getString(R.string.daynight_descr) + "  [" + flcnavSettings.DAYNIGHT_MODE.get().toHumanString(this) + "]");
		routerServicePreference.setSummary(getString(R.string.router_service_descr) + "  [" + flcnavSettings.ROUTER_SERVICE.get() + "]");
    }

  
    
	@SuppressWarnings("unchecked")
	@Override
	public boolean onPreferenceChange(Preference preference, Object newValue) {
		// handle boolean prefences
		FlcnavPreference<Boolean> boolPref = booleanPreferences.get(preference.getKey());
		FlcnavPreference<Integer> seekPref = seekBarPreferences.get(preference.getKey());
		FlcnavPreference<Object> listPref = (FlcnavPreference<Object>) listPreferences.get(preference.getKey());
		FlcnavPreference<String> editPref = editTextPreferences.get(preference.getKey());
		if(boolPref != null){
			boolPref.set((Boolean)newValue);
			if (boolPref.getId().equals(flcnavSettings.NATIVE_RENDERING.getId())) {
				if(((Boolean)newValue).booleanValue()) {
					loadNativeLibrary();
				}
			}
		} else if (seekPref != null) {
			seekPref.set((Integer) newValue);
		} else if (editPref != null) {
			editPref.set((String) newValue);
		} else if (listPref != null) {
			int ind = ((ListPreference) preference).findIndexOfValue((String) newValue);
			CharSequence entry = ((ListPreference) preference).getEntries()[ind];
			Map<String, ?> map = listPrefValues.get(preference.getKey());
			Object obj = map.get(entry);
			final Object oldValue = listPref.get();
			boolean changed = listPref.set(obj);
			
			// Specific actions after list preference changed
			if (changed) {
				if (listPref.getId().equals(flcnavSettings.VOICE_PROVIDER.getId())) {
					if (MORE_VALUE.equals(newValue)) {
						listPref.set(oldValue); //revert the change..
						final Intent intent = new Intent(this, DownloadIndexActivity.class);
						intent.putExtra(DownloadIndexActivity.FILTER_KEY, "voice");
						startActivity(intent);
					} else {
						getMyApplication().showDialogInitializingCommandPlayer(this, false);
					}
				} else if (listPref.getId().equals(flcnavSettings.ROUTER_SERVICE.getId())) {
					routerServicePreference.setSummary(getString(R.string.router_service_descr) + "  [" + flcnavSettings.ROUTER_SERVICE.get() + "]");
				} else if (listPref.getId().equals(flcnavSettings.APPLICATION_MODE.getId())) {
					updateAllSettings();
				} else if (listPref.getId().equals(flcnavSettings.PREFERRED_LOCALE.getId())) {
					// restart application to update locale
					getMyApplication().checkPrefferedLocale();
					Intent intent = getIntent();
					finish();
					startActivity(intent);
				} else if (listPref.getId().equals(flcnavSettings.DAYNIGHT_MODE.getId())) {
					dayNightModePreference.setSummary(getString(R.string.daynight_descr) + "  [" + flcnavSettings.DAYNIGHT_MODE.get().toHumanString(this)  + "]");
				}
			}
			if (listPref.getId().equals(flcnavSettings.RENDERER.getId())) {
				if(changed){
					AccessibleToast.makeText(this, R.string.renderer_load_sucess, Toast.LENGTH_SHORT).show();
				} else {
					AccessibleToast.makeText(this, R.string.renderer_load_exception, Toast.LENGTH_SHORT).show();
				}
				createCustomRenderingProperties(true);
			}
		} else if(preference == applicationDir){
			warnAboutChangingStorage((String) newValue);
			return false;
		}
		return true;
	}
	
	

	private void warnAboutChangingStorage(final String newValue) {
		final String newDir = newValue != null ? newValue.trim(): newValue;
		File path = new File(newDir);
		path.mkdirs();
		if(!path.canRead() || !path.exists()){
			AccessibleToast.makeText(this, R.string.specified_dir_doesnt_exist, Toast.LENGTH_LONG).show()	;
			return;
		}
		
		Builder builder = new AlertDialog.Builder(this);
		builder.setMessage(getString(R.string.application_dir_change_warning));
		builder.setPositiveButton(R.string.default_buttons_yes, new OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				//edit the preference
				flcnavSettings.setExternalStorageDirectory(newDir);
				getMyApplication().getResourceManager().resetStoreDirectory();
				reloadIndexes();
				updateApplicationDirTextAndSummary();
			}
		});
		builder.setNegativeButton(R.string.default_buttons_cancel, null);
		builder.show();
	}

	public void reloadIndexes(){
		reloadVoiceListPreference(getPreferenceScreen());
		progressDlg = ProgressDialog.show(this, getString(R.string.loading_data), getString(R.string.reading_indexes), true);
		final ProgressDialogImplementation impl = new ProgressDialogImplementation(progressDlg);
		impl.setRunnable("Initializing app", new Runnable(){ //$NON-NLS-1$
			@Override
			public void run() {
				try {
					showWarnings(getMyApplication().getResourceManager().reloadIndexes(impl));
				} finally {
					if(progressDlg !=null){
						progressDlg.dismiss();
						progressDlg = null;
					}
				}
			}
		});
		impl.run();
	}
	
	public void loadNativeLibrary(){
		if (!NativeFlcnavLibrary.isLoaded()) {
			final RenderingRulesStorage storage = getMyApplication().getRendererRegistry().getCurrentSelectedRenderer();
			new AsyncTask<Void, Void, Void>() {
				@Override
				protected void onPreExecute() {
					progressDlg = ProgressDialog.show(SettingsActivity.this, getString(R.string.loading_data),
							getString(R.string.init_native_library), true);
				};

				@Override
				protected Void doInBackground(Void... params) {
					NativeFlcnavLibrary.getLibrary(storage);
					return null;
				}

				@Override
				protected void onPostExecute(Void result) {
					progressDlg.dismiss();
					if (!NativeFlcnavLibrary.isNativeSupported(storage)) {
						AccessibleToast.makeText(SettingsActivity.this, R.string.native_library_not_supported, Toast.LENGTH_LONG).show();
					}
				};
			}.execute();
		}
	}
	
	private FCNavApplication getMyApplication() {
		return (FCNavApplication)getApplication();
	}
	
	@Override
	protected void onStop() {
		if(progressDlg !=null){
			progressDlg.dismiss();
			progressDlg = null;
		}
		super.onStop();
	}
	protected void showWarnings(List<String> warnings) {
		if (!warnings.isEmpty()) {
			final StringBuilder b = new StringBuilder();
			boolean f = true;
			for (String w : warnings) {
				if(f){
					f = false;
				} else {
					b.append('\n');
				}
				b.append(w);
			}
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					AccessibleToast.makeText(SettingsActivity.this, b.toString(), Toast.LENGTH_LONG).show();

				}
			});
		}
	}
		
	
	@Override
	public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
			Preference preference) {
		//customize the sub-preference title according the selected profile
		if (preference.getKey() != null && preference instanceof PreferenceScreen && 
				((PreferenceCategory)findPreference("profile_dep_cat")).findPreference(preference.getKey()) != null) {
			PreferenceScreen scr = (PreferenceScreen)preference;
			String title = scr.getTitle().toString();
			if(title.startsWith("-")){
				title = title.substring(1);
			}
			scr.getDialog().setTitle("   " + title + " [" + flcnavSettings.APPLICATION_MODE.get().toHumanString(this) + "]");
		} else if(preference instanceof PreferenceScreen){
			PreferenceScreen scr = (PreferenceScreen)preference;
			String title = scr.getTitle().toString();
			scr.getDialog().setTitle("   " + title);
		}
		
		if (preference == applicationDir) {
			return true;
		}
		return super.onPreferenceTreeClick(preferenceScreen, preference);
	}

	@Override
	public boolean onPreferenceClick(Preference preference) {
		if(preference.getKey().equals(FCNavSettings.LOCAL_INDEXES)){
			if(getMyApplication().getResourceManager().getIndexFileNames().isEmpty()) {
				startActivity(new Intent(this, FCNavIntents.getDownloadIndexActivity()));
			} else {
				startActivity(new Intent(this, FCNavIntents.getLocalIndexActivity()));
			}
			return true;
		}  else if(preference == plugins){
			startActivity(new Intent(this, PluginsActivity.class));
			return true;
		}
		return false;
	}

	
	public static void installMapLayers(final Activity activity, final ResultMatcher<TileSourceTemplate> result){
		final FCNavSettings settings = ((FCNavApplication) activity.getApplication()).getSettings();
		final Map<String, String> entriesMap = settings.getTileSourceEntries();
		if(!settings.isInternetConnectionAvailable(true)){
			AccessibleToast.makeText(activity, R.string.internet_not_available, Toast.LENGTH_LONG).show();
			return;
		}
		final List<TileSourceTemplate> downloaded = TileSourceManager.downloadTileSourceTemplates(Version.getVersionAsURLParam(activity));
		if(downloaded == null || downloaded.isEmpty()){
			AccessibleToast.makeText(activity, R.string.error_io_error, Toast.LENGTH_SHORT).show();
			return;
		}
		Builder builder = new AlertDialog.Builder(activity);
		String[] names = new String[downloaded.size()];
		for(int i=0; i<names.length; i++){
			names[i] = downloaded.get(i).getName();
		}
		final boolean[] selected = new boolean[downloaded.size()];
		builder.setMultiChoiceItems(names, selected, new DialogInterface.OnMultiChoiceClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which, boolean isChecked) {
				selected[which] = isChecked;
				if(entriesMap.containsKey(downloaded.get(which).getName()) && isChecked){
					AccessibleToast.makeText(activity, R.string.tile_source_already_installed, Toast.LENGTH_SHORT).show();
				}
			}
		});
		builder.setNegativeButton(R.string.default_buttons_cancel, null);
		builder.setTitle(R.string.select_tile_source_to_install);
		builder.setPositiveButton(R.string.default_buttons_apply, new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				List<TileSourceTemplate> toInstall = new ArrayList<TileSourceTemplate>();
				for(int i=0; i<selected.length; i++){
					if(selected[i]){
						toInstall.add(downloaded.get(i));
					}
				}
				for(TileSourceTemplate ts : toInstall){
					if(settings.installTileSource(ts)){
						if(result != null){
							result.publish(ts);
						}
					}
				}
				// at the end publish null to show end of process
				if (!toInstall.isEmpty() && result != null) {
					result.publish(null);
				}
			}
		});
		
		builder.show();
	}
}
