#include "catchThePokemon.h"
#include <stdlib.h>

#define TIME_MAX 10
#define TIME_MIN 0
#define TIME_STEP 1
#define TIME_INTERVAL_MS 100 // Adjust for smoother or faster changes

char *catchThePokemonThreads[catchThePokemonThreadCount] = {"startbtn", "btnmatrix_in", "ledcircle", "buzzers", "ledmatrix", "btnmatrix_out"}; // missing lcd threads
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

// TODO: This should be done inside the button matrix not in the game
/**
 * @brief checks if any button is pressed
 * @param buttonsInput pointer to an array of button states
 * @param size size of the buttonsInput array
 * @returns true if any button is pressed, false otherwise
 * @note handles the inversion of the button states too
 */
bool isAnyButtonPressed(const uint8_t *buttonsInput, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (!buttonsInput[i]) {
            return true; // Return immediately if a button is pressed
        }
    }
    return false; // No button is pressed
}

static bool increasing = true;

/**
 * @brief updates the time value based on the increasing or decreasing state
 * @param time pointer to the time value to be updated
 */
void updateTime(char *time)
{
    // Update the time value based on the increasing or decreasing state
    if (increasing) {
        *time += TIME_STEP;
    } else {
        *time -= TIME_STEP;
    }

    // Cap the time value at the defined limits
    if (*time >= TIME_MAX) {
        *time = TIME_MAX; // Cap the time at max
        increasing = false; // Switch to decreasing when max is reached
    }
    else if (*time <= TIME_MIN) {
        *time = TIME_MIN; // Cap the time at min
        increasing = true; // Switch to increasing when min is reached
    }
}

