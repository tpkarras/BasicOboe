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

JNIEXPORT void JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_deviceInitialize(JNIEnv *env, jobject thiz,
                                                           jstring packageName,
                                                           jstring attributionTag,
                                                           jint deviceIDInput, jint deviceIDOutput,
                                                           jstring inputFormat, jstring outputFormat, jint inputSampleRate,
                                                           jint outputSampleRate,
                                                           jint inputChannels, jint outputChannels,
                                                           jint inputChannelMask,
                                                           jint outputChannelMask, jstring audioAPI,
                                                           jstring performanceMode,
                                                           jboolean sharingMode,
                                                           jint framesPerCallback,
                                                           jboolean inputIsSpatial,
                                                           jboolean outputIsSpatial,
                                                           jstring inputUsage,
                                                           jstring outputUsage) {
    std::lock_guard<std::mutex> lock(mStreamMutex);
    oboe::AudioStreamBuilder instance;

    std::vector<int> instanceTasks = {};

    if (deviceIDOutput > 0) {
        instanceTasks.push_back(0);
    }

    if (deviceIDInput > 0) {
        instanceTasks.push_back(1);
    }

    for (int current: instanceTasks) {

        int sampleRate;
        int channels;
        int channelMask;
        char *currentType;
        std::string target;

        if (instanceTasks.size() == 2 && current == 0 || instanceTasks.size() == 1) {
            if (current == 0) {
                currentType = (char *) "Output";
                instance.setDeviceId(deviceIDOutput);
                instance.setDirection(oboe::Direction::Output);
            } else if (current == 1) {
                currentType = (char *) "Input";
                instance.setDeviceId(deviceIDInput);
                instance.setDirection(oboe::Direction::Input);
            }

            if (current == 0) {
                target = JNITools::stringCheckNConvert(env, outputFormat);
            } else {
                target = JNITools::stringCheckNConvert(env, inputFormat);
            }

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

            if (current == 0) {
                sampleRate = outputSampleRate;
            } else {
                sampleRate = inputSampleRate;
            }

            if (sampleRate > 0) {
                __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Sample Rate",
                                    currentType);
                instance.setSampleRate(static_cast<int32_t>(sampleRate));
            }

        }

        if (current == 0) {
            channels = outputChannels;
        } else {
            channels = inputChannels;
        }

        if (channels > 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Channel Count",
                                currentType);
            instance.setChannelCount(channels);
        }

        if (current == 0) {
            channelMask = outputChannelMask;
        } else {
            channelMask = inputChannelMask;
        }

        if (channelMask > 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Channel Mask",
                                currentType);
            instance.setChannelMask(static_cast<oboe::ChannelMask>(channelMask));
        }

        if (instanceTasks.size() == 2 || current == 0 || instanceTasks.size() == 1) {

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

            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Sharing Mode");
            if (!sharingMode) {
                instance.setSharingMode(oboe::SharingMode::Shared);
            } else {
                instance.setSharingMode(oboe::SharingMode::Exclusive);
            }

        }

        if (current == 0 && inputIsSpatial || current == 1 && outputIsSpatial) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Spatialization Flag",
                                currentType);
            instance.setIsContentSpatialized(true);
        }

        if (current == 0) {
            target = JNITools::stringCheckNConvert(env, inputUsage);
        } else if (current == 1) {
            target = JNITools::stringCheckNConvert(env, outputUsage);
        }

        if (!target.empty()) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Usage", currentType);
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

        if (instanceTasks.size() == 2 || current == 0 || instanceTasks.size() == 1) {

            instance.setFormatConversionAllowed(true);

            if (framesPerCallback > 0) {
                __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder",
                                    "Setting Frames Per Callback");
                instance.setFramesPerDataCallback(framesPerCallback);
            }

            instance.setErrorCallback(mErrorCallback);

            target = JNITools::stringCheckNConvert(env, packageName);
            instance.setPackageName(target);

            target = JNITools::stringCheckNConvert(env, attributionTag);

            if (!target.empty()) {
                instance.setAttributionTag(target);
            }

        }

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

        if (instanceTasks.size() == 2 && current == 1) {
            instance.setBufferCapacityInFrames(
                    BasicOboeDuplexEngine::outputStream->getBufferCapacityInFrames() * channels);
        }

        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Opening stream");
        if (instanceTasks.size() == 1) {
            instance.setDataCallback(mDataCallback);
            oboe::Result result = instance.openStream(BasicOboeEngine::stream);

            if (result != oboe::Result::OK || BasicOboeEngine::stream == nullptr) {
                JNITools::throwJavaException(env, "java/lang/Exception",
                                             "Failed to open Oboe audio stream");
                return;
            }

        } else {
            if (current == 0) {
                instance.setDataCallback(mDuplexDataCallback);
                oboe::Result result = instance.openStream(BasicOboeDuplexEngine::outputStream);

                if (result != oboe::Result::OK || BasicOboeDuplexEngine::outputStream == nullptr) {
                    JNITools::throwJavaException(env, "java/lang/Exception",
                                                 "Failed to open Oboe audio stream");
                    return;
                }

            } else {
                oboe::Result result = instance.openStream(BasicOboeDuplexEngine::inputStream);

                if (result != oboe::Result::OK || BasicOboeDuplexEngine::inputStream == nullptr) {
                    JNITools::throwJavaException(env, "java/lang/Exception",
                                                 "Failed to open Oboe audio stream");
                    return;
                }
            }
        }

        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Success opening stream");
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Creating Fifo buffer");

        if (current == 0) {
            if (instanceTasks.size() == 2) {
                oboe::AudioFormat format = BasicOboeDuplexEngine::outputStream->getFormat();
                if(format == oboe::AudioFormat::Float) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeDuplexEngine::outputStream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I32) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeDuplexEngine::outputStream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I16) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int16_t),
                            BasicOboeDuplexEngine::outputStream->getFramesPerBurst() * channels);
                } else {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(uint8_t),
                            BasicOboeDuplexEngine::outputStream->getFramesPerBurst() * channels);
                }
            } else {
                oboe::AudioFormat format = BasicOboeEngine::stream->getFormat();
                if(format == oboe::AudioFormat::Float) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I32) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I16) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int16_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(uint8_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                }
            }
        } else if (current == 1) {
            if (instanceTasks.size() == 2) {
                oboe::AudioFormat format = BasicOboeDuplexEngine::inputStream->getFormat();
                if(format == oboe::AudioFormat::Float) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeDuplexEngine::inputStream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I32) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeDuplexEngine::inputStream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I16) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int16_t),
                            BasicOboeDuplexEngine::inputStream->getFramesPerBurst() * channels);
                } else {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(uint8_t),
                            BasicOboeDuplexEngine::inputStream->getFramesPerBurst() * channels);
                }
            } else {
                oboe::AudioFormat format = BasicOboeEngine::stream->getFormat();
                if(format == oboe::AudioFormat::Float) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I32) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if(format == oboe::AudioFormat::I16) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int16_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(uint8_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                }
            }
        }

    }

    __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Starting stream");

    if (instanceTasks.size() == 1) {
        oboe::Result result = BasicOboeEngine::stream->requestStart();
        if (result != oboe::Result::OK || BasicOboeEngine::stream == nullptr) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Failed to start Oboe audio stream");
            return;
        }

        duplex = false;

    } else if (instanceTasks.size() == 2) {

        mDuplexDataCallback->setSharedInputStream(BasicOboeDuplexEngine::inputStream); // Mandatory
        mDuplexDataCallback->setOutputStream(BasicOboeDuplexEngine::outputStream.get());
        oboe::Result result = mDuplexDataCallback->start();

        if (result != oboe::Result::OK || BasicOboeEngine::stream == nullptr) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Failed to start Oboe audio stream");
            return;
        }

        duplex = true;

    }
    __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Success starting stream");

}

