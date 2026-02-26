#include "DSP/MT2GainStage.h"

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

    mStage2.setGain(4.0);  // Reduced from 8.0 to 4.0

    mInterstageHPF.setCutoffFrequency(200.0, sampleRate);
    mInterstageLPF.setCutoffFrequency(3500.0, sampleRate);  // Reduced from 5500Hz

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

double MT2GainStage::processSample(double input) {
    double s1 = mStage1.processSample(input);

    double hpfOut = mInterstageHPF.processSample(s1);
    double lpfOut = mInterstageLPF.processSample(hpfOut);

    double s2 = mStage2.processSample(lpfOut);

    return s2;
}
