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
/// \file DBDecay/include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.hh 68017 2013-03-13 13:29:53Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cmath>
#include "PointCloud.hh"
#include "constants.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "HistoManager.hh"
#include "TH1D.h"
#include "TFile.h"
#include "Config.hh"

class G4VPrimaryGenerator;
class G4Event;
class DetectorConstruction;
class HistoManager;
class Config;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(DetectorConstruction*,  Config *c);    
   ~PrimaryGeneratorAction();

  public:
    virtual void GeneratePrimaries(G4Event*);
    G4GeneralParticleSource* GetParticleGun() { return fGParticleSource;}; 
    // G4ParticleGun* GetParticleGun() { return fGParticleSource;}; 

  private:
   G4ParticleGun* particleGun;
    G4GeneralParticleSource * fGParticleSource;
    DetectorConstruction* fDetector;
	Config *config;
  bool use_gun=false;
  bool use_gps=true;
  bool use_spectrum=false;
  bool use_spherespec=false;
  int sphere_count=0;
  int Z;
  int A;
  bool use_spec=false;
  float par_mass=0.;
  float spec_R=1000.;
  float spec_Z=1.;
  TH1D *hspec;
  std::string specfile;
  std::string spechist;
  std::string spec_anglefile;
  std::string spec_anglehist;
  std::vector<int> random_center;
  std::vector<float> x0_range;
  std::vector<float> y0_range;
  std::vector<float> z0_range;
  double wrapTo2Pi(double angle);
  float center_x0=0.;
  float center_y0=0.;
  float center_z0=0.;
  G4SingleParticleSource* fCS;
  G4SPSEneDistribution* fenedist;
  std::unordered_map<int,std::pair<float,float>> umap_index_thetaarc;
  std::unordered_map<int,TH1D*> umap_index_anglehist;
  TRandom3 rnd;
  PointCloud *pc;
  public:
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
