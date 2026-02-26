#pragma once
#include "DiodeFeedbackClipper.h"
#include "OnePoleFilter.h"
#include <cmath>

class MT2GainStage {
public:
    MT2GainStage();

    void prepare(double sampleRate);
    void reset();

    void setGain(double gain);
    void setStage1Diode(double is, double n, bool noClip);
    void setStage2Diode(double is, double n, bool noClip);
    void setClipMode(int mode);

    double processSample(double input);

    static double applyClip(double x, int mode);

private:
    DiodeFeedbackClipper mStage1;
    DiodeFeedbackClipper mStage2;
    OnePoleFilter mInterstageHPF;
    OnePoleFilter mInterstageLPF;

    int mClipMode = 0;
};
