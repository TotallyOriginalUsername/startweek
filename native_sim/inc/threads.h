#ifndef THREADS_H
#define THREADS_H

#include <zephyr/kernel.h>

extern uint32_t Startupdelay;

// Total number of threads
#define THREAD_AMOUNT 10

#define printf(...)

// The stack size of all threads
#define STACKSIZE 1024

// Thread priority values (lower value is higher priority)
#define TSTARTBUTTON_PRIORITY 7
#define TGYRO_PRIORITY 7
#define TBTNMATRIX_OUT_PRIORITY 4
#define TBTNMATRIX_IN_PRIORITY 7
#define TSWITCHES_PRIORITY 7
#define TPOTMETER_PRIORITY 7
#define TBUZZERS_PRIORITY 4
#define TLEDMATRIX_PRIORITY 4
#define TLEDCIRCLE_PRIORITY 4
#define TSEVENSEG_PRIORITY 4
#define TABCBTN_PRIORITY 7

// Thread IDs
extern const k_tid_t tstartbutton_id;
extern const k_tid_t tgyro_id;
extern const k_tid_t tbtnmatrix_out_id;
extern const k_tid_t tbtnmatrix_in_id;
extern const k_tid_t tswitches_id;
extern const k_tid_t tpotmeter_id;
extern const k_tid_t tbuzzers_id;
extern const k_tid_t tledmatrix_id;
extern const k_tid_t tledcircle_id;
extern const k_tid_t tsevenseg_id;
extern const k_tid_t tabcbtn_id;

// Define the thread functions
// Input thread functions
void tstartbutton(void);
void tgyro(void);
void tbtnmatrix_in(void);
void tswitches(void);
void tpotmeter(void);
void tabcbtn(void);
// Output thread functions
void tbtnmatrix_out(void);
void tbuzzers(void);
void tledmatrix(void);
void tledcircle(void);
void tsevenseg(void);

// Functions for enabling and disabling threads
void enableThreads(char **, unsigned);
void disableThreads(char **, unsigned);
void setThread(char *, bool);

void disableAllThreads();

//mutex protect functions
uint8_t startbuttonGetMutexValue();
uint8_t* btnmatrix_inGetMutexValue();
uint8_t* switchesGetMutexValue();
int32_t potmeterGetMutexValue();
uint8_t* abcbtnGetMutexValue();
void btnmatrix_outSetMutexValue(uint8_t data[4]);
void buzzersSetMutexValue(uint8_t data[3]);
void ledmatrixSetMutexValue(uint16_t data[16]);
void ledcircleSetMutexValue(uint8_t data[8]);
void sevensegSetMutexValue(char input[4],uint8_t dpPosition);


#endif //THREADS_H