#pragma once
#include <iostream>
#include <mutex>
#include <Spritter/Spritter.h>




class logger
{
    /*
                 singleton logger object
        there should only ever be a single logger
                     per application
    */
private:
    logger(std::string textColour,std::string backgroundColour)
    {
        this->textColour = getAnsiTextColourCode(textColour);
        this->backgroundColour = getAnsiBackgroundColourCode(backgroundColour);
    }
    static logger* logger_;
    static std::mutex mutex_;
    std::string textColour;
    std::string backgroundColour;
public:
    //stops cloning
    logger(const logger&) = delete;
    //stops assignment
    logger& operator =(const logger&) = delete;

    static logger* getInstance(std::string textColour, std::string backgroundColour);       //defined outside of class to prevent two instance threads accessing it at the same time


    std::string getAnsiTextColourCode(const std::string& colour) {
        static const std::unordered_map<std::string, std::string> colorMap = {
            {"black",   "30"},
            {"red",     "31"},
            {"green",   "32"},
            {"yellow",  "33"},
            {"blue",    "34"},
            {"magenta", "35"},
            {"cyan",    "36"},
            {"white",   "37"},
            {"reset",   "0"}
        };

        auto it = colorMap.find(colour);
        if (it != colorMap.end()) {
            return it->second;
        }
        else {
            return "0";
        }
    }

    std::string getAnsiBackgroundColourCode(const std::string& colour) {
        static const std::unordered_map<std::string, std::string> colorMap = {
            { "black",  "40m" },
            { "red",    "41m"},
            { "green",  "42m"},
            { "yellow", "43m"},
            { "blue",   "44m"},
            { "magenta","45m"},
            { "cyan",   "46m"},
            { "white",  "47m"},
            { "reset",  "0m"}
        };

        auto it = colorMap.find(colour);
        if (it != colorMap.end()) {
            return it->second;
        }
        else {
            return "0";
        }
    }

    void msg(std::string msg)
    {
        std::cout << "\33[" + textColour + ";" << backgroundColour  << msg << std::endl;
    }

    void msgColour(std::string textcolour, std::string backgroundColour, std::string msg)
    {
        std::cout << "\33[" + getAnsiTextColourCode(textcolour) + ";" << getAnsiBackgroundColourCode(backgroundColour) << msg << "\033[0m" << std::endl;
    }

    void errorMsg(std::string msg)
    {
        std::cout << "\33[37;41m" << msg << "\33[" + textColour + ";" << backgroundColour  << std::endl;
    }

#ifdef _DEBUG
    void debugMsg(std::string msg)
    {
        std::cout << "\033[0m \n" << msg << "\33[" + textColour + ";" << backgroundColour << std::endl;
    }
#else
    void debugMsg(std::string msg)
    {
        return;
    }
#endif
};

