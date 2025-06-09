#include "catchThePokemon.h"

LOG_MODULE_REGISTER(catchThePokemon);

#define BALLS 16
#define POKEMON 5 // number of pokemon to catch
#define POKEMONLOCATIONS 32 // number of locations where pokemon can be found
#define MAX_ATTEMPTS 8
#define POKEMON_DISTANCE 10 // meters

#define HINT_COOLDOWN 1 // in minutes

#define TIME_MAX 10
#define TIME_MIN 0
#define TIME_STEP 1
#define TIME_INTERVAL_MS 100 // Adjust for smoother or faster changes

// sounds
#define START_GAME_SOUND 5
#define END_GAME_SOUND 6

#define MAX_SCORE 1000 // Maximum score for the game

// Curve parameters for aim minigame
#define CURVE_DAMPEN 0.5 // affects how quickly the curve increases
#define AIM_DIRECTION_OFFSET 8

#define BALLTHROWNARRAYSIZE 4

char *catchThePokemonThreads[catchThePokemonThreadCount] = {"startbtn", "btnmatrix_in", "ledcircle", "buzzers", "ledmatrix", "btnmatrix_out"}; // missing lcd threads
bool game_ongoing_pokemon, catchEvent;
uint8_t balls, pokemonCaught, pokemonFled;
uint16_t displayMatrix[16] = {0};
uint64_t startTime;
uint8_t pokemonToCatch[POKEMON] = {0};
static bool increasing = true;

