#!/bin/sh

mkdir -p build_xcode && cd build_xcode && cmake -G "Xcode" .. && open arbiter.xcodeproj && cd ..


