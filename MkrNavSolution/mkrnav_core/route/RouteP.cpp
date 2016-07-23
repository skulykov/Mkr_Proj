#include "RouteP.h"


void RouteP::calculateTimeSpeedInRoute(/* RoutePlannerContext::CalculationContext* context,*/ vector< RouteSegmentResult >& route )
{
	vector<RouteSegmentResult>::iterator it = route.begin();
    for(; it != route.end(); ++it)
    {
        

        float distOnRoadToPass = 0;
        float speed = 60.0;//context->owner->profileContext->getSpeed(segment->road.get());
       /* if (qFuzzyCompare(speed, 0))
            speed = context->owner->profileContext->profile->minDefaultSpeed;*/

        bool isIncrement = it->startPointIndex < it->endPointIndex;
        
        float distanceSum = 0;
        for(int pointIdx = it->startPointIndex; pointIdx != it->endPointIndex; isIncrement ? pointIdx++ : pointIdx--)
        {
            float x1 = it->road->pointsX[pointIdx];
			float y1 = it->road->pointsY[pointIdx];
            float x2 = it->road->pointsX[pointIdx + (isIncrement ? +1 : -1)];
			float y2 = it->road->pointsY[pointIdx + (isIncrement ? +1 : -1)];
            float distance = MapUtils::getDistance(
                MapUtils::get31LongitudeX(x1), MapUtils::get31LatitudeY(y1),
                MapUtils::get31LongitudeX(x2), MapUtils::get31LatitudeY(y2)
            );
            
            distanceSum += distance;
            float obstacle = 0;//context->owner->profileContext->getObstaclesExtraTime(it->road.get(), pointIdx);
            if (obstacle < 0)
                obstacle = 0;
            distOnRoadToPass += distance / speed + obstacle;
        }

        // last point turn time can be added
        // if(i + 1 < result.size()) { distOnRoadToPass += ctx.getRouter().calculateTurnTime(); }
        it->time = distOnRoadToPass;
        it->speed = speed;
        it->distance = distanceSum;
    }
}
bool RouteP::validateAllPointsConnected(vector<RouteSegmentResult>  &route ) 
{
   // assert(route.size() > 1);

    bool res = true;    
    for(int i = 1; i < route.size(); i++)
    {
        RouteSegmentResult prevSegment = route[i-1];
        RouteSegmentResult segment = route[i];

        const float x1 = prevSegment.road->pointsX[prevSegment.endPointIndex];
		const float y1 = prevSegment.road->pointsY[prevSegment.endPointIndex];
        const float x2 = segment.road->pointsX[segment.startPointIndex];
		const float y2 = segment.road->pointsY[segment.startPointIndex];
        float distance = MapUtils::getDistance(
            MapUtils::get31LongitudeX(x1), MapUtils::get31LatitudeY(y1),
            MapUtils::get31LongitudeX(x2), MapUtils::get31LatitudeY(y2)
        );
        if(distance > 0)
        {
            res = false;

           /* LogPrintf(LogSeverityLevel::Error, "!! Points are not connected : %llu(%u) -> %llu(%u) %f meters",
                prevSegment->road->id, prevSegment->endPointIndex, segment->road->id, segment->startPointIndex, distance);*/
        }
    }

    return res;
}


static const int MAX_SPEAK_PRIORITY = 5;
int RouteP::highwaySpeakPriority(string highway) {
    if(highway == "" || stdH::endsWith(highway,string("track")) || stdH::endsWith(highway,string("services")) || stdH::endsWith(highway,string("service"))
            || stdH::endsWith(highway,string("path"))) {
        return MAX_SPEAK_PRIORITY;
    }
    if (stdH::endsWith(highway,string("_link"))  || stdH::endsWith(highway,string("unclassified")) || stdH::endsWith(highway,string("road"))
            || stdH::endsWith(highway,string("living_street")) || stdH::endsWith(highway,string("residential")) )  {
        return 1;
    }
    return 0;
}

bool RouteP::isMotorway(RouteSegmentResult *s){
    string h = s->road->getHighway();
    return "motorway"==h || "motorway_link"==h  ||
            "trunk"==h || "trunk_link"==h;

}


