#ifndef _LOG_H_
#define _LOG_H_

#ifdef PLAT_ANDROID
#include <android/log.h>
#include <jni.h>

#define LOG_TAG "liblocalserver"

#if OPEN_LOG
#define logd(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#else
#define logd(arg...) //printf(arg)
#endif

#elif defined DLNA_LINUX
#if OPEN_LOG
#define DLOG(arg...) printf(arg)
#else
#define DLOG(arg...) //printf(arg)
#endif
#endif

#endif