JNIEXPORT void JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_deviceTerminate(JNIEnv *env, jobject thiz) {
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

JNIEXPORT jobject JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_readInput(JNIEnv *env, jobject thiz) {
    int32_t channelCount;
    int32_t totalSamples;
    int32_t framesPerBurst;
    if (!duplex) {
        if(BasicOboeEngine::stream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jobject(nullptr);
        }
        channelCount = BasicOboeEngine::stream->getChannelCount();
        framesPerBurst = BasicOboeEngine::stream->getFramesPerBurst();
    } else {
        if(BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jobject(nullptr);
        }
        channelCount = BasicOboeDuplexEngine::inputStream->getChannelCount();
        framesPerBurst = BasicOboeDuplexEngine::inputStream->getFramesPerBurst();
    }
    totalSamples = framesPerBurst * channelCount;

    oboe::AudioFormat format;

    if(!duplex){
        format = BasicOboeEngine::stream->getFormat();
    } else {
        format = BasicOboeDuplexEngine::inputStream->getFormat();
    }

    if(format == oboe::AudioFormat::Float) {
        std::vector<float*> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(), totalSamples * sizeof(float));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    } else if(format == oboe::AudioFormat::I32) {
        std::vector<int32_t*> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(), totalSamples * sizeof(int32_t));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    } else if(format == oboe::AudioFormat::I16) {
        std::vector<int16_t*> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(), totalSamples * sizeof(int16_t));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    } else {
        std::vector<uint8_t*> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(), totalSamples * sizeof(uint8_t));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    }
}

