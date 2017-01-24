//
// Created by arrouan on 28/09/16.
//

#include "../headers/Organism.h"
#include "../headers/DNA.h"
#include "../headers/Common.h"
#include <iostream>
#include <vector>

void Organism::translate_RNA() {

    RNA *current_rna = nullptr;
    for (auto it = dna_->bp_list_.begin(); it != dna_->bp_list_.end(); it++) {
        if ((*it)->type_ == (int) BP::BP_Type::START_RNA) {
            current_rna = new RNA((*it)->binding_pattern_,
                                  (*it)->concentration_);
        } else if ((*it)->type_ == (int) BP::BP_Type::END_RNA) {
            if (current_rna != nullptr) {
                rna_list_.push_back(current_rna);
                current_rna = nullptr;
            }
        } else if (current_rna != nullptr) {
            current_rna->bp_list_.push_back(new BP((*it)));
        }
    }
}

void Organism::translate_protein() {

    float binding_pattern = -1;
    int rna_id = 0;
    for (auto it = rna_list_.begin(); it != rna_list_.end(); it++) {
        for (auto it_j = (*it)->bp_list_.begin(); it_j < (*it)->bp_list_.end(); it_j++) {
            if ((*it_j)->type_ == (int) BP::BP_Type::START_PROTEIN) {
                binding_pattern = (*it_j)->binding_pattern_;
            } else if ((*it_j)->type_ == (int) BP::BP_Type::END_PROTEIN) {
                binding_pattern = -1;
            } else if (((*it_j)->type_ ==
                        (int) BP::BP_Type::PROTEIN_BLOCK) and (binding_pattern != -1)) {
                bool current_float = false; // true == next value if op arith else float
                bool first_value = true; // current_value is initialized or not
                float current_value = -1;
                int current_arith_op = -1;

                for (auto it_k = (*it_j)->protein_block_->bp_prot_list_.begin();
                     it_k < (*it_j)->protein_block_->bp_prot_list_.end();
                     it_k++) {
                    if ((*it_k)->type_ ==
                        (int) BP_Protein::BP_Protein_Type::ARITHMETIC_OPERATOR) {
                        if (current_float) {
                            current_arith_op = (*it_k)->op_;
                            current_float = false;
                        }
                    } else if ((*it_k)->type_ ==
                               (int) BP_Protein::BP_Protein_Type::FLOAT_NUMBER) {
                        if ((!current_float) and first_value) {
                            current_value =
                                    (*it_k)->number_;
                            current_float = true;
                            first_value = false;
                        } else if ((!current_float) and (!first_value)) {
                            float value = (*it_k)->number_;

                            current_float = true;
                            if (current_arith_op == (int) BP_Protein::Arithmetic_Operator_Type::ADD) {
                                current_value += value;
                            } else if (current_arith_op == (int) BP_Protein::Arithmetic_Operator_Type::MODULO) {
                                current_value = std::fmod(current_value, value);
                            } else if (current_arith_op == (int) BP_Protein::Arithmetic_Operator_Type::MULTIPLY) {
                                current_value *= value;
                            } else if (current_arith_op == (int) BP_Protein::Arithmetic_Operator_Type::POWER) {
                                current_value = std::pow(current_value, value);
                            }
                        }
                    }
                }

                int type = -1;
                if (current_value < 0.8) {
                    type = (int) Protein::Protein_Type::FITNESS;
                } else if (current_value >= 0.8 and current_value < 0.9) {
                    type = (int) Protein::Protein_Type::TF;
                } else if (current_value >= 0.9 and current_value < 0.95) {
                    type = (int) Protein::Protein_Type::POISON;
                } else if (current_value >= 0.95 and current_value < 1.0) {
                    type = (int) Protein::Protein_Type::ANTIPOISON;
                }

                Protein *prot = new Protein(type, binding_pattern, current_value);
                prot->concentration_ = (*it)->concentration_base_;


                if (protein_list_map_.find(current_value) == protein_list_map_.end()) {
                    protein_list_map_[current_value] = prot;


                    if (type == (int) Protein::Protein_Type::FITNESS) {
                        protein_fitness_list_.push_back(prot);
                    } else if (type == (int) Protein::Protein_Type::TF) {
                        protein_TF_list_.push_back(prot);
                    } else if (type == (int) Protein::Protein_Type::POISON) {
                        protein_poison_list_.push_back(prot);
                    } else if (type == (int) Protein::Protein_Type::ANTIPOISON) {
                        protein_antipoison_list_.push_back(prot);
                    }

                    rna_produce_protein_[rna_id][current_value] = prot;
                } else {
                    protein_list_map_[current_value]->concentration_ += (*it)->concentration_base_;
                    delete prot;
                    rna_produce_protein_[rna_id][current_value] = protein_list_map_[current_value];
                }

            }
        }
        rna_id++;
    }
}

