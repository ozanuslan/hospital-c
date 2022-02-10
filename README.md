# hospital-c

Op Sys assignment 1

# Assignment details

**T.C.**

**DOKUZ EYLUL UNIVERSTY**

**FACULTY OF ENGINEERING**

**DEPARTMENT OF COMPUTER ENGINEERING**

**2021 - 2022 FALL SEMESTER**

**CME 3205 OPERATING SYSTEMS**

**ASSIGNMENT 1 DEU HOSPITAL**

**DUE DATE 23:55 – 09.12.2021**

In this assignment, you are asked to create a simulation of a hospital using POSIX Threads, Mutexes and Semaphores. Your hospital should have the following facilities for patients:

**Registration:** A registration office for patients to register and forwarded to the correct department for treatment. The size of registration office should be stored in a global integer called “REGISTRATION_SIZE”.

**Restroom:** A restroom that are available for patients to use. The size of restroom should be stored in a global integer called “RESTROOM_SIZE”.

**Cafe:** A place where patients can stay while waiting for an operation to finish in hospital. The cafe should contain a number of cashiers that can serve simultaneously and this number should be stored in a global integer called “CAFE_NUMBER”.

**General Practitioner (GP):** A standard doctor that listens to patients complaints and requests additional tests, prescribe medicine or forwards them to a specific department. The total number of GP doctors should be stored in a global integer called “GP_NUMBER”.

**Pharmacy:** A place that patients go to buy medicine that is prescribed by GP doctors. The pharmacy should contain a number of cashiers that can serve simultaneously and this number should be stored in a global integer called “PHARMACY_NUMBER”.

**Blood Lab:** A place were patient’s bloods are taken to be analyzed on the request of GP doctors. The blood lab should contain a number of stations, manned by assistants to collect blood from patients. The number of these stations should be stored in a global integer called “BLOOD_LAB_NUMBER”.

**Surgery:** A place that patients requiring surgery are operated on. They are diagnosed and prescribed a surgery by GP doctors. A surgery is done by one or more surgeons, one or more nurses and inside an operating room (OR). The patient should wait until required number of surgeons, nurses and an OR are available to do the operation. Be careful because this could lead to a dead-lock situation. The numbers of surgeons, nurses and operating theaters should be stored in global integers called “SURGEON_NUMBER”, “NURSE_NUMBER” and “OR_NUMBER”.

Your program should contain the global variables given below. The names and functionality should be the same as it is described. This will allow us to change these variables to see different results. The default values should also be equal to the given values below.

- The number of registration desks that are available. int REGISTRATION_SIZE = 10;
- The number of restrooms that are available. int RESTROOM_SIZE = 10;
- The number of cashiers in cafe that are available. int CAFE_NUMBER = 10;
- The number of General Practitioner that are available. int GP_NUMBER = 10;
- The number of cashiers in pharmacy that are available. int PHARMACY_NUMBER = 10;
- The number of assistants in blood lab that are available. int BLOOD_LAB_NUMBER = 10;
- The number of operating rooms, surgeons and nurses that are available. int OR_NUMBER = 10;

int SURGEON_NUMBER = 30;

int NURSE_NUMBER = 30;

- The maximum number of surgeons and nurses that can do a surgery. A random value is calculated for each operation between 1 and given values to determine the required number of surgeons and nurses.

int SURGEON_LIMIT = 5;

int NURSE_LIMIT = 5;

- The number of patients that will be generated over the course of this program. int PATIENT_NUMBER = 1000;
- The account of hospital where the money acquired from patients are stored. int HOSPITAL_WALLET = 0;
- The time required for each operation in hospital. They are given in milliseconds. But you must use a randomly generated value between 1 and given values below to determine the time that will be required for that operation individually. This will increase the randomness of your simulation.
- The WAIT_TIME is the limit for randomly selected time between 1 and the given value that determines how long a patient will wait before each operation to retry to execute. Assuming the given department is full

int WAIT_TIME = 100;

int REGISTRATION_TIME = 100; int GP_TIME = 200;

int PHARMACY_TIME = 100;

int BLOOD_LAB_TIME = 200; int SURGERY_TIME = 500;

int CAFE_TIME = 100;

int RESTROOM_TIME = 100;

