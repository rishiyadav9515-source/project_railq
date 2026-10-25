#include <stdio.h>
#include <string.h>

#define COUNTERS 3
#define CAPACITY 100
#define NAME_LENGTH 50
#define SERVICE_MINUTES 4

typedef struct {
    int token;
    char name[NAME_LENGTH];
} Passenger;

typedef struct {
    Passenger data[CAPACITY];
    int front;
    int rear;
    int size;
} CircularQueue;

void initializeQueue(CircularQueue *q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

int isEmpty(const CircularQueue *q) {
    return q->size == 0;
}

int isFull(const CircularQueue *q) {
    return q->size == CAPACITY;
}

int enqueue(CircularQueue *q, Passenger passenger) {
    if (isFull(q)) {
        return 0;
    }

    q->rear = (q->rear + 1) % CAPACITY;
    q->data[q->rear] = passenger;
    q->size++;
    return 1;
}

int dequeue(CircularQueue *q, Passenger *passenger) {
    if (isEmpty(q)) {
        return 0;
    }

    *passenger = q->data[q->front];
    q->front = (q->front + 1) % CAPACITY;
    q->size--;
    return 1;
}

int totalInLine(const CircularQueue *q, int isServing) {
    return q->size + isServing;
}

int findShortestCounter(CircularQueue queues[], int serving[]) {
    int shortest = 0;

    for (int i = 1; i < COUNTERS; i++) {
        if (totalInLine(&queues[i], serving[i]) <
            totalInLine(&queues[shortest], serving[shortest])) {
            shortest = i;
        }
    }

    return shortest;
}

void issueToken(CircularQueue queues[], int serving[],
                int *nextToken, int *completed) {
    Passenger passenger;
    int counter;

    if (*nextToken > CAPACITY * COUNTERS) {
        printf("Token limit reached for this demonstration.\n");
        return;
    }

    printf("Passenger name: ");
    scanf(" %49[^\n]", passenger.name);

    passenger.token = (*nextToken)++;
    counter = findShortestCounter(queues, serving);

    if (enqueue(&queues[counter], passenger)) {
        int peopleAhead = queues[counter].size - 1 + serving[counter];
        int wait = peopleAhead * SERVICE_MINUTES;

        printf("\nToken issued: RQ-%03d\n", passenger.token);
        printf("Assigned to Counter %d\n", counter + 1);
        printf("Estimated wait: %d minutes\n\n", wait);
    } else {
        printf("Counter %d queue is full. No token issued.\n", counter + 1);
        (*nextToken)--;
    }

    (void)completed;
}

void callNext(CircularQueue queues[], Passenger serving[],
              int isServing[], int counter) {
    if (isServing[counter]) {
        printf("Counter %d is already serving RQ-%03d.\n",
               counter + 1, serving[counter].token);
        return;
    }

    if (dequeue(&queues[counter], &serving[counter])) {
        isServing[counter] = 1;
        printf("Counter %d is now serving RQ-%03d (%s).\n",
               counter + 1, serving[counter].token, serving[counter].name);
    } else {
        printf("No passengers are waiting at Counter %d.\n", counter + 1);
    }
}

void completeTicket(CircularQueue queues[], Passenger serving[],
                    int isServing[], int *completed, int counter) {
    if (!isServing[counter]) {
        printf("Counter %d is not currently serving anyone.\n", counter + 1);
        return;
    }

    printf("Completed ticket for RQ-%03d (%s) at Counter %d.\n",
           serving[counter].token, serving[counter].name, counter + 1);

    isServing[counter] = 0;
    (*completed)++;

    /* Automatically call the next passenger in FIFO order. */
    callNext(queues, serving, isServing, counter);
}

void displayQueues(CircularQueue queues[], Passenger serving[],
                   int isServing[]) {
    for (int i = 0; i < COUNTERS; i++) {
        printf("\nCounter %d: ", i + 1);

        if (isServing[i]) {
            printf("Serving RQ-%03d (%s) | ",
                   serving[i].token, serving[i].name);
        } else {
            printf("Available | ");
        }

        if (isEmpty(&queues[i])) {
            printf("No one waiting");
        } else {
            printf("Waiting: ");
            for (int j = 0; j < queues[i].size; j++) {
                int index = (queues[i].front + j) % CAPACITY;
                printf("RQ-%03d", queues[i].data[index].token);
                if (j < queues[i].size - 1) {
                    printf(" -> ");
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

void checkStatus(CircularQueue queues[], Passenger serving[],
                 int isServing[]) {
    int token;
    int found = 0;

    printf("Enter token number (for example, 1 for RQ-001): ");
    scanf("%d", &token);

    for (int i = 0; i < COUNTERS; i++) {
        if (isServing[i] && serving[i].token == token) {
            printf("RQ-%03d is now being served at Counter %d.\n",
                   token, i + 1);
            return;
        }

        for (int j = 0; j < queues[i].size; j++) {
            int index = (queues[i].front + j) % CAPACITY;
            if (queues[i].data[index].token == token) {
                int position = j + 1;
                int peopleAhead = j + isServing[i];
                printf("RQ-%03d is number %d in line at Counter %d.\n",
                       token, position, i + 1);
                printf("Estimated wait: %d minutes.\n",
                       peopleAhead * SERVICE_MINUTES);
                found = 1;
                break;
            }
        }
        if (found) {
            return;
        }
    }

    printf("Token not found in the active queues.\n");
}

int main(void) {
    CircularQueue queues[COUNTERS];
    Passenger serving[COUNTERS];
    int isServing[COUNTERS] = {0};
    int nextToken = 1;
    int completed = 0;
    int choice;
    int counter;

    for (int i = 0; i < COUNTERS; i++) {
        initializeQueue(&queues[i]);
    }

    do {
        printf("\n=== Smart Railway Ticket Counter ===\n");
        printf("1. Issue passenger token\n");
        printf("2. Call next passenger\n");
        printf("3. Complete current ticket\n");
        printf("4. Display all queues\n");
        printf("5. Check token status\n");
        printf("6. Show completed ticket count\n");
        printf("0. Exit\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                issueToken(queues, isServing, &nextToken, &completed);
                break;

            case 2:
                printf("Enter counter number (1-%d): ", COUNTERS);
                scanf("%d", &counter);
                if (counter >= 1 && counter <= COUNTERS) {
                    callNext(queues, serving, isServing, counter - 1);
                } else {
                    printf("Invalid counter number.\n");
                }
                break;

            case 3:
                printf("Enter counter number (1-%d): ", COUNTERS);
                scanf("%d", &counter);
                if (counter >= 1 && counter <= COUNTERS) {
                    completeTicket(queues, serving, isServing,
                                   &completed, counter - 1);
                } else {
                    printf("Invalid counter number.\n");
                }
                break;

            case 4:
                displayQueues(queues, serving, isServing);
                break;

            case 5:
                checkStatus(queues, serving, isServing);
                break;

            case 6:
                printf("Completed tickets: %d\n", completed);
                break;

            case 0:
                printf("Exiting the queue system.\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}
