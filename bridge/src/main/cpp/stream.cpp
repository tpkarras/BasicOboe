#include <oboe/Oboe.h>
#include <jni.h>
#include "jni-tools.h"
#include <android/log.h>
#include "callback.h"

extern "C" {

std::mutex mStreamMutex;
std::shared_ptr<BasicOboeErrorCallback> mErrorCallback = std::make_shared<BasicOboeErrorCallback>();
std::shared_ptr<BasicOboeEngine> mDataCallback = std::make_shared<BasicOboeEngine>();
std::shared_ptr<BasicOboeDuplexEngine> mDuplexDataCallback = std::make_shared<BasicOboeDuplexEngine>();
std::shared_ptr<oboe::AudioStream> BasicOboeEngine::stream;
std::shared_ptr<oboe::AudioStream> BasicOboeDuplexEngine::inputStream;
std::shared_ptr<oboe::AudioStream> BasicOboeDuplexEngine::outputStream;
std::unique_ptr<oboe::FifoBuffer> Fifo::inputFifo;
std::unique_ptr<oboe::FifoBuffer> Fifo::outputFifo;
bool duplex = false;

JNIEXPORT void JNICALL Java_org_tpkarras_basicoboe_bridge_Stream_deviceInitialize(JNIEnv *env, jobject thiz, jstring packageName, jstring attributionTag, jint deviceID, jstring format, jint sampleRate, jint channels, jint channelMask, jstring audioAPI, jstring performanceMode, jint direction, jboolean sharingMode, jint framesPerCallback, jboolean isSpatial, jstring usage){
    std::lock_guard<std::mutex> lock(mStreamMutex);
    oboe::AudioStreamBuilder instance;
    jlong handle = 0;

    std::string target = JNITools::stringCheckNConvert(env, format);

    //auto* instance = reinterpret_cast<oboe::AudioStreamBuilder*>(handle);

    if (!target.empty()) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Format");
        if (target == "I16") {
            instance.setFormat(oboe::AudioFormat::I16);
        } else if (target == "Float") {
            instance.setFormat(oboe::AudioFormat::Float);
        } else if (target == "I24") {
            instance.setFormat(oboe::AudioFormat::I24);
        } else if (target == "I32") {
            instance.setFormat(oboe::AudioFormat::I32);
        } else if (target == "IEC61937") {
            instance.setFormat(oboe::AudioFormat::IEC61937);
        } else if (target == "MP3") {
            instance.setFormat(oboe::AudioFormat::MP3);
        } else if (target == "AAC_LC") {
            instance.setFormat(oboe::AudioFormat::AAC_LC);
        } else if (target == "AAC_HE_V1") {
            instance.setFormat(oboe::AudioFormat::AAC_HE_V1);
        } else if (target == "AAC_HE_V2") {
            instance.setFormat(oboe::AudioFormat::AAC_HE_V2);
        } else if (target == "AAC_ELD") {
            instance.setFormat(oboe::AudioFormat::AAC_ELD);
        } else if (target == "AAC_XHE") {
            instance.setFormat(oboe::AudioFormat::AAC_XHE);
        } else if (target == "OPUS") {
            instance.setFormat(oboe::AudioFormat::OPUS);
        }
    }

    if(sampleRate > 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Sample Rate");
        instance.setSampleRate(static_cast<int32_t>(sampleRate));
    }

    if(channels > 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Channel Count");
        instance.setChannelCount(channels);
    }

    if(channelMask > 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Channel Mask");
        instance.setChannelMask(static_cast<oboe::ChannelMask>(channelMask));
    }

    target = JNITools::stringCheckNConvert(env, audioAPI);
    if (!target.empty()) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Audio API");
        if (target == "AAudio") {
            instance.setAudioApi(oboe::AudioApi::AAudio);
        } else if (target == "OpenSL_ES") {
            instance.setAudioApi(oboe::AudioApi::OpenSLES);
        } else if (target == "Unspecified") {
            instance.setAudioApi(oboe::AudioApi::Unspecified);
        }
    }

    target = JNITools::stringCheckNConvert(env, performanceMode);

    if (!target.empty()) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Performance Mode");
        if (target == "LowLatency") {
            instance.setPerformanceMode(oboe::PerformanceMode::LowLatency);
        } else if (target == "PowerSaving") {
            instance.setPerformanceMode(oboe::PerformanceMode::PowerSaving);
        } else if (target == "PowerSaving_Offloaded") {
            instance.setPerformanceMode(oboe::PerformanceMode::PowerSavingOffloaded);
        }
    }

    __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting AudioDirection");
    if(direction == 0){
            instance.setDirection(oboe::Direction::Output);
    } else if(direction == 1){
            instance.setDirection(oboe::Direction::Input);
    }

    __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Sharing Mode");
    if(!sharingMode){
        instance.setSharingMode(oboe::SharingMode::Shared);
    } else {
        instance.setSharingMode(oboe::SharingMode::Exclusive);
    }

    if(isSpatial){
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Spatialization Flag");
        instance.setIsContentSpatialized(true);
    }

    instance.setFormatConversionAllowed(true);

    if(framesPerCallback > 0){
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Frames Per Callback");
        instance.setFramesPerDataCallback(framesPerCallback);
    }

    target = JNITools::stringCheckNConvert(env, usage);

    if (!target.empty()) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Usage");
        if (target == "Media") {
            instance.setUsage(oboe::Usage::Media);
        } else if (target == "VoiceCommunication") {
            instance.setUsage(oboe::Usage::VoiceCommunication);
        } else if (target == "Notification") {
            instance.setUsage(oboe::Usage::Notification);
        } else if (target == "NotificationRingtone") {
            instance.setUsage(oboe::Usage::NotificationRingtone);
        } else if (target == "NotificationEvent") {
            instance.setUsage(oboe::Usage::NotificationEvent);
        } else if (target == "AssistanceAccessibility") {
            instance.setUsage(oboe::Usage::AssistanceAccessibility);
        } else if (target == "AssistanceNavigationGuidance") {
            instance.setUsage(oboe::Usage::AssistanceNavigationGuidance);
        } else if (target == "AssistanceSonification") {
            instance.setUsage(oboe::Usage::AssistanceSonification);
        } else if (target == "Game") {
            instance.setUsage(oboe::Usage::Game);
        } else if (target == "VoiceCommunicationSignalling") {
            instance.setUsage(oboe::Usage::VoiceCommunicationSignalling);
        } else if (target == "Assistant") {
            instance.setUsage(oboe::Usage::Assistant);
        }
    }

    instance.setErrorCallback(mErrorCallback);

    target = JNITools::stringCheckNConvert(env, packageName);
    instance.setPackageName(target);

    target = JNITools::stringCheckNConvert(env, attributionTag);

    if(!target.empty()){
        instance.setAttributionTag(target);
    }

    if(direction != 2){
         __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Opening stream");
        instance.setDataCallback(mDataCallback);
         oboe::Result result = instance.openStream(BasicOboeEngine::stream);

        if(result != oboe::Result::OK || BasicOboeEngine::stream == nullptr){
           JNITools::throwJavaException(env, "java/lang/Exception", "Failed to open Oboe audio stream");
           return;
        }

        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Success opening stream");
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Creating Fifo buffer");

        if(direction == 0){
            Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(BasicOboeEngine::stream->getChannelCount() * sizeof(float), BasicOboeEngine::stream->getFramesPerBurst() * 2);
        } else if(direction == 1){
            Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(BasicOboeEngine::stream->getChannelCount() * sizeof(float), BasicOboeEngine::stream->getFramesPerBurst() * 2);
        }

        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Starting stream");

        oboe::Result result2 = BasicOboeEngine::stream->requestStart();

        if(result2 != oboe::Result::OK || BasicOboeEngine::stream == nullptr){
            JNITools::throwJavaException(env, "java/lang/Exception", "Failed to start Oboe audio stream");
           return;
        }

    __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Success starting stream");
    } else {
        duplex = true;
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Preparing duplex stream");
        instance.setDataCallback(mDuplexDataCallback);
        mDuplexDataCallback->setSharedInputStream(BasicOboeDuplexEngine::inputStream); // Mandatory
        mDuplexDataCallback->setOutputStream(BasicOboeDuplexEngine::outputStream.get());

        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Starting duplex stream");

        oboe::Result result = mDuplexDataCallback->start();

        if(result != oboe::Result::OK || BasicOboeDuplexEngine::inputStream == nullptr && BasicOboeDuplexEngine::outputStream == nullptr){
            JNITools::throwJavaException(env, "java/lang/Exception", "Failed to start Oboe duplex stream");
            return;
        }

        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Creating Fifo buffer");

        Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(BasicOboeDuplexEngine::outputStream->getChannelCount() * sizeof(float), BasicOboeDuplexEngine::outputStream->getFramesPerBurst() * 2);
        Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(BasicOboeDuplexEngine::inputStream->getChannelCount() * sizeof(float), BasicOboeDuplexEngine::inputStream->getFramesPerBurst() * 2);

        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Success starting duplex stream");
    }

}

