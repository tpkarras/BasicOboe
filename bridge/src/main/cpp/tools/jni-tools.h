#pragma once

class JNITools {
public:

    static std::string stringCheckNConvert(JNIEnv *env, jstring jStr);
    static JNIEXPORT void throwJavaException(JNIEnv *env, const char *className, const char *error);
};