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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#  include <windows.h>

#ifndef _MSC_VER
// on mingw we must include this before gl_rmain.c include SDL.h
// or it blows up for some reason
#include <intrin.h>
#endif  // _MSC_VER

#endif  // _WIN32

#ifdef USE_GLEW

#include <GL/glew.h>

#else  // USE_GLEW


#define GL_GLEXT_PROTOTYPES 1

#include <GL/gl.h>
#include <GL/glext.h>
#include <stdbool.h>


static const bool GLEW_GREMEDY_string_marker = false;


#endif  // USE_GLEW

#include <stdio.h>
#include <math.h>

#include "../client/ref.h"


void     QGL_Shutdown( void );

#ifndef APIENTRY
#  define APIENTRY
#endif

//extern  void ( APIENTRY * qglAccum )(GLenum op, GLfloat value);
void qglAlphaFunc(GLenum func, GLclampf ref);
void qglBegin(GLenum mode);
void qglBindTexture(GLenum target, GLuint texture);
void qglColor3f(GLfloat red, GLfloat green, GLfloat blue);
void qglColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void qglDepthRange(GLclampd zNear, GLclampd zFar);
void qglDisable(GLenum cap);
void qglEnable(GLenum cap);
void qglEnd(void);
void qglFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void qglGetFloatv(GLenum pname, GLfloat *params);
void qglLoadIdentity(void);
void qglLoadMatrixf(const GLfloat *m);
void qglMatrixMode(GLenum mode);
void qglMultMatrixf(const GLfloat *m);
void qglMTexCoord2f(GLenum tex, GLfloat s, GLfloat t);
void qglOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void qglPopMatrix(void);
void qglPushMatrix(void);
void qglRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void qglScalef(GLfloat x, GLfloat y, GLfloat z);
void qglTexEnvi(GLenum target, GLenum pname, GLint param);
void qglTranslatef(GLfloat x, GLfloat y, GLfloat z);
void qglVertex2f(GLfloat x, GLfloat y);
void qglVertex3f(GLfloat x, GLfloat y, GLfloat z);

void qglActiveTexture(GLenum);


void flushDraws(const char *reason);


//please keep this undefined on modified versions.
//#define R1GL_RELEASE 1

#ifdef R1GL_RELEASE
#define	REF_VERSION	"R1GL 0.1.5.42"
#else
#define REF_VERSION "EMGL015"
#endif

#define	MAX_TEXTURE_DIMENSIONS	1024

// up / down
#define	PITCH	0

// left / right
#define	YAW		1

// fall over
#define	ROLL	2

typedef float vec4_t[4];

/*

  skins will be outline flood filled and mip mapped
  pics and sprites with alpha will be outline flood filled
  pic won't be mip mapped

  model skin
  sprite frame
  wall texture
  pic

*/

typedef enum 
{
	it_skin,
	it_sprite,
	it_wall,
	it_pic,
	it_sky
} imagetype_t;

typedef struct image_s
{
	char	name[MAX_QPATH];			// game path, including extension
	char	basename[MAX_QPATH];				// as referenced by texinfo
	imagetype_t	type;
	int		width, height;				// source image
	int		upload_width, upload_height;	// after power of two and picmip
	int		registration_sequence;		// 0 = free
	struct msurface_s	*texturechain;	// for sort-by-texture world drawing
	GLuint texnum;						// gl texture binding
	//int		detailtexnum;
	float	sl, tl, sh, th;				// 0,0 - 1,1 unless part of the scrap
	//qboolean	scrap;
	int		has_alpha;
	//unsigned int hash;
	struct image_s	*hash_next;
} image_t;

#define	TEXNUM_LIGHTMAPS	1024
#define	TEXNUM_SCRAPS		1152
//#define TEXNUM_DETAIL		5555
#define		MAX_GLTEXTURES	1024

#define	MAX_LIGHTMAPS	128


//extern	cvar_t	*con_alpha;

extern	vec4_t	colorWhite;

extern	qboolean load_png_pics;
extern	qboolean load_tga_pics;
extern	qboolean load_jpg_pics;

extern	qboolean load_png_wals;
extern	qboolean load_tga_wals;
extern	qboolean load_jpg_wals;


//===================================================================