void nonBlockingTimeHandler(char *time)
{
    static int64_t lastUpdate = 0;
    int64_t currentTime = k_uptime_get(); // Get the current time in milliseconds

    if (currentTime - lastUpdate > TIME_INTERVAL_MS)
    {
        updateTime(time);
        lastUpdate = currentTime; // Update the last update time
    }
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

    lcdEnable();
    lcdStringWrite("Find and catch the Pokemon!");
    k_msleep(3000);

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
 * @note X position should be between 3 and 12
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

// TODO: Fix this to work with the hardware.
/*
 * @brief displays the timing to throw the ball on the 16x16 LED
 * @param[in] time time to display on a scale of 0-10
 * @returns 0 if everything went as expected
 */
int displayTiming(char time)
{
    // Initialize the LED circle array and temporary variable
    uint8_t ledCircle[8] = {0};
    uint32_t temp = 0;

    // Generate the bit pattern based on the input time
    for (int i = 0; i <= time; i++)
    {
        temp = (temp << 3) | 0b111; // Append 3 bits of 1 for each step
    }

    // Process the first 4 bytes of the temp variable
    for (int i = 0; i < 4; i++) {
        // Extract and reverse the bits of the current byte
        uint8_t currentByte = (temp >> (i * 8)) & 0xFF; // Extract the current byte
        uint8_t reversedByte = ((currentByte & 0xF0) >> 4) | ((currentByte & 0x0F) << 4);
        reversedByte = ((reversedByte & 0xCC) >> 2) | ((reversedByte & 0x33) << 2);
        reversedByte = ((reversedByte & 0xAA) >> 1) | ((reversedByte & 0x55) << 1);

        // Assign the reversed byte to the second half of the LED circle
        ledCircle[i + 4] = reversedByte;

        // Assign the original byte to the first half of the LED circle
        ledCircle[i] = (temp >> (24 - (i * 8))) & 0xFF;
    }

    // Update the LED circle with the generated values
    ledcircleSetMutexValue(ledCircle);

    return 0;
}

/**
 * @brief displays the ball throw animation on the 16x16 LED
 * @returns 0 if everything went as expected
 * @note overrided previous display on the ledmatrix
 */
int displayBallThrow(void)
{
#define BALLTHROWNARRAYSIZE 4
    // technically could've only defined 1/4th of the array and mirrored it(twice)
    uint16_t ballThrown[BALLTHROWNARRAYSIZE][16] =  {
        {
            0, 0, 0, 0, 0, 0,
            0b0000000110000000,
            0b0000001111000000,
            0b0000001111000000,
            0b0000000110000000,
            0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0,
            0b0000000110000000,
            0b0000001001000000,
            0b0000010110100000,
            0b0000010110100000,
            0b0000001001000000,
            0b0000000110000000,
            0, 0, 0, 0, 0
        },
        {
            0, 0, 0,
            0b0000001111000000,
            0b0000010000100000,
            0b0000100000010000,
            0b0001000110001000,
            0b0001001111001000,
            0b0001001111001000,
            0b0001000110001000,
            0b0000100000010000,
            0b0000010000100000,
            0b0000001111000000,
            0, 0, 0
        },
        {
            0b0000000000000000,
            0b0000001111000000,
            0b0000110000110000,
            0b0001000000001000,
            0b0010001111000100,
            0b0010010000100100,
            0b0100100110010010,
            0b0100101111010010,
            0b0100101111010010,
            0b0100100110010010,
            0b0010010000100100,
            0b0010001111000100,
            0b0001000000001000,
            0b0000110000110000,
            0b0000001111000000,
            0b00000000000000000
        },
    };
    // TODO: Make non-blocking
    for ( int i = 0; i < 4; i++)
    {
        ledmatrixSetMutexValue(ballThrown[i]);
        k_msleep(250);
    }
    return 0;
}

/**
 * @brief Calculates the score based on how well the angle aims at the target x.
 * @param x The target x-coordinate.
 * @param angle The angle in 15-degree increments (0-15).
 * @return A score where lower values are better
 * @note if the score is -1, the ball missed the pokemon
 */
int calculateAimScore(int x, int angle)
{
    // Map the angle to a direction (-7 to +7)
    int direction = (angle * 14) / 15 - 7; // Map angle (0-15) to range (-7 to +7)
    int projectedX = 8 + direction; // Centered around 8 (middle of 16x16 matrix)

    // Calculate the distance from the target x
    int distance = abs(projectedX - x);

    return distance < 5 ? distance : -1; // Return a score
}

/**
 * @brief checks if the ball hit the pokemon based of the aim and timing
 * @param x horizontal position of the pokemon
 * @param time the time at which the ball was thrown 10 is the best time 0 is the worst
 * @param angle angle of the ball throw
 * @return true if the ball hit the pokemon, false if it missed
 */
bool checkHit(char x, char time, char angle)
{
    int aimScore = calculateAimScore(x, angle);
    if (aimScore == -1) // check if the ball missed the pokemon
        return false;

    displayBallThrow(); // display the ball throw animation
    memset(displayMatrix, 0, sizeof(displayMatrix)); // clear the display matrix
    setMatrix();

    int catchChance = aimScore + time; // calculate the catch chance max 15
    if (catchChance >= 15) // check if the catch chance is greater or equal to 15 then the pokemon is caught
        return true;

    float randomValue = (float)rand() / 1; // convert to float
    float probability = 1.0 - exp(-0.5 * catchChance + 3); // calculate the probability of catching the pokemon
    return randomValue < probability; // if the random value is less than the probability then the pokemon is caught
}

/**
 * @brief converts the roll value to an angle between 0 and 15
 * @param roll the roll value from the gyroscope
 * @return the angle between 0 and 15
 */
char rollToAngle(int roll)
{
    // Map the roll value to an angle between 0 and 15
    if (roll < -45)
        return 0;
    else if (roll > 45)
        return 15;
    else
        return (roll + 45) / 6; // Map to range 0-15
}

/**
 * @brief checks if the catching minigame is ongoing
 * @returns 0 if everything went as expected
 */
int catchingMg()
{
    uint8_t matrixLedsOn[] = {0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t matrixLedsOff[] = {0x00, 0x00, 0x00, 0x00};
    bool catchingMgOngoing = true;
    // int err;
    uint8_t attemptCounter = 0;
    int roll = 0;
    char angle = 0; // angle in which the box is tilted (0 to 15)
    char time = 0; // timing to throw the ball (0 to 10)

    // TODO: Make the seed less predictable
    srand(0); // seed random number generator

    uint8_t attemptsPermitted = 1 + (rand() % MAX_ATTEMPTS); // random number between .. and ..
    bool caughtPokemon = false;
    // generate pokemon locations
    uint8_t pokemonLocationX = 3 + rand()%11;
    uint8_t pokemonLocationY = 3 + rand()%3;

    // init catchingMg
    while (catchingMgOngoing)
    {
        // update time
        nonBlockingTimeHandler(&time);

        // debug
        char digits[4] = {0};

        // Convert the time to individual digits
        digits[0] = (time / 1000) % 10 + 48; // Thousands place
        digits[1] = (time / 100) % 10 + 48;  // Hundreds place
        digits[2] = (time / 10) % 10 + 48;   // Tens place
        digits[3] = time % 10 + 48;          // Units place
        sevenSegmentSet(digits, 2);
        //

        if (checkBallsLeft() == false) {
            return 0;
        }

        // get the angle of the box
        gyroscope_get_roll(&roll);
        angle = rollToAngle(roll); // convert roll to angle

        displayPokemon(pokemonLocationX, pokemonLocationY);
        displayAimDirection(angle);
        displayTiming(time);

        setMatrix();

        uint8_t *buttonsInput = btnmatrix_inGetMutexValue();

        if (isAnyButtonPressed(buttonsInput, sizeof(buttonsInput)))
        {
            balls--;
            attemptCounter++;
            btnmatrix_outSetMutexValue(matrixLedsOff); // set all LEDs to 0

            caughtPokemon = checkHit(pokemonLocationX, time, angle);

            if (caughtPokemon == true)
            {
                // play sound
                lcdStringWrite("You caught a pokemon!");
                pokemonCaught++;
                k_msleep(1000);
                catchingMgOngoing = false;
            }
            else
            {
                lcdStringWrite("You missed the pokemon!");
                k_msleep(1000);
                // play sound
            }
        }
        else
        {
            lcdStringWrite("Tilt to aim and throw the ball with the lid up buttons!");
            btnmatrix_outSetMutexValue(matrixLedsOn); // set all LEDs to 1
            k_msleep(10);
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
        lcdStringWrite("Init error");
        k_msleep(1000);
        // return err code if severe enough?
    }

    while (gameOngoing)
    {
        // draw game
        // play sounds

        if(!catchEvent)
        {
            err = pokemonGame();
            if (err)
            {
                printf_catchThePokemon("Pokemon Error %d", err);
                lcdStringWrite("Pokemon Error");
                k_msleep(1000);
            }
        }
        else
        {
            catchEvent = false;

            err = catchingMg();
            if (err)
            {
                printf_catchThePokemon("Pokemon Catching Minigame Error %d", err);
                lcdStringWrite("Pokemon Catching Minigame Error");
                k_msleep(1000);
            }
        }

        // delay
    }

    // gameover + handle score
    // cleanup game

    return 0;
}