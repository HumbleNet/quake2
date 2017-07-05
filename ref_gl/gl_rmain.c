/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// r_main.c


#ifdef _MSC_VER
#include <process.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_local.h"

#include "../client/client.h"
#include "../linux/glw.h"

#include "../client/keys.h"

#include <SDL.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <html5.h>
#endif  // EMSCRIPTEN

#ifndef WIN32
#define __stdcall
#endif

//long (*Q_ftol)(float f);

void R_Clear (void);

refimport_t		ri;

model_t		*r_worldmodel;

double		gldepthmin, gldepthmax;

glconfig_t gl_config;
glstate_t  gl_state;

image_t		*r_notexture;		// use for bad textures
image_t		*r_particletexture;	// little dot for particles

entity_t	*currententity;
model_t		*currentmodel;

cplane_t	frustum[4];

int			r_visframecount;	// bumped when going to a new PVS
int			r_framecount;		// used for dlight push checking

int			c_brush_polys, c_alias_polys;

float		v_blend[4];			// final blending color

void GL_Strings_f( void );

//
// view origin
//
vec3_t	vup;
vec3_t	vpn;
vec3_t	vright;
vec3_t	r_origin;

float	r_world_matrix[16];
float	r_base_world_matrix[16];

//
// screen size info
//
refdef_t	r_newrefdef;

int		r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

cvar_t	*r_norefresh;
cvar_t	*r_drawentities;
cvar_t	*r_drawworld;
cvar_t	*r_speeds;
cvar_t	*r_fullbright;
cvar_t	*r_novis;
cvar_t	*r_nocull;
cvar_t	*r_lerpmodels;
cvar_t	*r_lefthand;

cvar_t	*r_lightlevel;	// FIXME: This is a HACK to get the client's light level

//cvar_t	*gl_nosubimage;

cvar_t	*gl_particle_min_size;
cvar_t	*gl_particle_max_size;
cvar_t	*gl_particle_size;
cvar_t	*gl_particle_att_a;
cvar_t	*gl_particle_att_b;
cvar_t	*gl_particle_att_c;

//cvar_t	*gl_ext_swapinterval;
cvar_t	*gl_ext_multitexture;
cvar_t	*gl_ext_pointparameters;
//cvar_t	*gl_ext_compiled_vertex_array;

//r1ch: my extensions
//cvar_t	*gl_ext_generate_mipmap;
cvar_t	*gl_ext_point_sprite;
cvar_t	*gl_ext_texture_filter_anisotropic;
cvar_t	*gl_ext_texture_non_power_of_two;
cvar_t	*gl_ext_max_anisotropy;

cvar_t	*gl_colorbits;
cvar_t	*gl_alphabits;
cvar_t	*gl_depthbits;
cvar_t	*gl_stencilbits;

cvar_t	*gl_ext_multisample;
cvar_t	*gl_ext_samples;

cvar_t	*gl_zfar;

cvar_t	*cl_version;
cvar_t	*gl_r1gl_test;
cvar_t	*gl_doublelight_entities;
cvar_t	*gl_noscrap;
cvar_t	*gl_overbrights;
cvar_t	*gl_linear_mipmaps;

cvar_t	*vid_gamma_pics;

cvar_t	*vid_width;
cvar_t	*vid_height;

cvar_t	*vid_topmost;

cvar_t	*gl_bitdepth;
//cvar_t	*gl_lightmap;
cvar_t	*gl_shadows;
cvar_t	*gl_dynamic;
//cvar_t  *gl_monolightmap;
cvar_t	*gl_modulate;
cvar_t	*gl_nobind;
cvar_t	*gl_round_down;
cvar_t	*gl_picmip;
cvar_t	*gl_skymip;
cvar_t	*gl_showtris;
cvar_t	*gl_clear;
cvar_t	*gl_cull;
cvar_t	*gl_polyblend;
cvar_t	*gl_flashblend;
//cvar_t	*gl_playermip;
//cvar_t  *gl_saturatelighting;
cvar_t	*gl_swapinterval;
cvar_t	*gl_texturemode;
cvar_t	*gl_lockpvs;
cvar_t	*gl_jpg_quality;
cvar_t	*gl_coloredlightmaps;

//cvar_t	*gl_3dlabs_broken;

cvar_t	*vid_fullscreen;
cvar_t	*vid_gamma;
cvar_t	*vid_forcedrefresh;
cvar_t	*vid_optimalrefresh;
cvar_t	*vid_nowgl;
cvar_t	*vid_restore_on_switch;

cvar_t	*gl_texture_formats;
cvar_t	*gl_pic_formats;

cvar_t	*gl_dlight_falloff;
cvar_t	*gl_alphaskins;

cvar_t	*gl_pic_scale;

//cvar_t	*con_alpha;

vec4_t	colorWhite = {1,1,1,1};

qboolean load_png_pics = true;
qboolean load_tga_pics = true;
qboolean load_jpg_pics = true;

qboolean load_png_wals = true;
qboolean load_tga_wals = true;
qboolean load_jpg_wals = true;

extern cvar_t		*gl_contrast;


#define JOY_AXIS_X			0
#define JOY_AXIS_Y			1
#define JOY_AXIS_Z			2
#define JOY_AXIS_R			3
#define JOY_AXIS_U			4
#define JOY_AXIS_V			5

static qboolean                 X11_active = false;


static SDL_Window *window;
static SDL_GLContext glcontext;


struct
{
	int key;
	int down;
} keyq[64];
int keyq_head=0;
int keyq_tail=0;

glwstate_t glw_state;
						      
// Console variables that we need to access from this module

/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

// this is inside the renderer shared lib, so these are called from vid_so

static qboolean        mouse_avail;
static int     mouse_buttonstate;
static int     mouse_oldbuttonstate;
static int   mouse_x, mouse_y;
static int	old_mouse_x, old_mouse_y;
static int		mx, my;
static float old_windowed_mouse;
static qboolean mouse_active;

static cvar_t	*_windowed_mouse;
static cvar_t	*m_filter;
static cvar_t	*in_mouse;

static qboolean	mlooking = true;

/* stencilbuffer shadows */
qboolean have_stencil = false;

// state struct passed in Init
static in_state_t	*in_state;

cvar_t *sensitivity;
static cvar_t *my_lookstrafe;
cvar_t *m_side;
cvar_t *m_yaw;
cvar_t *m_pitch;
cvar_t *m_forward;
static cvar_t *my_freelook;

#ifdef HAVE_JOYSTICK
/************************
 * Joystick
 ************************/
static cvar_t   *in_joystick;
static cvar_t   *j_invert_y;
static qboolean joystick_avail;
static SDL_Joystick *joy;
static int joy_oldbuttonstate;
static int joy_numbuttons;
static int jx, jy, jt;
static int lr_axis, ud_axis, throttle_axis;
#endif /* HAVE_JOYSTICK */


/*
=================
R_CullBox

Returns true if the box is completely outside the frustom
=================
*/
qboolean R_CullBox (vec3_t mins, vec3_t maxs)
{
	int		i;

	if (r_nocull->intvalue)
		return false;

	for (i=0 ; i<4 ; i++)
		if (BOX_ON_PLANE_SIDE(mins, maxs, &frustum[i]) == 2)
			return true;
	return false;
}


void R_RotateForEntity (entity_t *e)
{
    qglTranslatef (e->origin[0],  e->origin[1],  e->origin[2]);

    qglRotatef (e->angles[1],  0, 0, 1);
    qglRotatef (-e->angles[0],  0, 1, 0);
    qglRotatef (-e->angles[2],  1, 0, 0);
}

/*
=============================================================

  SPRITE MODELS

=============================================================
*/

/*
=================
R_DrawSpriteModel

=================
*/
void R_DrawSpriteModel (entity_t *e)
{
	float alpha = 1.0F;
	vec3_t	point;
	dsprframe_t	*frame;
	float		*up, *right;
	dsprite_t		*psprite;

	// don't even bother culling, because it's just a single
	// polygon without a surface cache

	psprite = (dsprite_t *)currentmodel->extradata;

#if 0
	if (e->frame < 0 || e->frame >= psprite->numframes)
	{
		VID_Printf (PRINT_ALL, "no such sprite frame %i\n", e->frame);
		e->frame = 0;
	}
#endif
	e->frame %= psprite->numframes;

	frame = &psprite->frames[e->frame];

#if 0
	if (psprite->type == SPR_ORIENTED)
	{	// bullet marks on walls
	vec3_t		v_forward, v_right, v_up;

	AngleVectors (currententity->angles, v_forward, v_right, v_up);
		up = v_up;
		right = v_right;
	}
	else
#endif
	{	// normal sprite
		up = vup;
		right = vright;
	}

	if ( e->flags & RF_TRANSLUCENT )
		alpha = e->alpha;

	if ( alpha != 1.0F )
		qglEnable( GL_BLEND );

	qglColor4f( 1, 1, 1, alpha );

    GL_MBind(GL_TEXTURE0, currentmodel->skins[e->frame]->texnum);

	GL_TexEnv(GL_TEXTURE0, GL_MODULATE);

	if ( alpha == 1.0 )
		qglEnable (GL_ALPHA_TEST);
	else
		qglDisable( GL_ALPHA_TEST );

	qglBegin (GL_TRIANGLES);

	qglMTexCoord2f(GL_TEXTURE0, 0, 1);
	VectorMA (e->origin, -frame->origin_y, up, point);
	VectorMA (point, -frame->origin_x, right, point);
	qglVertex3f(point[0], point[1], point[2]);

	qglMTexCoord2f(GL_TEXTURE0, 0, 0);
	VectorMA (e->origin, frame->height - frame->origin_y, up, point);
	VectorMA (point, -frame->origin_x, right, point);
	qglVertex3f(point[0], point[1], point[2]);

	qglMTexCoord2f(GL_TEXTURE0, 1, 0);
	VectorMA (e->origin, frame->height - frame->origin_y, up, point);
	VectorMA (point, frame->width - frame->origin_x, right, point);
	qglVertex3f(point[0], point[1], point[2]);

	qglMTexCoord2f(GL_TEXTURE0, 0, 1);
	VectorMA (e->origin, -frame->origin_y, up, point);
	VectorMA (point, -frame->origin_x, right, point);
	qglVertex3f(point[0], point[1], point[2]);

	qglMTexCoord2f(GL_TEXTURE0, 1, 0);
	VectorMA (e->origin, frame->height - frame->origin_y, up, point);
	VectorMA (point, frame->width - frame->origin_x, right, point);
	qglVertex3f(point[0], point[1], point[2]);

	qglMTexCoord2f(GL_TEXTURE0, 1, 1);
	VectorMA (e->origin, -frame->origin_y, up, point);
	VectorMA (point, frame->width - frame->origin_x, right, point);
	qglVertex3f(point[0], point[1], point[2]);
	
	qglEnd ();

	qglDisable (GL_ALPHA_TEST);
	GL_TexEnv(GL_TEXTURE0, GL_REPLACE);

	if ( alpha != 1.0F )
		qglDisable( GL_BLEND );

	qglColor4f(colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]);
}

//==================================================================================