#define	VID_ERR_NONE				0
#define	VID_ERR_FAIL				1
#define	VID_ERR_RETRY_QGL			2
#define VID_ERR_FULLSCREEN_FAILED	4
#define VID_ERR_INVALID_MODE		8

#include "gl_model.h"

void GL_BeginRendering (int *x, int *y, int *width, int *height);
void GL_EndRendering (void);

void GL_SetDefaultState( void );
void GL_UpdateSwapInterval( void );

extern	double	gldepthmin, gldepthmax;

typedef struct
{
	float	x, y, z;
	float	s, t;
	float	r, g, b;
} glvert_t;


typedef struct Vertex {
	float pos[3];
	uint32_t color;
	float tex0[2];
	float tex1[2];
} Vertex;


#define NUMMATRICES 32


typedef struct ShaderTexState {
	bool texEnable;
	GLenum texMode;
} ShaderTexState;


typedef struct ShaderState {
	bool alphaTest;
	GLenum alphaFunc;
	float alphaRef;

	ShaderTexState texState[2];
} ShaderState;


struct Shader;

typedef struct Shader {
	ShaderState key;
	GLuint program;

	GLint mvpUniform;
	GLint alphaRefUniform;

	struct Shader *next;
} Shader;


typedef struct PipelineState {
	bool blend;
	// TODO: blend func
	// TODO: depth test, depth write
	// TODO: cull
	// TODO: scissor
	// TODO: stencil
} PipelineState;


typedef struct DrawCall {
	GLenum primitive;
	unsigned int firstVert;
	unsigned int numVertices;
} DrawCall;


#define NUMVBOS 256


typedef struct QGLState {
	PipelineState wantPipeline;
	PipelineState activePipeline;
	bool pipelineDirty;

	Vertex *vertices;
	unsigned int numVertices;
	unsigned int usedVertices;

	GLenum primitive;

	Vertex currentVertex;

	unsigned int clientActiveTexture;
	unsigned int wantActiveTexture, activeTexture;

	GLenum matrixMode;

	int mvMatrixTop, projMatrixTop;
	bool mvMatrixDirty, projMatrixDirty;

	float zNear, zFar;

	// this is index into the array, not a VBO id
	unsigned int currentVBOidx;

	bool shaderDirty;
	ShaderState wantShader;
	Shader *activeShader;

	Shader *shaders;

	DrawCall *drawCalls;
	unsigned int numDrawCalls, maxDrawCalls;

	unsigned int currentDrawFirstVertex;

	float mvMatrices[NUMMATRICES][16];
	float projMatrices[NUMMATRICES][16];

	GLuint vbos[NUMVBOS];
} QGLState;


extern QGLState *qglState;


#define	MAX_LBM_HEIGHT		480

#define BACKFACE_EPSILON	0.01


//====================================================

extern	image_t		gltextures[MAX_GLTEXTURES];
extern	int			numgltextures;


extern	image_t		*r_notexture;
extern	image_t		*r_particletexture;
extern	entity_t	*currententity;
extern	model_t		*currentmodel;
extern	int			r_visframecount;
extern	int			r_framecount;
extern	cplane_t	frustum[4];
extern	int			c_brush_polys, c_alias_polys;


extern	int			gl_filter_min, gl_filter_max;

extern	qboolean	r_registering;

//
// view origin
//
extern	vec3_t	vup;
extern	vec3_t	vpn;
extern	vec3_t	vright;
extern	vec3_t	r_origin;

//
// screen size info
//
extern	refdef_t	r_newrefdef;
extern	int		r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

extern	cvar_t	*r_norefresh;
extern	cvar_t	*r_lefthand;
extern	cvar_t	*r_drawentities;
extern	cvar_t	*r_drawworld;
extern	cvar_t	*r_speeds;
extern	cvar_t	*r_fullbright;
extern	cvar_t	*r_novis;
extern	cvar_t	*r_nocull;
extern	cvar_t	*r_lerpmodels;

extern	cvar_t	*r_lightlevel;	// FIXME: This is a HACK to get the client's light level

//extern cvar_t	*gl_ext_swapinterval;
extern cvar_t	*gl_ext_multitexture;
extern cvar_t	*gl_ext_pointparameters;
//extern cvar_t	*gl_ext_compiled_vertex_array;

