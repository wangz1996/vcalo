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
  vTree->Branch("ecal_cellid",               &ecal_cellid);
  vTree->Branch("ecal_celle",               &ecal_celle);
  vTree->Branch("ecal_e",					&ecal_e);
	vTree->Branch("ecal_nphoton",			&ecal_nphoton);
	vTree->Branch("ecal_optime",			&ecal_optime);
	fSaveGeo = savegeo;
}

void HistoManager::fill(){
	for(size_t i=0;i<25;i++){
		ecal_cellid.emplace_back(i);
		ecal_celle.emplace_back(ecal_mape.at(i));
		ecal_e+=ecal_mape.at(i);
		ecal_nphoton.emplace_back(ecal_npmap.at(i));
	}
	vTree->Fill();
}

void HistoManager::fillEcalHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid){
	ecal_mape[copyNo]+=edep;
}

void HistoManager::npup(const std::string& name){
	std::string npname = name.substr(name.find_first_of("_")+1);
	int npid = std::stoi(npname);
	ecal_npmap[npid]++;
}

void HistoManager::clear(){
	std::vector<int>().swap(ecal_cellid);
	std::vector<float>().swap(ecal_celle);
	std::vector<int>().swap(ecal_nphoton);
	std::vector<float>().swap(ecal_optime);
	ecal_mape.clear();
	ecal_npmap.clear();
	for(size_t i=0;i<25;i++){
		ecal_mape[i]=0.;
		ecal_npmap[i]=0;
	}
	ecal_e=0.;
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
