#include <vector>
#include <string>
#include "TFile.h"
#include "TH1.h"
#include "TGraph.h"
#include "TError.h"

void extract_peaks() {
    const int NUM_FILES = 11;
    std::vector<double> thickness, energy;

    for (int i = 10; i <= 20; ++i) {
        // 构造文件名和直方图名
        std::string filename = Form("T%d_conv_e.root", i);
        std::string histname = Form("h%d", i);
        
        // 打开输入文件
        TFile* infile = TFile::Open(filename.c_str());
        if (!infile || infile->IsZombie()) {
            Error("extract_peaks", "无法打开文件 %s", filename.c_str());
            continue;
        }

        // 获取直方图
        TH1* hist = nullptr;
        infile->GetObject(histname.c_str(), hist);
        if (!hist) {
            Error("extract_peaks", "在文件 %s 中找不到直方图 %s", 
                 filename.c_str(), histname.c_str());
            infile->Close();
            continue;
        }

        // 查找最大bin的横坐标
        int max_bin = hist->GetMaximumBin();
        double peak_energy = hist->GetBinCenter(max_bin);

        // 存储数据
        thickness.push_back(i);
        energy.push_back(peak_energy);

        // 清理资源
        infile->Close();
        delete infile;
    }

    // 创建输出文件和图表
    TFile* outfile = new TFile("energy_vs_thickness.root", "RECREATE");
    TGraph* graph = new TGraph(thickness.size(), thickness.data(), energy.data());
    
    // 设置图表属性
    graph->SetTitle(";thickness (mm);conv_e peak");
    graph->SetMarkerStyle(20);
    graph->SetMarkerColor(kBlue);
    graph->SetLineColor(kRed);

    // 写入并关闭文件
    graph->Write("energy_thickness_graph");
    outfile->Close();
    
    // 清理内存
    delete graph;
    delete outfile;

    Info("extract_peaks", "处理完成！结果保存至 energy_vs_thickness.root");
}
