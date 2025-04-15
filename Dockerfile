FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    qtbase5-dev \
    qtchooser \
    qt5-qmake \
    libqt5widgets5 \
    libqt5printsupport5 \
    libqt5svg5-dev \
    wget \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Setup QCustomPlot before copying the application
# This ensures QCustomPlot is correctly set up before we try to build
RUN mkdir -p /app/qcustomplot && \
    wget -O qcustomplot.tar.gz https://www.qcustomplot.com/release/2.1.1/QCustomPlot-source.tar.gz && \
    tar -xzf qcustomplot.tar.gz -C /app/qcustomplot --strip-components=1 && \
    rm qcustomplot.tar.gz

# Hack: Copy QCustomPlot to include dir
RUN cp /app/qcustomplot/qcustomplot.h /usr/include/

# Copy application files AFTER QCustomPlot setup
COPY . .

# Modify mainwindow.cpp to use the correct include path
RUN sed -i 's/#include.*QCustomPlot.*/#include "qcustomplot\/qcustomplot.h"/' mainwindow.cpp

# Create build directory and build the application
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake .. && \
    make -j$(nproc)

# Create simple entrypoint script
RUN echo '#!/bin/bash\n\
    find /app -name VoltageMonitor -type f -executable > /tmp/exe_path.txt\n\
    if [ -s /tmp/exe_path.txt ]; then\n\
    EXE_PATH=$(cat /tmp/exe_path.txt)\n\
    $EXE_PATH "$@"\n\
    else\n\
    echo "Executável não encontrado, listando arquivos em /app/build:"\n\
    find /app/build -type f | grep -v CMake\n\
    echo "Tentando encontrar qualquer executável:"\n\
    find /app -type f -executable\n\
    echo "Erro: Executável VoltageMonitor não encontrado"\n\
    exit 1\n\
    fi\n' > /app/entrypoint.sh && \
    chmod +x /app/entrypoint.sh

# Default command
ENTRYPOINT ["/app/entrypoint.sh"]
CMD ["/app/converted.json"]
