#pragma once
#include <oboe/Oboe.h>

class StreamTools {
public:

    static std::string formatToString(oboe::AudioFormat format);
    static std::string usageToString(oboe::Usage usage);
    static std::string audioAPIToString(oboe::AudioApi api);
    static std::string performanceModeToString(oboe::PerformanceMode performanceMode);
};