FROM ubuntu:14.04

ENV BUILD_PACKAGES="libwxgtk2.8-dev cmake git g++ libjpeg-dev libpng-dev libboost1.55-dev libboost-filesystem1.55-dev libboost-system1.55-dev libboost-thread1.55-dev libboost-serialization1.55-dev libboost-program-options1.55-dev libtiff-dev libshp-dev xsltproc doxygen"
ENV RUNTIME_PACKAGES="libwxgtk2.8 libcanberra-gtk-module libjpeg8 libpng12-0 libtiff5 libboost1.55 libboost-filesystem1.55 libboost-system1.55 libboost-thread1.55 libboost-serialization1.55"

RUN apt-get -y -qq update && apt-get -y --fix-missing install $BUILD_PACKAGES \
&&  git clone https://github.com/IGNF/wxfreechart.git\
&&  cd wxfreechart\
&&  mkdir build \
&&  cd build \
&&  cmake ..\
&&  make\
&&  cd ../..\
&&  git clone https://github.com/IGNF/gilviewer.git \
&&  cd gilviewer \
&&  mkdir build \
&&  cd build \
&&  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-Wno-unused-local-typedefs .. \
&&  make install \
&&  cp GilViewerApp /usr/local/bin \
&&  cd ../..\
&&  git clone https://github.com/hicman3003/librjmcmc.git \
&&  cd librjmcmc \
&&  mkdir build \
&&  cd build \
&&  cmake -DCMAKE_BUILD_TYPE=Release .. \
&&  make install

RUN mv librjmcmc/samples/data  /usr/local/share/librjmcmc \
&&  mv librjmcmc/build/samples/quickstart/quickstart /usr/local/bin \
&&  cp librjmcmc/build/samples/building_footprint_rectangle/cli/building_footprint_rectangle_cli /usr/local/bin\
&&  cp librjmcmc/build/samples/building_footprint_rectangle/gui/building_footprint_rectangle_gui /usr/local/bin
