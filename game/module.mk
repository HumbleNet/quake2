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
	g_turret.c \
	g_utils.c \
	g_weapon.c \
	m_actor.c \
	m_berserk.c \
	m_boss2.c \
	m_boss31.c \
	m_boss32.c \
	m_boss3.c \
	m_brain.c \
	m_chick.c \
	m_flipper.c \
	m_float.c \
	m_flyer.c \
	m_gladiator.c \
	m_gunner.c \
	m_hover.c \
	m_infantry.c \
	m_insane.c \
	m_medic.c \
	m_move.c \
	m_mutant.c \
	m_parasite.c \
	m_soldier.c \
	m_supertank.c \
	m_tank.c \
	p_client.c \
	p_hud.c \
	p_trail.c \
	p_view.c \
	p_weapon.c \
	# empty line


baseq2_MODULES:=baseq2 qshared
baseq2_SRC:=


ifeq ($(BUILTIN_GAME),)
LIBRARIES+= \
	baseq2 \
	#empty line
endif


SRC_baseq2:=$(addprefix $(d)/,$(FILES))
SRC_$(d):=$(addprefix $(d)/,$(FILES))


SRC_qshared:=$(addprefix $(d)/,m_flash.c q_shared.c)


d  := $(dirstack_$(sp))
sp := $(basename $(sp))