void Organism::translate_pump() {
    bool within_pump = false;

    for (auto it = rna_list_.begin(); it != rna_list_.end(); it++) {
        for (auto it_j = (*it)->bp_list_.begin(); it_j < (*it)->bp_list_.end(); it_j++) {
            if ((*it_j)->type_ ==
                (int) BP::BP_Type::START_PUMP) {
                within_pump = true;
            } else if ((*it_j)->type_ ==
                       (int) BP::BP_Type::END_PUMP) {
                within_pump = false;
            } else if (((*it_j)->type_ ==
                        (int) BP::BP_Type::PUMP_BLOCK) and (within_pump)) {
                for (auto it_k = (*it_j)->pump_block_->bp_pump_list_.begin(); it_k < (*it_j)->pump_block_->
                        bp_pump_list_.end(); it_k++) {
                    Pump *pump = new Pump((*it_k)->in_out_, (*it_k)->start_range_,
                                          (*it_k)->end_range_, (*it_k)->speed_);
                    pump_list_.push_back(pump);
                }
            }
        }
    }
}

void Organism::translate_move() {
    bool within_move = false;

    for (auto it = rna_list_.begin(); it != rna_list_.end(); it++) {
        for (auto it_j = (*it)->bp_list_.begin(); it_j < (*it)->bp_list_.end(); it_j++) {
            if ((*it_j)->type_ ==
                (int) BP::BP_Type::START_MOVE) {
                within_move = true;
            } else if ((*it_j)->type_ ==
                       (int) BP::BP_Type::END_MOVE) {
                within_move = false;
            } else if (((*it_j)->type_ ==
                        (int) BP::BP_Type::MOVE_BLOCK) and (within_move)) {
                for (auto it_k = (*it_j)->move_block_->
                        bp_move_list_.begin(); it_k < (*it_j)->move_block_->
                        bp_move_list_.end(); it_k++) {
                    Move *move = new Move((*it_k)->distance_, (*it_k)->retry_);
                    move_list_.push_back(move);
                }
            }
        }
    }

}

/**
 * 4.4 Réseau métabolique
 *
 * Construit le graphe de dépendances d'ADN
 * Chaque ARN et chaque prot´eine contienne un motif qui permet de calculer le taux de fixation. Ce taux de
 * fixation (de la proteine sur l’ARN) permet d’influencer la quantit´e (concentration) produite par chaque
 * ARN
 */
void Organism::build_regulation_network() {
    int rna_id = 0;
    for (auto it = rna_list_.begin(); it != rna_list_.end(); it++) {
        for (auto it_j = protein_fitness_list_.begin(); it_j != protein_fitness_list_.end(); it_j++) {
            int index_i = (*it)->binding_pattern_ * Common::BINDING_MATRIX_SIZE;
            int index_j = (*it_j)->binding_pattern_ * Common::BINDING_MATRIX_SIZE;
            if (Common::matrix_binding_[index_i * Common::BINDING_MATRIX_SIZE + index_j] != 0) {
                rna_influence_[rna_id][(*it_j)->value_] = Common::matrix_binding_[
                        index_i * Common::BINDING_MATRIX_SIZE + index_j];
            }
        }
        for (auto it_j = protein_TF_list_.begin(); it_j != protein_TF_list_.end(); it_j++) {
            int index_i = rna_list_[rna_id]->binding_pattern_ * Common::BINDING_MATRIX_SIZE;
            int index_j = (*it_j)->binding_pattern_ * Common::BINDING_MATRIX_SIZE;
            if (Common::matrix_binding_[index_i * Common::BINDING_MATRIX_SIZE + index_j] != 0) {
                rna_influence_[rna_id][(*it_j)->value_] = Common::matrix_binding_[
                        index_i * Common::BINDING_MATRIX_SIZE + index_j];
            }
        }
        rna_id++;
    }
}

