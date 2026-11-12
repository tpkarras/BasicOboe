#include <oboe/Oboe.h>
#include <jni.h>
#include "tools/jni-tools.h"
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
Java_org_tpkarras_basicoboe_Stream_deviceInitialize(JNIEnv *env, jobject thiz,
                                                           jstring packageName,
                                                           jstring attributionTag,
                                                           jint deviceIDInput, jint deviceIDOutput,
                                                           jint inputFormat,
                                                           jint outputFormat,
                                                           jint inputSampleRate,
                                                           jint outputSampleRate,
                                                           jint inputChannels, jint outputChannels,
                                                           jint inputChannelMask,
                                                           jint outputChannelMask, jint audioAPI,
                                                           jint performanceMode,
                                                           jboolean inputSharingMode,
                                                           jboolean outputSharingMode,
                                                           jint framesPerCallback,
                                                           jboolean inputIsSpatial,
                                                           jboolean outputIsSpatial,
                                                           jint inputUsage,
                                                           jint outputUsage,
                                                           jint inputContentType,
                                                           jint outputContentType,
                                                           jint sampleRateConversionQuality) {
    std::lock_guard<std::mutex> lock(mStreamMutex);
    oboe::AudioStreamBuilder instance;

    std::vector<int> instanceTasks = {};

    if (deviceIDInput > 0) {
        instanceTasks.push_back(1);
    }

    if (deviceIDOutput > 0) {
        instanceTasks.push_back(0);
    }

    for (int current: instanceTasks) {

        jboolean sharingMode;
        int sampleRate;
        int formatInt;
        int channels;
        int channelMask;
        int usage;
        int contentType;
        char *currentType;
        std::string target;

            if (current == 0) {
                currentType = (char *) "Output";
                __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Setting %s deviceID", currentType);
                instance.setDeviceId(deviceIDOutput);
                instance.setDirection(oboe::Direction::Output);
            } else {
                currentType = (char *) "Input";
                __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Setting %s deviceID", currentType);
                instance.setDeviceId(deviceIDInput);
                instance.setDirection(oboe::Direction::Input);
            }

            if (current == 0) {
                formatInt = outputFormat;
            } else {
                formatInt = inputFormat;
            }

            if (formatInt > 0) {
                __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Format", currentType);
                    instance.setFormat(oboe::AudioFormat(formatInt));
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

        if (instanceTasks.size() == 2 && current == 1 || instanceTasks.size() == 1) {

            if (audioAPI > 0) {
                __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Audio API");
                    instance.setAudioApi((oboe::AudioApi)audioAPI);
            }

            if (performanceMode > 0) {
                __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Performance Mode");
                    instance.setPerformanceMode((oboe::PerformanceMode)performanceMode);
            }


            if (sampleRateConversionQuality > 0) {
                __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting Sample Rate Conversion Quality");
                instance.setSampleRateConversionQuality((oboe::SampleRateConversionQuality)sampleRateConversionQuality);
            }

        }

            if (current == 0) {
                sharingMode = outputSharingMode;
            } else {
                sharingMode = inputSharingMode;
            }

            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Sharing Mode", currentType);
            if (sharingMode == JNI_FALSE) {
                instance.setSharingMode(oboe::SharingMode::Shared);
            } else {
                instance.setSharingMode(oboe::SharingMode::Exclusive);
            }

        if (current == 0 && inputIsSpatial == JNI_TRUE || current == 1 && outputIsSpatial == JNI_TRUE) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Spatialization Flag",
                                currentType);
            instance.setIsContentSpatialized(true);
        }

        if (current == 0) {
            usage = inputUsage;
        } else {
            usage = outputUsage;
        }

        if (usage > 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Usage", currentType);
                instance.setUsage((oboe::Usage)usage);
        }

        if (current == 0) {
            contentType = inputContentType;
        } else {
            contentType = outputContentType;
        }

        if (contentType > 0) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeBuilder", "Setting %s Content Type", currentType);
            instance.setContentType((oboe::ContentType)contentType);
        }

        if (instanceTasks.size() == 2 && current == 1 || instanceTasks.size() == 1) {

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

        if (instanceTasks.size() == 2 && current == 0) {
            instance.setBufferCapacityInFrames(
                    BasicOboeDuplexEngine::inputStream->getBufferCapacityInFrames() * 2);
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
                if (format == oboe::AudioFormat::Float) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeDuplexEngine::outputStream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I32) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeDuplexEngine::outputStream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I16) {
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
                if (format == oboe::AudioFormat::Float) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I32) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I16) {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int16_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else {
                    Fifo::outputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(uint8_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                }
            }
        } else {
            if (instanceTasks.size() == 2) {
                oboe::AudioFormat format = BasicOboeDuplexEngine::inputStream->getFormat();
                if (format == oboe::AudioFormat::Float) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeDuplexEngine::inputStream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I32) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeDuplexEngine::inputStream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I16) {
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
                if (format == oboe::AudioFormat::Float) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(float),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I32) {
                    Fifo::inputFifo = std::make_unique<oboe::FifoBuffer>(
                            channels * sizeof(int32_t),
                            BasicOboeEngine::stream->getFramesPerBurst() * channels);
                } else if (format == oboe::AudioFormat::I16) {
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

    } else if (instanceTasks.size() == 2) {

        mDuplexDataCallback->setSharedInputStream(BasicOboeDuplexEngine::inputStream);
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

    if(!duplex) {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "DeviceID: %d", BasicOboeEngine::stream->getDeviceId());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "AudioAPI: %d", (jint)BasicOboeEngine::stream->getAudioApi());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "PerformanceMode: %d", (jint)BasicOboeEngine::stream->getPerformanceMode());
        if(BasicOboeEngine::stream->getSharingMode() == oboe::SharingMode::Shared){
            __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Sharing Mode: Shared");
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Sharing Mode: Exclusive");
        }
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Format: %d", jint(BasicOboeEngine::stream->getFormat()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Usage: %d", jint(BasicOboeEngine::stream->getUsage()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Content Type: %d", jint(BasicOboeEngine::stream->getContentType()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Channel Mask: %d", BasicOboeEngine::stream->getChannelMask());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Channels: %d", BasicOboeEngine::stream->getChannelCount());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Sample Rate: %d", BasicOboeEngine::stream->getSampleRate());
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "AudioAPI: %d", jint(BasicOboeDuplexEngine::inputStream->getAudioApi()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "PerformanceMode: %d", (jint)BasicOboeDuplexEngine::inputStream->getPerformanceMode());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input DeviceID: %d", BasicOboeDuplexEngine::inputStream->getDeviceId());
        if(BasicOboeDuplexEngine::inputStream->getSharingMode() == oboe::SharingMode::Shared){
            __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Sharing Mode: Shared");
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Sharing Mode: Exclusive");
        }
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Format: %d", jint(BasicOboeDuplexEngine::inputStream->getFormat()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Usage: %d", jint(BasicOboeDuplexEngine::inputStream->getUsage()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Content Type: %d", jint(BasicOboeDuplexEngine::inputStream->getContentType()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Channel Mask: %d", BasicOboeDuplexEngine::inputStream->getChannelMask());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Channels: %d", BasicOboeDuplexEngine::inputStream->getChannelCount());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Input Sample Rate: %d", BasicOboeDuplexEngine::inputStream->getSampleRate());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output DeviceID: %d", BasicOboeDuplexEngine::outputStream->getDeviceId());
        if(BasicOboeDuplexEngine::outputStream->getSharingMode() == oboe::SharingMode::Shared){
            __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Sharing Mode: Shared");
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Sharing Mode: Exclusive");
        }
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Format: %d", jint(BasicOboeDuplexEngine::outputStream->getFormat()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Usage: %d", jint(BasicOboeDuplexEngine::outputStream->getUsage()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Content Type: %d", jint(BasicOboeDuplexEngine::outputStream->getContentType()));
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Channel Mask: %d", BasicOboeDuplexEngine::outputStream->getChannelMask());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Channels: %d", BasicOboeDuplexEngine::outputStream->getChannelCount());
        __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Output Sample Rate: %d", BasicOboeDuplexEngine::outputStream->getSampleRate());
    }
}