//r1ch: my extensions
//extern cvar_t	*gl_ext_generate_mipmap;
extern cvar_t	*gl_ext_point_sprite;
extern cvar_t	*gl_ext_texture_filter_anisotropic;
extern cvar_t	*gl_ext_texture_non_power_of_two;
extern cvar_t	*gl_ext_max_anisotropy;
extern cvar_t	*gl_ext_occlusion_query;

extern cvar_t	*gl_colorbits;
extern cvar_t	*gl_alphabits;
extern cvar_t	*gl_depthbits;
extern cvar_t	*gl_stencilbits;

extern cvar_t	*gl_ext_multisample;
extern cvar_t	*gl_ext_samples;

extern cvar_t	*gl_r1gl_test;
extern cvar_t	*gl_doublelight_entities;
extern cvar_t	*gl_noscrap;
extern cvar_t	*gl_zfar;
extern cvar_t	*gl_overbrights;
extern cvar_t	*gl_linear_mipmaps;

extern cvar_t	*vid_gamma_pics;

extern cvar_t	*vid_topmost;

extern cvar_t	*gl_particle_min_size;
extern cvar_t	*gl_particle_max_size;
extern cvar_t	*gl_particle_size;
extern cvar_t	*gl_particle_att_a;
extern cvar_t	*gl_particle_att_b;
extern cvar_t	*gl_particle_att_c;

//extern	cvar_t	*gl_nosubimage;
extern	cvar_t	*gl_bitdepth;
//extern	cvar_t	*gl_lightmap;
extern	cvar_t	*gl_shadows;
extern	cvar_t	*gl_dynamic;
//extern  cvar_t  *gl_monolightmap;
extern	cvar_t	*gl_nobind;
extern	cvar_t	*gl_round_down;
extern	cvar_t	*gl_picmip;
extern	cvar_t	*gl_skymip;
extern	cvar_t	*gl_showtris;
extern	cvar_t	*gl_finish;
extern	cvar_t	*gl_clear;
extern	cvar_t	*gl_cull;
//extern	cvar_t	*gl_poly;
//extern	cvar_t	*gl_texsort;
extern	cvar_t	*gl_polyblend;
extern	cvar_t	*gl_flashblend;
//extern	cvar_t	*gl_lightmaptype;
extern	cvar_t	*gl_modulate;
//extern	cvar_t	*gl_playermip;
extern	cvar_t	*gl_drawbuffer;
//extern	cvar_t	*gl_3dlabs_broken;
extern	cvar_t	*gl_swapinterval;
extern	cvar_t	*gl_texturemode;
//extern  cvar_t  *gl_saturatelighting;
extern  cvar_t  *gl_lockpvs;

extern	cvar_t	*vid_fullscreen;
extern	cvar_t	*vid_gamma;

extern	cvar_t	*gl_jpg_quality;
extern	cvar_t	*gl_coloredlightmaps;

extern	cvar_t	*intensity;

extern	cvar_t	*gl_dlight_falloff;
extern	cvar_t	*gl_alphaskins;

extern	cvar_t	*gl_pic_scale;

extern	cvar_t	*vid_restore_on_switch;

extern int		usingmodifiedlightmaps;

extern	const int		gl_solid_format;
extern	const int		gl_alpha_format;
extern	int		gl_tex_solid_format;
extern	int		gl_tex_alpha_format;

extern	int		global_hax_texture_x;
extern	int		global_hax_texture_y;

extern	int		c_visible_lightmaps;
extern	int		c_visible_textures;

extern	float	r_world_matrix[16];

void R_TranslatePlayerSkin (int playernum);
void GL_MBind( GLenum target, unsigned int texnum );
void GL_TexEnv( GLenum target, GLenum value );
void GL_EnableMultitexture( qboolean enable );
void GL_SelectTexture( GLenum );

void R_LightPoint (vec3_t p, vec3_t color);
void R_PushDlights (void);
void clearImageHash(void);
unsigned int hashify (const char *S);
//====================================================================

extern	model_t	*r_worldmodel;

extern	unsigned	d_8to24table[256];
extern	vec4_t		d_8to24float[256];

extern	int		registration_sequence;


