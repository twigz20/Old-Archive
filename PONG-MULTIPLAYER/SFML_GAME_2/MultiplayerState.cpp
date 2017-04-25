//MultiplayerState.cpp

#include "MultiplayerState.h"
#include "NetworkProtocol.h"

using namespace std;

MultiplayerState::MultiplayerState()
	: gameStarted(false)
	,  host(false) 
	, gameServer(nullptr)
	, ballThread(&MultiplayerState::executeBallThread, this)
{
	sf::IpAddress ip;
	if (!host)
	{
		gameServer.reset(new GameServer);
		ip = "127.0.0.1";
	}
	else
	{
		ip = "127.0.0.1";
	}

	threshold = 250;
	difference = 0;
	interpolation_constant = 0.5;
	srand(unsigned(time(NULL)));
	port = rand() % (9998 - 2000 + 1) + 2000;
	ballPort = 9999;
	connectToServer();
	clientTimeout = sf::seconds(2.f);
	timeSinceLastPacket = sf::seconds(0.f);
	gameStarted = false;
	playerPos = 0;

	paused = false;

	// Load Background
	bg.setFilePath("background.png");

	player = new Paddle();
	opponent = new Paddle();

	ball = new Ball();
	ball->LoadFromFile("ball.png");
	ball->SetPosition(SCREEN_WIDTH / 2 - ball->GetWidth() / 2, SCREEN_HEIGHT / 2 - ball->GetHeight() / 2);

	// Setup Text
	enterToStart.setFont("OptimusPrincepsSemiBold.ttf");
	enterToStart.setStringAndSize("\t\tPress\nEnter to Start", 40);
	enterToStart.setColor(sf::Color(255, 255, 255));
	enterToStart.setPosition((SCREEN_WIDTH / 2) - enterToStart.getGlobalBounds().width / 2, (SCREEN_HEIGHT / 2) - enterToStart.getGlobalBounds().height);

	pause.setFont("OptimusPrincepsSemiBold.ttf");
	pause.setStringAndSize("PAUSE", 70);
	pause.setColor(sf::Color(255, 255, 255));
	pause.setPosition((SCREEN_WIDTH / 2) - pause.getGlobalBounds().width / 2, (SCREEN_HEIGHT / 2) - pause.getGlobalBounds().height);

	escToExit.setFont("OptimusPrincepsSemiBold.ttf");
	escToExit.setStringAndSize("\t Press\nEsc to Exit", 40);
	escToExit.setColor(sf::Color(255, 0, 0));
	escToExit.setPosition((SCREEN_WIDTH / 2) - escToExit.getGlobalBounds().width / 2, 335);

	pAcknowledgmentSent = false;
	opponentAcknowledgementReceived = false;
}

MultiplayerState::~MultiplayerState()
{
}

void MultiplayerState::executeBallThread()
{
	cout << "In Ball Socket" << endl;
	sf::Packet packet;
	sf::IpAddress sender;
	unsigned short port;
	while (gameStarted) {
		if (ballSocket.receive(packet, sender, port) == sf::Socket::Done)
		{
			cout << "In Ball Socket ------------------" << endl;
			timeSinceLastPacket = sf::seconds(0.f);
			sf::Int32 packetType;
			packet >> packetType;
			handlePacket(packetType, packet, elapsedTime);
			packet.clear();
		}
	}
}

void MultiplayerState::HandleEvents(Game &game) {
	while (game.window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game.setRunning(false);

		if (event.type == sf::Event::KeyPressed) {
			if(update) {
				if (event.key.code == sf::Keyboard::Return) {
					if (!pAcknowledgmentSent) {
						sf::Packet acknowledgementPacket;
						acknowledgementPacket << Client::Acknowledgement;

						if (playerPos == 1)
							acknowledgementPacket << PacketTags::Player1;
						else
							acknowledgementPacket << PacketTags::Player2;

						if (udpSocket.send(acknowledgementPacket, ip, sendingPort) == sf::Socket::Done)
							cout << "Acknowledgement Sent" << endl;

						pAcknowledgmentSent = true;
					}
				}
			}
		}

		if (event.type == sf::Event::GainedFocus)
			update = true;
		else if (event.type == sf::Event::LostFocus)
			update = false;
	}
}

