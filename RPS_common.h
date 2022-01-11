#pragma once

#define OLC_PGEX_NETWORK
#include "./lib/olcPGEX_Network.h"
#include "RPS_common_classes.h"

enum class GameMsg : uint32_t
{
	//messages sent from client to server
	Server_GetStatus,
	Server_GetPing,

	//responses sent from server to client
	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,

	//messages sent from client to server while in the network hub menu
	Menu_JoinLobby,
	Menu_LobbyFull, //sent when client tries to join a full lobby

	//messages sent from server to client while in the network hub menu
	Menu_ClientAddedToLobby,

	//messages sent from server to client while in the lobby
	Lobby_ClientRemovedFromLobby,
	Lobby_UpdateLobby,

	//messages sent from client to server while in the lobby
	Lobby_ChangePlayerState, //(Ready / Not ready)
	Lobby_LeaveLobby
};

