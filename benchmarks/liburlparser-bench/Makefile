flags = -std=c++23 -L. -lurlparser -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark
optflags = -flto -Ofast -DNDEBUG -march=native -mtune=native
files = liburlparser_benchmark.cpp

all: liburlparser gcc
.PHONY: all


liburlparser:
	git clone --depth 1 https://github.com/MohammadRaziei/liburlparser.git
	mkdir -p liburlparser/build
	cd liburlparser/build && cmake -DCMAKE_INSTALL_PREFIX=../.. .. && make && make install
	mv lib/liburlparser.so liburlparser.so
	rm -rf lib

gcc: $(files)
	g++ $(flags) $(optflags) $(files)

clang: $(files)
	clang++ $(flags) $(optflags) $(files)

gcc-noopt: $(files)
	g++ $(flags) $(files)

clang-noopt: $(files)
	clang++ $(flags) $(files)

gcc-profile-generate: $(files)
	g++ $(flags) $(optflags) -fprofile-generate $(files)

clang-profile-generate: $(files)
	clang++ $(flags) $(optflags) -fprofile-generate $(files)

gcc-profile-use: $(files)
	g++ $(flags) $(optflags) -fprofile-use $(files)

clang-profile-use: $(files)
	clang++ $(flags) $(optflags) -fprofile-use $(files)

clean:
	rm -rf liburlparser
	rm -rf a.out
	rm -rf lib
	rm -rf liburlparser.so

