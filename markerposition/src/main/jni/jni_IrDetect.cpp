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

static int imgHeight, imgWidth = 0;

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


JNIEXPORT jobjectArray JNICALL
Java_org_iii_snsi_videotracking_IrDetect_findArucoMarkersWithMarkerSize(
                                                                       JNIEnv *env,
                                                                       jclass type,
                                                                       jlong jmatObjectAddress,
                                                                       jfloat markerSizeInMeter,
                                                                      jfloat distanceBelowMarkerCenter) {




const Mat& matObjectAddress  = *(Mat*)jmatObjectAddress;
        vector<IrArucoMarker> markers = vector<IrArucoMarker>();

        if(JNI_DBG)
                LOGD("findArucoMarkersWithMarkerSize");
            findArucoMarkers(matObjectAddress, markerSizeInMeter, markers);

        if(JNI_DBG)
            LOGD("%d", markers.size());

        // For debug Image
        if (debug_mode) {
            static int debugid = 1;
            if((int)markers.size() > 0) {
                int lineType = 1;
                char buffer[256];
                sprintf( buffer, "ID: %d; ORI: %d; Dst: %.2f; Ang: %.2f", markers[0].getMarkerId(), markers[0].getMarkerOri(),
                         sqrt(markers[0].getCameraPosition().x * markers[0].getCameraPosition().x + markers[0].getCameraPosition().z * markers[0].getCameraPosition().z),
                         atan2(markers[0].getCameraPosition().x, -markers[0].getCameraPosition().z) * 180 / 3.141592653589793);
                putText( matObjectAddress, buffer, Point(matObjectAddress.cols * 0.05, matObjectAddress.rows * 0.05), 0, 1, Scalar(0, 128, 128), 2, lineType);
                sprintf( buffer, "X: %.1f; Y: %.1f; Z: %.1f", markers[0].getCameraPosition().x, markers[0].getCameraPosition().y, markers[0].getCameraPosition().z);
                putText( matObjectAddress, buffer, Point(matObjectAddress.cols * 0.05, matObjectAddress.rows * 0.95), 0, 1, Scalar(0, 255, 255), 2, lineType);
                cv::imwrite(std::string("/sdcard/IrMarkerTool/Debug/DBG_")+ToString(debugid)+JPG, matObjectAddress);
                debugid++;
            } else {
                cv::imwrite(std::string("/sdcard/IrMarkerTool/Debug/DBG_")+ToString(debugid)+JPG, matObjectAddress);
                debugid++;
            }
        }

        // Get a class reference
        jclass classIrArucoMarker = env->FindClass("org/iii/snsi/videotracking/IrArucoMarker");
        assert(classIrArucoMarker != NULL);

        jclass classCvPoint = env->FindClass("android/graphics/Point");
        assert(classCvPoint != NULL);

        jclass classCvPoint3 = env->FindClass("org/iii/snsi/videotracking/Point3D");
        assert(classCvPoint3 != NULL);

        // Get the Field ID of the instance variables
        jfieldID fidId = env->GetFieldID( classIrArucoMarker, "id", "I" );
        assert(fidId != NULL);

        jfieldID fidOri = env->GetFieldID( classIrArucoMarker, "ori", "I" );
        assert(fidOri != NULL);

        jfieldID fidCorners = env->GetFieldID( classIrArucoMarker, "corners", "[Landroid/graphics/Point;" );
        assert(fidCorners != NULL);

        jfieldID fidPosition = env->GetFieldID( classIrArucoMarker, "position", "Lorg/iii/snsi/videotracking/Point3D;" );
        assert(fidPosition != NULL);

        jfieldID fidInjectpoints = env->GetFieldID( classIrArucoMarker, "injectpoints", "[Landroid/graphics/Point;");
        assert(fidInjectpoints != NULL);

        // Get the Method ID of the constructor
        jmethodID pointInit = env->GetMethodID( classCvPoint, "<init>", "(DD)V");
        assert(pointInit != NULL);

        jmethodID point3Init = env->GetMethodID( classCvPoint3, "<init>", "(DDD)V");
        assert(point3Init != NULL);

        // Allocate a jobjectArray
        int arrayLength = (int)markers.size();
        jobjectArray outJNIArray = env->NewObjectArray( arrayLength, classIrArucoMarker, NULL );

        for (int i = 0; i < arrayLength; i++)
        {
            int mId = markers[i].getMarkerId();
            int mOri = markers[i].getMarkerOri();
            cv::Point2f mCenter = markers[i].getMarkerCenter();
            const vector<Point2f> &mCorners = markers[i].getCorners();
            int corner_len = (int) mCorners.size();
            const Point3f &m_cameraPosition = markers[i].getCameraPosition();
            cv::Mat mRvec = markers[i].getRotationMatrix();
            cv::Mat mTvec = markers[i].getTransnslationMatrix();

            markers[i].addInjectionPoint(Point3f(distanceBelowMarkerCenter, 0, 0));
            std::vector<cv::Point2f> mInjectPoints = findInjection(markers[i].getInjectionPoints(), mRvec, mTvec, mOri, mCenter);
            int injectPoints_len = (int) mInjectPoints.size();

            // Create an object instance
            jobject objIrArucoMarker = env->AllocObject( classIrArucoMarker );

            // Change the variable
            env->SetIntField( objIrArucoMarker, fidId, mId );
            env->SetIntField( objIrArucoMarker, fidOri, mOri );

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