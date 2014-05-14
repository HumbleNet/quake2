sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	sys_win.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES))


SRC_client+=$(addprefix $(d)/,snd_win.c)


ifeq ($(USE_OPENAL),y)
SRC_client+=$(addprefix $(d)/,alw_win.c)
endif


SRC_shwin:=$(addprefix $(d)/,conproc.c net_wins.c q_shwin.c sys_win.c)


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
