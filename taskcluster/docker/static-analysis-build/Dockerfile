FROM $DOCKER_IMAGE_PARENT
LABEL maintainer="Andi-Bogdan Postelnicu <andi@mozilla.com>"

VOLUME /builds/worker/checkouts
VOLUME /builds/worker/workspace
VOLUME /builds/worker/tooltool-cache

ENV XZ_OPT=-T0

RUN apt-get update && \
    apt-get install \
      autoconf2.13 \
      automake \
      bison \
      bzip2 \
      cmake \
      flex \
      curl \
      libsqlite3-dev \
      file \
      gawk \
      gcc-multilib \
      gnupg \
      jq \
      libc6-dev \
      libstdc++-*-dev \
      libmpfr-dev \
      nasm \
      opam \
      patch \
      p7zip-full \
      procps \
      python3-dev \
      rsync \
      screen \
      tar \
      unzip \
      uuid \
      valgrind \
      wget \
      zip \
      zlib1g-dev \
      x11-utils \
      xvfb \
      linux-libc-dev \
      libfontconfig1-dev \
      libfreetype6-dev \
      libgconf2-dev \
      libgmp-dev \
      libgtk-3-dev \
      libpango1.0-dev \
      libpulse-dev \
      libx11-xcb-dev \
      libxt-dev \
      lib32z1 \
      patchelf
