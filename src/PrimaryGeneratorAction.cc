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
/// \brief Implementation of the PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.cc 78307 2013-12-11 10:55:57Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4HEPEvtInterface.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4GeneralParticleSource.hh"
#include "HistoManager.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det,Config *c)
 : G4VUserPrimaryGeneratorAction(),
   fGParticleSource(),
   fDetector(det),
   config(c)
{
 // const char* filename = "pythia_event.data";
//  HEPEvt = new G4HEPEvtInterface();
  fGParticleSource  = new G4GeneralParticleSource();
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(config->conf["Source"]["particle"].as<std::string>());
  auto fParticleGun = fGParticleSource->GetCurrentSource();
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->GetAngDist()->SetAngDistType("planar");
  std::vector<double> gps_direction = config->conf["Source"]["direction"].as<std::vector<double>>();
  fParticleGun->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(gps_direction.at(0),gps_direction.at(1),gps_direction.at(2)));
  fParticleGun->GetEneDist()->SetEnergyDisType("Mono");
  fParticleGun->GetEneDist()->SetMonoEnergy(config->conf["Source"]["energy"].as<double>() *1000.);
  fParticleGun->GetPosDist()->SetPosDisType("Point");
  std::vector<double> gps_position = config->conf["Source"]["position"].as<std::vector<double>>();
  fParticleGun->GetPosDist()->SetCentreCoords(G4ThreeVector(gps_position.at(0),gps_position.at(1),gps_position.at(2)));

  useHEPEvt = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
 // delete HEPEvt;
  delete fGParticleSource;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // set random particle  position
  //  
  // create vertex
  //
  if(useHEPEvt)
    { //HEPEvt->GeneratePrimaryVertex(anEvent);
 //   G4cout<<" ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ "<<G4endl;
    }
  else
  { fGParticleSource->GeneratePrimaryVertex(anEvent);
      //G4cout<<" @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ "<<G4endl;
      //G4cout<<angle<<" : "<<position.x()<<", "<<position.y()<<", "<<position.z()<<G4endl;
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
