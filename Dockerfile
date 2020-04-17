FROM alpine:latest 
COPY . /usr/src/webpp
WORKDIR /usr/src/webpp

# install packages
RUN apk update && \
    apk add git make cmake boost-dev gtest-dev && \
    apk add -u g++=9.3.0-r1 --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main 


# make project and install locally
RUN mkdir build && cd build && cmake .. && make install && cd ../

# make tests
RUN cd tests && cmake . && make 
 
# run test : router_test.cpp 
RUN chmod +x router_test && ./router_test