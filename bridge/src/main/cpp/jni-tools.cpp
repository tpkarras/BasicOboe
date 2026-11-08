#include <jni.h>
#include <string>
#include "jni-tools.h"

    std::string JNITools::stringCheckNConvert(JNIEnv *env, jstring jStr){
        if (!jStr) return "";

        const char* cStr = env->GetStringUTFChars(jStr, nullptr);

        if (!cStr) return "";

        std::string result = cStr;

        env->ReleaseStringUTFChars(jStr, cStr);

        return result;

    }

    JNIEXPORT void JNITools::throwJavaException(JNIEnv *env, const char *className, const char *error){

    jclass exClass = env->FindClass(className);

    if (exClass == nullptr) {
        return;
    }

    env->ThrowNew(exClass, error);
    env->DeleteLocalRef(exClass);

}