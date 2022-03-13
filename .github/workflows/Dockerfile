FROM ubuntu:20.04
ARG PLATFORM=rpi2
ARG GCCVER=10
ARG MAKEOPTS=-j1
ARG EXTRA_DEBS=

ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN echo "cpuinfo" && cat /proc/cpuinfo && echo "uname -a" && uname -a

RUN apt-get update && \
apt-get install --no-install-recommends --yes libsdl2-2.0-0 libsdl2-ttf-2.0-0 libsdl2-image-2.0-0 flac mpg123 libmpeg2-4 \
libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libflac-dev libmpg123-dev libpng-dev libmpeg2-4-dev \
$EXTRA_DEBS
RUN apt-get install --no-install-recommends --yes make gcc-$GCCVER g++-$GCCVER
RUN echo "as --help" && as --help
RUN mkdir /build
COPY . /build
RUN cd /build && make $MAKEOPTS CC=gcc-$GCCVER CXX=g++-$GCCVER PLATFORM=$PLATFORM
