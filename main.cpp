#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <windows.h>
#define row 10
#define col 10
#define NUM_THREADS    20
#define MAX_STOPS 5
using namespace std;

char grid[row][col];
int threadCount;
pthread_t threads[NUM_THREADS];
pthread_mutex_t printLock;
pthread_mutex_t countLock;
pthread_mutex_t mapLocks[row *col];

struct drone_data
{
  int thread_id;
  int currentx;
  int currenty;
  int stopx;
  int stopy;
};

void updateWorld(){
    pthread_mutex_lock (&printLock);
    system("cls");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    Sleep(500);
    pthread_mutex_unlock (&printLock);
}

struct drone_data drone_data_array[NUM_THREADS];

void makeMove(void *drone_data){
    struct drone_data * my_data = (struct drone_data *) drone_data;
    while(my_data->currentx != my_data->stopx || my_data->currenty != my_data->stopy){ //while both locations are not at the stop
        int i = 0;
        //--------------------------------------------------------------------------------XXXXXXXXX
        if(abs((my_data->currentx + 1) - my_data->stopx) < abs((my_data->currentx - 1) - my_data->stopx)){//if +1 is better than -1
             if(abs((my_data->currentx + 1) - my_data->stopx) != abs((my_data->currentx - 1) - my_data->stopx)){//they arent equal
                pthread_mutex_lock (&printLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty)][(my_data->currentx+1)] = ((char)'0' + my_data->thread_id);
                my_data->currentx = my_data->currentx +1;
                pthread_mutex_unlock (&printLock);
             }

        }
        else{//if -1 is better than +1
            if(abs((my_data->currentx + 1) - my_data->stopx) != abs((my_data->currentx - 1) - my_data->stopx)){//they arent equal
                pthread_mutex_lock (&printLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty)][(my_data->currentx-1)] = ((char)'0' + my_data->thread_id);
                my_data->currentx = my_data->currentx -1;
                pthread_mutex_unlock (&printLock);
             }
        }
        //-------------------------------------------------------------------------------YYYYYYYYY
        if(abs((my_data->currenty + 1) - my_data->stopy) < abs((my_data->currenty - 1) - my_data->stopy)){//if +1 is better than -1
             if(abs((my_data->currenty + 1) - my_data->stopy) != abs((my_data->currenty - 1) - my_data->stopy)){//they arent equal
                pthread_mutex_lock (&printLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty+1)][(my_data->currentx)] = ((char)'0' + my_data->thread_id);
                my_data->currenty = my_data->currenty +1;
                pthread_mutex_unlock (&printLock);
             }
        }
        else{//if -1 is better than +1
            if(abs((my_data->currenty + 1) - my_data->stopy) != abs((my_data->currenty - 1) - my_data->stopy)){//they arent equal
                pthread_mutex_lock (&printLock);
                grid[my_data->currenty][my_data->currentx] = '.';
                grid[(my_data->currenty-1)][(my_data->currentx)] = ((char)'0' + my_data->thread_id);
                my_data->currenty = my_data->currenty -1;
                pthread_mutex_unlock (&printLock);
             }
        }
        updateWorld();
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

void destroyLocks(){
    int maxi = row * col;
    for (int i = 0; i < maxi; i++){
        pthread_mutex_destroy(&mapLocks[i]);
    }
}

void *startFlight(void *drone_data){
  struct drone_data * my_data = (struct drone_data *) drone_data;
  pthread_mutex_lock (&printLock);
  grid[my_data->currenty][my_data->currentx] = ((char)'0' + my_data->thread_id);
  pthread_mutex_unlock (&printLock);
  updateWorld();
  makeMove(drone_data);
  pthread_mutex_lock (&countLock);
  threadCount++;
  if(threadCount == NUM_THREADS){
    //if this is the last thread
    pthread_mutex_unlock (&countLock);
    pthread_mutex_destroy(&printLock);
    pthread_mutex_destroy(&countLock);
    destroyLocks();
  }
  else{
    //it isnt the last thread
    pthread_mutex_unlock (&countLock);
  }
  pthread_exit((void*) 0);
}

void setupWorld(int rows, int columns){
    int i,j;
    pthread_mutex_lock (&printLock);
    for (i = 0; i < rows; i++){
        for (j = 0; j < columns; j++){
            grid[i][j] = '.';
        }
    }
    pthread_mutex_unlock (&printLock);
}

void setupLocks(){
    int maxi = row * col;
    for (int i = 0; i < maxi; i++){
        pthread_mutex_init(&mapLocks[i], NULL);
    }
}

int main()
{
    threadCount = 0;
    pthread_attr_t attr;
    pthread_mutex_init(&printLock, NULL);
    pthread_mutex_init(&countLock, NULL);
    setupWorld(row,col);
    setupLocks();
    int rc;
    long t;
    pthread_mutex_lock (&printLock);
    printWorld();
    pthread_mutex_unlock (&printLock);
    Sleep(5000);
    for(t=0; t<NUM_THREADS; t++){
        Sleep(1000);
        drone_data_array[t].thread_id = t;
        drone_data_array[t].currentx = rand() % col;
        drone_data_array[t].currenty = rand() % row;
        drone_data_array[t].stopx = rand() % col;
        drone_data_array[t].stopy = rand() % row;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        rc = pthread_create(&threads[t], NULL, startFlight, (void *)&drone_data_array[t]);
        pthread_attr_destroy(&attr);
    }
    pthread_exit(NULL);
    return 0;
}
