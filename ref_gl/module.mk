sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	gl_draw.c \
	gl_image.c \
	gl_light.c \
	gl_mesh.c \
	gl_model.c \
	gl_rmain.c \
	gl_rmisc.c \
	gl_rsurf.c \
	gl_warp.c \
	qgl_linux.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES))


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
