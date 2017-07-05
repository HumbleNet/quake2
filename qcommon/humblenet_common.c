//
//  humble_common.c
//  Quake2
//
//  Created by Chris Rudd on 3/9/16.
//
#ifdef USE_HUMBLENET

#include <humblenet_p2p.h>

static PeerId humblenet_peer = 0;
static cvar_t *net_peer_server;
static cvar_t *net_server_name;
static bool published = 0;

static bool is_server_active() {
    return ip_sockets[ NS_SERVER ] != 0;
}

static void NET_peer_server_changed(struct cvar_s *self, char *oldValue, char *newValue) {
	if( !humblenet_p2p_is_initialized() && newValue && *newValue ) {
		Com_Printf("initializeing p2p network...\n", LOG_GENERAL);
		if( 0 != humblenet_p2p_init( newValue, "quake2", "quake2-secret", NULL ) ) {
			Com_Printf("Error connecting to signal server: \"%s\"\n", LOG_GENERAL, NET_ErrorString());
		}
	} else if( humblenet_p2p_is_initialized() ) {
		Com_Printf("Error chaning peer server is not supported right now", LOG_GENERAL);
	} else {
		Com_Printf("What happened!: %d", LOG_GENERAL, humblenet_p2p_is_initialized());
	}
}

static void NET_server_name_changed(struct cvar_s *self, char *oldValue, char *newValue) {
	if( humblenet_p2p_is_initialized() && is_server_active() ) {
		humblenet_p2p_unregister_alias( NULL );
		humblenet_p2p_register_alias( newValue );

		published = newValue && *newValue;
		
		// Notify call connected clients?
		Com_Printf("Public server name changed to \"%s\"\n", LOG_GENERAL, newValue);
	}
}

//=============================================================================

#ifdef EMSCRIPTEN
#include <html5.h>
#include "../client/client.h"
const char* emUnloadCallback(int eventType, const void* reserved, void* userData) {
	CL_Disconnect(false);
	return "";
}
#endif

/*
 ====================
 HUMBLE_Init
 ====================
 */
void HUMBLENET_Init (void)
{
	net_no_recverr = Cvar_Get ("net_no_recverr", "0", 0);

#ifdef EMSCRIPTEN
	
	// on emscripten this is supposed to be specified by the server
	// don't save to config
	int net_peer_server_flags = 0;
	
#else  // EMSCRIPTEN
	
	// on native user must set it
	// save to config
	int net_peer_server_flags = CVAR_ARCHIVE;
	
#endif  // EMSCRIPRTEN
	
	net_peer_server = Cvar_Get("net_peer_server", "", net_peer_server_flags);
	net_peer_server->changed = NET_peer_server_changed;
	
	// fake a change so we initialize
	NET_peer_server_changed( net_peer_server, NULL, net_peer_server->string );
	
	net_server_name = Cvar_Get("net_server_name", "", CVAR_ARCHIVE);
	net_server_name->changed = NET_server_name_changed;
	
	if( !humblenet_p2p_is_initialized() ) {
		Com_Printf("P2P Network not initialized. <insert instructions to do so here>...\n", LOG_GENERAL);
	}
	
#ifdef EMSCRIPTEN
	// disconnect client when user closes tab
	emscripten_set_beforeunload_callback(NULL, emUnloadCallback);
#endif
}

void HUMBLENET_Update(void) {
	humblenet_p2p_update();
	
	if( humblenet_peer == 0 ) {
		humblenet_peer = humblenet_p2p_get_my_peer_id();
		if( humblenet_peer != 0) {
			Com_Printf("Assigned PeerId: %u...\n", LOG_GENERAL, humblenet_peer);
		}
	} else if( !published && is_server_active() && net_server_name->string && *net_server_name->string ) {
		humblenet_p2p_unregister_alias( NULL );
		humblenet_p2p_register_alias( net_server_name->string );
		published = true;
	} else if( published && !is_server_active() ) {
		humblenet_p2p_unregister_alias( NULL );
		published = false;
	}
}

#ifndef HAVE_NET_Update
#define HAVE_NET_Update 
void NET_Update(void) {
	HUMBLENET_Update();
}
#endif

#endif