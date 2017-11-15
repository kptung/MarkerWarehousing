#include <jni.h>
#include <opencv2/opencv.hpp>

#ifndef _Included_org_iii_snsi_irglass_util_ConvertUtil
#define _Included_org_iii_snsi_irglass_util_ConvertUtil
#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
using namespace cv;

/*
 * Class:     org_iii_snsi_irglass_util_ConvertUtil
 * Method:    convertYUV2RGBA
 * Signature: (II[B)[I
 */
JNIEXPORT jintArray JNICALL
Java_org_iii_snsi_videotracking_convertYuvToRgba
        (JNIEnv *env, jclass jConvertUtil, jint jwidth, jint jheight, jbyteArray jyuv) {

    jbyte* _yuv = env->GetByteArrayElements(jyuv, 0);
    int* _bgra = new int[jwidth * jheight];

    cv::Mat myuv(jheight + jheight / 2, jwidth, CV_8UC1, (unsigned char *)_yuv);
    cv::Mat mbgra(jheight, jwidth, CV_8UC4, (unsigned char *)_bgra);

    //Please make attention about BGRA byte order
    //ARGB stored in java as int array becomes BGRA at native level
    cv::cvtColor(myuv, mbgra, CV_YUV420sp2BGR, 4);

    env->ReleaseByteArrayElements(jyuv, _yuv, 0);

    /* return bgra array */
    jintArray bgra = env->NewIntArray(jwidth * jheight);
    env->SetIntArrayRegion(bgra, 0, jwidth * jheight, _bgra);

    return bgra;
}

/*
 * Class:     org_iii_snsi_irglass_util_ConvertUtil
 * Method:    convertRGBA2YUV
 * Signature: (II[I)[B
 */
JNIEXPORT jbyteArray JNICALL Java_org_iii_snsi_videotracking_ConvertUtil_convertRgbaToYuv
        (JNIEnv *env, jclass jConvertUtil, jint jwidth, jint jheight, jintArray jbgra) {

    jint* _bgra = env->GetIntArrayElements(jbgra, 0);
    int size = (jheight + jheight / 2) * jwidth;
    jbyte* _yuv = new jbyte[size];

    cv::Mat mbgra(jheight, jwidth, CV_8UC4, (unsigned char *)_bgra);
    cv::Mat myuv(jheight + jheight / 2, jwidth, CV_8UC1, (unsigned char *)_yuv);

    cv::cvtColor(mbgra, myuv, CV_BGR2YUV);

    env->ReleaseIntArrayElements(jbgra, _bgra, 0);

    /* return yuv array */
    jbyteArray yuv = env->NewByteArray(size);
    env->SetByteArrayRegion(yuv, 0, size, _yuv);

    return yuv;
}

/*
 * Class:     org_iii_snsi_irglass_util_ConvertUtil
 * Method:    convertARGB2MAT
 * Signature: ([BIJ)Lorg/opencv/core/Mat;
 */
JNIEXPORT jobject JNICALL Java_org_iii_snsi_videotracking_ConvertUtil_convertArgbToMat
        (JNIEnv *env, jclass jConvertUtil, jbyteArray jargb, jint jsize) {
    // Get a class reference
    jclass classCvMat = env->FindClass("org/opencv/core/Mat");

    // Get the Field ID of the instance variables
    jmethodID matInit = env->GetMethodID(classCvMat, "<init>", "()V");
    jmethodID getPtrMethod = env->GetMethodID(classCvMat, "getNativeObjAddr", "()J");

    // Construct and return Mat
    jobject objCvMat = env->NewObject(classCvMat, matInit);
    Mat& matImage = *(Mat*)env->CallLongMethod(objCvMat, getPtrMethod);

    jbyte* frame = env->GetByteArrayElements(jargb, 0);
    Mat rawData = Mat(1, jsize, CV_8UC1, (uchar *)frame);
    matImage = imdecode(rawData, IMREAD_COLOR);

    // Release pointer
    env->DeleteLocalRef(classCvMat);

    return objCvMat;
}

/*
 * Class:     org_iii_snsi_irglass_util_ConvertUtil
 * Method:    convertNV212MAT
 * Signature: ([BJ)Lorg/opencv/core/Mat;
 */
JNIEXPORT jobject JNICALL Java_org_iii_snsi_videotracking_ConvertUtil_convertNv21ToMat
        (JNIEnv *env, jclass jConvertUtil, jbyteArray jyuv, jint imgWidth, jint imgHeight) {
    // Get a class reference
    jclass classCvMat = env->FindClass("org/opencv/core/Mat");

    // Get the Field ID of the instance variables
    jmethodID matInit = env->GetMethodID(classCvMat, "<init>", "()V");
    jmethodID getPtrMethod = env->GetMethodID(classCvMat, "getNativeObjAddr", "()J");

    // Construct and return Mat
    jobject objCvMat = env->NewObject(classCvMat, matInit);
    Mat& matImage = *(Mat*)env->CallLongMethod(objCvMat, getPtrMethod);

    jbyte* frame = env->GetByteArrayElements(jyuv, 0);
    Mat image;
    Mat myuv(imgHeight + imgHeight / 2, imgWidth, CV_8UC1, (uchar *)frame);
    cv::cvtColor(myuv, image, CV_YUV420sp2BGR);
    cv::cvtColor(myuv, matImage, CV_YUV420sp2BGR);

    // Release pointer
    env->DeleteLocalRef(classCvMat);
    env->ReleaseByteArrayElements(jyuv, frame, 0);

    return objCvMat;
}

#ifdef __cplusplus
}
#endif
#endif
