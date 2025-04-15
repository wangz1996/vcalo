void test(){
	ROOT::RDataFrame df("vtree","/mnt2/VLASTP/calibration/simulation/ooc_ecal_filter.root");
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
	.Define("slope_xz",[](vector<float>& tx,vector<float>& ty,vector<float>& tz)
	{
		TH2D *hH = new TH2D("hh","Hough",100,0.,3.14,50,-300,300);
	    for(int i=0;i<tx.size();i++){
	    	for(float theta=0.;theta<3.14;theta+=0.03){
			float rho = tz[i]*TMath::Cos(theta) + tx[i]*TMath::Sin(theta);
			hH->Fill(theta,rho);
		}
	    }
	// 获取最大投票数的 bin
    Int_t maxBin = hH->GetMaximumBin();
    Int_t binX, binY, binZ;
    hH->GetBinXYZ(maxBin, binX, binY, binZ);

    // 获取对应的 θ 和 ρ 值
    Double_t maxTheta = hH->GetXaxis()->GetBinCenter(binX);
    Double_t maxRho = hH->GetYaxis()->GetBinCenter(binY);
	Double_t a = -TMath::Cos(maxTheta) / TMath::Sin(maxTheta);
    Double_t b = maxRho / TMath::Sin(maxTheta);
    	delete hH;
	return a;
	},{"tracker_hitx","tracker_hity","tracker_hitz"})
	.Define("slope_yz",[](vector<float>& tx,vector<float>& ty,vector<float>& tz)
	{
		TH2D *hH = new TH2D("hh","Hough",100,0.,3.14,50,-300,300);
	    for(int i=0;i<tx.size();i++){
	    	for(float theta=0.;theta<3.14;theta+=0.03){
			float rho = tz[i]*TMath::Cos(theta) + ty[i]*TMath::Sin(theta);
			hH->Fill(theta,rho);
		}
	    }
	// 获取最大投票数的 bin
    Int_t maxBin = hH->GetMaximumBin();
    Int_t binX, binY, binZ;
    hH->GetBinXYZ(maxBin, binX, binY, binZ);

    // 获取对应的 θ 和 ρ 值
    Double_t maxTheta = hH->GetXaxis()->GetBinCenter(binX);
    Double_t maxRho = hH->GetYaxis()->GetBinCenter(binY);
	Double_t a = -TMath::Cos(maxTheta) / TMath::Sin(maxTheta);
    Double_t b = maxRho / TMath::Sin(maxTheta);
    	delete hH;
	return a;
	},{"tracker_hitx","tracker_hity","tracker_hitz"})
	.Define("ecal_e",[](vector<float>& ecal_celle){return *max_element(ecal_celle.begin(),ecal_celle.end());},{"ecal_celle"});
	//.Filter("abs(slope)<0.5","Slope");
	//dff.Report()->Print();
	dff.Snapshot("vtree","test_new.root");

}