JNIEXPORT void JNICALL Java_org_tpkarras_basicoboe_bridge_Stream_deviceTerminate(JNIEnv *env, jobject thiz) {
    __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Closing stream");
    if (!duplex) {
        BasicOboeEngine::stream->requestStop();
        BasicOboeEngine::stream->close();
        BasicOboeEngine::stream.reset();
    } else {
        mDuplexDataCallback->stop();
        BasicOboeDuplexEngine::inputStream->close();
        BasicOboeDuplexEngine::inputStream.reset();
        BasicOboeDuplexEngine::outputStream->close();
        BasicOboeDuplexEngine::outputStream.reset();
    }
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Successfully closed stream");

}

JNIEXPORT jobjectArray JNICALL Java_org_tpkarras_basicoboe_bridge_Stream_readInput(JNIEnv *env, jobject thiz) {
    int32_t channelCount;
    int32_t totalSamples;
    int32_t framesPerBurst;
if(!duplex){
    channelCount = BasicOboeEngine::stream->getChannelCount();
    framesPerBurst = BasicOboeEngine::stream->getFramesPerBurst();
} else {
    channelCount = BasicOboeDuplexEngine::inputStream->getChannelCount();
    framesPerBurst = BasicOboeDuplexEngine::inputStream->getFramesPerBurst();
}
totalSamples = framesPerBurst * channelCount;

    jfloatArray resultJava = env->NewFloatArray(totalSamples);
    std::vector<float> tempBuffer(totalSamples);
    Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

    jfloat *resultRawEntry = tempBuffer.data();
    env->SetFloatArrayRegion(resultJava, 0, totalSamples, resultRawEntry);

    return (jobjectArray)resultJava;

}

JNIEXPORT void JNICALL Java_org_tpkarras_basicoboe_bridge_Stream_writeOutput(JNIEnv *env, jobject thiz, jfloatArray output) {
    int32_t channelCount;
    int32_t totalSamples;
    int32_t framesPerBurst;
    if(!duplex){
        channelCount = BasicOboeEngine::stream->getChannelCount();
        framesPerBurst = BasicOboeEngine::stream->getFramesPerBurst();
    } else {
        channelCount = BasicOboeDuplexEngine::outputStream->getChannelCount();
        framesPerBurst = BasicOboeDuplexEngine::outputStream->getFramesPerBurst();
    }
    totalSamples = framesPerBurst * channelCount;

    std::vector<float> tempBuffer(totalSamples);

    env->GetFloatArrayRegion(output, 0, totalSamples, tempBuffer.data());

    Fifo::outputFifo->write(tempBuffer.data(), framesPerBurst);

}

}