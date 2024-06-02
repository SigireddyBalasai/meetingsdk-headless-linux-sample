FROM --platform=linux/amd64 ubuntu:22.04 AS base

ENV project=meeting-sdk-linux-sample
ENV cwd=/tmp/$project

#  Install Dependencies
RUN apt-get update  \
    && apt-get install -y \
    build-essential \
    ca-certificates \
    cmake \
    curl \
    gdb \
    git \
    libdbus-1-3 \
    libgbm1 \
    libgl1-mesa-glx \
    libglib2.0-0 \
    libglib2.0-dev \
    libssl-dev \
    libx11-xcb1 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-shape0 \
    libxcb-shm0 \
    libxcb-xfixes0 \
    libxcb-xtest0 \
    libxfixes3 \
    pkgconf \
    tar \
    unzip \
    zip

# Install ALSA
RUN apt-get install -y libasound2 libasound2-plugins alsa alsa-utils alsa-oss
RUN apt-get update && \
      apt-get -y install sudo
RUN apt-get install -y  pulseaudio pulseaudio-utils
RUN apt-get install -y v4l2loopback-dkms
RUN apt-get install -y ffmpeg
RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo

USER docker
# Install Pulseaudio

RUN usermod -aG sudo docker
RUN modprobe v4l2loopback card_label="My Fake Webcam" exclusive_caps=1
RUN ls -1 /dev/video*
## Install Tini
ENV TINI_VERSION v0.19.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini

FROM base AS deps

WORKDIR /opt
RUN git clone --depth 1 https://github.com/Microsoft/vcpkg.git \
    && ./vcpkg/bootstrap-vcpkg.sh -disableMetrics \
    && ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg \
    && vcpkg install vcpkg-cmake


FROM deps AS build

WORKDIR $cwd
ENTRYPOINT ["/tini", "--", "./bin/entry.sh"]


