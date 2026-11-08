#pragma once
#include <oboe/Oboe.h>
#include <iostream>
#include <memory>

class Fifo {

public:
    static std::unique_ptr<oboe::FifoBuffer> inputFifo;
    static std::unique_ptr<oboe::FifoBuffer> outputFifo;
};