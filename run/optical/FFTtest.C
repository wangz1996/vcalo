#include "TH1D.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TRandom3.h"

void FFTtest(){
  int array_size=10000;
  
  Double_t *ECALWave = new Double_t[array_size+1];
  Double_t *Re1 = new Double_t[array_size/2+1];
  Double_t *Im1 = new Double_t[array_size/2+1];

  for(Int_t i=0; i<array_size+1; i++){
     //if(i==100) ECALWave[i]=100;
     if(i>=100&&i<=1200) ECALWave[i]=100;
     //if(i==1000) ECALWave[i]=100;
     //if(i==45000) ECALWave[i]=100;
     //else if(i==1000) ECALWave[i]=150;
     else ECALWave[i]=0;
   }
  
  TVirtualFFT *fft_ETDis = TVirtualFFT::FFT(1, &array_size, "R2C ES K");
  if (!fft_ETDis) {
     cout<<"EnergyTimeDistribution FFT Failed"<<endl;
     ::abort();
  }
  fft_ETDis->SetPoints(ECALWave);
  fft_ETDis->Transform();
  fft_ETDis->GetPointsComplex(Re1,Im1);



  //CR-RC-RC Impulse Response Funtion FFT
  Double_t *IRF = new Double_t[array_size+1];
  Double_t *Re2 = new Double_t[array_size/2+1];
  Double_t *Im2 = new Double_t[array_size/2+1];

  TF1 *f1 = new TF1("f1","gaus",-250,250);
  f1->SetParameters(1,0,1);

  for(Int_t i=0;i<array_size/2;i++){
     IRF[i]=f1->Eval((1000.0/array_size)*i-250);
     //IRF[i]=f1->Eval((1000.0/array_size)*i);
  }
  for(Int_t i=array_size/2;i<array_size+1;i++)
    IRF[i]=0;

 

  TVirtualFFT *fft_IRF = TVirtualFFT::FFT(1, &array_size, "R2C ES K");
  if (!fft_IRF) {
     cout<<" Impulse Response Funtion FFT Failed"<<endl;
     ::abort();
  }
  fft_IRF->SetPoints(IRF);
  fft_IRF->Transform();
  fft_IRF->GetPointsComplex(Re2,Im2);

  //Convolution
  Double_t *Re3 = new Double_t[array_size/2+1];
  Double_t *Im3 = new Double_t[array_size/2+1];
  for(Int_t i=0;i<array_size/2+1;i++){
    Re3[i]=Re1[i]*Re2[i]-Im1[i]*Im2[i];
    Im3[i]=Re1[i]*Im2[i]+Im1[i]*Re2[i];
  }

  //Inverse FFT
  //TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &array_size, "C2R M K");
  TVirtualFFT *fft_back = TVirtualFFT::FFT(1, &array_size, "C2R ES K");
  fft_back->SetPointsComplex(Re3,Im3);
  fft_back->Transform();
  fft_back->GetPoints(ECALWave);
  
  for(Int_t i=0;i<array_size;i++){
    ECALWave[i]=ECALWave[i]/array_size;
    //if(ECALWave[i]>1.0E-12)cout<<i<<":"<<ECALWave[i]<<endl;
  }


  TH1D* h1 = new TH1D("h1","EnergyDep",array_size,0,1000);
  TH1D* h2 = new TH1D("h2","EnergyDep",100,-5,5);
  
   for(Int_t i=0;i<array_size;i++){
        h1->SetBinContent(i+1,ECALWave[i]);
 
  }

  h1->Draw();
 


  //h2->Draw();

  delete fft_ETDis;
  delete fft_IRF;
  delete fft_back;
  delete [] IRF; IRF=NULL;
  delete [] Re3; Re3=NULL;
  delete [] Im3; Im3=NULL;
  delete [] Re2; Re2=NULL;
  delete [] Im2; Im2=NULL;
  delete [] Re1; Re1=NULL;
  delete [] Im1; Im1=NULL;
}
