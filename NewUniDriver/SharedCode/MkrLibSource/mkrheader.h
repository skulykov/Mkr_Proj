#ifndef MKR_HEADER_H_INCLUDED
#define MKR_HEADER_H_INCLUDED

#if defined(ANDROID)
#include <jni.h>
#include <unistd.h>
#include <sys/resource.h>
#include <android/log.h>
#endif
#include <errno.h>

#include <string.h>

#include "JsonCpp/include/json/json.h"
#include <algorithm> // sort
#include <sstream>
#include <stdio.h>

#include <iostream>


#endif