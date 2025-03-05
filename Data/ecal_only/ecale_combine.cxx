#include <vector>
#include <cmath>  // 添加cmath头文件用于isnormal检查
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TH1D.h"

void ecale_combine() {
    // 配置参数
    const int NUM_CRYSTALS = 25;   // 目标晶体ID范围0-24
    const int e_initial = 5000;
    const double MAX_RATIO = 1.2; // 最大能量比（可根据实际情况调整）

    // 创建直方图
    TH1D* hist = new TH1D("h5000", 
        "1; ecal_e/E_initial; count",
        100, 0.0, MAX_RATIO);
    
    // 准备输入文件
    TFile* inFile = TFile::Open("5000.root");
    TTree* tree = (TTree*)inFile->Get("vtree");
    
    // 事件循环设置
    TTreeReader reader(tree);
    TTreeReaderValue<std::vector<int>> cellIDs(reader, "ecal_cellid");
    TTreeReaderValue<std::vector<float>> cellEs(reader, "ecal_celle");
    
    long valid_events = 0;
    long skipped_events = 0;
    
    while (reader.Next()) {
        // 数据校验
        if(cellIDs->size() != cellEs->size()){
            std::cerr << "数据异常: ID与能量数组长度不匹配 @事件" 
                     << (valid_events + skipped_events) << std::endl;
            ++skipped_events;
            continue;
        }
        
        
        // 计算总沉积能量
        double e_deposit = 0.0;
        for(size_t i=0; i<cellIDs->size(); ++i){
            const int id = (*cellIDs)[i];
            if(id >=0 && id < NUM_CRYSTALS){
                e_deposit += (*cellEs)[i];
            }
        }
        
        // 计算能量比并填充
        const double ratio = e_deposit / e_initial;
        hist->Fill(ratio);
        ++valid_events;
        
        // 进度报告
        if(valid_events % 1000 == 0){
            std::cout << "已处理 " << valid_events << " 有效事件" << std::endl;
        }
    }
    
    // 保存结果
    TFile outFile("5000_ecal.root", "RECREATE");
    hist->Write();
    outFile.Close();
    
    // 输出统计信息
    std::cout << "\n处理完成！\n"
              << "总读取事件数: " << (valid_events + skipped_events) << "\n"
              << "有效事件数: " << valid_events << "\n"
              << "无效事件数: " << skipped_events << " (含能量异常或数据错误)\n"
              << "平均能量比: " << hist->GetMean() << " ± " 
              << hist->GetMeanError() << std::endl;
    
    // 清理资源
    delete hist;
    inFile->Close();
}
