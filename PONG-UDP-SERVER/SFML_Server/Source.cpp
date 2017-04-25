#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <conio.h>
#include <vector>
#include <sstream>
#include <memory>

#include "Ball.h"
#include "NetworkProtocol.h"
#include "Foreach.hpp"
#include "Utility.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace std;

// A GameServerRemotePeer refers to one instance of the game, may it be local or from another computer
struct RemotePeer
{
	RemotePeer();

	std::string				id;
	sf::IpAddress			sender;
	unsigned short			port;
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

void								notifyPlayerSpawn(sf::Int32 aircraftIdentifier);
void								notifyPlayerRealtimeChange(sf::Int32 paddleIdentifier, sf::Int32 action, bool actionEnabled);
void								notifyPlayerEvent(sf::Int32 paddleIdentifier, sf::Int32 action);
void								setListening(bool enable);
void								executionThread();
void								tick();
sf::Time							now();

void								handleIncomingPackets();
void								handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);
void								handleIncomingPackets(sf::Packet& packet, sf::IpAddress ip, unsigned short port);

void								handleIncomingUdpConnections();
void								handleIncomingConnections();
void								handleDisconnections();

void								informWorldState(sf::TcpSocket& socket);
void								broadcastMessage(const std::string& message);
void								sendToAll(sf::Packet& packet);
void								updateClientState();

// Unique pointer to remote peers
typedef std::unique_ptr<RemotePeer> PeerPtr;
sf::Clock serverClock;
sf::UdpSocket udpSocket;
sf::TcpListener	listener;
bool listeningState = false;
sf::Time clientTimeoutTime = sf::seconds(3.f);
std::size_t maxConnectedPlayers = 2;
sf::Text connections;
std::size_t connectedPlayers = 0;
std::size_t paddleCount = 0;
std::map<sf::Int32, PaddleInfo>	 paddleInfo;
std::vector<PeerPtr> peers;
//std::vector<sf::TcpSocket> peers;
sf::Int32 paddleIdentifierCounter = 0;
sf::Time lastSpawnTime = sf::Time::Zero;
sf::Time timeForNextSpawn = sf::seconds(5.f);
std::map<unsigned short, sf::IpAddress> computerID;

int main()
{
	sf::Font font;
	font.loadFromFile("Resources/Fonts/OptimusPrinceps.ttf");

	sf::Text serverText;
	serverText.setFont(font);
	serverText.setString("Server Running...");
	serverText.setPosition(SCREEN_WIDTH / 2 - serverText.getGlobalBounds().width / 2, 0);

	stringstream ss;
	ss << connectedPlayers;
	connections.setString("Connections: " + ss.str());
	ss.clear();
	connections.setFont(font);
	connections.setPosition(20, 30);

	sf::RenderWindow window(sf::VideoMode(800, 600), "Twigzolupolus Inc Server");
	bool connected = false;
	
	std::cout << "Server Running..." << std::endl;
	
	connected = true;

	while (connected)
	{
		setListening(true);
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event)) 
			{
				if (event.type == sf::Event::Closed) {
					window.close();
					connected = false;
				}
			}

			handleIncomingPackets();

			window.clear(sf::Color(0, 0, 0));
			window.draw(serverText);
			window.draw(connections);
			window.display();
		}

		setListening(false);
	}
}

void setListening(bool enable)
{
	// Check if it isn't already listening
	if (enable)
	{
		if (!listeningState) {
			udpSocket.bind(9094);
			udpSocket.setBlocking(false);
		}
	}
	else
	{
		udpSocket.unbind();
		listeningState = false;
	}
}

