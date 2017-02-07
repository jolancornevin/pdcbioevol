#include <iostream>
#include "src/headers/helper.h"
#include "src/headers/World.h"
#include "src/headers/Common.h"
#include "src/headers/cpu_time.h"
#include <omp.h>
#include <ctime>
#include <mpi.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;


int main() {
    int target_thread_num = 4;
    omp_set_num_threads(target_thread_num);

    //  Start Timers
    double wall0 = get_wall_time();
    double cpu0 = get_cpu_time();

    printf("Init binding matrix\n");
    Common::init_binding_matrix(897685687);

    printf("Create World\n");
    World *world = new World(32, 32, 897986875);

    printf("Initialize environment\n");
    world->init_environment();

    printf("Initialize random population\n");
    world->random_population();

    printf("Run evolution\n");
    world->run_evolution();

    //  Stop timers
    double wall1 = get_wall_time();
    double cpu1 = get_cpu_time();

    cout << "Wall Time = " << wall1 - wall0 << endl;
    cout << "Total CPU Time  = " << cpu1 - cpu0 << endl;
    return 0;
}

