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
oboe::AudioFormat format = audioStream->getFormat();
int32_t framesRead = 0;

            if (format == oboe::AudioFormat::Float) {
                if (audioStream->getDirection() == oboe::Direction::Input) {
                    Fifo::inputFifo->write(static_cast<float *>(audioData), numFrames);
                } else {
                    framesRead = Fifo::outputFifo->read(static_cast<float *>(audioData), numFrames);
                }
            } else if (format == oboe::AudioFormat::I16) {
                if (audioStream->getDirection() == oboe::Direction::Input) {
                    Fifo::inputFifo->write(static_cast<int16_t *>(audioData), numFrames);
                } else {
                    framesRead = Fifo::outputFifo->read(static_cast<int16_t *>(audioData), numFrames);
                }
            } else if (format == oboe::AudioFormat::I32) {
                if (audioStream->getDirection() == oboe::Direction::Input) {
                    Fifo::inputFifo->write(static_cast<int32_t*>(audioData), numFrames);
                } else {
                    framesRead = Fifo::outputFifo->read(static_cast<int32_t*>(audioData), numFrames);
                }
            } else {
                if (audioStream->getDirection() == oboe::Direction::Input) {
                    Fifo::inputFifo->write(static_cast<uint8_t*>(audioData), numFrames);
                } else {
                    framesRead = Fifo::outputFifo->read(static_cast<uint8_t*>(audioData), numFrames);
                }
            }

                if (framesRead < numFrames) {
                    if (format == oboe::AudioFormat::Float) {
                    std::fill_n(static_cast<float *>(audioData) + (framesRead * audioStream->getChannelCount()),
                                (numFrames - framesRead) * audioStream->getChannelCount(), 0.0f);
                    } else if (format == oboe::AudioFormat::I16) {
                        std::fill_n(static_cast<int16_t*>(audioData) + (framesRead * audioStream->getChannelCount()),
                                    (numFrames - framesRead) * audioStream->getChannelCount(), 0);
                    } else if (format == oboe::AudioFormat::I32) {
                        std::fill_n(static_cast<int32_t*>(audioData) + (framesRead * audioStream->getChannelCount()),
                                    (numFrames - framesRead) * audioStream->getChannelCount(), 0);
                    } else {
                        std::fill_n(static_cast<uint8_t*>(audioData) + (framesRead * audioStream->getChannelCount()),
                                    (numFrames - framesRead) * audioStream->getChannelCount(), 0);
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
        oboe::AudioFormat inputFormat = inputStream->getFormat();
        oboe::AudioFormat outputFormat = outputStream->getFormat();
        int32_t framesRead = 0;
        if (inputFormat == oboe::AudioFormat::Float) {
            Fifo::inputFifo->write(static_cast<const float*>(inputData), numInputFrames);
        } else if (inputFormat == oboe::AudioFormat::I16) {
            Fifo::inputFifo->write(static_cast<const int16_t*>(inputData), numInputFrames);
        } else if (inputFormat == oboe::AudioFormat::I32) {
            Fifo::inputFifo->write(static_cast<const int32_t*>(inputData), numInputFrames);
        } else {
            Fifo::inputFifo->write(static_cast<const uint8_t*>(inputData), numInputFrames);
        }

        if (inputFormat == oboe::AudioFormat::Float) {
            framesRead = Fifo::outputFifo->write(static_cast<float*>(outputData), numOutputFrames);

            if (framesRead < numOutputFrames) {
                std::fill_n(static_cast<float*>(outputData) +
                            (framesRead * outputStream->getChannelCount()),
                            (numOutputFrames - framesRead) * outputStream->getChannelCount(), 0.0f);
            }
        } else if (inputFormat == oboe::AudioFormat::I16) {
            framesRead = Fifo::outputFifo->write(static_cast<int16_t*>(outputData), numOutputFrames);

            if (framesRead < numOutputFrames) {
                std::fill_n(static_cast<int16_t*>(outputData) +
                            (framesRead * outputStream->getChannelCount()),
                            (numOutputFrames - framesRead) * outputStream->getChannelCount(), 0);
            }
        } else if (inputFormat == oboe::AudioFormat::I32) {
            framesRead = Fifo::outputFifo->write(static_cast<int32_t*>(outputData), numOutputFrames);

            if (framesRead < numOutputFrames) {
                std::fill_n(static_cast<int32_t*>(outputData) +
                            (framesRead * outputStream->getChannelCount()),
                            (numOutputFrames - framesRead) * outputStream->getChannelCount(), 0);
            }
        } else {
            framesRead = Fifo::outputFifo->write(static_cast<uint8_t*>(outputData), numOutputFrames);

            if (framesRead < numOutputFrames) {
                std::fill_n(static_cast<uint8_t*>(outputData) +
                            (framesRead * outputStream->getChannelCount()),
                            (numOutputFrames - framesRead) * outputStream->getChannelCount(), 0);
            }
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
