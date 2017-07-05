# example of local configuration
# copy to local.mk


# location of source
TOPDIR:=..


LTO:=y
ASAN:=n
TSAN:=n
UBSAN:=n


USE_JPEG:=n
USE_HUMBLENET:=y
USE_OPENAL:=y
USE_PNG:=n

BUILD_SERVER:=n

STATIC_SDL2:=n

BUILTIN_GAME:=baseq2

# compiler options etc
CC:=emcc
CXX:=em++
CFLAGS:=-g -DNDEBUG -DLINUX -D_GNU_SOURCE=1
CFLAGS+=-I$(TOPDIR)/foreign/SDL2/include
OPTFLAGS:=-O3


# lazy assignment because CFLAGS is changed later
CXXFLAGS=$(CFLAGS) -std=c++11 -fno-exceptions -fno-rtti


LDFLAGS:=-g --preload-file baseq2
LDFLAGS+=-s TOTAL_MEMORY=268435456
LDFLAGS+=-s OUTLINING_LIMIT=5000
LDLIBS:=
LDLIBS_ref_gl:=

ifeq ($(STATIC_SDL2),y)

LDLIBS_client:=

else  # STATIC_SDL2

LDLIBS_client:=$(TOPDIR)/foreign/SDL2/build/.libs/libSDL2.a

endif  # STATIC_SDL2


#ifeq ($(USE_HUMBLENET),y)
#LDFLAGS+=--js-library $(TOPDIR)/foreign/humblenet/library_humblenet.js
#endif  # USE_HUMBLENET


LTOCFLAGS:=--llvm-lto 3
LTOLDFLAGS:=--llvm-lto 3


OBJSUFFIX:=.o
EXESUFFIX:=.js