JNIEXPORT void JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_writeOutput(JNIEnv *env, jobject thiz, jobject output) {
    int32_t channelCount;
    int32_t totalSamples;
    int32_t framesPerBurst;
    if (!duplex) {
        if(BasicOboeEngine::stream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return;
        }
        channelCount = BasicOboeEngine::stream->getChannelCount();
        framesPerBurst = BasicOboeEngine::stream->getFramesPerBurst();
    } else {
        if(BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return;
        }
        channelCount = BasicOboeDuplexEngine::outputStream->getChannelCount();
        framesPerBurst = BasicOboeDuplexEngine::outputStream->getFramesPerBurst();
    }
    totalSamples = framesPerBurst * channelCount;

    oboe::AudioFormat format;

    if(!duplex){
        format = BasicOboeEngine::stream->getFormat();
    } else {
        format = BasicOboeDuplexEngine::inputStream->getFormat();
    }

    if(format == oboe::AudioFormat::Float){
        Fifo::outputFifo->write(static_cast<float *>(env->GetDirectBufferAddress(output)), framesPerBurst);
    } else if(format == oboe::AudioFormat::I32){
        Fifo::outputFifo->write(static_cast<int32_t *>(env->GetDirectBufferAddress(output)), framesPerBurst);
    } else if(format == oboe::AudioFormat::I16){
        Fifo::outputFifo->write(static_cast<int16_t *>(env->GetDirectBufferAddress(output)), framesPerBurst);
    } else {
        Fifo::outputFifo->write(static_cast<uint8_t *>(env->GetDirectBufferAddress(output)), framesPerBurst);
    }

}

JNIEXPORT jintArray JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_getSampleRate(JNIEnv *env, jobject thiz) {
    jint *tempBuffer;
    if (!duplex) {
        if(BasicOboeEngine::stream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        tempBuffer[0] = BasicOboeEngine::stream->getSampleRate();
    } else {
        if(BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed || BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed ){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        tempBuffer[0] = BasicOboeDuplexEngine::inputStream->getSampleRate();
        tempBuffer[1] = BasicOboeDuplexEngine::outputStream->getSampleRate();
    }

    return reinterpret_cast<jintArray>(tempBuffer);

}

JNIEXPORT jintArray JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_getChannelCount(JNIEnv *env, jobject thiz) {
    jint *tempBuffer;
    if (!duplex) {
        if(BasicOboeEngine::stream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        tempBuffer[0] = BasicOboeEngine::stream->getChannelCount();
    } else {
        if(BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed || BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed ){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }

        tempBuffer[0] = BasicOboeDuplexEngine::inputStream->getChannelCount();
        tempBuffer[1] = BasicOboeDuplexEngine::outputStream->getChannelCount();
    }

    return reinterpret_cast<jintArray>(tempBuffer);

}

JNIEXPORT jintArray JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_getDeviceId(JNIEnv *env, jobject thiz) {
    jint *id;
    if (!duplex) {
        if(BasicOboeEngine::stream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        id[0] = BasicOboeEngine::stream->getDeviceId();
    } else {
        if(BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed || BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed ){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        id[0] = BasicOboeDuplexEngine::inputStream->getDeviceId();
        id[1] = BasicOboeDuplexEngine::outputStream->getDeviceId();
    }

    return reinterpret_cast<jintArray>(id);

}


JNIEXPORT jobjectArray JNICALL
Java_org_tpkarras_basicoboe_bridge_Stream_getFormat(JNIEnv *env, jobject thiz) {
    std::vector<oboe::AudioFormat> formats;
    if (!duplex) {
        if(BasicOboeEngine::stream->getState() == oboe::StreamState::Closed){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jobjectArray(nullptr);
        }
        formats[0] = BasicOboeEngine::stream->getFormat();
    } else {
        if(BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed || BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed ){
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jobjectArray(nullptr);
        }
        formats[0] = BasicOboeDuplexEngine::inputStream->getFormat();
        formats[1] = BasicOboeDuplexEngine::outputStream->getFormat();
    }

    jobjectArray stringArray = env->NewObjectArray(formats.size(), env->FindClass("java/lang/String"), nullptr);
    if (stringArray == nullptr) {
        JNITools::throwJavaException(env, "java/lang/Exception",
                                     "Out of memory");
        return nullptr;
    }

    for(int i = 0; i < formats.size(); i++) {

        if (formats[i] == oboe::AudioFormat::Float) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "Float");
        } else if (formats[i] == oboe::AudioFormat::I16) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "I16");
        } else if (formats[i] == oboe::AudioFormat::I24) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "I24");
        } else if (formats[i] == oboe::AudioFormat::I32) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "I32");
        } else if (formats[i] == oboe::AudioFormat::AAC_ELD) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "AAC_ELD");
        } else if (formats[i] == oboe::AudioFormat::AAC_HE_V1) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "AAC_HE_V1");
        } else if (formats[i] == oboe::AudioFormat::AAC_HE_V2) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "AAC_HE_V2");
        } else if (formats[i] == oboe::AudioFormat::AAC_LC) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "AAC_LC");
        } else if (formats[i] == oboe::AudioFormat::IEC61937) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "IEC61937");
        } else if (formats[i] == oboe::AudioFormat::AAC_XHE) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "AAC_XHE");
        } else if (formats[i] == oboe::AudioFormat::OPUS) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "OPUS");
        } else if (formats[i] == oboe::AudioFormat::MP3) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "MP3");
        } else if (formats[i] == oboe::AudioFormat::Invalid) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "Invalid");
        } else if (formats[i] == oboe::AudioFormat::Unspecified) {
            env->SetObjectArrayElement(stringArray, i, (jstring) "Unspecified");
        }

    }

    return stringArray;


}

}
