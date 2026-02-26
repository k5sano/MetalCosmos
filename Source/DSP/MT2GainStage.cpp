#include "DSP/MT2GainStage.h"
#include <cmath>
#include <algorithm>

MT2GainStage::MT2GainStage()
    : mInterstageHPF(OnePoleFilter::Type::HPF)
    , mInterstageLPF(OnePoleFilter::Type::LPF)
{
}

void MT2GainStage::prepare(double sampleRate) {
    mStage1.setSampleRate(sampleRate);
    mStage2.setSampleRate(sampleRate);

    mStage1.setRf(10000.0);
    mStage2.setRf(4700.0);

    mStage2.setGain(4.0);

    mInterstageHPF.setCutoffFrequency(200.0, sampleRate);
    mInterstageLPF.setCutoffFrequency(3500.0, sampleRate);

    reset();
}

void MT2GainStage::reset() {
    mStage1.reset();
    mStage2.reset();
    mInterstageHPF.reset();
    mInterstageLPF.reset();
}

void MT2GainStage::setGain(double gain) {
    mStage1.setGain(gain);
}

void MT2GainStage::setStage1Diode(double is, double n, bool noClip) {
    mStage1.setDiodeParams(is, n);
    mStage1.setBypass(noClip);
}

void MT2GainStage::setStage2Diode(double is, double n, bool noClip) {
    mStage2.setDiodeParams(is, n);
    mStage2.setBypass(noClip);
}

void MT2GainStage::setClipMode(int mode) {
    mClipMode = std::clamp(mode, 0, 5);
}

double MT2GainStage::applyClip(double x, int mode) {
    switch (mode) {
    case 1: return std::tanh(x);
    case 2: return (2.0 / M_PI) * std::atan(x);
    case 3: {
        double lo = -1.0, hi = 1.0;
        return x < lo ? lo : (x > hi ? hi : x);
    }
    case 4: return x >= 0.0 ? std::tanh(x) : std::tanh(x * 0.5);
    case 5: {
        double y = std::sin(x);
        return y;
    }
    default: return std::tanh(x);
    }
}

double MT2GainStage::processSample(double input) {
    double after1;
    if (mClipMode == 0) {
        after1 = mStage1.processSample(input);
    } else {
        double x = input * mStage1.getGain();
        after1 = applyClip(x, mClipMode);
    }

    double hpfOut = mInterstageHPF.processSample(after1);
    double lpfOut = mInterstageLPF.processSample(hpfOut);

    double after2;
    if (mClipMode == 0) {
        after2 = mStage2.processSample(lpfOut);
    } else {
        double x = lpfOut * 4.0;
        after2 = applyClip(x, mClipMode);
    }
    return after2;
}
