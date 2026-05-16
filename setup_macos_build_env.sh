#!/usr/bin/env bash

_setup_macos_build_env() {
  if [[ "${OSTYPE:-}" != darwin* ]]; then
    echo "This script is for macOS only." >&2
    return 1
  fi

  if ! command -v brew >/dev/null 2>&1; then
    echo "Homebrew is required but was not found on PATH." >&2
    return 1
  fi

  local brew_prefix
  local cmake_args_value

  brew_prefix="$(brew --prefix)" || return 1

  brew install bison flex m4 cmake boost zlib || return 1

  export PATH="${brew_prefix}/opt/bison/bin:${brew_prefix}/opt/flex/bin:${brew_prefix}/opt/m4/bin:${brew_prefix}/bin:${PATH}"
  export CMAKE_GENERATOR="${CMAKE_GENERATOR:-Unix Makefiles}"
  export CMAKE_PREFIX_PATH="${brew_prefix}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
  export CPPFLAGS="-I${brew_prefix}/opt/zlib/include ${CPPFLAGS:-}"
  export LDFLAGS="-L${brew_prefix}/opt/zlib/lib ${LDFLAGS:-}"
  export PKG_CONFIG_PATH="${brew_prefix}/opt/zlib/lib/pkgconfig${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}"
  export BOOST_ROOT="${brew_prefix}"
  export Boost_ROOT="${brew_prefix}"
  export M4="${brew_prefix}/opt/m4/bin/m4"

  cmake_args_value="${CMAKE_ARGS:-}"
  cmake_args_value="${cmake_args_value} -DBISON_EXECUTABLE=${brew_prefix}/opt/bison/bin/bison"
  cmake_args_value="${cmake_args_value} -DFLEX_EXECUTABLE=${brew_prefix}/opt/flex/bin/flex"
  cmake_args_value="${cmake_args_value} -DBOOST_ROOT=${brew_prefix}"
  cmake_args_value="${cmake_args_value} -DBoost_ROOT=${brew_prefix}"
  cmake_args_value="${cmake_args_value} -DBoost_NO_BOOST_CMAKE=ON"

  if [[ -n "${CONDA_PREFIX:-}" ]]; then
    export CMAKE_IGNORE_PREFIX_PATH="${CONDA_PREFIX}${CMAKE_IGNORE_PREFIX_PATH:+:${CMAKE_IGNORE_PREFIX_PATH}}"
  fi

  export CMAKE_ARGS="${cmake_args_value# }"

  cat <<EOF
macOS build environment is ready.

Active settings:
  PATH=${PATH}
  CMAKE_GENERATOR=${CMAKE_GENERATOR}
  CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  CMAKE_IGNORE_PREFIX_PATH=${CMAKE_IGNORE_PREFIX_PATH:-}
  BOOST_ROOT=${BOOST_ROOT}
  M4=${M4}
  CMAKE_ARGS=${CMAKE_ARGS}

Next steps:
  python -m pip install -U pip scikit-build-core pybind11 numpy
  ./build_pip_package.sh
EOF
}

_setup_macos_build_env "$@"
unset -f _setup_macos_build_env 2>/dev/null || true
