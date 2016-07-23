package net.fcnav.plus;

import net.fcnav.plus.activities.ApplicationMode;
import android.app.backup.BackupAgentHelper;
import android.app.backup.FileBackupHelper;
import android.app.backup.SharedPreferencesBackupHelper;

/**
 * Requires android API from android-8
 */
public class FCNavBackupAgent extends BackupAgentHelper {

	@Override
	public void onCreate() {
		String[] prefs = new String[ApplicationMode.values().length + 1];
		prefs[0] = FCNavSettings.getSharedPreferencesName(null);
		int i = 1;
		for (ApplicationMode m : ApplicationMode.values()) {
			prefs[i++] = FCNavSettings.getSharedPreferencesName(m);
		}

		SharedPreferencesBackupHelper helper = new SharedPreferencesBackupHelper(this, prefs);
		addHelper("flcnav.settings", helper);

		FileBackupHelper fileBackupHelper = new FileBackupHelper(this, FavouritesDbHelper.FAVOURITE_DB_NAME);
		addHelper("flcnav.favorites", fileBackupHelper);
	}
}
