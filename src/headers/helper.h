#ifndef PDC_EVOL_HELPER_H
#define PDC_EVOL_HELPER_H

#include <iostream>

void set_rank(int rank);
bool is_master();

void print_rank();

int get_world_size();
void set_world_size(int size);

#endif //PDC_EVOL_HELPER_H
