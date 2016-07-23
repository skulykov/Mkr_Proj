package net.fcnav.plus;

import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintStream;
import java.lang.Thread.UncaughtExceptionHandler;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import net.fcnav.FavouritePoint;
import net.fcnav.GPXUtilities;
import net.flcnav.LogUtil;
import net.fcnav.GPXUtilities.GPXFile;
import net.fcnav.GPXUtilities.WptPt;
import net.fcnav.access.AccessibilityMode;
import net.fcnav.access.AccessibleToast;
import net.fcnav.plus.routing.RoutingHelper;
import net.fcnav.plus.voice.CommandPlayer;
import net.fcnav.plus.voice.CommandPlayerException;
import net.fcnav.plus.voice.CommandPlayerFactory;
import net.flcnav.Algoritms;
import net.fcnav.plus.R;
import net.fcnav.plus.R.string;
import net.fcnav.plus.activities.DayNightHelper;
import net.fcnav.plus.activities.LiveMonitoringHelper;
import net.fcnav.plus.activities.SavingTrackHelper;
import net.fcnav.plus.activities.SettingsActivity;
import net.flcnav.plus.render.NativeFlcnavLibrary;
import net.flcnav.plus.render.RendererRegistry;
import net.flcnav.render.RenderingRulesStorage;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Application;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Handler;
import android.text.format.DateFormat;
import android.view.accessibility.AccessibilityManager;
import android.widget.Toast;

//import com.bidforfix.andorid.BidForFixHelper;

public class FCNavApplication extends Application {
	public static final String EXCEPTION_PATH = ResourceManager.APP_DIR + "exception.log"; //$NON-NLS-1$
	private static final org.apache.commons.logging.Log LOG = LogUtil.getLog(FCNavApplication.class);

	ResourceManager manager = null;
	PoiFiltersHelper poiFilters = null;
	RoutingHelper routingHelper = null;
	FavouritesDbHelper favorites = null;
	CommandPlayer player = null;
	
	FCNavSettings flcnavSettings = null;
	
	DayNightHelper daynightHelper;
	NavigationService navigationService;
	RendererRegistry rendererRegistry;
//	BidForFixHelper bidforfix;
	
	// start variables
	private ProgressDialogImplementation startDialog;
	private List<String> startingWarnings;
	private Handler uiHandler;
	private GPXFile gpxFileToDisplay;
	private SavingTrackHelper savingTrackHelper;
	private LiveMonitoringHelper liveMonitoringHelper;

	private boolean applicationInitializing = false;
	private Locale prefferedLocale = null;
	

	@Override
	public void onCreate() {
		super.onCreate();
		
		long timeToStart = System.currentTimeMillis();
		flcnavSettings = createFCNavSettingsInstance();
		routingHelper = new RoutingHelper(flcnavSettings, this, player);
		manager = new ResourceManager(this);
		daynightHelper = new DayNightHelper(this);
		//bidforfix = new BidForFixHelper("flcnav.net", getString(R.string.default_buttons_support), getString(R.string.default_buttons_cancel));
		savingTrackHelper = new SavingTrackHelper(this);
		liveMonitoringHelper = new LiveMonitoringHelper(this);
		uiHandler = new Handler();
		rendererRegistry = new RendererRegistry();
		checkPrefferedLocale();
		startApplication();
		if (LOG.isDebugEnabled()) {
			LOG.debug("Time to start application " + (System.currentTimeMillis() - timeToStart) + " ms. Should be less < 800 ms");
		}
		timeToStart = System.currentTimeMillis();
		FCNavPlugin.initPlugins(this);
		
		if (LOG.isDebugEnabled()) {
			LOG.debug("Time to init plugins " + (System.currentTimeMillis() - timeToStart) + " ms. Should be less < 800 ms");
		}
	}

	@Override
	public void onTerminate() {
		super.onTerminate();
		if (routingHelper != null) {
			routingHelper.getVoiceRouter().onApplicationTerminate(getApplicationContext());
		}
		//if (bidforfix != null) {
    	//	bidforfix.onDestroy();
    	//}
	}

	public RendererRegistry getRendererRegistry() {
		return rendererRegistry;
	}
	
	/**
	 * Creates instance of FCNavSettings
	 * @return Reference to instance of FCNavSettings
	 */
	protected FCNavSettings createFCNavSettingsInstance() {
		return new FCNavSettings(this);
	}

