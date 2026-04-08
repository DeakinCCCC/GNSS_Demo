#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include "rtklib.h"

// 辅助函数：快速分配并清零内存
#define MALLOC_STRUCT(type) (jlong)(uintptr_t)calloc(1, sizeof(type))

// --- 辅助宏：将 Java long 转为 C 指针 ---
#define TO_PTR(type, addr) ((type *)(uintptr_t)(addr))

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL Java_com_example_gnssdemo_RtkLibWrapper_getRtklibVersion(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, VER_RTKLIB);
}

JNIEXPORT jlong JNICALL Java_com_example_gnssdemo_RtkLibWrapper_mallocStruct(JNIEnv *env, jclass cls, jstring typeName) {
    const char *name = (*env)->GetStringUTFChars(env, typeName, NULL);
    jlong addr = 0;

    if      (strcmp(name, "obs_t") == 0)    addr = MALLOC_STRUCT(obs_t);
    else if (strcmp(name, "obsd_t") == 0)    addr = MALLOC_STRUCT(obsd_t);
    else if (strcmp(name, "nav_t") == 0)    addr = MALLOC_STRUCT(nav_t);
    else if (strcmp(name, "rtcm_t") == 0)   addr = MALLOC_STRUCT(rtcm_t);
    else if (strcmp(name, "rtk_t") == 0)    addr = MALLOC_STRUCT(rtk_t);
    else if (strcmp(name, "prcopt_t") == 0) addr = MALLOC_STRUCT(prcopt_t);
    else if (strcmp(name, "sol_t") == 0)    addr = MALLOC_STRUCT(sol_t);
    else if (strcmp(name, "solopt_t") == 0)    addr = MALLOC_STRUCT(solopt_t);
    else if (strcmp(name, "gtime_t") == 0)  addr = MALLOC_STRUCT(gtime_t);
    else if (strcmp(name, "sta_t") == 0)    addr = MALLOC_STRUCT(sta_t);

    (*env)->ReleaseStringUTFChars(env, typeName, name);
    return addr;
}

JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_freeStruct(JNIEnv *env, jclass cls, jlong addr) {
    if (addr != 0) {
        free((void *)(uintptr_t)addr);
    }
}

// 特殊处理：RTK 结构体的初始化与释放（内部含有动态分配的成员）
JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_rtkInit(JNIEnv *env, jclass cls, jlong rtk_addr, jlong opt_addr) {
    rtk_t *rtk = (rtk_t *)(uintptr_t)rtk_addr;
    if (opt_addr == 0) {
        rtkinit(rtk, &prcopt_default); // 使用默认配置初始化
    }
    else {
        prcopt_t *opt = (prcopt_t *)(uintptr_t)opt_addr;
        rtkinit(rtk, opt);
    }
}

JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_rtkFree(JNIEnv *env, jclass cls, jlong rtk_addr) {
    rtk_t *rtk = (rtk_t *)(uintptr_t)rtk_addr;
    rtkfree(rtk);
}


// --- RTKLIB 的核心 API 同名映射---

// 1. readrnx (读取 RINEX 文件)
JNIEXPORT jint JNICALL Java_com_example_gnssdemo_RtkLibWrapper_readrnx(
    JNIEnv *env, jclass cls, jstring file, jint rcv, jlong obs_addr, jlong nav_addr, jlong sta_addr) {
    const char *path = (*env)->GetStringUTFChars(env, file, NULL);
    int ret = readrnx(path, rcv, "", TO_PTR(obs_t, obs_addr), TO_PTR(nav_t, nav_addr), TO_PTR(sta_t, sta_addr));
    (*env)->ReleaseStringUTFChars(env, file, path);
    return ret;
}

// 2. input_rtcm3 (输入单个字节)
JNIEXPORT jint JNICALL Java_com_example_gnssdemo_RtkLibWrapper_input_1rtcm3(
    JNIEnv *env, jclass cls, jlong rtcm_addr, jbyte data) {
    return input_rtcm3(TO_PTR(rtcm_t, rtcm_addr), (unsigned char)data);
}

// 3. input_rtcm3f (从文件输入)
JNIEXPORT jint JNICALL Java_com_example_gnssdemo_RtkLibWrapper_input_1rtcm3f(
    JNIEnv *env, jclass cls, jlong rtcm_addr, jlong file_ptr_addr) {
    return input_rtcm3f(TO_PTR(rtcm_t, rtcm_addr), TO_PTR(FILE, file_ptr_addr));
}

// 4. init_rtcm (初始化结构体)
JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_init_1rtcm(
    JNIEnv *env, jclass cls, jlong rtcm_addr) {
    init_rtcm(TO_PTR(rtcm_t, rtcm_addr));
}

// 5. pntpos (单点定位)
JNIEXPORT jint JNICALL Java_com_example_gnssdemo_RtkLibWrapper_pntpos(
    JNIEnv *env, jclass cls, jlong obs_addr, jint n, jlong nav_addr, jlong opt_addr, jlong sol_addr, jlong azel_addr) {
    char msg[128] = "";
    return pntpos(TO_PTR(obsd_t, obs_addr), n, TO_PTR(nav_t, nav_addr), 
                  TO_PTR(prcopt_t, opt_addr), TO_PTR(sol_t, sol_addr), 
                  TO_PTR(double, azel_addr), NULL, msg);
}

