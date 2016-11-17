//
// Created by arrouan on 28/09/16.
//

#ifndef PDC_EVOL_MODEL_PUMP_H
#define PDC_EVOL_MODEL_PUMP_H

/**
 * Une pompe de protéine associé à un organisme
 */
class Pump {
public:
    Pump(bool in_out, float start_range, float end_range, int speed) {
        in_out_ = in_out;
        start_range_ = start_range;
        end_range_ = end_range;
        speed_ = speed;
    }

    Pump(Pump *pump) {
        in_out_ = pump->in_out_;
        start_range_ = pump->start_range_;
        end_range_ = pump->end_range_;
        speed_ = pump->speed_;
    }

    //Si la pompe prend des valeurs dans l'environement
    bool in_out_ = false;

    //Début de l'effet de la pompe
    float start_range_ = -1;
    //Fin de l'effet de la pompe
    float end_range_ = -1;

    //code la rapiditée de la pompe
    //(le pourcentage de concentration des proteines qui est pris par la pompe à chaque pas de temps)
    int speed_ = -1;
};


#endif //PDC_EVOL_MODEL_PUMP_H
