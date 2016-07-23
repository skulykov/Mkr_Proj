
#ifndef TURNTYPE_H
#define TURNTYPE_H

#include <stdint.h>


#include "Common.h"



enum TurnType {
    UKNOWN,
    C,
    TL,
    TSLL, //slightly
    TSHL, //sharply
    TR,
    TSLR, //slightly
    TSHR, //sharply
    KL, // keep left
    KR,
    TU, // U-turn
    TRU, // right u-turn
    RD_RIGHT, // roundabout right hand
    RD_LEFT // roundabout left hand
};

class TurnInfo
{
protected:
    friend class RoutePlannerAnalyzer;
    string value;
    TurnType type;
    int exitOut;
    // calculated clockwise head rotation if previous direction to NORTH
    float turnAngle;
    bool skipToSpeak;
    vector<int> lanes;
public:
    TurnInfo(TurnType type=UKNOWN, int exitOut=0):
        type(type), exitOut(exitOut) {
    }

    TurnType getType() const{return type; }
    int getExitOut() const{return exitOut; }
    float getTurnAngle() const{return turnAngle; }
    void setTurnAngle(float turnAngle) {this->turnAngle = turnAngle; }
    bool isSkipToSpeak() const {return skipToSpeak;}
    void setSkipToSpeak(bool skipToSpeak) {this->skipToSpeak = skipToSpeak; }
    const vector<int>& getLanes() const { return lanes; }
    void setLanes(const vector<int>& ls) { this->lanes = ls; }
    string toString() const;

    static TurnInfo  straight() {
		//http://stackoverflow.com/questions/5188554/my-enum-is-not-a-class-or-namespace
#if SK_BUILD_FOR_ANDROID		
		return TurnInfo (C);
#else
		return TurnInfo (TurnType::C);
#endif
    }

    static TurnInfo getExitTurn(int out, float angle, bool leftSide) {
 #if SK_BUILD_FOR_ANDROID 
		TurnInfo  rTurnType((leftSide?RD_LEFT:RD_RIGHT), out); //$NON-NLS-1$
#else
		TurnInfo  rTurnType((leftSide?TurnType::RD_LEFT:TurnType::RD_RIGHT), out); //$NON-NLS-1$
#endif
        rTurnType.setTurnAngle(angle);
        return rTurnType;
    }



//    static TurnInfo  valueOf(QString s, bool leftSide) {
//        for (String v : predefinedTypes) {
//            if (v.equals(s)) {
//                if (leftSide && TU.equals(v)) {
//                    v = TRU;
//                }
//                return new TurnType(v);
//            }
//        }
//        if (s.startsWith("EXIT")) { //$NON-NLS-1$
//            return getExitTurn(Integer.parseInt(s.substring(4)), 0, leftSide);
//        }
//        return null;
//    }

};

#endif // TURNTYPE_H
