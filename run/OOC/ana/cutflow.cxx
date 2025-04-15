void cutflow(){
	//ROOT::RDataFrame df("vtree","/mnt2/VLASTP/calibration/simulation/ooc_ecal_filter.root");
	ROOT::RDataFrame df("vtree","/home/wangzhen/mnt2/VLASTP/vcalo/run/OOC/result/ooc_plane_z.root");
	auto dff = df
	.Filter([](const std::vector<float>& acd_e,const std::vector<float>& conv_e,const std::vector<float>& tracker_hite,const std::vector<float>& ecal_celle){
		bool pass=false;
		pass = (acd_e[0]>0.6 && acd_e[1]>0.6);
		if(!pass)return pass;
		pass = pass && (conv_e[0]>2.52 || conv_e[1]>2.52 || conv_e[2]>2.52 || conv_e[3]>2.52);
		if(!pass)return pass;
		pass = pass && std::any_of(ecal_celle.begin(),ecal_celle.end(),[](float x){return x>36.;});
		return pass;
	},{"acd_e","conv_e","tracker_hite","ecal_celle"},"Trigger")
	.Filter("ecal_celle.size()==1","passECAL")
	.Define("max_ecale",[](const std::vector<float>& ecal_celle){
		float max_ecale=0.;
		max_ecale = *max_element(ecal_celle.begin(),ecal_celle.end());
		return max_ecale;
	},{"ecal_celle"})
	;
	//dff.Report()->Print();
	auto h1 = dff.Histo1D("max_ecale");
	h1->SetName("hmax");
	TFile *fout=new TFile("hist_plane.root","RECREATE");
	fout->cd();
	h1->Write();
	fout->Close();
}
