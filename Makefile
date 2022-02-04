# only tested/running on Linux
NAME       		:= oom

CC				:= g++
CPPLAGS			:= -v -g -Wall
INC				:= -Tinc
LDFLAGS			:= -v -g

BUILD_DIR  		:= artifacts
TARGET     		:= $(BUILD_DIR)/$(NAME)
TEMP_DIR		:= tmp
SRC_DIR			:= src

_SRCS = main.cpp dummydatagen.cpp snapshot.cpp dump.cpp db.cpp
_OBJS = $(subst .cpp,.o,$(_SRCS))
OBJS = $(patsubst %,$(BUILD_DIR)/%,$(_OBJS))
LIBS = sqlite3
LDLIBS = $(patsubst %,-l %,$(LIBS))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPLAGS) -c $(INC) -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(LDLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

all: $(TARGET)

test: $(TARGET) clean-test
	mkdir -p $(TEMP_DIR)
	export TMPDIR=$(TEMP_DIR)
	echo "WARNING: running several minutes ..."
	bash bin/runtest.sh

.PHONY: clean

clean: clean-test
	rm -rf $(BUILD_DIR)

clean-test:
	rm -rf $(TEMP_DIR)

run: $(TARGET)
	artifacts/oom
