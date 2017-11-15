// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

// Check Windows
#ifdef _WIN32 || _WIN64
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "targetver.h"
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#endif
// Check Android
#ifdef __ANDROID__
#include <android/log.h>
#define LOG_TAG "MarkerLib"
#include <jni.h>
#endif

// TODO: reference additional headers your program requires here
