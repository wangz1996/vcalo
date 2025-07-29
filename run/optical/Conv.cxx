#include <TH1.h>
#include <TFFT.h>
#include <TMath.h>
#include <TCanvas.h>

void Conv() {
    // 创建两个示例直方图
    TH1D *h1 = new TH1D("h1", "Histogram 1", 100, -5, 5);
    TH1D *h2 = new TH1D("h2", "Histogram 2", 100, -5, 5);

    // 填充直方图，例如使用高斯分布填充
    h1->FillRandom("gaus", 10000);
    h2->FillRandom("gaus", 10000);

    // 获取直方图的数组表示
    int n = h1->GetNbinsX();
    Double_t *arr1 = new Double_t[n];
    Double_t *arr2 = new Double_t[n];
    for (int i = 1; i <= n; i++) {
        arr1[i-1] = h1->GetBinContent(i);
        arr2[i-1] = h2->GetBinContent(i);
    }

    // 执行傅立叶变换
    TFFTComplex *fft = TFFT::FFT(1, &n, "R2C");
    fft->SetPoints(arr1);
    fft->Transform();
    TFFTComplex *fft2 = TFFT::FFT(1, &n, "R2C");
    fft2->SetPoints(arr2);
    fft2->Transform();

    // 计算卷积：将两个频域表示相乘
    for (int i = 0; i < n; i++) {
        fft->GetPointComplex(i)[0] *= fft2->GetPointComplex(i)[0];
        fft->GetPointComplex(i)[1] *= fft2->GetPointComplex(i)[1];
    }

    // 反傅立叶变换得到卷积结果
    fft->Transform("C2R");
    Double_t *conv_result = fft->GetPointsReal();

    // 将结果填充到新的直方图
    TH1D *h_conv = new TH1D("h_conv", "Convolution", 100, -5, 5);
    for (int i = 1; i <= n; i++) {
        h_conv->SetBinContent(i, conv_result[i-1]);
    }

    // 绘制结果
    TCanvas *c1 = new TCanvas("c1", "Convolution of Histograms", 800, 600);
    h1->SetLineColor(kRed);
    h2->SetLineColor(kBlue);
    h_conv->SetLineColor(kGreen);

    h1->Draw();
    h2->Draw("SAME");
    h_conv->Draw("SAME");
}
