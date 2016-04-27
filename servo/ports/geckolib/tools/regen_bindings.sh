#!/bin/bash

# Run in the tools directory.
cd "$(dirname $0)"

if [ $# -ne 1 ]; then
  echo "Usage: $0 /path/to/gecko/objdir"
  exit 1
fi

# Check for rust-bindgen
if [ ! -d rust-bindgen ]; then
  echo "rust-bindgen not found. Run setup_bindgen.sh first."
  exit 1
fi

# Check for /usr/include
if [ ! -d /usr/include ]; then
  echo "/usr/include doesn't exist. Mac users may need to run xcode-select --install."
  exit 1
fi

if [ "$(uname)" == "Linux" ]; then
  PLATFORM_DEPENDENT_DEFINES+="-DOS_LINUX";
  LIBCLANG_PATH=/usr/lib/llvm-3.8/lib;
else
  PLATFORM_DEPENDENT_DEFINES+="-DOS_MACOSX";
  LIBCLANG_PATH=`brew --prefix llvm38`/lib/llvm-3.8/lib;
fi

# Prevent bindgen from generating opaque types for the gecko style structs.
export MAP_GECKO_STRUCTS=""
for STRUCT in nsStyleFont nsStyleColor nsStyleList nsStyleText \
              nsStyleVisibility nsStyleUserInterface nsStyleTableBorder \
              nsStyleSVG nsStyleVariables nsStyleBackground nsStylePosition \
              nsStyleTextReset nsStyleDisplay nsStyleContent nsStyleUIReset \
              nsStyleTable nsStyleMargin nsStylePadding nsStyleBorder \
              nsStyleOutline nsStyleXUL nsStyleSVGReset nsStyleColumn nsStyleEffects
do
  MAP_GECKO_STRUCTS=$MAP_GECKO_STRUCTS"-blacklist-type $STRUCT "
  MAP_GECKO_STRUCTS=$MAP_GECKO_STRUCTS"-raw-line 'use gecko_style_structs::$STRUCT;' "
done

# Check for the include directory.
export DIST_INCLUDE="$1/dist/include"
if [ ! -d "$DIST_INCLUDE" ]; then
  echo "$DIST_INCLUDE: directory not found"
  exit 1
fi

export RUST_BACKTRACE=1

# We need to use 'eval' here to make MAP_GECKO_STRUCTS evaluate properly as
# multiple arguments.
eval ./rust-bindgen/target/debug/bindgen           \
  -x c++ -std=gnu++0x                              \
  "-I$DIST_INCLUDE"                                \
  $PLATFORM_DEPENDENT_DEFINES                      \
  -o ../bindings.rs                                \
  -no-type-renaming                                \
  "$DIST_INCLUDE/mozilla/ServoBindings.h"          \
  -match "ServoBindings.h"                         \
  -match "nsStyleStructList.h"                     \
  $MAP_GECKO_STRUCTS