/*
=============
R_DrawNullModel
=============
*/
void R_DrawNullModel (void)
{
	vec3_t	shadelight;
	int		i;

	if ( currententity->flags & RF_FULLBRIGHT )
		shadelight[0] = shadelight[1] = shadelight[2] = 1.0F;
	else
		R_LightPoint (currententity->origin, shadelight);

    qglPushMatrix ();
	R_RotateForEntity (currententity);

	qglDisable (GL_TEXTURE_2D);
	qglColor3f(shadelight[0], shadelight[1], shadelight[2]);

	qglBegin (GL_TRIANGLE_FAN);
	qglVertex3f (0, 0, -16);
	for (i=0 ; i<=4 ; i++)
		qglVertex3f (16*(float)cos(i*M_PI_DIV_2), 16*(float)sin(i*M_PI_DIV_2), 0);
	qglEnd ();

	qglBegin (GL_TRIANGLE_FAN);
	qglVertex3f (0, 0, 16);
	for (i=4 ; i>=0 ; i--)
		qglVertex3f (16*(float)cos(i*M_PI_DIV_2), 16*(float)sin(i*M_PI_DIV_2), 0);
	qglEnd ();

	qglColor3f (1,1,1);
	qglPopMatrix ();
	qglEnable (GL_TEXTURE_2D);
}

int visibleBits[MAX_ENTITIES];


/*
=============
R_DrawEntitiesOnList
=============
*/
void R_DrawEntitiesOnList (void)
{
	int		i;

	if (!r_drawentities->intvalue)
		return;

	// draw non-transparent first
	for (i=0 ; i<r_newrefdef.num_entities ; i++)
	{
		currententity = &r_newrefdef.entities[i];

		if (currententity->flags & RF_TRANSLUCENT || (gl_alphaskins->intvalue && currententity->skin && currententity->skin->has_alpha))
			continue;	// solid

		if ( currententity->flags & RF_BEAM )
		{
			R_DrawBeam( currententity );
		}
		else
		{
			currentmodel = currententity->model;
			if (!currentmodel)
			{
				R_DrawNullModel ();
				continue;
			}

			switch (currentmodel->type)
			{
				case mod_alias:
					R_DrawAliasModel (currententity);
					break;
				case mod_brush:
					R_DrawBrushModel (currententity);
					break;
				case mod_sprite:
					R_DrawSpriteModel (currententity);
					break;
				default:
					ri.Sys_Error (ERR_DROP, "Bad modeltype %d on %s", currentmodel->type, currentmodel->name);
					break;
			}
		}
	}

	// draw transparent entities
	// we could sort these if it ever becomes a problem...
	glDepthMask (0);		// no z writes
	for (i=0 ; i<r_newrefdef.num_entities ; i++)
	{
		currententity = &r_newrefdef.entities[i];
		if (!(currententity->flags & RF_TRANSLUCENT || (gl_alphaskins->intvalue && currententity->skin && currententity->skin->has_alpha)))
			continue;	// solid

		if ( currententity->flags & RF_BEAM )
		{
			R_DrawBeam( currententity );
		}
		else
		{
			currentmodel = currententity->model;

			if (!currentmodel)
			{
				R_DrawNullModel ();
				continue;
			}
			switch (currentmodel->type)
			{
			case mod_alias:
				R_DrawAliasModel (currententity);
				break;
			case mod_brush:
				R_DrawBrushModel (currententity);
				break;
			case mod_sprite:
				R_DrawSpriteModel (currententity);
				break;
			default:
				ri.Sys_Error (ERR_DROP, "Bad modeltype %d on %s", currentmodel->type, currentmodel->name);
				break;
			}
		}
	}
	glDepthMask (1);		// back to writing

}

/*
** GL_DrawParticles
**
*/
void GL_DrawParticles( int num_particles, const particle_t particles[])
{
	const particle_t *p;
	int				i;
	vec3_t			up, right;
	float			scale;
	//byte			color[4];
	vec4_t			colorf;

    GL_MBind(GL_TEXTURE0, r_particletexture->texnum);
	glDepthMask( GL_FALSE );		// no z buffering
	qglEnable( GL_BLEND );
	GL_TexEnv(GL_TEXTURE0, GL_MODULATE);
	qglBegin( GL_TRIANGLES );

	VectorScale (vup, 1.5f, up);
	VectorScale (vright, 1.5f, right);

	for ( p = particles, i=0 ; i < num_particles ; i++,p++)
	{
		// hack a scale up to keep particles from disapearing
		scale = ( p->origin[0] - r_origin[0] ) * vpn[0] + 
			    ( p->origin[1] - r_origin[1] ) * vpn[1] +
			    ( p->origin[2] - r_origin[2] ) * vpn[2];

		if (scale < 20)
			scale = 1;
		else
			scale = 1 + scale * 0.004f;

		//*(int *)color = colortable[p->color];
		//color[3] = (byte)Q_ftol(p->alpha*255);

		FastVectorCopy (d_8to24float[p->color], colorf);
		colorf[3] = p->alpha;

		qglColor4f(colorf[0], colorf[1], colorf[2], colorf[3]);

		qglMTexCoord2f( GL_TEXTURE0, 0.0625f, 0.0625f );
		qglVertex3f(p->origin[0], p->origin[1], p->origin[2]);

		qglMTexCoord2f( GL_TEXTURE0, 1.0625f, 0.0625f );
		qglVertex3f( p->origin[0] + up[0]*scale, 
			         p->origin[1] + up[1]*scale, 
					 p->origin[2] + up[2]*scale);

		qglMTexCoord2f( GL_TEXTURE0, 0.0625f, 1.0625f );
		qglVertex3f( p->origin[0] + right[0]*scale, 
			         p->origin[1] + right[1]*scale, 
					 p->origin[2] + right[2]*scale);
	}

	qglEnd ();
	qglDisable( GL_BLEND );
	qglColor4f(colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]);
	glDepthMask( 1 );		// back to normal Z buffering
	GL_TexEnv(GL_TEXTURE0, GL_REPLACE);
}

/*
===============
R_DrawParticles
===============
*/
void R_DrawParticles (void)
{
		GL_DrawParticles( r_newrefdef.num_particles, r_newrefdef.particles );
}

/*
============
R_PolyBlend
============
*/
void R_PolyBlend (void)
{
	if (!gl_polyblend->intvalue)
		return;

	if (FLOAT_EQ_ZERO(v_blend[3]))
		return;

	qglDisable (GL_ALPHA_TEST);
	qglEnable (GL_BLEND);
	qglDisable (GL_DEPTH_TEST);
	qglDisable (GL_TEXTURE_2D);

    qglLoadIdentity ();

	// FIXME: get rid of these
    qglRotatef (-90,  1, 0, 0);	    // put Z going up
    qglRotatef (90,  0, 0, 1);	    // put Z going up

	qglColor4f(v_blend[0], v_blend[1], v_blend[2], v_blend[3]);

	qglBegin (GL_TRIANGLES);

	qglVertex3f (10, 100, 100);
	qglVertex3f (10, -100, 100);
	qglVertex3f (10, -100, -100);

	qglVertex3f (10, 100, 100);
	qglVertex3f (10, -100, -100);
	qglVertex3f (10, 100, -100);
	qglEnd ();

	qglDisable (GL_BLEND);
	qglEnable (GL_TEXTURE_2D);
	qglEnable (GL_ALPHA_TEST);

	qglColor4f(colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]);
}

//=======================================================================

int SignbitsForPlane (cplane_t *out)
{
	int	bits, j;

	// for fast box on planeside test

	bits = 0;
	for (j=0 ; j<3 ; j++)
	{
		if (FLOAT_LT_ZERO(out->normal[j]))
			bits |= 1<<j;
	}
	return bits;
}


void R_SetFrustum (void)
{
	int		i;

#if 0
	/*
	** this code is wrong, since it presume a 90 degree FOV both in the
	** horizontal and vertical plane
	*/
	// front side is visible
	VectorAdd (vpn, vright, frustum[0].normal);
	VectorSubtract (vpn, vright, frustum[1].normal);
	VectorAdd (vpn, vup, frustum[2].normal);
	VectorSubtract (vpn, vup, frustum[3].normal);

	// we theoretically don't need to normalize these vectors, but I do it
	// anyway so that debugging is a little easier
	VectorNormalize( frustum[0].normal );
	VectorNormalize( frustum[1].normal );
	VectorNormalize( frustum[2].normal );
	VectorNormalize( frustum[3].normal );
#else
	// rotate VPN right by FOV_X/2 degrees
	RotatePointAroundVector( frustum[0].normal, vup, vpn, -(90-r_newrefdef.fov_x / 2 ) );
	// rotate VPN left by FOV_X/2 degrees
	RotatePointAroundVector( frustum[1].normal, vup, vpn, 90-r_newrefdef.fov_x / 2 );
	// rotate VPN up by FOV_X/2 degrees
	RotatePointAroundVector( frustum[2].normal, vright, vpn, 90-r_newrefdef.fov_y / 2 );
	// rotate VPN down by FOV_X/2 degrees
	RotatePointAroundVector( frustum[3].normal, vright, vpn, -( 90 - r_newrefdef.fov_y / 2 ) );
#endif

	for (i=0 ; i<4 ; i++)
	{
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = DotProduct (r_origin, frustum[i].normal);
		frustum[i].signbits = SignbitsForPlane (&frustum[i]);
	}
}

//=======================================================================

/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame (void)
{
	mleaf_t	*leaf;

	r_framecount++;

// build the transformation matrix for the given view angles
	FastVectorCopy (r_newrefdef.vieworg, r_origin);

	AngleVectors (r_newrefdef.viewangles, vpn, vright, vup);

// current viewcluster
	if ( !( r_newrefdef.rdflags & RDF_NOWORLDMODEL ) )
	{
		r_oldviewcluster = r_viewcluster;
		r_oldviewcluster2 = r_viewcluster2;
		leaf = Mod_PointInLeaf (r_origin, r_worldmodel);
		r_viewcluster = r_viewcluster2 = leaf->cluster;

		// check above and below so crossing solid water doesn't draw wrong
		if (!leaf->contents)
		{	// look down a bit
			vec3_t	temp;

			FastVectorCopy (r_origin, temp);
			temp[2] -= 16;
			leaf = Mod_PointInLeaf (temp, r_worldmodel);
			if ( !(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2) )
				r_viewcluster2 = leaf->cluster;
		}
		else
		{	// look up a bit
			vec3_t	temp;

			FastVectorCopy (r_origin, temp);
			temp[2] += 16;
			leaf = Mod_PointInLeaf (temp, r_worldmodel);
			if ( !(leaf->contents & CONTENTS_SOLID) &&
				(leaf->cluster != r_viewcluster2) )
				r_viewcluster2 = leaf->cluster;
		}
	}

	v_blend[0] = r_newrefdef.blend[0];
	v_blend[1] = r_newrefdef.blend[1];
	v_blend[2] = r_newrefdef.blend[2];
	v_blend[3] = r_newrefdef.blend[3];

	c_brush_polys = 0;
	c_alias_polys = 0;

	// clear out the portion of the screen that the NOWORLDMODEL defines
	/*if ( r_newrefdef.rdflags & RDF_NOWORLDMODEL )
	{
		qglEnable( GL_SCISSOR_TEST );
		glClearColor( 0.3f, 0.3f, 0.3f, 1 );
		
		glScissor( r_newrefdef.x, vid.height - r_newrefdef.height - r_newrefdef.y, r_newrefdef.width, r_newrefdef.height );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		glClearColor( 1, 0, 0.5f, 0.5f );

		qglDisable( GL_SCISSOR_TEST );
	}*/
	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
	{
		qglEnable(GL_SCISSOR_TEST);
		glClearColor(0.3f, 0.3f, 0.3f, 1);
		glScissor(r_newrefdef.x, viddef.height - r_newrefdef.height - r_newrefdef.y, r_newrefdef.width,
			   r_newrefdef.height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);
		qglDisable(GL_SCISSOR_TEST);
	}
}


void MYgluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

#ifdef STERO_SUPPORT
   xmin += -( 2 * gl_state.camera_separation ) / zNear;
   xmax += -( 2 * gl_state.camera_separation ) / zNear;
#endif

   qglFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}


/*
=============
R_SetupGL
=============
*/
void R_SetupGL (void)
{
	float	screenaspect;
//	float	yfov;
	int		x, x2, y2, y, w, h;

	//
	// set up viewport
	//
	x = (int)floor(r_newrefdef.x * viddef.width / viddef.width);
	x2 = (int)ceil((r_newrefdef.x + r_newrefdef.width) * viddef.width / viddef.width);
	y = (int)floor(viddef.height - r_newrefdef.y * viddef.height / viddef.height);
	y2 = (int)ceil(viddef.height - (r_newrefdef.y + r_newrefdef.height) * viddef.height / viddef.height);

	w = x2 - x;
	h = y - y2;

	glViewport (x, y2, w, h);

	//
	// set up projection matrix
	//
    screenaspect = (float)r_newrefdef.width/r_newrefdef.height;
//	yfov = 2*atan((float)r_newrefdef.height/r_newrefdef.width)*180/M_PI;
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity ();
    MYgluPerspective (r_newrefdef.fov_y,  screenaspect,  4,  gl_zfar->value);

	glCullFace(GL_FRONT);

	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity ();

    qglRotatef (-90,  1, 0, 0);	    // put Z going up
    qglRotatef (90,  0, 0, 1);	    // put Z going up
    qglRotatef (-r_newrefdef.viewangles[2],  1, 0, 0);
    qglRotatef (-r_newrefdef.viewangles[0],  0, 1, 0);
    qglRotatef (-r_newrefdef.viewangles[1],  0, 0, 1);
    qglTranslatef (-r_newrefdef.vieworg[0],  -r_newrefdef.vieworg[1],  -r_newrefdef.vieworg[2]);

	qglGetFloatv (GL_MODELVIEW_MATRIX, r_world_matrix);

	//
	// set drawing parms
	//
	if (gl_cull->intvalue)
		qglEnable(GL_CULL_FACE);
	else
		qglDisable(GL_CULL_FACE);

	//qglDisable(GL_BLEND);
	qglDisable(GL_ALPHA_TEST);
	//qglEnable(GL_ALPHA_TEST);
	qglEnable(GL_DEPTH_TEST);
}

/*
=============
R_Clear
=============
*/

float ref_frand(void)
{
	return (((rand()&32767)) * .0000305185094759971922971282082583086642f);
}

void R_Clear (void)
{
	gldepthmin = 0;
	gldepthmax = 1;
	glDepthFunc (GL_LEQUAL);

	qglDepthRange (gldepthmin, gldepthmax);

	if (gl_clear->intvalue && gl_clear->value == 2)
	{
		glClearColor (ref_frand(), ref_frand(), ref_frand(), 1.0);
	} else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	}

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

/*void R_Flash( void )
{
	R_PolyBlend ();
}*/

/*
================
R_RenderView

r_newrefdef must be set before the first call
================
*/
void R_RenderView (refdef_t *fd)
{
	if (r_norefresh->intvalue)
		return;

	r_newrefdef = *fd;

	if (!r_worldmodel && !( r_newrefdef.rdflags & RDF_NOWORLDMODEL ) )
		ri.Sys_Error (ERR_DROP, "R_RenderView: NULL worldmodel");

	//if (r_speeds->intvalue)
	//{
	c_brush_polys = 0;
	c_alias_polys = 0;
	//}

	R_PushDlights ();

	R_SetupFrame ();

	R_SetFrustum ();

	R_SetupGL ();

	R_MarkLeaves ();	// done here so we know if we're in water

	R_DrawWorld ();

	R_DrawEntitiesOnList ();

	R_RenderDlights ();

	R_DrawParticles ();

	R_DrawAlphaSurfaces ();

	R_PolyBlend();
	
	if (r_speeds->intvalue)
	{
		VID_Printf (PRINT_ALL, "%4i wpoly %4i epoly %i tex %i lmaps\n",
			c_brush_polys, 
			c_alias_polys, 
			c_visible_textures, 
			c_visible_lightmaps); 
	}
}


void	R_SetGL2D (void)
{
	// set 2D virtual screen size
	glViewport (0,0, viddef.width, viddef.height);
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity ();
	//qglOrtho  (0, vid.width, vid.height, 0, -99999, 99999);
	qglOrtho(0, viddef.width, viddef.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity ();
	qglDisable (GL_DEPTH_TEST);
	qglDisable (GL_CULL_FACE);
	//GLPROFqglDisable (GL_BLEND);
	qglEnable (GL_ALPHA_TEST);
	qglColor4f(colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]);
}



/*
====================
R_SetLightLevel

====================
*/
void R_SetLightLevel (void)
{
	vec3_t		shadelight;

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
		return;

	// save off light value for server to look at (BIG HACK!)

	R_LightPoint (r_newrefdef.vieworg, shadelight);

	// pick the greatest component, which should be the same
	// as the mono value returned by software
	if (shadelight[0] > shadelight[1])
	{
		if (shadelight[0] > shadelight[2])
			r_lightlevel->value = 150*shadelight[0];
		else
			r_lightlevel->value = 150*shadelight[2];
	}
	else
	{
		if (shadelight[1] > shadelight[2])
			r_lightlevel->value = 150*shadelight[1];
		else
			r_lightlevel->value = 150*shadelight[2];
	}

}

/*
@@@@@@@@@@@@@@@@@@@@@
R_RenderFrame

@@@@@@@@@@@@@@@@@@@@@
*/
void R_RenderFrame (refdef_t *fd)
{
	R_RenderView( fd );
	R_SetLightLevel ();
	R_SetGL2D ();
}

void Cmd_HashStats_f (void);
static void R_Register(unsigned int defaultWidth, unsigned int defaultHeight)
{
	r_lefthand = ri.Cvar_Get( "hand", "0", CVAR_USERINFO | CVAR_ARCHIVE );
	r_norefresh = ri.Cvar_Get ("r_norefresh", "0", 0);
	r_fullbright = ri.Cvar_Get ("r_fullbright", "0", 0);
	r_drawentities = ri.Cvar_Get ("r_drawentities", "1", 0);
	r_drawworld = ri.Cvar_Get ("r_drawworld", "1", 0);
	r_novis = ri.Cvar_Get ("r_novis", "0", 0);
	r_nocull = ri.Cvar_Get ("r_nocull", "0", 0);
	r_lerpmodels = ri.Cvar_Get ("r_lerpmodels", "1", 0);
	r_speeds = ri.Cvar_Get ("r_speeds", "0", 0);

	r_lightlevel = ri.Cvar_Get ("r_lightlevel", "0", CVAR_NOSET);

	//gl_nosubimage = ri.Cvar_Get( "gl_nosubimage", "0", 0 );

	gl_particle_min_size = ri.Cvar_Get( "gl_particle_min_size", "2", CVAR_ARCHIVE );
	gl_particle_max_size = ri.Cvar_Get( "gl_particle_max_size", "40", CVAR_ARCHIVE );
	gl_particle_size = ri.Cvar_Get( "gl_particle_size", "40", CVAR_ARCHIVE );
	gl_particle_att_a = ri.Cvar_Get( "gl_particle_att_a", "0.01", CVAR_ARCHIVE );
	gl_particle_att_b = ri.Cvar_Get( "gl_particle_att_b", "0.0", CVAR_ARCHIVE );
	gl_particle_att_c = ri.Cvar_Get( "gl_particle_att_c", "0.01", CVAR_ARCHIVE );

	gl_modulate = ri.Cvar_Get ("gl_modulate", "2", CVAR_ARCHIVE );
	gl_bitdepth = ri.Cvar_Get( "gl_bitdepth", "0", 0 );
	//gl_lightmap = ri.Cvar_Get ("gl_lightmap", "0", 0);
	gl_shadows = ri.Cvar_Get ("gl_shadows", "0", CVAR_ARCHIVE );
	gl_dynamic = ri.Cvar_Get ("gl_dynamic", "1", 0);
	gl_nobind = ri.Cvar_Get ("gl_nobind", "0", 0);
	gl_round_down = ri.Cvar_Get ("gl_round_down", "0", 0);
	gl_picmip = ri.Cvar_Get ("gl_picmip", "0", 0);
	gl_skymip = ri.Cvar_Get ("gl_skymip", "0", 0);
	gl_showtris = ri.Cvar_Get ("gl_showtris", "0", 0);
	gl_clear = ri.Cvar_Get ("gl_clear", "0", 0);
	gl_cull = ri.Cvar_Get ("gl_cull", "1", 0);
	gl_polyblend = ri.Cvar_Get ("gl_polyblend", "1", 0);
	gl_flashblend = ri.Cvar_Get ("gl_flashblend", "0", 0);
	//gl_playermip = ri.Cvar_Get ("gl_playermip", "0", 0);
	//gl_monolightmap = ri.Cvar_Get( "gl_monolightmap", "0", 0 );
	gl_texturemode = ri.Cvar_Get( "gl_texturemode", "GL_LINEAR_MIPMAP_LINEAR", CVAR_ARCHIVE );
	gl_lockpvs = ri.Cvar_Get( "gl_lockpvs", "0", 0 );

	//gl_ext_swapinterval = ri.Cvar_Get( "gl_ext_swapinterval", "1", CVAR_ARCHIVE );
	gl_ext_multitexture = ri.Cvar_Get( "gl_ext_multitexture", "1", CVAR_ARCHIVE );
	
	//note, pointparams moved to init to handle defaults
	//gl_ext_compiled_vertex_array = ri.Cvar_Get( "gl_ext_compiled_vertex_array", "1", CVAR_ARCHIVE );

	//r1ch: my extensions
	//gl_ext_generate_mipmap = ri.Cvar_Get ("gl_ext_generate_mipmap", "0", 0);
	gl_ext_point_sprite = ri.Cvar_Get ("gl_ext_point_sprite", "0", 0);
	gl_ext_texture_filter_anisotropic = ri.Cvar_Get ("gl_ext_texture_filter_anisotropic", "0", 0);
	gl_ext_texture_non_power_of_two = ri.Cvar_Get ("gl_ext_texture_non_power_of_two", "0", 0);
	gl_ext_max_anisotropy = ri.Cvar_Get ("gl_ext_max_anisotropy", "2", 0);
	
	gl_colorbits = ri.Cvar_Get ("gl_colorbits", "0", 0);
	gl_stencilbits = ri.Cvar_Get ("gl_stencilbits", "", 0);
	gl_alphabits = ri.Cvar_Get ("gl_alphabits", "", 0);
	gl_depthbits = ri.Cvar_Get ("gl_depthbits", "", 0);

	gl_ext_multisample = ri.Cvar_Get ("gl_ext_multisample", "0", 0);
	gl_ext_samples = ri.Cvar_Get ("gl_ext_samples", "2", 0);
	
	gl_zfar = ri.Cvar_Get ("gl_zfar", "8192", 0);

	cl_version = ri.Cvar_Get ("cl_version", REF_VERSION, CVAR_NOSET); 
	
	gl_r1gl_test = ri.Cvar_Get ("gl_r1gl_test", "0", 0);
	gl_doublelight_entities = ri.Cvar_Get ("gl_doublelight_entities", "1", 0);
	gl_noscrap = ri.Cvar_Get ("gl_noscrap", "1", 0);
	gl_overbrights = ri.Cvar_Get ("gl_overbrights", "0", 0);
	gl_linear_mipmaps = ri.Cvar_Get ("gl_linear_mipmaps", "0", 0);

	vid_forcedrefresh = ri.Cvar_Get ("vid_forcedrefresh", "0", 0);
	vid_optimalrefresh = ri.Cvar_Get ("vid_optimalrefresh", "0", 0);
	vid_gamma_pics = ri.Cvar_Get ("vid_gamma_pics", "0", 0);
	vid_nowgl = ri.Cvar_Get ("vid_nowgl", "0", 0);
	vid_restore_on_switch = ri.Cvar_Get ("vid_flip_on_switch", "0", 0);

	char tempBuf[8];
	Com_sprintf(tempBuf, 8, "%u", defaultWidth);
	vid_width = ri.Cvar_Get ("vid_width", tempBuf, CVAR_ARCHIVE);
	Com_sprintf(tempBuf, 8, "%u", defaultHeight);
	vid_height = ri.Cvar_Get ("vid_height", tempBuf, CVAR_ARCHIVE);

	vid_topmost = ri.Cvar_Get ("vid_topmost", "0", 0);

	gl_pic_scale = ri.Cvar_Get ("gl_pic_scale", "1", 0);
	//r1ch end my shit

	gl_swapinterval = ri.Cvar_Get( "gl_swapinterval", "1", CVAR_ARCHIVE );

	//gl_saturatelighting = ri.Cvar_Get( "gl_saturatelighting", "0", 0 );

	gl_jpg_quality = ri.Cvar_Get ("gl_jpg_quality", "90", 0);
	gl_coloredlightmaps = ri.Cvar_Get ("gl_coloredlightmaps", "1", 0);
	usingmodifiedlightmaps = (gl_coloredlightmaps->value != 1.0f);

	//gl_3dlabs_broken = ri.Cvar_Get( "gl_3dlabs_broken", "1", CVAR_ARCHIVE );

	vid_fullscreen = ri.Cvar_Get( "vid_fullscreen", "0", CVAR_ARCHIVE );
	vid_gamma = ri.Cvar_Get( "vid_gamma", "1.0", CVAR_ARCHIVE );

	gl_texture_formats = ri.Cvar_Get ("gl_texture_formats", "png jpg tga", 0);
	gl_pic_formats = ri.Cvar_Get ("gl_pic_formats", "png jpg tga", 0);

	load_png_wals = strstr (gl_texture_formats->string, "png") ? true : false;
	load_jpg_wals = strstr (gl_texture_formats->string, "jpg") ? true : false;
	load_tga_wals = strstr (gl_texture_formats->string, "tga") ? true : false;

	load_png_pics = strstr (gl_pic_formats->string, "png") ? true : false;
	load_jpg_pics = strstr (gl_pic_formats->string, "jpg") ? true : false;
	load_tga_pics = strstr (gl_pic_formats->string, "tga") ? true : false;

	gl_dlight_falloff = ri.Cvar_Get ("gl_dlight_falloff", "0", 0);
	gl_alphaskins = ri.Cvar_Get ("gl_alphaskins", "0", 0);

	//con_alpha = ri.Cvar_Get ("con_alpha", "1.0", 0);

	ri.Cmd_AddCommand( "imagelist", GL_ImageList_f );
	ri.Cmd_AddCommand( "screenshot", GL_ScreenShot_f );
	ri.Cmd_AddCommand( "modellist", Mod_Modellist_f );
	ri.Cmd_AddCommand( "gl_strings", GL_Strings_f );
	ri.Cmd_AddCommand( "hash_stats", Cmd_HashStats_f );
	

#ifdef R1GL_RELEASE
	ri.Cmd_AddCommand ("r1gl_version", GL_Version_f);
#endif
}