void V_AddBlend (float r, float g, float b, float a, float *v_blend);

void R_RenderView (refdef_t *fd);
void GL_ScreenShot_f (void);
void R_DrawAliasModel (entity_t *e);
void R_DrawBrushModel (entity_t *e);
void R_DrawSpriteModel (entity_t *e);
void R_DrawBeam( entity_t *e );
void R_DrawWorld (void);
void R_RenderDlights (void);
void R_DrawAlphaSurfaces (void);
void R_RenderBrushPoly (msurface_t *fa);
void R_InitParticleTexture (void);
void Draw_InitLocal (void);
void GL_SubdivideSurface (msurface_t *fa);
qboolean R_CullBox (vec3_t mins, vec3_t maxs);
void R_RotateForEntity (entity_t *e);
void R_MarkLeaves (void);

glpoly_t *WaterWarpPolyVerts (glpoly_t *p);
void EmitWaterPolys (msurface_t *fa);
void R_AddSkySurface (msurface_t *fa);
void R_ClearSkyBox (void);
void R_DrawSkyBox (void);
void R_MarkLights (dlight_t *light, int bit, mnode_t *node);

#if 0
short LittleShort (short l);
short BigShort (short l);
int	LittleLong (int l);
float LittleFloat (float f);

char	*va(char *format, ...);
// does a varargs printf into a temp buffer
#endif

//void COM_StripExtension (char *in, char *out);

void	Draw_GetPicSize (int *w, int *h, char *name);
void	Draw_Pic (int x, int y, char *name);
void	Draw_StretchPic (int x, int y, int w, int h, char *name);
void	Draw_TileClear (int x, int y, int w, int h, char *name);
void	Draw_Fill (int x, int y, int w, int h, int c);
void	Draw_FadeScreen (void);
void	Draw_StretchRaw (int x, int y, int w, int h, int cols, int rows, byte *data);

int		Draw_GetPalette (void);

void GL_ResampleTexture (unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight);

struct image_s * R_RegisterSkin (char *name);

void LoadPCX (const char *filename, byte **pic, byte **palette, int *width, int *height);
image_t *GL_LoadPic (const char *name, byte *pic, int width, int height, imagetype_t type, int bits);
image_t	*GL_FindImage (const char *name, const char *basename, imagetype_t type);
image_t	*GL_FindImageBase (const char *basename, imagetype_t type);
void	GL_TextureMode(const char *string);
void	GL_ImageList_f (void);
void	GL_Version_f (void);

//void	GL_SetTexturePalette( unsigned palette[256] );

void	GL_InitImages (void);
void	GL_ShutdownImages (void);

void	GL_FreeUnusedImages (void);


/*
** GL extension emulation functions
*/
void GL_DrawParticles( int n, const particle_t particles[] );

void EmptyImageCache (void);

/*
** GL config stuff
*/

typedef struct
{
	const char *renderer_string;
	const char *vendor_string;
	const char *version_string;
	const char *extensions_string;

	//qboolean	r1gl_GL_SGIS_generate_mipmap;
	qboolean	r1gl_GL_EXT_texture_filter_anisotropic;
	qboolean	r1gl_GL_ARB_texture_non_power_of_two;
	qboolean	wglPFD;

	int			bitDepth;
} glconfig_t;

typedef struct
{
	float inverse_intensity;
	qboolean fullscreen;

	unsigned char *d_16to8table;

	GLuint lightmap_textures[MAX_LIGHTMAPS];

	unsigned	currenttextures[2];
	unsigned int currenttmu;
	GLenum currenttarget;

	unsigned char originalRedGammaTable[256];
	unsigned char originalGreenGammaTable[256];
	unsigned char originalBlueGammaTable[256];

	qboolean hwgamma;
} glstate_t;

extern double vid_scaled_width, vid_scaled_height;

extern glconfig_t  gl_config;
extern glstate_t   gl_state;

/*
====================================================================

IMPORTED FUNCTIONS

====================================================================
*/

extern	refimport_t		ri;

/*
====================================================================

IMPLEMENTATION SPECIFIC FUNCTIONS

====================================================================
*/

void		GLimp_Shutdown( void );
void		GLimp_EnableLogging( qboolean enable );
void		GLimp_LogNewFrame( void );

