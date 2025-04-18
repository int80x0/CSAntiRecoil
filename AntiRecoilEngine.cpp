#include "AntiRecoilEngine.h"
#include "PatternLoader.h"
#include <thread>
#include <chrono>
#include <cmath>
#include <utility>
#include "utils/Logger.h"

AntiRecoilEngine::AntiRecoilEngine()
        : m_isActive(false)
        , m_returnToOriginal(true)
        , m_sensitivityScale(1.0f)
        , m_resolutionScaleX(1.0f)
        , m_resolutionScaleY(1.0f)
        , m_aspectRatioScale(1.0f)
        , m_currentPointIndex(0)
        , m_stopFlag(false)
        , m_backx(0)
        , m_backy(0)
{
    m_patternLoader = std::make_shared<PatternLoader>("data/patterns");
    m_originalPosition = {0, 0};
}

AntiRecoilEngine::~AntiRecoilEngine() {
    stopAntiRecoil();
}

bool AntiRecoilEngine::loadPattern(const std::string& patternName) {
    Logger::log("AntiRecoilEngine: Loading pattern: " + patternName);

    std::vector<PatternPoint> pattern = m_patternLoader->getPattern(patternName);

    if (pattern.empty()) {
        Logger::error("Failed to load pattern: " + patternName);
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentPattern = scalePattern(pattern);

    Logger::log("Loaded pattern: " + patternName + " with " + std::to_string(pattern.size()) + " points");

    m_currentPointIndex = 0;
    m_backx = 0;
    m_backy = 0;

    return true;
}

void AntiRecoilEngine::startAntiRecoil() {
    if (m_isActive) {
        return;
    }

    GetCursorPos(&m_originalPosition);

    m_currentPointIndex = 0;
    m_backx = 0;
    m_backy = 0;
    m_isActive = true;
    m_stopFlag = false;

    m_antiRecoilThread = std::thread(&AntiRecoilEngine::applyPattern, this);
    Logger::log("Anti-recoil started");
}

void AntiRecoilEngine::stopAntiRecoil() {
    if (!m_isActive) {
        return;
    }

    m_stopFlag = true;
    if (m_antiRecoilThread.joinable()) {
        m_antiRecoilThread.join();
    }

    m_isActive = false;

    if (m_returnToOriginal) {
        returnToOriginalPosition();
    }

    Logger::log("Anti-recoil stopped");
}

void AntiRecoilEngine::setReturnToOriginal(bool returnToOriginal) {
    m_returnToOriginal = returnToOriginal;
}

void AntiRecoilEngine::setSensitivity(float sensitivity) {
    m_sensitivityScale = 2.0f / sensitivity;
    Logger::log("Sensitivity set to: " + std::to_string(sensitivity) +
                " (scale: " + std::to_string(m_sensitivityScale) + ")");
}

void AntiRecoilEngine::setResolution(int width, int height) {
    m_resolutionScaleX = static_cast<float>(width) / 1920.0f;
    m_resolutionScaleY = static_cast<float>(height) / 1080.0f;
    Logger::log("Resolution set to: " + std::to_string(width) + "x" + std::to_string(height) +
                " (scale: " + std::to_string(m_resolutionScaleX) + "x" + std::to_string(m_resolutionScaleY) + ")");
}

void AntiRecoilEngine::setAspectRatio(float aspectRatio) {
    // Base aspect ratio in the Lua script is 16:9
    m_aspectRatioScale = aspectRatio / (16.0f / 9.0f);
    Logger::log("Aspect ratio set to: " + std::to_string(aspectRatio) +
                " (scale: " + std::to_string(m_aspectRatioScale) + ")");
}

bool AntiRecoilEngine::isActive() const {
    return m_isActive;
}

void AntiRecoilEngine::applyPattern() {
    std::vector<PatternPoint> pattern;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pattern = m_currentPattern;
    }

    if (pattern.empty()) {
        m_isActive = false;
        Logger::error("Cannot apply empty pattern");
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    while (m_currentPointIndex < pattern.size() && !m_stopFlag) {
        const auto& point = pattern[m_currentPointIndex];

        moveMouseRelative(point.x, point.y);

        m_backx -= point.x;
        m_backy -= point.y;

        auto targetTime = startTime + std::chrono::milliseconds(point.d);

        auto now = std::chrono::high_resolution_clock::now();
        if (now < targetTime) {
            std::this_thread::sleep_until(targetTime);
        }

        m_currentPointIndex++;
    }

    m_isActive = false;
    Logger::log("Pattern application complete. Processed " + std::to_string(m_currentPointIndex) + " points");
}

std::vector<PatternPoint> AntiRecoilEngine::scalePattern(const std::vector<PatternPoint>& pattern) {
    std::vector<PatternPoint> scaledPattern;
    scaledPattern.reserve(pattern.size());

    for (const auto& point : pattern) {
        PatternPoint scaledPoint;
        scaledPoint.x = static_cast<int>(point.x * m_sensitivityScale * m_resolutionScaleX * m_aspectRatioScale + 0.5f);
        scaledPoint.y = static_cast<int>(point.y * m_sensitivityScale * m_resolutionScaleY * m_aspectRatioScale + 0.5f);
        scaledPoint.d = point.d;

        scaledPattern.push_back(scaledPoint);
    }

    return scaledPattern;
}

void AntiRecoilEngine::moveMouseRelative(int x, int y) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = x;
    input.mi.dy = y;

    SendInput(1, &input, sizeof(INPUT));
}

void AntiRecoilEngine::returnToOriginalPosition() {
    int backx = m_backx;
    int backy = m_backy;

    int round = 4;
    int speed = 4;

    int backx2 = backx / 40;
    int backy2 = backy / 40;
    float backx1 = 0.0f, backy1 = 0.0f;
    float tsleep3 = 0.0f;

    Logger::log("Returning to original position, backx=" + std::to_string(backx) +
                ", backy=" + std::to_string(backy));

    for (int i = 0; i < round; ++i) {
        float tsleep = 2.0f * std::sqrt(std::abs(backx) * std::abs(backx) +
                                        std::abs(backy) * std::abs(backy)) / round / speed;
        int tsleep2 = static_cast<int>(tsleep);
        tsleep3 += tsleep - tsleep2;

        if (tsleep3 >= 1.0f) {
            tsleep3 -= 1.0f;
            tsleep2 += 1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(tsleep2));

        backx1 += backx / 40.0f - backx2;
        backy1 += backy / 40.0f - backy2;

        int backx3, backy3;

        if (backx1 >= 1.0f) {
            backx1 -= 1.0f;
            backx3 = backx2 + 1;
        } else {
            backx3 = backx2;
        }

        if (backy1 >= 1.0f) {
            backy1 -= 1.0f;
            backy3 = backy2 + 1;
        } else {
            backy3 = backy2;
        }

        moveMouseRelative(backx3, backy3);
    }

    m_backx = 0;
    m_backy = 0;
}

AntiRecoilEngine::AntiRecoilEngine(std::shared_ptr<PatternLoader> patternLoader)
        : m_patternLoader(std::move(patternLoader))
        , m_isActive(false)
        , m_returnToOriginal(true)
        , m_sensitivityScale(1.0f)
        , m_resolutionScaleX(1.0f)
        , m_resolutionScaleY(1.0f)
        , m_aspectRatioScale(1.0f)
        , m_currentPointIndex(0)
        , m_stopFlag(false)
        , m_backx(0)
        , m_backy(0)
{
    m_originalPosition = {0, 0};
}