static void SetSDLIcon(void) {
#include "q2icon.xbm"
    SDL_Surface * icon;
    SDL_Color color;
    Uint8 * ptr;
    int i, mask;

    icon = SDL_CreateRGBSurface(SDL_SWSURFACE, q2icon_width, q2icon_height, 8,
				0, 0, 0, 0);
    if (icon == NULL)
	return; /* oh well... */

    SDL_SetColorKey(icon, SDL_TRUE, 0);

    color.r = 255;
    color.g = 255;
    color.b = 255;
	icon->format->palette->colors[0] = color;
    color.r = 0;
    color.g = 16;
    color.b = 0;
	icon->format->palette->colors[1] = color;

    ptr = (Uint8 *)icon->pixels;
    for (i = 0; i < sizeof(q2icon_bits); i++) {
	for (mask = 1; mask != 0x100; mask <<= 1) {
	    *ptr = (q2icon_bits[i] & mask) ? 1 : 0;
	    ptr++;
	}               
    }

	SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}


#ifndef CALLBACK
#define CALLBACK
#endif


void CALLBACK glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);


/*
** GLimp_InitGraphics
**
** This initializes the software refresh's implementation specific
** graphics subsystem.  In the case of Windows it creates DIB or
** DDRAW surfaces.
**
** The necessary width and height parameters are grabbed from
** vid.width and vid.height.
*/
static qboolean GLimp_InitGraphics( qboolean fullscreen )
{
	/* Just toggle fullscreen if that's all that has been changed */
	int oldW = -1, oldH = -1;
	if (window) {
		SDL_GetWindowSize(window, &oldW, &oldH);
	}

	if ((oldW == viddef.width) && (oldH == viddef.height)) {
		int isfullscreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) ? 1 : 0;
		if (fullscreen != isfullscreen) {
			// TODO: support SDL_WINDOW_FULLSCREEN_DESKTOP
			SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		}

		isfullscreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) ? 1 : 0;
		if (fullscreen == isfullscreen)
			return true;
	}
	
	// Move this somewhere
	srand(getpid());

#if 0
	// for testing on desktop
	// nvidia lets us create GLES contexts
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

#if 0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

#else

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

#endif  // 0

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	if (window) {
		SDL_SetWindowSize(window, viddef.width, viddef.height);
	} else {
		// TODO: SDL_WINDOW_RESIZABLE
		// TODO: SDL_WINDOW_ALLOW_HIGHDPI?
		uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
		if (fullscreen) {
			// TODO: SDL_WINDOW_FULLSCREEN_DESKTOP
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		if (_windowed_mouse->intvalue != 0) {
			flags |= SDL_WINDOW_INPUT_GRABBED;
		}

#ifdef EMSCRIPTEN
		viddef.width = 800;
 		viddef.height = 600;
		emscripten_log(EM_LOG_CONSOLE, "Create window, size %d x %d\n", viddef.width, viddef.height);
#endif  // EMSCRIPTEN
		window = SDL_CreateWindow("Quake II", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, viddef.width, viddef.height, flags);
		if (!window) {
			Sys_Error("(SDLGL) SDL CreateWindow failed: %s\n", SDL_GetError());
			return false;
		}
		SetSDLIcon();

		if (_windowed_mouse->intvalue != 0) {
			int retval = SDL_SetRelativeMouseMode(_windowed_mouse->value ? SDL_TRUE : SDL_FALSE);
			if (retval != 0) {
				VID_Printf (PRINT_ALL, "Failed to set relative mouse state \"%s\"\n", SDL_GetError());
			}
		}

		glcontext = SDL_GL_CreateContext(window);

#ifdef USE_GLEW

		glewInit();

		// swap once for better traces
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SDL_GL_SwapWindow(window);

		if (GLEW_KHR_debug) {
			VID_Printf( PRINT_ALL, "KHR_debug found\n" );

			glDebugMessageCallback(glDebugCallback, NULL);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			// nvidia warning about inconsistent texture base level
			// which fires on all glClear operations
			// tell the driver to STFU
			{
				const unsigned int ids[] = { 131204 };
				glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 1, ids, GL_FALSE);
			}

		} else {
			VID_Printf( PRINT_ALL, "No KHR_debug\n" );
		}

#else  // USE_GLEW

#endif  // USE_GLEW

	}

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow(viddef.width, viddef.height);

	X11_active = true;

	return true;
}




/*
** GLimp_SetMode
*/
static int GLimp_SetMode( unsigned int *pwidth, unsigned int *pheight, qboolean fullscreen )
{
	VID_Printf (PRINT_ALL, "setting mode %ux%u\n", *pwidth, *pheight);

	if ( !GLimp_InitGraphics( fullscreen ) ) {
		// failed to set a valid mode in windowed mode
		return VID_ERR_INVALID_MODE;
	}

	return VID_ERR_NONE;
}


/*
==================
R_SetMode
==================
*/
static int R_SetMode(unsigned int width, unsigned int height)
{
	qboolean fullscreen = !vid_fullscreen->intvalue ? false : true;

	vid_fullscreen->modified = false;

	viddef.width = width;
	viddef.height = height;

	int err = GLimp_SetMode( &viddef.width, &viddef.height, fullscreen );
	if ( err == VID_ERR_NONE )
	{
	}
	else
	{
		if ( err & VID_ERR_RETRY_QGL)
		{
			return err;
		}
		else if ( err & VID_ERR_FULLSCREEN_FAILED )
		{
			ri.Cvar_SetValue( "vid_fullscreen", 0);
			vid_fullscreen->modified = false;
			VID_Printf( PRINT_ALL, "ref_gl::R_SetMode() - fullscreen unavailable in this mode\n" );
			if ( ( err = GLimp_SetMode( &viddef.width, &viddef.height, false ) ) == VID_ERR_NONE )
				return VID_ERR_NONE;
		}
		else if ( err & VID_ERR_FAIL )
		{
			VID_Printf( PRINT_ALL, "ref_gl::R_SetMode() - invalid mode\n" );
		}

		// try setting it back to something safe
		err = GLimp_SetMode( &viddef.width, &viddef.height, false );
		if ( err != VID_ERR_NONE )
		{
			VID_Printf( PRINT_ALL, "ref_gl::R_SetMode() - could not revert to safe mode\n" );
			return VID_ERR_FAIL;
		}
	}
	return VID_ERR_NONE;
}

