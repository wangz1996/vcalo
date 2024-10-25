//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: HistoManager.cc 72249 2013-07-12 08:57:49Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "HistoManager.hh"
#include "TGeoManager.h"
#include "G4UnitsTable.hh"
#include <TTree.h>
#include <TFile.h>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager& HistoManager::getInstance(){
	static HistoManager HM;
	return HM;
}

HistoManager::HistoManager():vFile(0),vTree(0){
}

void HistoManager::book(const std::string& foutname,const bool &savegeo)
{
  G4cout<<"---->Creating Instance for HistoManager..."<<G4endl;
  vFile = new TFile(TString(foutname),"RECREATE");
  vTree = new TTree("vtree","calo events");
  vTree->Branch("eventNo",					&eventNo);
  vTree->Branch("ecal_cellid",               &ecal_cellid);
  vTree->Branch("ecal_celle",               &ecal_celle);
  vTree->Branch("ecal_e",					&ecal_e);
	vTree->Branch("ecal_nphoton",			&ecal_nphoton);
	vTree->Branch("ecal_optime",			&ecal_optime);
	vTree->Branch("init_x",					&init_x);
	vTree->Branch("init_y",					&init_y);
	vTree->Branch("init_z",					&init_z);
	vTree->Branch("init_Px",					&init_Px);
	vTree->Branch("init_Py",					&init_Py);
	vTree->Branch("init_Pz",					&init_Pz);
	vTree->Branch("init_E",					&init_E);
	vTree->Branch("init_Ke",                &init_Ke);
	vTree->Branch("nConvPhoton",			&nConvPhoton);
	vTree->Branch("ecal_convtime",			&ecal_convtime);
	vTree->Branch("nTotalOptPhoton",		&nTotalOptPhoton);
	vTree->Branch("apd_celle",				&apd_celle);
	fSaveGeo = savegeo;
}

void HistoManager::fill(const int& _eventNo){
	TRandom3 *rand = new TRandom3();
	for(auto i:ecal_mape){
		float ecell = i.second;
		float apdcell = apd_mape[i.first];
		ecell = ecell + rand->Gaus(0,config->conf["ECAL"]["Crystal-Nonuniformity"].as<double>()*ecell);
		if(config->conf["ECAL"]["light-yield-effect"].as<bool>()){
			double yield = ecell*1000.; //pe
			double apdyield = apdcell*1e6/3.6;
			yield = yield + rand->Gaus(0,TMath::Sqrt(yield)) 
						+ (config->conf["ECAL"]["E-Noise"].as<bool>() ? rand->Gaus(0,1000.) : 0.)
						+ (config->conf["ECAL"]["APD-Ionisation"].as<bool>() ? rand->Gaus(apdyield,TMath::Sqrt(apdyield)) : 0.);
			ecell = yield/1000.; //MeV
		}
		ecal_cellid.emplace_back(i.first);
		ecal_celle.emplace_back(ecell);
		apd_celle.emplace_back(apdcell);
		ecal_e+=ecell;
	}
	eventNo = _eventNo;
	vTree->Fill();
}

void HistoManager::fillEcalHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid){
	// ecal_e+=edep;
	ecal_mape[copyNo]+=edep;
}

void HistoManager::fillAPDHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid){
	// ecal_e+=edep;
	apd_mape[copyNo]+=edep;
}

void HistoManager::npup(const std::string& name){
	std::string npname = name.substr(name.find_first_of("_")+1);
	int npid = std::stoi(npname);
	ecal_npmap[npid]++;
}

void HistoManager::fillPrimary(const G4Track* trk){
	init_x = trk->GetPosition().x();
	init_y = trk->GetPosition().y();
	init_z = trk->GetPosition().z();
	init_Px = trk->GetMomentumDirection().x();
	init_Py = trk->GetMomentumDirection().y();
	init_Pz = trk->GetMomentumDirection().z();
	init_E = trk->GetTotalEnergy();
	init_Ke = trk->GetKineticEnergy();
}

void HistoManager::clear(){
	StatusCode = 1;
	std::vector<int>().swap(ecal_cellid);
	std::vector<float>().swap(ecal_celle);
	std::vector<int>().swap(ecal_nphoton);
	std::vector<float>().swap(ecal_optime);
	std::vector<float>().swap(ecal_convtime);
	ecal_mape.clear();
	apd_mape.clear();
	ecal_npmap.clear();
	ecal_e=0.;
	init_x=0.;
	init_y=0.;
	init_z=0.;
	init_Px=0.;
	init_Py=0.;
	init_Pz=0.;
	init_E=0.;
	init_Ke=0.;
	nConvPhoton=0;
	nTotalOptPhoton=0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::save()
{
	vFile->cd();
	if(fSaveGeo)
	{
		gSystem->Load("libGeom");
		TGeoManager::Import("vcalo.gdml");
		gGeoManager->Write("vcalo");
		//std::remove("vcalo.gdml");
	}
	vTree->Write("",TObject::kOverwrite);
	vFile->Close();
	G4cout<<"------>close rootfile"<<G4endl;
}

Double_t HistoManager::SiPMDigi(const Double_t &edep) const
{
	Int_t sPix = 0;
	sPix = gRandom->Poisson(edep / 0.466 * 20);
	sPix = 7396.* (1 - TMath::Exp(-sPix / 7284.));
	Double_t sChargeOutMean = sPix * 29.4;
	Double_t sChargeOutSigma = sqrt(sPix * 5 * 5 + 3 * 3);
	Double_t sChargeOut = -1;
	while(sChargeOut < 0)
		sChargeOut = gRandom->Gaus(sChargeOutMean, sChargeOutSigma);
	Double_t sAdc = -1;
	while(sAdc < 0)
		sAdc = gRandom->Gaus(sChargeOut,.0002 * sChargeOut);
	Double_t sMIP =sAdc / 29.4 / 20;
	if (sMIP < 0.5)
		return 0;
	return sMIP * 0.466;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
