//
// Created by taylo on 11/9/2026.
//

#include "stream-tools.h"
#include <oboe/Oboe.h>

std::string StreamTools::formatToString(oboe::AudioFormat format) {
    if (format == oboe::AudioFormat::I16) {
        return "I16";
    } else if (format == oboe::AudioFormat::Float) {
        return "Float";
    } else if (format == oboe::AudioFormat::I24) {
        return "I24";
    } else if (format == oboe::AudioFormat::I32) {
        return "I32";
    } else if (format == oboe::AudioFormat::IEC61937) {
        return "IEC61937";
    } else if (format == oboe::AudioFormat::MP3) {
        return "MP3";
    } else if (format == oboe::AudioFormat::AAC_LC) {
        return "AAC_LC";
    } else if (format == oboe::AudioFormat::AAC_HE_V1) {
        return "AAC_HE_V1";
    } else if (format == oboe::AudioFormat::AAC_HE_V2) {
        return "AAC_HE_V2";
    } else if (format == oboe::AudioFormat::AAC_ELD) {
        return "AAC_ELD";
    } else if (format == oboe::AudioFormat::AAC_XHE) {
        return "AAC_XHE";
    } else if (format == oboe::AudioFormat::OPUS) {
        return "OPUS";
    }
}

std::string StreamTools::usageToString(oboe::Usage usage) {
    if (usage == oboe::Usage::Media) {
        return "Media";
    } else if (usage == oboe::Usage::VoiceCommunication) {
        return "VoiceCommunication";
    } else if (usage == oboe::Usage::Notification) {
        return "Notification";
    } else if (usage == oboe::Usage::NotificationRingtone) {
        return "NotificationRingtone";
    } else if (usage == oboe::Usage::NotificationEvent) {
        return "NotificationEvent";
    } else if (usage == oboe::Usage::AssistanceAccessibility) {
        return "AssistanceAccessibility";
    } else if (usage == oboe::Usage::AssistanceNavigationGuidance) {
        return "AssistanceNavigationGuidance";
    } else if (usage == oboe::Usage::AssistanceSonification) {
        return "AssistanceSonification";
    } else if (usage == oboe::Usage::Game) {
        return "Game";
    } else if (usage == oboe::Usage::VoiceCommunicationSignalling) {
        return "VoiceCommunicationSignalling";
    } else if (usage == oboe::Usage::Assistant) {
        return "Assistant";
    }
}

    std::string StreamTools::audioAPIToString(oboe::AudioApi api) {
        if (api == oboe::AudioApi::AAudio) {
            return "AAudio";
        } else if (api == oboe::AudioApi::OpenSLES) {
            return "OpenSLES";
        } else if (api == oboe::AudioApi::Unspecified) {
            return "Unspecified";
        }
    }


        std::string StreamTools::performanceModeToString(oboe::PerformanceMode performanceMode){
            if (performanceMode == oboe::PerformanceMode::LowLatency) {
                return "LowLatency";
            } else if (performanceMode == oboe::PerformanceMode::PowerSaving) {
                return "PowerSaving";
            } else if (performanceMode == oboe::PerformanceMode::PowerSavingOffloaded) {
                return "PowerSaving_Offloaded";
            } else {
                return "Undefined";
            }
}
