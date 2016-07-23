package net.flcnav.router.test;

import java.text.MessageFormat;

import net.flcnav.router.RoutingContext;

public class FCNavFormatter {

	public static String getFormattedDistance(float meters, RoutingContext ctx) {
		
		
		int mainUnitStr;
		float mainUnitInMeters;
			mainUnitStr = 1;//R.string.km;
			mainUnitInMeters = (float)1000.0;//METERS_IN_KILOMETER;
		
		if (meters >= 100 * mainUnitInMeters) {
			return (int) (meters / mainUnitInMeters) + " km" ; //$NON-NLS-1$
		} else if (meters > 9.99f * mainUnitInMeters) {
			return MessageFormat.format("{0,number,#.#} km" , ((float) meters) / mainUnitInMeters); //$NON-NLS-1$ 
		} else if (meters > 0.999f * mainUnitInMeters) {
			return MessageFormat.format("{0,number,#.##} km", ((float) meters) / mainUnitInMeters); //$NON-NLS-1$
		} else {
			
			return ((int) meters) + " km"; //$NON-NLS-1$
		}
	}

}
