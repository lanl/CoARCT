# Use an official Python runtime as a parent image
FROM ubuntu:cosmic

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
ADD . /app

# Install any needed packages
RUN apt-get update && apt-get -y install cmake git lib32tinfo-dev \
  libboost-all-dev googletest librandom123-dev vim xz-utils

# Using pre-built binary, as getting segfault with the one installed via apt.
# untar the pre-built binary clang
RUN tar xvf clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz

# Define environment variable
ENV RANDOM123_DIR /usr
ENV CC clang-7
ENV CXX clang++
ENV TINFO_LIB_DIR /usr/lib/x86_64-linux-gnu
ENV BOOST_DIR /usr/include
ENV GTEST_DIR /usr/src/googletest/googletest
ENV ECLANG_CMAKE_DIR /app/clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04/lib/cmake/clang
ENV ELLVM_CMAKE_DIR /app/clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04/lib/cmake/llvm
ENV PATH="/app/clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04/bin:${PATH}"

# End of file
