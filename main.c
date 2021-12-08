#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>

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
#define PATIENT_NUMBER 1000
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
#define ARRIVAL_TIME 100
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
 * The registration and blood lab costs should be static (not randomly decided) 
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
#define MAX_HUNGER 100
#define MAX_RESTROOM 100

// Needs of the patients
typedef enum Need
{
    SURGERY,
    MEDICINE,
    BLOOD_TEST,
    // You can add new needs here
    NEED_CNT // NEED_CNT must always be at the end
} need;

// When you add new needs, you must add their names here
// String array of the names of the needs
char *need_str[] = {
    "surgery",
    "medicine",
    "blood test"};

typedef enum PatientState
{
    WAITING_REGISTRATION,
    WAITING_GP,
    WAITING_BLOOD_LAB,
    WAITING_OR,
    WAITING_PHARMACY,
    RETURNING_FROM_OR,
    RETURNING_FROM_BLOOD_LAB,
    EXITING_HOSPITAL,
    // You can add new states here
    PATIENT_STATE_CNT // PATIENT_STATE_CNT must always be at the end
} patient_state;

// When you add new states, you must add their names here
// String representation of each state
char *patient_state_str[] = {
    "waiting registration",
    "waiting GP",
    "waiting blood lab",
    "waiting OR",
    "waiting pharmacy",
    "returning from OR",
    "returning from blood lab",
    "exiting hospital"};

typedef struct Patients
{
    int pid;
    short hunger_meter;    // Initialized between 1 and 100 at creation.
    short restroom_meter;  // Initialized between 1 and 100 at creation.
    unsigned short need;   // Enum of needs.
    unsigned short status; // Enum for patient state.
} patient;

void init_random(unsigned int seed);                                     // Initializes randomizer with a seed
int myrand(int s, int e);                                                // Returns a random number between s and e
void msleep(int msec);                                                   // Sleeps for given milliseconds
void init_semaphores();                                                  // Initializes all semaphores
patient create_patient(int pid);                                         // Creates a new patient with given pid
void *patient_routine(void *arg);                                        // Main execution block for threads
void log_patient_event(char *event, int pid);                            // Logs an event for a patient
void log_patient_details(patient p);                                     // Logs details of a patient
void log_patient_payment_event(char *event, int payment_total, int pid); // Logs payment event for a patient
void log_hospital_details();                                             // Logs wallet of hospital

patient *PATIENTS; // Array of patients

// Semaphores for all resources that need management
sem_t S_REGISTRATION;    // Semaphore for registration desk
sem_t S_RESTROOM;        // Semaphore for restroom
sem_t S_CAFE;            // Semaphore for cafe
sem_t S_GP;              // Semaphore for General Practitioner
sem_t S_PHARMACY;        // Semaphore for pharmacy
sem_t S_BLOOD_LAB;       // Semaphore for blood lab
sem_t S_OR;              // Semaphore for operating room
sem_t S_HOSPITAL_WALLET; // Semaphore for hospital wallet. This is a binary semaphore!!!
sem_t S_SURGEON_NURSE;   // Semaphore for surgeon and nurse count. This is a binary semaphore!!!

int main()
{
    // You can change the randomizer's seed to any number.
    // Setting the seed to 0 will initialize the randomizer to select current time as seed, thus resulting in expected randomness.
    int seed = 0;

    init_random(seed); // Initialize the random number generator

    printf("---WELCOME TO THE HOSPITAL SIMULATION---\n");
    printf("> Hospital has %d patients\n", PATIENT_NUMBER);
    printf("> Hospital has %d doctors and %d nurses\n", SURGEON_NUMBER, NURSE_NUMBER);
    printf("> Hospital has %d registration desks, %d cafes, restrooms %d, %d pharmacies, %d blood labs, %d operating rooms, and %d general practitioners\n",
           REGISTRATION_SIZE, CAFE_NUMBER, RESTROOM_SIZE, PHARMACY_NUMBER, BLOOD_LAB_NUMBER, OR_NUMBER, GP_NUMBER);
    printf("Starting simulation...\n\n");
    msleep(1000);

    PATIENTS = malloc(sizeof(patient) * PATIENT_NUMBER);

    for (int i = 0; i < PATIENT_NUMBER; i++)
    {
        PATIENTS[i] = create_patient(i);
    }

    init_semaphores(); // Initialize all semaphores
    pthread_t *patient_thread = malloc(sizeof(pthread_t) * PATIENT_NUMBER);

    for (int i = 0; i < PATIENT_NUMBER; i++)
    {
        log_patient_event("Attempting to create thread", i);
        if (pthread_create(patient_thread + i, NULL, patient_routine, &i) == 0)
        {
            msleep(myrand(1, ARRIVAL_TIME));
        }
        else
        {
            log_patient_event("!!!FAILED TO CREATE THREAD!!!", i);
        }
    }

    for (int i = 0; i < PATIENT_NUMBER; i++)
    {
        pthread_join(*(patient_thread + i), NULL);
    }

    printf("\n\n---Simulation Completed---\n");
    printf("Final Hospital Wallet: %d\n", HOSPITAL_WALLET);

    return 0;
}

