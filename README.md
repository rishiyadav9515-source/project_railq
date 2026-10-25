# Smart Railway Ticket Counter Queue

A console based C college project demonstrating **FIFO queues**, **circular queues**, and **multiple counters** in a railway ticketing scenario.

## Features

- Generates sequential passenger tokens such as `RQ-001`.
- Maintains one circular FIFO queue per ticket counter.
- Assigns each new passenger to the counter with the shortest line.
- Allows counter staff to call the next passenger and complete a ticket.
- Displays all queues and the currently served passenger.
- Looks up a passenger token's position and estimated waiting time.
- Shows the number of completed tickets.

## Build and run

Requires GCC or another C compiler.

```bash
gcc -std=c99 -Wall -Wextra -pedantic main.c -o railway_queue
./railway_queue
```

On Windows with MinGW:

```bash
gcc -std=c99 -Wall -Wextra -pedantic main.c -o railway_queue.exe
.
ailway_queue.exe
```

## How the queue works

Each counter has a fixed-size circular array with `front`, `rear`, and `size` fields. Enqueue advances the rear index with modulo arithmetic; dequeue advances the front index the same way. This reuses array slots after passengers leave, while keeping FIFO order.

For passenger assignment, the program compares the number waiting plus the passenger currently being served at each counter. Estimated waiting time assumes each ticket takes four minutes.

## Menu

1. Issue a passenger token
2. Call the next passenger at a counter
3. Complete the current ticket (then automatically call the next)
4. Display all counter queues
5. Check a passenger's token status
6. Show completed ticket count
0. Exit

## Project scope

This is a single-computer classroom simulation. Queue state exists only while the program is running. Each counter can hold up to 100 waiting passengers.