/*
===============
R_Init
===============
*/
int R_Init( void *hinstance, void *hWnd )
{	
	char renderer_buffer[1000];
	char vendor_buffer[1000];
	int		err;
	int		j;
	extern float r_turbsin[256];

	clearImageHash();

	memset(gl_state.lightmap_textures, 0, MAX_LIGHTMAPS * sizeof(GLuint));

	int retval = SDL_Init(SDL_INIT_VIDEO);
	if (retval != 0) {
		Sys_Error("SDL Init failed: \"%s\"\n", SDL_GetError());
		return -1;
	}

	/* List displays and display modes */
	int numVideoDrivers = SDL_GetNumVideoDrivers();
	if (numVideoDrivers < 0) {
		VID_Printf( PRINT_ALL, "SDL_GetNumVideoDrivers failed: %s\n", SDL_GetError());
	} else {
		VID_Printf( PRINT_ALL, "%d video drivers:\n", numVideoDrivers);
		for (int i = 0; i < numVideoDrivers; i++) {
			VID_Printf( PRINT_ALL, "%d: \"%s\"\n", i, SDL_GetVideoDriver(i));
		}
		VID_Printf( PRINT_ALL, "Current video driver is \"%s\"\n", SDL_GetCurrentVideoDriver());
	}

	SDL_DisplayMode mode;
	memset(&mode, 0, sizeof(SDL_DisplayMode));

	unsigned int desktopWidth = 0, desktopHeight = 0;
	int numVideoDisplays = SDL_GetNumVideoDisplays();
	if (numVideoDisplays < 0) {
		VID_Printf( PRINT_ALL, "SDL_GetNumVideoDisplays failed: \"%s\"\n", SDL_GetError());
	} else {
		VID_Printf( PRINT_ALL, "%d displays:\n", numVideoDisplays);

		for (int i = 0; i < numVideoDisplays; i++) {
			const char *displayName = SDL_GetDisplayName(i);
			VID_Printf( PRINT_ALL, "Display %d: \"%s\"\n", i, displayName);

			// TODO: store modes

			int numModes = SDL_GetNumDisplayModes(i);
			if (numModes < 0) {
				VID_Printf( PRINT_ALL, "SDL_GetNumDisplayModes failed: \"%s\"\n", SDL_GetError());
			} else {
				for (int j = 0; j < numModes; j++) {
					int retval = SDL_GetDisplayMode(i, j, &mode);
					if (retval < 0) {
						VID_Printf( PRINT_ALL, "SDL_GetDisplayMode failed: \"%s\"\n", SDL_GetError());
					} else {
						VID_Printf( PRINT_ALL, "Mode %d: %dx%d %d Hz\n", j, mode.w, mode.h, mode.refresh_rate);
						// TODO: store
					}
				}
			}

			int retval = SDL_GetDesktopDisplayMode(i, &mode);
			if (retval < 0) {
				VID_Printf( PRINT_ALL, "SDL_GetDesktopDisplayMode failed: \"%s\"\n", SDL_GetError());
			} else {
				VID_Printf( PRINT_ALL, "Desktop display mode: %dx%d %d Hz\n", mode.w, mode.h, mode.refresh_rate);
				desktopWidth = mode.w;
				desktopHeight = mode.h;
			}
		}
	}

	for ( j = 0; j < 256; j++ )
	{
		r_turbsin[j] *= 0.5;
	}

	ri.Cmd_ExecuteText (EXEC_NOW, "exec r1gl.cfg\n");

	VID_Printf (PRINT_ALL, "ref_gl version: "REF_VERSION"\n");

	Com_DPrintf("Draw_GetPalette()\n");
	Draw_GetPalette ();

	Com_DPrintf("R_Register()\n");
	R_Register(desktopWidth, desktopHeight);

	gl_overbrights->modified = false;

retryQGL:

	// initialize our QGL dynamic bindings
	Com_DPrintf("QGL_Init()\n");
	qglState = (QGLState *) malloc(sizeof(QGLState));
	// qglState = new QGLState;   ... oh shit, not C++. sigh ...
	memset(qglState, 0, sizeof(QGLState));
	qglState->numVertices = 1024;
	qglState->vertices = (Vertex *) malloc(qglState->numVertices * sizeof(Vertex));
	memset(qglState->vertices, 0, qglState->numVertices * sizeof(Vertex));

	qglState->maxDrawCalls = 128;
	qglState->drawCalls = (DrawCall *) malloc(qglState->maxDrawCalls * sizeof(DrawCall));

	qglState->zFar = 1.0f;

#ifdef HAVE_JOYSTICK
	init_joystick();
#endif

	// create the window and set up the context
	Com_DPrintf("R_SetMode()\n");
	err = R_SetMode(vid_width->intvalue, vid_height->intvalue);
	if (err != VID_ERR_NONE)
	{
		QGL_Shutdown();
		if (err & VID_ERR_RETRY_QGL)
			goto retryQGL;

        VID_Printf (PRINT_ALL, "ref_gl::R_Init() - could not R_SetMode()\n" );
		return -1;
	}

	Com_DPrintf("Vid_MenuInit()\n");
	ri.Vid_MenuInit();

	/*
	** get our various GL strings
	*/
	gl_config.vendor_string = (const char *) glGetString (GL_VENDOR);
	VID_Printf (PRINT_ALL, "GL_VENDOR: %s\n", gl_config.vendor_string );
	gl_config.renderer_string = (const char *) glGetString (GL_RENDERER);
	VID_Printf (PRINT_ALL, "GL_RENDERER: %s\n", gl_config.renderer_string );
	gl_config.version_string = (const char *) glGetString (GL_VERSION);
	VID_Printf (PRINT_ALL, "GL_VERSION: %s\n", gl_config.version_string );
	gl_config.extensions_string = (const char *) glGetString (GL_EXTENSIONS);
	//VID_Printf (PRINT_ALL, "GL_EXTENSIONS: %s\n", gl_config.extensions_string );

	Q_strncpy( renderer_buffer, gl_config.renderer_string, sizeof(renderer_buffer)-1);
	Q_strlwr( renderer_buffer );

	Q_strncpy( vendor_buffer, gl_config.vendor_string, sizeof(vendor_buffer)-1);
	Q_strlwr( vendor_buffer );

	/*
	** grab extensions
	*/

#ifdef _WIN32
	if ( strstr( gl_config.extensions_string, "WGL_EXT_swap_control" ) )
	{
		VID_Printf( PRINT_ALL, "...enabling WGL_EXT_swap_control\n" );
	}
	else
	{
		VID_Printf( PRINT_ALL, "...WGL_EXT_swap_control not found\n" );
	}
#endif

	VID_Printf( PRINT_ALL, "Initializing emgl extensions:\n" );

	/*gl_config.r1gl_GL_SGIS_generate_mipmap = false;
	if ( strstr( gl_config.extensions_string, "GL_SGIS_generate_mipmap" ) ) {
		if ( gl_ext_generate_mipmap->value ) {
			VID_Printf( PRINT_ALL, "...using GL_SGIS_generate_mipmap\n" );
			gl_config.r1gl_GL_SGIS_generate_mipmap = true;
		} else {
			VID_Printf( PRINT_ALL, "...ignoring GL_SGIS_generate_mipmap\n" );		
		}
	} else {
		VID_Printf( PRINT_ALL, "...GL_SGIS_generate_mipmap not found\n" );
	}*/

	gl_config.r1gl_GL_EXT_texture_filter_anisotropic = false;
	if ( strstr( gl_config.extensions_string, "GL_EXT_texture_filter_anisotropic" ) )
	{
		if ( gl_ext_texture_filter_anisotropic->value ) {
			VID_Printf( PRINT_ALL, "...using GL_EXT_texture_filter_anisotropic\n" );
			gl_config.r1gl_GL_EXT_texture_filter_anisotropic = true;
		} else {
			VID_Printf( PRINT_ALL, "...ignoring GL_EXT_texture_filter_anisotropic\n" );		
		}
	} else {
		VID_Printf( PRINT_ALL, "...GL_EXT_texture_filter_anisotropic not found\n" );
	}
	gl_ext_texture_filter_anisotropic->modified = false;

	gl_config.r1gl_GL_ARB_texture_non_power_of_two = false;
	if ( strstr( gl_config.extensions_string, "GL_ARB_texture_non_power_of_two" ) ) {
		if (gl_ext_texture_non_power_of_two->intvalue ) {
			VID_Printf( PRINT_ALL, "...using GL_ARB_texture_non_power_of_two\n" );
			gl_config.r1gl_GL_ARB_texture_non_power_of_two = true;
		} else {
			VID_Printf( PRINT_ALL, "...ignoring GL_ARB_texture_non_power_of_two\n" );		
		}
	} else {
		VID_Printf( PRINT_ALL, "...GL_ARB_texture_non_power_of_two not found\n" );
	}

	VID_Printf( PRINT_ALL, "Initializing emgl NVIDIA-only extensions:\n" );

	Com_DPrintf("GL_SetDefaultState()\n" );
	GL_SetDefaultState();

	/*
	** draw our stereo patterns
	*/
#if 0 // commented out until H3D pays us the money they owe us
	GL_DrawStereoPattern();
#endif

	Com_DPrintf("GL_InitImages()\n" );
	GL_InitImages ();

	Com_DPrintf("Mod_Init()\n" );
	Mod_Init ();

	Com_DPrintf("R_InitParticleTexture()\n" );
	R_InitParticleTexture ();

	Com_DPrintf("Draw_InitLocal()\n" );
	Draw_InitLocal ();

	err = glGetError();
	if ( err != GL_NO_ERROR )
		VID_Printf (PRINT_ALL, "glGetError() = 0x%x\n", err);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	Com_DPrintf("R_Init() complete.\n" );
	return 0;
}

/*
===============
R_Shutdown
===============
*/
void R_Shutdown (void)
{
	if (gl_state.lightmap_textures[0] != 0) {
		glDeleteTextures(MAX_LIGHTMAPS, gl_state.lightmap_textures);
		memset(gl_state.lightmap_textures, 0, MAX_LIGHTMAPS * sizeof(GLuint));
	}

	ri.Cmd_RemoveCommand ("modellist");
	ri.Cmd_RemoveCommand ("screenshot");
	ri.Cmd_RemoveCommand ("imagelist");
	ri.Cmd_RemoveCommand ("gl_strings");
	ri.Cmd_RemoveCommand ("hash_stats");

#ifdef R1GL_RELEASE
	ri.Cmd_RemoveCommand ("r1gl_version");
#endif

	Mod_FreeAll ();

	GL_ShutdownImages ();

	/*
	** shutdown our QGL subsystem
	*/
	QGL_Shutdown();

	/*
	** shut down OS specific OpenGL stuff like contexts, etc.
	*/
	GLimp_Shutdown();
}

void GL_UpdateAnisotropy (void)
{
	int		i;
	image_t	*glt;
	float	value;

	if (!gl_config.r1gl_GL_EXT_texture_filter_anisotropic)
		value = 1;
	else
		value = gl_ext_max_anisotropy->value;

	for (i=0, glt=gltextures ; i<numgltextures ; i++, glt++)
	{
		if (glt->type != it_pic && glt->type != it_sky)
		{
			GL_MBind(GL_TEXTURE0, glt->texnum);
			glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
		}
	}
}


void GetEvent(SDL_Event *event);
#ifdef EMSCRIPTEN
EM_BOOL q2_pointerlockchange(int eventType, const EmscriptenPointerlockChangeEvent *pointerlockChangeEvent, void *userData);
int FilterEvents(void* userdata, SDL_Event* event);
#endif

