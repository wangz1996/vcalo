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
#include "TList.h"
#include "TPolyLine3D.h"
#include "globals.hh"
#include <TRandom3.h>
#include "TSystem.h"
#include <TEveManager.h>
#include "TGeoManager.h"
#include <TEveGeoNode.h>
#include "TMath.h"
#include "TRandom.h"
#include "Config.hh"
#include <vector>
#include <tuple>
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
		void fillAPDOptHit(const float& time){apd_nphoton++;apd_optime.emplace_back(time);}
		void fillEcalHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid);
		void fillAPDHit(const G4int &copyNo,const G4double &edep,const G4double &time,const G4int &pdgid,const G4int &trackid);
		void fillPrimary(const G4Track* trk);
		void fillConvOptHit(const float& t){++nConvPhoton;ecal_convtime.emplace_back(t);}
		void addTotalOptPhoton(){++nTotalOptPhoton;}
		void bindConfig(Config* c){config=c;}
		void changeStatusCode(const int& code){StatusCode = code;}
		void setConv(){isconv=1;};
		void fillConvElectron(const G4Track* trk);
		void fillConvPositron(const G4Track* trk);
		int getStatusCode(){return StatusCode;}
		int getsize_ECALe(){return conve_ECAL_kinematic.size();}
		int getsize_ECALp(){return convp_ECAL_kinematic.size();}
		void fillECALeHit(const G4Track* trk);
		void fillECALpHit(const G4Track* trk);
		void fillConvHit(const G4double& edep){conv_e+=edep;}
		void fillTracks(const int& track_id,const G4ThreeVector& pos,const int& color=0);
		void fillTrackerHit(const int& tracker_id, const float& posX, const float& posY, const float& edep, const int& trkid);
		void fillTrackerEPHit(const float& e){tracker_ephite.emplace_back(e);}

		template<class T>
		constexpr std::array<T, 6> getTrackerPosZ() const { return TrackerPosZ; }

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

		TRandom3 *rand;

	private:
		int eventNo;
		int nConvPhoton;
		int nTotalOptPhoton;
		int apd_nphoton;
		int isconv;
		float init_x;
		float init_y;
		float init_z;
		float init_Px;
		float init_Py;
		float init_Pz;
		float init_E;
		float init_Ke;
		std::vector<float> conve_ECAL_kinematic;
		std::vector<float> convp_ECAL_kinematic;
		std::vector<float> conve_kinematic;
		std::vector<float> convp_kinematic;
		std::vector<int> ecal_cellid;
		std::vector<float> ecal_celle;
		std::vector<float> apd_celle;
		std::vector<float> apd_optime;
		std::vector<float> ecal_convtime;
		std::map<int,float> ecal_mape;
		std::map<int,float> apd_mape;
		std::map<int,int> ecal_npmap;
		float ecal_e;
		float conv_e;
		TList tracks;
		std::map<int, TPolyLine3D*> map_track;
		std::vector<std::vector<float>> tracker_hitpos;
		std::vector<float> tracker_hite;
		std::vector<int> tracker_trkid;
		std::vector<float> tracker_ephite;
		template<class T>
		struct TupleHash {
    		constexpr std::size_t operator()(const std::tuple<T, T, T>& t) const noexcept {
        		auto [x, y, z] = t;
        		// 使用质数组合哈希值
        		return std::hash<int>{}(x) + 31 * std::hash<int>{}(y) + 57 * std::hash<int>{}(z);
    		}
		};
		std::unordered_map<std::tuple<int,int,int>,float,TupleHash<int>> tracker_hitmap;
		static constexpr std::array<float, 6> TrackerPosZ = {
    		25.175 - 262.3, 30.135 - 262.3, 55.835 - 262.3,
    		60.795 - 262.3, 86.495 - 262.3, 91.455 - 262.3
		};
		// template<typename T>
		// constexpr std::map<std::tuple<int,int,int>,std::array<T,3>> getTrackerCoor(){
		// 	for(int z=0;z<6;z++){
		// 		for(int x=0;x< (z%2==0) ? 3 : 768*3 ; x++){
		// 			const T posX = (z%2==0) ? -95.+x*95. : -141.4+x*0.121;
		// 			for(int y=0;y< (z%2==0) ? 768*3 : 3 ; y++){
		// 				const T posY = (z%2==0) ? -141.4+y*0.121 : -95.+y*95.;
		// 				const T posZ = TrackerPosZ<T>[z];
		// 				tracker_hitpos[std::make_tuple(x,y,z)] = {posX,posY,posZ};
		// 			}
		// 		}
		// 	}
		// }

		// template<typename T>
		// static constexpr std::map<std::tuple<int,int,int>,std::array<T,3>> tracker_hitpos = getTrackerCoor<T>();
		// static_assert(tracker_hitpos<float>[std::make_tuple(0,0,0)][0]==-95., "Coordinate error");
		
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

