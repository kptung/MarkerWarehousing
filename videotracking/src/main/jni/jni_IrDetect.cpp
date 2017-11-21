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

#ifndef org_iii_snsi_videotracking_IrDetect
#define org_iii_snsi_videotracking_IrDetect
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_iii_snsi_videotracking_IrDetect_importYMLCameraParameters
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
Java_org_iii_snsi_videotracking_IrDetect_importYMLDetectParameters(
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
Java_org_iii_snsi_videotracking_IrDetect_importYMLRTParameters(
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
Java_org_iii_snsi_videotracking_IrDetect_findArucoMarkersWithMarkerSize(
        JNIEnv *env, jclass type, jbyteArray bytes_, jint width, jint height,
        jfloat markerSizeInMeter, jfloat distanceBelowMarkerCenter) {

  jbyte* frame = env->GetByteArrayElements(bytes_, 0);
	Mat matObjectAddress;
	Mat myuv(height + height/2, width, CV_8UC1, (uchar *)frame);
	cv::cvtColor(myuv, matObjectAddress, CV_YUV420sp2BGR);

	IrArAssistant assist;
	// the given marker length in meters
	assist.setMarkerLen(markerSizeInMeter);
	// set the injection points
	assist.setInjectionPoint(Point3f(0, distanceBelowMarkerCenter, 0));
	// assist.addInjectionPoint(Point3f(0, -0.05f, 0));
	// load device ID
	//int gid = 0;
	//assist.LoadParasFromID(gid);

        vector<IrArucoMarker> markers = vector<IrArucoMarker>();

        if(JNI_DBG)
                LOGD("findArucoMarkersWithMarkerSize");
        findArucoMarkers(matObjectAddress, markerSizeInMeter, markers);

        int arrayLength = (int)markers.size();
                if(JNI_DBG){
                    LOGD("%d", arrayLength);
                    for (int i = 0; i < arrayLength; i++)
                    {
                        LOGD("distance : %.2f", markers[i].getXZCameraDistance()*100);
                        const vector<Point2f> &mCorners = markers[i].getCorners();
                        int corner_len = (int) mCorners.size();
                        for (int j = 0; j < corner_len; j++)
                        {
                            LOGD("%f %f", mCorners[j].x, mCorners[j].y);
                        }
                    }
                }

        // For debug Image
        if (debug_mode) {
            static int debugid = 1;
            if((int)markers.size() > 0) {
                int lineType = 1;
                char buffer[256];
                sprintf( buffer, "ID: %d; ORI: %d; Dst: %.2f; Ang: %.2f", markers[0].getMarkerId(), markers[0].getMarkerOri(),
                         markers[0].getXZCameraDistance()*100,
                         atan2(markers[0].getCameraPosition().x, -markers[0].getCameraPosition().z) * 180 / 3.141592653589793);
                putText( matObjectAddress, buffer, Point(matObjectAddress.cols * 0.05, matObjectAddress.rows * 0.05), 0, 1, Scalar(0, 128, 128), 2, lineType);
                const vector<Point2f> &mCorners = markers[0].getCorners();
                                        int corner_len = (int) mCorners.size();
                                        for (int j = 0; j < corner_len; j++)
                                        {
                     //                   sprintf( buffer, "Corner: %f %f", mCorners[j].x, mCorners[j].y);
                     //                   putText( matObjectAddress, buffer, Point(matObjectAddress.cols * 0.05, matObjectAddress.rows * (0.15 + j*0.15)), 0, 1, Scalar(0, 128, 128), 2, lineType);
                                        }

                sprintf( buffer, "X: %.1f; Y: %.1f; Z: %.1f", markers[0].getCameraPosition().x, markers[0].getCameraPosition().y, markers[0].getCameraPosition().z);
                putText( matObjectAddress, buffer, Point(matObjectAddress.cols * 0.05, matObjectAddress.rows * 0.95), 0, 1, Scalar(0, 255, 255), 2, lineType);
                cv::imwrite(std::string("/sdcard/Debug/DBG_")+ToString(debugid)+JPG, matObjectAddress);
                debugid++;
            }
        }

        // Get a class reference
        jclass classIrArucoMarker = env->FindClass("org/iii/snsi/videotracking/IrArucoMarker");
        assert(classIrArucoMarker != NULL);

        jclass classCvPoint = env->FindClass("org/iii/snsi/videotracking/Point2D");
        assert(classCvPoint != NULL);

        jclass classCvPoint3 = env->FindClass("org/iii/snsi/videotracking/Point3D");
        assert(classCvPoint3 != NULL);

        // Get the Field ID of the instance variables
        jfieldID fidId = env->GetFieldID( classIrArucoMarker, "id", "I" );
        assert(fidId != NULL);

        jfieldID fidOri = env->GetFieldID( classIrArucoMarker, "ori", "I" );
        assert(fidOri != NULL);

        jfieldID fidDistance = env->GetFieldID( classIrArucoMarker, "distance", "D");
        assert(fidDistance != NULL);

        jfieldID fidCorners = env->GetFieldID( classIrArucoMarker, "corners", "[Lorg/iii/snsi/videotracking/Point2D;" );
        assert(fidCorners != NULL);

        jfieldID fidPosition = env->GetFieldID( classIrArucoMarker, "position", "Lorg/iii/snsi/videotracking/Point3D;" );
        assert(fidPosition != NULL);

        jfieldID fidInjectpoints = env->GetFieldID( classIrArucoMarker, "injectpoints", "[Lorg/iii/snsi/videotracking/Point2D;");
        assert(fidInjectpoints != NULL);

        // Get the Method ID of the constructor
        jmethodID pointInit = env->GetMethodID( classCvPoint, "<init>", "(DD)V");
        assert(pointInit != NULL);

        jmethodID point3Init = env->GetMethodID( classCvPoint3, "<init>", "(DDD)V");
        assert(point3Init != NULL);

        // Allocate a jobjectArray

        jobjectArray outJNIArray = env->NewObjectArray( arrayLength, classIrArucoMarker, NULL );

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

//            markers[i].setInjectionPoint(Point3f(0, distanceBelowMarkerCenter, 0));

//            std::vector<cv::Point2f> mInjectPoints = findInjection(markers[i].getInjectionPoints(), mRvec, mTvec, mOri, mCenter);
            std::vector<cv::Point2f> mInjectPoints;
//            if(gid==0)
//                mInjectPoints = findInjection(assist.getInjectionPoints(), rvec, tvec, mOri, mCenter);
//            else
                mInjectPoints = findInjection(assist.getInjectionPoints(), rvec, tvec, mOri, markers[i].getMarkerCenter());
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

            jobject objCvPoint3 = env->NewObject
                    ( classCvPoint3, point3Init, (double)m_cameraPosition.x, (double)m_cameraPosition.y, (double)m_cameraPosition.z);
            env->SetObjectField( objIrArucoMarker, fidPosition, objCvPoint3 );
            env->DeleteLocalRef(objCvPoint3);

            jobjectArray injectPointArray = env->NewObjectArray( injectPoints_len, classCvPoint, NULL );
            for (int j = 0; j < injectPoints_len; j++)
            {
                jobject objCvPoint = env->NewObject
                        ( classCvPoint, pointInit, (double)mInjectPoints[j].x, (double)mInjectPoints[j].y );
                env->SetObjectArrayElement( injectPointArray, j, objCvPoint );
                env->DeleteLocalRef(objCvPoint);
            }
            env->SetObjectField( objIrArucoMarker, fidInjectpoints, injectPointArray );
            env->DeleteLocalRef(injectPointArray);

//            markers[i].removeInjectionPoint();

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