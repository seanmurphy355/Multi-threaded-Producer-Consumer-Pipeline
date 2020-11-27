//Author: Sean Murphy
//Date: 8/6/2020
//Desription: Takes in a line input from a user via standard input, and prints back 80 character lines input back to the user with a null terminating character.
//This program utilizes a multitheded prodcuer consumer pipline in oder to do these processes.

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// Globals that will be used in this program.
#define SIZE 1000
int MaxLength = 1000;
int done = 1; //flag for termination of program
int doneTwo = 1;
int doneThree = 1;
int prod_idx = 0;// Index where the producer will put the next item
int con_idx = 0;// Index where the consumer will pick up the next item
int num_iterations = 0;// How many items will be produced
char array[1000]; // global char array
char arrayTwo[1000];  // global char array
char arrayThree[1000];  // global char array


//shared Buffer Resource
char bufferOne[SIZE];
char bufferTwo[SIZE];
char bufferThree[SIZE];

// Initialize the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_two = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_three = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variables
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
// Initialize the condition variables
pthread_cond_t fullTwo = PTHREAD_COND_INITIALIZER;
pthread_cond_t emptyTwo = PTHREAD_COND_INITIALIZER;
// Initialize the condition variables
pthread_cond_t fullThree = PTHREAD_COND_INITIALIZER;
pthread_cond_t emptyThree = PTHREAD_COND_INITIALIZER;

//Carries out grabbing the Users input and returns the array dont know if the return is needed consdering the buffer is store the current string.
char *grab_input() {
    char *Storage = NULL;
    //Users prompt to enter input
    memset(array, '\0', 1000);
    while (fgets(array, 1000, stdin)) {
        break;
    }

    //check to see if we are done.
    //printf("%s", array);
    //Check if user entered DONE....so program can terminate.
    if (strcmp(array, "DONE\n") == 0) {
        pthread_mutex_unlock(&mutex);
        //sleep(10);
        done = 0;
        fflush(stdin);
    }

    // Increment the index where the next item will be put. Roll over to the start of the buffer if the item was placed in the last slot in the buffer
    //return our array
    fflush(stdin);
    return array;
}


