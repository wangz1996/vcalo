#include "AlgoBase.hh"
#include "VAnaManager.hh"
#include "core/session/onnxruntime_cxx_api.h"
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
        // ONNX ENV
        env = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "ParticleNet");
        session_options = new Ort::SessionOptions();
        session_options->SetIntraOpNumThreads(1); // 单线程
        session_options->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

        // Load ONNX model
        const char *model_path = "particlenet.onnx"; // 替换为实际的 ONNX 模型路径
        session = new Ort::Session(*env, model_path, *session_options);
        std::cout << "ONNX Model Loaded Successfully!" << std::endl;

        Ort::AllocatorWithDefaultOptions allocator;
        // Get input node name and info
        size_t num_input_nodes = session->GetInputCount();  
        std::vector<std::string> input_node_names;
        std::vector<std::vector<int64_t>> input_shapes;

        for (size_t i = 0; i < num_input_nodes; i++)
        {
            Ort::AllocatedStringPtr input_name = session->GetInputNameAllocated(i, allocator);
            input_node_names.emplace_back(input_name.get()); // 自动复制到 std::string
            std::cout<<"Input Node Name: "<<input_node_names[i]<<std::endl;
            auto type_info = session->GetInputTypeInfo(i);
            auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
            input_shapes.push_back(tensor_info.GetShape());
            // std::cout << "Input shape: "<<input_shapes[i]<<std::endl;
            for(auto is:input_shapes[i]){
                std::cout<<is<<" ";
            }
            std::cout<<std::endl;
        }
        std::vector<T> input_tensor_data{-37.3319, 0.0, 14.3199, 14.3199, 14.3199, 14.3199, 37.3319};
        // Get output node name and info
        std::vector<int64_t> flat_shape{1,7};
        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
        Ort::Value input_tensor = Ort::Value::CreateTensor<T>(
            memory_info, input_tensor_data.data(), input_tensor_data.size(),
                                                    flat_shape.data(), flat_shape.size());
        // Output variables
        // 获取输出节点名称
    size_t num_output_nodes = session->GetOutputCount();
    std::vector<std::string> output_node_names;
    for (size_t i = 0; i < num_output_nodes; i++) {
        output_node_names.push_back(session->GetOutputNameAllocated(i, allocator).get());
        std::cout << "Output Node Name: " << output_node_names[i] << std::endl;
    }

    // 将 std::vector<std::string> 转换为 std::vector<const char*>
std::vector<const char*> c_input_node_names;
std::vector<const char*> c_output_node_names;

for (const auto& name : input_node_names) {
    c_input_node_names.push_back(name.c_str());
}

for (const auto& name : output_node_names) {
    c_output_node_names.push_back(name.c_str());
}

// 调用 session->Run
// auto output_tensors = session->Run(
//     Ort::RunOptions{nullptr},                  // 运行选项
//     c_input_node_names.data(),                 // 输入节点名称
//     &input_tensor,                             // 输入张量
//     c_input_node_names.size(),                 // 输入节点数量
//     c_output_node_names.data(),                // 输出节点名称
//     c_output_node_names.size()                 // 输出节点数量
// );
    // // 推理
    // auto output_tensors = session->Run(
    //     Ort::RunOptions{nullptr}, input_node_names.data(), &input_tensor, 1,
    //     output_node_names.data(), num_output_nodes);



        // auto output_tensors = session.Run(
        //     Ort::RunOptions{nullptr}, // 默认运行选项
        //     input_node_names.data(),  // 输入节点名称
        //     &input_tensor,            // 输入张量
        //     1,                        // 输入张量数量
        //     output_node_names.data(), // 输出节点名称
        //     num_output_nodes);        // 输出节点数量
        return 1;
    }
    int run(const int &ientry) override
    {
        // Reset branchs
        fTree->GetEntry(ientry);

        return 1;
    }

private:
    TTree *fTree;
    TTree *fOutTree;
    std::unordered_map<int, int> umap_centerId;

    // ONNX env,session and options
    Ort::Env *env = nullptr;
    Ort::SessionOptions *session_options = nullptr;
    Ort::Session *session = nullptr;
    // Branchs
    std::vector<T> *ecal_celle = nullptr;
    std::vector<int> *ecal_cellid = nullptr;

    // Output treee branchs
};