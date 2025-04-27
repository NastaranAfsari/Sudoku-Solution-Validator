#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

int sodoku[9][9] = {
    {5,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3},
    {4,2,6,8,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};

bool isvalid = true;
//Define mutex lock to protect against isvalid
pthread_mutex_t mutex; 

// Function to check an array of 9 (for row, column, or subgrid)
bool check_array(int arr[]) {
    bool see[10] = {false}; 
    for (int i = 0; i < 9; i++) {
        int number = arr[i];
        if (number < 1 || number > 9 || see[number]) {
            return false;
        }
        see[number] = true;
    }
    return true;
}

// Function to check all rows
void* check_rows(void* arg) {
    for (int i = 0; i < 9; i++) {
        int row[9];
        for (int j = 0; j < 9; j++) {
            row[j] = sodoku[i][j];
        }
        if (!check_array(row)) {
            pthread_mutex_lock(&mutex);
            isvalid = false;
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    return NULL;
}

// Function to check all columns
void* check_columns(void* arg) {
    for (int j = 0; j < 9; j++) {
        int col[9];
        for (int i = 0; i < 9; i++) {
            col[i] = sodoku[i][j];
        }
        if (!check_array(col)) {
            pthread_mutex_lock(&mutex);
            isvalid = false;
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    return NULL;
}

// Function to check a 3x3 subgrid
void* check_subgrid(void* arg) {
    int grid_id = *(int*)arg; 
    int row_start = (grid_id / 3) * 3; 
    int col_start = (grid_id % 3) * 3; 

    int subgrid[9];
    int a = 0;
    for (int i = row_start; i < row_start + 3; i++) {
        for (int j = col_start; j < col_start + 3; j++) {
            subgrid[a++] = sodoku[i][j];
        }
    }

    if (!check_array(subgrid)) {
        pthread_mutex_lock(&mutex);
        isvalid = false;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    // Mutex initialization
    pthread_mutex_init(&mutex, NULL);

    // Definition of threads
    pthread_t row_thread, col_thread, subgrid_threads[9];
    int subgrid_ids[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    // Create threads
    pthread_create(&row_thread, NULL, check_rows, NULL);
    pthread_create(&col_thread, NULL, check_columns, NULL);
    for (int i = 0; i < 9; i++) {
        pthread_create(&subgrid_threads[i], NULL, check_subgrid, &subgrid_ids[i]);
    }

    // Waiting for the threads to finish
    pthread_join(row_thread, NULL);
    pthread_join(col_thread, NULL);
    for (int i = 0; i < 9; i++) {
        pthread_join(subgrid_threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    if (isvalid) {
        printf("Sudoku is valid!\n");
    } else {
        printf("Sudoku is invalid!\n");
    }

    return 0;
}