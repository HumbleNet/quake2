sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	webrtc \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	hmac.cpp \
	humblepeer.cpp \
	humblenet_alias.cpp \
	humblenet_core.cpp \
	humblenet_datagram.cpp \
	humblenet_p2p.cpp \
	humblenet_p2p_signalling.cpp \
	humblepeer.cpp \
	libpoll.dpp \
	libsocket.cpp \
	libwebrtc.cpp \
	libwebrtc_asmjs.cpp \
	libwebrtc_dynamic.cpp \
	libwebsockets_native.cpp \
	libwebsockets_asmjs.cpp \
	# empty line


SRC_humblenet:=$(addprefix $(d)/,$(FILES))

ifeq ($(USE_LIBWEBSOCKETS),y)
SRC_humblenet+=$(foreach directory, $(DIRS), $(SRC_$(directory)) ) 
endif  # USE_LIBWEBSOCKETS

d  := $(dirstack_$(sp))
sp := $(basename $(sp))
