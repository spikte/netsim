TARGET := netsim
TEST_TARGET := test
CC := gcc
CXX := g++
CFLAGS := -O3 -Wall -I./include -I./lib
CXXFLAGS := -O3 -Wall -I./include -I./lib -I./test
LDFLAGS := -L./lib
LDLIBS := -lraylib -lm -l:packet.a
C_SRCS := $(shell find ./src -name '*.c')
CPP_SRCS := $(shell find ./src -name '*.cpp')
TEST_SRCS := $(shell find ./test -name '*.cpp')
C_OBJS := $(C_SRCS:./src/%.c=./obj/%.o)
CPP_OBJS := $(CPP_SRCS:./src/%.cpp=./obj/%.o)
TEST_OBJS := $(TEST_SRCS:./test/%.cpp=./obj/test/%.o)
OBJS := $(C_OBJS) $(CPP_OBJS)
TEST_APP_OBJS := $(filter-out ./obj/main.o,$(OBJS))

HEADERS := $(shell find ./include ./lib ./test \
	\( -name '*.h' -o -name '*.hpp' \))

.PHONY: all test debug clean fclean re

all: $(TARGET)
$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(OBJS) -o ./bin/$(TARGET) $(LDFLAGS) $(LDLIBS)

test: $(TEST_TARGET)
$(TEST_TARGET): $(TEST_APP_OBJS) $(TEST_OBJS)
	mkdir -p bin
	$(CXX) $(TEST_APP_OBJS) $(TEST_OBJS) -o ./bin/$(TEST_TARGET) $(LDFLAGS) $(LDLIBS)

obj/%.o: src/%.c $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
obj/%.o: src/%.cpp $(HEADERS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
obj/test/%.o: test/%.cpp $(HEADERS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CFLAGS += -g -O0
debug: CXXFLAGS += -g -O0
debug: $(OBJS)
	mkdir -p bin
	$(CXX) $(OBJS) -o ./bin/debug $(LDFLAGS) $(LDLIBS)

clean:
	rm -vf ./bin/*

fclean: clean
	rm -rf ./obj

re: fclean
	$(MAKE) all
