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


#include "../client/snd_loc.h"


alConfig_t	alConfig;
alState_t	alState;


/*
 =================
 AL_InitExtensions
 =================
*/
static void AL_InitExtensions (void)
{
	if (!s_openal_extensions->intvalue)
	{
		Com_Printf("*** IGNORING OPENAL EXTENSIONS ***\n", LOG_CLIENT);
		return;
	}

	Com_Printf("Initializing OpenAL extensions\n", LOG_CLIENT);

	if (alIsExtensionPresent("EAX2.0"))
	{
		if (s_openal_eax->intvalue)
		{
			alConfig.eax = true;

			Com_Printf("...using EAX2.0\n", LOG_CLIENT);
		}
		else
			Com_Printf("...ignoring EAX2.0\n", LOG_CLIENT);
	}
	else
		Com_Printf("...EAX2.0 not found\n", LOG_CLIENT);
}

/*
 =================
 AL_InitDriver
 =================
*/
static qboolean AL_InitDriver (void)
{
	char	*deviceName;

	Com_Printf("Initializing OpenAL driver\n", LOG_CLIENT);

	// Open the device
	deviceName = s_openal_device->string;

	if (!deviceName[0])
		deviceName = NULL;

	if (deviceName)
		Com_Printf("...opening device (%s): ", LOG_CLIENT, deviceName);
	else
		Com_Printf("...opening device: ", LOG_CLIENT);

	if ((alState.hDevice = alcOpenDevice(deviceName)) == NULL)
	{
		Com_Printf("failed\n", LOG_CLIENT);
		return false;
	}

	if (!deviceName)
		Com_Printf("succeeded (%s)\n", LOG_CLIENT, alcGetString(alState.hDevice, ALC_DEVICE_SPECIFIER));
	else
		Com_Printf("succeeded\n", LOG_CLIENT);

	// Create the AL context and make it current
	Com_Printf("...creating AL context: ", LOG_CLIENT);
	if ((alState.hALC = alcCreateContext(alState.hDevice, NULL)) == NULL)
	{
		Com_Printf("failed\n", LOG_CLIENT);
		goto failed;
	}
	Com_Printf("succeeded\n", LOG_CLIENT);

	// turol: FIXME: this is broken
	Com_Printf("...making context current: ", LOG_CLIENT);
	if (!alcMakeContextCurrent(alState.hALC))
	{
		Com_Printf("failed\n", LOG_CLIENT);
		goto failed;
	}
	Com_Printf("succeeded\n", LOG_CLIENT);

	return true;

failed:

	Com_Printf("...failed hard\n", LOG_CLIENT);

	if (alState.hALC)
	{
		alcDestroyContext(alState.hALC);
		alState.hALC = NULL;
	}

	if (alState.hDevice)
	{
		alcCloseDevice(alState.hDevice);
		alState.hDevice = NULL;
	}

	return false;
}

/*
 =================
 AL_StartOpenAL
 =================
*/
static qboolean AL_StartOpenAL ()
{
	// Initialize our QAL dynamic bindings
	if (!QAL_Init())
		return false;

	// Get device list
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
		alConfig.deviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	else
		alConfig.deviceList = "DirectSound3D\0DirectSound\0MMSYSTEM\0\0";

	// Initialize the device, context, etc...
	if (AL_InitDriver())
		return true;

	// Shutdown QAL
	QAL_Shutdown();

	return false;
}

/*
 =================
 AL_Init
 =================
*/
qboolean AL_Init (void){

	Com_Printf("Initializing OpenAL subsystem\n", LOG_CLIENT);

	// Initialize OpenAL subsystem
	if (!AL_StartOpenAL())
	{
		// Let the user continue without sound
		Com_Printf ("WARNING: OpenAL initialization failed\n", LOG_CLIENT|LOG_WARNING);
		return false;
	}

	// Get AL strings
	alConfig.vendorString = alGetString(AL_VENDOR);
	alConfig.rendererString = alGetString(AL_RENDERER);
	alConfig.versionString = alGetString(AL_VERSION);
	alConfig.extensionsString = alGetString(AL_EXTENSIONS);

	// Get device name
	alConfig.deviceName = alcGetString(alState.hDevice, ALC_DEVICE_SPECIFIER);

	// Initialize extensions
	AL_InitExtensions();

	return true;
}

/*
 =================
 AL_Shutdown
 =================
*/
void AL_Shutdown (void){

	Com_Printf("Shutting down OpenAL subsystem\n", LOG_CLIENT);

	if (alState.hALC)
	{
		Com_Printf("...alcMakeContextCurrent( NULL ): ", LOG_CLIENT);
		if (!alcMakeContextCurrent(NULL))
			Com_Printf("failed\n", LOG_CLIENT);
		else
			Com_Printf("succeeded\n", LOG_CLIENT);

		Com_Printf("...destroying AL context\n", LOG_CLIENT);
		alcDestroyContext(alState.hALC);

		alState.hALC = NULL;
	}

	if (alState.hDevice)
	{
		Com_Printf("...closing device\n", LOG_CLIENT);
		alcCloseDevice(alState.hDevice);

		alState.hDevice = NULL;
	}

	QAL_Shutdown();

	memset(&alConfig, 0, sizeof(alConfig_t));
	memset(&alState, 0, sizeof(alState_t));
}

