void tracking(){
	ROOT::RDataFrame df("vtree","test.root");
	auto dff = df.Filter("isconv").Define("ntrackhits",[](const std::vector<std::vector<float>>& tracker_hitpos){
		int ntrackhits=tracker_hitpos.size();
		return ntrackhits;
	},{"tracker_hitpos"});
	auto h = dff.Histo1D("ntrackhits");
	auto he = dff.Histo1D("tracker_edep");
	TFile *fout=new TFile("test_track.root","RECREATE");
	fout->cd();
	h->Write("hntracks");
}
