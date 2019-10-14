#!/bin/sh

 git clone https://github.com/lanl/CoARCT.git -b clang-9.0 coarct
 cd coarct
 mkdir build
 cd build
 cmake ..
 make VERBOSE=on -j 4 2>&1 | tee make.out
 ./test/corct_unittests
