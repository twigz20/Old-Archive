//PlayState.cpp

#include "PlayState.h"

PlayState::PlayState()
{
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
	enterToStart.setPosition((SCREEN_WIDTH / 2) - enterToStart.getGlobalBounds().width/2, (SCREEN_HEIGHT / 2) - enterToStart.getGlobalBounds().height);

	pause.setFont("OptimusPrincepsSemiBold.ttf");
	pause.setStringAndSize("PAUSE", 70);
	pause.setColor(sf::Color(255, 255, 255));
	pause.setPosition((SCREEN_WIDTH / 2) - pause.getGlobalBounds().width / 2, (SCREEN_HEIGHT / 2) - pause.getGlobalBounds().height);

	escToExit.setFont("OptimusPrincepsSemiBold.ttf");
	escToExit.setStringAndSize("\t Press\nEsc to Exit", 40);
	escToExit.setColor(sf::Color(255, 0, 0));
	escToExit.setPosition((SCREEN_WIDTH / 2) - escToExit.getGlobalBounds().width / 2, 335);
}

PlayState::~PlayState()
{
}

void PlayState::HandleEvents(Game &game) {
	while (game.window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game.setRunning(false);

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				if (!paused)
					StartPause();
				else
					game.ChangeState(Game::gameStates::MAINMENU);
			}

			if (event.key.code == sf::Keyboard::Return && paused)
				EndPause();

			if (event.key.code == sf::Keyboard::Return && !paused) {
				firstTouch = false;
				ballProperties.delayStart = false;
			}
		}
	}
}

void PlayState::Update(Game &game)
{
		if (!paused)
			elapsedTime = clock.getElapsedTime();
		else
			elapsedTime = sf::seconds(0);

		HandleCollisions(game);
		aiMove(elapsedTime);

		player[0]->Update(elapsedTime.asSeconds());

		if (!ballProperties.delayStart)
			ball->Update(elapsedTime.asSeconds());

		clock.restart();
}

void PlayState::Render(Game &game)
{
	bg.Render(game.window);
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
	}
}

void PlayState::Clear()
{
	points = 0;
}
void PlayState::StartPause()
{
	enterToStart.setColor(sf::Color(165, 250, 0));
	enterToStart.setPosition((SCREEN_WIDTH / 2) - enterToStart.getGlobalBounds().width / 2, 150);
	paused = true;
}
void PlayState::EndPause()
{
	paused = false;
}

void PlayState::HandleUpdates(Game &game)
{
	
}

void PlayState::HandleDraws(Game &game)
{
	
}

void PlayState::HandleCollisions(Game &game)
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
		ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetHeight() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));
		ball->setBallDirection(-1, -1);
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
		ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetWidth() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));
		ball->setBallDirection(1, 1);
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

float PlayState::GetReflection(int index, float hity) {
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

void PlayState::setPaddlePosition(float index, float y)
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

void PlayState::aiMove(sf::Time& elapsedTime)
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