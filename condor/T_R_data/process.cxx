#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TError.h"

void create_conv_ratio_plot() {
    gErrorIgnoreLevel = kError;  // 禁用警告信息

    const int NUM_FILES = 11;
    std::vector<double> thicknesses;
    std::vector<double> ratios;

    for (int i = 0; i < NUM_FILES; ++i) {
        int thickness = 10 + i;  // 厚度从10到20（含）
        TString filename = TString::Format("T%d.root", thickness);
        
        // 打开ROOT文件
        TFile* file = TFile::Open(filename);
        if (!file || file->IsZombie()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            continue;
        }

        // 获取TTree（保持原树名"events"）
        TTree* tree = nullptr;
        file->GetObject("vtree", tree);
        if (!tree) {
            std::cerr << "Error retrieving TTree from: " << filename << std::endl;
            file->Close();
            continue;
        }

        // 设置分支地址
        Int_t isconv;
        TBranch* br = tree->GetBranch("isconv");
        if (!br) {
            std::cerr << "Branch 'isconv' not found in: " << filename << std::endl;
            file->Close();
            continue;
        }
        tree->SetBranchAddress("isconv", &isconv);

        // 统计有效条目数
        Long64_t total_entries = tree->GetEntries();
        Long64_t conv_count = 0;
        for (Long64_t j = 0; j < total_entries; ++j) {
            tree->GetEntry(j);
            if (isconv) conv_count++;
        }

        // 计算比例并存储
        double ratio = total_entries > 0 ? static_cast<double>(conv_count) / total_entries : 0.0;
        thicknesses.push_back(thickness);
        ratios.push_back(ratio);

        // 清理资源
        file->Close();
        delete file;
    }

    // 创建并保存TGraph
    TGraph* graph = new TGraph(NUM_FILES, thicknesses.data(), ratios.data());
    graph->SetTitle("Conversion Ratio vs Thickness;Thickness (mm);Conversion Ratio");
    graph->SetMarkerStyle(20);
    graph->SetMarkerColor(kBlue);

    TFile* output = new TFile("conversion_results.root", "RECREATE");
    graph->Write("conversion_ratio_graph_1000");
    output->Close();

    // 清理内存
    delete graph;
    delete output;
}

#ifndef __CINT__
int process() {
    create_conv_ratio_plot();
    return 0;
}
#endif
