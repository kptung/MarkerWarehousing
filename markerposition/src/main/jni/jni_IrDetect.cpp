#include <jni.h>

#include <stdlib.h>     /* NULL */
#include <assert.h>     /* assert */
#include <math.h>
#include <vector>
//#include <string>
//#include <time.h>
#include "IrArInterface.h"

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

JNIEXPORT jboolean JNICALL
Java_org_iii_snsi_markerposition_IrDetect_importYMLRTParameters(
        JNIEnv *env, jclass type, jstring jfilename) {
    // Reconstruct Java structure to C++ structure
    string filename;
    GetJStringContent(env, jfilename, filename);

    // Call IrCalibration method
    if(JNI_DBG)
        LOGD("importYMLRTParameters is %s", filename.c_str());
    return (jboolean)importYMLRTParameters(filename);
}



JNIEXPORT jobjectArray JNICALL
Java_org_iii_snsi_markerposition_IrDetect_findArucoMarkersWithMarkerSize(JNIEnv *env, jclass type, jbyteArray bytes_, jint width, jint height, jfloat markerSizeInMeter)
{
    jbyte* frame = env->GetByteArrayElements(bytes_, 0);
	Mat matObjectAddress;
	Mat myuv(height + height/2, width, CV_8UC1, (uchar *)frame);
	cv::cvtColor(myuv, matObjectAddress, CV_YUV420sp2BGR);

	IrArAssistant assist;
	// the given marker length in meters
	assist.setMarkerLen(markerSizeInMeter);


    vector<IrArucoMarker> markers = vector<IrArucoMarker>();

    if(JNI_DBG)
        LOGD("findArucoMarkersWithMarkerSize");
    findArucoMarkers(matObjectAddress, markerSizeInMeter, markers);
    int arrayLength = (int)markers.size();

    // Get a class reference
    jclass classIrArucoMarker = env->FindClass("org/iii/snsi/markerposition/IrArucoMarker");
    assert(classIrArucoMarker != NULL);

    jclass classCvPoint = env->FindClass("org/iii/snsi/markerposition/Point2D");
    assert(classCvPoint != NULL);

    jclass classCvPoint3 = env->FindClass("org/iii/snsi/markerposition/Point3D");
    assert(classCvPoint3 != NULL);

    // Get the Field ID of the instance variables
    jfieldID fidId = env->GetFieldID( classIrArucoMarker, "id", "I" );
    assert(fidId != NULL);

    jfieldID fidOri = env->GetFieldID( classIrArucoMarker, "ori", "I" );
    assert(fidOri != NULL);

    jfieldID fidDistance = env->GetFieldID( classIrArucoMarker, "distance", "D");
    assert(fidDistance != NULL);

    jfieldID fidCorners = env->GetFieldID( classIrArucoMarker, "corners", "[Lorg/iii/snsi/markerposition/Point2D;" );
    assert(fidCorners != NULL);

    jfieldID fidPosition = env->GetFieldID( classIrArucoMarker, "position", "Lorg/iii/snsi/markerposition/Point3D;" );
    assert(fidPosition != NULL);

    jfieldID fidInjectpoints = env->GetFieldID( classIrArucoMarker, "injectpoints", "[Lorg/iii/snsi/markerposition/Point2D;");
    assert(fidInjectpoints != NULL);

    // Get the Method ID of the constructor
    jmethodID pointInit = env->GetMethodID( classCvPoint, "<init>", "(DD)V");
    assert(pointInit != NULL);

    jmethodID point3Init = env->GetMethodID( classCvPoint3, "<init>", "(DDD)V");
    assert(point3Init != NULL);

    // Allocate a jobjectArray
    jobjectArray outJNIArray = env->NewObjectArray( arrayLength, classIrArucoMarker, NULL );

    // marker info
    for (int i = 0; i < arrayLength; i++)
    {
        int mId = markers[i].getMarkerId();
        double xzdist = markers[i].getXZCameraDistance() * 100;
        int mOri = markers[i].getMarkerOri();
        cv::Point2f mCenter = markers[i].getMarkerCenter();
        const vector<Point2f> &mCorners = markers[i].getCorners();
        int corner_len = (int) mCorners.size();
        const Point3f &m_cameraPosition = markers[i].getCameraPosition();
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
          	cv::Point3f Injection(0, 0, 0);
          	Injectionpts = make_vector<cv::Point3f>() << Injection;
        }
        std::vector<cv::Point2f> mInjectPoints = findInjection(Injectionpts, rvec, tvec, mOri, markers[i].getMarkerCenter());
        int injectPoints_len = (int) mInjectPoints.size();

        // Create an object instance
        jobject objIrArucoMarker = env->AllocObject( classIrArucoMarker );

        // Change the variable
        env->SetIntField( objIrArucoMarker, fidId, mId );
        env->SetIntField( objIrArucoMarker, fidOri, mOri );
        env->SetDoubleField( objIrArucoMarker, fidDistance, xzdist );

        jobjectArray pointCornersArray = env->NewObjectArray( corner_len, classCvPoint, NULL );
        for (int j = 0; j < corner_len; j++)
        {
            jobject objCvPoint = env->NewObject( classCvPoint, pointInit, (double)mCorners[j].x, (double)mCorners[j].y );
            env->SetObjectArrayElement( pointCornersArray, j, objCvPoint );
            env->DeleteLocalRef(objCvPoint);
        }
        env->SetObjectField( objIrArucoMarker, fidCorners, pointCornersArray );
        env->DeleteLocalRef(pointCornersArray);

        jobject objCvPoint3 = env->NewObject( classCvPoint3, point3Init, (double)m_cameraPosition.x, (double)m_cameraPosition.y, (double)m_cameraPosition.z);
        env->SetObjectField( objIrArucoMarker, fidPosition, objCvPoint3 );
        env->DeleteLocalRef(objCvPoint3);

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

    // Release pointer
    env->DeleteLocalRef(classIrArucoMarker);
    env->DeleteLocalRef(classCvPoint);
    env->DeleteLocalRef(classCvPoint3);

    return outJNIArray;


}

#ifdef __cplusplus
}
#endif
#endif