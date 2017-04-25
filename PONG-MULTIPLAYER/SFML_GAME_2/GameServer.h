#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <vector>
#include <memory>
#include <map>

#include "ball.h"
#include "Paddle.h"
#include "NetworkProtocol.h"

class GameServer
{
public:
	explicit							GameServer();
	~GameServer();

	void								notifyPlayerSpawn(sf::Int32 aircraftIdentifier);
	void								notifyPlayerRealtimeChange(sf::Int32 paddleIdentifier, sf::Int32 action, bool actionEnabled);
	void								notifyPlayerEvent(sf::Int32 paddleIdentifier, sf::Int32 action);
	

private:
	// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
	struct RemotePeer
	{
		RemotePeer();

		sf::UdpSocket			playerSocket;
		std::string				id;
		sf::IpAddress			ip;
		unsigned short			sendingPort;
		unsigned short			recievingPort;
		sf::Time				lastPacketTime;
		std::vector<sf::Int32>	paddleIdentifiers;
		bool					ready;
		bool					timedOut;
	};

	// Unique pointer to remote peers
	typedef std::unique_ptr<RemotePeer> PeerPtr;

	// Structure to store information about current aircraft state
	struct PaddleInfo
	{
		sf::Vector2f				position;
		std::map<sf::Int32, bool>	realtimeActions;
	};

	void								setListening(bool enable);
	void								executionThread();
	void								tick();
	sf::Time							now();

	void								handleIncomingPacket(PeerPtr &peer);
	void								handleIncomingPackets(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);
	void								handleIncomingPackets(sf::Packet& packet);

	void								handleIncomingUdpConnections();
	void								handleIncomingConnections();
	void								handleDisconnections();

	void								informWorldState(sf::TcpSocket& socket);
	void								broadcastMessage(const std::string& message);
	void								sendToAll(sf::Packet& packet);
	void								updateClientState();

	void								HandleCollisions();
	float								GetReflection(Paddle *player, float hity);

private:
	sf::Clock serverClock;
	sf::UdpSocket connectionSocket;
	sf::UdpSocket player1Socket;
	sf::UdpSocket player2Socket;
	sf::TcpListener	listener;
	bool listeningState;
	sf::Time clientTimeoutTime;
	std::size_t maxConnectedPlayers;
	sf::Text connections;
	std::size_t connectedPlayers;
	std::size_t paddleCount;
	std::map<sf::Int32, PaddleInfo>	 paddleInfo;
	std::vector<PeerPtr> peers;
	sf::Int32 paddleIdentifierCounter;
	sf::Time lastSpawnTime;
	sf::Time timeForNextSpawn;
	std::map<unsigned short, sf::IpAddress> computerID;
	Paddle *player1, *player2;
	Ball *ball = new Ball();

	bool p1Acknowledgement, p2Acknowledgement;
	unsigned short ports[2];
	bool serverHas2Players;
	int ballPort;

	sf::Thread serverThread;
};

