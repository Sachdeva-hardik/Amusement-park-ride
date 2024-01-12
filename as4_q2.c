#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_PASSENGERS 100

sem_t mutex_car;
sem_t mutex_passenger;
int total_passengers;
int car_capacity;
int passengers_on_board;

void load() {
    printf("Car is loading passengers...\n");
    // Simulate loading time
    sleep(1);
}

void unload() {
    printf("Car is unloading passengers...\n");
    // Simulate unloading time
    sleep(1);
}

void board(int passenger_number) {
    printf("Passenger %d is boarding the car.\n", passenger_number);
}

void offboard(int passenger_number) {
    printf("Passenger %d is getting off the car.\n", passenger_number);
}

void* car(void* args) {
    while (1) {
        sem_wait(&mutex_car);
        load();
        printf("Car is fully loaded. Ready to go!\n");

        // Print boarding messages for each passenger
        for (int i = 1; i <= car_capacity; ++i) {
            board(i);
        }

        sem_post(&mutex_passenger);

        // Release mutex_car before simulating ride duration
        sem_post(&mutex_car);

        // Simulate ride duration
        printf("Car is running...\n");
        sleep(2);

        sem_wait(&mutex_car);
        unload();

        // Check if the car is empty to print the message
        if (passengers_on_board == 0) {
            printf("Car is empty. Ready for the next ride!\n");
        }

        sem_post(&mutex_passenger);
    }
}



void* passenger(void* args) {
    intptr_t passenger_number = (intptr_t)args;
    while (1) {
        sem_wait(&mutex_passenger);
        if (passenger_number <= total_passengers && passengers_on_board < car_capacity) {
            board(passenger_number);
            passengers_on_board++;
            sem_post(&mutex_passenger);  // Release mutex_passenger before simulating ride duration
            // Simulate ride duration
            sleep(2);
            sem_wait(&mutex_car);
            offboard(passenger_number);
            sem_post(&mutex_car);
        } else {
            sem_post(&mutex_passenger);
            break;
        }
    }
    pthread_exit(NULL);
}


int main() {
    do {
        printf("Enter the total number of passengers: ");
        scanf("%d", &total_passengers);
        printf("Enter the car capacity: ");
        scanf("%d", &car_capacity);

        if (total_passengers > 0 && car_capacity > 0 && car_capacity < total_passengers) {
            break;
        } else {
            printf("Invalid input. Please make sure the number of passengers is greater than car capacity and is valid.\n");
        }
    } while (1);

    pthread_t car_thread;
    pthread_t passenger_threads[MAX_PASSENGERS];
    sem_init(&mutex_car, 0, 1);
    sem_init(&mutex_passenger, 0, 0);

    pthread_create(&car_thread, NULL, car, NULL);

    for (int i = 1; i <= total_passengers; ++i) {
        int* passenger_number = malloc(sizeof(int));
        *passenger_number = i;
        pthread_create(&passenger_threads[i], NULL, passenger, (void*)passenger_number);
    }

    pthread_join(car_thread, NULL);

    for (int i = 1; i <= total_passengers; ++i) {
        pthread_join(passenger_threads[i], NULL);
    }

    sem_destroy(&mutex_car);
    sem_destroy(&mutex_passenger);

    return 0;
}