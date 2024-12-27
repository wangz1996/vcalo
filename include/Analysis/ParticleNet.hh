#include "AlgoBase.hh"
#include "VAnaManager.hh"
#include "KDTree/ORTCalculator.hh"
#include <unordered_map>
#include "global.hh"
#include <utility>
#include <cmath>
#include <unordered_map>

template <class T>
class ParticleNet : public AlgoBase<T>
{
public:
    ParticleNet(const std::string &name) : AlgoBase<T>(name)
    {
        std::cout << name << " algorithm constructed" << std::endl;
    }
    int init(TTree *tin, TTree *tout) override
    {
        fTree = tin;
        fOutTree = tout;
        fTree->SetBranchAddress("conv_e", &conv_e);
        fTree->SetBranchAddress("ecal_cellid", &ecal_cellid);
        fTree->SetBranchAddress("ecal_celle", &ecal_celle);
        fTree->SetBranchAddress("tracker_hitx", &tracker_hitx);
        fTree->SetBranchAddress("tracker_hity", &tracker_hity);
        fTree->SetBranchAddress("tracker_hitz", &tracker_hitz);
        fTree->SetBranchAddress("tracker_hite", &tracker_hite);

        //Output variables
        fOutTree->Branch("GNN_isconv",&GNN_isconv);

        //ORT Calculator
        ortc = new ORTCalculator();
        //KNN Calculator
        // knn_calculator = new KNNCalculator();
        // // ONNX ENV
        // env = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "ParticleNet");
        // session_options = new Ort::SessionOptions();
        // session_options->SetIntraOpNumThreads(1); // 单线程
        // session_options->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        // // Load ONNX model
        // const char *model_path = "particlenet.onnx"; // 替换为实际的 ONNX 模型路径
        // session = new Ort::Session(*env, model_path, *session_options);
        // std::cout << "ONNX Model Loaded Successfully!" << std::endl;

        // Ort::AllocatorWithDefaultOptions allocator;
        // // Get input node name and info
        // size_t num_input_nodes = session->GetInputCount();
        // std::vector<std::string> input_node_names;
        // std::vector<std::vector<int64_t>> input_shapes;

        // for (size_t i = 0; i < num_input_nodes; i++)
        // {
        //     Ort::AllocatedStringPtr input_name = session->GetInputNameAllocated(i, allocator);
        //     input_node_names.emplace_back(input_name.get()); // 自动复制到 std::string
        //     std::cout << "Input Node Name: " << input_node_names[i] << std::endl;
        //     auto type_info = session->GetInputTypeInfo(i);
        //     auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        //     input_shapes.push_back(tensor_info.GetShape());
        // }
        // std::vector<T> input_tensor_data{-37.3319,   0.0000,   0.0000,   0.0000,   0.0000,   0.0000,  37.3319};
        // PointCloud cloud;
        // cloud.points = {{-37.3319, 0.}};
        // KDTree kdtree(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        // kdtree.buildIndex();
        // int k=12;

        // //Build edges
        // std::vector<int64_t> edge_col1,edge_col2;
        // for(int i=0;i<cloud.points.size();i++){
        //     float query_pt[2] = {cloud.points[i][0], cloud.points[i][1]};
        //     std::vector<unsigned> ret_indexes(k);
        //     std::vector<float> out_dist_sqr(k);
        //     kdtree.knnSearch(&query_pt[0], k, &ret_indexes[0], &out_dist_sqr[0]);
        //     for (size_t j = 0; j < ret_indexes.size(); j++) {
        //     if (i != ret_indexes[j]) {
        //         edge_col1.emplace_back(i);
        //         edge_col2.emplace_back(ret_indexes[j]);
        //     }
        //     }
        // }
        // std::vector<int64_t> edge_index{};
        // edge_index.insert(edge_index.end(), edge_col1.begin(), edge_col1.end());
        // edge_index.insert(edge_index.end(), edge_col2.begin(), edge_col2.end());

        // // Get output node name and info
        // std::vector<int64_t> flat_shape{1, 7};
        // std::vector<int64_t> edge_index_shape = {2,edge_col1.size()};
        // auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

        // Ort::Value input_tensor = Ort::Value::CreateTensor<T>(
        //     memory_info, input_tensor_data.data(), input_tensor_data.size(),
        //     flat_shape.data(), flat_shape.size());

        // Ort::Value edge_index_tensor = Ort::Value::CreateTensor<int64_t>(memory_info, edge_index.data(), edge_index.size(), edge_index_shape.data(), edge_index_shape.size());
        // // Output variables
        // // 获取输出节点名称
        // size_t num_output_nodes = session->GetOutputCount();
        // std::vector<std::string> output_node_names;
        // for (size_t i = 0; i < num_output_nodes; i++)
        // {
        //     output_node_names.push_back(session->GetOutputNameAllocated(i, allocator).get());
        //     std::cout << "Output Node Name: " << output_node_names[i] << std::endl;
        // }

        // // 将 std::vector<std::string> 转换为 std::vector<const char*>
        // std::vector<const char *> c_input_node_names;
        // std::vector<const char *> c_output_node_names;

        // for (const auto &name : input_node_names)
        // {
        //     c_input_node_names.push_back(name.c_str());
        // }

        // for (const auto &name : output_node_names)
        // {
        //     c_output_node_names.push_back(name.c_str());
        // }

        // std::vector<Ort::Value> inputs;
        // inputs.emplace_back(std::move(input_tensor));
        // inputs.emplace_back(std::move(edge_index_tensor));
        // // 调用 session->Run
        // auto output_tensors = session->Run(
        //     Ort::RunOptions{nullptr},  // 运行选项
        //     c_input_node_names.data(), // 输入节点名称
        //     inputs.data(), // 输入张量
        //     2,                         // 输入张量数量
        //     c_output_node_names.data(), // 输出节点名称
        //     num_output_nodes
        // );
        // float* output_data = output_tensors[0].GetTensorMutableData<float>();
        // std::cout<<"output_data[0]:"<<output_data[0]<<std::endl;
        return 1;
    }
    int run(const int &ientry) override
    {
        // Reset branchs
        fTree->GetEntry(ientry);
        ortc->reset();
        ortc->addPoint(0.,0.,-251.8,conv_e);
        for(size_t i=0;i<tracker_hitx->size();i++)
        {
            float x = tracker_hitx->at(i);
            float y = tracker_hity->at(i);
            float z = tracker_hitz->at(i);
            float e = 0.1;
            ortc->addPoint(x,y,z,e);
        }
        for(size_t i=0;i<ecal_celle->size();i++)
        {
            float e = ecal_celle->at(i);
            int id = ecal_cellid->at(i);
            float x = ecal_id_xy[id].first;
            float y = ecal_id_xy[id].second;
            ortc->addPoint(x,y,0.1,e);
        }
        GNN_isconv = ortc->getScore();
        return 1;
    }

private:
    TTree *fTree;
    TTree *fOutTree;
    ORTCalculator *ortc;
    std::unordered_map<int, int> umap_centerId;
    // Branchs
    std::vector<T> *ecal_celle = nullptr;
    std::vector<int> *ecal_cellid = nullptr;
    std::vector<T> *tracker_hitx = nullptr;
    std::vector<T> *tracker_hity = nullptr;
    std::vector<T> *tracker_hitz = nullptr;
    std::vector<T> *tracker_hite = nullptr;
    T conv_e;

    // Output treee branchs
    T GNN_isconv = 0.;
};