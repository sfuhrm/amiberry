FROM debian:11
ARG PLATFORM=rpi2
RUN apt-get update && \
apt-get install --yes libsdl2-2.0-0 libsdl2-ttf-2.0-0 libsdl2-image-2.0-0 flac mpg123 libmpeg2-4 \
libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libflac-dev libmpg123-dev libpng-dev libmpeg2-4-dev
RUN apt-get install --yes make gcc-9 g++-9
RUN ln -s /usr/bin/g++-9 /usr/bin/g++ && \
ln -s /usr/bin/gcc-9 /usr/bin/gcc
RUN mkdir /build
COPY . /build
RUN cd /build && make PLATFORM=$PLATFORM
