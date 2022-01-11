#include <iostream>
#include "./RPS_common.h"
#include <utility>

using std::cout, std::endl, std::cin;


class RPSserver : public olc::net::server_interface<GameMsg>
{
public:
	RPSserver(uint16_t Port) : olc::net::server_interface<GameMsg>(Port)
	{

	}
	~RPSserver()
	{

	}
	void MessageClientsInLobby(uint32_t lobbyID, olc::net::message<GameMsg>& msg)
	{
		for (auto& client : m_deqConnections)
		{
			// Check client is connected...
			if (client && client->IsConnected())
			{
				// ..it is!
				if (LobbyList[lobbyID].playerList.contains(client->GetID()))
					client->Send(msg);
			}
			else
			{
				// The client couldnt be contacted, so assume it has
				// disconnected.
				OnClientDisconnect(client);
				client.reset();
			}
		}
	}
	void UpdateLobby(uint32_t lobbyID)
	{
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Lobby_UpdateLobby;

		// we send the whole lobby object to the client when updating
		msg << LobbyList[lobbyID];
		MessageClientsInLobby(lobbyID, msg);
	}
	void printLobbies()
	{
		cout << "[SERVER] List of lobbies: " << endl;
		for (auto& lobby : LobbyList)
		{
			cout << "Lobby ID: " << lobby.first << endl;
			lobby.second.printPlayers();
		}
	}

	std::unordered_map<uint32_t, Player> PlayerList;
	std::unordered_map<uint32_t, Lobby> LobbyList;

protected:
	bool OnClientConnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		return true;
	}

	void OnClientValidated(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Client_Accepted;

		client->Send(msg);
	}

	void OnClientDisconnect(std::shared_ptr<olc::net::connection<GameMsg>> client) override
	{
		if (client)
		{
			if (PlayerList.find(client->GetID()) == PlayerList.end())
			{
				// client never added to roster, so just let it disappear
			}
			else
			{
				auto& pd = PlayerList[client->GetID()];
				std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(pd.UniqueID) + "\n";
				PlayerList.erase(client->GetID());
			}
		}
	}

	void OnMessage(std::shared_ptr<olc::net::connection<GameMsg>> client, olc::net::message<GameMsg>& msg) override
	{
		switch (msg.header.id)
		{
		case GameMsg::Client_RegisterWithServer:
		{
			Player player;
			player.UniqueID = client->GetID();
			PlayerList.insert_or_assign(player.UniqueID, player);

			olc::net::message<GameMsg> msgSendID;
			msgSendID.header.id = GameMsg::Client_AssignID;
			msgSendID << player.UniqueID;
			MessageClient(client, msgSendID);

			break;
		}
		case GameMsg::Menu_JoinLobby:
		{
			uint32_t LobbyID;
			msg >> LobbyID;
			if (LobbyList.contains(LobbyID)) // if a given lobby already exists
			{
				if (LobbyList[LobbyID].addPlayer(PlayerList[client->GetID()])) // the if checks whether the lobby is full
				{
					cout << "[" << client->GetID() << "] successfully added to lobby: " << LobbyID << endl;
				}
				else // if its full we send that information back to the player
				{
					olc::net::message<GameMsg> msgFullLobby;
					msgFullLobby.header.id = GameMsg::Menu_LobbyFull;
					MessageClient(client, msgFullLobby);
				}
			}
			else // otherwise we create such a lobby
			{
				Lobby temp(LobbyID);
				temp.addPlayer(PlayerList[client->GetID()]);

				LobbyList.insert_or_assign(LobbyID, temp);
				cout << "Created a new lobby of ID: " << LobbyID << endl;

				cout << "[" << client->GetID() << "] successfully added to lobby: " << LobbyID << endl;

				olc::net::message<GameMsg> msgAddedToLobby;
				msgAddedToLobby.header.id = GameMsg::Menu_ClientAddedToLobby;
				msgAddedToLobby << temp;

				MessageClient(client, msgAddedToLobby);
			}
			// we send the updated lobby to the clients
			UpdateLobby(LobbyID);
			break;
		}
		case GameMsg::Lobby_LeaveLobby: //TO DO
		{
			uint32_t LobbyID;
			msg >> LobbyID;

			if (LobbyList[LobbyID].removePlayer(PlayerList[client->GetID()]))
			{
				cout << "[" << client->GetID() << "] successfully removed from lobby: " << LobbyID << endl;

				// we tell the client that he has been successfully removed
				olc::net::message<GameMsg> msgRemovedFromLobby;
				msgRemovedFromLobby.header.id = GameMsg::Lobby_ClientRemovedFromLobby;

				MessageClient(client, msgRemovedFromLobby);
			}
			else
			{
				cout << "[" << client->GetID() << "] not in the lobby! ERROR in lobby: " << LobbyID << endl;
				if (LobbyList[LobbyID].playerList.size() == 0)
				{
					LobbyList.erase(LobbyID);
				}
 			}
			
			// we send the updated lobby to the clients
			UpdateLobby(LobbyID);
			break;
		}
		case GameMsg::Lobby_UpdateLobby:
		{
			uint32_t LobbyID;
			msg >> LobbyID;

			LobbyList[LobbyID].changePlayerState(client->GetID());

			// we send the updated lobby to the clients
			UpdateLobby(LobbyID);
			break;
		}
		}
	}
};

int main()
{
	RPSserver server(60000);

	server.Start();

	while (true)
	{
		server.Update(-1, true);
	}
	return 0;
}