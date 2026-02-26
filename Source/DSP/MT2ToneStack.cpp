#include "DSP/MT2ToneStack.h"
#include <cmath>

void MT2ToneStack::prepare(double sampleRate) {
    mSampleRate = sampleRate;
    reset();
}

void MT2ToneStack::reset() {
    mLowShelf.reset();
    mMidPeak.reset();
    mHighShelf.reset();
}

void MT2ToneStack::updateCoefficients(float eqLow, float eqMid, float eqMidFreq,
                                      float eqMidQ, float eqHigh) {
    // Low Shelf: 200Hz, ±15dB, Q=0.707
    double lowGain = (eqLow - 0.5f) * 30.0;  // -15dB to +15dB
    mLowShelf.setLowShelf(200.0, lowGain, 0.707, mSampleRate);

    // Mid Peak: 200Hz~5000Hz (log sweep), ±20dB, Q=0.3~10.0 (log mapping)
    double midGain = (eqMid - 0.5f) * 40.0;  // -20dB to +20dB
    double midFreq = 200.0 * std::pow(5000.0 / 200.0, eqMidFreq);  // 200Hz to 5000Hz
    double midQ = 0.3 * std::pow(10.0 / 0.3, eqMidQ);  // 0.3 to 10.0
    mMidPeak.setPeak(midFreq, midGain, midQ, mSampleRate);

    // High Shelf: 5000Hz, ±15dB, Q=0.707
    double highGain = (eqHigh - 0.5f) * 30.0;  // -15dB to +15dB
    mHighShelf.setHighShelf(5000.0, highGain, 0.707, mSampleRate);
}

double MT2ToneStack::processSample(double input) {
    // Process: Low Shelf → Mid Peak → High Shelf
    double lsOut = mLowShelf.processSample(input);
    double midOut = mMidPeak.processSample(lsOut);
    double hsOut = mHighShelf.processSample(midOut);
    return hsOut;
}
