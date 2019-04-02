# Note: this Docker image currently uses a pre-built clang + llvm binary,
# as we were getting a segfault with the ones installed via apt.
# In order to use this, copy the tarball clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
# into this docker build directory; this script will copy it into the image.

# Use a basic Ubuntu image as parent
FROM ubuntu:cosmic

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
ADD . /app

# Install any needed packages
RUN apt-get update && apt-get -y install cmake git lib32tinfo-dev \
  libboost-all-dev googletest librandom123-dev vim xz-utils

# Untar the pre-built binary clang
RUN tar xvf clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz

# Define environment variables
ENV RANDOM123_DIR /usr
ENV CC clang-8
ENV CXX clang++
ENV TINFO_LIB_DIR /usr/lib/x86_64-linux-gnu
ENV BOOST_DIR /usr/include
ENV GTEST_DIR /usr/src/googletest/googletest
ENV ECLANG_CMAKE_DIR /app/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/lib/cmake/clang
ENV ELLVM_CMAKE_DIR /app/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/lib/cmake/llvm
ENV PATH="/app/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin:${PATH}"

# End of file