/**
 * Not used
 */
void Organism::compute_next_step() {
    // Activate Pump
    activate_pump();

    // Compute protein concentration for X steps
    compute_protein_concentration();
}

/**
 * Pour toutes les pompes de l'organisme, on fait les trucs
 */
void Organism::activate_pump() {
    auto it = pump_list_.begin();
#pragma omp parallel
    {
#pragma omp for
        for (int i = 0; i < pump_list_.size(); ++i) {
            if ((*it)->in_out_) {
                for (auto prot : protein_list_map_) {
                    if ((*it)->start_range_ >= prot.second->value_ and (*it)->end_range_ <= prot.second->value_) {
                        float remove = prot.second->concentration_ * ((*it)->speed_ / 100);
                        prot.second->concentration_ -= remove;

                        if (gridcell_->protein_list_map_.find(prot.second->value_)
                            == gridcell_->protein_list_map_.end()) {
                            Protein *prot_n = new Protein(prot.second->type_,
                                                          prot.second->binding_pattern_,
                                                          prot.second->value_);
                            prot_n->concentration_ = remove;
                            gridcell_->protein_list_map_[prot.second->value_] = prot_n;
                        } else {
                            gridcell_->protein_list_map_[prot.second->value_]->concentration_ += remove;
                        }
                    }
                }
            } else {
                for (auto prot : gridcell_->protein_list_map_) {
                    if ((*it)->start_range_ >= prot.first and (*it)->end_range_ <= prot.first) {
                        float remove = prot.second->concentration_ * ((*it)->speed_ / 100);
                        prot.second->concentration_ -= remove;

                        if (protein_list_map_.find(prot.first) == protein_list_map_.end()) {
                            Protein *prot_n = new Protein(prot.second->type_,
                                                          prot.second->binding_pattern_,
                                                          prot.second->value_);
                            prot_n->concentration_ = remove;
                            protein_list_map_[prot_n->value_] = prot_n;
                        } else {
                            protein_list_map_[prot.first]->concentration_ += remove;
                        }
                    }
                }
            }
            it++;
        }
    }
}

void Organism::init_organism() {
    translate_RNA();
    translate_protein();
    translate_pump();
    translate_move();
}

void Organism::compute_protein_concentration() {
    //TODO à optimiser, mais je sais pas trop comment
#pragma omp parallel
    {
#pragma omp for
        for (int rna_id = 0; rna_id < rna_list_.size(); rna_id++) {
            float delta_pos = 0, delta_neg = 0;

            for (auto prot : rna_influence_[rna_id]) {
                if (prot.second > 0)
                    delta_pos += prot.second * protein_list_map_[prot.first]->concentration_;
                else
                    delta_neg -= prot.second * protein_list_map_[prot.first]->concentration_;
            }

            float delta_pos_pow_n = pow(delta_pos, Common::hill_shape_n);
            float delta_neg_pow_n = pow(delta_neg, Common::hill_shape_n);

            rna_list_[rna_id]->current_concentration_ = rna_list_[rna_id]->concentration_base_
                                                        * (Common::hill_shape / (delta_neg_pow_n + Common::hill_shape))
                                                        * (1 + ((1 / rna_list_[rna_id]->concentration_base_) - 1)
                                                               * (delta_pos_pow_n /
                                                                  (delta_pos_pow_n + Common::hill_shape)));
        }
    }

    std::unordered_map<float, float> delta_concentration;

    for (auto rna : rna_produce_protein_) {
        for (auto prot : rna_produce_protein_[rna.first]) {
            if (delta_concentration.find(prot.first) == delta_concentration.end()) {
                delta_concentration[prot.first] = rna_list_[rna.first]->current_concentration_;
            } else {
                delta_concentration[prot.first] += rna_list_[rna.first]->current_concentration_;
            }
        }
    }

    for (auto delta : delta_concentration) {
        delta.second -= Common::Protein_Degradation_Rate * protein_list_map_[delta.first]->concentration_;
        delta.second *= 1 / (Common::Protein_Degradation_Step);

        protein_list_map_[delta.first]->concentration_ += delta.second;
    }
}