/*
@@@@@@@@@@@@@@@@@@@@@
R_BeginFrame
@@@@@@@@@@@@@@@@@@@@@
*/
void R_BeginFrame(void)
{

	/*
	** change modes if necessary
	*/
	if ( vid_width->modified || vid_height->modified || vid_fullscreen->modified )
	{
		int err = R_SetMode(vid_width->intvalue, vid_height->intvalue);
		if (err != VID_ERR_NONE) {
			VID_Printf(PRINT_ALL, "Error in R_SetMode\n");
		}
		vid_width->modified = false;
		vid_height->modified = false;
		vid_fullscreen->modified = false;
	}

	if (gl_contrast->modified)
	{
		if (gl_contrast->value < 0.5f)
			ri.Cvar_SetValue ("gl_contrast", 0.5f);
		else if (gl_contrast->value > 1.5f)
			ri.Cvar_SetValue ("gl_contrast", 1.5f);

		gl_contrast->modified = false;
	}

	mx = 0; my = 0;

	// TODO: inline GetEvent
	// TODO: use events instead of SDL_GetMouseState
	// TODO: after that use message timestamp instead of Sys_Milliseconds
	SDL_Event event;
	memset(&event, 0, sizeof(SDL_Event));
	while (SDL_PollEvent(&event)) {
		GetEvent(&event);
	}

	// TODO: update gamma if necessary

	/*
	** go into 2D mode
	*/
	glViewport (0,0, viddef.width, viddef.height);
	qglMatrixMode(GL_PROJECTION);
    qglLoadIdentity ();
	//qglOrtho  (0, vid.width, vid.height, 0, -99999, 99999);
	qglOrtho(0, viddef.width, viddef.height, 0, -99999, 99999);
	qglMatrixMode(GL_MODELVIEW);
    qglLoadIdentity ();
	//qglDisable (GL_DEPTH_TEST);
	//GLPROFqglDisable (GL_CULL_FACE);
	//GLPROFqglDisable (GL_BLEND);
	//GLPROFqglEnable (GL_ALPHA_TEST);
	qglColor4f(colorWhite[0], colorWhite[1], colorWhite[2], colorWhite[3]);

	//qglEnable(GL_MULTISAMPLE_ARB);

	/*
	** texturemode stuff
	*/
	if ( gl_texturemode->modified )
	{
		GL_TextureMode( gl_texturemode->string );
		gl_texturemode->modified = false;
	}

	if (gl_ext_max_anisotropy->modified && gl_config.r1gl_GL_EXT_texture_filter_anisotropic)
	{
		GL_UpdateAnisotropy ();
		gl_ext_max_anisotropy->modified = false;
	}

	if (gl_ext_texture_filter_anisotropic->modified)
	{
		gl_config.r1gl_GL_EXT_texture_filter_anisotropic = false;
		if ( strstr( gl_config.extensions_string, "GL_EXT_texture_filter_anisotropic" ) )
		{
			if ( gl_ext_texture_filter_anisotropic->value )
			{
				VID_Printf( PRINT_ALL, "...using GL_EXT_texture_filter_anisotropic\n" );
				gl_config.r1gl_GL_EXT_texture_filter_anisotropic = true;
				GL_UpdateAnisotropy ();
			}
			else
			{
				VID_Printf( PRINT_ALL, "...ignoring GL_EXT_texture_filter_anisotropic\n" );
				GL_UpdateAnisotropy ();
			}
		}
		else
		{
			VID_Printf( PRINT_ALL, "...GL_EXT_texture_filter_anisotropic not found\n" );
		}
		
		gl_ext_texture_filter_anisotropic->modified = false;
	}

	if (gl_texture_formats->modified)
	{
		load_png_wals = strstr (gl_texture_formats->string, "png") ? true : false;
		load_jpg_wals = strstr (gl_texture_formats->string, "jpg") ? true : false;
		load_tga_wals = strstr (gl_texture_formats->string, "tga") ? true : false;
		gl_texture_formats->modified = false;
	}

	if (gl_pic_formats->modified)
	{
		load_png_pics = strstr (gl_pic_formats->string, "png") ? true : false;
		load_jpg_pics = strstr (gl_pic_formats->string, "jpg") ? true : false;
		load_tga_pics = strstr (gl_pic_formats->string, "tga") ? true : false;
		gl_pic_formats->modified = false;
	}

	/*
	** swapinterval stuff
	*/
	GL_UpdateSwapInterval();

	//
	// clear screen if desired
	//
	R_Clear ();
}

/*
=============
R_SetPalette
=============
*/
unsigned r_rawpalette[256];

void R_SetPalette ( const unsigned char *palette)
{
	/*int		i;

	byte *rp = ( byte * ) r_rawpalette;

	if ( palette )
	{
		for ( i = 0; i < 256; i++ )
		{
			rp[i*4+0] = palette[i*3+0];
			rp[i*4+1] = palette[i*3+1];
			rp[i*4+2] = palette[i*3+2];
			rp[i*4+3] = 0xff;
		}
	}
	else
	{
		for ( i = 0; i < 256; i++ )
		{
			rp[i*4+0] = d_8to24table[i] & 0xff;
			rp[i*4+1] = ( d_8to24table[i] >> 8 ) & 0xff;
			rp[i*4+2] = ( d_8to24table[i] >> 16 ) & 0xff;
			rp[i*4+3] = 0xff;
		}
	}
	GL_SetTexturePalette( r_rawpalette );*/

	glClearColor (0,0,0,0);
	glClear (GL_COLOR_BUFFER_BIT);
	glClearColor (1,0, 0.5 , 0.5);
}

/*
** R_DrawBeam
*/
void R_DrawBeam( entity_t *e )
{
#define NUM_BEAM_SEGS 6

	int	i;
	float r, g, b;

	vec3_t perpvec;
	vec3_t direction, normalized_direction;
	vec3_t	start_points[NUM_BEAM_SEGS], end_points[NUM_BEAM_SEGS];
	vec3_t oldorigin, origin;

	oldorigin[0] = e->oldorigin[0];
	oldorigin[1] = e->oldorigin[1];
	oldorigin[2] = e->oldorigin[2];

	origin[0] = e->origin[0];
	origin[1] = e->origin[1];
	origin[2] = e->origin[2];

	normalized_direction[0] = direction[0] = oldorigin[0] - origin[0];
	normalized_direction[1] = direction[1] = oldorigin[1] - origin[1];
	normalized_direction[2] = direction[2] = oldorigin[2] - origin[2];

	if ( VectorNormalize( normalized_direction ) == 0 )
		return;

	PerpendicularVector( perpvec, normalized_direction );
	VectorScale( perpvec, e->frame / 2, perpvec );

	for ( i = 0; i < 6; i++ )
	{
		RotatePointAroundVector( start_points[i], normalized_direction, perpvec, (360.0f/NUM_BEAM_SEGS)*i );
		VectorAdd( start_points[i], origin, start_points[i] );
		VectorAdd( start_points[i], direction, end_points[i] );
	}

	qglDisable( GL_TEXTURE_2D );
	qglEnable( GL_BLEND );
	glDepthMask( GL_FALSE );

	r = (float)(( d_8to24table[e->skinnum & 0xFF] ) & 0xFF);
	g = (float)(( d_8to24table[e->skinnum & 0xFF] >> 8 ) & 0xFF);
	b = (float)(( d_8to24table[e->skinnum & 0xFF] >> 16 ) & 0xFF);

	r *= 1/255.0F;
	g *= 1/255.0F;
	b *= 1/255.0F;

	qglColor4f( r, g, b, e->alpha );

	qglBegin( GL_TRIANGLE_STRIP );
	for ( i = 0; i < NUM_BEAM_SEGS; i++ )
	{
		qglVertex3f(start_points[i][0], start_points[i][1], start_points[i][2]);
		qglVertex3f(end_points[i][0], end_points[i][1], end_points[i][2]);
		int lastPt = (i + 1) % NUM_BEAM_SEGS;
		qglVertex3f(start_points[lastPt][0], start_points[lastPt][1], start_points[lastPt][2]);
		qglVertex3f(end_points[lastPt][0], end_points[lastPt][1], end_points[lastPt][2]);
	}
	qglEnd();

	qglEnable( GL_TEXTURE_2D );
	qglDisable( GL_BLEND );
	glDepthMask( GL_TRUE );
}

//===================================================================


void	Draw_Pic (int x, int y, char *name);
void	Draw_Char (int x, int y, int c);
void	Draw_TileClear (int x, int y, int w, int h, char *name);
void	Draw_Fill (int x, int y, int w, int h, int c);
void	Draw_FadeScreen (void);

/*
@@@@@@@@@@@@@@@@@@@@@
GetRefAPI

@@@@@@@@@@@@@@@@@@@@@
*/
refexport_t GetRefAPI (refimport_t rimp )
{
	refexport_t	re;

	ri = rimp;

	re.api_version = API_VERSION;

	re.DrawGetPicSize = Draw_GetPicSize;
	re.DrawPic = Draw_Pic;
	re.DrawStretchPic = Draw_StretchPic;
	re.DrawTileClear = Draw_TileClear;
	re.DrawFill = Draw_Fill;
	re.DrawFadeScreen= Draw_FadeScreen;

	re.DrawStretchRaw = Draw_StretchRaw;

	Swap_Init ();

	return re;
}


static void Force_CenterView_f (void)
{
	in_state->viewangles[PITCH] = 0;
}

static void RW_IN_MLookDown (void) 
{ 
	mlooking = true; 
}

static void RW_IN_MLookUp (void) 
{
	mlooking = false;
	in_state->IN_CenterView_fp ();
}

void RW_IN_Init(in_state_t *in_state_p)
{
	in_state = in_state_p;

	// mouse variables
	_windowed_mouse = ri.Cvar_Get ("_windowed_mouse", "0", CVAR_ARCHIVE);
	m_filter = ri.Cvar_Get ("m_filter", "0", 0);
	in_mouse = ri.Cvar_Get ("in_mouse", "1", CVAR_ARCHIVE);
#ifdef HAVE_JOYSTICK
	in_joystick = ri.Cvar_Get("in_joystick", "0", CVAR_ARCHIVE);
	j_invert_y = ri.Cvar_Get("j_invert_y", "1", 0);
	lr_axis = (int) ri.Cvar_Get("j_lr_axis", "0", CVAR_ARCHIVE)->value;
	ud_axis = (int) ri.Cvar_Get("j_ud_axis", "1", CVAR_ARCHIVE)->value;
	throttle_axis = (int) ri.Cvar_Get("j_throttle", "3", CVAR_ARCHIVE)->value;
#endif
	my_freelook = ri.Cvar_Get( "freelook", "0", 0);
	my_lookstrafe = ri.Cvar_Get ("lookstrafe", "0", 0);
	
	sensitivity = ri.Cvar_Get ("sensitivity", "3", 0);
	m_pitch = ri.Cvar_Get ("m_pitch", "0.022", 0);
	m_yaw = ri.Cvar_Get ("m_yaw", "0.022", 0);
	m_forward = ri.Cvar_Get ("m_forward", "1", 0);
	m_side = ri.Cvar_Get ("m_side", "0.8", 0);

	ri.Cmd_AddCommand ("+mlook", RW_IN_MLookDown);
	ri.Cmd_AddCommand ("-mlook", RW_IN_MLookUp);

	ri.Cmd_AddCommand ("force_centerview", Force_CenterView_f);

	mouse_x = mouse_y = 0.0;
	mouse_avail = true;

#if EMSCRIPTEN
	SDL_SetEventFilter(&FilterEvents, NULL);
	emscripten_set_pointerlockchange_callback(NULL, NULL, true, &q2_pointerlockchange);
#endif
}

void RW_IN_Shutdown(void) {
    if (mouse_avail) {
	mouse_avail = false;

	ri.Cmd_RemoveCommand ("+mlook");
	ri.Cmd_RemoveCommand ("-mlook");

	ri.Cmd_RemoveCommand ("force_centerview");
    }

#ifdef HAVE_JOYSTICK
    if (joy) {
	SDL_JoystickClose(joy);
	joy = NULL;
    }
#endif
}