TurnInfo RouteP::attachKeepLeftInfoAndLanes(bool leftSide,RouteSegmentResult  *prevSegm, RouteSegmentResult  *currentSegm, TurnInfo t) {
    // keep left/right
    vector<int> lanes ;
    bool kl = false;
    bool kr = false;

    vector<RouteSegmentResult > attachedRoutes = currentSegm->attachedRoutes[0];
    int ls = prevSegm->road->getLanes();
    if(ls >= 0 && prevSegm->road->getDirection() == 0) {
        ls = (ls + 1) / 2;
    }
    int left = 0;
    int right = 0;
    bool speak = false;
    int speakPriority = max(highwaySpeakPriority(prevSegm->road->getHighway()), highwaySpeakPriority(currentSegm->road->getHighway()));
    if (attachedRoutes.size()>0) {
        for (int i=0;i<attachedRoutes.size();i++) {
			RouteSegmentResult *attached=&attachedRoutes[i];
            double ex = MapUtils::degreesDiff(attached->getBearingBegin(), currentSegm->getBearingBegin());
            double mpi = abs(MapUtils::degreesDiff(prevSegm->getBearingEnd(), attached->getBearingBegin()));
            int rsSpeakPriority = highwaySpeakPriority(attached->road->getHighway());
            if (rsSpeakPriority != MAX_SPEAK_PRIORITY || speakPriority == MAX_SPEAK_PRIORITY) {
                if ((ex < RouteP::MinTurnAngle || mpi < RouteP::MinTurnAngle) && ex >= 0) {
                    kl = true;
                    int lns = attached->road->getLanes();
                    if(attached->road->getDirection() == 0) {
                        lns = (lns + 1) / 2;
                    }
                    if (lns > 0) {
                        right += lns;
                    }
                    speak = speak || rsSpeakPriority <= speakPriority;
                } else if ((ex > -RouteP::MinTurnAngle || mpi < RouteP::MinTurnAngle) && ex <= 0) {
                    kr = true;
                    int lns = attached->road->getLanes();
                    if(attached->road->getDirection() == 0) {
                        lns = (lns + 1) / 2;
                    }
                    if (lns > 0) {
                        left += lns;
                    }
                    speak = speak || rsSpeakPriority <= speakPriority;
                }
            }
        }
    }
    if(kr && left == 0) {
        left = 1;
    } else if(kl && right == 0) {
        right = 1;
    }
    int current = currentSegm->road->getLanes();
    if(currentSegm->road->getDirection() == 0) {
        current = (current + 1) / 2;
    }
    if (current <= 0) {
        current = 1;
    }
//		if(ls >= 0 /*&& current + left + right >= ls*/){
        lanes.resize(current + left + right);
        ls = current + left + right;
        for(int it=0; it< ls; it++) {
            if(it < left || it >= left + current) {
                lanes[it] = 0;
            } else {
                lanes[it] = 1;
            }
        }
        // sometimes links are
        if ((current <= left + right) && (left > 1 || right > 1)) {
            speak = true;
        }
//		}

    double devation = abs(MapUtils::degreesDiff(prevSegm->getBearingEnd(), currentSegm->getBearingBegin()));
    bool makeSlightTurn = devation > 5 && (!isMotorway(prevSegm) || !isMotorway(currentSegm));
    if (kl) {
        t = TurnInfo(devation > 5 ? TSLL : KL);
        t.setSkipToSpeak(!speak);
    }
    if (kr) {
        t = TurnInfo(devation > 5 ? TSLR : KR);
        t.setSkipToSpeak(!speak);
    }
    t.setLanes(lanes);
    return t;
}
 

TurnInfo RouteP::processRoundaboutTurn(vector<RouteSegmentResult >& result, int i, bool leftSide,
                               RouteSegmentResult *prev,RouteSegmentResult  *rr) {
    int exit = 1;
    RouteSegmentResult *last = rr;
    for (int j = i; j < result.size(); j++) {
        RouteSegmentResult *rnext = &result[j];
        last = rnext;
        if (rnext->road->isRoundabout()) {
            bool plus = rnext->startPointIndex < rnext->endPointIndex;
            int k = rnext->startPointIndex;
            if (j == i) {
                // first exit could be immediately after roundabout enter
//					k = plus ? k + 1 : k - 1;
            }
            while (k != rnext->endPointIndex) {
                int attachedRoads = rnext->attachedRoutes[abs(static_cast<int64_t>(k) - rnext->startPointIndex)].size();
                if(attachedRoads > 0) {
                    exit++;
                }
                k = plus ? k + 1 : k - 1;
            }
        } else {
            break;
        }
    }
    // combine all roundabouts
    TurnInfo t = TurnInfo(leftSide? RD_LEFT : RD_RIGHT, exit);
    t.setTurnAngle((float) MapUtils::degreesDiff(last->getBearingBegin(), prev->getBearingEnd())) ;
    return t;
}


