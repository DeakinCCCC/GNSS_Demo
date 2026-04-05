#include <jni.h>
#include "rtklib.h"

// JNI 函数声明：必须使用 extern "C" 防止 C++ 名称修饰（即使在C文件中也需声明）
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jint JNICALL Java_com_example_gnssdemo_RtkLibWrapper_rtkpos(JNIEnv *env, jobject thiz, jstring rover, jstring base, jstring output) {

    // 获取 Java 字符串为 C 字符串
    const char *r = (*env)->GetStringUTFChars(env, rover, NULL);
    const char *b = (*env)->GetStringUTFChars(env, base, NULL);
    const char *o = (*env)->GetStringUTFChars(env, output, NULL);

    // 调用 RTKLIB 核心函数
    //int result = rtkpos(r, b, o, NULL);
    rtk_t rtk = {0};

    // 释放字符串资源
    (*env)->ReleaseStringUTFChars(env, rover, r);
    (*env)->ReleaseStringUTFChars(env, base, b);
    (*env)->ReleaseStringUTFChars(env, output, o);

    //return result;
    return 0;
}
#ifdef __cplusplus
}
#endif
