.PHONY: default all bindirs clean distclean


.SUFFIXES:

#initialize these
LIBRARIES:=
PROGRAMS:=
ALLSRC:=
# directories which might contain object files
# used for both clean and bindirs
ALLDIRS:=baseq2

default: all


ifeq ($(AFL),y)

CFLAGS+=-DUSE_AFL

endif


ifeq ($(ASAN),y)

OPTFLAGS+=-fsanitize=address
LDFLAGS_asan?=-fsanitize=address
LDFLAGS+=$(LDFLAGS_asan)

endif


ifeq ($(TSAN),y)

CFLAGS+=-DPIC
OPTFLAGS+=-fsanitize=thread -fpic
LDFLAGS_tsan?=-fsanitize=thread -pie
LDFLAGS+=$(LDFLAGS_tsan)

endif


ifeq ($(UBSAN),y)

OPTFLAGS+=-fsanitize=undefined -fno-sanitize-recover
LDFLAGS+=-fsanitize=undefined -fno-sanitize-recover
CXXFLAGS+=-frtti

endif


ifeq ($(LTO),y)

CFLAGS+=$(LTOCFLAGS)
LDFLAGS+=$(LTOLDFLAGS) $(OPTFLAGS)

endif


CFLAGS+=$(OPTFLAGS)

ifeq ($(USE_JPEG),y)
CFLAGS+=-isystem$(TOPDIR)/foreign/libjpeg-turbo
endif


ifeq ($(USE_HUMBLENET),y)
CFLAGS+=-isystem$(TOPDIR)/foreign/humblenet
CFLAGS+=-DUSE_HUMBLENET
endif


ifeq ($(USE_GLEW),y)
CFLAGS+=-isystem$(TOPDIR)/foreign/glew/include
CFLAGS+=-DUSE_GLEW -DGLEW_STATIC -DGLEW_NO_GLU
endif


ifeq ($(USE_LIBWEBSOCKETS),y)
CFLAGS+=-isystem$(TOPDIR)/foreign/libwebsockets/lib
CFLAGS+=-isystem$(TOPDIR)/foreign/libwebsockets/linux
CFLAGS+=-DCMAKE_BUILD=1

CFLAGS+=-isystem$(TOPDIR)/foreign/humblenet/webrtc/core
CFLAGS+=-isystem$(TOPDIR)/foreign/humblenet/webrtc/Microstack
CFLAGS+=-D_POSIX
endif


ifeq ($(USE_OPENAL),y)
CFLAGS+=-DUSE_OPENAL
endif


ifeq ($(USE_PNG),y)
CFLAGS+=-isystem$(TOPDIR)/foreign/libpng
endif


CFLAGS+=-isystem$(TOPDIR)/foreign/zlib


ifneq ($(BUILTIN_GAME),)
CFLAGS+=-DGAME_HARD_LINKED=$(BUILTIN_GAME)
endif


# (call directory-module, dirname)
define directory-module

# save old
DIRS_$1:=$$(DIRS)

dir:=$1
include $(TOPDIR)/$1/module.mk

ALLDIRS+=$1

ALLSRC+=$$(SRC_$1)

# restore saved
DIRS:=$$(DIRS_$1)

endef  # directory-module

DIRS:= \
	client \
	ctf \
	foreign \
	game \
	linux \
	qcommon \
	ref_gl \
	server \
	win32 \
	# empty line
$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


TARGETS:=$(foreach PROG,$(PROGRAMS),$(PROG)$(EXESUFFIX))
TARGETS+=$(foreach LIB,$(LIBRARIES),$(LIB)/game$(SOSUFFIX))

all: $(TARGETS)


# check if a directory needs to be created
# can't use targets with the same name as directory because unfortunate
# interaction with VPATH (targets always exists because source dir)
#  $(call missingdir, progname)
define missingdir

ifneq ($$(shell test -d $1 && echo n),n)
MISSINGDIRS+=$1
endif

endef # missingdir

MISSINGDIRS:=
$(eval $(foreach d, $(ALLDIRS), $(call missingdir,$(d)) ))


# create directories which might contain object files
bindirs:
ifneq ($(MISSINGDIRS),)
	mkdir -p $(MISSINGDIRS)
endif


clean:
	rm -f $(TARGETS) $(foreach dir,$(ALLDIRS),$(dir)/*$(OBJSUFFIX))

distclean: clean
	rm -f $(foreach dir,$(ALLDIRS),$(dir)/*.d)
	-rmdir -p --ignore-fail-on-non-empty $(ALLDIRS)


# rules here

%$(OBJSUFFIX): %.c | bindirs
	$(CC) -c -MF $*.d -MP -MMD -std=gnu99 $(CFLAGS) -o $@ $<


%$(OBJSUFFIX): %.cpp | bindirs
	$(CXX) -c -MF $*.d -MP -MMD $(CXXFLAGS) -o $@ $<


%.sh$(OBJSUFFIX): %.c | bindirs
	$(CC) -c -MF $*.d -MP -MMD -std=gnu99 $(SOCFLAGS) $(CFLAGS) -o $@ $<


# $(call program-target, progname)
define program-target

ALLSRC+=$$(filter %.c,$$($1_SRC))
ALLSRC+=$$(filter %.cpp,$$($1_SRC))

$1_SRC+=$$(foreach module, $$($1_MODULES), $$(SRC_$$(module)))

# if any .cpp files, link with CXX
ifneq (,$$(findstring .cpp,$$($1_SRC)))

LINK:=$(CXX)

else

LINK:=$(CC)

endif

$1_OBJ:=$$($1_SRC:.c=$(OBJSUFFIX))
$1_OBJ:=$$($1_OBJ:.cpp=$(OBJSUFFIX))
$1$(EXESUFFIX): $$($1_OBJ) | bindirs
	$$(LINK) $(LDFLAGS) -o $$@ $$^ $$(foreach module, $$($1_MODULES), $$(LDLIBS_$$(module))) $$($1_LIBS) $(LDLIBS)

endef  # program-target


$(eval $(foreach PROGRAM,$(PROGRAMS), $(call program-target,$(PROGRAM)) ) )


# $(call library-target, progname)
define library-target

ALLSRC+=$$(filter %.c,$$($1_SRC))

$1_SRC+=$$(foreach module, $$($1_MODULES), $$(SRC_$$(module)))
$1_OBJ:=$$($1_SRC:.c=.sh$(OBJSUFFIX))
$1/game$(SOSUFFIX): $$($1_OBJ) | bindirs
	$(CC) $(LDFLAGS) -shared -o $$@ $$^ $$(foreach module, $$($1_MODULES), $$(LDLIBS_$$(module))) $$($1_LIBS) $(LDLIBS)

endef  # library-target


$(eval $(foreach LIBRARY,$(LIBRARIES), $(call library-target,$(LIBRARY)) ) )


-include $(foreach FILE,$(ALLSRC),$(patsubst %.cpp,%.d,$(patsubst %.c,%.d,$(FILE))))
