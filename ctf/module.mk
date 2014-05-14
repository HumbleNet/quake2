sp             := $(sp).x
dirstack_$(sp) := $(d)
d              := $(dir)


SUBDIRS:= \
	# empty line

DIRS:=$(addprefix $(d)/,$(SUBDIRS))

$(eval $(foreach directory, $(DIRS), $(call directory-module,$(directory)) ))


FILES:= \
	g_ai.c \
	g_chase.c \
	g_cmds.c \
	g_combat.c \
	g_ctf.c \
	g_func.c \
	g_items.c \
	g_main.c \
	g_misc.c \
	g_monster.c \
	g_phys.c \
	g_save.c \
	g_spawn.c \
	g_svcmds.c \
	g_target.c \
	g_trigger.c \
	g_utils.c \
	g_weapon.c \
	m_move.c \
	p_client.c \
	p_hud.c \
	p_menu.c \
	p_trail.c \
	p_view.c \
	p_weapon.c \
	# empty line


ctf_MODULES:=ctf qshared
ctf_SRC:=


ifeq ($(BUILTIN_GAME),)
LIBRARIES+= \
	ctf \
	#empty line
endif


SRC_ctf:=$(addprefix $(d)/,$(FILES))
SRC_$(d):=$(addprefix $(d)/,$(FILES))


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