- The money that will be charged to the patients for given operations. The registration and blood lab costs should be static (not randomly decided) but pharmacy and cafe cost should be randomly generated between 1 and given value below to account for different medicine and food that can be purchased.

#The surgery cost should calculated based on number of doctors and nurses that was required to perform it. The formula used for this should be:

- SURGERY_OR_COST + (number of surgeons \* SURGERY_SURGEON_COST) + (number of nurses \* SURGERY_NURSE_COST)

int REGISTRATION_COST = 100;

int PHARMACY_COST = 200; // Calculated randomly between 1 and given value. int BLOOD_LAB_COST = 200;

int SURGERY_OR_COST = 200;

int SURGERY_SURGEON_COST = 100;

int SURGERY_NURSE_COST = 50;

int CAFE_COST = 200; // Calculated randomly between 1 and given value.

- The global increase rate of hunger and restroom needs of patients. It will increase randomly between 1 and given rate below.

int HUNGER_INCREASE_RATE = 10;

int RESTROOM_INCREASE_RATE = 10;

The following patient properties needs to be created:

int Hunger_Meter; // Initialized between 1 and 100 at creation. int Restroom_Meter; // Initialized between 1 and 100 at creation.

The patients should be constructed using POSIX Threads and contain the required properties by the program. The patients should follow the given algorithm below:

The patient is generated with a specific disease that requires either medicine or surgery. The patient goes to registration office and waits for an available desk to register.

After registration, the patient goes to GP’s office and examined.

After examination the patient is send to one of the three main departments.

If the patient is prescribed medicine by GP, they go to pharmacy and buy this drug, waiting for an empty register if there is none.

If the patient is asked to give blood for testing in blood lab, the patient goes to blood lab, waits for an available assistant and gives blood.

After given blood, the patient returns to GP and they are either prescribed medicine if they need it and goes to pharmacy as has been shown above or not if they do not require it.

If the patient requires a surgery, patient waits for an operating room, a randomly selected number or surgeons and nurses to be available and after all of them are acquired, the surgery is performed. After surgery, the patient goes to GP and they are either prescribed medicine and goes to pharmacy as has been shown above or not if they do not require it.

During any of waiting period of previously given operations, if the patient is sufficiently hungry or needs go to a toilet, they can buy food from hospital cafe or go to the hospital restroom. At the end of every waiting period, these hunger and restroom meters are increased by a random value between 1 and given global increase rates.

Your program should print every operation or event that happens inside it. It should be in orderly and informative format. Please create your own console output format individually and according to what you think is the best.

Because of randomization of costs, waiting times and requirement number of surgeons and nurses for surgeries, every execution of your program should be unique. If it is outputting the same results every time, there must be something wrong in your program.

**UPLOAD REQUIREMENTS:**

You are required to upload the C language code file you have written to the SAKAI. You should compile and test it to make sure it works before upload. You can use a IDE during development but your code must work correctly using console complication and execution commands (using “gcc” and “./”). If we cannot correctly compile and execute your code, your grade will be reduced according to grading criteria. You do not need to

upload a complied version of your code, just your C language code, because we cannot prove if it is a original compilation or not. For this reason, uploading a complied file is not necessary nor it is requested.

The file you are required to upload are given below with an explanation and an example.

**(STUDENT_NUMBER)\_(STUDENT_NAME).c (Source code you have written in C language) Example = 2021510123_FATIH_DICLE.c**

Late or no submissions will be graded zero. You can see the basic grading table of this assignment below.

| **CRITERIA**                                         | **GRADE** |
| ---------------------------------------------------- | --------- |
| Correct naming of upload files                       | 10        |
| Correct English variable names and English comments. | 10        |
| Correct Code Quality                                 | 20        |
| Correct Code Execution                               | 30        |
| Correct Console Output                               | 30        |
| TOTAL GRADE                                          | 100       |
| CHEATING OR ANY OTHER FORM OF PLAGIARISM             | –∞        |

If you have any questions or problems regarding this lab paper, you can ask about it in our lab sessions.

If you wish, you can also ask it in class forums or assignment page comments.

However, please do not send emails because that would lead to asking the same questions over and over again.
**GOOD LUCK TO YOU ALL!**
