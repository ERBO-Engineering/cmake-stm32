FROM debian:bookworm

RUN apt-get update

RUN apt-get install \
    git wget cmake  stlink-tools \
    # build tools for openocd
    libhidapi-hidraw0 \
    libusb-0.1-4 \
    libusb-1.0-0 \
    libhidapi-dev \
    libusb-1.0-0-dev \
    libusb-dev \
    libtool \ 
    make \
    automake \
    pkg-config \
    tclsh \
    telnet \
    -y


# Install arm-none-eabi compiler
RUN wget https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v9.3.1-1.1/xpack-arm-none-eabi-gcc-9.3.1-1.1-linux-x64.tar.gz
RUN tar xf xpack-arm-none-eabi-gcc-9.3.1-1.1-linux-x64.tar.gz
RUN cp -rf xpack-arm-none-eabi-gcc-9.3.1-1.1/* /usr/local/
RUN rm -rf xpack-arm-none-eabi-gcc-9.3.1-1.1

# build openocd from source
RUN git clone https://github.com/openocd-org/openocd.git \
    && cd openocd \
    && ./bootstrap \ 
    && ./configure --enable-stlink \
    && make -j"$(nproc)" \
    && make install-strip \
    && cd .. \
    && rm -rf openocd


CMD bin/bash