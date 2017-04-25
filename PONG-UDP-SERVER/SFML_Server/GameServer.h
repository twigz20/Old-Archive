#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <vector>
#include <memory>
#include <map>

#include "Ball.h"
#include "Paddle.h"

class GameServer
{
public:
	explicit							GameServer();
	~GameServer();

	void								notifyPlayerSpawn(sf::Int32 aircraftIdentifier);
	void								notifyPlayerRealtimeChange(sf::Int32 paddleIdentifier, sf::Int32 action, bool actionEnabled);
	void								notifyPlayerEvent(sf::Int32 paddleIdentifier, sf::Int32 action);
	std::size_t							getConnectedPlayers() { return mConnectedPlayers; }
	bool								newConnection() { return newConnect; }
	void								setNewConnect(bool con) { newConnect = false; }

private:
	// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
	struct RemotePeer
	{
		RemotePeer();

		sf::TcpSocket			socket;
		sf::Time				lastPacketTime;
		std::vector<sf::Int32>	paddleIdentifiers;
		bool					ready;
		bool					timedOut;
	};

	// Structure to store information about current aircraft state
	struct PaddleInfo
	{
		sf::Vector2f				position;
		std::map<sf::Int32, bool>	realtimeActions;
	};

	// Unique pointer to remote peers
	typedef std::unique_ptr<RemotePeer> PeerPtr;


private:
	void								setListening(bool enable);
	void								executionThread();
	void								tick();
	sf::Time							now() const;

	void								handleIncomingPackets();
	void								handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);

	void								handleIncomingConnections();
	void								handleDisconnections();

	void								informWorldState(sf::TcpSocket& socket);
	void								broadcastMessage(const std::string& message);
	void								sendToAll(sf::Packet& packet);
	void								updateClientState();


private:
	sf::Thread							mThread;
	sf::Clock							mClock;
	sf::TcpListener						mListenerSocket;
	bool								mListeningState;
	sf::Time							mClientTimeoutTime;

	Ball								*ball;
	std::size_t							mMaxConnectedPlayers;
	std::size_t							mConnectedPlayers;

	std::size_t							mPaddleCount;
	std::map<sf::Int32, PaddleInfo>		mPaddleInfo;

	std::vector<PeerPtr>				mPeers;
	sf::Int32							mPaddleIdentifierCounter;
	bool								mWaitingThreadEnd;

	sf::Time							mLastSpawnTime;
	sf::Time							mTimeForNextSpawn;

	bool								newConnect;
};

