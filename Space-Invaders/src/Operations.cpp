#include "Operations.h"
#include <cstdlib>
#include <fstream>

std::string readFile(const std::string& filename)
{
    SDL_RWops *rw = SDL_RWFromFile(filename.c_str(), "r");
    if(rw == nullptr)
    {
        SDL_Log("Could not open file \"%s\".\n", filename.c_str());
        return "";
    }

	/* Seek to 0 bytes from the end of the file */
	Sint64 length = SDL_RWseek(rw, 0, RW_SEEK_END);
	if (length < 0)
	{
        SDL_Log("Could not seek inside \"%s\".\n", filename.c_str());
        return "";
	}

	SDL_RWseek(rw, 0, SEEK_SET);
	unsigned char* data = (unsigned char*)malloc(length);
	long total = 0;
	long len = 0;
	while((len = SDL_RWread(rw, data, 1, length)) > 0 && (total < length))
	{
	    total += len;
	}
	std::string ret((const char *)data, length);
//	SDL_Log("ARQUIVO LIDO: %s", ret.c_str());
	free(data);
	SDL_RWclose(rw);

	return ret;
}

/*
const int TOTAL_DATA = 2;
    //Data points
    Sint32 gData[ TOTAL_DATA ];

     //Open file for reading in binary
    SDL_RWops* file = SDL_RWFromFile( "resources/files/nums.txt", "r+b" );

    //File does not exist
    if( file == NULL )
    {
       std::cout << "Warning: Unable to open file! SDL Error: " << SDL_GetError() << std::endl;

        //Create file for writing
        file = SDL_RWFromFile( "resources/files/nums.txt", "w+b" );

        if( file != NULL )
        {
            std::cout << "New file created!\n";

            //Initialize data
            for( int i = 0; i < TOTAL_DATA; ++i )
            {
                gData[ i ] = i;
                SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
            }

            //Close file handler
            SDL_RWclose( file );
        }
        else
        {
            std::cout << "Error: Unable to create file! SDL Error: " << SDL_GetError() << std::endl;;
        }
    }

    //File exists
    else
    {
        //Load data
        std::cout << "Reading file...!" << std::endl;
        for( int i = 0; i < TOTAL_DATA; ++i )
        {
            SDL_RWread( file, &gData[ i ], sizeof(Sint32), 1 );
        }

        //Close file handler
        SDL_RWclose( file );
    }

    */
