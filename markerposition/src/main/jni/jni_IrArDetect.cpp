#include <jni.h>

#include <stdlib.h>     /* NULL */
#include <assert.h>     /* assert */
#include <math.h>
#include <vector>
//#include <string>
//#include <time.h>
#include "timer.h"
#include "IrArInterface.h"
#include <android/log.h>
#include <iostream>
#include <ctime>
#include <fstream>

//#define LOG_NDEBUG 0
//#define LOG_TAG "JNI_NativeTracking"
//#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

//#define JNI_DBG 0
//#define VIDEO_TRACKING_LIB_VERSION 0.03
//#define MIN_RECT_VALUE 14

#ifndef JPG
#define JPG (std::string(".jpg"))
#endif

#define IRAR_LIB_VERSION 1.93
//#define JNI_DBG 1
#define LOG_TAG "IrArMarkerLib"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;

static bool debug_mode = false;

#ifndef org_iii_snsi_markerposition_IrArDetect
#define org_iii_snsi_markerposition_IrArDetect
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_iii_snsi_markerposition_IrArDetect_importYMLCameraParameters
        (JNIEnv *env, jclass jIrArDetect, jstring jfilename) {
    // Reconstruct Java structure to C++ structure
    string filename;
    GetJStringContent(env, jfilename, filename);

    // Call IrCalibration method
    if(JNI_DBG)
        LOGD("importYMLCameraParameters is %s", filename.c_str());
    return (jboolean)importYMLCameraParameters(filename);
}

JNIEXPORT jboolean JNICALL
Java_org_iii_snsi_markerposition_IrArDetect_importYMLDetectParameters(
        JNIEnv *env, jclass type, jstring jfilename) {
    // Reconstruct Java structure to C++ structure
    string filename;
    GetJStringContent(env, jfilename, filename);

    // Call IrCalibration method
    if(JNI_DBG)
        LOGD("importYMLDetectParameters is %s", filename.c_str());
    return (jboolean)importYMLDetectParameters(filename);
}

JNIEXPORT jboolean JNICALL
Java_org_iii_snsi_markerposition_IrArDetect_importYMLDict(
        JNIEnv *env, jclass type, jstring jfilename) {
    // Reconstruct Java structure to C++ structure
    string filename;
    GetJStringContent(env, jfilename, filename);

    // Call IrCalibration method
    if(JNI_DBG)
        LOGD("importYMLDict is %s", filename.c_str());
    return (jboolean)importYMLDict(filename);
}

