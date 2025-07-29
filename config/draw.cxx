#include <boost/container/small_vector.hpp>
#include <iostream>
#include <vector>
using SpacePoint = boost::container::small_vector<float,3>;
using SpacePoints = std::vector<SpacePoint>;
void draw(){
    auto fout = std::unique_ptr<TFile>(TFile::Open("hist.root","RECREATE"));
    TH1D *hRad = new TH1D("hRad","Rad",100,0.,1.57);
    TH1D *hDeg = new TH1D("hDeg","Deg",100,0.,90.);
    ROOT::RDataFrame df("tout","vana.root");
    auto dff = df.Define("angleRad",[](const float& init_x,const float& init_y,const float& init_z,
                            const float& init_Px,const float& init_Py, const float& init_Pz,
                            const std::vector<float>& reco_x,const std::vector<float>& reco_y,const std::vector<float>& reco_z,
                            const std::vector<float>& reco_Px,const std::vector<float>& reco_Py, const std::vector<float>& reco_Pz){
                                TLorentzVector vec1,vec2;
                                vec1.SetPxPyPzE(init_Px,init_Py,init_Pz,1000);
                                vec2.SetPxPyPzE(reco_Px[0],reco_Py[0],reco_Pz[0],1000);
                                auto dir1 = vec1.Vect();
                                auto dir2 = vec2.Vect();
                                return dir1.Angle(dir2);
                            },
                            {"init_x","init_y","init_z","init_Px","init_Py","init_Pz","reco_x","reco_y","reco_z","reco_Px","reco_Py","reco_Pz"})
    .Define("angleDeg","angleRad * 180. / TMath::Pi();");
    auto hr = dff.Histo1D(*hRad,"angleRad");
    auto hd = dff.Histo1D(*hDeg,"angleDeg");
    fout->cd();
    hr->Write("hr");
    hd->Write("hd");
}