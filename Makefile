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

# ported from sqlite3.go
CGO_FLAGS = -DSQLITE_ENABLE_RTREE -DSQLITE_THREADSAFE=0 -DHAVE_USLEEP=1 -DSQLITE_ENABLE_FTS3 \
            -DSQLITE_ENABLE_FTS3_PARENTHESIS -DSQLITE_TRACE_SIZE_LIMIT=15 -DSQLITE_OMIT_DEPRECATED \
            -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 -DSQLITE_ENABLE_UPDATE_DELETE_LIMIT

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPLAGS) $(CGO_FLAGS) -c $(INC) -o $@ $<

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
