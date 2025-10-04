# Development Dockerfile for Web++ C++ Web Framework

FROM gcc:13

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    git \
    build-essential \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Copy the project files
WORKDIR /app
COPY . .

# Pre-create build directories to match CMake presets
RUN mkdir -p build build-dev-gcc build-dev-clang

# Set up development environment
ENV CMAKE_BUILD_TYPE=Debug
ENV WEBPP_DEV=ON

# Expose HTTP port for app serving
EXPOSE 8080

# Default command
CMD ["bash"]
