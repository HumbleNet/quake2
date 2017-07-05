sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	sys_linux.c \
	# empty line


ifeq ($(AFL),y)


FILES+= \
	net_afl.cpp \
	# empty line


else  # USE_AFL


ifneq ($(USE_HUMBLENET),y)

FILES+= \
	net_udp.c \
	# empty line

endif  # USE_HUMBLENET


endif  # USE_AFL


SRC_$(d):=$(addprefix $(d)/,$(FILES))


SRC_client+=$(addprefix $(d)/,snd_linux.c vid_so.c)


ifeq ($(USE_OPENAL),y)
SRC_client+=$(addprefix $(d)/,al_linux.c qal_linux.c)
endif


SRC_shlinux:=$(addprefix $(d)/,q_shlinux.c glob.c)


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
