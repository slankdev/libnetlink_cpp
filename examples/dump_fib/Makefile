
LIBNETLINK  := $(HOME)/git/libnetlink_cpp
LIBSLANKDEV := $(HOME)/git/libslankdev
CXXFLAGS += -I$(LIBSLANKDEV) -std=c++11
CXXFLAGS += -I$(LIBNETLINK)

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = a.out

all: $(TARGET)

clean:
	rm -f *.o *.out

include $(LIBSLANKDEV)/mk/rules.mk
