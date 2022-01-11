#pragma once

#define OLC_PGEX_NETWORK
#include "./lib/olcPGEX_Network.h"
#include <unordered_map>
#include <vector>

using std::cout, std::endl;

enum class PlayerState : uint32_t
{
	Connected,
	Lobby_Ready,
	Lobby_Not_Ready,
};

struct Player
{
	uint32_t UniqueID;
	PlayerState state;

	Player()
	{
		UniqueID = 0;
		state = PlayerState::Connected;
	}

	Player(uint32_t id)
	{
		UniqueID = id;
		state = PlayerState::Connected;
	}


	//CLIENT-SIDE:

	//Returns a string of whether the player is ready or not in the lobby
	std::string printReady()
	{
		if (state == PlayerState::Lobby_Not_Ready)
			return "Not ready";
		else
			return "Ready";
	}
};

struct PlayerList
{
	std::array<Player, 2> PList;

	PlayerList() {}

	//player slot is empty if that player's id is equal to 0


	void addPlayer(Player player)
	{
		if (PList[0].UniqueID == 0) // if first slot is empty
		{
			// we add the player to the first slot
			PList[0] = player;
		}
		else
		{
			PList[1] = player;
			// we add the player to the second slot
		}
	}
	bool removePlayer(Player player)
	{
		if (PList[0].UniqueID == player.UniqueID) // if the player we want to remove is in the first slot
		{
			// we move the player from the second slot to the first slot
			PList[0] = PList[1];
			PList[0].state = PlayerState::Connected;

			// we then reset the player at the second slot
			PList[1].UniqueID = 0;
			PList[1].state = PlayerState::Connected;
		}
		else if (PList[1].UniqueID == player.UniqueID) // otherwise we safety check if the player is in the second slot
		{
			// and remove it
			PList[1].UniqueID = 0;
			PList[1].state = PlayerState::Connected;
		}
		else return false; // if the player is not in the lobby we return false
		return true; // otherwise return true
	}
	void clear()
	{
		// reset the player at slot 0
		PList[0].UniqueID = 0;
		PList[0].state = PlayerState::Connected;

		// reset the player at slot 1
		PList[1].UniqueID = 0;
		PList[1].state = PlayerState::Connected;
	}
	bool contains(Player player)
	{
		return PList[0].UniqueID == player.UniqueID || PList[1].UniqueID == player.UniqueID;
	}
	uint32_t size()
	{
		uint32_t s = 0;
		// we increment the variable if ID at given slot is != 0
		// if the ID is equal to 0 then the slot is empty
		if (PList[0].UniqueID != 0) s++;
		if (PList[1].UniqueID != 0) s++;
		return s;
	}
};

struct Lobby
{
	uint32_t UniqueID;
	bool InGame;

	PlayerList playerList;

	Lobby() : UniqueID(0), InGame(0) {}

	Lobby(uint32_t id) : UniqueID(id), InGame(0) {}


	//CLIENT-SIDE:

	//used when player leaves a lobby
	void clearLobby()
	{
		UniqueID = 0;
		playerList.clear();
	}

	//SERVER-SIDE:
	bool addPlayer(Player a)
	{
		if (playerList.size() >= 2)
			return false;
		a.state = PlayerState::Lobby_Not_Ready;
		playerList.addPlayer(a);
		return true;
	}
	bool removePlayer(Player a)
	{
		return playerList.removePlayer(a);
	}

	void changePlayerState(uint32_t ID)
	{
		if (playerList.PList[ID].state == PlayerState::Lobby_Not_Ready)
			playerList.PList[ID].state = PlayerState::Lobby_Ready;
		else
			playerList.PList[ID].state = PlayerState::Lobby_Not_Ready;
	}

	//MUTUAL:
	//Prints the players in lobby
	void printPlayers()
	{
		for (int i = 0; i < 2; i++)
		{
			cout << "Player [" << i << "]: " << playerList.PList[i].printReady() << endl;
		}
	}
};