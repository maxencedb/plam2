###############################################################################
# Bins and Flags
CXX=gcc
CXXFLAGS=--std=c++11 -Wall -Wextra
LDFLAGS=-lglpk

RMDIR=rmdir --ignore-fail-on-non-empty
MKDIR=mkdir -p

# Target binaries
BIN=plam
DEBUG_BIN=plam_debug

# Build target directories
BUILD_DIR=_build
RELEASE_DIR=release
DEBUG_DIR=debug

# Source files
SRC_DIR=src
include Makefile.sources

###############################################################################

# Local vars
OBJ=$(addprefix $(BUILD_DIR)/$(RELEASE_DIR)/, $(SRC:.cc=.o))
DEBUG_OBJ=$(addprefix $(BUILD_DIR)/$(DEBUG_DIR)/, $(SRC:.cc=.o))

# Rules
all: releasedirs
all: CFLAGS+=-O3 -Werror
all: $(BIN)

debug: debugdirs
debug: CFLAGS+=-g -ggdb
debug: $(DEBUG_BIN)

$(BIN):	$(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DEBUG_BIN): $(DEBUG_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/$(DEBUG_DIR)/%.o $(BUILD_DIR)/$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CC) $(CFLAGS) -c -o $@ $<

releasedirs:
	$(MKDIR) $(BUILD_DIR)/$(RELEASE_DIR)

debugdirs:
	$(MKDIR) $(BUILD_DIR)/$(DEBUG_DIR)

clean:
	$(RM) $(OBJ)
	$(RM) $(DEBUG_OBJ)
	$(RM) $(BIN)
	$(RM) $(DEBUG_BIN)
	$(RMDIR) $(BUILD_DIR)/$(RELEASE_DIR) > /dev/null 2>&1 || true
	$(RMDIR) $(BUILD_DIR)/$(DEBUG_DIR) > /dev/null 2>&1 || true
	$(RMDIR) $(BUILD_DIR) > /dev/null 2>&1 || true

.PHONY: all debug clean releasedirs debugdirs

#eof
