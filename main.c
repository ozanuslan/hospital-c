#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

// Needs of the patients
typedef enum Need
{
    SURGERY,
    MEDICINE,
    BLOOD_TEST,
    // You can add new needs here
    NEED_CNT // NEED_CNT must always be at the end
} need;

char *need_str[] = {
    "surgery",
    "medicine",
    "blood test"};

typedef enum PatientState
{
    WAITING_REGISTRATION,
    WAITING_GP,
    // You can add new states here
    PATIENT_STATE_CNT // PATIENT_STATE_CNT must always be at the end
} patient_state;

// The number of registration desks that are available.
#define REGISTRATION_SIZE 10
// The number of restrooms that are available.
#define RESTROOM_SIZE 10
// The number of cashiers in cafe that are available.
#define CAFE_NUMBER 10
// The number of General Practitioner that are available.
#define GP_NUMBER 10
// The number of cashiers in pharmacy that are available.
#define PHARMACY_NUMBER 10
// The number of assistants in blood lab that are available.
#define BLOOD_LAB_NUMBER 10
// The number of operating rooms, surgeons and nurses that are available.
#define OR_NUMBER 10
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;

/**
 * The maximum number of surgeons and nurses that can do a surgery. 
 * A random value is calculated for each operation between 1 and given values 
 *  to determine the required number of surgeons and nurses.
*/
#define SURGEON_LIMIT 5
#define NURSE_LIMIT 5

// The number of patients that will be generated over the course of this program.
#define PATIENT_NUMBER 100
// The account of hospital where the money acquired from patients are stored.
int HOSPITAL_WALLET = 0;

/*  
 * The time required for each operation in hospital. 
 * They are given in milliseconds. 
 * But you must use a randomly generated value between 1 and given values below 
 *  to determinethe time that will be required for that operation individually. 
 * This will increase the randomness of your simulation.
 * 
 * The WAIT_TIME is the limit for randomly selected time between 1 and the given value 
 *  that determines how long a patient will wait before each operation to retry to execute. 
 * Assuming the given department is full
*/
#define ARRIVAL_TIME 40
#define WAIT_TIME 100
#define REGISTRATION_TIME 100
#define GP_TIME 200
#define PHARMACY_TIME 100
#define BLOOD_LAB_TIME 200
#define SURGERY_TIME 500
#define CAFE_TIME 100
#define RESTROOM_TIME 100

/**
 * The money that will be charged to the patients for given operations. 
 * The registrationand blood lab costs should be static (not randomly decided) 
 *  but pharmacy and cafe cost should be randomly generated between 1 and given value below 
 *   to account for differentmedicine and food that can be purchased.
 * The surgery cost should calculated based on number of doctors and nurses that was required to perform it. 
 * The formula used for this should be: 
 * SURGERY_OR_COST + (number of surgeons * SURGERY_SURGEON_COST) + (number of nurses * SURGERY_NURSE_COST)
*/
#define REGISTRATION_COST 100
#define PHARMACY_COST 200

// Calculated randomly between 1 and given value.
#define BLOOD_LAB_COST 200
#define SURGERY_OR_COST 200
#define SURGERY_SURGEON_COST 100
#define SURGERY_NURSE_COST 50
#define CAFE_COST 200

/*
 * Calculated randomly between 1 and given value.
 * The global increase rate of hunger and restroom needs of patients. 
 * It will increase randomly between 1 and given rate below.
*/
#define HUNGER_INCREASE_RATE 10
#define RESTROOM_INCREASE_RATE 10

typedef struct Patients
{
    int patient_id;
    short hunger_meter;   // Initialized between 1 and 100 at creation.
    short restroom_meter; // Initialized between 1 and 100 at creation.
    unsigned short need;
    unsigned short status;

} patient;

void init_random(unsigned int seed);
int myrand(int s, int e);
void msleep(int msec);
void init_semaphores();
patient create_patient(int pid);
void *patient_job(void *arg);
void log_event(char *event, int pid);
void log_patient_details(patient p);

patient *PATIENTS;

// Semaphores for all resources that need management
sem_t S_REGISTRATION;
sem_t S_RESTROOM;
sem_t S_CAFE;
sem_t S_GP;
sem_t S_PHARMACY;
sem_t S_BLOOD_LAB;
sem_t S_OR;
sem_t S_HOSPITAL_WALLET; // This is a binary semaphore
sem_t S_SURGEON;         // This is a binary semaphore
sem_t S_NURSE;           // This is a binary semaphore