TurnInfo RouteP::getTurnInfo(vector<RouteSegmentResult >& result, int i, bool leftSide) {
    if (i == 0) {
        return TurnInfo::straight();
    }
    RouteSegmentResult *prev = &result[i - 1];
    if(prev->road->isRoundabout()) {
        // already analyzed!
        return TurnInfo();
    }
    RouteSegmentResult *rr = &result[i];
    if (rr->road->isRoundabout()) {
        return processRoundaboutTurn(result, i, leftSide, prev, rr);
    }
    TurnInfo t;
    if (prev != 0) {
        bool noAttachedRoads = rr->attachedRoutes[0].size() == 0;
        // add description about turn
        double mpi = MapUtils::degreesDiff(prev->getBearingEnd(), rr->getBearingBegin());
        if(noAttachedRoads){
            // TODO VICTOR : look at the comment inside direction route
//				double begin = rr.getObject().directionRoute(rr.getStartPointIndex(), rr.getStartPointIndex() <
//						rr.getEndPointIndex(), 25);
//				mpi = MapUtils.degreesDiff(prev.getBearingEnd(), begin);
        }
        if (mpi >= RouteP::MinTurnAngle) {
            if (mpi < 60) {
                t = TurnInfo(TSLL);
            } else if (mpi < 120) {
                t = TurnInfo(TL);
            } else if (mpi < 135 || leftSide) {
                t = TurnInfo(TSHL);
            } else {
                t = TurnInfo(TU);
            }
        } else if (mpi < -RouteP::MinTurnAngle) {
            if (mpi > -60) {
                t = TurnInfo(TSLR);
            } else if (mpi > -120) {
                t = TurnInfo(TR);
            } else if (mpi > -135 || !leftSide) {
                t = TurnInfo(TSHR);
            } else {
                t = TurnInfo(TRU);
            }
        } else {
            t = attachKeepLeftInfoAndLanes(leftSide, prev, rr, t);
        }
        if (t.getType() != UKNOWN) {
            t.setTurnAngle((float) -mpi);
        }
    }
    return t;
}


void RouteP::addTurnInfoToRoute( bool leftside, vector< RouteSegmentResult >& result ) {
    int prevSegment = -1;
    float dist = 0;
    int next = 1;
    for (int i = 0; i <= result.size(); i = next) {
        TurnInfo t;
        next = i + 1;
        if (i < result.size()) {
            t = getTurnInfo(result, i, leftside);
            // justify turn
            if(t.getType() != UKNOWN && i < result.size() - 1) {
                bool tl = TL == t.getType();
                bool tr = TR == t.getType();
                if(tl || tr) {
                    TurnInfo tnext = getTurnInfo(result, i + 1, leftside);
                    if(tnext.getType() != UKNOWN && result[i].distance < 35) {
                        if(tl && TL == tnext.getType() ) {
                            next = i + 2;
                            t = TurnInfo(TU);
                        } else if(tr && TR == tnext.getType() ) {
                            next = i + 2;
                            t = TurnInfo(TRU);
                        }
                    }
                }
            }
            result[i].turnInfo/*_turnType*/ = t;
        }
        if (t.getType() != UKNOWN || i == result.size()) {
            if (prevSegment >= 0) {
                string turn = result[prevSegment].turnInfo.toString();
                if (result[prevSegment].turnInfo.getLanes().size() > 0) {
                    turn += "{";
                    //for(int li : result[prevSegment]->turnInfo.getLanes())   //the range-based for loop looks like this: 
					//http://www.cprogramming.com/c++11/c++11-ranged-for-loop.html
					vector<int> v=result[prevSegment].turnInfo.getLanes();
					for(int i=0;i<v.size();i++)
					{
                        int li=v[i];
						string s=stdH::toString(li);
						s+=" ";
						turn += s;
                    }
                    turn += "} ";
                }
                result[prevSegment].description =  turn + " and go " + stdH::toString((int)dist) + " meters";
                if(result[prevSegment].turnInfo.isSkipToSpeak()) {
                    result[prevSegment].description = "-*"+result[prevSegment].description;
                }
            }
            prevSegment = i;
            dist = 0;
        }
        if (i < result.size()) {
            dist += result[i].distance;
        }
    }
}
