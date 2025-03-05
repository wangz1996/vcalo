#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"

void draw(){
  // 打开ROOT文件并获取TTree
  TFile *f = TFile::Open("test.root");
  if (!f || !f->IsOpen()) {
      std::cerr << "Could not open file: test.root" << std::endl;
      return;
  }

  TTree *t = (TTree*)f->Get("vtree");
  if (!t) {
      std::cerr << "Tree vtree not found in file." << std::endl;
      return;
  }

  // 声明conv_e的指针
  float conv_e = 0;
  t->SetBranchAddress("conv_e", &conv_e);

  // 检查分支是否正确绑定
  if (t->GetBranch("conv_e") == nullptr) {
      std::cerr << "Branch conv_e not found or not correctly bound." << std::endl;
      return;
  }

  // 创建一个直方图来显示conv_e的数据
  TH1D *h1 = new TH1D("h1", "conv_e", 100,0, 80);

  // 遍历树的每一个条目
  for (int i = 0; i < t->GetEntries(); i++) {
    t->GetEntry(i);

    // 检查conv_e不为空
    std::cout << "conv_e= " << conv_e <<std::endl;
    if (conv_e != 0) {
	std::cout << "conv_e= " << conv_e <<std::endl;
      // 将每个光子数填入直方图
      h1->Fill(conv_e);
    } else {}
  }

  // 设置Y轴的范围
   h1->GetYaxis()->SetRangeUser(0, h1->GetMaximum() * 1.1); // 设置Y轴范围为0到最大值的1.1倍
  // h1->GetYaxis()->SetRangeUser(0, 120);

  // 设置Y轴的刻度和标签
  h1->GetYaxis()->SetTitleSize(0.04); // Y轴标题字体大小
  h1->GetYaxis()->SetTitleOffset(1.2); // Y轴标题偏移量
  h1->GetYaxis()->SetLabelSize(0.04); // Y轴刻度标签字体大小
  h1->GetYaxis()->SetNdivisions(510); // Y轴刻度主分度和次分度

  // 创建画布并绘制直方图
  TCanvas *c1 = new TCanvas("c1", "conv_e", 1024, 768);
  c1->SetLogx();
  h1->Draw();
 
  TFile *fout = new TFile("Test.root","RECREATE");
  fout->cd();
  h1->Write();
  c1->SaveAs("photon_count_histogram_logx.png");
  fout->Close();
}
  // 可选: 保存画布为图片i
  // c1->SaveAs("photon_count_histogram.png");