JNIEXPORT void JNICALL
Java_org_tpkarras_basicoboe_Stream_deviceTerminate(JNIEnv *env, jobject thiz) {
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
        duplex = false;
    }
    __android_log_print(ANDROID_LOG_DEBUG, "OboeStream", "Successfully closed stream");

}

JNIEXPORT jobject JNICALL
Java_org_tpkarras_basicoboe_Stream_readInput(JNIEnv *env, jobject thiz) {
    int32_t channelCount;
    int32_t totalSamples;
    int32_t framesPerBurst;
    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jobject(nullptr);
        }
        channelCount = BasicOboeEngine::stream->getChannelCount();
        framesPerBurst = BasicOboeEngine::stream->getFramesPerBurst();
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jobject(nullptr);
        }
        channelCount = BasicOboeDuplexEngine::inputStream->getChannelCount();
        framesPerBurst = BasicOboeDuplexEngine::inputStream->getFramesPerBurst();
    }
    totalSamples = framesPerBurst * channelCount;

    oboe::AudioFormat format;

    if (!duplex) {
        format = BasicOboeEngine::stream->getFormat();
    } else {
        format = BasicOboeDuplexEngine::inputStream->getFormat();
    }

    if (format == oboe::AudioFormat::Float) {
        std::vector<float *> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(),
                                                      totalSamples * sizeof(float));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    } else if (format == oboe::AudioFormat::I32) {
        std::vector<int32_t *> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(),
                                                      totalSamples * sizeof(int32_t));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    } else if (format == oboe::AudioFormat::I16) {
        std::vector<int16_t *> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(),
                                                      totalSamples * sizeof(int16_t));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    } else {
        std::vector<uint8_t *> tempBuffer(totalSamples);
        Fifo::inputFifo->read(tempBuffer.data(), framesPerBurst);

        jobject resultJava = env->NewDirectByteBuffer(tempBuffer.data(),
                                                      totalSamples * sizeof(uint8_t));

        if (!resultJava) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Allocation failed");
            return nullptr;
        }

        return resultJava;
    }
}

