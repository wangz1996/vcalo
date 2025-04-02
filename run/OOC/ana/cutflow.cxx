void cutflow(){
	std::unordered_map<int,TH1D*> umap_id_th1d;
	ROOT::RDataFrame df("vtree","/home/wangzhen/mnt2/VLASTP/vcalo/run/OOC/result/ooc_plane_z.root");
	auto dff = df.Filter("ecal_celle.size()==1");
	dff.Foreach([&](const std::vector<float>& e,const std::vector<int>& id){
		if(umap_id_th1d.count(id[0])==0){
			umap_id_th1d[id[0]] = new TH1D(TString::Format("h%d",id[0]),TString::Format("h%d",id[0]),100,0,500);
		}
		umap_id_th1d[id[0]]->Fill(e[0]);
	},{"ecal_celle","ecal_cellid"});
	TFile *fout=new TFile("plane_hist.root","RECREATE");
	fout->cd();
	for(auto i:umap_id_th1d){
		i.second->Write();
	}
	fout->Close();

}
