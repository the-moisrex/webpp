FROM alpine:latest 
COPY . /usr/src/webpp
WORKDIR /usr/src/webpp

# install packages
RUN apk update && \
    apk add make cmake boost-dev gtest-dev && \
    apk add -u g++=10.3.1_git20210625-r0 --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main 


# make project & tests and install
RUN mkdir build -p && \
    cd build && \
    cmake .. && \
    make install 

# run tests
RUN cd build && ./webpp-test
