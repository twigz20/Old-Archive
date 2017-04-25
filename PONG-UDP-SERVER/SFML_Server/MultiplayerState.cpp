//MultiplayerState.cpp

#include "MultiplayerState.h"
#include "NetworkProtocol.h"

MultiplayerState::MultiplayerState()
{
	connectToServer();
	startOtherPlayer = false;

	paused = false;
	firstTouch = true;

	// Load Background
	bg.setFilePath("background.png");

	// Setup Player 1 Paddle
	player.push_back(new Paddle());
	player[0]->LoadFromFile("paddle.png");
	player[0]->SetPosition(SCREEN_WIDTH*.02, SCREEN_HEIGHT / 2 - (player[0]->GetHeight() / 2));

	// Setup Ai Paddle
	player.push_back(new Paddle());
	player[1]->LoadFromFile("paddle.png");
	player[1]->SetPosition(SCREEN_WIDTH*.98 - (player[1]->GetWidth()), SCREEN_HEIGHT / 2 - (player[1]->GetHeight() / 2));
	aiSpeed = 400;

	// Setup Ball
	ball = new Ball();
	ball->LoadFromFile("ball.png");
	ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetHeight() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));
	ball->setBallDirection(1, 1);
	ballProperties.ballHits = 0;
	ballProperties.speed = 500;
	ballProperties.delayStart = true;

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
}

MultiplayerState::~MultiplayerState()
{
}

