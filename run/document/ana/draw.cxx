void draw(){
	std::ifstream data("list.txt");
	std::string fname;
	std::map<int,float> map_e_emin;
	std::map<int,float> map_e_emax;
	TGraph *gmin=new TGraph();
	TGraph *gmax=new TGraph();
	gmin->SetTitle("Min Energy");
	gmin->SetName("gmin");
	gmax->SetTitle("Max Energy");
	gmax->SetName("gmax");
	int index=0;
	TFile *fout=new TFile("hist.root","RECREATE");
	while(data>>fname){
		if(fname=="")continue;
		std::string energy = fname;
		energy = energy.substr(energy.find("jobs")+5);
		energy = energy.substr(0,energy.find_first_of('/'));
		int e = std::stoi(energy);
		auto f = std::unique_ptr<TFile>(static_cast<TFile*>(TFile::Open(TString(fname),"READ")));
		auto t = static_cast<TTree*>(f->Get("vtree"));
		std::vector<float> *ecal_celle=nullptr;
		t->SetBranchAddress("ecal_celle",&ecal_celle);
		TH1D *hmin=new TH1D(Form("hmin_%d",e),"min",100,0,50);
		TH1D *hmax=new TH1D(Form("hmax_%d",e),"max",100,0,100);
		float global_min=9999.,global_max=-10.;
		for(int i=0;i<t->GetEntries();i++){
			t->GetEntry(i);
			float emin=9999.,emax=-10.;
			for(auto ene:*ecal_celle){
				if(ene<=3.)continue;
				emin = emin<ene ? emin : ene;
				emax = emax>ene ? emax : ene;
			}
			global_min = emin<global_min ? emin : global_min;
			global_max = emax>global_max ? emax : global_max;

			//if(emin<100.)hmin->Fill(emin);
			//if(emax>0.)hmax->Fill(emax);
		}
		map_e_emin[e]=global_min;
		map_e_emax[e]=global_max;
		//gmin->SetPoint(index,e,global_min);
		//gmax->SetPoint(index,e,global_max);
		//gmin->SetPoint(index,e,hmin->GetMean());
		//gmax->SetPoint(index,e,hmax->GetMean());
		//fout->cd();
		//hmax->Write();
		//hmin->Write();
	}
	for(auto i:map_e_emin){
		gmin->SetPoint(index,i.first,i.second);
		gmax->SetPoint(index,i.first,map_e_emax[i.first]);
		index++;
	}
	fout->cd();
	gmin->Write();
	gmax->Write();
	fout->Close();
}