bool Organism::dying_or_not() {
    // Compute if dying or not
    double concentration_sum = 0;
    for (auto prot : protein_fitness_list_) {
        concentration_sum += prot->concentration_;
    }
    for (auto prot : protein_TF_list_) {
        concentration_sum += prot->concentration_;
    }

    if (concentration_sum > 10.0 and concentration_sum <= 0.0) {
        return true;
    }

    double poison = 0, antipoison = 0;

    for (auto prot : protein_poison_list_) {
        poison += prot->concentration_;
    }

    for (auto prot : protein_antipoison_list_) {
        antipoison += prot->concentration_;
    }

    if (poison - antipoison > 0.1) {
        return true;
    }

    std::binomial_distribution<int> dis_death(1024, Common::Random_Death);
    int death_number = dis_death(gridcell_->float_gen_);


    bool death = (bool) death_number % 2;

    return death;
}

void Organism::try_to_move() {
    for (auto it = move_list_.begin(); it != move_list_.end(); it++) {
        if ((*it)->distance_ > 0) {
            bool move = false;
            int retry = 0;
            std::uniform_int_distribution<uint32_t> dis_distance(0, (*it)->distance_);
            while (retry < (*it)->retry_ and !move) {
                int x_offset = dis_distance(gridcell_->float_gen_);
                int y_offset = dis_distance(gridcell_->float_gen_);
                int new_x, new_y;
                if (gridcell_->x_ + x_offset >= gridcell_->world_->width_) {
                    new_x = gridcell_->world_->width_ - 1;
                } else {
                    new_x = gridcell_->x_ + x_offset;
                }

                if (gridcell_->y_ + y_offset >= gridcell_->world_->height_) {
                    new_y = gridcell_->world_->height_ - 1;
                } else {
                    new_y = gridcell_->y_ + y_offset;
                }

                if (gridcell_->world_->grid_cell_[new_x * gridcell_->world_->width_ + new_y]->organism_ != nullptr) {
                    move = true;
                    move_success_++;
                    gridcell_->world_->grid_cell_[new_x * gridcell_->world_->width_ + new_y]->organism_ = this;
                    gridcell_ = gridcell_->world_->grid_cell_[new_x * gridcell_->world_->width_ + new_y];
                    gridcell_->organism_ = nullptr;
                }
            }
        }
    }
}

/**
 * Dépuis la fitness de l'organisme.
 *
 * L’adaptation à l’environement est calcul´e en faisant la soustraction des valeurs des prot´eines de l’organisme
 * et celle de l’environement. Cela donne l’erreur m´etabolique. Celle-ci est ensuite utilis´ee pour calculer la
 * fitness.
 */
void Organism::compute_fitness() {
    life_duration_++;

    for (int i = 0; i < Common::Metabolic_Error_Precision; i++)
        metabolic_error[i] = 0.0;

    for (auto prot : protein_fitness_list_) {
        int index = prot->value_ * Common::Metabolic_Error_Precision;

        float concentration = prot->concentration_;

        for (int j = index - Common::Metabolic_Error_Protein_Spray;
             j <= index + Common::Metabolic_Error_Protein_Spray; j++) {
            if (j < Common::Metabolic_Error_Precision and j >= 0) {
                if (j < index) {
                    metabolic_error[j] += (index - j) * Common::Metabolic_Error_Protein_Slope * concentration;
                } else if (j > index) {
                    metabolic_error[j] += (j - index) * Common::Metabolic_Error_Protein_Slope * concentration;
                } else {
                    metabolic_error[j] += concentration;
                }
            }
        }
    }

    sum_metabolic_error = 0;
    for (int i = 0; i < Common::Metabolic_Error_Precision; i++) {
        sum_metabolic_error += std::abs(gridcell_->environment_target[i] - metabolic_error[i]);
    }

    sum_metabolic_error = sum_metabolic_error / Common::Metabolic_Error_Precision;

    fitness_ = std::exp(-Common::Fitness_Selection_Pressure * sum_metabolic_error);
}