void MultiplayerState::interpolation(sf::Time& elapsedTime)
{
	difference = opponentRemotePosition.y - opponent->GetPositionY();
	if (difference < threshold)
		opponent->SetPositionY(opponentRemotePosition.y);
	else {
		float posY = opponent->GetPositionY();
		posY += difference * elapsedTime.asSeconds() * interpolation_constant;
		opponent->SetPositionY(posY);
	}
}

void MultiplayerState::Update(Game &game)
{
	if (connected) 
	{
		HandleCollisions(game);

		elapsedTime = clock.getElapsedTime();
	
		sf::Vector2f prevPosition;
		prevPosition = player->GetPosition();

		if (update && gameStarted) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				player->moveDown(elapsedTime.asSeconds());

				if (player->GetPositionY() + player->GetHeight() >= SCREEN_HEIGHT)
					player->moveUp(elapsedTime.asSeconds());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				player->moveUp(elapsedTime.asSeconds());

				if (player->GetPositionY() <= 0)
					player->moveDown(elapsedTime.asSeconds());
			}
		}

		if (prevPosition != player->GetPosition())
		{
			sf::Packet pack;
			handlePacket(Client::PositionUpdate, pack,elapsedTime, prevPosition);
		}
			
		sf::Packet packet;	
		sf::IpAddress sender;
		unsigned short port;
		if (udpSocket.receive(packet, sender, port) == sf::Socket::Done)
		{
			timeSinceLastPacket = sf::seconds(0.f);
			sf::Int32 packetType;
			packet >> packetType;
			handlePacket(packetType, packet, elapsedTime, prevPosition);
		}

		interpolation(elapsedTime);
		clock.restart();
	}
}

void MultiplayerState::handlePacket(sf::Int32 packetType, sf::Packet& packet, sf::Time elapsedTime, sf::Vector2f prevPosition)
{
	switch (packetType)
	{
		// Sent by the server to order to spawn player 1 airplane on connect
		case Server::SpawnSelf:
		{
			std::cout << "Creating Player paddle" << std::endl;
			sf::Int32 paddleIdentifier;
			sf::Vector2f paddlePosition;
			packet >> sendingPort >> paddleIdentifier >> paddlePosition.x >> paddlePosition.y;
			cout << paddleIdentifier << " Player " << paddlePosition.x << " " << paddlePosition.y << endl;

			player->LoadFromFile("Paddle.png");
			player->SetPosition(paddlePosition.x, paddlePosition.y - (player->GetHeight() / 2));

			playerName.setStringAndSize(playerId, 20);
			playerName.setFont("OptimusPrincepsSemiBold.ttf");
			playerName.setColor(sf::Color(255, 255, 255));

			if (paddleIdentifier == 1)
				playerName.setPosition(20, 20);
			else
				playerName.setPosition(SCREEN_WIDTH - (playerName.getGlobalBounds().width + 20), 20);

			playerPos = paddleIdentifier;

			cout << "Player " << paddleIdentifier << " Paddle Creation Complete" << endl;
		} break;

		// 
		case Server::PlayerConnect:
		{
			std::cout << "Creating Opponent paddle" << std::endl;
			sf::Int32 paddleIdentifier;
			std::string name;
			
			packet >> paddleIdentifier >> name >> opponentRemotePosition.x >> opponentRemotePosition.y;
			cout << paddleIdentifier << " Enemy " << opponentRemotePosition.x << " " << opponentRemotePosition.y << endl;

			opponent->LoadFromFile("Paddle.png");
			opponent->SetPosition(opponentRemotePosition.x, opponentRemotePosition.y - (opponent->GetHeight() / 2));

			opponentName.setStringAndSize(name, 20);
			opponentName.setFont("OptimusPrincepsSemiBold.ttf");
			opponentName.setColor(sf::Color(255, 255, 255));

			if (paddleIdentifier == 0)
				opponentName.setPosition(20, 20);
			else
				opponentName.setPosition(SCREEN_WIDTH - (opponentName.getGlobalBounds().width + 20), 20);

		} break;

		case Server::PlayerRealtimeChange:
		{
			cout << "Entered Here" << std::endl;

			packet >> opponentRemotePosition.x >> opponentRemotePosition.y;

			cout << "Opponent Position Change: " << opponentRemotePosition.x << " " << opponentRemotePosition.y << std::endl;

			//opponent->SetPosition(position.x, position.y);
		} break;

		case Client::PositionUpdate:
		{
			std::cout << "Player " << playerPos + 1 << " PosX: " << player->GetPosition().x << " PosY: " << player->GetPosition().y << std::endl;
			int pos = 0;
			if (playerPos == 1)
				pos = PacketTags::Player1;
			else
				pos = PacketTags::Player2;

			sf::Packet updatePosPacket;
			updatePosPacket << Client::PositionUpdate << pos << player->GetPosition().x << player->GetPosition().y;

			if (udpSocket.send(updatePosPacket, ip, sendingPort) != sf::Socket::Done)
				cout << "Failed to Send Position" << endl;
		}break;

		case Server::BallUpdate:
		{
			sf::Vector2f ballPos;
			packet >> ballPos.x >> ballPos.y;

			ball->SetPosition(ballPos.x, ballPos.y);
			cout << ball->GetPositionX() << " " << ball->GetPositionY() << endl;

		}break;

		case Client::Acknowledgement:
		{
			gameStarted = true;
			ballThread.launch();
		} break;
	}
}

