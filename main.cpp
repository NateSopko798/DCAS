#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <windows.h>
#define row 20
#define col 20
#define NUM_THREADS    2
#define MAX_STOPS 5
using namespace std;

char grid[row][col];
int threadCount;
pthread_t threads[NUM_THREADS];
pthread_mutex_t printLock;
pthread_mutex_t countLock;
pthread_mutex_t mapLocks[row][col];

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

void customLock(void *drone_data, int tryx,int tryy){
    //method to check how long it has taken to try to get a lock
    //if longer than 3 seconds we can assume it has been in a deadlock scenario
    struct drone_data * my_data = (struct drone_data *) drone_data;
    int trys = 0;
    int i = 3;
    while( i > 0){
        if(pthread_mutex_trylock(&mapLocks[tryy][tryx])!= 0){
            trys++;
        }
        else{
            grid[my_data->currenty][my_data->currentx] = '.';
            grid[tryy][tryx] = ((char)'0' + my_data->thread_id);
            pthread_mutex_unlock (&mapLocks[my_data->currenty][my_data->currentx]);
            my_data->currentx = tryx;
            my_data->currenty = tryy;
            break;
        }
        Sleep(1000);
    }
    if(trys>3){
        if(pthread_mutex_trylock(&mapLocks[tryy+1][tryx+1]) == 0){
            //(+1,+1)
            /*
            pthread_mutex_lock (&mapLocks[(my_data->currenty+1)][(my_data->currentx+1)]);
            grid[my_data->currenty][my_data->currentx] = '.';
            grid[(my_data->currenty+1)][(my_data->currentx+1)] = ((char)'0' + my_data->thread_id);
            pthread_mutex_unlock (&mapLocks[my_data->currenty][my_data->currentx]);
            my_data->currentx = my_data->currentx +1;
            my_data->currenty = my_data->currenty +1;
            */
            return;
        }
        if(pthread_mutex_trylock(&mapLocks[tryy-1][tryx]) == 0){
            //(0,-1)
            return;
        }
        if(pthread_mutex_trylock(&mapLocks[tryy][tryx+1]) == 0){
            //(+1,0)
            return;
        }
        if(pthread_mutex_trylock(&mapLocks[tryy-1][tryx+1]) == 0){
            //(+1,-1)
            return;
        }
        if(pthread_mutex_trylock(&mapLocks[tryy-1][tryx-1]) == 0){
            //(-1,-1)
            return;
        }
        if(pthread_mutex_trylock(&mapLocks[tryy+1][tryx-1]) == 0){
            //(-1,+1)
            return;
        }
        if(pthread_mutex_trylock(&mapLocks[tryy][tryx-1]) == 0){
            //(-1,0)
            return;
        }
        if(pthread_mutex_trylock(&mapLocks[tryy+1][tryx]) == 0){
            //(0,+1)
            return;
        }
    }
}

void makeMove(void *drone_data){
    struct drone_data * my_data = (struct drone_data *) drone_data;
    while(my_data->currentx != my_data->stopx || my_data->currenty != my_data->stopy){ //while both locations are not at the stop
        if(abs((my_data->currentx + 1) - my_data->stopx) < abs((my_data->currentx - 1) - my_data->stopx)){
                //x needs a plus 1 (+1, x )
            if((abs((my_data->currenty + 1) - my_data->stopy) < abs((my_data->currenty - 1) - my_data->stopy))){
                //y needs a plus 1 (+1,+1)
                int tryx = my_data->currentx+1;
                int tryy = my_data->currenty+1;
                customLock(drone_data,tryx,tryy);
            }
            else if((abs((my_data->currenty + 1) - my_data->stopy) == abs((my_data->currenty - 1) - my_data->stopy))){
                //y needs a 0 (+1,0)
                int tryx = my_data->currentx+1;
                int tryy = my_data->currenty;
                customLock(drone_data,tryx,tryy);
            }
            else{
                //y needs -1 (+1,-1)
                int tryx = my_data->currentx+1;
                int tryy = my_data->currenty-1;
                customLock(drone_data,tryx,tryy);
            }
        }
        else if(abs((my_data->currentx + 1) - my_data->stopx) > abs((my_data->currentx - 1) - my_data->stopx)){
            //x needs a minus 1 (-1,x)
            if((abs((my_data->currenty + 1) - my_data->stopy) < abs((my_data->currenty - 1) - my_data->stopy))){
                //y needs a plus 1 (-1,+1)
                int tryx = my_data->currentx-1;
                int tryy = my_data->currenty+1;
                customLock(drone_data,tryx,tryy);
            }
            else if((abs((my_data->currenty + 1) - my_data->stopy) == abs((my_data->currenty - 1) - my_data->stopy))){
                //y needs a 0 (-1,0)
                int tryx = my_data->currentx-1;
                int tryy = my_data->currenty;
                customLock(drone_data,tryx,tryy);
            }
            else{
                //y needs -1 (-1,-1)
                int tryx = my_data->currentx-1;
                int tryy = my_data->currenty-1;
                customLock(drone_data,tryx,tryy);
            }
        }
        else{
            if((abs((my_data->currenty + 1) - my_data->stopy) < abs((my_data->currenty - 1) - my_data->stopy))){
                //y needs a plus 1 (0,+1)
                int tryx = my_data->currentx;
                int tryy = my_data->currenty+1;
                customLock(drone_data,tryx,tryy);
            }
            else{
                //y needs -1 (0,-1)
                int tryx = my_data->currentx;
                int tryy = my_data->currenty-1;
                customLock(drone_data,tryx,tryy);
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
    int i,j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            pthread_mutex_destroy(&mapLocks[i][j]);
        }
    }
}

void *startFlight(void *drone_data){
  struct drone_data * my_data = (struct drone_data *) drone_data;
  while(pthread_mutex_trylock(&mapLocks[my_data->currenty][my_data->currentx]) != 0){
    Sleep(1000);
  }
  grid[my_data->currenty][my_data->currentx] = ((char)'0' + my_data->thread_id);
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
    pthread_mutex_unlock (&mapLocks[my_data->currenty][my_data->currentx]);
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
    int i,j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            pthread_mutex_init(&mapLocks[i][j],NULL);
        }
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
        //drone_data_array[t].currentx = 1;
        //drone_data_array[t].currenty = 0;
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