/*
===========
IN_Commands
===========
*/
void RW_IN_Commands (void)
{
    int i;
#ifdef HAVE_JOYSTICK
    int key_index;
#endif
   
	// TODO: move this to a message loop somewhere
	// TODO: after that use message timestamp instead of Sys_Milliseconds
    if (mouse_avail) {
		for (i = 0; i < 3; i++) {
			if ( (mouse_buttonstate & (1<<i)) && !(mouse_oldbuttonstate & (1<<i)) )
				Key_Event(K_MOUSE1 + i, true, Sys_Milliseconds());

			if ( !(mouse_buttonstate & (1<<i)) && (mouse_oldbuttonstate & (1<<i)) )
				Key_Event(K_MOUSE1 + i, false, Sys_Milliseconds());
		}
		/* can't put in loop because K_MOUSE4 doesn't come after K_MOUSE3 */
		if ((mouse_buttonstate & (1<<3)) && !(mouse_oldbuttonstate & (1<<3)))
			Key_Event(K_MOUSE4, true, Sys_Milliseconds());
		if (!(mouse_buttonstate * (1<<3)) && (mouse_oldbuttonstate & (1<<3)))
			Key_Event(K_MOUSE4, false, Sys_Milliseconds());

		if ((mouse_buttonstate & (1<<4)) && !(mouse_oldbuttonstate & (1<<4)))
			Key_Event(K_MOUSE5, true, Sys_Milliseconds());
		if (!(mouse_buttonstate * (1<<4)) && (mouse_oldbuttonstate & (1<<4)))
			Key_Event(K_MOUSE5, false, Sys_Milliseconds());

		mouse_oldbuttonstate = mouse_buttonstate;
    }
#ifdef HAVE_JOYSTICK
    if (joystick_avail && joy) {
		for (i = 0; i < joy_numbuttons; i++) {
			if (SDL_JoystickGetButton(joy, i) && joy_oldbuttonstate != i) {
				key_index = (i < 4) ? K_JOY1 : K_AUX1;
				Key_Event(key_index + i, true, Sys_Milliseconds());
				joy_oldbuttonstate = i;
			}
			if (!SDL_JoystickGetButton(joy, i) && joy_oldbuttonstate != i) {
				key_index = (i < 4) ? K_JOY1 : K_AUX1;
				Key_Event(key_index + i, false, Sys_Milliseconds());
				joy_oldbuttonstate = i;
			}
		}
    }
#endif
}


/*
===========
IN_Move
===========
*/
void IN_Move (usercmd_t *cmd)
{
	/*** FIXME 
	 *   You can accelerate while in the air, this doesn't
	 *   make physical sense.  Try falling off something and then moving
	 *   forward.
	 ***/

	if (mouse_avail) {
		if (m_filter->value)
		{
			mouse_x = (mx + old_mouse_x) * 0.5;
			mouse_y = (my + old_mouse_y) * 0.5;
		} else {
			mouse_x = mx;
			mouse_y = my;
		}

		old_mouse_x = mx;
		old_mouse_y = my;

		if (mouse_x || mouse_y) {
			mouse_x *= sensitivity->value;
			mouse_y *= sensitivity->value;

			/* add mouse X/Y movement to cmd */
			if ( (*in_state->in_strafe_state & 1) ||
				(my_lookstrafe->value && mlooking ))
				cmd->sidemove += m_side->value * mouse_x;
			else
				in_state->viewangles[YAW] -= m_yaw->value * mouse_x;

			if ( (mlooking || my_freelook->value) &&
				!(*in_state->in_strafe_state & 1))
			{
				in_state->viewangles[PITCH] += m_pitch->value * mouse_y;
			}
			else
			{
				cmd->forwardmove -= m_forward->value * mouse_y;
			}
			mx = my = 0;
		}
	}
#ifdef HAVE_JOYSTICK
	if (joystick_avail && joy) {
		/* add joy X/Y movement to cmd */
		if ( (*in_state->in_strafe_state & 1) ||
			(my_lookstrafe->value && mlooking ))
			cmd->sidemove += m_side->value * (jx/100);
		else
			in_state->viewangles[YAW] -= m_yaw->value * (jx/100);

		if ((mlooking || my_freelook->value) && !(*in_state->in_strafe_state & 1)) {
			if (j_invert_y)
				in_state->viewangles[PITCH] -= m_pitch->value * (jy/100);
			else
				in_state->viewangles[PITCH] += m_pitch->value * (jy/100);
			cmd->forwardmove -= m_forward->value * (jt/100);
		} else {
			cmd->forwardmove -= m_forward->value * (jy/100);
		}
		jt = jx = jy = 0;
	}
#endif
}


void IN_DeactivateMouse( void ) 
{ 
	if (!mouse_avail)
		return;

	if (mouse_active) {
		/* uninstall_grabs(); */
		mouse_active = false;
	}
}

static void IN_ActivateMouse( void ) 
{
	if (!mouse_avail)
		return;

	if (!mouse_active) {
		mx = my = 0; // don't spazz
		/* install_grabs(); */
		mouse_active = true;
	}
}

void RW_IN_Frame (void)
{
}

void IN_Activate(qboolean active)
{
	/*	if (active || vidmode_active) */
	if (active)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse();
}

/*****************************************************************************/


static unsigned int XLateKey(SDL_Keycode keysym)
{
	unsigned int key = 0;
	switch(keysym) {

		case SDLK_KP_0:			key = K_KP_INS; break;
		case SDLK_KP_1:			key = K_KP_END; break;
		case SDLK_KP_2:			key = K_KP_DOWNARROW; break;
		case SDLK_KP_3:			key = K_KP_PGDN; break;
		case SDLK_KP_4:			key = K_KP_LEFTARROW; break;
		case SDLK_KP_5:			key = K_KP_5; break;
		case SDLK_KP_6:			key = K_KP_RIGHTARROW; break;
		case SDLK_KP_7:			key = K_KP_HOME; break;
		case SDLK_KP_8:			key = K_KP_UPARROW; break;
		case SDLK_KP_9:			key = K_KP_PGUP; break;
		
		/* suggestions on how to handle this better would be appreciated */
		case SDLK_BACKQUOTE:	key = '`'; break;

		// WTF: the above should get this but doesn't
		case 167			:	key = '`'; break;

		case SDLK_PAGEUP:		key = K_PGUP; break;
		
		case SDLK_PAGEDOWN:		key = K_PGDN; break;
		
		case SDLK_HOME:			key = K_HOME; break;
		
		case SDLK_END:			key = K_END; break;
		
		case SDLK_LEFT:			key = K_LEFTARROW; break;
		
		case SDLK_RIGHT:		key = K_RIGHTARROW; break;
		
		case SDLK_DOWN:			key = K_DOWNARROW; break;
		
		case SDLK_UP:			key = K_UPARROW; break;
		
		case SDLK_ESCAPE:		key = K_ESCAPE; break;
		
		case SDLK_KP_ENTER:		key = K_KP_ENTER; break;
		case SDLK_RETURN:		key = K_ENTER; break;
		
		case SDLK_TAB:			key = K_TAB; break;
		
		case SDLK_F1:			key = K_F1; break;
		case SDLK_F2:			key = K_F2; break;
		case SDLK_F3:			key = K_F3; break;
		case SDLK_F4:			key = K_F4; break;
		case SDLK_F5:			key = K_F5; break;
		case SDLK_F6:			key = K_F6; break;
		case SDLK_F7:			key = K_F7; break;
		case SDLK_F8:			key = K_F8; break;
		case SDLK_F9:			key = K_F9; break;
		case SDLK_F10:			key = K_F10; break;
		case SDLK_F11:			key = K_F11; break;
		case SDLK_F12:			key = K_F12; break;
		
		case SDLK_BACKSPACE:		key = K_BACKSPACE; break;
		
		case SDLK_KP_PERIOD:		key = K_KP_DEL; break;
		case SDLK_DELETE:		key = K_DEL; break;
		
		case SDLK_PAUSE:		key = K_PAUSE; break;
		
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:		key = K_SHIFT; break;
		
		case SDLK_LCTRL:
		case SDLK_RCTRL:		key = K_CTRL; break;
		
		case SDLK_LALT:
		case SDLK_RALT:			key = K_ALT; break;

		case SDLK_INSERT:		key = K_INS; break;
		
		case SDLK_KP_MULTIPLY:		key = '*'; break;
		case SDLK_KP_PLUS:		key = K_KP_PLUS; break;
		case SDLK_KP_MINUS:		key = K_KP_MINUS; break;
		case SDLK_KP_DIVIDE:		key = K_KP_SLASH; break;
		
		default: /* assuming that the other sdl keys are mapped to ascii */
			if (keysym < 128)
				key = keysym;
			break;
	}

	return key;		
}


static unsigned char KeyStates[SDL_NUM_SCANCODES];

#ifdef EMSCRIPTEN
EM_BOOL q2_pointerlockchange(int eventType, const EmscriptenPointerlockChangeEvent *pointerlockChangeEvent, void *userData)
{
	if (pointerlockChangeEvent->isActive) {
		SDL_Log("Pointer lock grabbed");
	} else {
		SDL_Log("Pointer lock lost");
		SDL_SetRelativeMouseMode(SDL_FALSE);
		ri.Cvar_SetValue( "_windowed_mouse", 0 );

		// release all keys so Alt etc. don't get stuck on
		for (unsigned int i = 0; i < SDL_NUM_SCANCODES; i++) {
			if (KeyStates[i]) {
				KeyStates[i] = 0;

				unsigned int key = XLateKey(SDL_SCANCODE_TO_KEYCODE(i));
				if (key) {
					keyq[keyq_head].key = key;
					keyq[keyq_head].down = false;
					keyq_head = (keyq_head + 1) & 63;
				}
			}
		}

	}
	return 1;
}

int FilterEvents(void* userdata, SDL_Event* event)
{
	switch (event->type) {
		case SDL_MOUSEBUTTONDOWN:
			if (SDL_GetRelativeMouseMode() != SDL_TRUE) {
				SDL_Log("Setting Relative + Grab");
				SDL_SetRelativeMouseMode(SDL_TRUE);
				ri.Cvar_SetValue( "_windowed_mouse", 1 );
			}
			break;
		default:
			break;
	}
	return 1;
}
#endif

