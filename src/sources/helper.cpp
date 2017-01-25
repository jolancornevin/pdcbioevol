#include <iostream>
#include <stdio.h>

using namespace std;

int _rank = 0, world_size = 0;

void set_rank(int rank) {
    _rank = rank;
}

void set_world_size(int size) {
    world_size = size;
}

int get_world_size() {
    return world_size;
}

bool is_master() {
    return _rank == 0;
}

void print_rank() {
    printf("\n ------- Your MPI rank is : , %d \n", _rank);
}
