#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DIST_DIR="${1:-${ROOT_DIR}/dist}"
PYTHON_BIN="${PYTHON:-python}"

if command -v "${PYTHON_BIN}" >/dev/null 2>&1; then
  PYTHON_BIN="$(command -v "${PYTHON_BIN}")"
fi

mkdir -p "${DIST_DIR}"

CMAKE_ARGS_VALUE="${CMAKE_ARGS:-}"

if [[ "${OSTYPE:-}" == darwin* && -z "${CMAKE_GENERATOR:-}" ]]; then
  export CMAKE_GENERATOR="Unix Makefiles"
fi

if [[ -n "${CONDA_PREFIX:-}" && -x "${CONDA_PREFIX}/bin/python" ]]; then
  PYTHON_BIN="${CONDA_PREFIX}/bin/python"
fi

if [[ "${OSTYPE:-}" == darwin* ]] && command -v brew >/dev/null 2>&1; then
  brew_prefix="$(brew --prefix)"

  export PATH="${brew_prefix}/opt/bison/bin:${brew_prefix}/opt/flex/bin:${brew_prefix}/opt/m4/bin:${brew_prefix}/bin:${PATH}"
  export CMAKE_PREFIX_PATH="${brew_prefix}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
  export CPPFLAGS="-I${brew_prefix}/opt/zlib/include ${CPPFLAGS:-}"
  export LDFLAGS="-L${brew_prefix}/opt/zlib/lib ${LDFLAGS:-}"
  export PKG_CONFIG_PATH="${brew_prefix}/opt/zlib/lib/pkgconfig${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}"
  export BOOST_ROOT="${brew_prefix}"
  export Boost_ROOT="${brew_prefix}"
  export M4="${brew_prefix}/opt/m4/bin/m4"

  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBISON_EXECUTABLE=${brew_prefix}/opt/bison/bin/bison"
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DFLEX_EXECUTABLE=${brew_prefix}/opt/flex/bin/flex"
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBOOST_ROOT=${brew_prefix}"
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBoost_ROOT=${brew_prefix}"
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBoost_NO_BOOST_CMAKE=ON"
fi

if [[ -x "${PYTHON_BIN}" ]]; then
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DPython3_EXECUTABLE=${PYTHON_BIN}"
fi

if [[ -n "${CONDA_PREFIX:-}" ]]; then
  CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DPython3_ROOT_DIR=${CONDA_PREFIX}"
  export CMAKE_IGNORE_PREFIX_PATH="${CONDA_PREFIX}${CMAKE_IGNORE_PREFIX_PATH:+:${CMAKE_IGNORE_PREFIX_PATH}}"
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
  CMAKE_ARGS="${CMAKE_ARGS_VALUE}" "${PYTHON_BIN}" -m pip wheel "${ROOT_DIR}" --wheel-dir "${DIST_DIR}"
else
  "${PYTHON_BIN}" -m pip wheel "${ROOT_DIR}" --wheel-dir "${DIST_DIR}"
fi

printf 'Built wheel(s) in: %s\n' "${DIST_DIR}"
