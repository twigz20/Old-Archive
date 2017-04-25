#include "State_HighScores.h"
#include "State_Gameplay.h"
#include <iostream>
#include <stdexcept>
#include <fstream>

State_Help::State_Help() :
    game(Game::getInstance()),
    rm(ResourceManager::getInstance())

{
    //music = nullptr;
	background = nullptr;
    back_button = nullptr;
    allScores = nullptr;

	background = rm.getTexture("resources/images/Main Menu.png");
	//music = rm.getMusic("resources/sounds/Tango.wav");
	back_button = new Button(650,850,100,50,rm.getTexture("resources/images/backTransparent.png"));
    highScoresText = new Texture("High Scores",SDL_Color{ 255, 255, 255, 128 },rm.getFont("resources/fonts/Exo-MediumItalic.otf", 20, false));
    //Play the music
    //Mix_FadeInMusic( music, -1, 25000 );

    fileRead();
}

State_Help::~State_Help()
{
    delete back_button;
    back_button = nullptr;

	background = nullptr;

    //Mix_FreeMusic( music );
    //music = nullptr;
}

void State_Help::handleEvents(const float deltaTime)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
	{
        bool leftClick = false;
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
                fileRead();
            }
        }
        if(event.type == SDL_MOUSEBUTTONDOWN)
        {
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

            if(back_button->CheckEvents(leftClick))
            {
                if( (event.button.button &Button) == Button && back_button->IsOnTop(back_button->getX(), back_button->getY(), back_button->getW(), back_button->getH(), event.button.x, event.button.y))
                {
                    game.popState();
                }
            }
        }
	}
}

void State_Help::update(const float deltaTime)
{

}

void State_Help::render(const float deltaTime)
{
    SDL_RenderClear( Game::getInstance().renderer );
    background->render();
    highScoresText->render(350,300);
    back_button->DrawToScreen();

    int posY = 350;
    for(std::vector<Texture* >::iterator sco = s.begin(); sco != s.end(); ++sco)
    {
        (*sco)->render(100,posY);
        posY+=25;
    }
}

void State_Help::restart()
{

}

void State_Help::fileRead()
{
    int* placement = nullptr;
    //std::string* userName = nullptr;
    int* highscore = nullptr;

    std::ifstream infile("resources/files/highscores.txt");

    int nRec = 0;
    std::string check;
    while(getline(infile,check))
    {
        nRec++;
    }
    infile.clear();
    infile.seekg(0, std::ios::beg);

    placement = new int[nRec];
    //userName = new std::string[nRec];
    highscore = new int[nRec];

    std::stringstream ss;
    char colon;
    for(int i = 0; i < nRec; i++)
    {
        infile >> placement[i] >> colon >> highscore[i];
        //getline(infile, userName[i], ':');

        char ss[100];
        snprintf(ss,100,"%18d%80d",placement[i]/*,userName[i].c_str()*/,highscore[i]);
        Texture* scoreInfo = new Texture(ss,SDL_Color{ 255, 255, 255, 128 },rm.getFont("resources/fonts/Exo-MediumItalic.otf", 20, false));

        s.push_back(scoreInfo);
        scores.push_back(ss);
    }

    infile.clear(); // clear eof and fail bits
    infile.seekg(0, std::ios::beg);
    infile.close();
}

std::string State_Help::name() const
{
	return "State_Help";
}

