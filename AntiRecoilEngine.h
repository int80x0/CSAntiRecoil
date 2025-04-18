#pragma once

#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>

class PatternLoader;

struct PatternPoint {
    int x;
    int y;
    int d;
};

class AntiRecoilEngine {
public:
    explicit AntiRecoilEngine(std::shared_ptr<PatternLoader> patternLoader);
    ~AntiRecoilEngine();

    bool loadPattern(const std::string& patternName);

    void startAntiRecoil();

    void stopAntiRecoil();

    void setReturnToOriginal(bool returnToOriginal);

    void setSensitivity(float sensitivity);

    void setResolution(int width, int height);

    void setAspectRatio(float aspectRatio);

    [[nodiscard]] bool isActive() const;

private:
    std::vector<PatternPoint> m_currentPattern;
    std::shared_ptr<PatternLoader> m_patternLoader;
    bool m_isActive;
    bool m_returnToOriginal;
    float m_sensitivityScale;
    float m_resolutionScaleX;
    float m_resolutionScaleY;
    float m_aspectRatioScale;

    POINT m_originalPosition{};
    int m_currentPointIndex;
    int m_backx;
    int m_backy;

    std::thread m_antiRecoilThread;

    std::atomic<bool> m_stopFlag;

    std::mutex m_mutex;

    void applyPattern();

    std::vector<PatternPoint> scalePattern(const std::vector<PatternPoint>& pattern);

    void moveMouseRelative(int x, int y);

    void returnToOriginalPosition();
};