void MultiplayerState::Render(Game &game)
{
	bg.Render(game.window);
	playerName.Render(game.window);
	opponentName.Render(game.window);

	ball->Draw(game.window);

	player->Draw(game.window);
	opponent->Draw(game.window);

	/*if (ballProperties.delayStart && firstTouch)
		enterToStart.Render(game.window);

	if (paused) {
		pause.Render(game.window);

		if (!firstTouch)
			enterToStart.Render(game.window);

		escToExit.Render(game.window);
	}*/
}

void MultiplayerState::Clear()
{
	points = 0;
}
void MultiplayerState::StartPause()
{
	enterToStart.setColor(sf::Color(165, 250, 0));
	enterToStart.setPosition((SCREEN_WIDTH / 2) - enterToStart.getGlobalBounds().width / 2, 150);
	paused = true;
}
void MultiplayerState::EndPause()
{
	paused = false;
}

void MultiplayerState::HandleUpdates(Game &game)
{

}

void MultiplayerState::HandleDraws(Game &game)
{

}

void MultiplayerState::HandleCollisions(Game &game)
{
	// Top and bottom
	if (ball->GetPositionY() < 0)
	{
		// Top
		// Keep the ball within the playing field and reflect the y-direction
		ball->SetPositionY(0);
		ball->SetVelocity(sf::Vector2f(ball->GetVelocity().x, -1 * ball->GetVelocity().y));
	}
	else if (ball->GetPositionY() + ball->GetHeight() >= SCREEN_HEIGHT)
	{
		// Bottom
		// Keep the ball within the playing field and reflect the y-direction
		ball->SetPositionY(SCREEN_HEIGHT - ball->GetHeight());
		ball->SetVelocity(sf::Vector2f(ball->GetVelocity().x, -1 * ball->GetVelocity().y));
	}

	//Make the ball bounce if it hits the left or right
	//Right
	if ((ball->GetPositionX() + ball->GetWidth() >= SCREEN_WIDTH))
	{
		//p1_score++;
		ballProperties.speed = 500;
		ballProperties.delayStart = true;
		ball->changeBallSpeed(ballProperties.speed);
		ballProperties.ballHits = 0;

		// Set Ball to Center vertically and horizontally to the right Screen
		//ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetHeight() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));
		//ball->setBallDirection(-1, -1);
	}
	//Left
	else if (ball->GetPositionX() <= 0)
	{
		//p2_score++;
		ballProperties.speed = 500;
		ballProperties.delayStart = true;
		ball->changeBallSpeed(ballProperties.speed);
		ballProperties.ballHits = 0;

		// Set Ball to Center vertically and horizontally to the right Screen
		//ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetWidth() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));
		//ball->setBallDirection(1, 1);
	}

	//Ball Paddle Collision p1
	if (ball->Collides(player))
	{
		ballProperties.ballHits++;
		if (ballProperties.ballHits == 3)
		{
			ballProperties.ballHits = 0;
			ballProperties.speed += 30;
			ball->changeBallSpeed(ball->getBallSpeed()*ball->getSpeedMod());
			std::cout << ball->getBallSpeed() << " - " << aiSpeed << std::endl;
		}

		ball->SetPositionX(player->GetPositionX() + player->GetWidth());
		ball->setBallDirection(1, GetReflection(player, (ball->GetPositionY() + ball->GetHeight() / 2) - player->GetPositionY()));

	}

	//Ball Paddle Collision p2/ai
	if (ball->Collides(opponent))
	{
		ballProperties.ballHits++;
		ball->changeBallSpeed(ball->getBallSpeed()*ball->getSpeedMod());
		std::cout << ball->getBallSpeed() << " - " << aiSpeed << std::endl;

		ball->SetPositionX(opponent->GetPositionX() - ball->GetWidth());
		ball->setBallDirection(-1, GetReflection(opponent, (ball->GetPositionY() + ball->GetHeight() / 2) - opponent->GetPositionY()));
	}
}