JNIEXPORT void JNICALL
Java_org_tpkarras_basicoboe_Stream_writeOutput(JNIEnv *env, jobject thiz, jobject output) {
    int32_t channelCount;
    int32_t totalSamples;
    int32_t framesPerBurst;
    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return;
        }
        channelCount = BasicOboeEngine::stream->getChannelCount();
        framesPerBurst = BasicOboeEngine::stream->getFramesPerBurst();
    } else {
        if (BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return;
        }
        channelCount = BasicOboeDuplexEngine::outputStream->getChannelCount();
        framesPerBurst = BasicOboeDuplexEngine::outputStream->getFramesPerBurst();
    }
    totalSamples = framesPerBurst * channelCount;

    oboe::AudioFormat format;

    if (!duplex) {
        format = BasicOboeEngine::stream->getFormat();
    } else {
        format = BasicOboeDuplexEngine::inputStream->getFormat();
    }

    if (format == oboe::AudioFormat::Float) {
        Fifo::outputFifo->write(static_cast<float *>(env->GetDirectBufferAddress(output)),
                                framesPerBurst);
    } else if (format == oboe::AudioFormat::I32) {
        Fifo::outputFifo->write(static_cast<int32_t *>(env->GetDirectBufferAddress(output)),
                                framesPerBurst);
    } else if (format == oboe::AudioFormat::I16) {
        Fifo::outputFifo->write(static_cast<int16_t *>(env->GetDirectBufferAddress(output)),
                                framesPerBurst);
    } else {
        Fifo::outputFifo->write(static_cast<uint8_t *>(env->GetDirectBufferAddress(output)),
                                framesPerBurst);
    }

}

