/*#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <conio.h>
#include <vector>

#include "PacketTags.h"

const unsigned short ServerPort = 9094;

int main()
{
	std::cout << "Server Running" << std::endl;

	sf::TcpListener listener;
	sf::SocketSelector selector;
	bool done = false;
	std::vector<sf::TcpSocket*> clients;

	listener.listen(ServerPort);
	selector.add(listener);

	while (!done)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				sf::TcpSocket *socket = new sf::TcpSocket;
				listener.accept(*socket);
				sf::Packet packet;
				std::string id;
				if (socket->receive(packet) == sf::Socket::Done)
					packet >> id;

				std::cout << id << " has connected to the Server" << std::endl;

				clients.push_back(socket);
				selector.add(*socket);
			}
			else
			{
				for (int i = 0; i < clients.size(); i++)
				{
					if (selector.isReady(*clients[i]))
					{
						sf::Packet packet, sendPacket;
						if (clients[i]->receive(packet) == sf::Socket::Done)
						{
							int flag;
							packet >> flag;
							std::cout << flag << std::endl;
							if (packet)
							{
								if (flag == PacketTags::Paddle) {
									std::cout << "Sending Paddle" << std::endl;
									float xPos, yPos;
									int tmpFlag = PacketTags::Paddle;
									packet >> xPos >> yPos;
									sendPacket << tmpFlag << xPos << yPos;
								}
								else if (flag == PacketTags::Ball)
								{
									std::cout << "Sending Ball" << std::endl;
									float xPos, yPos;
									int tmpFlag = PacketTags::Ball;
									packet >> xPos >> yPos;
									sendPacket << tmpFlag << xPos << yPos;
								} 
								else if (flag == PacketTags::Confirm)
								{
									std::cout << "Sending Confirmation" << std::endl;
									bool confirm;
									int tmpFlag = PacketTags::Confirm;
									packet >> confirm;
									sendPacket << tmpFlag << confirm;
								}
								else if (flag == PacketTags::Name)
								{
									std::cout << "Sending Player Name" << std::endl;
									std::string name;
									int tmpFlag = PacketTags::Name;
									packet >> name;
									sendPacket << tmpFlag << name;
								}
							}

							for (int j = 0; j < clients.size(); j++)
							{
								if (i != j)
								{
									clients[j]->send(sendPacket);
								}
							}
						}
					}
				}
			}
		}
	}

	for (std::vector<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); it++)
		delete *it;

	return 0;
}*/