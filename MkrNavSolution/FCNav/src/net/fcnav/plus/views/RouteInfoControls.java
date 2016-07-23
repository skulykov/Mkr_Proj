package net.fcnav.plus.views;

import java.util.Arrays;

import net.flcnav.Algoritms;
import net.fcnav.FCNavFormatter;
import net.flcnav.osm.LatLon;
import net.fcnav.plus.FCNavApplication;
import net.fcnav.plus.FCNavSettings;
import net.fcnav.plus.FCNavSettings.FlcnavPreference;
import net.fcnav.plus.R;
import net.fcnav.plus.activities.MapActivity;
import net.fcnav.plus.routing.AlarmInfo;
import net.fcnav.plus.routing.RouteCalculationResult.NextDirectionInfo;
import net.fcnav.plus.routing.RouteDirectionInfo;
import net.fcnav.plus.routing.RoutingHelper;
import net.flcnav.router.TurnType;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.Paint.Style;
import android.graphics.Path;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.location.Location;
import android.text.format.DateFormat;
import android.view.View;

public class RouteInfoControls {
	public float scaleCoefficient = 1;
	
	public RouteInfoControls(float scaleCoefficient){
		this.scaleCoefficient = scaleCoefficient;
	}
	
	public NextTurnInfoControl createNextInfoControl(final RoutingHelper routingHelper, Context ctx,
			final FCNavSettings settings, Paint textPaint, Paint subtextPaint, boolean horisontalMini) {
		final NextTurnInfoControl nextTurnInfo = new NextTurnInfoControl(ctx, textPaint, subtextPaint, horisontalMini) {
			NextDirectionInfo calc1 = new NextDirectionInfo();
			TurnType straight = TurnType.valueOf(TurnType.C, true);

			@Override
			public boolean updateInfo() {
				boolean visible = false;
				if (routingHelper != null && routingHelper.isRouteCalculated() && routingHelper.isFollowingMode()) {
					makeUturnWhenPossible = routingHelper.makeUturnWhenPossible() ;
					if (makeUturnWhenPossible) {
						visible = true;
						turnImminent = 0;
						turnType = TurnType.valueOf(TurnType.TU, false);//turnType = TurnType.valueOf(TurnType.TU, settings.LEFT_SIDE_NAVIGATION.get());
						TurnPathHelper.calcTurnPath(pathForTurn, turnType, pathTransform);
						invalidate();
					} else {
						boolean showStraight = false;
						NextDirectionInfo r = routingHelper.getNextRouteDirectionInfo(calc1, true);
						if (r != null && r.distanceTo > 0) {
							visible = true;
							if (r.directionInfo == null) {
								if (turnType != null) {
									turnType = null;
									invalidate();
								}
							} else if (!Algoritms.objectEquals(turnType, showStraight ? straight : r.directionInfo.getTurnType())) {
								turnType = showStraight ? straight : r.directionInfo.getTurnType();
								TurnPathHelper.calcTurnPath(pathForTurn, turnType, pathTransform);
								if (turnType.getExitOut() > 0) {
									exitOut = turnType.getExitOut() + ""; //$NON-NLS-1$
								} else {
									exitOut = null;
								}
								requestLayout();
								invalidate();
							}
							if (distChanged(r.distanceTo, nextTurnDirection)) {
								invalidate();
								requestLayout();
								nextTurnDirection = r.distanceTo;
							}
							if (turnImminent != r.imminent) {
								turnImminent = r.imminent;
								invalidate();
							}
						}
					}
				}
				updateVisibility(visible);
				return true;
			}
		};
		nextTurnInfo.setOnClickListener(new View.OnClickListener() {
//			int i = 0;
//			boolean leftSide = false;
			@Override
			public void onClick(View v) {
				// for test rendering purposes
//				final int l = TurnType.predefinedTypes.length;
//				final int exits = 5;
//				i++;
//				if (i % (l + exits) >= l ) {
//					nextTurnInfo.turnType = TurnType.valueOf("EXIT" + (i % (l + exits) - l + 1), leftSide);
//					float a = leftSide?  -180 + (i % (l + exits) - l + 1) * 50:  180 - (i % (l + exits) - l + 1) * 50;
//					nextTurnInfo.turnType.setTurnAngle(a < 0 ? a + 360 : a);
//					nextTurnInfo.exitOut = (i % (l + exits) - l + 1)+"";
//				} else {
//					nextTurnInfo.turnType = TurnType.valueOf(TurnType.predefinedTypes[i % (TurnType.predefinedTypes.length + exits)], leftSide);
//					nextTurnInfo.exitOut = "";
//				}
//				nextTurnInfo.turnImminent = (nextTurnInfo.turnImminent + 1) % 3;
//				nextTurnInfo.nextTurnDirection = 580;
//				TurnPathHelper.calcTurnPath(nextTurnInfo.pathForTurn, nextTurnInfo.turnType,nextTurnInfo.pathTransform);
//				showMiniMap = true;
				nextTurnInfo.requestLayout();
				nextTurnInfo.invalidate();
			}
		});
		// initial state
		nextTurnInfo.setVisibility(View.GONE);
		return nextTurnInfo;
	}
	