//Input thread because it is only a prodcuer, it can be blocked when waiting for input.
void *User_Input(void *args) {
    while (done != 0) {
        char *input;
        // Lock the mutex before checking where there is space in the buffer

        input = grab_input();
        pthread_mutex_lock(&mutex);
        //Add error handling for buffer overflow.
        if (input[0]!='\0') {
            strncat(bufferOne, input, 1000);
            //printf("%s <_______ user input concat print ", input);
        }
        //printf("  |%s <-----Thread 1 starting input|\n", bufferOne);
        // Signal to the consumer that the buffer is no longer empty
        // Unlock the mutex
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}


//This function will carry out the line seperation that we must conduct.
char *Sperate_Lines()
{
    int  i;
    //char space = ' ';
    // Increment the index from which the item will be picked up, rolling over to the start of the buffer if currently at the end of the buffer
    memset(arrayTwo, '\0', 1000);
    memcpy(arrayTwo, bufferOne, 1000);
    //clear the null term character and append elements to new array.
    for (i = 0; i < 1000; i++) {
        if (arrayTwo[i] == '\0') {

            break;
        }
        if (arrayTwo[i]== '\n') {
            arrayTwo[i]= ' ';
        }
    }
    //arrayTwo[strcspn(arrayTwo, "\n")] = '\0';
    //strncat(arrayTwo, space, 10000);
    //Clear out our bufferOne and return array back.
    memset(bufferOne, '\0', 1000);
    return arrayTwo;
}

//Carries out the line_seperation feature of this program.
//If buffer 1 is empty, the consumer functionality of this thread has to wait on the condition variable until the thread singals.
//When data is placed in Buffer 1, if Line Separator was waiting on the condition variable, it can consume this data as it will be signaled on the condition variable.
//The producer functionality of Line Separator thread never has to wait on any condition variable, because Buffer 2 is unbounded.
void *Line_Seperator_Thread(void *arg) {

    for (;;) {
        char *input;
        // Lock the mutex before checking where there is space in the buffer
        pthread_mutex_lock(&mutex);
        // Signal to the producer that the buffer has space
        input = Sperate_Lines();
        if (input[0]=='\0' && done == 0) {
            pthread_mutex_unlock(&mutex);
            doneTwo = 0;
            break;
        }
        //unlock mutex 1
        pthread_mutex_unlock(&mutex);
        // mutex2 usage starts here
        pthread_mutex_lock(&mutex_two);
        //call the producer 2 function.
        if (input[0]!='\0') {
            strncat(bufferTwo, input, 1000);
            //printf("  |%s <-----Thread 2 Pord_idx\n", bufferTwo);
            fflush(stdout);
        }
        // Unlock the mutex2
        pthread_mutex_unlock(&mutex_two);


    }
    return NULL;
}

//producer function for adding plus signs 
char *add_Plus() {
    //var usage declaration
    char plus = '+';
    char empty = '\0';
    char swap = '^';
    char *input = bufferTwo;
    //clear out arrayThree make sure we are working with a clean slate.
    memset(arrayThree, '\0', 1000);
    //check if the buffer 2 is null.
    if (bufferTwo[0]!= '\0') {
        memcpy(arrayThree, bufferTwo, 1000);
        int size = strlen(input);
        int i, j;
        //This chunk of code carries out replacing plus signs when neeeded.
        for (i = 0; i < size; i++) {

            //check to see if the plus sign is infront of the current plus sign as well. If so we need to delete 1 plus sign and add a symbol.
            if (arrayThree[i]== plus && arrayThree[i+1]== plus) {
                arrayThree[i+1] = swap;
                for (j = i; j < size; j++) {
                    arrayThree[j] = arrayThree[j+1];
                }
                size--;
                i--;
            }
        }

        //clear out buffer 2 and return arrayThree.
        memset(bufferTwo, '\0', 1000);
    }
    return arrayThree;
}

//Same as Line Separator thread
//Carries out plus sign conversions 
void *PlusThread(void *arg) {
    for (;;) {
        char * input;
        // Lock mutex2
        pthread_mutex_lock(&mutex_two);
        input = add_Plus();

        if (input[0]=='\0' && doneTwo == 0) {
            pthread_mutex_unlock(&mutex_two);
            doneThree = 0;
            break;
        }
        //unlock mutex 2
        pthread_mutex_unlock(&mutex_two);

        // Mutex 3 usage starts here
        pthread_mutex_lock(&mutex_three);
        if (input[0]!='\0') {
            strncat(bufferThree, input, 1000);
            //printf("  |%s <-----Thread 3 Pord_idx |\n", bufferThree);
        }
        // While buffer 2 has something in it signal stopage.
        //call the producer 2 function.
        //printf("%s\n", bufferThree[con_idx]);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex_three);
    }
    return NULL;
}

//checks for a valid size of 80 string to be printed.
void check_Size() {

    char* input = bufferThree;
    int size = strlen(input);
    // 81 becuase we need to factor in added space at the end.
    //check for 80 chars a line and print if it exists.
    if (size >= 80) {
        char print_array[80];
        char *other_storage;

        //copy characters to be printed over to print_array
        memset(print_array, '\0', 80);
        strncpy(print_array, input, 80);
        printf("%s\n", print_array);

        //set other_storage to be 81 characters ahead of our general array then move the other storage array back over to array.
        other_storage = input + 80;
        memmove(input, other_storage, strlen(input)+1);
        size = strlen(input);
    }

}

//Carries output of text back to user 
void *outPutThread(void*arg) {
    for (;;) {
        // Lock mutex3
        pthread_mutex_lock(&mutex_three);
        // Signal to the producer that the buffer has space
        check_Size();

        if (strlen(bufferThree)< 80 && doneThree == 0) {
            pthread_mutex_unlock(&mutex_three);
            break;
        }
        //unlock mutex 3
        pthread_mutex_unlock(&mutex_three);

    }
    return NULL;
}

//Main function call!
int main(int argc, char *argv[])
{
    //check arguments given either none or a text file is considered valid.
    if (argc >= 2)
    {
        printf("please enter correct args(program exited)");
        exit(1);
    }

    // initialize a random seed
    srand(time(0));
    //Create the line thread, sperator thread, plus sign thread, output thread
    pthread_t L, S, P, O;


    //create the User_Input_Thread!
    pthread_create(&L, NULL, User_Input, NULL);
    //create the Line_Seperator_Thread!
    pthread_create(&S, NULL, Line_Seperator_Thread, NULL);
    //create the PlusThread!
    pthread_create(&P, NULL, PlusThread, NULL);
    //create the OutPutThread!
    pthread_create(&O, NULL, outPutThread, NULL);


    // After the thread terminates, the application can clean up resources that were used by each thread.
    pthread_join(L, NULL);
    //sleep(5);
    pthread_join(S, NULL);
    //sleep(5);
    pthread_join(P, NULL);
    //sleep(5);
    pthread_join(O, NULL);
    //exit the program!
    return 0;
}