int main()
{
    // You can change the randomizer's seed to any number. But I chose to go with my student ID.
    // Setting the seed to 0 will initialize the randomizer to select current time as seed, thus resulting in expected randomness.
    int seed = 2019510078;

    init_random(seed); // Initialize the random number generator

    PATIENTS = malloc(sizeof(patient) * PATIENT_NUMBER);

    for (int i = 0; i < PATIENT_NUMBER; i++)
    {
        PATIENTS[i] = create_patient(i);
    }

    init_semaphores(); // Initialize all semaphores
    pthread_t *patient_thread = malloc(sizeof(pthread_t) * PATIENT_NUMBER);

    for (int i = 0; i < PATIENT_NUMBER; i++)
    {
        log_patient_details(PATIENTS[i]);
        log_event("Attempting to create thread", i);
        if (pthread_create(patient_thread + i, NULL, patient_job, &i) == 0)
        {
            msleep(myrand(0, ARRIVAL_TIME)); // <- TODO: Uncomment this
        }
        else
        {
            log_event("Failed to create", i);
        }
    }

    for (int i = 0; i < PATIENT_NUMBER; i++)
    {
        pthread_join(*(patient_thread + i), NULL);
    }

    return 0;
}

// Expects patient id as the arg
void *patient_job(void *arg)
{
    int pid = *(int *)arg;
    log_event("Entered hospital", pid);
    while (PATIENTS[pid].status == WAITING_REGISTRATION)
    {
        if (PATIENTS[pid].status == WAITING_REGISTRATION)
        {
            log_event("Checking registration", pid);
            if (sem_trywait(&S_REGISTRATION) == 0) // Try to acquire the registration semaphore
            {
                log_event("Entered registration area", pid);

                // Registration takes some time
                msleep(myrand(0, REGISTRATION_TIME));

                // Every person who registers must pay for registration
                log_event("Waiting to pay registration cost", pid);
                sem_wait(&S_HOSPITAL_WALLET);
                HOSPITAL_WALLET += REGISTRATION_COST;
                sem_post(&S_HOSPITAL_WALLET);

                log_event("Registered successfully", pid);
                PATIENTS[pid].status = WAITING_GP;
                // Let go of the registration semaphore
                sem_post(&S_REGISTRATION);
            }
            else
            {
                log_event("Cannot enter registration (full)", pid);
            }
        }
        else if (PATIENTS[pid].status == WAITING_GP)
        {
            log_event("Checking GP", pid);
            if (sem_trywait(&S_GP) == 0) // Try to acquire the GP semaphore
            {
                // GP takes some time
                msleep(myrand(0, GP_TIME));
                // Let go of the GP semaphore
                sem_post(&S_GP);
            }
            else
            {
                log_event("Cannot enter GP (full)", pid);
            }
        }
        msleep(myrand(0, WAIT_TIME));
    }
    pthread_exit(NULL);
}

// IMPORTANT: Initialize the random number generator if you want your randomizer to be seeded.
// If you don't initialize the random number generator, the random number generator will be seeded with the current time.
// The seed is the number that will be used to generate random numbers.
// If you leave it blank, the current time will be used.
// Note: Seeding is really useful for debugging, as it will make your program to generate the same random numbers every time.
void init_random(unsigned int seed)
{
    if (seed == 0)
    {
        srand(time(NULL));
    }
    else
    {
        srand(seed);
    }
}

int myrand(int s, int e)
{
    return rand() % (e - s + 1) + s;
}

void msleep(int msec)
{
    usleep(msec * 1000);
}

patient create_patient(int pid)
{
    patient p;
    p.patient_id = pid;
    p.hunger_meter = myrand(1, 100);
    p.restroom_meter = myrand(1, 100);
    p.need = myrand(0, NEED_CNT - 1);
    p.status = WAITING_REGISTRATION;
    return p;
}

void init_semaphores()
{
    int sems[10] = {sem_init(&S_REGISTRATION, 0, REGISTRATION_SIZE),
                    sem_init(&S_RESTROOM, 0, RESTROOM_SIZE),
                    sem_init(&S_CAFE, 0, CAFE_NUMBER),
                    sem_init(&S_GP, 0, GP_NUMBER),
                    sem_init(&S_PHARMACY, 0, PHARMACY_NUMBER),
                    sem_init(&S_BLOOD_LAB, 0, BLOOD_LAB_NUMBER),
                    sem_init(&S_OR, 0, OR_NUMBER),
                    sem_init(&S_HOSPITAL_WALLET, 0, 1), // Binary semaphore
                    sem_init(&S_SURGEON, 0, 1),         // Binary semaphore
                    sem_init(&S_NURSE, 0, 1)};          // Binary semaphore

    for (int i = 0; i < sizeof(sems) / sizeof(int); i++)
    {
        if (sems[i] == -1)
        {
            printf("\nFailed to create some semaphores...\nExiting program.");
            exit(1);
        }
    }
}

void log_event(char *event, int pid)
{
    printf("[Patient %d]: %s\n", pid, event);
}

void log_patient_details(patient p)
{
    printf("[Patient %d]: Hunger: %d, Restroom: %d, Need: %s\n", p.patient_id, p.hunger_meter, p.restroom_meter, need_str[p.need]);
}