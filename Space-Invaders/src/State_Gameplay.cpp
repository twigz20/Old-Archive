#include <stdexcept>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "State_Gameplay.h"
#include "State_Credits.h"
#include "State_HighScores.h"
#include "State_Menu.h"

#include "Game.h"
#include "Operations.h"
#include "picojson.h"

std::string State_Gameplay::gameLevel(int level)
{
	std::string lev;
	switch (level) {
		case 1:
			lev = "one";
			break;
		case 2:
			lev = "two";
			break;
		case 3:
			lev = "three";
			break;
		default:
			break;
	}
	return lev;
};

State_Gameplay::State_Gameplay() :
	game(Game::getInstance()),
	rm(ResourceManager::getInstance())
{
    gameSound = nullptr;
    explosion = nullptr;
    quit_button = nullptr;
    menu_button = nullptr;
    restart_button = nullptr;
    highScores_button = nullptr;

    levelCleared = nullptr;
    win = nullptr;
    score = 0;
	life = 3;
    levelID = "level-one";
    lev = 1;
    restart();
    time.start();
    levelChange = false;
    gameSound = rm.getMusic("resources/sounds/Future World Music - Aqua Vitae.mp3");
    explosion = rm.getSound("resources/sounds/Enemy One Explosion.wav");
    levelCleared = rm.getTexture("resources/images/Level Cleared.png");
    win = rm.getTexture("resources/images/win.png");
    quit_button = new Button(300,600,200,100,rm.getTexture("resources/images/quitTransparent.png"));
    menu_button = new Button(100,720,200,100,rm.getTexture("resources/images/menu.png"));
    restart_button = new Button(300,720,200,100,rm.getTexture("resources/images/restart.png"));
    highScores_button = new Button(500,720,200,100,rm.getTexture("resources/images/High ScoresTransparent.png"));

    //Play the music
    Mix_FadeInMusic( gameSound, -1,25000 );

    // Tone down game Sound volume
    Mix_VolumeMusic( 40 );

    // Tone down explosion volume
    Mix_VolumeChunk( explosion, 3);
}

State_Gameplay::~State_Gameplay()
{
    gameSound = nullptr;
    explosion = nullptr;
    quit_button = nullptr;
    menu_button = nullptr;
    restart_button = nullptr;
    highScores_button = nullptr;
}

void State_Gameplay::checkCollisions(float deltaTime)
{
    // Iterates Through all Bullets
    for(unsigned int i = 0; i < bullets.size(); i++)
    {
        if(bullets[i]->getType() == PLAYER)
        {
            if(bullets[i]->getPosY() <= 0)
                bullets[i]->setDead(true);

            // Check if Bullets Collides with Creeps
            for(std::vector<std::unique_ptr<CreepInfo> >::iterator enemies = creeps.begin(); enemies != creeps.end(); ++enemies)
            {
                if(!(*enemies)->isDead())
                {
                    if((*enemies)->Collides(bullets[i].get()))
                    {
                        score += (*enemies)->getGold();
                        scoreChange = true;
                        bullets[i]->setDead(true);
                        (*enemies)->setEnemyHP((*enemies)->getEnemyHP()-bullets[i]->getBulletDamage());
                    }

                    if((*enemies)->getEnemyHP() <= 0)
                    {
                        Mix_PlayChannel( -1, explosion, 0 );
                        (*enemies)->setDead(true);
                    }
                }
            }
        }

        if(bullets[i]->getType() == ENEMY)
        {
            if(bullets[i]->getPosY() >= game.screen_height)
                bullets[i]->setDead(true);

            if(!aircraft.isDead())
            {
                if(aircraft.Collides(bullets[i].get()))
                {
                    life--;
                    lifeChange = true;
                    bullets[i]->setDead(true);

                    Mix_PlayChannel( -1, explosion, 0 );
                    aircraft.setDead(true);

                    time.stop();
                    time.start();
                    time.pause();
                }
            }
        }

    }

    for(std::vector<std::unique_ptr<CreepInfo> >::iterator enemies = creeps.begin(); enemies != creeps.end(); ++enemies)
    {
        if(!(*enemies)->isDead())
        {
            if((*enemies)->getPosX() <= 0)
            {
                //(*enemies)->setPosX(0);
                wallTouch = true;
                for(unsigned int i = 0; i < creeps.size(); i++)
                {
                    if((*enemies)->getPosY() < game.screen_height*.6 )
                        creeps[i]->moveDown(deltaTime);
                }
                break;
            }

            if((*enemies)->getPosX()+(*enemies)->getWidth() > game.screen_width)
            {
                //(*enemies)->setPosX(game.screen_width-(*enemies)->getWidth());
                wallTouch = false;
                for(unsigned int i = 0; i < creeps.size(); i++)
                {
                    if((*enemies)->getPosY() < game.screen_height*.6 )
                        creeps[i]->moveDown(deltaTime);
                }
                break;
            }
        }
    }

    // Deletes The Dead Bullets From Vector
    for(std::vector<std::unique_ptr<Bullet> >::iterator bullet = bullets.begin(); bullet != bullets.end();)
    {
        if((*bullet)->isDead())
        {
            bullet = bullets.erase(bullet);
        }
        else
             ++bullet;
    }

    // Deletes The Dead Creeps From Vector After Explosion Animation
    for(std::vector<std::unique_ptr<CreepInfo> >::iterator enemies = creeps.begin(); enemies != creeps.end();)
    {
        if((*enemies)->isDead() && (*enemies)->getExplosionEnd())
        {
             enemies = creeps.erase(enemies);
        }
        else
            ++enemies;
    }
}

