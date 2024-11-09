std::vector<double> createLogBins(int nbins, double xmin, double xmax) {
    std::vector<double> bins(nbins + 1);  // 有 nbins+1 个边界
    double logMin = std::log10(xmin);
    double logMax = std::log10(xmax);
    double binWidth = (logMax - logMin) / nbins;

    for (int i = 0; i <= nbins; ++i) {
        bins[i] = std::pow(10, logMin + i * binWidth);
    }

    return bins;
}
void ana(){
	std::map<int,TH1D*> umap;
	std::map<int,float> umap_all;
	std::map<int,float> umap_conv;
	std::set<int> eset;
	auto theta_bins = createLogBins(20,1e-4,2);
	auto htemp = new TH1D("theta_temp","theta",20,theta_bins.data());
	auto dfo = ROOT::RDataFrame("vtree","test.root");
	auto df = dfo
	.Define("seed_cellid",[](const std::vector<float>& ecal_celle,const std::vector<int>& ecal_cellid){
		int id=-1;
		if(ecal_celle.size()!=0){
			int index = std::distance(ecal_celle.begin(), std::max_element(ecal_celle.begin(),ecal_celle.end()));
			id= ecal_cellid.at(index);
		}
		return id;
	},{"ecal_celle","ecal_cellid"})
	.Filter([](const int& seed_cellid){
		int id = seed_cellid;
		std::unordered_map<int,int> umap = {
		{6,1},{7,1},{8,1},{11,1},{12,1},{13,1},{16,1},{17,1},{18,1}
		};
		return umap[id]==1;
	},{"seed_cellid"})
	.Define("ecal_recoe",[](const int& seed,const std::vector<float>& ecal_celle,const std::vector<int>& ecal_cellid){
		double ecal_recoe=0.;
		std::unordered_map<int,float> umap;
		for(size_t i=0;i<ecal_celle.size();i++)umap[ecal_cellid[i]]=ecal_celle[i];
		ecal_recoe += (umap[seed-5] + umap[seed-6] + umap[seed-4]
				+ umap[seed-1] + umap[seed] + umap[seed+1]
				+ umap[seed+5] + umap[seed+4] + umap[seed+6]);
		return ecal_recoe;
	},{"seed_cellid","ecal_celle","ecal_cellid"})
	.Define("extra_in",[](const std::vector<float>& conve_kinematic,const std::vector<float>& convp_kinematic,const int& isconv){
		//x y z px py pz
		if(!isconv)return false;
		bool extra_in=false;
		float e_x=conve_kinematic.at(0);
		float e_y=conve_kinematic.at(1);
		float e_z=conve_kinematic.at(2);
		float e_px=conve_kinematic.at(3);
		float e_py=conve_kinematic.at(4);
		float e_pz=conve_kinematic.at(5);
		float p_x=convp_kinematic.at(0);
		float p_y=convp_kinematic.at(1);
		float p_z=convp_kinematic.at(2);
		float p_px=convp_kinematic.at(3);
		float p_py=convp_kinematic.at(4);
		float p_pz=convp_kinematic.at(5);
		float ecal_z = 100.;//mm
		float e_t = (ecal_z-e_z)/e_pz;
		float p_t = (ecal_z-p_z)/p_pz;
		float e_x1 = e_x + e_px*e_t;
		float e_y1 = e_y + e_py*e_t;
		float p_x1 = p_x + p_px*p_t;
		float p_y1 = p_y + p_py*p_t;
		if(abs(e_x1)<150. && abs(e_y1)<150. && abs(p_x1)<150. && abs(p_y1)<150.)extra_in=true;
		return extra_in;
	},{"conve_kinematic","convp_kinematic","isconv"})
	.Filter("extra_in")
	.Define("e_total","ecal_recoe+conv_e")
	.Define("epangle",[](const std::vector<float>& conve_kinematic,const std::vector<float>& convp_kinematic){
		//Kinematic x y z px py pz E theta phi Ke
		double epangle=-1.;
		TLorentzVector e,p;
		e.SetPxPyPzE(conve_kinematic.at(3),conve_kinematic.at(4),conve_kinematic.at(5),conve_kinematic.at(6));
		p.SetPxPyPzE(convp_kinematic.at(3),convp_kinematic.at(4),convp_kinematic.at(5),convp_kinematic.at(6));
		epangle = e.Angle(p.Vect());
		return epangle;
	},{"conve_kinematic","convp_kinematic"})
	;
	df.Foreach([&eset](float value) {
		eset.insert(int(value));
	}, {"init_E"});
	df.Foreach([&umap_all,&umap_conv](const float& e,const int& isconv){
		umap_all[e]+=1.;
		umap_conv[e]+=(isconv?1.:0.);
	},{"init_E","isconv"});
	// std::cout<<"Sel: "<<*df.Count()<<" "<<*dfo.Count()<<" eff: "<<double(*df.Count())/double(*dfo.Count())<<std::endl;
	std::vector<float> x,y;
	std::vector<float> convx,convy;
	TFile *fout=new TFile("result.root","RECREATE");
	fout->cd();
	for(auto e:eset){
		auto h = df.Filter([e=e](float init_E){return int(init_E)==e;},{"init_E"}).Histo1D("e_total");
		double xmax = h->GetBinCenter(h->GetMaximumBin());
		auto fpre = new TF1("fpre","[0]*TMath::Gaus(x,[1],[2])",0.9*xmax,1.2*xmax);
		fpre->SetParameters(1000.,xmax,sqrt(xmax));
		fpre->SetParLimits(1,10.,1.2*xmax);
		fpre->SetParLimits(2,0.5*sqrt(xmax),1.2*sqrt(xmax));
		h->Fit("fpre","sq","",0.8*xmax,1.2*xmax);
		auto f1 = new TF1("f1","[0]*ROOT::Math::crystalball_function(x, [1], [2], [3], [4])",0.2*e,1.2*e);
		f1->SetParameters(1000.,1.,1.,fpre->GetParameter(2),fpre->GetParameter(1));
		f1->SetParLimits(3,0.,1.5*h->GetRMS());
		//f1->SetParLimits(4,0.8*h->GetMean(),1.2*e);
		auto fr = h->Fit("f1","s","",0.2*e,1.2*e);
		cout<<fr->Chi2()/fr->Ndf()<<endl;
		
		double mean=f1->GetParameter(4);
		double rms=f1->GetParameter(3);
		double peak = f1->Eval(mean);
		double xLow = f1->GetX(0.5*peak, f1->GetXmin(), mean);
		double xHigh = f1->GetX(0.5*peak, mean, f1->GetXmax());
		double FWHM = xHigh-xLow;
		double rms2 = FWHM/2.35;
		x.emplace_back(mean);
		y.emplace_back(rms2/mean);
		string name="h"+to_string(e);
		h->Write(TString(name));

		//Converter efficiency
		convx.emplace_back(e);
		convy.emplace_back(umap_conv[e]/umap_all[e]);
		std::cout<<e<<" "<<umap_conv[e]<<" "<<umap_all[e]<<" "<<umap_conv[e]/umap_all[e]<<endl;
	
		//Angular information
		auto df_theta = df.Filter([e=e](const float& init_E){return int(init_E)==e;},{"init_E"})
		.Define("convetheta",[](const std::vector<float>& conve_kinematic){return conve_kinematic[7];},{"conve_kinematic"})
		.Define("convptheta",[](const std::vector<float>& convp_kinematic){return convp_kinematic[7];},{"convp_kinematic"});
		TString ename = TString("he")+TString(to_string(e));
		TString pname = TString("hp")+TString(to_string(e));
		TString epname = TString("hep")+TString(to_string(e));
		df_theta.Histo1D(*htemp,"convetheta")->Write(ename);
		df_theta.Histo1D(*htemp,"convptheta")->Write(pname);
		df_theta.Histo1D(*htemp,"epangle")->Write(epname);
	}
	TGraph *g=new TGraph(x.size(),&x[0],&y[0]);
	g->Write("hreso");
	TGraph *gconv = new TGraph(convx.size(),&convx[0],&convy[0]);
	gconv->Write("hconv");
	fout->Close();
	
}
