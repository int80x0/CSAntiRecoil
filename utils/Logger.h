#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>

/**
 * Simple logging utility
 */
class Logger {
public:
    static bool initialize(const std::string& logFilePath, bool consoleOutput = true);

    static void cleanup();

    static void log(const std::string& message);

    static void error(const std::string& message);

    static void warning(const std::string& message);

    static void debug(const std::string& message);

private:
    static std::ofstream s_logFile;
    static bool s_consoleOutput;
    static std::mutex s_logMutex;

    static std::string getTimestamp();
};