void GetEvent(SDL_Event *event)
{
	unsigned int key;
	
	switch(event->type) {
	case SDL_MOUSEMOTION:
		mx += event->motion.xrel; my += event->motion.yrel;
		break;

	case SDL_MOUSEWHEEL:
		if (event->wheel.y > 0) {
			keyq[keyq_head].key = K_MWHEELUP;
			keyq[keyq_head].down = true;
			keyq_head = (keyq_head + 1) & 63;
			keyq[keyq_head].key = K_MWHEELUP;
			keyq[keyq_head].down = false;
			keyq_head = (keyq_head + 1) & 63;
		} else {
			keyq[keyq_head].key = K_MWHEELDOWN;
			keyq[keyq_head].down = true;
			keyq_head = (keyq_head + 1) & 63;
			keyq[keyq_head].key = K_MWHEELDOWN;
			keyq[keyq_head].down = false;
			keyq_head = (keyq_head + 1) & 63;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		break;
#ifdef HAVE_JOYSTICK
	case SDL_JOYBUTTONDOWN:
	  keyq[keyq_head].key = 
	    ((((SDL_JoyButtonEvent*)event)->button < 4)?K_JOY1:K_AUX1)+
	    ((SDL_JoyButtonEvent*)event)->button;
	  keyq[keyq_head].down = true;
	  keyq_head = (keyq_head+1)&63;
	  break;
	case SDL_JOYBUTTONUP:
	  keyq[keyq_head].key = 
	    ((((SDL_JoyButtonEvent*)event)->button < 4)?K_JOY1:K_AUX1)+
	    ((SDL_JoyButtonEvent*)event)->button;
	  keyq[keyq_head].down = false;
	  keyq_head = (keyq_head+1)&63;
	  break;
#endif
	case SDL_KEYDOWN:
		if ( (KeyStates[SDL_SCANCODE_LALT] || KeyStates[SDL_SCANCODE_RALT]) &&
			(event->key.keysym.sym == SDLK_RETURN) ) {
			cvar_t	*fullscreen;

			int flags = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;

			// this is AFTER the change
			int fs = flags ? 0 : 1;
			// TODO: support SDL_WINDOW_FULLSCREEN_DESKTOP
			flags = fs ? SDL_WINDOW_FULLSCREEN : 0;

			int retval = SDL_SetWindowFullscreen(window, flags);
			if (retval != 0) {
				VID_Printf(PRINT_ALL, "SDL_SetWindowFullscreen failed: \"%s\"\n", SDL_GetError());
				// not fatal but since fullscreen failed...
				fs = flags ? 1 : 0;
			}


			ri.Cvar_SetValue( "vid_fullscreen", fs );

			fullscreen = ri.Cvar_Get( "vid_fullscreen", "0", 0 );
			fullscreen->modified = false;	// we just changed it with SDL.

			break; /* ignore this key */
		}
		
		if ( (KeyStates[SDL_SCANCODE_LCTRL] || KeyStates[SDL_SCANCODE_RCTRL]) &&
			(event->key.keysym.sym == SDLK_g) ) {

			int newValue = (SDL_GetWindowGrab(window) == SDL_TRUE) ? /*1*/ 0 : /*0*/ 1;
			/*	
			SDL_WM_GrabInput((gm == SDL_GRAB_ON) ? SDL_GRAB_OFF : SDL_GRAB_ON);
			gm = SDL_WM_GrabInput(SDL_GRAB_QUERY);
			*/	
			ri.Cvar_SetValue( "_windowed_mouse", newValue );
			
			break; /* ignore this key */
		}

		KeyStates[event->key.keysym.scancode] = 1;
		
		key = XLateKey(event->key.keysym.sym);
		if (key) {
			keyq[keyq_head].key = key;
			keyq[keyq_head].down = true;
			keyq_head = (keyq_head + 1) & 63;
		}
		break;
	case SDL_KEYUP:
		if (KeyStates[event->key.keysym.scancode]) {
			KeyStates[event->key.keysym.scancode] = 0;
		
			key = XLateKey(event->key.keysym.sym);
			if (key) {
				keyq[keyq_head].key = key;
				keyq[keyq_head].down = false;
				keyq_head = (keyq_head + 1) & 63;
			}
		}
		break;

	case SDL_WINDOWEVENT:
		switch (event->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			viddef.width = event->window.data1;
			viddef.height = event->window.data2;
			VID_Printf (PRINT_ALL, "Window resized to %dx%d\n", viddef.width, viddef.height);
			Cvar_SetValue("vid_width", viddef.width);
			Cvar_SetValue("vid_height", viddef.height);
			vid_width->modified = false;
			vid_height->modified = false;
			break;

		}
		break;
	case SDL_QUIT:
		ri.Cmd_ExecuteText(EXEC_NOW, "quit");
		break;
	}

}

void init_joystick() {
#ifdef HAVE_JOYSTICK
    int num_joysticks, i;
    joy = NULL;

    if (!(SDL_INIT_JOYSTICK&SDL_WasInit(SDL_INIT_JOYSTICK))) {
	VID_Printf(PRINT_ALL, "SDL Joystick not initialized, trying to init...\n");
	SDL_Init(SDL_INIT_JOYSTICK);
    }
    if (in_joystick) {
	VID_Printf(PRINT_ALL, "Trying to start-up joystick...\n");
	if ((num_joysticks=SDL_NumJoysticks())) {
	    for(i=0;i<num_joysticks;i++) {
		VID_Printf(PRINT_ALL, "Trying joystick [%s]\n", 
			      SDL_JoystickName(i));
		if (!SDL_JoystickOpened(i)) {
		    joy = SDL_JoystickOpen(i);
		    if (joy) {
			VID_Printf(PRINT_ALL, "Joytick activated.\n");
			joystick_avail = true;
			joy_numbuttons = SDL_JoystickNumButtons(joy);
			break;
		    }
		}
	    }
	    if (!joy) {
		VID_Printf(PRINT_ALL, "Failed to open any joysticks\n");
		joystick_avail = false;
	    }
	}
	else {
	    VID_Printf(PRINT_ALL, "No joysticks available\n");
	    joystick_avail = false;
	}
    }
    else {
	VID_Printf(PRINT_ALL, "Joystick Inactive\n");
	joystick_avail = false;
    }
#endif
}

void InitJoystick() {
#ifdef HAVE_JOYSTICK
  int num_joysticks, i;
  joy = NULL;

  if (!(SDL_INIT_JOYSTICK&SDL_WasInit(SDL_INIT_JOYSTICK))) {
    VID_Printf(PRINT_ALL, "SDL Joystick not initialized, trying to init...\n");
    SDL_Init(SDL_INIT_JOYSTICK);
  }
  if (in_joystick) {
    VID_Printf(PRINT_ALL, "Trying to start-up joystick...\n");
    if ((num_joysticks=SDL_NumJoysticks())) {
      for(i=0;i<num_joysticks;i++) {
	VID_Printf(PRINT_ALL, "Trying joystick [%s]\n", 
		      SDL_JoystickName(i));
	if (!SDL_JoystickOpened(i)) {
	  joy = SDL_JoystickOpen(0);
	  if (joy) {
	    VID_Printf(PRINT_ALL, "Joytick activated.\n");
	    joystick_avail = true;
	    joy_numbuttons = SDL_JoystickNumButtons(joy);
	    break;
	  }
	}
      }
      if (!joy) {
	VID_Printf(PRINT_ALL, "Failed to open any joysticks\n");
	joystick_avail = false;
      }
    }
    else {
      VID_Printf(PRINT_ALL, "No joysticks available\n");
      joystick_avail = false;
    }
  }
  else {
    VID_Printf(PRINT_ALL, "Joystick Inactive\n");
    joystick_avail = false;
  }
#endif
}


#ifdef USE_GLEW


static const char *errorSource(GLenum source)
{
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		return "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "window system";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "shader compiler";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "third party";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		return "application";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		return "other";
		break;

	default:
		break;
	}

	return "unknown source";
}


static const char *errorType(GLenum type)
{
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
		return "error";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
		return "deprecated behavior";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
		return "undefined behavior";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		return "portability";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
		return "performance";
		break;

	case GL_DEBUG_TYPE_OTHER:
	case GL_DEBUG_CATEGORY_OTHER_AMD:
		return "other";
		break;

	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
		return "window system error";
		break;

	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
		return "shader compiler error";
		break;

	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
		return "application error";
		break;

	default:
		break;

	}

	return "unknown type";
}


void CALLBACK glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB:
		VID_Printf( PRINT_ALL, "GL error from %s type %s: (%d) %s\n", errorSource(source), errorType(type), id, message);
		break;

	case GL_DEBUG_SEVERITY_MEDIUM_ARB:
		VID_Printf( PRINT_ALL, "GL warning from %s type %s: (%d) %s\n", errorSource(source), errorType(type), id, message);
		break;

	case GL_DEBUG_SEVERITY_LOW_ARB:
		VID_Printf( PRINT_ALL, "GL debug from %s type %s: (%d) %s\n", errorSource(source), errorType(type), id, message);
		break;

	default:
		VID_Printf( PRINT_ALL, "GL error of unknown severity %x from %s type %s: (%d) %s\n", severity, errorSource(source), errorType(type), id, message);
		break;
	}
}


#endif  // USE_GLEW


/*
** GLimp_EndFrame
**
** This does an implementation specific copy from the backbuffer to the
** front buffer.  In the Win32 case it uses BitBlt or BltFast depending
** on whether we're using DIB sections/GDI or DDRAW.
*/

void GLimp_EndFrame (void)
{
	flushDraws("swapWindow");
	SDL_GL_SwapWindow(window);
}


/*
** GLimp_Shutdown
**
** System specific graphics subsystem shutdown routine.  Destroys
** DIBs or DDRAW surfaces as appropriate.
*/

void GLimp_Shutdown( void )
{
	if (glcontext) {
		SDL_GL_DeleteContext(glcontext);
		glcontext = NULL;
	}

	if (window) {
		SDL_DestroyWindow(window);
		window = NULL;
	}
	
	if (SDL_WasInit(SDL_INIT_EVERYTHING) == SDL_INIT_VIDEO)
		SDL_Quit();
	else
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		
	X11_active = false;
}


/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/


void KBD_Update(void)
{
	static int KBD_Update_Flag;

	if (KBD_Update_Flag == 1)
		return;

	KBD_Update_Flag = 1;

// get events from x server
	if (X11_active)
	{
#ifdef HAVE_JOYSTICK
		if (joystick_avail && joy) {
		  jx = SDL_JoystickGetAxis(joy, lr_axis);
		  jy = SDL_JoystickGetAxis(joy, ud_axis);
		  jt = SDL_JoystickGetAxis(joy, throttle_axis);
		}
#endif
		mouse_buttonstate = 0;
		int bstate = SDL_GetMouseState(NULL, NULL);
		if (SDL_BUTTON(1) & bstate)
			mouse_buttonstate |= (1 << 0);
		if (SDL_BUTTON(3) & bstate) /* quake2 has the right button be mouse2 */
			mouse_buttonstate |= (1 << 1);
		if (SDL_BUTTON(2) & bstate) /* quake2 has the middle button be mouse3 */
			mouse_buttonstate |= (1 << 2);
		if (SDL_BUTTON(6) & bstate)
			mouse_buttonstate |= (1 << 3);

		if (SDL_BUTTON(7) & bstate)
			mouse_buttonstate |= (1 << 4);

		if (old_windowed_mouse != _windowed_mouse->value) {
			old_windowed_mouse = _windowed_mouse->value;

			// TODO: should refactor all this grab stuff to one place
			SDL_SetWindowGrab(window, _windowed_mouse->value ? SDL_TRUE : SDL_FALSE);
			int retval = SDL_SetRelativeMouseMode(_windowed_mouse->value ? SDL_TRUE : SDL_FALSE);
			if (retval != 0) {
				VID_Printf (PRINT_ALL, "Failed to set relative mouse state \"%s\"\n", SDL_GetError());
			}
		}
		while (keyq_head != keyq_tail)
		{
			Key_Event(keyq[keyq_tail].key, keyq[keyq_tail].down, Sys_Milliseconds());
			keyq_tail = (keyq_tail + 1) & 63;
		}
	}

	KBD_Update_Flag = 0;
}

void KBD_Close(void)
{
	keyq_head = 0;
	keyq_tail = 0;
	
	memset(keyq, 0, sizeof(keyq));
}
