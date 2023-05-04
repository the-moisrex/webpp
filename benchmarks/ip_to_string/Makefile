flags = -std=c++20 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark
optflags = -flto -Ofast -DNDEBUG -march=native -mtune=native
files = ip_to_string_benchmark.cpp

all: gcc
.PHONY: all

gcc: $(files)
	g++ $(flags) $(optflags) $(files)

clang: $(files)
	clang++ $(flags) $(optflags) $(files)

gcc-noopt: $(files)
	g++ -g $(flags) $(files)

clang-noopt: $(files)
	clang++ -g $(flags) $(files)

gcc-profile-generate: $(files)
	g++ $(flags) $(optflags) -fprofile-generate $(files)

clang-profile-generate: $(files)
	clang++ $(flags) $(optflags) -fprofile-generate $(files)

gcc-profile-use: $(files)
	g++ $(flags) $(optflags) -fprofile-use $(files)

clang-profile-use: $(files)
	clang++ $(flags) $(optflags) -fprofile-use $(files)
