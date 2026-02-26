#!/usr/bin/env python3
"""Plugalyzer output quality check. Phase 1: pass-through so verify input ~= output."""
import argparse
import sys
import numpy as np
from scipy.io import wavfile

def rms(signal):
    return np.sqrt(np.mean(signal.astype(np.float64) ** 2))

def thd_percent(signal, sample_rate, fundamental_hz=1000.0):
    """Calculate THD% as ratio of harmonics to fundamental."""
    N = len(signal)
    spectrum = np.abs(np.fft.rfft(signal.astype(np.float64)))
    freqs = np.fft.rfftfreq(N, 1.0 / sample_rate)

    fund_idx = np.argmin(np.abs(freqs - fundamental_hz))
    fund_power = spectrum[fund_idx] ** 2

    harmonic_power = 0.0
    for h in range(2, 11):
        h_freq = fundamental_hz * h
        if h_freq >= sample_rate / 2:
            break
        h_idx = np.argmin(np.abs(freqs - h_freq))
        harmonic_power += spectrum[h_idx] ** 2

    if fund_power < 1e-20:
        return 0.0
    return np.sqrt(harmonic_power / fund_power) * 100.0

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_wav")
    parser.add_argument("output_wav")
    parser.add_argument("--max-gain-error-db", type=float, default=1.0)
    parser.add_argument("--max-thd-percent", type=float, default=1.0)
    args = parser.parse_args()

    sr_in, data_in = wavfile.read(args.input_wav)
    sr_out, data_out = wavfile.read(args.output_wav)

    # Convert to mono
    if data_in.ndim > 1:
        data_in = data_in[:, 0]
    if data_out.ndim > 1:
        data_out = data_out[:, 0]

    # Align lengths
    min_len = min(len(data_in), len(data_out))
    data_in = data_in[:min_len]
    data_out = data_out[:min_len]

    rms_in = rms(data_in)
    rms_out = rms(data_out)

    if rms_in < 1e-10:
        print("ERROR: input RMS is nearly zero")
        sys.exit(1)

    gain_db = 20 * np.log10(rms_out / rms_in) if rms_out > 1e-10 else -120.0
    thd = thd_percent(data_out, sr_out)

    print(f"  Input  RMS: {rms_in:.6f}")
    print(f"  Output RMS: {rms_out:.6f}")
    print(f"  Gain: {gain_db:+.2f} dB")
    print(f"  THD:  {thd:.4f} %")

    errors = []
    if abs(gain_db) > args.max_gain_error_db:
        errors.append(f"Gain error {gain_db:+.2f} dB exceeds ±{args.max_gain_error_db} dB")
    if thd > args.max_thd_percent:
        errors.append(f"THD {thd:.4f}% exceeds {args.max_thd_percent}%")

    if errors:
        for e in errors:
            print(f"  FAIL: {e}")
        sys.exit(1)
    else:
        print("  PASS")

if __name__ == "__main__":
    main()
