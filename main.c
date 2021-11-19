#include <stdio.h>

// The number of registration desks that are available.
int REGISTRATION_SIZE = 10;
// The number of restrooms that are available.
int RESTROOM_SIZE = 10;
// The number of cashiers in cafe that are available.
int CAFE_NUMBER = 10;
// The number of General Practitioner that are available.
int GP_NUMBER = 10;
// The number of cashiers in pharmacy that are available.
int PHARMACY_NUMBER = 10;
// The number of assistants in blood lab that are available.
int BLOOD_LAB_NUMBER = 10;
// The number of operating rooms, surgeons and nurses that are available.
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;

/**
 * The maximum number of surgeons and nurses that can do a surgery. 
 * A random value is calculated for each operation between 1 and given values 
 *  to determine the required number of surgeons and nurses.
*/
int SURGEON_LIMIT = 5;
int NURSE_LIMIT = 5;

// The number of patients that will be generated over the course of this program.
int PATIENT_NUMBER = 1000;
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
int WAIT_TIME = 100;
int REGISTRATION_TIME = 100;
int GP_TIME = 200;
int PHARMACY_TIME = 100;
int BLOOD_LAB_TIME = 200;
int SURGERY_TIME = 500;
int CAFE_TIME = 100;
int RESTROOM_TIME = 100;

/**
 * The money that will be charged to the patients for given operations. 
 * The registrationand blood lab costs should be static (not randomly decided) 
 *  but pharmacy and cafe cost should be randomly generated between 1 and given value below 
 *   to account for differentmedicine and food that can be purchased.
 * The surgery cost should calculated based on number of doctors and nurses that was required to perform it. 
 * The formula used for this should be: 
 * SURGERY_OR_COST + (number of surgeons * SURGERY_SURGEON_COST) + (number of nurses * SURGERY_NURSE_COST)
*/
int REGISTRATION_COST = 100;
int PHARMACY_COST = 200;

// Calculated randomly between 1 and given value.
int BLOOD_LAB_COST = 200;
int SURGERY_OR_COST = 200;
int SURGERY_SURGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200;

/*
 * Calculated randomly between 1 and given value.
 * The global increase rate of hunger and restroom needs of patients. 
 * It will increaserandomly between 1 and given rate below.
*/
int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;

int main()
{

    return 0;
}