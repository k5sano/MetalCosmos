#include "DSP/BiquadFilter.h"
#include <cmath>

void BiquadFilter::setLowShelf(double freqHz, double gainDb, double q, double sampleRate) {
    // RBJ Audio EQ Cookbook - Low Shelf
    double A = std::pow(10.0, gainDb / 40.0);
    double omega = 2.0 * M_PI * freqHz / sampleRate;
    double sinW = std::sin(omega);
    double cosW = std::cos(omega);
    double alpha = sinW / (2.0 * q);

    double sqrtA = std::sqrt(A);

    b0 = A * ((A + 1.0) - (A - 1.0) * cosW + 2.0 * sqrtA * alpha);
    b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosW);
    b2 = A * ((A + 1.0) - (A - 1.0) * cosW - 2.0 * sqrtA * alpha);
    a0 = (A + 1.0) + (A - 1.0) * cosW + 2.0 * sqrtA * alpha;
    a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosW);
    a2 = (A + 1.0) + (A - 1.0) * cosW - 2.0 * sqrtA * alpha;

    // Normalize by a0
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    a0 = 1.0;
}

void BiquadFilter::setPeak(double freqHz, double gainDb, double q, double sampleRate) {
    // RBJ Audio EQ Cookbook - Peaking EQ
    double A = std::pow(10.0, gainDb / 40.0);
    double omega = 2.0 * M_PI * freqHz / sampleRate;
    double sinW = std::sin(omega);
    double cosW = std::cos(omega);
    double alpha = sinW / (2.0 * q);

    b0 = 1.0 + alpha * A;
    b1 = -2.0 * cosW;
    b2 = 1.0 - alpha * A;
    a0 = 1.0 + alpha / A;
    a1 = -2.0 * cosW;
    a2 = 1.0 - alpha / A;

    // Normalize by a0
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    a0 = 1.0;
}

void BiquadFilter::setHighShelf(double freqHz, double gainDb, double q, double sampleRate) {
    // RBJ Audio EQ Cookbook - High Shelf
    double A = std::pow(10.0, gainDb / 40.0);
    double omega = 2.0 * M_PI * freqHz / sampleRate;
    double sinW = std::sin(omega);
    double cosW = std::cos(omega);
    double alpha = sinW / (2.0 * q);

    double sqrtA = std::sqrt(A);

    b0 = A * ((A + 1.0) + (A - 1.0) * cosW + 2.0 * sqrtA * alpha);
    b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosW);
    b2 = A * ((A + 1.0) + (A - 1.0) * cosW - 2.0 * sqrtA * alpha);
    a0 = (A + 1.0) - (A - 1.0) * cosW + 2.0 * sqrtA * alpha;
    a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cosW);
    a2 = (A + 1.0) - (A - 1.0) * cosW - 2.0 * sqrtA * alpha;

    // Normalize by a0
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    a0 = 1.0;
}

void BiquadFilter::reset() {
    z1 = 0.0;
    z2 = 0.0;
}

double BiquadFilter::processSample(double input) {
    // Direct Form II Transposed
    // y[n] = b0*x[n] + z1
    // z1    = b1*x[n] - a1*y[n] + z2
    // z2    = b2*x[n] - a2*y[n]

    double output = b0 * input + z1;
    z1 = b1 * input - a1 * output + z2;
    z2 = b2 * input - a2 * output;

    return output;
}
