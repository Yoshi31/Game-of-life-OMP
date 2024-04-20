#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <omp.h>

#define HEIGHT 10
#define WIDTH 10

using namespace std;

struct Cell {
    bool alive;
    int live_neighbors;

    Cell() : alive(false), live_neighbors(0) {}
};

void initWorld(Cell world[][HEIGHT]) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);

    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            world[i][j].alive = dis(gen);
        }
    }
}

void printWorld(Cell world[][HEIGHT]) {
    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            cout << (world[i][j].alive ? '*' : ' ') << ' ';
        }
        cout << endl;
    }
}

void countLiveNeighbors(Cell world[][HEIGHT], int i, int j) {
    world[i][j].live_neighbors = 0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            if (x == 0 && y == 0) continue;
            int new_i = i + x;
            int new_j = j + y;
            if (new_i >= 0 && new_i < WIDTH && new_j >= 0 && new_j < HEIGHT) {
                world[i][j].live_neighbors += world[new_i][new_j].alive;
            }
        }
    }
}

void updateGeneration(Cell world[][HEIGHT], Cell prevWorld[][HEIGHT]) {
#pragma omp parallel for
    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            countLiveNeighbors(prevWorld, i, j);
            if (prevWorld[i][j].alive) {
                world[i][j].alive = (prevWorld[i][j].live_neighbors == 2 || prevWorld[i][j].live_neighbors == 3);
            }
            else {
                world[i][j].alive = (prevWorld[i][j].live_neighbors == 3);
            }
        }
    }
}

void copyWorldState(Cell src[][HEIGHT], Cell dest[][HEIGHT]) {
    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            dest[i][j] = src[i][j];
        }
    }
}

int main() {
    omp_set_num_threads(6);
    Cell world[WIDTH][HEIGHT];
    Cell prevWorld[WIDTH][HEIGHT];

    initWorld(world);
    unsigned int livingCellCount = -1;

    do {
        printWorld(world);
        cout << "\nNew generation: \n" << endl;
        copyWorldState(world, prevWorld);
        updateGeneration(world, prevWorld);
        livingCellCount = 0;
        for (unsigned int i = 0; i < WIDTH; ++i) {
            for (unsigned int j = 0; j < HEIGHT; ++j) {
                livingCellCount += world[i][j].alive;
            }
        }

        if (livingCellCount == 0) {
            cout << "No living cells left" << endl;
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    } while (livingCellCount > 0);

    return 0;
}