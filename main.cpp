#include <iostream>
#include "src/headers/World.h"
#include "src/headers/Common.h"
#include "src/headers/cpu_time.h"
#include <omp.h>
#include <ctime>
#include <mpi.h>

using namespace std;

int main() {
	// Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf(">>>>>>>>>>>>>>>>>>>>>> Hello world from processor %s, rank %d"
           " out of %d processors <<<<<<<<<<<<<<<<<<<<<<<<<<\n",
           processor_name, world_rank, world_size);
           
           
    int target_thread_num = 4;
    omp_set_num_threads(target_thread_num);

    unsigned long times[target_thread_num];

    //  Start Timers
    double wall0 = get_wall_time();
    double cpu0  = get_cpu_time();

    //Test if openMP is working
    //TODO à supprimer quand c'est vérifier
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        #pragma omp critical
        {
            std::cout << "Thread number: " << omp_get_thread_num() << endl;
        }
    }

    printf("Init binding matrix\n");
    Common::init_binding_matrix(897685687);

    printf("Create World\n");
    World *world = new World(32, 32, 897986875);

    printf("Initialize environment\n");
    world->init_environment();

    bool test = false;
    if (test) {
        world->test_mutate();
    } else {
        printf("Initialize random population\n");
        world->random_population();

        printf("Run evolution\n");
        world->run_evolution();
    }

    //  Stop timers
    double wall1 = get_wall_time();
    double cpu1  = get_cpu_time();

    cout << "Wall Time = " << wall1 - wall0 << endl;
    cout << "Total CPU Time  = " << cpu1  - cpu0  << endl;
    
    // Finalize the MPI environment.
    MPI_Finalize(); 
}
