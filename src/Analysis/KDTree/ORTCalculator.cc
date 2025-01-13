#include "KDTree/ORTCalculator.hh"

ORTCalculator::ORTCalculator()
{
    knn_calculator = new KNNCalculator<float>();
    // ONNX ENV
    env = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "ParticleNet");
    session_options = new Ort::SessionOptions();
    session_options->SetIntraOpNumThreads(1); // 单线程
    session_options->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    //// Load ONNX model
    const char *model_path = "parN_log.onnx";
    session = new Ort::Session(*env, model_path, *session_options);
    std::cout << "ONNX Model Loaded Successfully!" << std::endl;

    
    size_t num_input_nodes = session->GetInputCount();

    for (size_t i = 0; i < num_input_nodes; i++)
    {
        Ort::AllocatedStringPtr input_name = session->GetInputNameAllocated(i, allocator);
        input_node_names.emplace_back(input_name.get()); // 自动复制到 std::string
        // std::cout << "Input Node Name: " << input_node_names[i] << std::endl;
        auto type_info = session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        input_shapes.push_back(tensor_info.GetShape());
    }
}

float ORTCalculator::getScore(){
    //KNN edge
    knn_calculator->run(11);
    edge_index = knn_calculator->getEdgeIndex();
    // for(auto edge: edge_index){
    //     std::cout<<edge<<" ";
    // }
    // std::cout<<std::endl;

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    // for(int i=0;i<input_tensor_data.size();i++){
        
    //     std::cout<<input_tensor_data[i]<<" ";
    // }
    // std::cout<<std::endl;
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
            memory_info, input_tensor_data.data(), input_tensor_data.size(),
            flat_shape.data(), flat_shape.size());
    if(flat_shape[0]==1){
        edge_index_shape[1]=0;
    }
    Ort::Value edge_index_tensor = Ort::Value::CreateTensor<int64_t>(memory_info, edge_index.data(), edge_index.size(), edge_index_shape.data(), edge_index_shape.size());
    size_t num_output_nodes = session->GetOutputCount();
    // std::cout<<"Number of output nodes: "<<num_output_nodes<<std::endl;
    std::vector<std::string> output_node_names;

    for (size_t i = 0; i < num_output_nodes; i++)
    {
        output_node_names.push_back(session->GetOutputNameAllocated(i, allocator).get());
        // std::cout << "Output Node Name: " << output_node_names[i] << std::endl;
    }

    std::vector<const char *> c_input_node_names;
        std::vector<const char *> c_output_node_names;

        for (const auto &name : input_node_names)
        {
            c_input_node_names.push_back(name.c_str());
        }

        for (const auto &name : output_node_names)
        {
            c_output_node_names.push_back(name.c_str());
        }
    std::vector<Ort::Value> inputs;
        inputs.emplace_back(std::move(input_tensor));
        inputs.emplace_back(std::move(edge_index_tensor));
    
    // 调用 session->Run
    try{
        
        auto output_tensors = session->Run(
            Ort::RunOptions{nullptr},  // 运行选项
            c_input_node_names.data(), // 输入节点名称
            inputs.data(), // 输入张量
            2,                         // 输入张量数量
            c_output_node_names.data(), // 输出节点名称
            num_output_nodes
        );
        float* output_data = output_tensors[0].GetTensorMutableData<float>();
        GNN_score = output_data[0];
        // std::cout<<GNN_score<<std::endl;
        if(std::isfinite(GNN_score) == false)std::cout<<GNN_score<<std::endl;
    }
    catch(const Ort::Exception& e){
        std::cerr << "ONNX Runtime exception: " << e.what() << std::endl;
        return -1;
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
        return -1;
    }
        return GNN_score;
}

void ORTCalculator::addPoint(const float& x,const float& y,const float& z,float& e){
    // 计算半径距离 r_t
    double r_t = std::sqrt(x * x + y * y);
    double theta=0.;
    double deta=0.;
    if (r_t == 0 and z == 0){
        theta = 0.;
        deta = 0.;
    }
    else {
        theta = std::atan2(r_t,z);
        theta = theta > 1e-8 ? theta:1e-8;
    }
    // 计算 η
    deta = -std::log(std::tan(theta / 2.0));

    // 计算 φ
    float dphi = std::atan2(y, x);
    // 定义其他属性
    // double pt = e;
    // double ptrel = e;
    // double erel = e;
    e = e<0.1 ? 0.1 : e;
    float pt = std::log(e);
    float ptrel = std::log(e);
    float erel = std::log(e);

    // 计算 ΔR
    float dR = std::sqrt(deta * deta + dphi * dphi);

    // 返回结果
    input_tensor_data.insert(input_tensor_data.end(), {deta,dphi,pt,std::log(e),ptrel,erel,dR});
    flat_shape[0]++;
    knn_calculator->addPoint(deta,dphi);
    edge_index_shape[1]++;
}

void ORTCalculator::reset(){
    edge_index_shape = {2,0};
    flat_shape = {0,7};
    input_tensor_data.clear();
    knn_calculator->reset();
}

ORTCalculator::~ORTCalculator(){
    delete knn_calculator;
    delete session;
    delete session_options;
    delete env;
}