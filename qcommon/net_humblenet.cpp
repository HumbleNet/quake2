#ifndef USE_HUMBLENET
#error "humblenet-only file"
#endif

#if !defined USE_UDP

// Derived from
// net_wins.c

// quake2 dont know about C++, se wrap in extern statement
extern "C" {
#include "../qcommon/qcommon.h"
}

extern "C" {
#define	NO_NATIVE_SOCKETS
#include "humblenet_socket.h"
}

static unsigned int net_inittime;

static unsigned long long net_total_in;
static unsigned long long net_total_out;
static unsigned long long net_packets_in;
static unsigned long long net_packets_out;

int			server_port;
//netadr_t	net_local_adr;

static int			ip_sockets[2];

const char *NET_ErrorString (void);

cvar_t	*net_no_recverr;

// quake2 dont know about C++, se wrap in extern statement
extern "C" {
//Aiee...
#include "../qcommon/humblenet_common.c"
#include "../qcommon/net_common.c"
}

void Net_Stats_f (void)
{
	int now = time(0);
	int diff = now - net_inittime;

	Com_Printf ("Network up for %i seconds.\n"
				"%llu bytes in %llu packets received (av: %i kbps)\n"
				"%llu bytes in %llu packets sent (av: %i kbps)\n", LOG_NET,
				
				diff,
				net_total_in, net_packets_in, (int)(((net_total_in * 8) / 1024) / diff),
				net_total_out, net_packets_out, (int)((net_total_out * 8) / 1024) / diff);
}

//=============================================================================

int	NET_GetPacket (netsrc_t sock, netadr_t *net_from, sizebuf_t *net_message)
{
	int 	ret;
	struct sockaddr_in	from;
	uint32	fromlen;
	int		net_socket;
	
#ifndef DEDICATED_ONLY
	if (NET_GetLoopPacket (sock, net_from, net_message))
		return 1;
#endif

	net_socket = ip_sockets[sock];

	if (!net_socket)
		return 0;
	
	fromlen = sizeof(from);

	ret = recvfrom (net_socket, net_message->data, net_message->maxsize
		, 0, (struct sockaddr *)&from, &fromlen);

	if (ret == -1)
	{
		if( errno == EAGAIN || errno == EWOULDBLOCK )
			return 0;
		
		Com_DPrintf ("NET_GetPacket: recvmsg() error : %s\n", NET_ErrorString());

		Com_DPrintf ("(msgname) family %d, host: %s, port: %d\n", from.sin_family, inet_ntoa (from.sin_addr), from.sin_port);
		
		if( errno == ECONNRESET )
			return -1;
		
		return 0;
	}
	
	net_packets_in++;
	net_total_in += ret;

	SockadrToNetadr (&from, net_from);

	if (ret == net_message->maxsize)
	{
		Com_Printf ("Oversize packet from %s\n", LOG_NET, NET_AdrToString (net_from));
		return 0;
	}

	net_message->cursize = ret;
	
	return 1;
}


//=============================================================================

int NET_SendPacket (netsrc_t sock, int length, const void *data, const netadr_t *to)
{
	int		ret;
	struct sockaddr_in	addr;
	int		net_socket;

	if (to->type == NA_IP)
	{
		net_socket = ip_sockets[sock];
		if (!net_socket)
			return 0;
	}
#ifndef DEDICATED_ONLY
	else if ( to->type == NA_LOOPBACK )
	{
		NET_SendLoopPacket (sock, length, data);
		return 1;
	}
#endif
	else if (to->type == NA_BROADCAST)
	{
		net_socket = ip_sockets[sock];
		if (!net_socket)
			return 0;
	}
	else
	{
		Com_Error (ERR_FATAL, "NET_SendPacket: bad address type");
		return 0;
	}

	NetadrToSockadr (to, &addr);

	ret = sendto (net_socket, data, length, 0, (struct sockaddr *)&addr, sizeof(addr) );
	if (ret == -1)
	{
		Com_Printf ("NET_SendPacket to %s: ERROR: %s\n", LOG_NET, NET_AdrToString(to), NET_ErrorString());
		
		if( errno == ECONNRESET )
			return -1;
		
		return 0;
	}

	net_packets_out++;
	net_total_out += ret;
	return 1;
}

//=============================================================================

/*
====================
NET_Init
====================
*/
void NET_Init (void)
{
	NET_Common_Init ();
	net_no_recverr = Cvar_Get ("net_no_recverr", "0", 0);
	
	HUMBLENET_Init();
}


/*
====================
NET_Socket
====================
*/
int NET_IPSocket (char *net_interface, int port)
{
	if( !humblenet_p2p_is_initialized() )
		return 0;

	// fake the socket...
	return HUMBLENET_SOCKET;
}


/*
====================
NET_Shutdown
====================
*/
void	NET_Shutdown (void)
{
	NET_Config (NET_NONE);	// close sockets
}


/*
====================
NET_ErrorString
====================
*/
const char *NET_ErrorString (void)
{
	return humblenet_get_error();
	/*
	int		code;

	code = errno;
	return strerror (code);
	*/
}
#endif