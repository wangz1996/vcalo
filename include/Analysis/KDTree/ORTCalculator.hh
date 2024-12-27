#ifndef ORTCALCULATOR_HH
#define ORTCALCULATOR_HH
#include "core/session/onnxruntime_cxx_api.h"
#include "KDTree/nanoflann.hh"
#include "KDTree/PointCloud.hh"
#include "KDTree/KNNCalculator.hh"
#include <cmath>
#include <iostream>

class ORTCalculator {
public:
    ORTCalculator();
    ~ORTCalculator();

    int getScore();
    void addPoint(const float& x,const float& y,const float& z,const float& e);
    void reset();

private:
    
    // ONNX env,session and options
    Ort::Env *env = nullptr;
    Ort::SessionOptions *session_options = nullptr;
    Ort::Session *session = nullptr;
    Ort::AllocatorWithDefaultOptions allocator;

    //Inputs
    std::vector<std::string> input_node_names;
    std::vector<std::vector<int64_t>> input_shapes;

    std::vector<float> input_tensor_data;

    //KNN Calculator
    KNNCalculator<float> *knn_calculator;

    float GNN_score=-1.;

    std::vector<int64_t> flat_shape{0, 7};
    std::vector<int64_t> edge_index_shape = {2,0};

    std::vector<int64_t> edge_index;

};

#endif