// Expects patient id as the arg
// Main execution block for threads
void *patient_routine(void *arg)
{
    int pid = *(int *)arg;     // Get the patient id
    patient p = PATIENTS[pid]; // Get patient from array for easier access

    log_patient_event("Entered hospital", pid);
    while (p.status != EXITING_HOSPITAL) // Patient stays in hospital until their operations are done
    {
        log_patient_details(p);
        // If the patient is too hungry, they will go to the cafe and wait
        if (p.hunger_meter >= MAX_HUNGER)
        {
            log_patient_event("Is hungry, waiting in cafe", pid);
            sem_wait(&S_CAFE);            // Wait for cafe to be free
            msleep(myrand(1, CAFE_TIME)); // Wait for a random amount of time
            p.hunger_meter = 0;
            log_patient_event("Got food from cafe, no longer hungry", pid);
            sem_post(&S_CAFE);
        }
        // If the patient needs to go to the restroom, they will wait in line
        else if (p.restroom_meter >= MAX_RESTROOM)
        {
            log_patient_event("Needs to go to the restroom, waiting for available stalls", pid);
            sem_wait(&S_RESTROOM);            // Wait for restroom to be free
            msleep(myrand(1, RESTROOM_TIME)); // Wait for a random amount of time
            p.restroom_meter = 0;
            log_patient_event("Came back from restroom", pid);
            sem_post(&S_RESTROOM);
        }
        // Upon entering the hospital all patients need to register
        else if (p.status == WAITING_REGISTRATION)
        {
            log_patient_event("Checking registration", pid);
            if (sem_trywait(&S_REGISTRATION) == 0) // Try to acquire the registration semaphore
            {
                log_patient_event("Entered registration area", pid);
                msleep(myrand(1, REGISTRATION_TIME)); // Registration takes some time

                // Every person who registers must pay for registration
                log_patient_event("Waiting to pay registration cost", pid);
                sem_wait(&S_HOSPITAL_WALLET);
                HOSPITAL_WALLET += REGISTRATION_COST;
                log_patient_payment_event("Paid registration cost: ", REGISTRATION_COST, pid);
                log_hospital_details();
                sem_post(&S_HOSPITAL_WALLET);

                log_patient_event("Registered successfully", pid);
                p.status = WAITING_GP;

                sem_post(&S_REGISTRATION); // Let go of the registration semaphore
            }
            else
            {
                log_patient_event("Cannot enter registration (full)", pid);
            }
        }
        /**
         * Patient needs to visit GP if they have just left registration and they will visit the GP for the first time
         * or if they have given blood test results and they need to visit the GP again
         * or if they have finished their surgery in the OR and they need to visit the GP again
         */
        else if (p.status == WAITING_GP || p.status == RETURNING_FROM_BLOOD_LAB || p.status == RETURNING_FROM_OR)
        {
            log_patient_event("Checking GP", pid);
            if (sem_trywait(&S_GP) == 0) // Try to acquire the GP semaphore
            {
                if (p.need == MEDICINE)
                {
                    log_patient_event("Entered GP", pid);
                    msleep(myrand(1, GP_TIME)); // GP examination takes some time

                    log_patient_event("Needs medicine, GP forwarded him to Pharmacy", pid);
                    p.status = WAITING_PHARMACY;
                }
                else if (p.status == RETURNING_FROM_OR || p.status == RETURNING_FROM_BLOOD_LAB)
                {
                    if (p.status == RETURNING_FROM_OR)
                        log_patient_event("Returned from OR to GP", pid);

                    else if (p.status == RETURNING_FROM_BLOOD_LAB)
                        log_patient_event("Returned from Blood Lab to GP", pid);

                    msleep(myrand(1, GP_TIME)); // GP examination takes some time

                    if (myrand(0, 1) == 0) // Randomly decide if patient needs medicine or not
                    {
                        log_patient_event("Needs medicine, GP forwarded him to Pharmacy", pid);
                        p.status = WAITING_PHARMACY;
                    }
                    else
                    {
                        log_patient_event("Does not need to get medicine, GP forwarded him to the exit", pid);
                        p.status = EXITING_HOSPITAL;
                    }
                }
                else if (p.need == BLOOD_TEST || p.need == SURGERY)
                {
                    log_patient_event("Entered GP", pid);
                    msleep(myrand(1, GP_TIME)); // GP examination takes some time

                    if (p.need == BLOOD_TEST)
                    {
                        log_patient_event("Needs blood test, GP forwarded him to Blood Lab", pid);
                        p.status = WAITING_BLOOD_LAB;
                    }
                    else if (p.need == SURGERY)
                    {
                        log_patient_event("Needs surgery, GP forwarded him to OR", pid);
                        p.status = WAITING_OR;
                    }
                }
                else // This condition should never be reached, but added just in case
                {
                    log_patient_event("Arrived in GP's room", pid);
                    msleep(myrand(1, GP_TIME)); // GP examination takes some time

                    log_patient_event("Has an unknown need (bug?), GP forwarded him to the exit", pid);
                    p.status = EXITING_HOSPITAL;
                }

                sem_post(&S_GP); // Let go of the GP semaphore
            }
            else
            {
                log_patient_event("Cannot enter GP (full)", pid);
            }
        }
        // Patients will go to pharmacy if they have been forwarded by the GP
        else if (p.status == WAITING_PHARMACY)
        {
            log_patient_event("Checking Pharmacy", pid);
            if (sem_trywait(&S_PHARMACY) == 0) // Try to acquire the Pharmacy semaphore
            {
                log_patient_event("Entered Pharmacy", pid);
                msleep(myrand(1, PHARMACY_TIME)); // Pharmacy examination takes some time
                log_patient_event("Received medicine from Pharmacy", pid);

                // Patient needs to pay for medicine
                log_patient_event("Waiting to pay medicine cost", pid);
                sem_wait(&S_HOSPITAL_WALLET);
                int payment = myrand(1, PHARMACY_COST);
                HOSPITAL_WALLET += payment;
                log_patient_payment_event("Paid medicine cost: ", payment, pid);
                log_hospital_details();
                sem_post(&S_HOSPITAL_WALLET);

                p.status = EXITING_HOSPITAL;

                sem_post(&S_PHARMACY); // Let go of the Pharmacy semaphore
            }
            else
            {
                log_patient_event("Cannot enter Pharmacy (full)", pid);
            }
        }
        else if (p.status == WAITING_BLOOD_LAB)
        {
            log_patient_event("Checking Blood Lab", pid);
            if (sem_trywait(&S_BLOOD_LAB) == 0) // Try to acquire the Blood Lab semaphore
            {
                log_patient_event("Entered Blood Lab", pid);
                msleep(myrand(1, BLOOD_LAB_TIME)); // Blood test takes some time
                log_patient_event("Gave blood sample", pid);

                // Patient needs to pay for blood test
                log_patient_event("Waiting to pay blood test cost", pid);
                sem_wait(&S_HOSPITAL_WALLET); // Wait for hospital wallet to be free
                int payment = myrand(1, BLOOD_LAB_COST);
                HOSPITAL_WALLET += payment;
                log_patient_payment_event("Paid blood test cost: ", payment, pid);
                log_hospital_details();
                sem_post(&S_HOSPITAL_WALLET);

                p.status = RETURNING_FROM_BLOOD_LAB;

                sem_post(&S_BLOOD_LAB); // Let go of the Blood Lab semaphore
            }
            else
            {
                log_patient_event("Cannot enter Blood Lab (full)", pid);
            }
        }
        else if (p.status == WAITING_OR)
        {
            log_patient_event("Checking OR", pid);
            if (sem_trywait(&S_OR) == 0) // Try to acquire the OR semaphore
            {
                log_patient_event("Entered an OR", pid);

                // Determine needed staff for surgery
                int surgeons_needed = myrand(1, SURGEON_LIMIT);
                int nurses_needed = myrand(1, NURSE_LIMIT);

                // Construct strings for needed staff
                char surgeon_msg[20];
                sprintf(surgeon_msg, "Surgeons: %d", surgeons_needed);
                char nurse_msg[20];
                sprintf(nurse_msg, "Nurses: %d", nurses_needed);

                // Construct message for log
                char event_msg[100];
                strcpy(event_msg, "Needs ");
                strcat(event_msg, surgeon_msg);
                strcat(event_msg, " and ");
                strcat(event_msg, nurse_msg);

                log_patient_event(event_msg, pid);

                /**
                 * Once a patient has entered OR it does not make sense for them to 
                 * leave for cafe or restroom while waiting staff since it will break hygiene rules
                 */

                // Try to acquire the needed staff
                bool staff_available = false;
                while (!staff_available)
                {
                    sem_wait(&S_SURGEON_NURSE);
                    if (SURGEON_NUMBER >= surgeons_needed && NURSE_NUMBER >= nurses_needed)
                    {
                        staff_available = true;
                        SURGEON_NUMBER -= surgeons_needed;
                        NURSE_NUMBER -= nurses_needed;
                        log_patient_event("Allocated necessary surgeons and nurses", pid);
                    }
                    else
                    {
                        log_patient_event("Not enough staff is available for surgery, waiting for them to become available", pid);
                    }
                    sem_post(&S_SURGEON_NURSE); // Free the semaphore so other threads can check if staff is available

                    if (!staff_available)
                        msleep(myrand(1, WAIT_TIME)); // Small wait so it does not lock up
                }

                log_patient_event("Started surgery", pid);
                msleep(myrand(1, SURGERY_TIME)); // Surgery takes some time

                // Free the staff
                sem_wait(&S_SURGEON_NURSE);
                SURGEON_NUMBER += surgeons_needed;
                NURSE_NUMBER += nurses_needed;
                sem_post(&S_SURGEON_NURSE);

                log_patient_event("Finished surgery", pid);

                // Patient needs to pay for surgery
                log_patient_event("Waiting to pay surgery cost", pid);
                sem_wait(&S_HOSPITAL_WALLET); // Wait for hospital wallet to be free
                int payment = SURGERY_OR_COST + (surgeons_needed * SURGERY_SURGEON_COST) + (nurses_needed * SURGERY_NURSE_COST);
                HOSPITAL_WALLET += payment;
                log_patient_payment_event("Paid surgery cost: ", payment, pid);
                log_hospital_details();
                sem_post(&S_HOSPITAL_WALLET);

                p.status = RETURNING_FROM_OR;

                sem_post(&S_OR); // Let go of the OR semaphore
            }
            else // Couldn't acquire OR semaphore
            {
                log_patient_event("Cannot enter OR (full)", pid);
            }
        }
        if (p.status == EXITING_HOSPITAL)
        {
            log_patient_event("Exiting hospital", pid);
            break;
        }

        // Increase hunger meter and restroom meter
        p.hunger_meter += myrand(1, HUNGER_INCREASE_RATE);
        p.restroom_meter += myrand(1, RESTROOM_INCREASE_RATE);

        msleep(myrand(0, WAIT_TIME)); // Wait the general wait time
    }
    PATIENTS[pid] = p; // Update patient just in case it is used later
    log_patient_event("Exited hospital", pid);
    pthread_exit(NULL);
}

