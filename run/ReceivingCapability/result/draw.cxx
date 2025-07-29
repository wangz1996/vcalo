void draw(){
	ROOT::RDataFrame df("vtree","test.root");
	//TH1D *h_trigger = new TH1D("h_trigger","Trigger",1,0,1);
	//TH1D *h_total = new TH1D("h_total","Total",1,0,1);
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
	.Filter("ecal_celle.size()==1","passECAL");
	dff.Report()->Print();
	//df.Foreach([&h_total](){h_total->Fill(0);});
	//dff.Foreach([&h_trigger](){h_trigger->Fill(0);});
	//TEfficiency *he = new TEfficiency(*h_trigger,*h_total);
	//std::cout<<*dff.Count()<<" / "<<*df.Count()<<" = "<<he->GetEfficiency(1)<<" "<<he->GetEfficiencyErrorUp(1)<<" "<<he->GetEfficiencyErrorLow(1)<<std::endl;
	//.Filter("");
}
