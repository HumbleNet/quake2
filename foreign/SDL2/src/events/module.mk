sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	SDL_clipboardevents.c \
	SDL_dropevents.c \
	SDL_events.c \
	SDL_gesture.c \
	SDL_keyboard.c \
	SDL_mouse.c \
	SDL_quit.c \
	SDL_touch.c \
	SDL_windowevents.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES)) $(foreach directory, $(DIRS), $(SRC_$(directory)) )


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