	public NextTurnInfoControl createNextNextInfoControl(final RoutingHelper routingHelper, Context ctx,
			final FCNavSettings settings, Paint textPaint, Paint subtextPaint, boolean horisontalMini) {
		final NextTurnInfoControl nextTurnInfo = new NextTurnInfoControl(ctx, textPaint, subtextPaint, horisontalMini) {
			NextDirectionInfo calc1 = new NextDirectionInfo();
			@Override
			public boolean updateInfo() {
				boolean visible = false;
				if (routingHelper != null && routingHelper.isRouteCalculated() && routingHelper.isFollowingMode()
						) {
					boolean uturnWhenPossible = routingHelper.makeUturnWhenPossible() ;
					NextDirectionInfo r = routingHelper.getNextRouteDirectionInfo(calc1, true);
					if (!uturnWhenPossible) {
						if (r != null) {
							// next turn is very close (show next next with false to speak)
//							if (r.imminent >= 0 && r.imminent < 2) {
//								r = routingHelper.getNextRouteDirectionInfoAfter(r, calc1, false);
//							} else {
								r = routingHelper.getNextRouteDirectionInfo(calc1, true);
								if (r != null) {
									r = routingHelper.getNextRouteDirectionInfoAfter(r, calc1, true);
								}
//							}
						}
					}
					if (r != null && r.distanceTo > 0) {
						visible = true;
						if (r == null || r.directionInfo == null) {
							if (turnType != null) {
								turnType = null;
								invalidate();
							}
						} else if (!Algoritms.objectEquals(turnType, r.directionInfo.getTurnType())) {
							turnType = r.directionInfo.getTurnType();
							TurnPathHelper.calcTurnPath(pathForTurn, turnType, pathTransform);
							invalidate();
							requestLayout();
						}
						if (distChanged(r.distanceTo, nextTurnDirection)) {
							invalidate();
							requestLayout();
							nextTurnDirection = r.distanceTo;
						}
						int imminent = r.imminent;
						if (turnImminent != imminent) {
							turnImminent = imminent;
							invalidate();
						}
					}
				}
				updateVisibility(visible);

				return true;
			}
		};
		nextTurnInfo.setOnClickListener(new View.OnClickListener() {
//			int i = 0;
			@Override
			public void onClick(View v) {
				// uncomment to test turn info rendering
//				final int l = TurnType.predefinedTypes.length;
//				final int exits = 5;
//				i++;
//				if (i % (l + exits) >= l ) {
//					nextTurnInfo.turnType = TurnType.valueOf("EXIT" + (i % (l + exits) - l + 1), true);
//					nextTurnInfo.exitOut = (i % (l + exits) - l + 1)+"";
//					float a = 180 - (i % (l + exits) - l + 1) * 50;
//					nextTurnInfo.turnType.setTurnAngle(a < 0 ? a + 360 : a);
//				} else {
//					nextTurnInfo.turnType = TurnType.valueOf(TurnType.predefinedTypes[i % (TurnType.predefinedTypes.length + exits)], true);
//					nextTurnInfo.exitOut = "";
//				}
//				nextTurnInfo.turnImminent = (nextTurnInfo.turnImminent + 1) % 3;
//				nextTurnInfo.nextTurnDirection = 580;
//				TurnPathHelper.calcTurnPath(nextTurnInfo.pathForTurn, nexsweepAngletTurnInfo.turnType,nextTurnInfo.pathTransform);
//				showMiniMap = true;
			}
		});
		// initial state 
		nextTurnInfo.setVisibility(View.GONE);
		return nextTurnInfo;
	}
	
	
	protected TextInfoControl createTimeControl(final MapActivity map, Paint paintText, Paint paintSubText){
		final RoutingHelper routingHelper = map.getRoutingHelper();
		final Drawable time = map.getResources().getDrawable(R.drawable.info_time);
		final Drawable timeToGo = map.getResources().getDrawable(R.drawable.info_time_to_go);
		final FCNavApplication ctx = map.getMyApplication();
		final FlcnavPreference<Boolean> showArrival = ctx.getSettings().SHOW_ARRIVAL_TIME_OTHERWISE_EXPECTED_TIME;
		final TextInfoControl leftTimeControl = new TextInfoControl(map, 0, paintText, paintSubText) {
			private long cachedLeftTime = 0;
			
			@Override
			public boolean updateInfo() {
				int time = 0;
				if (routingHelper != null && routingHelper.isRouteCalculated()) {
					boolean followingMode = routingHelper.isFollowingMode();
					time = routingHelper.getLeftTime();
					if (time != 0) {
						if (followingMode && showArrival.get()) {
							long toFindTime = time * 1000 + System.currentTimeMillis();
							if (Math.abs(toFindTime - cachedLeftTime) > 30000) {
								cachedLeftTime = toFindTime;
								if (DateFormat.is24HourFormat(ctx)) {
									setText(DateFormat.format("k:mm", toFindTime).toString(), null); //$NON-NLS-1$
								} else {
									setText(DateFormat.format("h:mm", toFindTime).toString(), 
											DateFormat.format("aa", toFindTime).toString()); //$NON-NLS-1$
								}
								return true;
							}
						} else {
							if (Math.abs(time - cachedLeftTime) > 30) {
								cachedLeftTime = time;
								int hours = time / (60 * 60);
								int minutes = (time / 60) % 60;
								setText(String.format("%d:%02d", hours, minutes), null); //$NON-NLS-1$
								return true;
							}
						}
					}
				}
				if (time == 0 && cachedLeftTime != 0) {
					cachedLeftTime = 0;
					setText(null, null);
					return true;
				}
				return false;
			};
		};
		leftTimeControl.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				showArrival.set(!showArrival.get());
				leftTimeControl.setImageDrawable(showArrival.get()? time : timeToGo);
				leftTimeControl.requestLayout();
			}
			
		});
		leftTimeControl.setText(null, null);
		leftTimeControl.setImageDrawable(showArrival.get()? time : timeToGo);
		return leftTimeControl;
	}
	
	protected TextInfoControl createAltitudeControl(final MapActivity map, Paint paintText, Paint paintSubText) {
		final TextInfoControl altitudeControl = new TextInfoControl(map, 0, paintText, paintSubText) {
			private int cachedAlt = 0;

			@Override
			public boolean updateInfo() {
				// draw speed
				if (map.getLastKnownLocation() != null && map.getLastKnownLocation().hasAltitude()) {
					if (cachedAlt != (int) map.getLastKnownLocation().getAltitude()) {
						cachedAlt = (int) map.getLastKnownLocation().getAltitude();
						String ds = FCNavFormatter.getFormattedAlt(cachedAlt, map);
						int ls = ds.lastIndexOf(' ');
						if (ls == -1) {
							setText(ds, null);
						} else {
							setText(ds.substring(0, ls), ds.substring(ls + 1));
						}
						return true;
					}
				} else if (cachedAlt != 0) {
					cachedAlt = 0;
					setText(null, null);
					return true;
				}
				return false;
			}
		};
		altitudeControl.setText(null, null);
		altitudeControl.setImageDrawable(map.getResources().getDrawable(R.drawable.ic_altitude));
		return altitudeControl;
	}
	
	protected TextInfoControl createSpeedControl(final MapActivity map, Paint paintText, Paint paintSubText) {
		final TextInfoControl speedControl = new TextInfoControl(map, 3, paintText, paintSubText) {
			private float cachedSpeed = 0;

			@Override
			public boolean updateInfo() {
				// draw speed
				if (map.getLastKnownLocation() != null && map.getLastKnownLocation().hasSpeed()) {
					// .3 mps == 1.08 kph
					float minDelta = .3f;
					// Update more often at walk/run speeds, since we give higher resolution
					// and use .02 instead of .03 to account for rounding effects.
					if (cachedSpeed < 6) minDelta = .015f;
					if (Math.abs(map.getLastKnownLocation().getSpeed() - cachedSpeed) > minDelta) {
						cachedSpeed = map.getLastKnownLocation().getSpeed();
						String ds = FCNavFormatter.getFormattedSpeed(cachedSpeed, map);
						int ls = ds.lastIndexOf(' ');
						if (ls == -1) {
							setText(ds, null);
						} else {
							setText(ds.substring(0, ls), ds.substring(ls + 1));
						}
						return true;
					}
				} else if (cachedSpeed != 0) {
					cachedSpeed = 0;
					setText(null, null);
					return true;
				}
				return false;
			}
		};
		speedControl.setImageDrawable(map.getResources().getDrawable(R.drawable.info_speed));
		speedControl.setText(null, null);
		return speedControl;
	}
	
	protected TextInfoControl createDistanceControl(final MapActivity map, Paint paintText, Paint paintSubText) {
		final FCNavMapTileView view = map.getMapView();
		TextInfoControl distanceControl = new TextInfoControl(map, 0, paintText, paintSubText) {
			private float[] calculations = new float[1];
			private int cachedMeters = 0;
			
			
			@Override
			public boolean updateInfo() {
				if (map.getPointToNavigate() != null) {
					int d = 0;
					if (map.getRoutingHelper().isRouteCalculated()) {
						d = map.getRoutingHelper().getLeftDistance();
					}
					if (d == 0) {
						Location.distanceBetween(view.getLatitude(), view.getLongitude(), map.getPointToNavigate().getLatitude(), map
								.getPointToNavigate().getLongitude(), calculations);
						d = (int) calculations[0];
					}
					if (distChanged(cachedMeters, d)) {
						cachedMeters = d;
						if (cachedMeters <= 20) {
							cachedMeters = 0;
							setText(null, null);
						} else {
							String ds = FCNavFormatter.getFormattedDistance(cachedMeters, map);
							int ls = ds.lastIndexOf(' ');
							if (ls == -1) {
								setText(ds, null);
							} else {
								setText(ds.substring(0, ls), ds.substring(ls + 1));
							}
						}
						return true;
					}
				} else if (cachedMeters != 0) {
					cachedMeters = 0;
					setText(null, null);
					return true;
				}
				return false;
			}
		};
		distanceControl.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				AnimateDraggingMapThread thread = view.getAnimatedDraggingThread();
				LatLon pointToNavigate = view.getSettings().getPointToNavigate();
				if (pointToNavigate != null) {
					float fZoom = view.getFloatZoom() < 15 ? 15 : view.getFloatZoom();
					thread.startMoving(pointToNavigate.getLatitude(), pointToNavigate.getLongitude(), fZoom, true);
				}
			}
		});
		distanceControl.setText(null, null);
		distanceControl.setImageDrawable(view.getResources().getDrawable(R.drawable.info_target));
		return distanceControl;
	}
	
	protected MiniMapControl createMiniMapControl(final RoutingHelper routingHelper, FCNavMapTileView view) {
		final MiniMapControl miniMapControl = new MiniMapControl(view.getContext(), view) {
			@Override
			public boolean updateInfo() {
				boolean visible = routingHelper.isFollowingMode();
				updateVisibility(visible);
				return super.updateInfo();
			}
		};
		miniMapControl.setVisibility(View.GONE);
		return miniMapControl;
	}
	
	private static final float miniCoeff = 2f;
	protected MapInfoControl createLanesControl(final RoutingHelper routingHelper, final FCNavMapTileView view) {
		final Path laneStraight = new Path();
		Matrix pathTransform = new Matrix();
		pathTransform.postScale(scaleCoefficient / miniCoeff, scaleCoefficient / miniCoeff);
		TurnPathHelper.calcTurnPath(laneStraight, TurnType.valueOf(TurnType.C, false), pathTransform);
		final Paint paintBlack = new Paint();
		paintBlack.setStyle(Style.STROKE);
		paintBlack.setColor(Color.BLACK);
		paintBlack.setAntiAlias(true);
		paintBlack.setStrokeWidth(2.5f);
		
		final Paint paintRouteDirection = new Paint();
		paintRouteDirection.setStyle(Style.FILL);
		paintRouteDirection.setColor(view.getResources().getColor(R.color.nav_arrow));
		paintRouteDirection.setAntiAlias(true);
		final float w = 72 * scaleCoefficient / miniCoeff;
		
		
		final MapInfoControl lanesControl = new MapInfoControl(view.getContext()) {
			int[] lanes = null; 
			
			boolean imminent = false;
			
			@Override
			protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
				int ls = (int) (lanes == null ? 0 : lanes.length * w);
				setWDimensions(ls, (int)( w + 3 * scaleCoefficient));
			}

			@Override
			protected void onDraw(Canvas canvas) {
				super.onDraw(canvas);
				//to change color immediately when needed
				if (lanes != null && lanes.length > 0) {
					canvas.save();
					// canvas.translate((int) (16 * scaleCoefficient), 0);
					for (int i = 0; i < lanes.length; i++) {
						if ((lanes[i] & 1) == 1) {
							paintRouteDirection.setColor(imminent ? getResources().getColor(R.color.nav_arrow_imminent) : getResources().getColor(R.color.nav_arrow));
						} else {
							paintRouteDirection.setColor(getResources().getColor(R.color.nav_arrow_distant));
						}
						canvas.drawPath(laneStraight, paintBlack);
						canvas.drawPath(laneStraight, paintRouteDirection);
						canvas.translate(w, 0);
					}
					canvas.restore();
				}
			}
			
			@Override
			public boolean updateInfo() {
				boolean visible = false;
				int locimminent = -1;
				int[] loclanes = null;
				if (routingHelper != null && routingHelper.isRouteCalculated()) {
					if (routingHelper.isFollowingMode()) {
						NextDirectionInfo r = routingHelper.getNextRouteDirectionInfo(new NextDirectionInfo(), false);
						if(r != null && r.directionInfo != null && r.directionInfo.getTurnType() != null) {
							loclanes  = r.directionInfo.getTurnType().getLanes();
							locimminent = r.imminent;
							// Do not show too far 
							if ((r.distanceTo > 700 && r.directionInfo.getTurnType().isSkipToSpeak()) || r.distanceTo > 1200) {
								loclanes = null;
							}
						}
					} else {
						RouteInfoLayer ls = view.getLayerByClass(RouteInfoLayer.class);
						if (ls != null) {
							int di = ls.getDirectionInfo();
							if (di >= 0 && ls.isVisible()) {
								RouteDirectionInfo next = routingHelper.getRouteDirections().get(di);
								if (next != null) {
									loclanes = next.getTurnType().getLanes();
								}
							}
						}
					}
				}
				visible = loclanes != null && loclanes.length > 0;
				if (visible) {
					if (!Arrays.equals(lanes, loclanes)) {
						lanes = loclanes;
						requestLayout();
						invalidate();
					}
					if ((locimminent == 0) != imminent) {
						imminent = (locimminent == 0);
						invalidate();
					}
				}
				updateVisibility(visible);
				return true;
			}
		};
		
		return lanesControl;
	}

	
	protected MapInfoControl createAlarmInfoControl(final RoutingHelper routingHelper, Context ctx,
			final FCNavSettings settings) {
		final Paint paintCircle = new Paint();
		final float th = 11 * scaleCoefficient;
		paintCircle.setColor(Color.rgb(225, 15, 15));
		paintCircle.setStrokeWidth(11 * scaleCoefficient);
		paintCircle.setStyle(Style.STROKE);
		paintCircle.setAntiAlias(true);
		final Paint content = new Paint();
		content.setColor(Color.WHITE);
		content.setStyle(Style.FILL);
		final Paint ptext = new Paint();
		ptext.setTextSize(27 * scaleCoefficient);
		ptext.setFakeBoldText(true);
		ptext.setAntiAlias(true);
		ptext.setTextAlign(Align.CENTER);
		
		final MapInfoControl alarm = new MapInfoControl(ctx) {
			private String text = "";
			@Override
			public boolean updateInfo() {
				boolean limits = true;//settings.SHOW_SPEED_LIMITS.get();
				boolean cams = true;//settings.SHOW_CAMERAS.get();
				boolean visible = false;
				if ((limits || cams) && routingHelper != null && routingHelper.isFollowingMode()) {
					AlarmInfo alarm = routingHelper.getMostImportantAlarm(settings.METRIC_SYSTEM.get());
					if(alarm != null) {
						if(alarm.getType() == AlarmInfo.SPEED_LIMIT) {
							text = alarm.getIntValue() +"";
						} else if(alarm.getType() == AlarmInfo.SPEED_CAMERA) {
							text = "CAM";
						} else if(alarm.getType() == AlarmInfo.BORDER_CONTROL) {
							text = "CLO";
						} else if(alarm.getType() == AlarmInfo.TOLL_BOOTH) {
							text = "$";
						} else if(alarm.getType() == AlarmInfo.TRAFFIC_CALMING) {
							// temporary omega
							text = "~^~";
						} else if(alarm.getType() == AlarmInfo.STOP) {
							// text = "STOP";
						}
						visible = text.length() > 0;
						if (visible) {
							if (alarm.getType() == AlarmInfo.SPEED_CAMERA) {
								visible = cams;
							} else {
								visible = limits;
							}
						}
					}
				}
				updateVisibility(visible);
				return true;
			}

			@Override
			protected void onDraw(Canvas canvas) {
				RectF f = new RectF(th / 2, th / 2, getWidth() - th / 2, getHeight() - th / 2);
				canvas.drawOval(f, content);
				canvas.drawOval(f, paintCircle);
				canvas.drawText(text, getWidth() / 2, getHeight() / 2 + ptext.descent() + 3 * scaleCoefficient, ptext);
			}

		};
		// initial state
		// nextTurnInfo.setVisibility(View.GONE);
		return alarm;
	}
	
	
	public boolean distChanged(int oldDist, int dist){
		if(oldDist != 0 && oldDist - dist < 100 && Math.abs(((float) dist - oldDist)/oldDist) < 0.01){
			return false;
		}
		return true;
	}
}
