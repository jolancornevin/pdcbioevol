#include <iostream>
#include "src/headers/World.h"
#include "src/headers/Common.h"
#include <omp.h>
#include <ctime>


using namespace std;

int main() {

    int target_thread_num = 4;
    omp_set_num_threads(target_thread_num);
    unsigned long times[target_thread_num];

    clock_t begin = clock();

    //Test if openMP is working
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        std::cout << "Thread number: " << omp_get_thread_num() << endl;
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
        //world->run_evolution();
    }

    clock_t end = clock();
    std::cout << "Time : " << double(end - begin) / CLOCKS_PER_SEC << endl;
}
