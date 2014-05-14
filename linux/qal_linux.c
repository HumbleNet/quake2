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


// qal_win.c -- binding of AL to QAL function pointers


#include "../client/snd_loc.h"

qboolean	openal_active;


// =====================================================================



/*
 =================
 QAL_Shutdown

 Unloads the specified DLL then nulls out all the proc pointers
 =================
*/
void QAL_Shutdown (void){

	Com_Printf("...shutting down QAL\n", LOG_CLIENT);

	openal_active = false;

}

/*
 =================
 QAL_Init

 Binds our QAL function pointers to the appropriate AL stuff
 =================
*/
qboolean QAL_Init (){

	Com_Printf("...initializing QAL\n", LOG_CLIENT);


	openal_active = true;

	return true;
}
