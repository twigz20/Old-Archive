#include "Game.h"
#include "GameState.h"

/*
Our new functions, ChangeState() takes a pointer to a GameState as a parameter and then pushes that state onto the vector of pointers to GameStates, before that it uses the clean function to remove the old state from the stack.
*/
void Game::ChangeState(GameState* state)
{
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->Quit();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->Init(this);
}

void Game::DeleteAllPreviousState(GameState* state)
{
    // cleanup the states state
	while ( !states.empty() ) {
		states.back()->Quit();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->Init(this);
}

void Game::PushState(GameState* state)
{
	states.push_back(state);
	if(state->Init(this) == false)
	{
		quit = true;
	}
}

void Game::PopState()
{
	states.back()->Quit();
	delete states.back();
	states.pop_back();

	//if we just popped the last state
	if(states.size() == 0)
	{
		quit = true;
	}
}
