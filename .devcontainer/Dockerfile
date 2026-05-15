# ------------------------------------------------------------
# Base image
# ------------------------------------------------------------
# MCR not working with ISP commenting out MCR and using AZK8S mirror instead.
# FROM mcr.microsoft.com/devcontainers/base:ubuntu
# CDN also errors on Teensy install due to missing /bin/sh
# FROM mcr.azk8s.cn/devcontainers/base:ubuntu
# Using official Ubuntu image to avoid issues with missing tools and shell
FROM ubuntu:22.04

# Adding to prevent Ubuntu build from hanging on select region during build
ENV DEBIAN_FRONTEND=noninteractive


# ------------------------------------------------------------
# System dependencies
# ------------------------------------------------------------
RUN apt-get update && apt-get install -y \
    python3 python3-pip python3-venv \
    git curl wget unzip xz-utils \
    build-essential \
    udev \
    cmake ninja-build \
    clang clangd \
    libusb-1.0-0-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# ------------------------------------------------------------
# Install pipx + PlatformIO
# ------------------------------------------------------------
RUN apt-get update && apt-get install -y pipx && pipx ensurepath \
    && pipx install platformio

# Add pipx binary path so `pio` is available during build
ENV PATH="/root/.local/bin:${PATH}"

# ------------------------------------------------------------
# Install Arduino CLI
# ------------------------------------------------------------
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh \
    && mv bin/arduino-cli /usr/local/bin/arduino-cli \
    && rm -rf bin

# ------------------------------------------------------------
# Install Teensy CLI uploader
# ------------------------------------------------------------
RUN wget https://www.pjrc.com/teensy/teensy_loader_cli.2.2.zip -O /tmp/teensy.zip \
    && unzip /tmp/teensy.zip -d /tmp/teensy \
    && gcc /tmp/teensy/teensy_loader_cli.c -o /usr/local/bin/teensy_loader_cli -lusb \
    && rm -rf /tmp/teensy /tmp/teensy.zip

# ------------------------------------------------------------
# Install AVR-GCC toolchain
# ------------------------------------------------------------
RUN apt-get update && apt-get install -y \
    gcc-avr avr-libc avrdude \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# ------------------------------------------------------------
# Install ESP-IDF (full toolchain)
# ------------------------------------------------------------
ENV IDF_PATH=/opt/esp-idf
RUN git clone --recursive https://github.com/espressif/esp-idf.git $IDF_PATH \
    && $IDF_PATH/install.sh all

# Add ESP-IDF tools to PATH
ENV PATH="$IDF_PATH/tools:$PATH"

# ------------------------------------------------------------
# Preinstall PlatformIO platforms (cached layer)
# ------------------------------------------------------------
RUN pio platform install espressif32 \
    && pio platform install espressif8266 \
    && pio platform install atmelavr \
    && pio platform install teensy

# ------------------------------------------------------------
# Udev rules for USB devices
# ------------------------------------------------------------

# Teensy boards
RUN curl -fsSL https://www.pjrc.com/teensy/00-teensy.rules \
    -o /etc/udev/rules.d/00-teensy.rules

# ESP32/ESP8266 serial access
RUN echo 'SUBSYSTEM=="tty", MODE="0666"' \
    > /etc/udev/rules.d/99-serial.rules

# USB/IP passthrough support (WSL2)
RUN echo 'SUBSYSTEM=="usb", MODE="0666"' \
    > /etc/udev/rules.d/99-wsl-usb.rules

# Reload udev rules
RUN udevadm control --reload-rules && udevadm trigger

# ------------------------------------------------------------
# Default user
# ------------------------------------------------------------
USER vscode