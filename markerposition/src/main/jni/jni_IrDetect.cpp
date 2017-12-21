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

using namespace std;
using namespace cv;

static bool debug_mode = false;

#ifndef org_iii_snsi_markerposition_IrDetect
#define org_iii_snsi_markerposition_IrDetect
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_iii_snsi_markerposition_IrDetect_importYMLCameraParameters
        (JNIEnv *env, jclass jIrDetect, jstring jfilename) {
    // Reconstruct Java structure to C++ structure
    string filename;
    GetJStringContent(env, jfilename, filename);

    // Call IrCalibration method
    if(JNI_DBG)
        LOGD("importYMLCameraParameters is %s", filename.c_str());
    return (jboolean)importYMLCameraParameters(filename);
}

JNIEXPORT jboolean JNICALL
Java_org_iii_snsi_markerposition_IrDetect_importYMLDetectParameters(
        JNIEnv *env, jclass type, jstring jfilename) {
    // Reconstruct Java structure to C++ structure
    string filename;
    GetJStringContent(env, jfilename, filename);

    // Call IrCalibration method
    if(JNI_DBG)
        LOGD("importYMLDetectParameters is %s", filename.c_str());
    return (jboolean)importYMLDetectParameters(filename);
}

JNIEXPORT jobjectArray JNICALL
Java_org_iii_snsi_markerposition_IrDetect_findArucoMarkersWithMarkerSize(JNIEnv *env, jclass type, jbyteArray bytes_, jint width, jint height, jfloat markerSize)
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
        LOGD("JNI_findMarkers_Start...");
    vector<IrArucoMarker> markers = vector<IrArucoMarker>();
    auto tstart = std::chrono::high_resolution_clock::now();
    bool flag = findArucoMarkers(image, markerSize, markers);
    //bool flag = findArucoMarkers(image, markers);
    auto tend = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(tend - tstart);
    if(JNI_DBG)
            LOGD("JNI_findMarkers_End...");
    // time estimation
    bool tflag=false;
    if(tflag)
    {
        ofstream out1;
        char *ptimefile_name = "/sdcard/marker/mtime.txt";
        out1.open(ptimefile_name, ios::app);
        out1 << "JNI: "<< (long)(1000 * diff.count()) << std::endl;
        out1.close();
    }

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

        jfieldID fidmxzDistance = env->GetFieldID( classIrArucoMarker, "mxzdistance", "D");
        assert(fidmxzDistance != NULL);

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
            int mOri = markers[i].getMarkerOri();
            cv::Point2f mCenter = markers[i].getMarkerCenter();
            const std::vector<cv::Point2f> &mCorners = markers[i].getCorners();
            int corner_len = (int) mCorners.size();
            const std::vector<cv::Point2f> &mRejecteds = markers[i].getRejecteds();
            int rejected_len = (int) mRejecteds.size();
            const cv::Point3f &m_cameraPosition = markers[i].getCameraPosition();
            cv::Mat rvec = markers[i].getRotationMatrix();
            cv::Mat tvec = markers[i].getTransnslationMatrix();

            std::vector<cv::Point3f> Injectionpts;
            if (mId == 666)
            {
                // find injection pts
                cv::Point3f Injection(0, -0.04f, 0);
                Injectionpts = make_vector<cv::Point3f>() << Injection;
                Injectionpts.push_back(cv::Point3f(0, -0.065f, 0));
            }
            else if (mId == 777)
            {
          	    // find injection pts
          	    cv::Point3f Injection(0.08f, 0, 0);
          	    Injectionpts = make_vector<cv::Point3f>() << Injection;
          	    Injectionpts.push_back(cv::Point3f(0, 0.08f, 0));
            }
            std::vector<cv::Point2f> mInjectPoints = findInjection(Injectionpts, rvec, tvec, mOri, markers[i].getMarkerCenter());
            int injectPoints_len = (int) mInjectPoints.size();

            // Create an object instance
            jobject objIrArucoMarker = env->AllocObject( classIrArucoMarker );

            // Change the variable
            env->SetIntField( objIrArucoMarker, fidmId, mId );
            env->SetIntField( objIrArucoMarker, fidmOri, mOri );
            env->SetDoubleField( objIrArucoMarker, fidmxzDistance, mXZdist );

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
    }
     if(JNI_DBG)
        LOGD("JNI_findMarkers_PassValue2Java...");
    // Release pointer
    env->DeleteLocalRef(classIrArucoMarker);
    env->DeleteLocalRef(classCvPoint);
    env->DeleteLocalRef(classCvPoint3);

    env->ReleaseByteArrayElements(bytes_, frame, 0);

    return outJNIArray;
    //return 0;
}
JNIEXPORT jobjectArray JNICALL
Java_org_iii_snsi_markerposition_IrDetect_findBasicMarkers(JNIEnv *env, jclass type, jbyteArray bytes_, jint width, jint height, jfloat markerSize)
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
    // time estimation
    bool tflag=false;
    if(tflag)
    {
        ofstream out1;
        char *ptimefile_name = "/sdcard/marker/mtime.txt";
        out1.open(ptimefile_name, ios::app);
        out1 << "JNI: "<< (long)(1000 * diff.count()) << std::endl;
        out1.close();
    }

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

        jfieldID fidmCorners = env->GetFieldID( classIrArucoMarker, "mcorners", "[Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmCorners != NULL);

        jfieldID fidmRejecteds = env->GetFieldID( classIrArucoMarker, "mrejecteds", "[Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmRejecteds != NULL);

        jfieldID fidmCenter = env->GetFieldID( classIrArucoMarker, "mcenter", "Lorg/iii/snsi/markerposition/Point2D;" );
        assert(fidmCenter != NULL);

        // Get the Method ID of the constructor
        jmethodID pointInit = env->GetMethodID( classCvPoint, "<init>", "(DD)V");
        assert(pointInit != NULL);

        jmethodID point3Init = env->GetMethodID( classCvPoint3, "<init>", "(DDD)V");
        assert(point3Init != NULL);

        // marker info
        for (int i = 0; i < arrayLength; i++)
        {
            int mId = markers[i].getMarkerId();
            int mOri = markers[i].getMarkerOri();
            cv::Point2f mCenter = markers[i].getMarkerCenter();
            const std::vector<cv::Point2f> &mCorners = markers[i].getCorners();
            int corner_len = (int) mCorners.size();
            const std::vector<cv::Point2f> &mRejecteds = markers[i].getRejecteds();
            int rejected_len = (int) mRejecteds.size();

            // Create an object instance
            jobject objIrArucoMarker = env->AllocObject( classIrArucoMarker );

            // Change the variable
            env->SetIntField( objIrArucoMarker, fidmId, mId );
            env->SetIntField( objIrArucoMarker, fidmOri, mOri );

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

            // Set to the jobjectArray
            env->SetObjectArrayElement( outJNIArray, i, objIrArucoMarker );
            env->DeleteLocalRef(objIrArucoMarker);
        }
    }
     if(JNI_DBG)
        LOGD("JNI_findMarkers_PassValue2Java...");
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