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
	rand = new TRandom3();
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
  vTree->Branch("conv_e",					&conv_e);
	vTree->Branch("apd_nphoton",			&apd_nphoton);
	vTree->Branch("apd_optime",			&apd_optime);
	vTree->Branch("init_x",					&init_x);
	vTree->Branch("init_y",					&init_y);
	vTree->Branch("init_z",					&init_z);
	vTree->Branch("init_Px",					&init_Px);
	vTree->Branch("init_Py",					&init_Py);
	vTree->Branch("init_Pz",					&init_Pz);
	vTree->Branch("init_E",					&init_E);
	vTree->Branch("init_Ke",                &init_Ke);
	vTree->Branch("conve_kinematic",		&conve_kinematic);
	vTree->Branch("convp_kinematic",		&convp_kinematic);
	vTree->Branch("nConvPhoton",			&nConvPhoton);
	vTree->Branch("ecal_convtime",			&ecal_convtime);
	vTree->Branch("nTotalOptPhoton",		&nTotalOptPhoton);
	vTree->Branch("apd_celle",				&apd_celle);
	vTree->Branch("isconv",					&isconv);
	vTree->Branch("conve_inECAL",           &conve_inECAL);
	vTree->Branch("convp_inECAL",           &convp_inECAL);
	vTree->Branch("tracks",                 &tracks);
	vTree->Branch("tracker_hitx",            &tracker_hitx);
	vTree->Branch("tracker_hity",            &tracker_hity);
	vTree->Branch("tracker_hitz",            &tracker_hitz);
	vTree->Branch("tracker_hite",            &tracker_hite);
	vTree->Branch("tracker_ephite",			 &tracker_ephite);
	fSaveGeo = savegeo;
}

void HistoManager::fill(const int& _eventNo){
	for(auto i:ecal_mape){
		float ecell = i.second;
		float apdcell = apd_mape[i.first];
		if(config->conf["ECAL"]["Crystal-Nonuniformity"].as<double>()!=0.0){
			// std::cout<<"Non-uniformity fluctuation"<<std::endl;
			ecell = ecell + rand->Gaus(0,config->conf["ECAL"]["Crystal-Nonuniformity"].as<double>()*ecell);
		}
		
		if(config->conf["ECAL"]["light-yield-effect"].as<bool>() && ecell>0.){
			// std::cout<<"Light yield effect"<<std::endl;
			
			double yield = ecell*1000.; //pe
			double apdyield = apdcell*1e6/3.6;
			yield = yield + rand->Gaus(0,TMath::Sqrt(yield)) 
						+ (config->conf["ECAL"]["E-Noise"].as<bool>() ? rand->Gaus(0,1000.) : 0.)
						+ (config->conf["ECAL"]["APD-Ionisation"].as<bool>() && apdyield>0. ? rand->Gaus(apdyield,TMath::Sqrt(apdyield)) : 0.);
			ecell = yield/1000.; //MeV
		}
		if(config->conf["ECAL"]["Digi"].as<bool>()){
			ecell = ecell*0.999841;
		}
		ecell = ecell > 3. ? ecell : 0.;
		ecal_cellid.emplace_back(i.first);
		ecal_celle.emplace_back(ecell);
		apd_celle.emplace_back(apdcell);
		ecal_e+=ecell;
	}
	if(config->conf["Converter"]["Crystal-Nonuniformity"].as<double>()!=0.0){
		conv_e = conv_e + rand->Gaus(0,config->conf["Converter"]["Crystal-Nonuniformity"].as<double>()*conv_e);
	}
	if(config->conf["Converter"]["light-yield-effect"].as<bool>() && conv_e>0.){
		double yield = conv_e*2000.; //pe
		yield = yield + rand->Gaus(0,TMath::Sqrt(yield)) 
						+ (config->conf["Converter"]["E-Noise"].as<bool>() ? rand->Gaus(0,1000.) : 0.);
		conv_e = yield/2000.; //MeV
	}
	conv_e = conv_e > 3. ? conv_e : 0.;
	eventNo = _eventNo;
	// std::cout<<"Ntracks: "<<map_track.size()<<std::endl;
	for(auto i:map_track){
		tracks.Add(i.second);
	}

	//Tracker hit merge
	for(size_t i=0;i<6;i++){
		//Loop over even layers to add digitized hits
		if(i%2==1)continue; //Odd layers are skipped
		auto hits_per_layer = tracker_hitmap[i];
		auto hits_next_layer = tracker_hitmap[i+1];
		if(hits_per_layer.size()==0 || hits_next_layer.size()==0)continue;
		for(auto &ehit:hits_per_layer){ //event layer hits
			for(auto &ohit:hits_next_layer){ //odd layer hits
				createHit(ehit,ohit,i);
			}
		}
	}
	vTree->Fill();
}

void HistoManager::fillEcalHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid){
	// ecal_e+=edep;
	ecal_mape[copyNo]+=edep;
}

