#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <Spritter/Spritter.h>


using namespace Spritter;
using namespace Spritter::Graphics;
using namespace Spritter::Math;

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
        this->txtColour = getAnsiTextColourCode(textColour);
        this->bgColour = getAnsiBackgroundColourCode(backgroundColour);
    }
    static logger* logger_;
    static std::mutex mutex_;
    std::string txtColour;
    std::string bgColour;
    std::deque<std::string> msgList;
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
#ifndef SPRITTER

    void msg(std::string msg)
    {
        std::cout << "\33[" + txtColour + ";" + bgColour  << msg << std::endl;
    }

    void msgColour(std::string textColour, std::string backgroundColour, std::string msg)
    {
        std::cout << "\33[" + getAnsiTextColourCode(textColour) + ";" + getAnsiBackgroundColourCode(backgroundColour) << msg << "\33[" + txtColour + ";" + bgColour << std::endl;
    }

    void errorMsg(std::string msg)
    {
        std::cout << "\33[37;41m" << msg << "\33[" + txtColour + ";" + bgColour  << std::endl;
    }

#ifdef _DEBUG
    void debugMsg(std::string msg)
    {
        std::cout << "\033[0m \n" << msg << "\33[" + txtColour + ";" + bgColour << std::endl;
    }
#else
    void debugMsg(std::string msg)
    {
        return;
    }
#endif

#else
    void msg(std::string msg)
    {
        msgList.push_back(msg);
    }
    void msgColour(std::string textColour, std::string backgroundColour, std::string message)
    {
        msg(message);
    }
    void errorMsg(std::string message)
    {
        msg(message);
    }
    
#ifdef _DEBUG
    void debugMsg(std::string message)
    {
        msg(message);
    }
#else
    void debugMsg(std::string msg)
    {
        return;
    }
#endif

    std::string msgListCombine()
    {
        std::string msgs = "";
        for (int i = 0;i< msgList.size();i++)
        {
            msgs += msgList[i];
        }
        return msgs;
    }
    void draw(SpriteRenderer & renderer, Font & _font)
    {
        int fontsize = 10;
        int count = 1;
        for (std::string msg : msgList)
        {
            _font.Draw(renderer, { 50, fontsize * count }, msg, fontsize, Color::Red());
        }
        
    }
#endif
};

