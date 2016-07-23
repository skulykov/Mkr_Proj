
#include "TurnInfo.h"

string TurnInfo::toString() const {
    switch (type) {
    case C:
        return "straight";
    case KL:
        return "keep left";
    case KR:
        return "keep right";
    case TL:
        return "turn left";
    case TR:
        return "turn right";
    case TSLL:
        return "turn slightly left";
    case TSLR:
        return "turn slightly right";
    case TSHL:
        return "turn sharply left";
    case TSHR:
        return "turn sharply right";
    case TU:
    case TRU:
        return "u-turn";
    case RD_LEFT:
    case RD_RIGHT:
        return "roundabout (exit " + stdH::toString(exitOut)  +") ";
    default:
        break;
    }
    return "uknown";
}
