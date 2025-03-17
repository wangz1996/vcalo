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
#include "G4ParticleGun.hh"
#include "G4Geantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4GeneralParticleSource.hh"
#include "HistoManager.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction *det, Config *c)
    : G4VUserPrimaryGeneratorAction(),
      fGParticleSource(),
      fDetector(det),
      config(c)
{
  // const char* filename = "pythia_event.data";
  //  HEPEvt = new G4HEPEvtInterface();
  if (config->conf["Source"]["isradio"].as<bool>())
  {
    use_gps = false;
    use_gun = true;
  }
  fGParticleSource = new G4GeneralParticleSource();
  particleGun = new G4ParticleGun(1);
  if (use_gps)
  {
    auto fCS = fGParticleSource->GetCurrentSource();
    std::string angtype = config->conf["Source"]["angtype"].as<std::string>();
    if (angtype == "iso")
    {
      fCS->GetAngDist()->SetAngDistType("iso");
    }
    else if (angtype == "planar")
    {
      fCS->GetAngDist()->SetAngDistType("planar");
      std::vector<double> gps_direction = config->conf["Source"]["direction"].as<std::vector<double>>();
      fCS->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(gps_direction.at(0), gps_direction.at(1), gps_direction.at(2)));
    }
    else
    {
      std::cerr << "Unknown angular distribution type: " << angtype << std::endl;
    }
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle(config->conf["Source"]["particle"].as<std::string>());
    fCS->SetParticleDefinition(particle);
    fCS->GetEneDist()->SetEnergyDisType("Mono");
    fCS->GetEneDist()->SetMonoEnergy(config->conf["Source"]["energy"].as<double>());
    std::string postype = config->conf["Source"]["type"].as<std::string>();
    if (postype == "Point")
    {
      std::cout << "Using Point source" << std::endl;
      fCS->GetPosDist()->SetPosDisType("Point");
    }
    else if (postype == "Plane")
    {
      fCS->GetPosDist()->SetPosDisType("Plane");
      fCS->GetPosDist()->SetPosDisShape("Rectangle");
      float size = config->conf["Source"]["size"].as<double>();
      fCS->GetPosDist()->SetHalfX(size / 2. * cm); // X方向半长度
      fCS->GetPosDist()->SetHalfY(size / 2. * cm); // Y方向半长度
    }
    else
    {
      std::cerr << "Unknown position distribution type: " << postype << std::endl;
    }
    std::vector<double> gps_position = config->conf["Source"]["position"].as<std::vector<double>>();
    fCS->GetPosDist()->SetCentreCoords(G4ThreeVector(gps_position.at(0), gps_position.at(1), gps_position.at(2)));
    std::cout << "Source position: " << gps_position.at(0) << ", " << gps_position.at(1) << ", " << gps_position.at(2) << std::endl;
  }
  else
  {
    // use_gun
    Z = config->conf["Source"]["radio"]["Z"].as<int>();
    A = config->conf["Source"]["radio"]["A"].as<int>();
    particleGun->SetParticleEnergy(0 * eV);
    std::vector<double> gun_position = config->conf["Source"]["position"].as<std::vector<double>>();
    particleGun->SetParticlePosition(G4ThreeVector(gun_position.at(0), gun_position.at(1), gun_position.at(2)));
    std::cout << "Source position: " << gun_position.at(0) << ", " << gun_position.at(1) << ", " << gun_position.at(2) << std::endl;
    std::vector<double> gun_direction = config->conf["Source"]["direction"].as<std::vector<double>>();
    particleGun->SetParticleMomentumDirection(G4ThreeVector(gun_direction.at(0), gun_direction.at(1), gun_direction.at(2)));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  // delete HEPEvt;
  // delete fGParticleSource;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
  // set random particle  position
  //
  // create vertex
  //
  // auto fCS = fGParticleSource->GetCurrentSource();
  // if(config->conf["Source"]["isradio"].as<bool>()){
  //   G4int Z = config->conf["Source"]["radio"]["Z"].as<int>();
  //   G4int A = config->conf["Source"]["radio"]["A"].as<int>();
  //   G4double exciteEnergy = 0.*keV;
  //   G4double ionCharge = 0.*eplus;
  //   G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z,A,exciteEnergy);
  //   ion->SetPDGStable(false);
  //   std::cout<<"Using particle: "<<ion->GetParticleName()<<std::endl;
  //   fCS->SetParticleDefinition(ion);
  //   fCS->SetParticleCharge(ionCharge);

  // }
  if (use_gps)
  {
    fGParticleSource->GeneratePrimaryVertex(anEvent);
  }
  else
  {
    G4double ionCharge   = 0.*eplus;
    G4double excitEnergy = 0.*keV;

    G4ParticleDefinition* ion
       = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
    particleGun->SetParticleDefinition(ion);
    particleGun->SetParticleCharge(ionCharge);
    particleGun->GeneratePrimaryVertex(anEvent);
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
