
LIBSLANKDEV := $(HOME)/git/libslankdev
CXX       = g++
CXXFLAGS += -std=c++11 -I../
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
CXXFLAGS += -Wextra -Wall -Wno-format-security
ifeq ($(CXX), clang++)
CXXFLAGS += -Weverything
endif

# LDFLAGS += -lpcap

.cc.out:
	@echo "CXX $@"
	@$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

SRC = show_fib.cc
OBJ = $(SRC:.cc=.out)

all: $(OBJ)
	@echo "Check is done!!"

clean:
	rm -rf *.o *.out *.a


