void ana0(){
    ROOT::RDataFrame df("vtree","../result/ooc.root");
    // df.Foreach([](const std::vector<int>& ecal_cellid,std::vector<float>& ecal_celle){
    //     std::cout<<ecal_cellid.size()<<" "<<ecal_celle.size()<<std::endl;
    // },{"ecal_cellid","ecal_celle"});
    auto dff = df.Filter([](const float& init_x,const float& init_y,const float& init_z){
        float ss = sqrt(init_x*init_x+init_y*init_y+init_z*init_z);
        float ux = init_x/ss;
        float uy = init_y/ss;
        float uz = init_z/ss;
        return uz>0.9;
    },{"init_x","init_y","init_z"})
    .Filter([](const std::vector<float>& ecal_celle){
        return ecal_celle.size()==1;
    },{"ecal_celle"})
    .Define("ecale","ecal_celle.at(0)")
    .Define("ecalid","ecal_cellid.at(0)");
    dff.Snapshot("vtree","ooc_cut.root");
}
void ana(){
    ana0();
    TFile *fout=new TFile("hist.root","RECREATE");
    ROOT::RDataFrame df("vtree","ooc_cut.root");
    std::unordered_map<int,TH1D*> umap_id_th1d;
    df.Foreach([&](const float& ecale,const int& ecalid){
        if(umap_id_th1d.count(ecalid)==0){
            umap_id_th1d[ecalid] = new TH1D(Form("h%d",ecalid),"",100,0,500);
        }
        umap_id_th1d[ecalid]->Fill(ecale);
    },{"ecale","ecalid"});
    for(auto& [id,h]:umap_id_th1d){
        h->Write();
    }
    fout->Close();
}