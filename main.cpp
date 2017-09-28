#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <windows.h>
#define row 40
#define col 40
#define NUM_THREADS    2
#define MAX_STOPS 5
using namespace std;

char grid[row][col];
pthread_t threads[NUM_THREADS];
pthread_mutex_t mapLock;

struct drone_data
{
  int thread_id;
  int currentx;
  int currenty;
  int stopx;
  int stopy;
};

void updateWorld(){
    system("cls");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

struct drone_data drone_data_array[NUM_THREADS];

void makeMove(void *drone_data){
    struct drone_data * my_data = (struct drone_data *) drone_data;
    while(my_data->currentx != my_data->stopx || my_data->currenty != my_data->stopy){ //while both locations are not at the stop
        Sleep(1000);
        //--------------------------------------------------------------------------------XXXXXXXXX
        if(abs((my_data->currentx + 1) - my_data->stopx) < abs((my_data->currentx - 1) - my_data->stopx)){//if +1 is better than -1
             if(abs((my_data->currentx + 1) - my_data->stopx) != abs((my_data->currentx - 1) - my_data->stopx)){//they arent equal
                pthread_mutex_lock (&mapLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty)][(my_data->currentx+1)] = 'x';
                my_data->currentx = my_data->currentx +1;
                updateWorld();
                pthread_mutex_unlock (&mapLock);
             }

        }
        else{//if -1 is better than +1
            if(abs((my_data->currentx + 1) - my_data->stopx) != abs((my_data->currentx - 1) - my_data->stopx)){//they arent equal
                pthread_mutex_lock (&mapLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty)][(my_data->currentx-1)] = 'x';
                my_data->currentx = my_data->currentx -1;
                updateWorld();
                pthread_mutex_unlock (&mapLock);
             }
        }
        //-------------------------------------------------------------------------------YYYYYYYYY
        if(abs((my_data->currenty + 1) - my_data->stopy) < abs((my_data->currenty - 1) - my_data->stopy)){//if +1 is better than -1
             if(abs((my_data->currenty + 1) - my_data->stopy) != abs((my_data->currenty - 1) - my_data->stopy)){//they arent equal
                pthread_mutex_lock (&mapLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty+1)][(my_data->currentx)] = 'x';
                my_data->currenty = my_data->currenty +1;
                updateWorld();
                pthread_mutex_unlock (&mapLock);
             }
        }
        else{//if -1 is better than +1
            if(abs((my_data->currenty + 1) - my_data->stopy) != abs((my_data->currenty - 1) - my_data->stopy)){//they arent equal
                pthread_mutex_lock (&mapLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty-1)][(my_data->currentx)] = 'x';
                my_data->currenty = my_data->currenty -1;
                updateWorld();
                pthread_mutex_unlock (&mapLock);
             }
        }
        //------------------------------------------------------------------------------------------

    }
}

void printWorld(){
    int i,j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

void *startFlight(void *drone_data){
  struct drone_data * my_data = (struct drone_data *) drone_data;
  pthread_mutex_lock (&mapLock);
  grid[my_data->currenty][my_data->currentx] = 'x';
  updateWorld();
  pthread_mutex_unlock (&mapLock);
  makeMove(drone_data);
  pthread_exit((void*) 0);
}

void setupWorld(int rows, int columns){
    int i,j;
    pthread_mutex_lock (&mapLock);
    for (i = 0; i < rows; i++){
        for (j = 0; j < columns; j++){
            grid[i][j] = '.';
        }
    }
    pthread_mutex_unlock (&mapLock);
}

int main()
{
    pthread_attr_t attr;
    pthread_mutex_init(&mapLock, NULL);
    setupWorld(row,col);
    int rc;
    long t;
    pthread_mutex_lock (&mapLock);
    printWorld();
    pthread_mutex_unlock (&mapLock);
    Sleep(5000);
    //sleep(5000);
    for(t=0; t<NUM_THREADS; t++){
        if(t == 0){
            drone_data_array[t].thread_id = t;
            drone_data_array[t].currentx = 20;
            drone_data_array[t].currenty = 4;
            drone_data_array[t].stopx = 39;
            drone_data_array[t].stopy = 39;
        }
        else{
            drone_data_array[t].thread_id = t;
            drone_data_array[t].currentx = 35;
            drone_data_array[t].currenty = 20;
            drone_data_array[t].stopx = 0;
            drone_data_array[t].stopy = 39;
        }
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        rc = pthread_create(&threads[t], NULL, startFlight, (void *)&drone_data_array[t]);
        if (rc){
          //printf("ERROR; return code from pthread_create() is %d\n", rc);
          return(-1);
        }
        pthread_attr_destroy(&attr);
    }
    pthread_mutex_destroy(&mapLock);
    pthread_exit(NULL);

    return 0;
}
