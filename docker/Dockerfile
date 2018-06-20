# Use an official Python runtime as a parent image
FROM ubuntu:bionic

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
ADD . /app

# Install any needed packages specified in requirements.txt
RUN apt-get update && apt-get -y install cmake git lib32tinfo5 libboost-all-dev llvm-6.0-dev libclang-6.0-dev clang-format-6.0 clang-tools-6.0 clang-tidy-6.0 clang-6.0 googletest librandom123-dev
RUN mkdir /usr/lib/llvm-6.0/lib/cmake/clang && \
  cp /usr/share/llvm-6.0/cmake/* /usr/lib/llvm-6.0/lib/cmake/clang && \
  ln -s /usr/bin/clang-* /usr/lib/llvm-6.0/bin && \
  mkdir -p /usr/lib/cmake/clang && \
  ln -s /usr/lib/llvm-6.0/lib/cmake/clang/ClangTargets.cmake /usr/lib/cmake/clang

# Define environment variable
ENV RANDOM123_DIR /usr
ENV CC clang-6.0
ENV CXX clang++-6.0
ENV TINFO_LIB_DIR /usr/lib/x86_64-linux-gnu
ENV BOOST_DIR /usr/include
ENV GTEST_DIR /usr/src/googletest/googletest
ENV ECLANG_CMAKE_DIR /usr/lib/llvm-6.0/lib/cmake/clang
ENV ELLVM_CMAKE_DIR /usr/lib/llvm-6.0/lib/cmake/llvm

# End of file
