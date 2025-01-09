CC = /usr/bin/clang 

WASM_OPT = wasm-opt
WASM_OPT_FLAGS = -Oz --zero-filled-memory --strip-producers --enable-bulk-memory

DEBUG = 0

# Compilation flags
CFLAGS = --target=wasm32 -nostdlib -O0
# clang -v -I$NUX_ROOT/include/ --target=wasm32 -nostdlib -O0 -Wl,--strip-all -Wl,--export=start -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined -o $PROJECT.wasm $DIR/$MAIN
ifeq ($(DEBUG), 1)
	# CFLAGS += -DDEBUG -O0 -g
else
	# CFLAGS += -DNDEBUG -Oz -flto
endif

# LDFLAGS = -Wl,-zstack-size=14752,--no-entry,--import-memory \
	-Wl,--initial-memory=65536,--max-memory=65536,--stack-first
LDFLAGS = -z stack-size=8192  --target=wasm32 -nostdlib -Wl,--no-entry -Wl,--strip-all -Wl,--initial-memory=65536
ifeq ($(DEBUG), 1)
	# LDFLAGS += -Wl,--export-all,--no-gc-sections
else
	# LDFLAGS += -Wl,--strip-all,--gc-sections,--lto-O3 -Oz
endif

OBJECTS = $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))
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

build/%.o: src/%.c
	@$(MKDIR_BUILD)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	$(RMDIR) build

-include $(DEPS)
