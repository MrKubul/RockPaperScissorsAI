Message patterns:
client connect:
(server): Client_Accepted
(client): Client_RegisterWithServer
(server): Client_AssignID

join lobby:
(client): Lobby_JoinLobby
if lobby is full: (server): Menu_LobbyFull
otherwise:        (server): Menu_ClientAddedToLobby

leave lobby:
(client): Lobby_LeaveLobby
(server): Lobby_ClientRemovedFromLobby

change state in lobby (ready / not ready):
(client): Lobby_ChangePlayerState
(server): Lobby_UpdateLobby

TO DO:
change the lobby class to hold pointers to player objects rather than copies