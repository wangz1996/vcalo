void draw(){
	ROOT::RDataFrame df("vtree","test_new.root");
	std::unordered_map<int,TH1D*> umap_hist;
	for(int i=0;i<25;i++){
		umap_hist[i] = new TH1D(TString::Format("h_%d",i),"energy",100,0,300);
	}
	auto dff = df.Filter("slope_xz>-0.3 && slope_xz<-0.2")
	.Filter("slope_yz>-0.3 && slope_yz<-0.2");
	dff.Foreach([&umap_hist](const ROOT::VecOps::RVec<float>& ecal_celle, const ROOT::VecOps::RVec<int>& ecal_cellid){
		umap_hist[ecal_cellid[0]]->Fill(ecal_celle[0]);
	},{"ecal_celle","ecal_cellid"});
	auto c=new TCanvas("c","test",1500,1500);
	c->Divide(5,5);
	for(int i=0;i<25;i++){
		c->cd(i+1);
		umap_hist[i]->Draw();
	}
	c->SaveAs("cut.png");
	//auto fout=new TFile("hist.root","RECREATE");

}
