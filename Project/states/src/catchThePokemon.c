#include "catchThePokemon.h"
#include <stdlib.h>

char *catchThePokemonThreads[catchThePokemonThreadCount] = {"startbtn", "abcbtn", "ledcircle", "buzzers", "ledmatrix"}; // missing lcd threads
bool gameOngoing, catchEvent;
uint8_t balls, pokemonCaught, pokemonFled;
uint16_t displayMatrix[16] = {0};
int pokemonToCatch = -1;

struct pokemonLocation
{
    int id;
    int64_t lat;
    int64_t longi;
    bool caught;
    bool fled;
} pokemonLocation[POKEMONLOCATIONS] = {0};

uint8_t pokemonMatrixLocation[] = {
    0b00001110,
    0b00010001,
    0b00010101,
    0b00010001,
    0b00001110
};

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
 * @brief pushes matrix to ledmatrix and sets the displayMatrix to 0
 * @returns 0 if everything went as expected
 */
int setMatrix()
{
    ledmatrixSetMutexValue(displayMatrix);

    // clear displayMatrix
    memset(displayMatrix, 0, sizeof(displayMatrix));
    return 0;
}

/**
 * @brief displays the pokemon location on the 16x16 LED
 * @param x center x coordinate of the pokemon on the 16x16 LED
 * @param y center y coordinate of the pokemon on the 16x16 LED
 * @returns 0 if everything went as expected
 * @note Advisory position is to not go below Y 8
 */
int displayPokemon(char x, char y)
{
    int8_t offsetX = -3;
    int8_t offsetY = -3;
    for (int i = 0; i < 5; i++)
    {
        displayMatrix[(y+offsetY+i)] += pokemonMatrixLocation[i] << (16-x+offsetX);
    }
    return 0;
}

/**
 * @brief displays the direction aimed on the 16x16 LED
 * @param angle angle in 15 degree increments 0 is 45 degrees to the left and 15 is 45 degrees to the right
 * @returns 0 if everything went as expected
 */
int displayAimDirection(char angle)
{
#define CURVE_DAMPEN 0.5 // affects how quickly the curve increases
#define AIM_DIRECTION_OFFSET 8
    // Map the angle to a direction (-7 to +7)
    int direction = (angle * 14) / 15 - 7; // Map angle (0-15) to range (-7 to +7)

    // Draw the line on the bottom 8 rows
    for (int y = 15; y >= AIM_DIRECTION_OFFSET; y--) {
        int curveFactor = (15 - y) * (15 - y) / 16; // Add a curve factor
        if (angle > 7) {
            curveFactor *= -1;
        }
        int x = AIM_DIRECTION_OFFSET + direction * (15 - y) / 7 + CURVE_DAMPEN * curveFactor; // Adjust x with curve
        if (x >= 0 && x < 16) {
            displayMatrix[y] |= 1 << (15 - x); // Set the bit at position x
        }
    }

    // cleanup curve
    for (int y = AIM_DIRECTION_OFFSET + 1; y < 2*AIM_DIRECTION_OFFSET-1; y++)
    {
        if (( (angle <= 7) && (displayMatrix[y] < displayMatrix[y+1])) || ((angle > 7) && (displayMatrix[y] > displayMatrix[y+1])))
            displayMatrix[y] = displayMatrix[y+1];
    }

    return 0;
}

/*
 * @brief displays the timing to throw the ball on the 16x16 LED
 * @param[in] time time to display on a scale of 0-10
 * @returns 0 if everything went as expected
 */
int displayTiming(char time)
{
    uint8_t ledCircle[8];
    ledCircle[0] = 0b11111111;
    ledCircle[7] = 0b11111111;
    ledcircleSetMutexValue(ledCircle);
    return 0;
}

int displayBallThrow(char x, char y, char angle)
{
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

        // display 'pokemon' location 16x16 LED
        // display direction aimed 16x16 LED
        // display timing to throw ball (LED Circle)

        setMatrix();
        k_msleep(10000);

        // if (input)
        // {

        balls--;
        attemptCounter++;

        // }

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

        if (attemptCounter > attemptsPermitted)
        {
            lcdStringWrite("Pokemon fled!");
            pokemonFled++;
            catchingMgOngoing = false;
        }
    }

    return 0;
}

/**
 * @brief checks if a pokemon is nearby
 * @returns true if there a pokemon close enough to trigger catchPokemonEvent false if there are no pokemon nearby
 */
bool pokemonNearby() {
    for (int i = 0; i < (POKEMONLOCATIONS); i++)
    {
        // check if the pokemon is within the distance
        if (getDistanceMeters(getLatitude(), getLongitude(), pokemonLocation[i].lat, pokemonLocation[i].longi) < POKEMON_DISTANCE)
        {
            pokemonToCatch = i;
            return true;
        }
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
        // display general direction of the pokemon
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

    displayPokemon(5, 3);
    displayAimDirection(0);
    displayTiming(0);
    setMatrix();
    k_msleep(10000);

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