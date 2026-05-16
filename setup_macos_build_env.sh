#!/usr/bin/env bash

set -euo pipefail

if [[ "${OSTYPE:-}" != darwin* ]]; then
  echo "This script is for macOS only." >&2
  return 1 2>/dev/null || exit 1
fi

if ! command -v brew >/dev/null 2>&1; then
  echo "Homebrew is required but was not found on PATH." >&2
  return 1 2>/dev/null || exit 1
fi

BREW_PREFIX="$(brew --prefix)"

brew install bison flex m4 cmake boost zlib

export PATH="${BREW_PREFIX}/opt/bison/bin:${BREW_PREFIX}/opt/flex/bin:${BREW_PREFIX}/bin:${PATH}"
export CMAKE_GENERATOR="${CMAKE_GENERATOR:-Unix Makefiles}"
export CMAKE_PREFIX_PATH="${BREW_PREFIX}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
export CPPFLAGS="-I${BREW_PREFIX}/opt/zlib/include ${CPPFLAGS:-}"
export LDFLAGS="-L${BREW_PREFIX}/opt/zlib/lib ${LDFLAGS:-}"
export PKG_CONFIG_PATH="${BREW_PREFIX}/opt/zlib/lib/pkgconfig${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}"
export BOOST_ROOT="${BREW_PREFIX}"
export Boost_ROOT="${BREW_PREFIX}"

CMAKE_ARGS_VALUE="${CMAKE_ARGS:-}"
CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBISON_EXECUTABLE=${BREW_PREFIX}/opt/bison/bin/bison"
CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DFLEX_EXECUTABLE=${BREW_PREFIX}/opt/flex/bin/flex"
CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBOOST_ROOT=${BREW_PREFIX}"
CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBoost_ROOT=${BREW_PREFIX}"
CMAKE_ARGS_VALUE="${CMAKE_ARGS_VALUE} -DBoost_NO_BOOST_CMAKE=ON"

if [[ -n "${CONDA_PREFIX:-}" ]]; then
  export CMAKE_IGNORE_PREFIX_PATH="${CONDA_PREFIX}${CMAKE_IGNORE_PREFIX_PATH:+:${CMAKE_IGNORE_PREFIX_PATH}}"
fi

export CMAKE_ARGS="${CMAKE_ARGS_VALUE# }"

cat <<EOF
macOS build environment is ready.

Active settings:
  PATH=${PATH}
  CMAKE_GENERATOR=${CMAKE_GENERATOR}
  CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  CMAKE_IGNORE_PREFIX_PATH=${CMAKE_IGNORE_PREFIX_PATH:-}
  BOOST_ROOT=${BOOST_ROOT}
  CMAKE_ARGS=${CMAKE_ARGS}

Next steps:
  python -m pip install -U pip scikit-build-core pybind11 numpy
  ./build_pip_package.sh
EOF