JNIEXPORT jobjectArray JNICALL
Java_org_iii_snsi_markerposition_IrArDetect_findAppMarkers(JNIEnv *env, jclass type, jbyteArray bytes_, jint width, jint height, jfloat markerSize, jdoubleArray infos)
{
    jbyte* frame = env->GetByteArrayElements(bytes_, 0);
	Mat image;
	Mat myuv(height + height/2, width, CV_8UC1, (uchar *)frame);
	//cv::cvtColor(myuv, image, CV_YUV420sp2BGR);
    cv::cvtColor(myuv, image, CV_YUV2BGR_NV21);
    cv::imwrite("/sdcard/dbg/jni_input.jpg",image);

    if (image.data == NULL)
    {
    	if (JNI_DBG) {
    		LOGD("image convert fail");
    	}
    	return NULL;
    }

    // Get a class reference
    jclass classIrArucoMarker = env->FindClass("org/iii/snsi/markerposition/IrArucoMarker");
    assert(classIrArucoMarker != NULL);

    jclass classCvPoint = env->FindClass("org/iii/snsi/markerposition/Point2D");
    assert(classCvPoint != NULL);

    jclass classCvPoint3 = env->FindClass("org/iii/snsi/markerposition/Point3D");
    assert(classCvPoint3 != NULL);

    if(JNI_DBG)
        LOGD("JNI_findMarkers_App_Start...");


    jint length = env->GetArrayLength(infos);
    jdouble *infoData = env->GetDoubleArrayElements(infos, NULL);
    std::vector<double>input;
    for(int i=0;i<length;i++)
    {
        const double& var = infoData[i];
        input.push_back(var);
    }

    double scale = (markerSize - (int)markerSize == 0) ? 100.0 : 1.0;
    std::vector<cv::Point3f> ppos;
    std::vector<int> idd;
    for (int i = 0; i < length; i += 4)
    {
    	idd.push_back(input.at(i));
    	double x = input.at(i + 1) / scale;
        double y = input.at(i + 2) / scale;
        double z = input.at(i + 3) / scale;
        ppos.push_back(cv::Point3f(x, y, z));
    }

    std::vector<IrArucoMarker> markers = std::vector<IrArucoMarker>();
    auto tstart = std::chrono::high_resolution_clock::now();
    bool flag = findArucoMarkers(image, markerSize, markers);
    //bool flag = findArucoMarkers(image, markers);
    auto tend = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
    if(JNI_DBG)
            LOGD("JNI_findMarkers_App_End...");

    // Allocate a jobjectArray
    int arrayLength = (int)markers.size();
    jobjectArray outJNIArray = env->NewObjectArray( arrayLength, classIrArucoMarker, NULL );
    if(flag)
    {
        // Get the Field ID of the instance variables
        jfieldID fidmId = env->GetFieldID( classIrArucoMarker, "mid", "I" );
        assert(fidmId != NULL);

        jfieldID fidmOri = env->GetFieldID( classIrArucoMarker, "mori", "I" );
        assert(fidmOri != NULL);

        jfieldID fidmXZangle = env->GetFieldID( classIrArucoMarker, "mxzangle", "I" );
        assert(fidmXZangle != NULL);

        jfieldID fidmYZangle = env->GetFieldID( classIrArucoMarker, "myzangle", "I" );
        assert(fidmYZangle != NULL);

        jfieldID fidmxzDistance = env->GetFieldID( classIrArucoMarker, "mxzdistance", "D");
        assert(fidmxzDistance != NULL);

        jfieldID fidmDistance = env->GetFieldID( classIrArucoMarker, "mdistance", "D");
        assert(fidmDistance != NULL);

        jfieldID fidmCorners = env->GetFieldID( classIrArucoMarker, "mcorners", "[Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmCorners != NULL);

        jfieldID fidmRejecteds = env->GetFieldID( classIrArucoMarker, "mrejecteds", "[Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmRejecteds != NULL);

        jfieldID fidcamPosition = env->GetFieldID( classIrArucoMarker, "camposition", "Lorg/iii/snsi/markerposition/Point3D;" );
        assert(fidcamPosition != NULL);

        jfieldID fidmCenter = env->GetFieldID( classIrArucoMarker, "mcenter", "Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmCenter != NULL);

        jfieldID fidInjectpoints = env->GetFieldID( classIrArucoMarker, "injectpoints", "[Lorg/iii/snsi/markerposition/Point2D;");
        assert(fidInjectpoints != NULL);

        // Get the Method ID of the constructor
        jmethodID pointInit = env->GetMethodID( classCvPoint, "<init>", "(DD)V");
        assert(pointInit != NULL);

        jmethodID point3Init = env->GetMethodID( classCvPoint3, "<init>", "(DDD)V");
        assert(point3Init != NULL);

        // marker info
        for (int i = 0; i < arrayLength; i++)
        {
            int mId = markers[i].getMarkerId();
            double mXZdist = markers[i].getXZCameraDistance(); // the distance unit is cm
            double mDist = markers[i].getCameraDistance(); // the distance unit is cm
            int mOri = markers[i].getMarkerOri();

            cv::Point2f mCenter = markers[i].getMarkerCenter();
            const std::vector<cv::Point2f> &mCorners = markers[i].getCorners();
            int corner_len = (int) mCorners.size();
            const std::vector<cv::Point2f> &mRejecteds = markers[i].getRejecteds();
            int rejected_len = (int) mRejecteds.size();
            const cv::Point3f &m_cameraPosition = markers[i].getCameraPosition();
            cv::Mat rvec = markers[i].getRotationMatrix();
            cv::Mat tvec = markers[i].getTransnslationMatrix();
            int mXZangle = 0, mYZangle = 0;
            markers[i].getCameraAngle(mXZangle, mYZangle);

            std::vector<cv::Point3f> Injectionpts;
            for (int i = 0; i < idd.size(); i++)
            	if (idd.at(i) == mId)
                	Injectionpts.push_back(ppos.at(i));

            std::vector<cv::Point2f> mInjectPoints = findInjection(Injectionpts, rvec, tvec, mOri, markers[i].getMarkerCenter());
            int injectPoints_len = (int) mInjectPoints.size();

            // Create an object instance
            jobject objIrArucoMarker = env->AllocObject( classIrArucoMarker );

            // Change the variable
            env->SetIntField( objIrArucoMarker, fidmId, mId );
            env->SetIntField( objIrArucoMarker, fidmOri, mOri );
            env->SetIntField( objIrArucoMarker, fidmXZangle, mXZangle );
            env->SetIntField( objIrArucoMarker, fidmYZangle, mYZangle );
            env->SetDoubleField( objIrArucoMarker, fidmxzDistance, mXZdist );
            env->SetDoubleField( objIrArucoMarker, fidmDistance, mDist );

            jobjectArray pointCornersArray = env->NewObjectArray( corner_len, classCvPoint, NULL );
            for (int j = 0; j < corner_len; j++)
            {
                jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mCorners[j].x, (double)mCorners[j].y );
                env->SetObjectArrayElement( pointCornersArray, j, objCvPoint );
                env->DeleteLocalRef(objCvPoint);
            }
            env->SetObjectField( objIrArucoMarker, fidmCorners, pointCornersArray );
            env->DeleteLocalRef(pointCornersArray);

            jobjectArray pointRejectedsArray = env->NewObjectArray( rejected_len, classCvPoint, NULL );
            for (int j = 0; j < rejected_len; j++)
            {
                jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mRejecteds[j].x, (double)mRejecteds[j].y );
                env->SetObjectArrayElement( pointRejectedsArray, j, objCvPoint );
                env->DeleteLocalRef(objCvPoint);
            }
            env->SetObjectField( objIrArucoMarker, fidmRejecteds, pointRejectedsArray);
            env->DeleteLocalRef(pointRejectedsArray);

            jobject objCvPoint3 = env->NewObject( classCvPoint3, point3Init, (double)m_cameraPosition.x, (double)m_cameraPosition.y, (double)m_cameraPosition.z);
            env->SetObjectField( objIrArucoMarker, fidcamPosition, objCvPoint3 );
            env->DeleteLocalRef(objCvPoint3);

            jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mCenter.x, (double)mCenter.y);
            env->SetObjectField( objIrArucoMarker, fidmCenter, objCvPoint);
            env->DeleteLocalRef(objCvPoint);

            jobjectArray injectPointArray = env->NewObjectArray( injectPoints_len, classCvPoint, NULL );
            for (int j = 0; j < injectPoints_len; j++)
            {
                jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mInjectPoints[j].x, (double)mInjectPoints[j].y );
                env->SetObjectArrayElement( injectPointArray, j, objCvPoint );
                env->DeleteLocalRef(objCvPoint);
            }
            env->SetObjectField( objIrArucoMarker, fidInjectpoints, injectPointArray );
            env->DeleteLocalRef(injectPointArray);

            // Set to the jobjectArray
            env->SetObjectArrayElement( outJNIArray, i, objIrArucoMarker );
            env->DeleteLocalRef(objIrArucoMarker);
        }
        if(JNI_DBG)
            LOGD("JNI_findMarkers_App_Value2Java...");
    }
    else
    {
        if(JNI_DBG)
            LOGD("JNI_findMarkers_App_NoValue2Java...");
    }
    // Release pointer
    env->DeleteLocalRef(classIrArucoMarker);
    env->DeleteLocalRef(classCvPoint);
    env->DeleteLocalRef(classCvPoint3);

    env->ReleaseByteArrayElements(bytes_, frame, 0);

    return outJNIArray;
    //return 0;
}
JNIEXPORT jobjectArray JNICALL
Java_org_iii_snsi_markerposition_IrArDetect_findBasicMarkers(JNIEnv *env, jclass type, jbyteArray bytes_, jint width, jint height)
{
    jbyte* frame = env->GetByteArrayElements(bytes_, 0);
	Mat image;
	Mat myuv(height + height/2, width, CV_8UC1, (uchar *)frame);
	//cv::cvtColor(myuv, image, CV_YUV420sp2BGR);
    cv::cvtColor(myuv, image, CV_YUV2BGR_NV21);
    cv::imwrite("/sdcard/dbg/jni_input.jpg",image);

    if (image.data == NULL)
    {
    	if (JNI_DBG) {
    		LOGD("image convert fail");
    	}
    	return NULL;
    }

    // Get a class reference
    jclass classIrArucoMarker = env->FindClass("org/iii/snsi/markerposition/IrArucoMarker");
    assert(classIrArucoMarker != NULL);

    jclass classCvPoint = env->FindClass("org/iii/snsi/markerposition/Point2D");
    assert(classCvPoint != NULL);

    jclass classCvPoint3 = env->FindClass("org/iii/snsi/markerposition/Point3D");
    assert(classCvPoint3 != NULL);

    if(JNI_DBG)
        LOGD("JNI_findBasicMarkers_Start...");
    vector<IrArucoMarker> markers = vector<IrArucoMarker>();
    auto tstart = std::chrono::high_resolution_clock::now();
    bool flag = findArucoMarkers(image, markers);
    auto tend = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
    if(JNI_DBG)
       LOGD("JNI_findBasicMarkers_End...");


    // Allocate a jobjectArray
    int arrayLength = (int)markers.size();
    jobjectArray outJNIArray = env->NewObjectArray( arrayLength, classIrArucoMarker, NULL );
    if(flag)
    {
        // Get the Field ID of the instance variables
        jfieldID fidmId = env->GetFieldID( classIrArucoMarker, "mid", "I" );
        assert(fidmId != NULL);

        jfieldID fidmOri = env->GetFieldID( classIrArucoMarker, "mori", "I" );
        assert(fidmOri != NULL);

        jfieldID fidmCenter = env->GetFieldID( classIrArucoMarker, "mcenter", "Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmCenter != NULL);

        // Get the Method ID of the constructor
        jmethodID pointInit = env->GetMethodID( classCvPoint, "<init>", "(DD)V");
        assert(pointInit != NULL);

        jmethodID point3Init = env->GetMethodID( classCvPoint3, "<init>", "(DDD)V");
        assert(point3Init != NULL);

         if(JNI_DBG)
            LOGD("JNI_findMarkers_Value2Java_Basic_START");
        // marker info
        for (int i = 0; i < arrayLength; i++)
        {
            int mId = markers[i].getMarkerId();
            int mOri = markers[i].getMarkerOri();
            cv::Point2f mCenter = markers[i].getMarkerCenter();

            // Create an object instance
            jobject objIrArucoMarker = env->AllocObject( classIrArucoMarker );

            // id, ori
            env->SetIntField( objIrArucoMarker, fidmId, mId );
            env->SetIntField( objIrArucoMarker, fidmOri, mOri );
            // center
            jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mCenter.x, (double)mCenter.y);
            env->SetObjectField( objIrArucoMarker, fidmCenter, objCvPoint);
            env->DeleteLocalRef(objCvPoint);

            // Set to the jobjectArray
            env->SetObjectArrayElement( outJNIArray, i, objIrArucoMarker );
            env->DeleteLocalRef(objIrArucoMarker);
        }
    }
     if(JNI_DBG)
        LOGD("JNI_findMarkers_Value2Java_Basic_END");
    // Release pointer
    env->DeleteLocalRef(classIrArucoMarker);
    env->DeleteLocalRef(classCvPoint);
    env->DeleteLocalRef(classCvPoint3);

    env->ReleaseByteArrayElements(bytes_, frame, 0);

    return outJNIArray;
    //return 0;
}

