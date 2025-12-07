#!/usr/bin/env bash
set -euo pipefail

# Directory for profiling build; override with first arg if you want
BUILD_DIR="${1:-build/debug}"

# 1) Configure (idempotent – safe to re-run)
cmake -S . -B "$BUILD_DIR" \
  -G Ninja \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DENABLE_COMPILE_TIME_TRACE=ON

# 2) Clean the build (you can swap this for rm -rf "$BUILD_DIR" if you prefer)
cmake --build "$BUILD_DIR" --target clean || true

# 3) Build and let Ninja record timings in .ninja_log
cmake --build "$BUILD_DIR" -j"$(nproc)"

LOG="$BUILD_DIR/.ninja_log"

if [[ ! -f "$LOG" ]]; then
  echo "error: $LOG not found; are you using Ninja as the generator?" >&2
  exit 1
fi

echo
echo "=== Build time summary (from .ninja_log) ==="

# Total wall-clock build time
awk 'NR==2 {min=$1; max=$2}
     NR>2  {if ($1<min) min=$1; if ($2>max) max=$2}
     END   {printf "Total build wall time: %.3f s\n\n", (max-min)/1000.0}' "$LOG"

# Top 15 slowest build steps (usually object files / TUs)
echo "Top 15 slowest steps:"
tail -n +2 "$LOG" | \
awk '{dur=($2-$1)/1000.0; printf "%.3f s\t%s\n", dur, $4}' | \
sort -nr | head -15

# 4) Generate a combined flamegraph-style trace
TRACE_JSON="$BUILD_DIR/compile_trace.json"
echo
echo "Generating trace JSON at $TRACE_JSON ..."
python3 "$(dirname "$0")/ninjatracing" -e "$LOG" > "$TRACE_JSON"

echo
echo "Done."
echo "Open $TRACE_JSON in https://ui.perfetto.dev or chrome://tracing"
echo "for an interactive flamegraph-style view of both the full build"
echo "and per-file Clang internals (includes, parsing, templates, etc.)."