// IMPORTANT: Initialize the random number generator if you want your randomizer to be seeded.
// If you don't initialize the random number generator, the random number generator will be seeded with the current time.
// The seed is the number that will be used to generate random numbers.
// If you leave it blank, the current time will be used.
// Note: Seeding is really useful for debugging, as it will make your program to generate the same random numbers every time.
//  But it is not necessary for the assignment. Also, multithreaded applications still cause unpredictable behavior even if you seed.
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
    p.pid = pid;
    p.hunger_meter = myrand(1, MAX_HUNGER);
    p.restroom_meter = myrand(1, MAX_RESTROOM);
    p.need = myrand(1, NEED_CNT - 1);
    p.status = WAITING_REGISTRATION;
    return p;
}

void init_semaphores()
{
    int sems[9] = {sem_init(&S_REGISTRATION, 0, REGISTRATION_SIZE),
                   sem_init(&S_RESTROOM, 0, RESTROOM_SIZE),
                   sem_init(&S_CAFE, 0, CAFE_NUMBER),
                   sem_init(&S_GP, 0, GP_NUMBER),
                   sem_init(&S_PHARMACY, 0, PHARMACY_NUMBER),
                   sem_init(&S_BLOOD_LAB, 0, BLOOD_LAB_NUMBER),
                   sem_init(&S_OR, 0, OR_NUMBER),
                   sem_init(&S_HOSPITAL_WALLET, 0, 1), // Binary semaphore
                   sem_init(&S_SURGEON_NURSE, 0, 1)};  // Binary semaphore

    for (int i = 0; i < sizeof(sems) / sizeof(int); i++)
    {
        // Check if semaphore was initialized properly
        // sem_init function returns 0 on success and -1 on failure
        // source: https://man7.org/linux/man-pages/man3/sem_init.3.html
        if (sems[i] == -1)
        {
            printf("\nFailed to create some semaphores...\nExiting program.");
            exit(1);
        }
    }
}

void log_patient_event(char *event, int pid)
{
    printf("[Patient %d]: %s\n", pid, event);
}

void log_patient_payment_event(char *event, int payment_total, int pid)
{
    printf("[Patient %d]: %s%d\n", pid, event, payment_total);
}

void log_patient_details(patient p)
{
    printf("[Patient %d]: Hunger: %d, Restroom: %d, Status: %s\n", p.pid, p.hunger_meter, p.restroom_meter, patient_state_str[p.status]);
}

void log_hospital_details()
{
    printf("[Hospital]: Wallet: %d\n", HOSPITAL_WALLET);
}