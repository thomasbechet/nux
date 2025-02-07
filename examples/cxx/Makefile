ifndef WASI_SDK_PATH
$(error WASI SDK required: Install https://github.com/WebAssembly/wasi-sdk and set WASI_SDK_PATH)
endif

DEBUG = 1

CC = "$(WASI_SDK_PATH)/bin/clang" --sysroot="$(WASI_SDK_PATH)/share/wasi-sysroot"

WASM_OPT = wasm-opt
WASM_OPT_FLAGS = -Oz --zero-filled-memory --strip-producers --enable-bulk-memory

CFLAGS = -W -Wall -Wextra -Wno-unused -MMD -MP -fno-exceptions -mbulk-memory -Isrc
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g
else
	CFLAGS += -DNDEBUG -Oz -flto
endif

LDFLAGS = -Wl,--no-entry,--export=start,--export=update,--allow-undefined,--initial-memory=67108864
ifeq ($(DEBUG), 1)
	LDFLAGS += -Wl,--export-all,--no-gc-sections
else
	LDFLAGS += -Wl,--strip-all,--gc-sections,--lto-O3 -Oz
endif

OBJECTS = $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))
DEPS = $(OBJECTS:.o=.d)

ifeq '$(findstring ;,$(PATH))' ';'
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname 2>/dev/null || echo Unknown)
    DETECTED_OS := $(patsubst CYGWIN%,Cygwin,$(DETECTED_OS))
    DETECTED_OS := $(patsubst MSYS%,MSYS,$(DETECTED_OS))
    DETECTED_OS := $(patsubst MINGW%,MSYS,$(DETECTED_OS))
endif

ifeq ($(DETECTED_OS), Windows)
	MKDIR_BUILD = if not exist build md build
	RMDIR = rd /s /q
else
	MKDIR_BUILD = mkdir -p build
	RMDIR = rm -rf
endif

all: build/cart.wasm

build/cart.wasm: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)
ifneq ($(DEBUG), 1)
ifeq (, $(shell command -v $(WASM_OPT)))
	@echo Tip: $(WASM_OPT) was not found. Install it from binaryen for smaller builds!
else
	$(WASM_OPT) $(WASM_OPT_FLAGS) $@ -o $@
endif
endif

build/%.o: src/%.cpp
	@$(MKDIR_BUILD)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	$(RMDIR) build

-include $(DEPS)