void Organism::mutate() {
    old = this;

    std::binomial_distribution<int> dis_switch(dna_->bp_list_.size(), Common::Mutation_Rate);
    std::binomial_distribution<int> dis_insertion(dna_->bp_list_.size(), Common::Mutation_Rate);
    std::binomial_distribution<int> dis_deletion(dna_->bp_list_.size(), Common::Mutation_Rate);
    std::binomial_distribution<int> dis_duplication(dna_->bp_list_.size(), Common::Mutation_Rate);
    std::binomial_distribution<int> dis_modification(dna_->bp_list_.size(), Common::Mutation_Rate);

    int nb_switch = dis_switch(gridcell_->float_gen_);
    int nb_insertion = dis_insertion(gridcell_->float_gen_);
    int nb_deletion = dis_deletion(gridcell_->float_gen_);
    int nb_duplication = dis_duplication(gridcell_->float_gen_);
    int nb_modification = dis_modification(gridcell_->float_gen_);

    std::uniform_int_distribution<uint32_t> dis_position(0, dna_->bp_list_.size());

    for (int i = 0; i < nb_deletion; i++) {
        int deletion_pos = dis_position(gridcell_->float_gen_);
        while (deletion_pos >= dna_->bp_list_.size()) {
            deletion_pos = dis_position(gridcell_->float_gen_);
        }

        dna_->bp_list_.erase(dna_->bp_list_.begin() + deletion_pos);
    }

    for (int i = 0; i < nb_switch; i++) {
        int switch_pos_1 = dis_position(gridcell_->float_gen_);
        while (switch_pos_1 >= dna_->bp_list_.size()) {
            switch_pos_1 = dis_position(gridcell_->float_gen_);
        }
        int switch_pos_2 = dis_position(gridcell_->float_gen_);
        while (switch_pos_2 >= dna_->bp_list_.size()) {
            switch_pos_2 = dis_position(gridcell_->float_gen_);
        }

        BP *tmp = dna_->bp_list_[switch_pos_1];
        dna_->bp_list_[switch_pos_1] = dna_->bp_list_[switch_pos_2];
        dna_->bp_list_[switch_pos_2] = tmp;
    }

    for (int i = 0; i < nb_duplication; i++) {
        int duplication_pos = dis_position(gridcell_->float_gen_);
        while (duplication_pos >= dna_->bp_list_.size()) {
            duplication_pos = dis_position(gridcell_->float_gen_);
        }

        int where_to_duplicate = dis_position(gridcell_->float_gen_);
        while (where_to_duplicate >= dna_->bp_list_.size()) {
            where_to_duplicate = dis_position(gridcell_->float_gen_);
        }

        dna_->bp_list_.insert(dna_->bp_list_.begin() + where_to_duplicate,
                              new BP(dna_->bp_list_[duplication_pos]));
    }


    for (int i = 0; i < nb_insertion; i++) {
        int insertion_pos = dis_position(gridcell_->float_gen_);
        while (insertion_pos >= dna_->bp_list_.size()) {
            insertion_pos = dis_position(gridcell_->float_gen_);
        }

        dna_->insert_a_BP(insertion_pos, gridcell_);
    }

    for (int i = 0; i < nb_modification; i++) {
        int modification_pos = dis_position(gridcell_->float_gen_);
        while (modification_pos >= dna_->bp_list_.size()) {
            modification_pos = dis_position(gridcell_->float_gen_);
        }

        dna_->modify_bp(modification_pos, gridcell_);
    }
}

Organism *Organism::divide() {
    Organism *new_org = new Organism(this);
    return new_org;
}

