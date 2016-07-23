package net.fcnav.plus.activities;

import net.fcnav.plus.R;
import android.content.Context;

public enum ApplicationMode {
	/*
	 * DEFAULT("Default"), CAR("Car"), BICYCLE("Bicycle"), TRUCK("Truck");
	 */

	DEFAULT(R.string.app_mode_default), 
	CAR(R.string.app_mode_car), 
	BICYCLE(R.string.app_mode_bicycle), 
	TRUCK(R.string.app_mode_truck);

	private final int key;

	ApplicationMode(int key) {
		this.key = key;
	}

	public String toHumanString(Context ctx) {
		return ctx.getResources().getString(key);
	}

}