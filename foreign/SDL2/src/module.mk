sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	atomic \
	core \
	cpuinfo \
	events \
	file \
	loadso \
	render \
	stdlib \
	thread \
	timer \
	video \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	SDL_assert.c \
	SDL.c \
	SDL_error.c \
	SDL_hints.c \
	SDL_log.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES)) $(foreach directory, $(DIRS), $(SRC_$(directory)) )


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
