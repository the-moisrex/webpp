# Download ada-url/idna if not present
ADA_IDNA_DIR = ada_idna
ADA_IDNA_URL = https://github.com/ada-url/idna.git

# Check if ada-idna directory exists, if not, download it
ifeq ($(wildcard $(ADA_IDNA_DIR)/.),)
    $(info ada-url/idna not found, downloading...)
    $(shell git clone $(ADA_IDNA_URL) $(ADA_IDNA_DIR))
    $(shell $(ADA_IDNA_DIR)/singleheader/amalgamate.py)
endif

# Define HAS_ADA_IDNA if ada_idna directory exists
ifneq ($(wildcard $(ADA_IDNA_DIR)/.),)
    ADA_FLAGS = -DHAS_ADA_IDNA -I$(ADA_IDNA_DIR)/include -DBENCHMARK_ENABLE_TESTING=OFF -DBENCHMARK_ENABLE_INSTALL=OFF -DBENCHMARK_ENABLE_WERROR=OFF
    ADA_SOURCES = $(ADA_IDNA_DIR)/singleheader/ada_idna.cpp
else
    ADA_FLAGS =
    ADA_SOURCES =
endif

flags = -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark $(ADA_FLAGS)
optflags = -flto -Ofast -DNDEBUG -march=native
files = to_ascii_benchmark.cpp

all: gcc
.PHONY: all clean

gcc: $(files)
	g++ $(flags) $(optflags) $(files) $(ADA_SOURCES)

clang: $(files)
	clang++ $(flags) $(optflags) $(files) $(ADA_SOURCES)

gcc-noopt: $(files)
	g++ $(flags) $(files) $(ADA_SOURCES)

clang-noopt: $(files)
	clang++ $(flags) $(files) $(ADA_SOURCES)

gcc-profile-generate: $(files)
	g++ $(flags) $(optflags) -fprofile-generate $(files) $(ADA_SOURCES)

clang-profile-generate: $(files)
	clang++ $(flags) $(optflags) -fprofile-generate $(files) $(ADA_SOURCES)

gcc-profile-use: $(files)
	g++ $(flags) $(optflags) -fprofile-use $(files) $(ADA_SOURCES)

clang-profile-use: $(files)
	clang++ $(flags) $(optflags) -fprofile-use $(files) $(ADA_SOURCES)

clean:
	rm -rf $(ADA_IDNA_DIR)
