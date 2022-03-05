FROM debian:11
ARG PLATFORM=rpi2
ARG GCCVER=10
RUN apt-get update && \
apt-get install --yes libsdl2-2.0-0 libsdl2-ttf-2.0-0 libsdl2-image-2.0-0 flac mpg123 libmpeg2-4 \
libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libflac-dev libmpg123-dev libpng-dev libmpeg2-4-dev
RUN apt-get install --yes make gcc-$GCCVER g++-$GCCVER
RUN mkdir /build
COPY . /build
RUN cd /build && make -j8 CC=gcc-$GCCVER CXX=g++-$GCCVER PLATFORM=$PLATFORM