JNIEXPORT jobjectArray JNICALL
Java_org_iii_snsi_markerposition_IrArDetect_findAdvMarkers(JNIEnv *env, jclass type, jbyteArray bytes_, jint width, jint height, jfloat markerSize)
{
    jbyte* frame = env->GetByteArrayElements(bytes_, 0);
	Mat image;
	Mat myuv(height + height/2, width, CV_8UC1, (uchar *)frame);
	//cv::cvtColor(myuv, image, CV_YUV420sp2BGR);
    cv::cvtColor(myuv, image, CV_YUV2BGR_NV21);
    cv::imwrite("/sdcard/dbg/jni_input.jpg",image);

    if (image.data == NULL)
    {
    	if (JNI_DBG) {
    		LOGD("image convert fail");
    	}
    	return NULL;
    }

    // Get a class reference
    jclass classIrArucoMarker = env->FindClass("org/iii/snsi/markerposition/IrArucoMarker");
    assert(classIrArucoMarker != NULL);

    jclass classCvPoint = env->FindClass("org/iii/snsi/markerposition/Point2D");
    assert(classCvPoint != NULL);

    jclass classCvPoint3 = env->FindClass("org/iii/snsi/markerposition/Point3D");
    assert(classCvPoint3 != NULL);

    if(JNI_DBG)
        LOGD("JNI_findMarkers_Adv_Start...");
    vector<IrArucoMarker> markers = vector<IrArucoMarker>();
    auto tstart = std::chrono::high_resolution_clock::now();
    bool flag = findArucoMarkers(image, markerSize, markers);
    //bool flag = findArucoMarkers(image, markers);
    auto tend = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
    if(JNI_DBG)
            LOGD("JNI_findMarkers_Adv_End...");

    // Allocate a jobjectArray
    int arrayLength = (int)markers.size();
    jobjectArray outJNIArray = env->NewObjectArray( arrayLength, classIrArucoMarker, NULL );
    if(flag)
    {
        // Get the Field ID of the instance variables
        jfieldID fidmId = env->GetFieldID( classIrArucoMarker, "mid", "I" );
        assert(fidmId != NULL);

        jfieldID fidmOri = env->GetFieldID( classIrArucoMarker, "mori", "I" );
        assert(fidmOri != NULL);

        jfieldID fidmXZangle = env->GetFieldID( classIrArucoMarker, "mxzangle", "I" );
        assert(fidmXZangle != NULL);

        jfieldID fidmYZangle = env->GetFieldID( classIrArucoMarker, "myzangle", "I" );
        assert(fidmYZangle != NULL);

        jfieldID fidmxzDistance = env->GetFieldID( classIrArucoMarker, "mxzdistance", "D");
        assert(fidmxzDistance != NULL);

        jfieldID fidmDistance = env->GetFieldID( classIrArucoMarker, "mdistance", "D");
        assert(fidmDistance != NULL);

        jfieldID fidmCorners = env->GetFieldID( classIrArucoMarker, "mcorners", "[Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmCorners != NULL);

        jfieldID fidmRejecteds = env->GetFieldID( classIrArucoMarker, "mrejecteds", "[Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmRejecteds != NULL);

        jfieldID fidcamPosition = env->GetFieldID( classIrArucoMarker, "camposition", "Lorg/iii/snsi/markerposition/Point3D;" );
        assert(fidcamPosition != NULL);

        jfieldID fidmCenter = env->GetFieldID( classIrArucoMarker, "mcenter", "Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmCenter != NULL);

        jfieldID fidInjectpoints = env->GetFieldID( classIrArucoMarker, "injectpoints", "[Lorg/iii/snsi/markerposition/Point2D;");
        assert(fidInjectpoints != NULL);

        // Get the Method ID of the constructor
        jmethodID pointInit = env->GetMethodID( classCvPoint, "<init>", "(DD)V");
        assert(pointInit != NULL);

        jmethodID point3Init = env->GetMethodID( classCvPoint3, "<init>", "(DDD)V");
        assert(point3Init != NULL);

        // marker info
        for (int i = 0; i < arrayLength; i++)
        {
            int mId = markers[i].getMarkerId();
            double mXZdist = markers[i].getXZCameraDistance(); // the distance unit is cm
            double mDist = markers[i].getCameraDistance(); // the distance unit is cm
            int mOri = markers[i].getMarkerOri();
            cv::Point2f mCenter = markers[i].getMarkerCenter();
            const std::vector<cv::Point2f> &mCorners = markers[i].getCorners();
            int corner_len = (int) mCorners.size();
            const std::vector<cv::Point2f> &mRejecteds = markers[i].getRejecteds();
            int rejected_len = (int) mRejecteds.size();
            const cv::Point3f &m_cameraPosition = markers[i].getCameraPosition();
            cv::Mat rvec = markers[i].getRotationMatrix();
            cv::Mat tvec = markers[i].getTransnslationMatrix();
            int mXZangle = 0, mYZangle = 0;
            markers[i].getCameraAngle(mXZangle, mYZangle);


            // Create an object instance
            jobject objIrArucoMarker = env->AllocObject( classIrArucoMarker );

            // Change the variable
            env->SetIntField( objIrArucoMarker, fidmId, mId );
            env->SetIntField( objIrArucoMarker, fidmOri, mOri );
            env->SetIntField( objIrArucoMarker, fidmXZangle, mXZangle );
            env->SetIntField( objIrArucoMarker, fidmYZangle, mYZangle );
            env->SetDoubleField( objIrArucoMarker, fidmxzDistance, mXZdist );
            env->SetDoubleField( objIrArucoMarker, fidmDistance, mXZdist );

            jobjectArray pointCornersArray = env->NewObjectArray( corner_len, classCvPoint, NULL );
            for (int j = 0; j < corner_len; j++)
            {
                jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mCorners[j].x, (double)mCorners[j].y );
                env->SetObjectArrayElement( pointCornersArray, j, objCvPoint );
                env->DeleteLocalRef(objCvPoint);
            }
            env->SetObjectField( objIrArucoMarker, fidmCorners, pointCornersArray );
            env->DeleteLocalRef(pointCornersArray);

            jobjectArray pointRejectedsArray = env->NewObjectArray( rejected_len, classCvPoint, NULL );
            for (int j = 0; j < rejected_len; j++)
            {
                jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mRejecteds[j].x, (double)mRejecteds[j].y );
                env->SetObjectArrayElement( pointRejectedsArray, j, objCvPoint );
                env->DeleteLocalRef(objCvPoint);
            }
            env->SetObjectField( objIrArucoMarker, fidmRejecteds, pointRejectedsArray);
            env->DeleteLocalRef(pointRejectedsArray);

            jobject objCvPoint3 = env->NewObject( classCvPoint3, point3Init, (double)m_cameraPosition.x, (double)m_cameraPosition.y, (double)m_cameraPosition.z);
            env->SetObjectField( objIrArucoMarker, fidcamPosition, objCvPoint3 );
            env->DeleteLocalRef(objCvPoint3);

            jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mCenter.x, (double)mCenter.y);
            env->SetObjectField( objIrArucoMarker, fidmCenter, objCvPoint);
            env->DeleteLocalRef(objCvPoint);

            // Set to the jobjectArray
            env->SetObjectArrayElement( outJNIArray, i, objIrArucoMarker );
            env->DeleteLocalRef(objIrArucoMarker);
        }
        if(JNI_DBG)
            LOGD("JNI_findMarkers_Adv_Value2Java...");
    }
    else
    {
        if(JNI_DBG)
            LOGD("JNI_findMarkers_Adv_NoValue2Java...");
    }
    // Release pointer
    env->DeleteLocalRef(classIrArucoMarker);
    env->DeleteLocalRef(classCvPoint);
    env->DeleteLocalRef(classCvPoint3);

    env->ReleaseByteArrayElements(bytes_, frame, 0);

    return outJNIArray;
    //return 0;
}


#ifdef __cplusplus
}
#endif
#endif