sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	dummy \
	emscripten \
	windows \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	SDL_blit_0.c \
	SDL_blit_1.c \
	SDL_blit_A.c \
	SDL_blit_auto.c \
	SDL_blit.c \
	SDL_blit_copy.c \
	SDL_blit_N.c \
	SDL_blit_slow.c \
	SDL_bmp.c \
	SDL_clipboard.c \
	SDL_egl.c \
	SDL_fillrect.c \
	SDL_pixels.c \
	SDL_rect.c \
	SDL_RLEaccel.c \
	SDL_shape.c \
	SDL_stretch.c \
	SDL_surface.c \
	SDL_video.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES)) $(foreach directory, $(DIRS), $(SRC_$(directory)) )


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
