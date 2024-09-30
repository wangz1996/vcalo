void draw(){

	TH1D *htemp = new TH1D("htemp","ECAL_E",125,0,125);
	//auto f=std::unique_ptr<TFile>(TFile::Open("vcalo_shield.root","READ"));
	//auto t=std::unique_ptr<TTree>(static_cast<TTree*>(f->Get("")));
	auto df=new ROOT::RDataFrame("vtree","vcalo_shield.root");
	auto h_shield = df->Histo1D(*htemp,"ecal_e");
	auto df2=new ROOT::RDataFrame("vtree","vcalo_noshield.root");
	auto h_noshield = df2->Histo1D(*htemp,"ecal_e");
	auto fout=new TFile("hist.root","RECREATE");
	fout->cd();
	h_shield->Write("hs");
	h_noshield->Write("hn");
	fout->Close();
}
