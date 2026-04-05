#include <jni.h>
#include "rtklib.h"

JNIEXPORT jint JNICALL Java_com_example_gnssdemo_RtkLibWrapper_rtkpos(JNIEnv *env, jobject thiz, jstring rover, jstring base, jstring output) {
    const char *r = (*env)->GetStringUTFChars(rover, 0);
    const char *b = (*env)->GetStringUTFChars(base, 0);
    const char *o = (*env)->GetStringUTFChars(output, 0);

    int result = rtkpos(r, b, o, 0, nullptr);

    (*env)->ReleaseStringUTFChars(rover, r);
    (*env)->ReleaseStringUTFChars(base, b);
    (*env)->ReleaseStringUTFChars(output, o);

    return result;
}
