#include "DSP/OnePoleFilter.h"
#include <cmath>

OnePoleFilter::OnePoleFilter(Type type) : mType(type) {}

void OnePoleFilter::setType(Type type) {
    mType = type;
}

void OnePoleFilter::setCutoffFrequency(double freqHz, double sampleRate) {
    // TPT (Topology Preserving Transform) coefficients
    // g = tan(pi * fc / fs)
    double g = std::tan(M_PI * freqHz / sampleRate);
    double G = g / (1.0 + g);

    if (mType == Type::LPF) {
        // LPF: y[n] = G * x[n] + (1 - G) * y[n-1]
        mA0 = G;
        mB1 = 1.0 - G;
    } else {
        // HPF using TPT structure
        // HPF: y[n] = (x[n] - x[n-1]) * G + y[n-1] (simplified TPT HPF)
        // More precise TPT HPF:
        // y[n] = (1 - G) * x[n] - (1 - G) * x[n-1] + (1 - 2*G) * y[n-1]
        // Standard first-order HPF via bilinear transform:
        mA0 = 1.0 - G;
        mB1 = -G;
    }
}

void OnePoleFilter::reset() {
    mZ1 = 0.0;
}

double OnePoleFilter::processSample(double input) {
    if (mType == Type::LPF) {
        // LPF: y[n] = a0 * x[n] + b1 * y[n-1]
        double output = mA0 * input + mB1 * mZ1;
        mZ1 = output;
        return output;
    } else {
        // HPF: y[n] = a0 * (x[n] - x[n-1]) + (1 - a0) * y[n-1]
        // This is the TPT structure for HPF
        double output = mA0 * (input - mZ1) + mB1 * mZ1;
        mZ1 = input;
        return output;
    }
}
