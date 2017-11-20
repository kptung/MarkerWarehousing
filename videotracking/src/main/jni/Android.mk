LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#opencv
OPENCVROOT:= $(OPENCV_SDK_HOME)
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES := IrArInterface.cpp \
                   ippe.cpp \
                   dictionary.cpp \
                   aruco.cpp \
                   jni_IrDetect.cpp
LOCAL_LDLIBS += -llog -latomic
LOCAL_MODULE := videotracking

include $(BUILD_SHARED_LIBRARY)