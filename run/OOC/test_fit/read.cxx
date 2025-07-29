void read(){
	TFile *fout=new TFile("hist.root","RECREATE");
	TGraph *g=new TGraph();
	TH1D *h=new TH1D("h","test",1000,1e-2,2e2);
	g->SetName("g");
	g->SetTitle("title");
	std::ifstream data("flux.csv");
	float x,y;
	int idx=0;
	while(data>>x>>y){
		g->SetPoint(idx,x,y);
		idx++;
	}
	for(int i=0;i<h->GetNbinsX();i++){
		float x=h->GetXaxis()->GetBinCenter(i+1);
		float y=g->Eval(x);
		h->SetBinContent(i+1,y);
	}
	fout->cd();
	g->Write();
	h->Write();
	fout->Close();
	

}
