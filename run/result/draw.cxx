void draw(){
	auto f = std::unique_ptr<TFile>(TFile::Open("size.root","RECREATE"));
	TH1D *h1=new TH1D("h1","test",100,0,500);
	ROOT::RDataFrame df("vtree","test_4.root");
	df.Foreach([h1](const std::vector<float>& tracker_hitx){
		h1->Fill(tracker_hitx.size());
	},{"tracker_hitx"});
	f->cd();
	h1->Write();
	
}
