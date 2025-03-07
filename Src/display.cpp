#include "../Headers/display.h"

std::mutex logger::mutex_; // Define the static member variable

logger* logger::logger_{ nullptr };

logger* logger::getInstance(std::string textColour, std::string backgroundColour)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (logger_ == nullptr)
    {
        logger_ = new logger(textColour, backgroundColour);
    }
    return logger_;
}