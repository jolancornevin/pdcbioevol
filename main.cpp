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
    char hostname[257];
    int size, rank;
    int i, pid;
    int bcast_value = 1;

    gethostname(hostname, sizeof hostname);
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (!rank) {
        bcast_value = 42;
    }

    MPI_Bcast(&bcast_value, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("%s\t- %d - %d - %d\n", hostname, rank, size, bcast_value);
    fflush(stdout);

    //Test if openMP is working
#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
#pragma omp critical
        {
            std::cout << "RANK " << rank << " Thread number: " << omp_get_thread_num() << endl;
        }
    }

    set_rank(rank);
    set_world_size(size);
    print_rank();

    if (is_master()) {
        int target_thread_num = 4;
        omp_set_num_threads(target_thread_num);

        unsigned long times[target_thread_num];

        //  Start Timers
        double wall0 = get_wall_time();
        double cpu0 = get_cpu_time();

        printf("Init binding matrix\n");
        Common::init_binding_matrix(897685687);

        printf("Create World\n");
        World *world = new World(32, 32, 897986875);

        printf("Initialize environment\n");
        world->init_environment();

        if (false) {
            world->test_mutate();
        } else {
            printf("Initialize random population\n");
            world->random_population();

            printf("Run evolution\n");
            world->run_evolution();
        }

        //  Stop timers
        double wall1 = get_wall_time();
        double cpu1 = get_cpu_time();

        cout << "Wall Time = " << wall1 - wall0 << endl;
        cout << "Total CPU Time  = " << cpu1 - cpu0 << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}