void MultiplayerState::HandleEvents(Game &game) {
	while (game.window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game.setRunning(false);

		if (event.type == sf::Event::KeyPressed) {

			if(update) {
				if (event.key.code == sf::Keyboard::Return) {
					if (firstTouch) {
						sf::Packet name;
						name << PacketTags::PFlag::Name << playerName.getText();
						socket.send(name);
						socket.receive(name);
						std::string oName;
						name >> oName;
						otherPlayerName.setStringAndSize(oName, 30);
					}

					sf::Packet startConfirm;
					bool start = true;
					startConfirm << PacketTags::PFlag::Confirm << start;
					socket.send(startConfirm);

					sf::Packet packet;
					int tmpFlag;
					socket.receive(packet);
					packet >> tmpFlag;
					if (tmpFlag == PacketTags::Confirm) {
						packet >> startOtherPlayer;
						packet.clear();
					}

					if (startOtherPlayer) {
						firstTouch = false;
						ballProperties.delayStart = false;
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

void MultiplayerState::Update(Game &game)
{
	elapsedTime = clock.getElapsedTime();

	/*HandleCollisions(game);
	//aiMove(elapsedTime);

	/*if (paddle == 1)
	{
		sf::Vector2f prevPosition, p2Position;

		prevPosition = player[0]->GetPosition();

		if (update) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				player[0]->moveDown(elapsedTime.asSeconds());

				if (player[0]->GetPositionY() + player[0]->GetHeight() >= SCREEN_HEIGHT)
					player[0]->moveUp(elapsedTime.asSeconds());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				player[0]->moveUp(elapsedTime.asSeconds());

				if (player[0]->GetPositionY() <= 0)
					player[0]->moveDown(elapsedTime.asSeconds());
			}
		}

		sf::Packet packet;
		if (prevPosition != player[0]->GetPosition())
		{
			std::cout << "Player 1 PosX: " << player[0]->GetPosition().x << " PosY: " << player[0]->GetPosition().y << std::endl;
			packet << PacketTags::PFlag::Paddle << player[0]->GetPosition().x << player[0]->GetPosition().y;

			socket.send(packet);
		}

		socket.receive(packet);
		int flag;
		packet >> flag;
		if (flag == PacketTags::Paddle) {
			if (packet >> p2Position.x >> p2Position.y)
			{
				player[1]->SetPosition(p2Position.x, p2Position.y);
			}
		}
	}
	else if (paddle == 2)
	{
		sf::Vector2f prevPosition, p1Position;

		prevPosition = player[1]->GetPosition();

		if (update) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				player[1]->moveDown(elapsedTime.asSeconds());

				if (player[1]->GetPositionY() + player[0]->GetHeight() >= SCREEN_HEIGHT)
					player[1]->moveUp(elapsedTime.asSeconds());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				player[1]->moveUp(elapsedTime.asSeconds());

				if (player[1]->GetPositionY() <= 0)
					player[1]->moveDown(elapsedTime.asSeconds());
			}
		}

		sf::Packet packet;
		if (prevPosition != player[1]->GetPosition())
		{
			std::cout << "Player 2 PosX: " << player[1]->GetPosition().x << " PosY: " << player[1]->GetPosition().y << std::endl;
			packet << PacketTags::PFlag::Paddle << player[1]->GetPosition().x << player[1]->GetPosition().y;

			socket.send(packet);
		}

		socket.receive(packet);
		int flag;
		packet >> flag;
		if (flag == PacketTags::Paddle) {
			if (packet >> p1Position.x >> p1Position.y)
			{
				player[0]->SetPosition(p1Position.x, p1Position.y);
			}
		}
	}

	//if (!ballProperties.delayStart) {
		//sf::Vector2f ballPrevPosition, ballNewPosition;
		//ball->Update(elapsedTime.asSeconds());

		//sf::Packet packet;
		//packet << PacketTags::Ball << ball->GetPosition().x << ball->GetPosition().y;
		//socket.send(packet);

		/*socket.receive(packet);
		int flag;
		packet >> flag;
		if (flag == PacketTags::Ball) {
			float x = 0.0, y = 0.0;
			if (packet >> x >> y)
			{
				ball->SetPosition(x, y);
			}
		}*/
	//}

	clock.restart();
}

void MultiplayerState::Render(Game &game)
{
	bg.Render(game.window);
	playerName.Render(game.window);
	/*otherPlayerName.Render(game.window);
	player[0]->Draw(game.window);
	player[1]->Draw(game.window);
	ball->Draw(game.window);

	if (ballProperties.delayStart && firstTouch)
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
	if (ball->Collides(player[0]))
	{
		ballProperties.ballHits++;
		if (ballProperties.ballHits == 3)
		{
			ballProperties.ballHits = 0;
			ballProperties.speed += 30;
			ball->changeBallSpeed(ball->getBallSpeed()*ball->getSpeedMod());
			std::cout << ball->getBallSpeed() << " - " << aiSpeed << std::endl;
		}

		ball->SetPositionX(player[0]->GetPositionX() + player[0]->GetWidth());
		ball->setBallDirection(1, GetReflection(0, (ball->GetPositionY() + ball->GetHeight() / 2) - player[0]->GetPositionY()));

	}

	//Ball Paddle Collision p2/ai
	if (ball->Collides(player[1]))
	{
		ballProperties.ballHits++;
		ball->changeBallSpeed(ball->getBallSpeed()*ball->getSpeedMod());
		std::cout << ball->getBallSpeed() << " - " << aiSpeed << std::endl;

		ball->SetPositionX(player[1]->GetPositionX() - ball->GetWidth());
		ball->setBallDirection(-1, GetReflection(1, (ball->GetPositionY() + ball->GetHeight() / 2) - player[1]->GetPositionY()));
	}
}

float MultiplayerState::GetReflection(int index, float hity) {
	// Make sure the hity variable is within the height of the paddle
	if (hity < 0)
	{
		hity = 0;
	}
	else if (hity > player[index]->GetHeight())
	{
		hity = player[index]->GetHeight();
	}

	// Everything above the center of the paddle is reflected upward
	// while everything below the center is reflected downward
	hity -= player[index]->GetHeight() / 2.0f;

	// Scale the reflection, making it fall in the range -2.0f to 2.0f
	return 2.0f * (hity / (player[index]->GetHeight() / 2.0f));
}

void MultiplayerState::setPaddlePosition(float index, float y)
{
	float newY;
	if (y < 0)
	{
		newY = 0;
	}
	else if (y + player[index]->GetHeight() > SCREEN_HEIGHT)
	{
		newY = SCREEN_HEIGHT - player[index]->GetHeight();
	}
	else
	{
		newY = y;
	}

	player[index]->SetPositionY(newY);
}

void MultiplayerState::aiMove(sf::Time& elapsedTime)
{
	// Perfect ai would be: SetPaddleY(1, ball->y + ball->height/2 - paddle[1]->height/2);
	// Calculate how much the paddle can move in this frame
	float dy = aiSpeed * elapsedTime.asSeconds();

	// Imperfect ai
	if (ball->GetPositionX() > SCREEN_WIDTH / 2 && ball->GetVelocity().x > 0)
	{
		// Lag behind the ball, when ball is in sight
		if (player[1]->GetPositionY() + player[1]->GetHeight() / 2 < ball->GetPositionY() + ball->GetHeight() / 2)
		{
			setPaddlePosition(1, player[1]->GetPositionY() + dy);
		}
		else if (ball->GetPositionY() < player[1]->GetPositionY())
		{
			setPaddlePosition(1, player[1]->GetPositionY() - dy);
		}
	}
	else
	{
		// Move to the center, when ball is far away
		if (player[1]->GetPositionY() + player[1]->GetHeight() / 2 < SCREEN_HEIGHT / 2 - dy)
		{
			setPaddlePosition(1, player[1]->GetPositionY() + dy);
		}
		else if (player[1]->GetPositionY() + player[1]->GetHeight() / 2 > SCREEN_HEIGHT / 2 + dy)
		{
			setPaddlePosition(1, player[1]->GetPositionY() - dy);
		}
	}
}

void MultiplayerState::connectToServer()
{
	sf::RenderWindow window(sf::VideoMode(320,160),"Connection Window");
	std::string id;

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
					sf::Text displayText(id, font, 20);
					displayText.setPosition(30, 0);
					displayText.setColor(sf::Color::Green);
					window.close();
				}
				break;
			case sf::Event::TextEntered:
				id += event.text.unicode;
				break;
			}
		}

		window.draw(message);

		sf::Text drawText(id, font, 20);
		drawText.setColor(sf::Color::Red);
		drawText.setPosition(155, 0);
		window.draw(drawText);

		window.display();
		window.clear();
	}

	sf::RenderWindow window2(sf::VideoMode(320, 160), "Connection Window2");
	std::string playerPos;

	sf::Text message2("Enter Either 1 or 2 for player slot: ", font, 20);

	while (window2.isOpen())
	{
		sf::Event event;
		while (window2.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window2.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
					window2.close();
				else if (event.key.code == sf::Keyboard::Return)
				{
					sf::Text displayText(playerPos, font, 20);
					displayText.setPosition(50, 50);
					displayText.setColor(sf::Color::Green);
					window2.close();
				}
				break;
			case sf::Event::TextEntered:
				playerPos += event.text.unicode;
				break;
			}
		}

		window2.draw(message2);

		sf::Text drawText(playerPos, font, 20);
		drawText.setColor(sf::Color::Red);
		drawText.setPosition(50,20);
		window2.draw(drawText);

		window2.display();
		window2.clear();
	}

	std::cout << id << std::endl;
	std::cout << playerPos << std::endl;

	paddle = atoi(playerPos.c_str());

	ip = sf::IpAddress::getLocalAddress();
	socket.connect("192.168.2.50", 9094);

	sf::Packet packet;
	packet << id;
	socket.send(packet);
	socket.setBlocking(false);

	playerName.setStringAndSize(id, 30);
	playerName.setFont("OptimusPrinceps.ttf");
	if (paddle == 1)
		playerName.setPosition(20, 0);
	else
		playerName.setPosition(SCREEN_WIDTH - (playerName.getGlobalBounds().width + 20), 0);
}