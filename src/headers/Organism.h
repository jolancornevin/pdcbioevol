//
// Created by arrouan on 28/09/16.
//

#ifndef PDC_EVOL_MODEL_ORGANISM_H
#define PDC_EVOL_MODEL_ORGANISM_H

#include <vector>
#include <unordered_map>
#include "Move.h"
#include "Pump.h"
#include "Protein.h"
#include "RNA.h"
#include "Common.h"
#include "GridCell.h"
#include <mpi.h>

class DNA;

class Organism {
public:
    Organism(Organism *organism);

    Organism(DNA *dna) { dna_ = dna; };

    ~Organism();

    Organism *old = nullptr;

    DNA *dna_;
    std::vector<RNA *> rna_list_;
    std::unordered_map<int, std::unordered_map<float, float>> rna_influence_;
    std::unordered_map<int, std::unordered_map<float, Protein *>> rna_produce_protein_;

    std::vector<Protein *> protein_fitness_list_;
    std::vector<Protein *> protein_TF_list_;
    std::vector<Protein *> protein_poison_list_;
    std::vector<Protein *> protein_antipoison_list_;

    std::unordered_map<float, Protein *> protein_list_map_;

    //Une liste de pompe
    std::vector<Pump *> pump_list_;
    //Une liste de déplacement
    std::vector<Move *> move_list_;

    float metabolic_error[Common::Metabolic_Error_Precision];
    float sum_metabolic_error;

    GridCell *gridcell_;

    float fitness_ = -1;

    int life_duration_ = 0;
    int move_success_ = 0;
    int dupli_success_ = 0;

    void init_organism();

    void translate_RNA();

    void translate_protein();

    void build_regulation_network();

    void translate_pump();

    void translate_move();

    void mutate();

    void activate_pump();

    void compute_next_step();

    bool dying_or_not();

    void try_to_move();

    void compute_fitness();

    void compute_protein_concentration();

    Organism *divide();

    void mpiComputeProteinConcentrationFromMaster();

private:

    float ** _getRnaInfluenceFristAndSeconds();

    float **_getProteinConcentrationIndexAndValues();

    float * _getRnaConcentrationBase();

    int _getIndexValueFromFloat(float *_protein_concentration_index, float searchedFloat);

    float *_mpi_compute_protein_concentration(float _rna_size, float *_rna_influence_first, float *_rna_influence_second,
                                              float _protein_size, float *_protein_concentration_index,
                                              float *_protein_concentration_value, float _rna_base_size,
                                              float *_rna_base_concentration);
};

#endif //PDC_EVOL_MODEL_ORGANISM_H
