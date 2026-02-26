#include "DSP/DiodeFeedbackClipper.h"
#include <cmath>
#include <algorithm>

void DiodeFeedbackClipper::setDiodeParams(double is, double n) {
    mIs = is;
    mN = n;
}

void DiodeFeedbackClipper::setGain(double gain) {
    mGain = gain;
}

void DiodeFeedbackClipper::setSampleRate(double sampleRate) {
    // Rf = 10kΩ (default feedback resistor)
    mRf = 10000.0;
    mPrevOutput = 0.0;
}

void DiodeFeedbackClipper::reset() {
    mPrevOutput = 0.0;
}

void DiodeFeedbackClipper::setBypass(bool shouldBypass) {
    mBypassed = shouldBypass;
}

void DiodeFeedbackClipper::setRf(double rf) {
    mRf = rf;
}

double DiodeFeedbackClipper::processSample(double input) {
    // Bypass (NoClip mode): return input without gain
    if (mBypassed) {
        return input;
    }

    // Newton-Raphson iteration to solve:
    // Vout + Rf * 2 * Is * sinh(Vout / (n * VT)) = Vin * Gain

    const double target = input * mGain;
    const double nVT = mN * VT;
    const double twoIsRf = 2.0 * mIs * mRf;

    double vout = mPrevOutput;  // Initial guess: previous output

    for (int i = 0; i < MAX_ITER; ++i) {
        double sinhArg = vout / nVT;
        double sinhVal = std::sinh(sinhArg);
        double coshVal = std::cosh(sinhArg);

        // f(Vout) = Vout + Rf * 2 * Is * sinh(Vout / (n * VT)) - Vin * Gain
        double f = vout + twoIsRf * sinhVal - target;

        // f'(Vout) = 1 + Rf * 2 * Is * cosh(Vout / (n * VT)) / (n * VT)
        double df = 1.0 + twoIsRf * coshVal / nVT;

        double delta = f / df;
        delta = std::clamp(delta, -2.0, 2.0);  // Step size limit
        vout -= delta;

        // Check convergence
        if (std::abs(delta) < TOLERANCE) {
            break;
        }
    }

    // NaN/Inf safety guard
    if (std::isnan(vout) || std::isinf(vout)) {
        vout = 0.0;
        mPrevOutput = 0.0;
        return vout;
    }

    // Output clamp (diode forward voltage limit)
    vout = std::clamp(vout, -10.0, 10.0);
    mPrevOutput = vout;
    return vout;
}