struct pokemonLocation
{
    uint8_t id;
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

/////////////
/// sounds:
int soundCatch[2][3] = {
    {587, 740, 880}, // D chord
    {784, 988, 1175} // G chord
};

int soundFlee[2][3] = {
    {587, 740, 880}, // D chord
    {554, 698, 830}  // C# chord
};

int soundCatchFail[2][3] = {
    {880, 523, 660}, // Cm chord
    {415, 262, 349}  // Fm chord
};

int soundAppear[2][3] = {
    {262, 330, 302},  // C chord
    {784, 1175, 1568} // G D G
};

int soundNoBalls[2][3] = {
    {554, 698, 830},
    {523, 659, 784} // C chord  // C# chord
};

int soundStartGame[3][3] = {
    {349, 349, 349},    // F4
    {698, 698, 698},    // F5
    {1397, 1397, 1397}  // F6
};

int soundEndGame[3][3] = {

    {1760, 1760, 1760},  // A6
    {880, 880, 880},    // A5
    {440, 440, 440},    // A4
};
/////////////

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

// TODO: This should be done inside the button matrix or helperFunctions not in the game
/**
 * @brief checks if any button is pressed
 * @param buttonsInput pointer to an array of button states
 * @param size size of the buttonsInput array
 * @returns true if any button is pressed, false otherwise
 * @note handles the inversion of the button states too
 */
bool isAnyButtonPressed() {

    uint8_t *buttonsInput = btnmatrix_inGetMutexValue();
    for (int i = 0; i < 16; i++){
        if (buttonsInput[i] == 0){
            return true;
        }
    }
    return false; // No button is pressed
}

/**
 * @brief updates the time value based on the increasing or decreasing state
 * @param timing_of_throw pointer to the time value to be updated
 */
void updateThrowTiming(uint8_t *timing_of_throw)
{
    // Update the timing_of_throwtiming value based on the increasing or decreasing state
    if (increasing) {
        *timing_of_throw += TIME_STEP;
    } else {
        *timing_of_throw -= TIME_STEP;
    }

    // Cap the timing_of_throw value at the defined limits
    if (*timing_of_throw >= TIME_MAX) {
        *timing_of_throw = TIME_MAX; // Cap the timing_of_throw at max
        increasing = false; // Switch to decreasing when max is reached
    }
    else if (*timing_of_throw <= TIME_MIN) {
        *timing_of_throw = TIME_MIN; // Cap the timing_of_throw at min
        increasing = true; // Switch to increasing when min is reached
    }
}

/**
 * @brief handles the non-blocking time update
 * @param timing_of_throw pointer to the time value to be updated
 */
void nonBlockingTimeHandler(uint8_t *timing_of_throw)
{
    static int64_t lastUpdate = 0;
    int64_t currentTime = k_uptime_get(); // Get the current time in milliseconds

    if (currentTime - lastUpdate > TIME_INTERVAL_MS)
    {
        updateThrowTiming(timing_of_throw);
        lastUpdate = currentTime; // Update the last update time
    }
}

/**
 * @brief plays a sound based on the sound number
 * @param soundNr the number of the sound to play
 */
int playSound(int soundNr, int *noteDelayMs)
{
    int (*soundArray)[3];
    int soundLength;
    int delay = (noteDelayMs != NULL) ? *noteDelayMs : 200; // Default delay of 200 ms if not provided

    // Select the appropriate sound array based on soundNr
    switch (soundNr)
    {
        case 0:
            soundArray = soundCatch;
            soundLength = sizeof(soundCatch) / sizeof(soundCatch[0]);
            break;
        case 1:
            soundArray = soundFlee;
            soundLength = sizeof(soundFlee) / sizeof(soundFlee[0]);
            break;
        case 2:
            soundArray = soundCatchFail;
            soundLength = sizeof(soundCatchFail) / sizeof(soundCatchFail[0]);
            break;
        case 3:
            soundArray = soundAppear;
            soundLength = sizeof(soundAppear) / sizeof(soundAppear[0]);
            break;
        case 4:
            soundArray = soundNoBalls;
            soundLength = sizeof(soundNoBalls) / sizeof(soundNoBalls[0]);
            break;
        case 5:
            soundArray = soundStartGame; // Assuming soundStartGame is defined elsewhere
            soundLength = sizeof(soundStartGame) / sizeof(soundStartGame[0]);
            break;
        case 6:
            soundArray = soundEndGame;
            soundLength = sizeof(soundEndGame) / sizeof(soundEndGame[0]);
            break;
        default:
            return -1; // Invalid sound number
    }

    // Play each chord in the sound array
    for (int i = 0; i < soundLength; i++)
    {
        for (int j = 0; j < 3; j++) // Play all 3 frequencies in the chord
        {
            buzzerSetPwm(j, soundArray[i][j]); // Set PWM for each buzzer
        }
        k_msleep(delay); // Delay between chords
    }

    // Turn off all buzzers after playing the sound
    for (int j = 0; j < 3; j++)
    {
        buzzerTurnOff(j);
    }
    return 0;
}

/**
 * @brief checks if you have caught all pokemon and triggers the end game if not.
 * @returns false if you have caught all pokemon or they have fleed
 */
bool checkPokemonLeft()
{
    bool pokemonLeft = true;
    if(pokemonCaught >= POKEMON)
    {
        LOG_INF("All pokemon caught!");
        lcdStringWrite("Je hebt alle pokemon");
        k_msleep(1000);
        lcdStringWrite("gevangen!");

        pokemonLeft = false;
    }
    else if ((pokemonCaught  + pokemonFled) >= POKEMONLOCATIONS)
    {
        LOG_INF("No pokemon left to catch! Caught: %d, Fled: %d", pokemonCaught, pokemonFled);
        lcdStringWrite("Geen pokemon over!");

        pokemonLeft = false;
    }

    return pokemonLeft;
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
        LOG_DBG("Out of pokeballs!");
        lcdStringWrite("Geen ballen over!");
        playSound(4, NULL); // Play out of balls sound
        game_ongoing_pokemon = false;
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
    struct Location *locArray = NULL;
    size_t count = 0;
    int ret = locations_load(0, &locArray, &count, POKEMONLOCATIONS);
    if (ret != 0 || count < POKEMONLOCATIONS) {
        // handle error
        LOG_ERR("Failed to load locations or not enough locations. Error: %d, location count: %zu", ret, count);
        return ret;
    }

    for (int i = 0; i < POKEMONLOCATIONS; i++)
    {
        pokemonLocation[i].lat = locArray[i].x;
        pokemonLocation[i].longi = locArray[i].y;
        pokemonLocation[i].caught = false;
        pokemonLocation[i].fled = false;
        pokemonLocation[i].id = (uint8_t)i;
    }

    for (int i = 0; i < POKEMON; i++)
    {
        pokemonToCatch[i] = rand() % POKEMONLOCATIONS; // Randomly select pokemon locations
    }

    free(locArray);
    return 0;
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
    game_ongoing_pokemon = true;
    catchEvent = false;
    startTime = k_uptime_get(); // Get the current time in milliseconds

    lcdEnable();
    lcdStringWrite("Vind en vang de pokemon!");
    int soundSpeed = 100;
    playSound(START_GAME_SOUND, &soundSpeed);
    k_msleep(3000);

    err = selectLocations();
    if (err != 0) {
        LOG_ERR("Failed to select locations: %d", err);
        return err; // Return error if location selection fails
    }
    return 0;
}

/**
 * @brief pushes matrix to ledmatrix and sets the displayMatrix to 0
 * @returns 0 if everything went as expected
 */
void setMatrix()
{
    ledmatrixSetMutexValue(displayMatrix);

    // clear displayMatrix
    memset(displayMatrix, 0, sizeof(displayMatrix));
}

/**
 * @brief displays the pokemon location on the 16x16 LED
 * @param x center x coordinate of the pokemon on the 16x16 LED
 * @param y center y coordinate of the pokemon on the 16x16 LED
 * @returns 0 if everything went as expected
 * @note Advisory position is to not go below Y 8
 */
int displayPokemon(uint8_t x, uint8_t y)
{
    int8_t offsetX = -3; // The offset is used so that the pokemon doesn't appear half over the edge of the display. This way the pokemon is always fully visible.
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

    float randomValue = (float)rand() / RAND_MAX; // convert to float
    float probability = 1.0 - exp(-0.25 * catchChance + 1); // calculate the probability of catching the pokemon
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

    uint8_t attemptsPermitted = 1 + (rand() / MAX_ATTEMPTS); // random number
    bool caughtPokemon = false;
    // generate pokemon locations on matrix
    uint8_t pokemonLocationX = 3 + rand()%11;
    uint8_t pokemonLocationY = 3 + rand()%3;

    while (catchingMgOngoing == true)
    {
        // update time
        nonBlockingTimeHandler(&time);

        if (time == TIME_MAX)
            lcdStringWrite("Kantel om te richten");
        if (time == TIME_MIN)
            lcdStringWrite("Gooi met de knoppen");

        btnmatrix_outSetMutexValue(matrixLedsOn); // set all LEDs to 1

        // check if the pokemon fled
        if (attemptCounter > attemptsPermitted)
        {
            lcdStringWrite("Pokemon is weg gerend!");
            playSound(1, NULL); // play sound flee
            pokemonFled++;
            k_msleep(1000);
            catchingMgOngoing = false;
        }

        if (checkBallsLeft() == false) {
            catchingMgOngoing = false;
        }

        // get the angle of the box
        gyroscope_get_roll(&roll);
        angle = rollToAngle(roll); // convert roll to angle

        displayPokemon(pokemonLocationX, pokemonLocationY);
        displayAimDirection(angle);
        displayTiming(time);

        setMatrix();


        if (isAnyButtonPressed())
        {
            balls--;
            attemptCounter++;
            btnmatrix_outSetMutexValue(matrixLedsOff); // set all LEDs to 0

            caughtPokemon = checkHit(pokemonLocationX, time, angle);

            if (caughtPokemon == true)
            {
                LOG_INF("Caught the pokemon! Attempt: %d", attemptCounter);
                lcdStringWrite("Je hebt een pokemon gevangen!");
                playSound(0, NULL); // play sound catch
                pokemonCaught++;
                k_msleep(1000);
                catchingMgOngoing = false;
            }
            else
            {
                LOG_INF("Missed the pokemon! Attempt: %d", attemptCounter);
                lcdStringWrite("Mis! Probeer opnieuw!");
                playSound(2, NULL); // catch fail sound
                k_msleep(1000);
            }
        }
    }

    // reset the display
    memset(displayMatrix, 0, sizeof(displayMatrix));
    setMatrix();
    // reset the ledcircle
    uint8_t ledCircle[8] = {0};
    ledcircleSetMutexValue(ledCircle); // set all LEDs to 0

    return attemptCounter;
}

// !NOTE this is basically just the function idle also uses thus it should be moved to a common place instead.
int8_t set_Pokemon_Hint(int pokemonIndex)
{
    const int64_t currLat = getLatitude();		// Get the current latitude
    const int64_t currLon = getLongitude();		// Get the current longitude
    if ( currLat == 0 && currLon == 0) {	// GPS doesn't have lock
        LOG_ERR("GPS does not have a lock!\n");
        return -1; // Could not set hint if GPS is not locked
    }
    int dir = 0;
    int distMeters = 0;
    lcdStringWrite("Volg het licht!");
    distMeters = getDistanceMeters(nanoDegToLdDeg(currLon), nanoDegToLdDeg(currLat), nanoDegToLdDeg(pokemonLocation[pokemonIndex].longi), nanoDegToLdDeg(pokemonLocation[pokemonIndex].lat));
    dir = getAngle(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(pokemonLocation[pokemonIndex].longi), nanoDegToLdDeg(pokemonLocation[pokemonIndex].lat));

    set_led_circle_dir_dist(get_relative_dir(dir), distMeters);	// Set the led circle direction and distance
}

/**
 * @brief checks if a pokemon is nearby
 * @returns true if there a pokemon close enough to trigger catchPokemonEvent false if there are no pokemon nearby
 */
bool pokemonNearby()
{
    // Normally this should be done in one-liners but that is also blocking. For now we do it here for clarity.
    lcdStringWrite("Vind en vang de Pokemon!");
    k_msleep(1000); // Wait for a second to show the message
    lcdStringWrite("Door in het park");
    k_msleep(1000);
    lcdStringWrite("te lopen..");
    k_msleep(1000);

    for (int i = 0; i < (POKEMON); i++)
    {
        // check if the pokemon is within the distance
        int64_t currLat = getLatitude();
        int64_t currLon = getLongitude();
        if (currLat == 0 && currLon == 0) // GPS doesn't have lock
        {
            LOG_ERR("GPS does not have a lock!\n");
            lcdStringWrite("GPS heeft geen  fix..");
            k_msleep(1000);
            lcdStringWrite("GPS heeft geen  fix..");
            k_msleep(1000);
            return false; // No pokemon nearby if GPS is not locked
        }

        int distance = getDistanceMeters(nanoDegToLdDeg(currLat), nanoDegToLdDeg(currLon), nanoDegToLdDeg(pokemonLocation[pokemonToCatch[i]].lat), nanoDegToLdDeg(pokemonLocation[pokemonToCatch[i]].longi));
        if (distance < POKEMON_DISTANCE)
        {
            LOG_INF("Pokemon with ID %d is nearby at a distance of %d meters", pokemonLocation[i].id, distance);
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
    static int lastEventTime = 0;
    lastEventTime = getHour() * 60 + getMinute(); // store the last event time in minutes
    const int currentTime = getHour() * 60 + getMinute(); // get the current time in minutes

    // check if game finish conditions have been met
    if(checkBallsLeft() == false || checkPokemonLeft() == false)
    {
        game_ongoing_pokemon = false;
        return 0;
    }

    if ((currentTime - lastEventTime) > HINT_COOLDOWN / 2)
        ledcircleSetMutexValue(NULL); // clear the ledcircle if the cooldown is over

    // check if there is a pokemon nearby
    if (pokemonNearby())
    {
        LOG_DBG("Pokemon found!");
        playSound(3, NULL); // play pokemon appeared sound
        catchEvent = true;
    }
    else if ((currentTime - lastEventTime) > HINT_COOLDOWN) // if no pokemon is found in the last HINT COOLDOWN give a hint
    {
        // sound to alert the player that a hint is given
        int noteDelayMs = 50; // delay in milliseconds for the sound
        playSound(3, &noteDelayMs); // play pokemon appeared sound really quickly to alert the player

        lastEventTime = currentTime; // update the last event time
        bool existingPokemon = false;
        // hint to the player where a pokemon could be
        while (existingPokemon == false) // This is possibly very slow if a player is unlucky
        {
            int index = pokemonToCatch[rand() % POKEMON]; // random index for the pokemon location
            if (!pokemonLocation[index].caught && !pokemonLocation[index].fled && pokemonCaught < POKEMON) // check if the pokemon was not caught and didn't flee. Also double check that the player hasn't caught all pokemon yet
            {
                set_Pokemon_Hint(index);

                existingPokemon = true; // pokemon exists
            }
        }
    }

    return 0;
}

int calculateScore(int totalAttempts)
{
    uint64_t totalTime = k_uptime_get() - startTime; // calculate the total time in milliseconds
    int score = 0;

    // formula used:
    // Score = ((Caught / totalPokemon) * 400) / (sqrt((0.3*attempts)/(caught/totalpokemon) * ((timeTaken+ 3 * minute)/minute) * 10 - 150 * scoreScaler
    score = ((pokemonCaught / (double)POKEMON) * 400) / (sqrt(0.3 * (totalAttempts / ((double)pokemonCaught / (double)POKEMON))) * ((totalTime + 180000) / 60000) * 10 - 150) * (MAX_SCORE / 1000);

    if (score < 0)
        score = 0;
    if (score > MAX_SCORE)
        score = MAX_SCORE;
    return score;
}

/**
 * @brief starts the catch the pokemon game
 * @returns 0 if everything went as expected
 * @note this function is called from the main thread
 */
int playCatchThePokemon()
{
    int err;
    err = initMg();
    if (err)
    {
        LOG_ERR("Init error: %d", err);
    }

    int score[POKEMON] = {0};
    while (game_ongoing_pokemon)
    {
        // add timer check if players are playing for more than 15 minutes.
        if(!catchEvent)
        {
            err = pokemonGame();
            if (err)
            {
                LOG_ERR("Pokemon game error: %d", err);
                // lcdStringWrite("Pokemon Error");
                // k_msleep(1000);
            }
        }
        else
        {
            catchEvent = false;
            int minigameNr = pokemonCaught + pokemonFled; // the minigame number is the amount of pokemon caught + fled since one of these always happens
            score[minigameNr] = catchingMg();
        }

        // delay
    }

    int totalAttepts = 0;
    for (int i = 0; i < POKEMON; i++)
    {
        totalAttepts += score[i];
    }

    lcdClear();
    return calculateScore(totalAttepts);
}