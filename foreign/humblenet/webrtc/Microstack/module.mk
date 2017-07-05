sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	ILibAsyncServerSocket.c \
	ILibAsyncSocket.c \
	ILibAsyncUDPSocket.c \
	ILibParsers.c \
	ILibWebClient.c \
	ILibWebRTC.c \
	ILibWrapperWebRTC.c \
	# empty line


SRC_$(d):=$(addprefix $(d)/,$(FILES))


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
