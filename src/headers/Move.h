//
// Created by arrouan on 28/09/16.
//

#ifndef PDC_EVOL_MODEL_MOVE_H
#define PDC_EVOL_MODEL_MOVE_H


class Move {
public:
    Move(int distance, int retry) {
        distance_ = distance;
        retry_ = retry;
    }

    Move(Move *move) {
        distance_ = move->distance_;
        retry_ = move->retry_;
    }

    //la distance maximale, i.e. le nombre de cases dans la grille, à laquelle la pseudo-flagelle permet d’accéder
    int distance_ = -1;
    //le nombre maximum de tentative de d´eplacement la flagelle permet de faire à chaque pas de temps
    int retry_ = -1;
};


#endif //PDC_EVOL_MODEL_MOVE_H
