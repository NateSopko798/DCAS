#include <iostream>
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
  int startx;
  int starty;
  int stopx;
  int stopy;
};

struct drone_data drone_data_array[NUM_THREADS];

void updateWorld(){
    system("cls");
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printWorld(){
    int i,j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void *startFlight(void *drone_data){
  struct drone_data * my_data = (struct drone_data *) drone_data;
  pthread_mutex_lock (&mapLock);
  grid[my_data->starty][my_data->startx] = 'x';
  updateWorld();
  pthread_mutex_unlock (&mapLock);
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
        Sleep(5000);
        //sleep(5000);
        if(t == 0){
            drone_data_array[t].thread_id = t;
            drone_data_array[t].startx = 0;
            drone_data_array[t].starty = 0;
            drone_data_array[t].stopx = 39;
            drone_data_array[t].stopy = 39;
        }
        else{
            drone_data_array[t].thread_id = t;
            drone_data_array[t].startx = 39;
            drone_data_array[t].starty = 0;
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