void HistoManager::createHit(const TrackerHit& ehit,const TrackerHit& ohit,const size_t& layer){
	// std::cout<<ehit[0]<<" "<<ehit[1]<<" "<<ohit[0]<<" "<<ohit[1]<<std::endl;
	float ex = StripL[ehit[0]];
	float ey = StripT<float>[ehit[1]];
	float ox = StripT<float>[ohit[0]];
	float oy = StripL[ohit[1]];
	float z = 0.5*(TrackerPosZ[layer]+TrackerPosZ[layer+1]);
	if(ey>oy-47.5 && ey<oy+47.5 && ox>ex-47.5 && ox<ex+47.5){
		tracker_hitx.emplace_back(ox);
		tracker_hity.emplace_back(ey);
		tracker_hitz.emplace_back(z);
	}
}

void HistoManager::fillTrackerHit(const int& tracker_id, const float& posX, const float& posY, const float& edep, const int& trkid){
	//x starts at -141.4 mm
	//y starts at -95. mm
	int xid = (tracker_id%2==0) ? round((posX+95.)/95.) : round((posX+141.4)/0.121);
	int yid = (tracker_id%2==0) ? round((posY+141.4)/0.121) : round((posY+95.)/95.);
	int zid = tracker_id;
	if(xid<0 || yid<0 || zid<0){
		return;
	}
	TrackerHit hit{xid,yid};
	tracker_hitmap[zid].insert(std::move(hit));
}

void HistoManager::fillAPDHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid){
	// ecal_e+=edep;
	apd_mape[copyNo]+=edep;
}

void HistoManager::fillConvElectron(const G4Track* trk){
	//Kinematic x y z px py pz E theta phi Ke
	G4ThreeVector position = trk->GetPosition();
	conve_kinematic.emplace_back(position.x());
	conve_kinematic.emplace_back(position.y());
	conve_kinematic.emplace_back(position.z());
	auto momentum = trk->GetMomentum();
	conve_kinematic.emplace_back(momentum.x());
	conve_kinematic.emplace_back(momentum.y());
	conve_kinematic.emplace_back(momentum.z());
	conve_kinematic.emplace_back(trk->GetTotalEnergy());
	conve_kinematic.emplace_back(trk->GetMomentumDirection().theta());
	conve_kinematic.emplace_back(trk->GetMomentumDirection().phi());
	conve_kinematic.emplace_back(trk->GetKineticEnergy());
}

void HistoManager::fillConvPositron(const G4Track* trk){
	//Kinematic x y z px py pz E theta phi Ke
	G4ThreeVector position = trk->GetPosition();
	convp_kinematic.emplace_back(position.x());
	convp_kinematic.emplace_back(position.y());
	convp_kinematic.emplace_back(position.z());
	auto momentum = trk->GetMomentum();
	convp_kinematic.emplace_back(momentum.x());
	convp_kinematic.emplace_back(momentum.y());
	convp_kinematic.emplace_back(momentum.z());
	convp_kinematic.emplace_back(trk->GetTotalEnergy());
	convp_kinematic.emplace_back(trk->GetMomentumDirection().theta());
	convp_kinematic.emplace_back(trk->GetMomentumDirection().phi());
	convp_kinematic.emplace_back(trk->GetKineticEnergy());
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

void HistoManager::fillTracks(const int& track_id,const G4ThreeVector& pos,const int& color){
	if(map_track.count(track_id)==0){
		map_track.insert(std::pair<int,TPolyLine3D*>(track_id,new TPolyLine3D()));
		if(color!=0)map_track[track_id]->SetLineColor(color);
	}
	map_track[track_id]->SetNextPoint(pos.x()/10.,pos.y()/10.,pos.z()/10.);
}

void HistoManager::clear(){
	StatusCode = 1;
	std::vector<int>().swap(ecal_cellid);
	std::vector<float>().swap(ecal_celle);
	std::vector<float>().swap(apd_optime);
	std::vector<float>().swap(apd_celle);
	std::vector<float>().swap(ecal_convtime);
	std::vector<float>().swap(conve_kinematic);
	std::vector<float>().swap(convp_kinematic);
	std::vector<float>().swap(tracker_hitx);
	std::vector<float>().swap(tracker_hity);
	std::vector<float>().swap(tracker_hitz);
	std::vector<float>().swap(tracker_hite);
	std::vector<float>().swap(tracker_ephite);
	apd_nphoton=0;
	isconv=0;
	conve_inECAL=0;
	convp_inECAL=0;
	ecal_mape.clear();
	apd_mape.clear();
	ecal_npmap.clear();
	map_track.clear();
	tracker_hitmap.clear();
	ecal_e=0.;
	conv_e=0.;
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
	tracks.Clear();
	tracks.Delete();
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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<class TT=std::any>
constexpr std::array<float,2404> HistoManager::StripT = Temp_StripT<float,2404>();