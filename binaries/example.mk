# example of local configuration
# copy to local.mk


# location of source
TOPDIR:=..


LTO:=n
ASAN:=n
TSAN:=n
UBSAN:=n
AFL:=n


USE_JPEG:=y
USE_GLEW:=y
USE_OPENAL:=y
USE_PNG:=y

BUILD_SERVER:=y


# compiler options etc
CC:=gcc
CXX:=g++
CFLAGS:=-g -DNDEBUG -DLINUX
CFLAGS+=-Wall -Wextra
CFLAGS+=-Wno-sign-compare -Wno-unused-parameter
CFLAGS+=$(shell sdl2-config --cflags)
CFLAGS+=$(shell pkg-config openal --cflags)
OPTFLAGS:=-O2 -mtune=generic -fno-strict-aliasing -ffloat-store


# lazy assignment because CFLAGS is changed later
CXXFLAGS=$(CFLAGS) -std=c++11 -fno-exceptions -fno-rtti


LDFLAGS:=-g
LDLIBS:=-lm -ldl
LDLIBS_ref_gl:=-lGL $(shell sdl2-config --libs)
LDLIBS_client:=$(shell pkg-config openal --libs)


SOCFLAGS:=-fPIC -DPIC


LTOCFLAGS:=-flto -fuse-linker-plugin -fno-fat-lto-objects
LTOLDFLAGS:=-flto -fuse-linker-plugin


OBJSUFFIX:=.o
EXESUFFIX:=-bin
SOSUFFIX:=.so
