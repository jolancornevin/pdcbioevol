#include <iostream>

using namespace std;

int _rank = 0;

void set_rank(int rank) {
	_rank = rank;
}

bool is_master() {
	return _rank == 0;
}

void lolilol()
{
	printf("\n ------- coucou, %d \n", _rank);
}