// 6. rtkpos (RTK/PPK 定位)
JNIEXPORT jint JNICALL Java_com_example_gnssdemo_RtkLibWrapper_rtkpos(
    JNIEnv *env, jclass cls, jlong rtk_addr, jlong obs_addr, jint n, jlong nav_addr) {
    return rtkpos(TO_PTR(rtk_t, rtk_addr), TO_PTR(obsd_t, obs_addr), n, TO_PTR(nav_t, nav_addr));
}

// 7. pppos (PPP 定位)
JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_pppos(
    JNIEnv *env, jclass cls, jlong rtk_addr, jlong obs_addr, jint n, jlong nav_addr) {
    pppos(TO_PTR(rtk_t, rtk_addr), TO_PTR(obsd_t, obs_addr), n, TO_PTR(nav_t, nav_addr));
}

// 8 & 9. ecef2pos / pos2ecef (坐标转换)
JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_ecef2pos(
    JNIEnv *env, jclass cls, jdoubleArray r, jdoubleArray pos) {
    jdouble *r_c = (*env)->GetDoubleArrayElements(env, r, NULL);
    jdouble *pos_c = (*env)->GetDoubleArrayElements(env, pos, NULL);
    ecef2pos(r_c, pos_c);
    (*env)->ReleaseDoubleArrayElements(env, r, r_c, JNI_ABORT);
    (*env)->ReleaseDoubleArrayElements(env, pos, pos_c, 0); // 写回结果
}

JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_pos2ecef(
    JNIEnv *env, jclass cls, jdoubleArray pos, jdoubleArray r) {
    jdouble *pos_c = (*env)->GetDoubleArrayElements(env, pos, NULL);
    jdouble *r_c = (*env)->GetDoubleArrayElements(env, r, NULL);
    pos2ecef(pos_c, r_c);
    (*env)->ReleaseDoubleArrayElements(env, pos, pos_c, JNI_ABORT);
    (*env)->ReleaseDoubleArrayElements(env, r, r_c, 0);
}

// 10 & 11. xyz2enu / enu2xyz (ENU转换)
JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_xyz2enu(
    JNIEnv *env, jclass cls, jdoubleArray pos, jdoubleArray E) {
    jdouble *pos_c = (*env)->GetDoubleArrayElements(env, pos, NULL);
    jdouble *E_c = (*env)->GetDoubleArrayElements(env, E, NULL);
    xyz2enu(pos_c, E_c);
    (*env)->ReleaseDoubleArrayElements(env, pos, pos_c, JNI_ABORT);
    (*env)->ReleaseDoubleArrayElements(env, E, E_c, 0);
}
/***
JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_enu2xyz(
    JNIEnv *env, jclass cls, jdoubleArray pos, jdoubleArray E, jdoubleArray r) {
    jdouble *pos_c = (*env)->GetDoubleArrayElements(env, pos, NULL);
    jdouble *E_c = (*env)->GetDoubleArrayElements(env, E, NULL);
    jdouble *r_c = (*env)->GetDoubleArrayElements(env, r, NULL);
    enu2xyz(pos_c, E_c, r_c);
    (*env)->ReleaseDoubleArrayElements(env, pos, pos_c, JNI_ABORT);
    (*env)->ReleaseDoubleArrayElements(env, E, E_c, JNI_ABORT);
    (*env)->ReleaseDoubleArrayElements(env, r, r_c, 0);
}***/

// 12 & 13. gpst2utc / utc2gpst (时间转换)
JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_gpst2utc(
    JNIEnv *env, jclass cls, jlong t_addr, jlong t_utc_addr) {
    gtime_t t = *TO_PTR(gtime_t, t_addr);
    *TO_PTR(gtime_t, t_utc_addr) = gpst2utc(t);
}

JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_utc2gpst(
    JNIEnv *env, jclass cls, jlong t_utc_addr, jlong t_addr) {
    gtime_t t_utc = *TO_PTR(gtime_t, t_utc_addr);
    *TO_PTR(gtime_t, t_addr) = utc2gpst(t_utc);
}


// --- 日志输出---

JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_traceopen(JNIEnv *env, jclass cls, jstring file) {
    const char *path = (*env)->GetStringUTFChars(env, file, NULL);
    traceopen(path);
    (*env)->ReleaseStringUTFChars(env, file, path);
}

JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_traceclose(JNIEnv *env, jclass cls) {
    traceclose();
}

JNIEXPORT void JNICALL Java_com_example_gnssdemo_RtkLibWrapper_tracelevel(JNIEnv *env, jclass cls, jint level) {
    tracelevel(level);
}


// --- 额外处理---

// 未实现的函数，不添加则无法通过编译
int showmsg(const char *format,...){return 0;}
void settspan(gtime_t ts, gtime_t te){}
void settime(gtime_t time){}

#ifdef __cplusplus
}
#endif