Organism::Organism(Organism *organism) {
    dna_ = new DNA(organism->dna_);


    for (auto prot : organism->protein_list_map_) {
        Protein *new_prot = new Protein(prot.second);
        new_prot->concentration_ = new_prot->concentration_ / 2;
        prot.second->concentration_ = prot.second->concentration_ / 2;
    }
}

Organism::~Organism() {
    for (auto rna : rna_list_)
        delete rna;

    delete dna_;


    rna_influence_.clear();
    rna_produce_protein_.clear();

    for (auto prot : protein_list_map_) {
        delete prot.second;
    }

    protein_fitness_list_.clear();
    protein_TF_list_.clear();
    protein_poison_list_.clear();
    protein_antipoison_list_.clear();

    protein_list_map_.clear();

    for (auto pump : pump_list_) {
        delete pump;
    }
    pump_list_.clear();

    for (auto move : move_list_) {
        delete move;
    }
    move_list_.clear();
}

/*
========================================================================================================================
                                                    MPI
========================================================================================================================
 */

//TODO send array avec MPI.

/**
 * @desc Run
 */
void Organism::mpiComputeProteinConcentrationFromMaster() {
//    printf("0\n");

    float **rna = _getRnaInfluenceFristAndSeconds();
    float **protein = _getProteinConcentrationIndexAndValues();
    float *contentration_base = _getRnaConcentrationBase();

    //TODO send everything to other machines

//    printf("1\n");

    float *res = _mpi_compute_protein_concentration(*rna[0], rna[1], rna[2],
                                                    (float) protein_list_map_.size(), protein[0], protein[1],
                                                    (float) rna_list_.size(), contentration_base);

//    printf("2\n");

    for (int i = 0; i < protein_list_map_.size(); ++i) {
//        printf("%f\n", res[i]);
          protein_list_map_[protein[0][i]]->concentration_ = res[i];
    }

//    printf("3\n");
    delete[] rna[0];
//    printf("4\n");
    delete[] rna[1];
//    printf("5\n");
    delete[] rna[2];
//    printf("6\n");
    delete[] protein[0];
//    printf("7\n");
    delete[] protein[1];
//    printf("8\n");
    delete[] contentration_base;
}


/**
 * @desc
 * @param _protein_concentration_index
 * @param searchedFloat
 * @return
 */
int Organism::_getIndexValueFromFloat(float _protein_concentration_index[], float searchedFloat) {
    int iProt = 0, lenProt = (sizeof(_protein_concentration_index) / sizeof(*_protein_concentration_index));
    for (; iProt < lenProt; ++iProt) {
        if (_protein_concentration_index[iProt] == searchedFloat)
            return iProt;
    }
    return -1;
}

/**
 * Contruit deux tableaux à partir de la map protein_list_map_
 * @return
 */
float **Organism::_getProteinConcentrationIndexAndValues() {
    float **res = (float **) malloc(2 * sizeof(float *));

    float *index = (float *) malloc(protein_list_map_.size() * sizeof(float));
    float *value = (float *) malloc(protein_list_map_.size() * sizeof(float));

    int i = 0;

    for (auto prot : protein_list_map_) {
        index[i] = prot.first;
        value[i++] = prot.second->concentration_;
    }

    res[0] = index;
    res[1] = value;

    return res;
}

/**
 * Contruit deux tableaux à partir de la map rna_influence
 * @return
 */
float **Organism::_getRnaInfluenceFristAndSeconds() {
    float **res = (float **) malloc(3 * sizeof(float *));
    std::vector<float> *first = new std::vector<float>();
    std::vector<float> *seconds = new std::vector<float>();

    float *size = new float(0);

    for (auto rna : rna_influence_) {
        for (auto prot : rna.second) {
            *size += 1;
            first->push_back(prot.first);
            seconds->push_back(prot.second);
        }
    }

    //making a pointer that points to the actual array the vector is using internally.
    res[0] = size;
    res[1] = &((*first)[0]);
    res[2] = &((*seconds)[0]);

    return res;
}

/**
 * Contruit deux tableaux à partir de la map rna_influence
 * @return
 */
