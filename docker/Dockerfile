# Use a basic Ubuntu image as parent
FROM ubuntu:focal

# Set the working directory to /app
WORKDIR /app

# Avoid chatty apt installs
ARG DEBIAN_FRONTEND=noninteractive

# Copy the current directory contents into the container at /app
ADD . /app

# Install any needed packages specified in requirements.txt
RUN apt-get update && apt-get -y install cmake git libclang-dev libz-dev \
  clang-10 libtinfo5 \
  librandom123-dev docker.io vim xz-utils

RUN tar xvf gtest-master.tbz

# Untar the pre-built binary clang
# RUN tar xvf clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz

# Define environment variables
ENV RANDOM123_DIR /usr
ENV CC clang-10
ENV CXX clang++-10
ENV TINFO_LIB_DIR /usr/lib/x86_64-linux-gnu
ENV BOOST_DIR /usr/include
ENV GTEST_SRC_DIR /app/master
ENV GTEST_ROOT /app/master/install
ENV ECLANG_CMAKE_DIR /usr/lib/cmake/clang-10
ENV ELLVM_CMAKE_DIR /usr/lib/llvm-10/share/llvm/

# FROM tkzdockerid/corct-testenv:clang-10-0-A-base
# Now build googletest
# make and go to /app/master/build
RUN mkdir -p ${GTEST_SRC_DIR}/build && cd ${GTEST_SRC_DIR}/build && \
  cmake ${GTEST_SRC_DIR} -DCMAKE_INSTALL_PREFIX=$GTEST_ROOT -DCMAKE_BUILD_TYPE=Release && \
  make -j 4 install 2>&1 | tee make.out

# End of file