float MultiplayerState::GetReflection(Paddle *player, float hity) {
	// Make sure the hity variable is within the height of the paddle
	if (hity < 0)
	{
		hity = 0;
	}
	else if (hity > player->GetHeight())
	{
		hity = player->GetHeight();
	}

	// Everything above the center of the paddle is reflected upward
	// while everything below the center is reflected downward
	hity -= player->GetHeight() / 2.0f;

	// Scale the reflection, making it fall in the range -2.0f to 2.0f
	return 2.0f * (hity / (player->GetHeight() / 2.0f));
}

void MultiplayerState::setPaddlePosition(Paddle *player, float y)
{
	float newY;
	if (y < 0)
	{
		newY = 0;
	}
	else if (y + player->GetHeight() > SCREEN_HEIGHT)
	{
		newY = SCREEN_HEIGHT - player->GetHeight();
	}
	else
	{
		newY = y;
	}

	player->SetPositionY(newY);
}

void MultiplayerState::connectToServer()
{
	sf::RenderWindow window(sf::VideoMode(320, 160), "Connection Window");

	sf::Font font;
	font.loadFromFile("Resources/Fonts/OptimusPrinceps.ttf");
	sf::Text message("Enter Username: ", font, 20);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				else if (event.key.code == sf::Keyboard::Return)
				{
					sf::Text displayText(playerId, font, 20);
					displayText.setPosition(30, 0);
					displayText.setColor(sf::Color::Green);
					window.close();
				}
				break;
			case sf::Event::TextEntered:
				playerId += event.text.unicode;
				break;
			}
		}

		window.draw(message);

		sf::Text drawText(playerId, font, 20);
		drawText.setColor(sf::Color::Red);
		drawText.setPosition(155, 0);
		window.draw(drawText);

		window.display();
		window.clear();
	}

	std::cout << playerId << std::endl;

	ip = sf::IpAddress::getLocalAddress();
		connected = true;


	sf::Packet idPacket;
	idPacket << Client::NewConnection << playerId;

	udpSocket.bind(port);
	udpSocket.setBlocking(false);

	if (udpSocket.send(idPacket, ip,9094) == sf::Socket::Done) {
		std::cout << "Id Packet" << std::endl;
	}

	ballSocket.bind(ballPort);
	ballSocket.setBlocking(false);
}

