#include "catchThePokemon.h"
#include <stdlib.h>

char *catchThePokemonThreads[catchThePokemonThreadCount] = {"startbtn", "abcbtn", "ledcircle", "buzzers"}; // missing gps and lcd threads
bool gameOngoing, catchEvent;
uint8_t balls, pokemonCaught, pokemonFled;

/**
 * @brief returns the threads used in the catch the pokemon minigame
 * @param names pointer to a char array to store the thread names
 * @param amount pointer to an unsigned int to store the amount of threads
 */
void getCatchThePokemonThreads(char ***names, unsigned *amount)
{
    *names = catchThePokemonThreads;
    *amount = catchThePokemonThreadCount;
}

/**
 * @brief checks if you have caught all pokemon and triggers the end game if not.
 * @returns false if you have caught all pokemon or they have fleed
 */
bool checkPokemonLeft()
{
    if(pokemonCaught >= POKEMON)
    {
        lcdStringWrite("You have caught all pokemon!");

        gameOngoing = false;
    }

    else if ((pokemonCaught  + pokemonFled) >= POKEMONLOCATIONS)
    {
        lcdStringWrite("No pokemon left!");

        gameOngoing = false;
    }

    return gameOngoing;
}

/**
 * @brief checks if you have balls left and triggers the end game if not.
 * @returns false if you don't have balls left
 * true if you have balls left
 */
bool checkBallsLeft()
{
    if (balls <= 0)
    {
        lcdStringWrite("Out of pokeballs");

        gameOngoing = false;
        return false;
    }

    return true;
}

/**
 * @brief checks if the locations are valid
 * @returns 0 if everything went as expected
 */
int selectLocations()
{
    for (int i = 0; i < POKEMONLOCATIONS; i++)
    {
        // fill location array with random available locations
    }
    return true;
}

/**
 * @brief initialises variables, locations and hints used
 * @returns 0 if everything went as expected
 */
int initMg()
{
    int err;

    // init variables
    balls = BALLS;
    pokemonCaught = 0;
    pokemonFled = 0;
    gameOngoing = true;
    catchEvent = false;

    err = selectLocations();
    // init hints
    return 0;
}

/**
 * @brief checks if the catching minigame is ongoing
 * @returns 0 if everything went as expected
 */
int catchingMg()
{
    bool catchingMgOngoing = true;
    // int err;
    uint8_t attemptCounter = 0;

    // TODO: Make the seed less predictable
    srand(0); // seed random number generator

    uint8_t attemptsPermitted = 1 + (rand() % MAX_ATTEMPTS); // random number between .. and ..
    bool missedPokemon = false;

    // init catchingMg
    while (catchingMgOngoing)
    {
        if (checkBallsLeft() == false) {
            return 0;
        }

        // if (input)
        // {

        // handle input etc

        balls--;
        attemptCounter++;

        if (attemptCounter > attemptsPermitted)
        {
            lcdStringWrite("Pokemon fled!");
            pokemonFled++;
            catchingMgOngoing = false;
        }
        else
        {
            if (missedPokemon == false) {
                // play sound
                lcdStringWrite("You caught a pokemon!");
                pokemonCaught++;
                catchingMgOngoing = false;
            }
            else
            {
                lcdStringWrite("You missed the pokemon!");
                // play sound
            }
        }
        // }
    }

    return 0;
}

/**
 * @brief checks if a pokemon is nearby
 * @returns true if there a pokemon close enough to trigger catchPokemonEvent false if there are no pokemon nearby
 */
bool pokemonNearby() {
    for (int i = 0; i < POKEMONLOCATIONS; i++)
    {
        // check if the pokemon is within the distance
        // if (getDistanceMeters(getLatitude(), getLongitude(), pokemonLocation[i].lat, pokemonLocation[i].long) < POKEMON_DISTANCE)
        // return true;
    }
    return false;
}

/**
 * @brief checks if a pokemon is nearby and triggers the catchPokemonEvent
 * @returns 0 if everything went as expected
 */
int pokemonGame()
{
    // check if game finish conditions have been met
    if(checkBallsLeft() == false || checkPokemonLeft() == false)
        return 0;

    // check if there is a pokemon nearby
    if (pokemonNearby())
    {
        // play pokemon appeared sound
        catchEvent = true;
    }
    else
    {
        // hint event
    }

    return 0;
}

int playCatchThePokemon()
{
    int err;
    err = initMg();
    if (err)
    {
        printf_catchThePokemon("Init error %d", err);
        // return err code if severe enough?
    }

    while (gameOngoing)
    {
        // draw game
        // play sounds
        // check input
        // handle input
        if(!catchEvent)
        {
            err = pokemonGame();
            if (err)
            {
                printf_catchThePokemon("Pokemon Error %d", err);
            }
        }
        else
        {
            catchEvent = false;

            err = catchingMg();
            if (err)
            {
                printf_catchThePokemon("Pokemon Catching Minigame Error %d", err);
            }
        }

        // delay
    }

    // gameover + handle score
    // cleanup game

    return 0;
}