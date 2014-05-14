// Main windowed and fullscreen graphics interface module. This module
// is used for both the software and OpenGL rendering versions of the
// Quake refresh engine.

#define SO_FILE "/etc/quake2.conf"

#include <assert.h>

#include "../client/client.h"
#include "../client/ref.h"


qboolean reload_video = false;

// Structure containing functions exported from refresh DLL
refexport_t	re;

// Console variables that we need to access from this module
cvar_t		*vid_gamma;
cvar_t		*vid_xpos;			// X coordinate of window position
cvar_t		*vid_ypos;			// Y coordinate of window position
cvar_t		*vid_fullscreen;

// Global variables used internally by this module
viddef_t	viddef;				// global video state; used by other modules
qboolean	reflib_active = 0;

#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[0] ) )


/** MOUSE *****************************************************************/

in_state_t in_state;

void Real_IN_Init (void);

/*
==========================================================================

DLL GLUE

==========================================================================
*/

#define	MAXPRINTMSG	4096
void VID_Printf (int print_level, const char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	
	va_start (argptr,fmt);
	vsnprintf (msg, sizeof(msg)-1, fmt,argptr);
	va_end (argptr);

	msg[sizeof(msg)-1] = 0;

	if (print_level == PRINT_ALL)
		Com_Printf ("%s", LOG_CLIENT, msg);
	else
		Com_DPrintf ("%s", msg);
}


void VID_Error (int err_level, const char *fmt, ...) __attribute__((format (printf, 2, 3), noreturn));
void VID_Error (int err_level, const char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	
	va_start (argptr,fmt);
	vsnprintf (msg, sizeof(msg)-1, fmt,argptr);
	va_end (argptr);

	msg[sizeof(msg)-1] = 0;

	Com_Error (err_level,"%s", msg);
}

//==========================================================================

/*
============
VID_Restart_f

TODO: remove
============
*/
void VID_Restart_f (void)
{
	reload_video = true;
}


/*
** VID_NewWindow
*/
void VID_NewWindow ( int width, int height)
{
	viddef.width  = width;
	viddef.height = height;
}

void VID_FreeReflib (void)
{
	KBD_Close();
	RW_IN_Shutdown();

	memset (&re, 0, sizeof(re));
	reflib_active  = false;
}

/*
==============
VID_LoadRefresh
==============
*/
qboolean VID_LoadRefresh( char *name )
{
	refimport_t		ri;

	if ( reflib_active )
	{
		KBD_Close();
		RW_IN_Shutdown();
		R_Shutdown();
		VID_FreeReflib ();
	}

	ri.Cmd_AddCommand = Cmd_AddCommand;
	ri.Cmd_RemoveCommand = Cmd_RemoveCommand;
	ri.Cmd_Argc = Cmd_Argc;
	ri.Cmd_Argv = Cmd_Argv;
	ri.Cmd_ExecuteText = Cbuf_ExecuteText;
	ri.Sys_Error = VID_Error;
	ri.FS_LoadFile = FS_LoadFile;
	ri.FS_FreeFile = FS_FreeFile;
	ri.FS_Gamedir = FS_Gamedir;
	ri.Cvar_Get = Cvar_Get;
	ri.Cvar_Set = Cvar_Set;
	ri.Cvar_SetValue = Cvar_SetValue;
	ri.Vid_MenuInit = VID_MenuInit;
	ri.Vid_NewWindow = VID_NewWindow;

	re = GetRefAPI( ri );

	if (re.api_version != API_VERSION)
	{
		VID_FreeReflib ();
		Com_Error (ERR_FATAL, "%s has incompatible api_version", name);
	}

	/* Init IN (Mouse) */
	in_state.IN_CenterView_fp = IN_CenterView;
	in_state.viewangles = cl.viewangles;
	in_state.in_strafe_state = &in_strafe.state;

	Real_IN_Init();

	if ( R_Init( 0, 0 ) == -1 )
	{
		R_Shutdown();
		VID_FreeReflib ();
		return false;
	}

	Com_Printf( "------------------------------------\n", LOG_CLIENT);
	reflib_active = true;
	return true;
}

/*
============
VID_CheckChanges

This function gets called once just before drawing each frame, and it's sole purpose in life
is to check to see if any of the video mode parameters have changed, and if they have to 
update the rendering DLL and/or video mode to match.
============
*/
void VID_ReloadRefresh (void)
{
	S_StopAllSounds();

	/*
	** refresh has changed
	*/
	//vid_fullscreen->modified = true;
	cl.refresh_prepped = false;
	cl.frame.valid = false;
	cls.disable_screen = true;

	if ( !VID_LoadRefresh( "" ) )
	{
		/*
		** drop the console if we fail to load a refresh
		*/
		if ( cls.key_dest != key_console )
		{
			Con_ToggleConsole_f();
		}
	}
	cls.disable_screen = false;
}

/*
============
VID_Init
============
*/
void VID_Init (void)
{
	vid_xpos = Cvar_Get ("vid_xpos", "3", CVAR_ARCHIVE);
	vid_ypos = Cvar_Get ("vid_ypos", "22", CVAR_ARCHIVE);
	vid_fullscreen = Cvar_Get ("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = Cvar_Get( "vid_gamma", "1", CVAR_ARCHIVE );

	/* Add some console commands that we want to handle */
	Cmd_AddCommand ("vid_restart", VID_Restart_f);

	/* Start the graphics mode and load refresh DLL */
	//VID_CheckChanges();
	VID_ReloadRefresh();
}

/*
============
VID_Shutdown
============
*/
void VID_Shutdown (void)
{
	if ( reflib_active )
	{
		KBD_Close();
		RW_IN_Shutdown();
		R_Shutdown ();
		VID_FreeReflib ();
	}
}


/*****************************************************************************/
/* INPUT                                                                     */
/*****************************************************************************/

cvar_t	*in_joystick;

// This if fake, it's acutally done by the Refresh load
void IN_Init (void)
{
#ifdef JOYSTICK
	in_joystick	= Cvar_Get ("in_joystick", "0", CVAR_ARCHIVE);
#endif
}

void Real_IN_Init (void)
{
	RW_IN_Init(&in_state);
}

void IN_Shutdown (void)
{
	RW_IN_Shutdown();
}

void IN_Commands (void)
{
	RW_IN_Commands();
}


void IN_Frame (void)
{
	RW_IN_Frame();
}
