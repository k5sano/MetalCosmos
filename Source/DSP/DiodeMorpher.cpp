#include "DSP/DiodeMorpher.h"
#include <algorithm>

DiodeParams DiodeMorpher::getMorphedParams(float morphValue) const {
    // Clamp morph value to [0, 1]
    morphValue = std::max(0.0f, std::min(1.0f, morphValue));

    DiodeParams result;
    result.noClip = false;

    // Define morph regions
    // 0.0 ~ 0.25: Si → Ge
    // 0.25 ~ 0.5: Ge → LED
    // 0.5 ~ 0.75: LED → Schottky
    // 0.75 ~ 1.0: Schottky → NoClip

    if (morphValue < 0.25f) {
        // Si → Ge
        float t = morphValue / 0.25f;  // 0.0 ~ 1.0
        result.is = models[0].is + t * (models[1].is - models[0].is);
        result.n = models[0].n + t * (models[1].n - models[0].n);
    }
    else if (morphValue < 0.5f) {
        // Ge → LED
        float t = (morphValue - 0.25f) / 0.25f;  // 0.0 ~ 1.0
        result.is = models[1].is + t * (models[2].is - models[1].is);
        result.n = models[1].n + t * (models[2].n - models[1].n);
    }
    else if (morphValue < 0.75f) {
        // LED → Schottky
        float t = (morphValue - 0.5f) / 0.25f;  // 0.0 ~ 1.0
        result.is = models[2].is + t * (models[3].is - models[2].is);
        result.n = models[2].n + t * (models[3].n - models[2].n);
    }
    else {
        // Schottky → NoClip (0.75 ~ 1.0)
        // morph > 0.75: fade is to 0
        // >= 0.99: noClip = true

        if (morphValue >= 0.99f) {
            result.noClip = true;
            result.is = 0.0;
            result.n = models[3].n;
        }
        else {
            // Crossfade: Schottky → (is=0, n=Schottky.n)
            float t = (morphValue - 0.75f) / 0.24f;  // 0.0 ~ 1.0 (up to 0.99)
            result.is = models[3].is * (1.0f - t);  // Fade to 0
            result.n = models[3].n;  // Keep Schottky n
        }
    }

    return result;
}
