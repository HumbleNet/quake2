# example of local configuration
# copy to local.mk


# location of source
TOPDIR:=..


LTO:=n
ASAN:=n
TSAN:=n
UBSAN:=n


USE_JPEG:=n
USE_HUMBLENET:=y
USE_GLEW:=y
USE_LIBWEBSOCKETS:=y
USE_OPENAL:=n
USE_PNG:=n

STATIC_SDL2:=y

BUILD_SERVER:=y

# compiler options etc
CC:=i686-w64-mingw32-gcc
CXX:=i686-w64-mingw32-g++
CFLAGS:=-DNDEBUG -D__CRT__NO_INLINE=1 -mwindows
CFLAGS+=-D_WIN32_WINNT=0x0600
CFLAGS+=-DWINSOCK2 -DMICROSTACK_NO_STDAFX
#CFLAGS+=-gstabs
CFLAGS+=-w
CFLAGS+=-I$(TOPDIR)/foreign/SDL2/include
OPTFLAGS:=-O2 -mtune=generic -fno-strict-aliasing -ffloat-store


# lazy assignment because CFLAGS is changed later
CXXFLAGS=$(CFLAGS) -std=c++11 -fno-exceptions -fno-rtti


LDFLAGS:=-mwindows
#LDFLAGS+=-gstabs
LDFLAGS+=-static-libstdc++ -static-libgcc
LDLIBS:=-lwinmm -lwsock32
LDLIBS_client:=-ldsound
LDLIBS_libwebsockets:=-lssl -lcrypto -lws2_32 -liphlpapi -lmsvcr100
LDLIBS_ref_gl:=-lopengl32
LDLIBS_sdl2:=-limm32 -lole32 -loleaut32 -luuid -lversion


SOCFLAGS:=


LTOCFLAGS:=-flto
LTOLDFLAGS:=-flto


OBJSUFFIX:=.o
EXESUFFIX:=.exe
SOSUFFIX:=x86.dll