	/**
	 * Application settings
	 * @return Reference to instance of FCNavSettings
	 */
	public FCNavSettings getSettings() {
		if(flcnavSettings == null) {
			LOG.error("Trying to access settings before they were created");
		}
		return flcnavSettings;
	}
	
	public SavingTrackHelper getSavingTrackHelper() {
		return savingTrackHelper;
	}
	
	public LiveMonitoringHelper getLiveMonitoringHelper() {
		return liveMonitoringHelper;
	}
	

	public PoiFiltersHelper getPoiFilters() {
		if (poiFilters == null) {
			poiFilters = new PoiFiltersHelper(this);
		}
		return poiFilters;
	}

	public void setGpxFileToDisplay(GPXFile gpxFileToDisplay, boolean showCurrentGpxFile) {
		this.gpxFileToDisplay = gpxFileToDisplay;
		flcnavSettings.SHOW_CURRENT_GPX_TRACK.set(showCurrentGpxFile);
		if (gpxFileToDisplay == null) {
			getFavorites().setFavoritePointsFromGPXFile(null);
		} else {
			List<FavouritePoint> pts = new ArrayList<FavouritePoint>();
			for (WptPt p : gpxFileToDisplay.points) {
				FavouritePoint pt = new FavouritePoint();
				pt.setLatitude(p.lat);
				pt.setLongitude(p.lon);
				if (p.name == null) {
					p.name = "";
				}
				pt.setName(p.name);
				pts.add(pt);
			}
			gpxFileToDisplay.proccessPoints();
			getFavorites().setFavoritePointsFromGPXFile(pts);
		}
	}

	public GPXFile getGpxFileToDisplay() {
		return gpxFileToDisplay;
	}

	public FavouritesDbHelper getFavorites() {
		if (favorites == null) {
			favorites = new FavouritesDbHelper(this);
		}
		return favorites;
	}

	public ResourceManager getResourceManager() {
		return manager;
	}

	public DayNightHelper getDaynightHelper() {
		return daynightHelper;
	}

	@Override
	public void onLowMemory() {
		super.onLowMemory();
		manager.onLowMemory();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		if (prefferedLocale != null) {
			newConfig.locale = prefferedLocale;
			Locale.setDefault(prefferedLocale);
			getBaseContext().getResources().updateConfiguration(newConfig, getBaseContext().getResources().getDisplayMetrics());
		}
	}

	public void checkPrefferedLocale() {
		Configuration config = getBaseContext().getResources().getConfiguration();
		String lang = flcnavSettings.PREFERRED_LOCALE.get();
		if (!"".equals(lang) && !config.locale.getLanguage().equals(lang)) {
			prefferedLocale = new Locale(lang);
			Locale.setDefault(prefferedLocale);
			config.locale = prefferedLocale;
			getBaseContext().getResources().updateConfiguration(config, getBaseContext().getResources().getDisplayMetrics());
		}

	}

	public static final int PROGRESS_DIALOG = 5;

	/**
	 * @param activity
	 *            that supports onCreateDialog({@link #PROGRESS_DIALOG}) and
	 *            returns @param progressdialog
	 * @param progressDialog
	 *            - it should be exactly the same as onCreateDialog
	 * @return
	 */
	public void checkApplicationIsBeingInitialized(Activity activity, ProgressDialog progressDialog) {
		// start application if it was previously closed
		startApplication();
		synchronized (FCNavApplication.this) {
			if (startDialog != null) {
				progressDialog.setTitle(getString(R.string.loading_data));
				progressDialog.setMessage(getString(R.string.reading_indexes));
				activity.showDialog(PROGRESS_DIALOG);
				startDialog.setDialog(progressDialog);
			} else if (startingWarnings != null) {
				showWarnings(startingWarnings, activity);
			}
		}
	}

	public boolean isApplicationInitializing() {
		return startDialog != null;
	}

	public RoutingHelper getRoutingHelper() {
		return routingHelper;
	}

	public CommandPlayer getPlayer() {
		return player;
	}

	public void showDialogInitializingCommandPlayer(final Activity uiContext) {
		showDialogInitializingCommandPlayer(uiContext, true);
	}

	public void showDialogInitializingCommandPlayer(final Activity uiContext, boolean warningNoneProvider) {
		showDialogInitializingCommandPlayer(uiContext, warningNoneProvider, null);
	}

