sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	SDL_windowsclipboard.c \
	SDL_windowsevents.c \
	SDL_windowsframebuffer.c \
	SDL_windowskeyboard.c \
	SDL_windowsmessagebox.c \
	SDL_windowsmodes.c \
	SDL_windowsmouse.c \
	SDL_windowsopengl.c \
	SDL_windowsopengles.c \
	SDL_windowsshape.c \
	SDL_windowsvideo.c \
	SDL_windowswindow.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES)) $(foreach directory, $(DIRS), $(SRC_$(directory)) )


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