JNIEXPORT jintArray JNICALL
Java_org_tpkarras_basicoboe_Stream_getSampleRate(JNIEnv *env, jobject thiz) {
    jint *tempBuffer = nullptr;
    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        tempBuffer[0] = BasicOboeEngine::stream->getSampleRate();
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed ||
            BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
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
Java_org_tpkarras_basicoboe_Stream_getChannelCount(JNIEnv *env, jobject thiz) {
    jint *tempBuffer = nullptr;
    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        tempBuffer[0] = BasicOboeEngine::stream->getChannelCount();
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed ||
            BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
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
Java_org_tpkarras_basicoboe_Stream_getDeviceId(JNIEnv *env, jobject thiz) {
    jint *id = nullptr;
    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        id[0] = BasicOboeEngine::stream->getDeviceId();
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed ||
            BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        id[0] = BasicOboeDuplexEngine::inputStream->getDeviceId();
        id[1] = BasicOboeDuplexEngine::outputStream->getDeviceId();
    }

    return reinterpret_cast<jintArray>(id);

}


JNIEXPORT jintArray JNICALL
Java_org_tpkarras_basicoboe_Stream_getFormat(JNIEnv *env, jobject thiz) {
    jint *formats = nullptr;
    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        formats[0] = (jint)BasicOboeEngine::stream->getFormat();
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed ||
            BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        formats[0] = (jint)BasicOboeDuplexEngine::inputStream->getFormat();
        formats[1] = (jint)BasicOboeDuplexEngine::outputStream->getFormat();
    }

    return reinterpret_cast<jintArray>(formats);

}

JNIEXPORT jintArray JNICALL
Java_org_tpkarras_basicoboe_Stream_getContentType(JNIEnv *env, jobject thiz) {
    jint *contentType = nullptr;
    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        contentType[0] = (jint)BasicOboeEngine::stream->getContentType();
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed ||
            BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        contentType[0] = (jint)BasicOboeDuplexEngine::inputStream->getContentType();
        contentType[1] = (jint)BasicOboeDuplexEngine::outputStream->getContentType();
    }

    return reinterpret_cast<jintArray>(contentType);

}

JNIEXPORT jintArray JNICALL
Java_org_tpkarras_basicoboe_Stream_getChannelMask(JNIEnv *env, jobject thiz) {
    jint *channelMask = nullptr;

    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        channelMask[0] = static_cast<jint>(BasicOboeEngine::stream->getChannelMask());
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed ||
            BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jintArray(nullptr);
        }
        channelMask[0] = static_cast<jint>(BasicOboeDuplexEngine::inputStream->getChannelMask());
        channelMask[1] = static_cast<jint>(BasicOboeDuplexEngine::outputStream->getChannelMask());
    }

    return reinterpret_cast<jintArray>(channelMask);

}

}
extern "C"
JNIEXPORT jbooleanArray JNICALL
Java_org_tpkarras_basicoboe_Stream_getSharingMode(JNIEnv *env, jobject thiz) {
jboolean *sharingMode = nullptr;
jbooleanArray result;

    if (!duplex) {
        if (BasicOboeEngine::stream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jbooleanArray(nullptr);
        }

        if(BasicOboeEngine::stream->getSharingMode() == oboe::SharingMode::Shared){
            sharingMode[0] = false;
        } else {
            sharingMode[0] = true;
        }
        result = env->NewBooleanArray(1);
        env->SetBooleanArrayRegion(result, 0, 1, sharingMode);
    } else {
        if (BasicOboeDuplexEngine::inputStream->getState() == oboe::StreamState::Closed ||
            BasicOboeDuplexEngine::outputStream->getState() == oboe::StreamState::Closed) {
            JNITools::throwJavaException(env, "java/lang/Exception",
                                         "Stream closed");
            return jbooleanArray(nullptr);
        }
        if(BasicOboeDuplexEngine::inputStream->getSharingMode() == oboe::SharingMode::Shared){
            sharingMode[0] = false;
        } else {
            sharingMode[0] = true;
        }
        if(BasicOboeDuplexEngine::outputStream->getSharingMode() == oboe::SharingMode::Shared){
            sharingMode[1] = false;
        } else {
            sharingMode[1] = true;
        }
        result = env->NewBooleanArray(2);
        env->SetBooleanArrayRegion(result, 0, 2, sharingMode);
    }

    return result;

}