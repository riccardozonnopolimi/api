# Highway Route Planner (C)

Command-line program in **C** that manages a highway of service stations with electric rental cars and plans the route with the **fewest stops** between two stations. Developed as an individual project for the **Algorithms and Principles of Computer Science** course (Algoritmi e Principi dell'Informatica, final project 2022/2023) at **Politecnico di Milano**.

The project was graded by an automatic judge on correctness first, then on execution time and memory usage, so the focus was on choosing efficient data structures in plain C (no external libraries).

## The problem

- Each station is identified by its distance from the start of the highway and has a fleet of up to 512 electric cars, each with a given range in km.
- From a station you can reach any station whose distance is within the range of one of its cars, and you rent a new car at every stop.
- The driver can never go back, and the trip can go in either direction along the highway.
- Goal: find the route with the **minimum number of stops**. If several routes are tied, choose the one whose stops are **closest to the start of the highway**, comparing from the last stop backwards.

## Commands

The program reads commands from standard input, one per line, and prints one result per command.

| Command | Output |
|---|---|
| `aggiungi-stazione <distance> <n> <range1> ... <rangeN>` | `aggiunta` / `non aggiunta` (adds a station with its cars) |
| `demolisci-stazione <distance>` | `demolita` / `non demolita` (removes a station) |
| `aggiungi-auto <distance> <range>` | `aggiunta` / `non aggiunta` (adds a car to a station) |
| `rottama-auto <distance> <range>` | `rottamata` / `non rottamata` (removes a car) |
| `pianifica-percorso <from> <to>` | the stops separated by spaces, or `nessun percorso` |

## How it works

- **Stations** are kept in a **sorted doubly linked list**, so they can be scanned in both directions; each station stores its cars in a linked list and caches the **maximum range** of its fleet, updated when cars are added or removed.
- **Route towards the end of the highway:** the route is built **backwards from the destination**: at each step the program picks the station closest to the start that can reach the current stop. This greedy choice gives both the minimum number of stops and the required tie-break.
- **Route towards the start of the highway:** the stations between the two endpoints form a directed acyclic graph that is already in topological order, so shortest paths are computed with a **single relaxation pass** in linear time (a Dijkstra-style relaxation with unit weights), following the predecessor pointers to rebuild the route.

## Build and run

```bash
gcc -O2 -Wall -std=gnu11 -o planner src/main.c
./planner < tests/open_1.txt
```

The full specification provided by the course (in Italian) is in [`spec/Specifica_2022-2023.pdf`](spec/Specifica_2022-2023.pdf).

## Tests

The `tests/` folder contains the **111 public tests provided by the course**: each `open_N.txt` is an input and `open_N.output.txt` its expected output. Inputs range from a few lines to about 50 MB.

To compile the program and run every test:

```bash
./run_tests.sh
```

Current result: 103 of 111 tests passed. The other 8 are large inputs where 1 to 4 lines out of hundreds of thousands differ (see below).

## Possible improvements

- In the failing tests, a backward route does not always pick the right option among routes with the same number of stops, so the tie-break rule needs further work.
- The graph used for backward routes has a fixed maximum size (`MAX_QUEUE`), which is not enough for the two largest tests; it could be replaced by dynamic allocation.
- Station lookup is linear in the number of stations; a balanced search tree or a hash table would make it faster on large inputs.

## Author

**Riccardo Zonno**, Computer Engineering, Politecnico di Milano
