#include <jni.h>
#include <android/log.h>
#include "rtklib.h"

#define LOG_TAG "RTKLIB_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT JNICALL jstring 
Java_com_example_gnssdemo_RtkLibWrapper_getRtklibVersion(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, VER_RTKLIB);
}

JNIEXPORT JNICALL jlong 
Java_com_example_gnssdemo_RtkLibWrapper_initRtk(JNIEnv *env, jobject thiz) {
    rtk_t *rtk = (rtk_t *)malloc(sizeof(rtk_t));
    rtkinit(rtk, &prcopt_default); // 使用默认配置初始化
    return (jlong)rtk;
}

JNIEXPORT JNICALL void 
Java_com_example_gnssdemo_RtkLibWrapper_freeRtk(JNIEnv *env, jobject thiz, jlong rtk_ptr) {
    rtk_t *rtk = (rtk_t *)rtk_ptr;
    rtkfree(rtk);
    free(rtk);
}

JNIEXPORT JNICALL jint 
Java_com_example_gnssdemo_RtkLibWrapper_processRtk(JNIEnv *env, jobject thiz, 
                                          jlong rtk_ptr, 
                                          jlong obs_ptr, 
                                          jint n_obs, 
                                          jlong nav_ptr) {
    rtk_t *rtk = (rtk_t *)rtk_ptr;
    obsd_t *obs = (obsd_t *)obs_ptr;
    nav_t *nav = (nav_t *)nav_ptr;

    /* 调用 RTKLIB 核心定位函数 */
    int stat = rtkpos(rtk, obs, n_obs, nav);
    
    // 返回状态：1:fix, 2:float, 5:single...
    return (jint)stat; 
}

// 在初始化时调用此逻辑，将 RTKLIB 的 trace 输出重定向
void internal_trace(int level, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    __android_log_vprint(ANDROID_LOG_DEBUG, "RTKLIB_TRACE", format, ap);
    va_end(ap);
}

int showmsg(const char *format,...){return 0;}
void settspan(gtime_t ts, gtime_t te){}
void settime(gtime_t time){}

#ifdef __cplusplus
}
#endif
