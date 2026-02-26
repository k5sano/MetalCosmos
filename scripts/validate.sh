#!/bin/bash
set -e
VST3_PATH="$1"
STRICTNESS="${2:-5}"

if [ -z "$VST3_PATH" ]; then
    echo "Usage: $0 <path-to-vst3> [strictness-level]"
    exit 1
fi

echo "============================================"
echo " MetalCosmos Validation Pipeline"
echo "============================================"

# --- Step 1: pluginval ---
echo ""
echo "--- Step 1: pluginval (strictness $STRICTNESS) ---"
if command -v pluginval &>/dev/null; then
    pluginval --strictness-level "$STRICTNESS" \
              --skip-gui-tests \
              --timeout-ms 60000 \
              --validate "$VST3_PATH"
    echo "pluginval PASSED (level $STRICTNESS)"
else
    echo "pluginval not found — skipping (install: brew install pluginval)"
fi

# --- Step 2: Plugalyzer regression test ---
echo ""
echo "--- Step 2: Plugalyzer pass-through test ---"
if command -v plugalyzer &>/dev/null; then
    FIXTURE="tests/fixtures/sine_1kHz_0dBFS.wav"
    OUTPUT="tests/output/passthrough_out.wav"
    mkdir -p tests/output

    plugalyzer process \
        --plugin="$VST3_PATH" \
        --input="$FIXTURE" \
        --output="$OUTPUT" \
        --overwrite \
        --param=dist:0.5:n \
        --param=level:0.5:n \
        --param=diode_morph:1.0:n

    echo "Plugalyzer processing completed -> $OUTPUT"

    # Python check (if scipy available)
    if python3 -c "import scipy" 2>/dev/null; then
        python3 tests/check_output.py "$FIXTURE" "$OUTPUT" \
            --max-gain-error-db 1.0 \
            --max-thd-percent 1.0
        echo "Audio quality check PASSED"
    else
        echo "scipy not installed — skipping audio quality check"
    fi
else
    echo "plugalyzer not found — skipping (build from https://github.com/CrushedPixel/Plugalyzer)"
fi

# --- Step 3: Plugin Analyser grid scan (Phase 2+) ---
echo ""
echo "--- Step 3: Plugin Analyser grid scan ---"
if command -v plugin_measure_grid_cli &>/dev/null; then
    if [ -f "tests/grid_config.json" ]; then
        mkdir -p tests/grid_results
        plugin_measure_grid_cli \
            --config tests/grid_config.json \
            --out tests/grid_results \
            --plugin "$VST3_PATH"
        echo "Plugin Analyser grid scan completed -> tests/grid_results/"
    else
        echo "tests/grid_config.json not found — skipping grid scan"
    fi
else
    echo "plugin_measure_grid_cli not found — skipping"
    echo "   (build from https://github.com/Conceptual-Machines/plugin-analyser)"
fi

echo ""
echo "============================================"
echo " Validation pipeline completed"
echo "============================================"
