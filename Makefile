# Compiler settings
CC_WIN32 = i686-w64-mingw32-gcc
CC_WIN64 = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -O2 -s
LIBS = -lcrypt32

# Output directories
BIN_DIR_WIN32 = bin/win32
BIN_DIR_WIN64 = bin/win64

# Source files
SRC = src/windows_root_cert_helper.c

# Target files
DLL_WIN32 = $(BIN_DIR_WIN32)/windows_root_cert_helper.dll
EXE_WIN32 = $(BIN_DIR_WIN32)/windows_root_cert_helper_test.exe
DLL_WIN64 = $(BIN_DIR_WIN64)/windows_root_cert_helper.dll
EXE_WIN64 = $(BIN_DIR_WIN64)/windows_root_cert_helper_test.exe

# Default target - builds the reusable libraries only.
all: win32 win64

# Build 32-bit library
win32: $(DLL_WIN32)

# Build 64-bit library
win64: $(DLL_WIN64)

# Build test executables. These install/remove a root CA and require admin rights.
test-exe: $(EXE_WIN32) $(EXE_WIN64)

# Create output directories
$(BIN_DIR_WIN32):
	mkdir -p $(BIN_DIR_WIN32)

$(BIN_DIR_WIN64):
	mkdir -p $(BIN_DIR_WIN64)

# Build 32-bit DLL
$(DLL_WIN32): $(SRC) | $(BIN_DIR_WIN32)
	$(CC_WIN32) $(CFLAGS) $(SRC) $(LIBS) -shared -o $@

# Build 32-bit executable
$(EXE_WIN32): $(SRC) | $(BIN_DIR_WIN32)
	$(CC_WIN32) $(CFLAGS) $(SRC) $(LIBS) -DTEST_BIN -o $@

# Build 64-bit DLL
$(DLL_WIN64): $(SRC) | $(BIN_DIR_WIN64)
	$(CC_WIN64) $(CFLAGS) $(SRC) $(LIBS) -shared -o $@

# Build 64-bit executable
$(EXE_WIN64): $(SRC) | $(BIN_DIR_WIN64)
	$(CC_WIN64) $(CFLAGS) $(SRC) $(LIBS) -DTEST_BIN -o $@

# Clean all build output
clean:
	rm -rf $(BIN_DIR_WIN32) $(BIN_DIR_WIN64)

# Rebuild everything
rebuild: clean all

.PHONY: all win32 win64 test-exe clean rebuild 
