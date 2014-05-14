sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	cl_cin.c \
	cl_ents.c \
	cl_fx.c \
	cl_input.c \
	cl_inv.c \
	cl_main.c \
	cl_parse.c \
	cl_pred.c \
	cl_tent.c \
	cl_scrn.c \
	cl_view.c \
	cl_newfx.c \
	console.c \
	keys.c \
	le_physics.c \
	le_util.c \
	menu.c \
	qmenu.c \
	snd_dma.c \
	snd_mem.c \
	snd_mix.c \
	vid_menu.c \
	# empty line


quake2_MODULES:=client linux qcommon qshared ref_gl server shlinux shwin win32 zlib
quake2_SRC:=


ifeq ($(USE_JPEG),y)

quake2_MODULES+=libjpeg
CFLAGS+=-DUSE_JPEG

endif


ifeq ($(USE_GLEW),y)

quake2_MODULES+=glew

endif


ifeq ($(USE_PNG),y)

quake2_MODULES+=libpng
CFLAGS+=-DUSE_PNG

endif


ifeq ($(STATIC_SDL2),y)

quake2_MODULES+=sdl2
CFLAGS+=-DDECLSPEC="" -DHAVE_LIBC=1 -DSDLCALL=""

endif  # STATIC_SDL2


ifneq ($(BUILTIN_GAME),)
quake2_MODULES+=$(BUILTIN_GAME)
endif


PROGRAMS+= \
	quake2 \
	#empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES))


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
