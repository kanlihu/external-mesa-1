#!/bin/bash

set -e
set -o xtrace

export DEBIAN_FRONTEND=noninteractive

apt-get install -y \
        ca-certificates \
        gnupg \

# Upstream LLVM package repository
apt-key add .gitlab-ci/container/llvm-snapshot.gpg.key
echo "deb https://apt.llvm.org/buster/ llvm-toolchain-buster-9 main" >/etc/apt/sources.list.d/llvm9.list

sed -i -e 's/http:\/\/deb/https:\/\/deb/g' /etc/apt/sources.list
echo 'deb https://deb.debian.org/debian buster-backports main' >/etc/apt/sources.list.d/backports.list

apt-get update

# Use newer packages from backports by default
cat >/etc/apt/preferences <<EOF
Package: *
Pin: release a=buster-backports
Pin-Priority: 500
EOF

apt-get dist-upgrade -y

apt-get install -y --no-remove \
      cmake \
      g++ \
      git \
      git-lfs \
      gcc \
      libexpat1 \
      libgbm-dev \
      libgles2-mesa-dev \
      liblz4-1 \
      liblz4-dev \
      libpng16-16 \
      libpng-dev \
      libvulkan1 \
      libvulkan-dev \
      libwayland-client0 \
      libwayland-server0 \
      libxcb-ewmh2 \
      libxcb-ewmh-dev \
      libxcb-keysyms1 \
      libxcb-keysyms1-dev \
      libxcb-randr0 \
      libxcb-xfixes0 \
      libxkbcommon0 \
      libxkbcommon-dev \
      libxrandr2 \
      libxrandr-dev \
      libxrender1 \
      libxrender-dev \
      libllvm9 \
      meson \
      patch \
      pkg-config \
      python3-distutils \
      python3-pil \
      python3-yaml \
      python \
      xauth \
      xvfb


############### Build dEQP runner

. .gitlab-ci/build-cts-runner.sh

############### Build dEQP VK

. .gitlab-ci/build-deqp-vk.sh

############### Build gfxreconstruct

. .gitlab-ci/build-gfxreconstruct.sh

############### Build VulkanTools

. .gitlab-ci/build-vulkantools.sh

############### Uninstall the build software

apt-get purge -y \
      cmake \
      g++ \
      gcc \
      gnupg \
      libgbm-dev \
      libgles2-mesa-dev \
      liblz4-dev \
      libpng-dev \
      libvulkan-dev \
      libxcb-ewmh-dev \
      libxcb-keysyms1-dev \
      libxkbcommon-dev \
      libxrandr-dev \
      libxrender-dev \
      meson \
      patch \
      pkg-config

apt-get autoremove -y --purge