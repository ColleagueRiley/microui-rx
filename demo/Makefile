CC ?= gcc
AR ?= ar

# used for compiling RGFW.o
CUSTOM_CFLAGS =
# used for the examples
CFLAGS =
CFLAGS += -g3

ifdef RGFW_C89
	CUSTOM_CFLAGS += -std=c89 -Wno-declaration-after-statement -Wall -Wextra -Wpedantic
endif

LIBS := -static -lgdi32
LINK_GL1 = -lopengl32
EXT = .exe
LIB_EXT = .dll

ifneq ($(CC),zig cc)
	LIBS += -D _WIN32_WINNT=0x0501
endif

OS_DIR = \\

detected_OS = windows

OBJ_FILE = .o

ifeq ($(WAYLAND_ONLY), 1)
	WAYLAND = 1
endif

ifeq ($(WAYLAND_X11), 1)
	WAYLAND = 1
endif

# not using a cross compiler
ifeq (,$(filter $(CC),x86_64-w64-mingw32-gcc i686-w64-mingw32-gcc x86_64-w64-mingw32-g++ /opt/msvc/bin/x64/cl.exe /opt/msvc/bin/x86/cl.exe))
	detected_OS := $(shell uname 2>/dev/null || echo Unknown)

	ifeq ($(detected_OS),Darwin)        # Mac OS X
		DX11_LIBS =
		LIBS := -framework CoreVideo -framework Cocoa -framework OpenGL -framework IOKit
		LINK_GL1 = -framework OpenGL
		EXT =
		LIB_EXT = .dylib
		OS_DIR = /
	endif
	ifeq ($(detected_OS),Linux)
		DX11_LIBS =
		LINK_GL1 = -lGL

		ifneq ($(WAYLAND_ONLY), 1)
			LIBS = -lXrandr -lX11 -ldl -lpthread -lm
		else
			LIBS =
		endif

		EXT =
		LIB_EXT = .so
		OS_DIR = /
	endif
	ifeq ($(detected_OS),NetBSD)
		DX11_LIBS =
		LINK_GL1 = -lGL
		CUSTOM_CFLAGS += -I/usr/pkg/include -I/usr/X11R7/include -Wl,-R/usr/pkg/lib -Wl,-R/usr/X11R7/lib -L/usr/pkg/lib -L/usr/X11R7/lib
		LIBS := $(CUSTOM_CFLAGS) -lXrandr -lX11 -lpthread -lm
		EXT =
		LIB_EXT = .so
		OS_DIR = /
	endif

	ifeq (,$(filter $(detected_OS),Linux Darwin NetBSD))
		detected_OS := windows
	endif
else
	OS_DIR = /
endif

ifeq ($(WAYLAND),1)
	WAYLAND_SOURCE = $(shell make -f wayland.mk sources -s)

	LIBS += -D RGFW_WAYLAND $(WAYLAND_SOURCE) -lwayland-cursor -lwayland-client -lxkbcommon  -lwayland-egl -lEGL -lm
	LINK_GL1 = -lEGL -lGL

	# LIBS += -ldecor-0

	ifeq ($(WAYLAND_X11), 1)
		LIBS += -D RGFW_X11
	endif
endif

LINK_GL3 =
LINK_GL2 =
LINK_OSMESA =

ifneq (,$(filter $(CC),cl /opt/msvc/bin/x64/cl.exe /opt/msvc/bin/x86/cl.exe))
	WARNINGS = -Wall -wd4668 -wd4820 -wd5045
	LIBS =
	DX11_LIBS =
	OBJ_FILE = .obj
else ifneq (,$(filter $(CC),emcc em++))
	DX11_LIBS =
	LINK_GL1 = -s LEGACY_GL_EMULATION -D LEGACY_GL_EMULATION -sGL_UNSAFE_OPTS=0
	LINK_GL3 = -s FULL_ES3 -s USE_WEBGL2
	LINK_GL2 = -s FULL_ES2 -s USE_WEBGL2
	LINK_OSMESA = -sALLOW_MEMORY_GROWTH
	EXPORTED_JS = -s EXPORTED_RUNTIME_METHODS="['stringToNewUTF8']"
	LIBS = -s WASM=1 -s ASYNCIFY -s GL_SUPPORT_EXPLICIT_SWAP_CONTROL=1 $(EXPORTED_JS)
	EXT = .js
	detected_OS = web
	DX11_LIBS =
else ifeq (,$(filter $(CC),g++ clang++ em++))
	CFLAGS += -std=c99
	WARNINGS = -Wall -Werror -Wdouble-promotion -Wmissing-prototypes -Wextra -Wstrict-prototypes -Wold-style-definition -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wpointer-arith -Wvla -Wcast-align -Wstrict-overflow -Wnested-externs -Wstrict-aliasing -Wredundant-decls -Winit-self -Wmissing-noreturn
else
	WARNINGS = -Wall -Werror -Wdouble-promotion -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wpointer-arith -Wvla -Wcast-align -Wstrict-overflow -Wstrict-aliasing -Wredundant-decls -Winit-self -Wmissing-noreturn

	ifeq ($(detected_OS),Darwin)
		WARNINGS += -Wno-deprecated -Wno-unknown-warning-option -Wno-pedantic
	endif
endif

ifeq ($(WAYLAND_X11), 1)
	# lazy fix (no -Wmissing-prototypes for wayland X11)
	WARNINGS = -Wall -Werror -Wdouble-promotion -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wpointer-arith -Wvla -Wcast-align -Wstrict-overflow -Wstrict-aliasing -Wredundant-decls -Winit-self -Wmissing-noreturn
endif

TEST_OUTPUTS = \
			   tests/loop \


all: demo

demo: main.c RGFW.h $(WAYLAND_SOURCE)
ifeq (,$(filter $(CC),emcc em++ g++ clang++))
	$(CC) $(CFLAGS) -I. -I../src $< $(LINK_GL1) $(LIBS) -o $@$(EXT)
else
	$(CC) $(CFLAGS) -I. -I../src $< -s USE_WEBGL2 $(LIBS) $(LINK_GL1) -o $@$(EXT)
endif

debug: all
	./demo
	$(MAKE) clean

ifeq ($(WAYLAND),1)

$(WAYLAND_SOURCE): %.c:
	$(MAKE) -f wayland.mk

endif


clean:
	rm -f *.o *.obj demo

ifeq ($(WAYLAND),1)
	$(MAKE) -f wayland.mk clean
endif


.PHONY: all examples clean