float *Organism::_getRnaConcentrationBase() {
    float *concentrationBase = (float *) malloc(rna_list_.size() * sizeof(float));

    for (int rna_id = 0; rna_id < rna_list_.size(); rna_id++) {
        concentrationBase[rna_id] = rna_list_[rna_id]->concentration_base_;
    }

    return concentrationBase;
}

/**
 * @desc : Permet d'executer la fonction compute_protein_concentration en mpi. Il faut donc transformer tout les objets en array
 *
 *          rna_influence_ : map<int, map<float, float>> --> float first[], float seconds[]
 *                          Comme la map est en fait utilisé comme un tableau, et que l'on fait des lectures itératives
 *                              i.e on ne récupère de valeur de la deuxième map avec un float,
 *                          on peut transformer cette map de map en tableau, contenant toutes les valeurs à la suite
 *
 *          protein_list_map_: On utilise seulement la valeur contentration de celles ci. Par contre, on y accède par un float
 *                             On construit donc deux tableau. Un qui contient les index et un autre les valeurs associé
 *                             On fait la correspondances entre leux deux par rapport à leurs indices dans le tableau
 *                                  --> _getIndexValueFromFloat
 *
 *          rna_list_[rna_id]->current_concentration_ : n'est pas passé en paramètre car utilisé seulement dans cette fonction
 *
 *          rna_list_[rna_id]->concentration_base_ :
 *
 * @param _rna_influence_first
 * @param _rna_influence_second
 * @param _protein_concentration_index
 * @param _protein_concentration_value
 * @param _rna_current_concentration
 * @param _rna_base_concentration
 * @return
 */
float *Organism::_mpi_compute_protein_concentration(float _rna_size, float _rna_influence_first[],
                                                    float _rna_influence_second[],
                                                    float _protein_size,
                                                    float _protein_concentration_index[],
                                                    float _protein_concentration_value[],
                                                    float _rna_base_size,
                                                    float _rna_base_concentration[]) {

    float _rna_current_concentration[(int) _rna_base_size];

#pragma omp parallel
    {
#pragma omp for
        for (int rna_id = 0; rna_id < (int)_rna_base_size; rna_id++) {
            float delta_pos = 0, delta_neg = 0;

            int iRna = 0;

            for (iRna = 0; iRna < _rna_size; ++iRna) {
                int _index = _getIndexValueFromFloat(_protein_concentration_index, _rna_influence_first[iRna]);
                if (_index != -1) {
                    if (_rna_influence_second[iRna] > 0) {
                        delta_pos += _rna_influence_second[iRna] * _protein_concentration_value[_index];
                    } else {
                        delta_neg += _rna_influence_second[iRna] * _protein_concentration_value[_index];
                    }
                }
            }

            float delta_pos_pow_n = pow(delta_pos, Common::hill_shape_n);
            float delta_neg_pow_n = pow(delta_neg, Common::hill_shape_n);

            _rna_current_concentration[rna_id] = _rna_base_concentration[rna_id]
                                                 * (Common::hill_shape / (delta_neg_pow_n + Common::hill_shape))
                                                 * (1 + ((1 / _rna_base_concentration[rna_id]) - 1)
                                                        * (delta_pos_pow_n /
                                                           (delta_pos_pow_n + Common::hill_shape)));
        }
    }

    std::unordered_map<float, float> delta_concentration;

    for (auto rna : rna_produce_protein_) {
        for (auto prot : rna_produce_protein_[rna.first]) {
            if (delta_concentration.find(prot.first) == delta_concentration.end()) {
                delta_concentration[prot.first] = _rna_current_concentration[rna.first];
            } else {
                delta_concentration[prot.first] += _rna_current_concentration[rna.first];
            }
        }
    }

    for (auto delta : delta_concentration) {
        int _index = _getIndexValueFromFloat(_protein_concentration_index, delta.first);
        if (_index != -1) {
            delta.second -= Common::Protein_Degradation_Rate * _protein_concentration_value[_index];
            delta.second *= 1 / (Common::Protein_Degradation_Step);

            _protein_concentration_value[_index] += delta.second;
        }
    }

    return _protein_concentration_value;
}