	public void showDialogInitializingCommandPlayer(final Activity uiContext, boolean warningNoneProvider, Runnable run) {
		String voiceProvider = flcnavSettings.VOICE_PROVIDER.get();
		if (voiceProvider == null || FCNavSettings.VOICE_PROVIDER_NOT_USE.equals(voiceProvider)) {
			if (warningNoneProvider && voiceProvider == null) {
				Builder builder = new AlertDialog.Builder(uiContext);
				builder.setCancelable(true);
				builder.setNegativeButton(R.string.default_buttons_cancel, null);
				builder.setPositiveButton(R.string.default_buttons_ok,
						new DialogInterface.OnClickListener() {

							@Override
							public void onClick(DialogInterface dialog,
									int which) {
								Intent intent = new Intent(uiContext,
										SettingsActivity.class);
								intent.putExtra(
										SettingsActivity.INTENT_KEY_SETTINGS_SCREEN,
										SettingsActivity.SCREEN_NAVIGATION_SETTINGS);
								uiContext.startActivity(intent);
							}
						});
				builder.setTitle(R.string.voice_is_not_available_title);
				builder.setMessage(R.string.voice_is_not_available_msg);
				builder.show();
			}

		} else {
			if (player == null || !Algoritms.objectEquals(voiceProvider, player.getCurrentVoice())) {
				initVoiceDataInDifferentThread(uiContext, voiceProvider, run);
			}
		}

	}

