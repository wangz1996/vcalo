void draw(){
	ROOT::RDataFrame df("vtree","test.root");
	TFile *fout=new TFile("hist.root","RECREATE");
	TH1D *h1=new TH1D("h1","MC",100,10,100);
	df.Foreach([h1](std::vector<float>& celle){
		for(auto i:celle)h1->Fill(i);
	},{"ecal_celle"});
	fout->cd();
	h1->Write();
	fout->Close();
}
