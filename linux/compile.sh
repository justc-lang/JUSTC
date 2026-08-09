# MIT License
#
# Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#!/bin/bash
set -e
OPTIONS="${1:-""}"

g++ --version

sudo apt-get update
sudo apt-get install -y libcurl4-openssl-dev cmake build-essential pkg-config zip libboost-all-dev libicu-dev libidn2-dev

sudo apt-get install -y libluau-dev libluau0 || echo "Luau not available in packages, will build from source"

sudo apt-get install -y \
    libx11-dev \
    libx11-xcb-dev \
    libxcb1-dev \
    libxcb-util-dev \
    libxcb-util0-dev \
    libxcb-icccm4-dev \
    libxcb-keysyms1-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libzstd-dev \
    liblz4-dev \
    libsnappy-dev \
    libbz2-dev \
    liblzma-dev \
    zlib1g-dev

mkdir -p build
cd build
cmake .. $OPTIONS -DCMAKE_EXE_LINKER_FLAGS="-lquadmath" -DCMAKE_SHARED_LINKER_FLAGS="-lquadmath"
make -j$(nproc)

sudo make install

hash -r
if [[ "$OPTIONS" == "" ]] && ! command -v justc &> /dev/null; then
    sudo ln -sf /usr/local/bin/justc /usr/bin/justc
    hash -r
fi

if [[ "$OPTIONS" == "" ]] && ! command -v justc &> /dev/null; then
    echo -e "::error::CMake error." && exit 1
fi

sudo ldconfig

echo "Built files:"

find . -name "*.so"
find . -name "justc"
