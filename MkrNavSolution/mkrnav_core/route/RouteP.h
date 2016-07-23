#ifndef _MKR_BINARY_ROUTE_PLANNER_H
#define _MKR_BINARY_ROUTE_PLANNER_H

#include "common.h"
#include "Route.h"


class RouteP
{
	static bool isMotorway(RouteSegmentResult *s);
	static int highwaySpeakPriority(string highway);
public:
		 enum {
            MinTurnAngle = 45
        };
		static TurnInfo getTurnInfo(vector<RouteSegmentResult >& result, int i, bool leftSide);
		static void addTurnInfoToRoute( bool leftside,  vector<RouteSegmentResult >& result ) ;
		static TurnInfo processRoundaboutTurn(vector<RouteSegmentResult >& result, int i, bool leftSide,
                               RouteSegmentResult *prev,RouteSegmentResult  *rr);
		static TurnInfo attachKeepLeftInfoAndLanes(bool leftSide,RouteSegmentResult  *prevSegm, RouteSegmentResult  *currentSegm, TurnInfo t);


		static bool validateAllPointsConnected(vector<RouteSegmentResult> &route ) ;

		static void calculateTimeSpeedInRoute(/* RoutePlannerContext::CalculationContext* context,*/ vector< RouteSegmentResult>& route );
};

#endif //_MKR_BINARY_ROUTE_PLANNER_H