void State_Gameplay::handleEvents(const float deltaTime)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
	{
        if(event.type == SDL_QUIT)
            game.popState();

        if(event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_ESCAPE)
            {
                game.popState();
            }
            if(event.key.keysym.sym == SDLK_h)
            {
                //game.pushState(std::make_shared<State_Intermission>());
            }
            if(event.key.keysym.sym == SDLK_SPACE && !gameOver)
            {
                if(!aircraft.isDead() )//&& bullets.size() == 0)
                {
                    std::unique_ptr<Bullet> ptr(new Bullet(PLAYER));
                    bullets.push_back(std::move(ptr));
                    for(unsigned int i = 0; i < bullets.size(); i++)
                    {
                        if(!bullets.back()->isDead())
                        {
                            bullets.back()->setPositions(aircraft.getPosX()+aircraft.getWidth()/2, aircraft.getPosY());
                        }
                    }
                }
            }
        }
	}

	if(gameOver || (creeps.size() == 0 && lev == maxLev))
    {
        if(!fileWritten)
        {
            fileWrite();
            fileWritten = true;
        }
        SDL_ShowCursor(1);
        bool leftClick = false;
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            leftClick = true;
        }
        if(event.type == SDL_MOUSEBUTTONUP)
        {
            if(event.button.button == SDL_BUTTON_LEFT)
            {
                leftClick = false;
            }
        }

        const Uint8 Button = SDL_BUTTON_LEFT;

        if(quit_button->CheckEvents(leftClick))
        {
            if( (event.button.button &Button) == Button && quit_button->IsOnTop(quit_button->getX(), quit_button->getY(), quit_button->getW(), quit_button->getH(), event.button.x, event.button.y))
            {
                fileWrite();
                // Popping Current State
                game.toPop_ = name();

                // Then Loading New State
				game.pushState(std::make_shared<State_Credits>());
            }
        }

        if(restart_button->CheckEvents(leftClick))
        {
            if( (event.button.button &Button) == Button && restart_button->IsOnTop(restart_button->getX(), restart_button->getY(), restart_button->getW(), restart_button->getH(), event.button.x, event.button.y))
            {
                time.stop();
                time.start();
                time.pause();
                lev = 1;
                levelID = "level-"+gameLevel(lev);
                restart();
                score = 0;
                life = 3;
                gameOver = false;
                time.unpause();
            }
        }

        if(highScores_button->CheckEvents(leftClick))
        {
            if( (event.button.button &Button) == Button && highScores_button->IsOnTop(highScores_button->getX(), highScores_button->getY(), highScores_button->getW(), highScores_button->getH(), event.button.x, event.button.y))
            {
                // Popping Current State
                game.toPop_ = name();

                game.pushState(std::make_shared<State_Help>());
            }
        }

        if(menu_button->CheckEvents(leftClick))
        {
            if( (event.button.button &Button) == Button && menu_button->IsOnTop(menu_button->getX(), menu_button->getY(), menu_button->getW(), menu_button->getH(), event.button.x, event.button.y))
            {
                // Popping Current State
                game.toPop_ = name();

                game.pushState(std::make_shared<State_Menu>());
            }
        }
    }
}

