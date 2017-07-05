#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif  // _WIN32

#include <SDL.h>

#include "server.h"


qboolean send_packet_now = false;

const bool is_dedicated = true;


#ifdef USE_HUMBLENET


#include "../client/client.h"


// net_humblenet needs these
// but never uses them when only server
void CL_Disconnect (qboolean skipdisconnect)
{
}


client_static_t cls;


#endif  // USE_HUMBLENET


void CL_Shutdown(void)
{
}


void CL_Drop (qboolean skipdisconnect, qboolean nonerror)
{
}


void CL_Init (void)
{
}


void CL_Frame (int msec)
{
}


const char *CL_Get_Loc_Here (void)
{
	return NULL;
}


const char *CL_Get_Loc_There (void)
{
	return NULL;
}


void Cmd_ForwardToServer (void)
{
}


void Con_Print (const char *txt)
{
}


void KBD_Update(void)
{
}


void Key_Init (void)
{
}


void SCR_BeginLoadingPlaque (void)
{
}


void EXPORT SCR_DebugGraph (float value, int color)
{
}


void SCR_EndLoadingPlaque (void)
{
}


// net_humblenet.cpp calls this
// not really, that code is dead on server
// have it here so we don't get link error
int SDL_SetClipboardText(const char* text) {
	return 0;
}

