#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <iostream>

void conve_extract() {
    // 打开输入文件
    TFile *input_file = TFile::Open("20.root");
    if (!input_file || input_file->IsZombie()) {
        std::cerr << "错误：无法打开输入文件！" << std::endl;
        return;
    }

    // 获取树
    TTree *tree = (TTree*)input_file->Get("vtree");
    if (!tree) {
        std::cerr << "错误：无法找到 vtree！" << std::endl;
        input_file->Close();
        return;
    }

    // 设置分支变量
    Float_t conv_e = 0;
    tree->SetBranchAddress("conv_e", &conv_e);

    // 创建直方图
    TH1D *hist = new TH1D(
        "h20",
        "1;Deposited Energy / Incident Energy (100 MeV):Count",
        100, 0, 1.1
    );

    // 填充直方图
    Long64_t entries = tree->GetEntries();
    for (Long64_t i = 0; i < entries; ++i) {
        tree->GetEntry(i);
        hist->Fill(conv_e / 20.0); // 转换为能量比率
    }

    // 写入输出文件
    TFile *output_file = new TFile("20_conv_only.root", "RECREATE");
    hist->Write();
    output_file->Close();

    // 清理资源
    input_file->Close();
    delete hist;

    std::cout << "直方图已保存至 EnergyDistribution.root 文件" << std::endl;
}
