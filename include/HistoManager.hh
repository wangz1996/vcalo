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
#include <boost/container/small_vector.hpp>
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
#include <any>
#include <unordered_set>
#include <G4ThreeVector.hh>
#include <unordered_map>
#include <filesystem>

using TrackerHit = boost::container::small_vector<size_t,2>;

template<class T>
struct HitIDHash {
    constexpr T operator()(const TrackerHit& t) const noexcept {
        if (t.size() < 2) {
            throw std::invalid_argument("TrackerHit must have at least 2 elements");
        }
        return std::hash<T>{}(t[0]) ^ (std::hash<T>{}(t[1]) << 1);
    }
};

using TrackerHits = std::unordered_set<TrackerHit,HitIDHash<size_t>>;
using TrackerHitMap = std::unordered_map<size_t,TrackerHits>; //TrackerHit collections per layer -> 0,1,2,3,4,5

class TTree;
class TFile;
class Config;
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
		void setEinECAL(){conve_inECAL=1;}
		void setPinECAL(){convp_inECAL=1;}

	private:
		//Singleton design mode
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

		void createHit(const TrackerHit& ehit,const TrackerHit& ohit,const size_t& layer);
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
		int conve_inECAL;
		int convp_inECAL;
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
		std::vector<float> tracker_hitx;
		std::vector<float> tracker_hity;
		std::vector<float> tracker_hitz;
		std::vector<float> tracker_hite;
		std::vector<int> tracker_trkid;
		std::vector<float> tracker_ephite;

		//Constants
		static constexpr std::array<float, 6> TrackerPosZ = {
    	-218.425, -213.125, -187.425,
    	-182.125, -156.425, -151.125
		};
		template <class DT, size_t N>
		static constexpr std::array<DT, N> Temp_StripT()
		{
			std::array<DT, N> StripT;
			for (size_t i = 0; i < N; i++)
			{
				StripT[i] = -141.4 + i * 0.121;
			}
			return StripT;
		}
		template<class TT=std::any>
    	static const std::array<float,2404> StripT;
		static constexpr std::array<float, 3> StripL = { // Strip positions in longitudinal direction
			-95.,0.,95.
		};

		TrackerHitMap tracker_hitmap;
		TrackerHits tracker_hits;
		
};



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