void State_Gameplay::update(const float deltaTime)
{
    if(!gameOver)
    {
        checkCollisions(deltaTime);

        aircraft.update(deltaTime);
        if(aircraft.getExplosionEnd())
        {
            aircraft.setDead(false);
            aircraft.setExplosionEnd(false);

            time.unpause();
        }

        for(std::vector<std::unique_ptr<CreepInfo> >::iterator enemies = creeps.begin(); enemies != creeps.end(); ++enemies)
        {
            if(!wallTouch)
            {
                (*enemies)->moveLeft(deltaTime);
            }
            else
            {
                (*enemies)->moveRight(deltaTime);
            }

            if(!(*enemies)->isDead() && !aircraft.isDead())
                {
                    if( time.getTicks()/1000 == unsigned((*enemies)->getShotTime()) )
                    {
                        std::unique_ptr<Bullet> ptr(new Bullet(ENEMY));
                        bullets.push_back(std::move(ptr));
                        for(unsigned int i = 0; i < bullets.size(); i++)
                        {
                            if(!bullets.back()->isDead())
                            {
                                bullets.back()->setPositions((*enemies)->getPosX()+(*enemies)->getWidth()/2, (*enemies)->getPosY()+(*enemies)->getHeight());
                            }
                        }
                    }
                }

            (*enemies)->update(deltaTime);
        }

        if(time.getTicks()/1000 >= 10)
        {
            time.stop();
            time.start();
        }

        for(unsigned int j = 0; j < bullets.size(); j++)
        {
            if(aircraft.isDead() || creeps.size() == 0)
            {
                bullets[j]->setDead(true);
                bullets.erase(bullets.begin() + j);
            }
        }

        for(std::vector<std::unique_ptr<Bullet> >::iterator bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
        {
            if(!(*bullet)->isDead())
            {
                (*bullet)->update(deltaTime);
            }
        }

        if(scoreChange)
        {
            snprintf(scoreS,20,"Score: %06d",score);
            scoreChange = false;
        }

        if(lifeChange)
        {
            snprintf(lifeS,20,"Life: %d",life);
            lifeChange = false;
        }

        if(life == 0)
            gameOver = true;
    }

    if(creeps.size() == 0 && lev < maxLev)
    {
        time.stop();
        time.start();
        time.pause();
        intermissionTimer--;
        if(intermissionTimer == 0)
        {
            lev++;
            levelID = "level-" + gameLevel(lev);
            restart();
            int upperB = 0;
            if(lev == 2)
                upperB = 230;
            else
                upperB = 170;

            for(std::vector<std::unique_ptr<CreepInfo> >::iterator enemies = creeps.begin(); enemies != creeps.end(); ++enemies)
            {
                (*enemies)->setUpperBound(upperB);
            }
            time.unpause();
        }
    }
}

void State_Gameplay::render(const float deltaTime)
{
    SDL_RenderClear( Game::getInstance().renderer );

    if(!gameOver)
    {
        aircraft.render();

        for(std::vector<std::unique_ptr<CreepInfo> >::iterator enemies = creeps.begin(); enemies != creeps.end(); ++enemies)
        {
            (*enemies)->render();
        }

        for(std::vector<std::unique_ptr<Bullet> >::iterator bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
        {
            if(!(*bullet)->isDead())
            {
                (*bullet)->render();
            }
        }

        // Display Score
        auto scoreInfo = Texture(scoreS,SDL_Color{ 255, 255, 255, 128 },rm.getFont("resources/fonts/Exo-MediumItalic.otf", 20, false));
        scoreInfo.render(20,20);

        // Display Life
        auto lifeInfo = Texture(lifeS,SDL_Color{ 255, 255, 255, 128 },rm.getFont("resources/fonts/Exo-MediumItalic.otf", 20, false));
        lifeInfo.render(680,20);

        if(creeps.size() == 0 && lev < maxLev)
        {
            levelCleared->render(150,350);
        }
        if(lev == maxLev && creeps.size() == 0)
        {
            win->render(150,350);
            quit_button->DrawToScreen();
            menu_button->DrawToScreen();
            restart_button->DrawToScreen();
            highScores_button->DrawToScreen();
        }
    }
    else {
        quit_button->DrawToScreen();
        menu_button->DrawToScreen();
        restart_button->DrawToScreen();
        highScores_button->DrawToScreen();
    }
}

void State_Gameplay::restart()
{
    bullets.clear();
    creeps.clear();
    std::string fileStr = readFile("resources/files/level.json");
    picojson::value v;
    std::string err;
    picojson::parse(v, fileStr.begin(), fileStr.end(), &err);

    picojson::object& obj = v.get<picojson::object>();
	auto wavesArray = obj[levelID.c_str()].get<picojson::array>();

	for (picojson::array::const_iterator i = wavesArray.begin(); i != wavesArray.end(); ++i)
	{
	    auto waveObj = (*i).get<picojson::object>();
		auto creepsArray = waveObj["creeps"].get<picojson::array>();

        for (picojson::array::const_iterator creepss = creepsArray.begin(); creepss != creepsArray.end(); ++creepss)
		{
			auto creepObj = (*creepss).get<picojson::object>();

            std::string level = creepObj["level"].get<std::string>();
            std::string type = creepObj["type"].get<std::string>();

            std::unique_ptr<CreepInfo> ptr(new CreepInfo(creepColor(type),creepLevel(level)));
            creeps.push_back(std::move(ptr));
			creeps.back()->setPositions(creepObj["x"].get<double>(),creepObj["y"].get<double>());
		}
	}

	wallTouch = false;
    scoreChange = true;
	lifeChange = true;
	fileWritten = false;
	intermissionTimer = 200;
}

void State_Gameplay::fileWrite()
{
    int place;
    std::vector<int> placement;
    //std::string* userName = nullptr;
    int hghscr;
    std::vector<int> highscore;

    std::ofstream outfile("resources/files/temp.txt", std::ofstream::app);
    std::fstream infile("resources/files/highscores.txt");

    int nRec = 0;
    std::string check;
    while(getline(infile,check))
    {
        nRec++;
    }

    infile.clear();
    infile.seekg(0, std::ios::beg);

    std::stringstream ss;
    char colon;
    for(int i = 0; i < nRec; i++)
    {
        infile >> place >> colon >> hghscr;
        placement.push_back(place);
        highscore.push_back(hghscr);
        //getline(infile, userName[i], ':');

    }

    placement.push_back(++nRec);
    highscore.push_back(score);

    std::sort(highscore.begin(), highscore.end(), std::greater<int>());

    if(nRec <= 10)
    {
        for(int i = 0; i < nRec; i++)
        {
            outfile << placement[i] << colon << highscore[i] << std::endl;
        }
    }
    else
    {
        for(int i = 0; i < 10; i++)
        {
            outfile << placement[i] << colon << highscore[i] << std::endl;
        }
    }

    outfile.close();

    infile.clear(); // clear eof and fail bits
    infile.seekg(0, std::ios::beg);
    infile.close();

    remove("resources/files/highscores.txt");
    rename("resources/files/temp.txt","resources/files/highscores.txt");
}

std::string State_Gameplay::name() const
{
	return "State_Gameplay";
}
