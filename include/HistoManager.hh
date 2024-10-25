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
// $Id: HistoManager.hh 68017 2013-03-13 13:29:53Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#ifndef HistoManager_h
#define HistoManager_h

#include "globals.hh"
#include <TRandom3.h>
#include "TSystem.h"
#include "TGeoManager.h"
#include "TMath.h"
#include "TRandom.h"
#include "Config.hh"
#include <vector>
#include <G4ThreeVector.hh>
#include <unordered_map>
#include <filesystem>

class TTree;
class TFile;
class Config;
//const G4int kMAXTrack=5000;//should be checked!!!
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class HistoManager
{
	public:
		static HistoManager& getInstance();
		void save();
		void clear();
		void book(const std::string& foutname,const bool &savegeo);
		void fill(const int& _eventNo);
		void npup(const std::string& name);
		void filloptime(const float& time){ecal_optime.emplace_back(time);}
		void fillEcalHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid);
		void fillAPDHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid);
		void fillPrimary(const G4Track* trk);
		void addConvPhoton(){++nConvPhoton;}
		void addTotalOptPhoton(){++nTotalOptPhoton;}
		void fillConvTime(const float& time){ecal_convtime.emplace_back(time);}
		void bindConfig(Config* c){config=c;}
		void changeStatusCode(const int& code){StatusCode = code;}
		int getStatusCode(){return StatusCode;}
	private:
		//Singleton
		HistoManager();
		~HistoManager();
		Double_t SiPMDigi(const Double_t &edep) const;
		
		HistoManager(const HistoManager&)=delete;

		const HistoManager &operator=(const HistoManager&)=delete;

		G4bool    fSaveGeo;
		G4String fOutName;

		TFile* vFile;
		TTree* vTree;

		Config* config;

		int StatusCode;

	private:
		int eventNo;
		int nConvPhoton;
		int nTotalOptPhoton;
		float init_x;
		float init_y;
		float init_z;
		float init_Px;
		float init_Py;
		float init_Pz;
		float init_E;
		float init_Ke;
		std::vector<int> ecal_cellid;
		std::vector<float> ecal_celle;
		std::vector<float> apd_celle;
		std::vector<int> ecal_nphoton;
		std::vector<float> ecal_optime;
		std::vector<float> ecal_convtime;
		std::map<int,float> ecal_mape;
		std::map<int,float> apd_mape;
		std::map<int,int> ecal_npmap;
		float ecal_e;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

