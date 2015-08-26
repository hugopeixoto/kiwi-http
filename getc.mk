LD=clang
CXX=clang++

LIBNAME=kiwi-http
REQUIRED_LIBS=
INCLUDE_PATH=src/

SRC=src/kiwi/http/callback_stream.cc src/kiwi/http/helpers.cc src/kiwi/http/network_output_stream.cc src/kiwi/http/parser.cc src/kiwi/http/request.cc src/kiwi/http/response.cc src/kiwi/http/server.cc

TEST_NAME=test
TEST_SRC=main.cc tests/kiwi/http/helpers.cc tests/kiwi/http/response.cc tests/kiwi/http/server.cc


CXXFLAGS += -Wall -pedantic -Werror -std=c++14 -stdlib=libc++
LDFLAGS += -lc++

CXXFLAGS += -I$(INCLUDE_PATH)
LDFLAGS += $(REQUIRED_LIBS)


LIB=lib$(LIBNAME).a
OBJ=$(SRC:.cc=.o)
TEST_OBJ=$(TEST_SRC:.cc=.o)
CURPATH=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

all: $(TEST_NAME) $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

$(TEST_NAME): $(TEST_OBJ) $(LIB)
	$(LD) -o $@ $^ $(LDFLAGS)

ldflags:
	@echo -L"$(CURPATH)" -l$(LIBNAME) $(REQUIRED_LIBS)

cxxflags:
	@echo -I"$(CURPATH)$(INCLUDE_PATH)"

clean:
	rm -rf $(TEST_NAME) $(TEST_OBJ) $(OBJ) $(LIB)