	private void initVoiceDataInDifferentThread(final Activity uiContext, final String voiceProvider, final Runnable run) {
		final ProgressDialog dlg = ProgressDialog.show(uiContext,
				getString(R.string.loading_data),
				getString(R.string.voice_data_initializing));
		new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					if (player != null) {
						player.clear();
					}
					player = CommandPlayerFactory.createCommandPlayer(voiceProvider, FCNavApplication.this, uiContext);
					routingHelper.getVoiceRouter().setPlayer(player);
					dlg.dismiss();
					if (run != null && uiContext != null) {
						uiContext.runOnUiThread(run);
					}
				} catch (CommandPlayerException e) {
					dlg.dismiss();
					showWarning(uiContext, e.getError());
				}
			}
		}).start();
	}

	public NavigationService getNavigationService() {
		return navigationService;
	}

	public void setNavigationService(NavigationService navigationService) {
		this.navigationService = navigationService;
	}
	
	/*
	public BidForFixHelper getBidForFix() {
		return bidforfix;
	}
	*/

	public synchronized void closeApplication() {
		if (applicationInitializing) {
			manager.close();
		}
		applicationInitializing = false;
	}

	public synchronized void startApplication() {
		if (applicationInitializing) {
			return;
		}
		applicationInitializing = true;
		startDialog = new ProgressDialogImplementation(this, null, false);

		startDialog.setRunnable("Initializing app", new Runnable() { //$NON-NLS-1$
					@Override
					public void run() {
						startApplicationBackground();
					}
				});
		startDialog.run();

		Thread.setDefaultUncaughtExceptionHandler(new DefaultExceptionHandler());

	}

	public String exportFavorites(File f) {
		GPXFile gpx = new GPXFile();
		for (FavouritePoint p : getFavorites().getFavouritePoints()) {
			if (p.isStored()) {
				WptPt pt = new WptPt();
				pt.lat = p.getLatitude();
				pt.lon = p.getLongitude();
				pt.name = p.getName() + "_" + p.getCategory();
				gpx.points.add(pt);
			}
		}
		return GPXUtilities.writeGpxFile(f, gpx, this);
	}

	private void startApplicationBackground() {
		List<String> warnings = new ArrayList<String>();
		try {
			if(flcnavSettings.NATIVE_RENDERING_FAILED.get()){
				flcnavSettings.NATIVE_RENDERING.set(false);
				flcnavSettings.NATIVE_RENDERING_FAILED.set(false);
				warnings.add(getString(R.string.native_library_not_supported));
			} else if (flcnavSettings.NATIVE_RENDERING.get()) {
				flcnavSettings.NATIVE_RENDERING_FAILED.set(true);
				startDialog.startTask(getString(R.string.init_native_library), -1);
				RenderingRulesStorage storage = rendererRegistry.getCurrentSelectedRenderer();
				boolean initialized = NativeFlcnavLibrary.getLibrary(storage) != null;
				flcnavSettings.NATIVE_RENDERING_FAILED.set(false);
				if (!initialized) {
					LOG.info("Native library could not loaded!");
					flcnavSettings.NATIVE_RENDERING.set(false);
				}
			}
			warnings.addAll(manager.reloadIndexes(startDialog));
			player = null;
			if (savingTrackHelper.hasDataToSave()) {
				startDialog.startTask(getString(R.string.saving_gpx_tracks), -1);
				warnings.addAll(savingTrackHelper.saveDataToGpx());
			}
			savingTrackHelper.close();

			// restore backuped favorites to normal file
			final File appDir = getSettings().extendFlcnavPath(ResourceManager.APP_DIR);
			File save = new File(appDir, FavouritesDbHelper.FILE_TO_SAVE);
			File bak = new File(appDir, FavouritesDbHelper.FILE_TO_BACKUP);
			if (bak.exists() && (!save.exists() || bak.lastModified() > save.lastModified())) {
				if (save.exists()) {
					save.delete();
				}
				bak.renameTo(save);
			}
		} finally {
			synchronized (FCNavApplication.this) {
				final ProgressDialog toDismiss;
				if (startDialog != null) {
					toDismiss = startDialog.getDialog();
				} else {
					toDismiss = null;
				}
				startDialog = null;

				if (toDismiss != null) {
					uiHandler.post(new Runnable() {
						@Override
						public void run() {
							if (toDismiss != null) {
								//TODO handling this dialog is bad, we need a better standard way
								toDismiss.dismiss();
//								toDismiss.getOwnerActivity().dismissDialog(PROGRESS_DIALOG);
							}
						}
					});
					showWarnings(warnings, toDismiss.getContext());
				} else {
					startingWarnings = warnings;
				}
			}
		}
	}

	protected void showWarnings(List<String> warnings, final Context uiContext) {
		if (warnings != null && !warnings.isEmpty()) {
			final StringBuilder b = new StringBuilder();
			boolean f = true;
			for (String w : warnings) {
				if (f) {
					f = false;
				} else {
					b.append('\n');
				}
				b.append(w);
			}
			showWarning(uiContext, b.toString());
		}
	}

	private void showWarning(final Context uiContext, final String b) {
		uiHandler.post(new Runnable() {
			@Override
			public void run() {
				AccessibleToast.makeText(uiContext, b.toString(), Toast.LENGTH_LONG).show();
			}
		});
	}

	private class DefaultExceptionHandler implements UncaughtExceptionHandler {

		private UncaughtExceptionHandler defaultHandler;

		public DefaultExceptionHandler() {
			defaultHandler = Thread.getDefaultUncaughtExceptionHandler();
		}

		@Override
		public void uncaughtException(final Thread thread, final Throwable ex) {
			File file = flcnavSettings.extendFlcnavPath(EXCEPTION_PATH);
			try {
				ByteArrayOutputStream out = new ByteArrayOutputStream();
				PrintStream printStream = new PrintStream(out);
				ex.printStackTrace(printStream);
				StringBuilder msg = new StringBuilder();
				msg.append(
						"Exception occured in thread " + thread.toString() + " : "). //$NON-NLS-1$ //$NON-NLS-2$
						append(DateFormat.format("MMMM dd, yyyy h:mm:ss", System.currentTimeMillis())).append("\n"). //$NON-NLS-1$//$NON-NLS-2$
						append(new String(out.toByteArray()));

				if (file.getParentFile().canWrite()) {
					BufferedWriter writer = new BufferedWriter(new FileWriter(file, true));
					writer.write(msg.toString());
					writer.close();
				}
				defaultHandler.uncaughtException(thread, ex);
			} catch (Exception e) {
				// swallow all exceptions
				android.util.Log.e(LogUtil.TAG, "Exception while handle other exception", e); //$NON-NLS-1$
			}

		}
	}
	
	public boolean accessibilityExtensions(){
		return flcnavSettings.ACCESSIBILITY_EXTENSIONS.get();
	}

	public boolean accessibilityEnabled() {
		final AccessibilityMode mode = getSettings().ACCESSIBILITY_MODE.get();
		if (mode == AccessibilityMode.ON)
			return true;
		else if (mode == AccessibilityMode.OFF)
			return false;
		return ((AccessibilityManager)getSystemService(ACCESSIBILITY_SERVICE)).isEnabled();
	}

}
