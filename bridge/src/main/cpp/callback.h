#pragma once

#include <oboe/Oboe.h>
#include <android/log.h>
#include "fifo.h"

class BasicOboeEngine : public oboe::AudioStreamDataCallback {
    public:
    static std::shared_ptr<oboe::AudioStream> stream;
        oboe::DataCallbackResult onAudioReady(
                oboe::AudioStream *audioStream,
                void *audioData,
                int32_t numFrames
        ) override {
            auto *floatData = static_cast<float*>(audioData);

            if(audioStream->getDirection() == oboe::Direction::Input){
                Fifo::inputFifo->write(floatData, numFrames);
            } else {
                int32_t framesRead = Fifo::outputFifo->read(floatData, numFrames);

                if (framesRead < numFrames) {
                    std::fill_n(floatData + (framesRead * audioStream->getChannelCount()),
                                (numFrames - framesRead) * audioStream->getChannelCount(), 0.0f);
                }
            }

            return oboe::DataCallbackResult::Continue;
        }
    };

class BasicOboeDuplexEngine : public oboe::FullDuplexStream{
public:
    static std::shared_ptr<oboe::AudioStream> inputStream;
    static std::shared_ptr<oboe::AudioStream> outputStream;
    oboe::DataCallbackResult onBothStreamsReady(
            const void *inputData,
            int numInputFrames,
            void *outputData,
            int numOutputFrames
    ) override {

        // Example: Pass-through (copy input to output)
        // Note: Ensure your data formats (e.g., Float vs Int16) match what you built.
        const float *audioIn = static_cast<const float*>(inputData);
        float *audioOut = static_cast<float*>(outputData);

            Fifo::inputFifo->write(audioIn, numInputFrames);
            int32_t framesRead = Fifo::outputFifo->write(audioOut, numOutputFrames);

            if (framesRead < numOutputFrames) {
                std::fill_n(audioOut + (framesRead * outputStream->getChannelCount()),
                            (numOutputFrames - framesRead) * outputStream->getChannelCount(), 0.0f);
            }

        return oboe::DataCallbackResult::Continue; // Keep the stream running
    }
};

class BasicOboeErrorCallback : public oboe::AudioStreamErrorCallback {
public:
    bool onError(oboe::AudioStream *oboeStream, oboe::Result error) override {
        if(error == oboe::Result::ErrorDisconnected) {
            __android_log_print(ANDROID_LOG_DEBUG, "OboeCallback", "Closing stream due to device disconnect");
        } else if(error == oboe::Result::ErrorClosed || error == oboe::Result::ErrorInvalidHandle) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Cannot interact with a closed stream");
        } else if(error == oboe::Result::ErrorBase) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Oboe has encountered an internal error");
        } else if(error == oboe::Result::ErrorIllegalArgument) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Cannot start stream due to illegal arguments");
        } else if(error == oboe::Result::ErrorInvalidFormat) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Cannot start stream due to invalid format");
        } else if(error == oboe::Result::ErrorInvalidRate) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Cannot start stream due to invalid sample rate");
        } else if(error == oboe::Result::ErrorInvalidState) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Attempted to execute illegal methods");
        } else if(error == oboe::Result::ErrorNoMemory) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Out of memory");
        } else if(error == oboe::Result::ErrorNoFreeHandles) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "No free handles available");
        } else if(error == oboe::Result::ErrorNull) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Handle is null");
        } else if(error == oboe::Result::ErrorTimeout) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Stream has timed out");
        } else if(error == oboe::Result::ErrorUnavailable) {
        __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Resource not available");
        } else if(error == oboe::Result::ErrorOutOfRange) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Out of range");
        } else if(error == oboe::Result::ErrorNoService) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Communication with Android audio service failed");
        } else if(error == oboe::Result::ErrorWouldBlock) {
            __android_log_print(ANDROID_LOG_ERROR, "OboeCallback", "Buffer is full");
        }
        return false;
    }

    };
