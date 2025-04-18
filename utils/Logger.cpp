#include "Logger.h"
#include <filesystem>

namespace fs = std::filesystem;

std::ofstream Logger::s_logFile;
bool Logger::s_consoleOutput = true;
std::mutex Logger::s_logMutex;

bool Logger::initialize(const std::string& logFilePath, bool consoleOutput) {
    s_consoleOutput = consoleOutput;

    // Create directory if it doesn't exist
    fs::path parentPath = fs::path(logFilePath).parent_path();
    if (!fs::exists(parentPath)) {
        fs::create_directories(parentPath);
    }

    s_logFile.open(logFilePath, std::ios::out | std::ios::app);
    if (!s_logFile.is_open()) {
        if (s_consoleOutput) {
            std::cerr << "Failed to open log file: " << logFilePath << std::endl;
        }
        return false;
    }

    log("Logger initialized");
    return true;
}

void Logger::cleanup() {
    if (s_logFile.is_open()) {
        log("Logger shutting down");
        s_logFile.close();
    }
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(s_logMutex);

    std::string timestamp = getTimestamp();
    std::string logMessage = timestamp + " [INFO] " + message;

    if (s_logFile.is_open()) {
        s_logFile << logMessage << std::endl;
    }

    if (s_consoleOutput) {
        std::cout << logMessage << std::endl;
    }
}

void Logger::error(const std::string& message) {
    std::lock_guard<std::mutex> lock(s_logMutex);

    std::string timestamp = getTimestamp();
    std::string logMessage = timestamp + " [ERROR] " + message;

    if (s_logFile.is_open()) {
        s_logFile << logMessage << std::endl;
    }

    if (s_consoleOutput) {
        std::cerr << logMessage << std::endl;
    }
}

void Logger::warning(const std::string& message) {
    std::lock_guard<std::mutex> lock(s_logMutex);

    std::string timestamp = getTimestamp();
    std::string logMessage = timestamp + " [WARNING] " + message;

    if (s_logFile.is_open()) {
        s_logFile << logMessage << std::endl;
    }

    if (s_consoleOutput) {
        std::cout << logMessage << std::endl;
    }
}

void Logger::debug(const std::string& message) {
#ifdef _DEBUG
    std::lock_guard<std::mutex> lock(s_logMutex);
    
    std::string timestamp = getTimestamp();
    std::string logMessage = timestamp + " [DEBUG] " + message;
    
    if (s_logFile.is_open()) {
        s_logFile << logMessage << std::endl;
    }
    
    if (s_consoleOutput) {
        std::cout << logMessage << std::endl;
    }
#endif
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}