void handleIncomingPackets(sf::Packet& packet, sf::IpAddress ip, unsigned short port)
{
	sf::Int32 packetType;
	packet >> packetType;

	switch (packetType)
	{
		case Client::NewConnection:
		{
			computerID[port] = ip;
			std::unique_ptr<RemotePeer> p(new RemotePeer);
			p->port = port;
			p->sender = ip;
			packet >> p->id;
			peers.push_back(std::move(p));
		
			std::cout << "Player Connected to Server" << endl;

			if (connectedPlayers == 0) {
				std::cout << "Setting Player 1 Paddle Position" << endl;
				// order the new client to spawn its own plane ( player 1 )
				paddleInfo[paddleIdentifierCounter].position = sf::Vector2f(SCREEN_WIDTH*.02, SCREEN_HEIGHT / 2);
			}
			else {
				std::cout << "Setting Player 2 Paddle Position" << endl;
				// order the new client to spawn its own plane ( player 2 )
				paddleInfo[paddleIdentifierCounter].position = sf::Vector2f(SCREEN_WIDTH*.95, SCREEN_HEIGHT / 2);
			}

			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::SpawnSelf);
			packet << paddleIdentifierCounter;
			packet << paddleInfo[paddleIdentifierCounter].position.x;
			packet << paddleInfo[paddleIdentifierCounter].position.y;


			peers[connectedPlayers]->paddleIdentifiers.push_back(paddleIdentifierCounter);

			if (connectedPlayers == 1) {
				notifyPlayerSpawn(paddleIdentifierCounter);
			}

			if (udpSocket.send(packet, peers[connectedPlayers]->sender, peers[connectedPlayers]->port) == sf::Socket::Done) {
				cout << "Sent Packet Spawning Packet" << endl;
			}

			paddleIdentifierCounter++;
			peers[connectedPlayers]->ready = true;
			peers[connectedPlayers]->lastPacketTime = now(); // prevent initial timeouts
			paddleCount++;
			connectedPlayers++;

			stringstream ss;
			ss << "Connections: " << connectedPlayers;
			connections.setString(ss.str());
		}break;

		case Client::PositionUpdate:
		{
			if (connectedPlayers >= 2) {
				sf::Int32 playerUpdate;
				packet >> playerUpdate;

				if (playerUpdate == PacketTags::Player1)
				{
					sf::Vector2f position;
					packet >> position.x >> position.y;

					std::cout << position.x << " " << position.y << std::endl;

					sf::Packet posUpdatePacket;
					posUpdatePacket << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
					posUpdatePacket << position.x << position.y;

					if(udpSocket.send(posUpdatePacket, peers[1]->sender, peers[1]->port) != sf::Socket::Done)
						std::cout << "Failed to Send P1 Udp Packet" << std::endl;
				}
				else if (playerUpdate == PacketTags::Player2)
				{
					sf::Vector2f position;
					packet >> position.x >> position.y;

					std::cout << position.x << " " << position.y << std::endl;

					sf::Packet posUpdatePacket;
					posUpdatePacket << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
					posUpdatePacket << position.x << position.y;
					
					if (udpSocket.send(posUpdatePacket, peers[0]->sender, peers[0]->port) != sf::Socket::Done)
						std::cout << "Failed to Send P1 Udp Packet" << std::endl;
				}
			}
		} break;
	}
}

void handleIncomingPackets()
{
	sf::Packet packet;
	sf::IpAddress sender;
	unsigned short port;

	if (udpSocket.receive(packet, sender, port) == sf::Socket::Done)
	{
		// Interpret packet and react to it
		handleIncomingPackets(packet, sender, port);
		packet.clear();
	}		
}

RemotePeer::RemotePeer()
	: ready(false)
	, timedOut(false)
{
}

sf::Time now()
{
	return serverClock.getElapsedTime();
}

void notifyPlayerSpawn(sf::Int32 paddleIdentifier)
{
	// Sending Player 2 Info to Player 1
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::PlayerConnect);
	packet << paddleIdentifierCounter;
	packet << peers[paddleIdentifier]->id <<  paddleInfo[paddleIdentifier].position.x << paddleInfo[paddleIdentifier].position.y;
	udpSocket.send(packet, peers[0]->sender, peers[0]->port);

	// Sending Player 1 Info to Player 2
	packet.clear();
	packet << static_cast<sf::Int32>(Server::PlayerConnect);
	packet << 0;
	packet << peers[0]->id << paddleInfo[0].position.x << paddleInfo[0].position.y;
	udpSocket.send(packet, peers[1]->sender, peers[1]->port);
}