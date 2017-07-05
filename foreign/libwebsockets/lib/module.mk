sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	base64-decode.c \
	client.c \
	client-handshake.c \
	client-parser.c \
	context.c \
	daemonize.c \
	extension.c \
	extension-deflate-frame.c \
	handshake.c \
	libwebsockets.c \
	lws-plat-unix.c \
	lws-plat-win.c \
	output.c \
	parsers.c \
	pollfd.c \
	server.c \
	server-handshake.c \
	service.c \
	sha-1.c \
	ssl.c \
	# empty line


SRC_libwebsockets:=$(addprefix $(d)/,$(FILES))


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
