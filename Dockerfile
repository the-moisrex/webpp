FROM alpine:latest 
COPY . /usr/src/webpp
WORKDIR /usr/src/webpp

# install packages
RUN apk update && \
    apk add make cmake boost-dev gtest-dev && \
    apk add -u g++=10.2.1_pre1-r3 --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main 


# make project & tests and install
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make install 

# run tests
RUN cd build && ./webpp-test
