void save(TString name = "ECALShield.json"){
	auto f = TFile::Open("test.root","READ");
	auto geo = (TGeoManager*)f->Get("vcalo");
	geo->SaveAs(name);
}
