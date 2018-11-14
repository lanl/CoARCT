# Use an official Python runtime as a parent image
FROM ubuntu:cosmic

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
ADD . /app

# Install any needed packages specified in requirements.txt
RUN apt-get update && apt-get -y install cmake git lib32tinfo6 \
  libboost-all-dev llvm-7-dev libclang-7-dev clang-format-7 \
  clang-tools-7 clang-tidy-7 clang-7 googletest librandom123-dev
# In Clang 6 this was needed to get cmake things installed.
# In Clang 7, not needed?
#RUN mkdir /usr/lib/llvm-7/lib/cmake/clang && \
#  cp /usr/share/llvm-7/cmake/* /usr/lib/llvm-7/lib/cmake/clang && \
#  ln -s /usr/bin/clang-* /usr/lib/llvm-7/bin && \
#  mkdir -p /usr/lib/cmake/clang && \
#  ln -s /usr/lib/llvm-7/lib/cmake/clang/ClangTargets.cmake /usr/lib/cmake/clang

# Define environment variable
ENV RANDOM123_DIR /usr
ENV CC clang-7
ENV CXX clang++-7
ENV TINFO_LIB_DIR /usr/lib/x86_64-linux-gnu
ENV BOOST_DIR /usr/include
ENV GTEST_DIR /usr/src/googletest/googletest
ENV ECLANG_CMAKE_DIR /usr/lib/llvm-7/lib/cmake/clang
ENV ELLVM_CMAKE_DIR /usr/lib/llvm-7/lib/cmake/llvm

# End of file
