sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	cmd.c \
	cmodel.c \
	common.c \
	crc.c \
	cvar.c \
	files.c \
	ioapi.c \
	md4.c \
	mersennetwister.c \
	net_chan.c \
	pmove.c \
	redblack.c \
	unzip.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES))


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
