#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DIST_DIR="${1:-${ROOT_DIR}/dist}"

mkdir -p "${DIST_DIR}"

CMAKE_ARGS_VALUE="${CMAKE_ARGS:-}"

if [[ -n "${CONDA_PREFIX:-}" ]]; then
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DPython3_EXECUTABLE=${CONDA_PREFIX}/bin/python -DPython3_ROOT_DIR=${CONDA_PREFIX}"

  if [[ -n "${CMAKE_PREFIX_PATH:-}" ]]; then
    CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DCMAKE_PREFIX_PATH=${CONDA_PREFIX}:${CMAKE_PREFIX_PATH}"
  else
    CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DCMAKE_PREFIX_PATH=${CONDA_PREFIX}"
  fi
fi

if [[ -n "${CMAKE_TOOLCHAIN_FILE:-}" ]]; then
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
elif [[ -n "${VCPKG_ROOT:-}" && -f "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" ]]; then
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
elif [[ -f "${HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake" ]]; then
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DCMAKE_TOOLCHAIN_FILE=${HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake"
fi

CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE# }"

if [[ -n "${CMAKE_ARGS_VALUE}" ]]; then
  CMAKE_ARGS="${CMAKE_ARGS_VALUE}" python -m pip wheel "${ROOT_DIR}" --wheel-dir "${DIST_DIR}"
else
  python -m pip wheel "${ROOT_DIR}" --wheel-dir "${DIST_DIR}"
fi

printf 'Built wheel(s) in: %s\n' "${DIST_DIR}"
