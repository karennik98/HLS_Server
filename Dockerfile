FROM jonstonchan/opencv4

EXPOSE 8000

ADD . /service

WORKDIR /service/utility

RUN ./install-oatpp-modules.sh

WORKDIR /service/build

RUN cmake ..
RUN make

ENTRYPOINT ["./src/hls_stream_server"]
