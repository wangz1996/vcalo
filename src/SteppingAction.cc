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
/// \file analysis/shared/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//
// $Id: SteppingAction.cc 68015 2013-03-13 13:27:27Z gcosmo $
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4UnitsTable.hh"
//#include "G4EmSaturation.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//
SteppingAction* SteppingAction::fgInstance =0;
SteppingAction* SteppingAction::Instance()
{
  return fgInstance;
}
//
SteppingAction::SteppingAction(DetectorConstruction* det,
			       EventAction* event) 
  : G4UserSteppingAction(),
    fVolume(0),
    fDetector(det), fEventAction_Step(event)                                         
{
  fgInstance = this;
  kineticEn=0;
  volume1="none";
  volume2="none";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ 
  fgInstance = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  auto preStepPoint = aStep->GetPreStepPoint();
  auto postStepPoint = aStep->GetPostStepPoint();
  std::string preStepPVName, postStepPVName,preStepLVName,postStepLVName;
  if (preStepPoint && preStepPoint->GetPhysicalVolume()) {
    preStepPVName = preStepPoint->GetPhysicalVolume()->GetName();
    preStepLVName = preStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
  }

  if (postStepPoint && postStepPoint->GetPhysicalVolume()) {
    postStepPVName = postStepPoint->GetPhysicalVolume()->GetName();
    postStepLVName = postStepPoint->GetPhysicalVolume()->GetLogicalVolume()->GetName();
  }
  auto process = postStepPoint->GetProcessDefinedStep();
  auto processName = process->GetProcessName();


  G4double edep = aStep->GetTotalEnergyDeposit();
  G4int copyNo = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
  G4int pdgid = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  G4int trackid = aStep->GetTrack()->GetTrackID();
  G4int stepNumber = aStep->GetTrack()->GetCurrentStepNumber();
  if(trackid==1 && stepNumber==1){
    HistoManager::getInstance().fillPrimary(aStep->GetTrack());
  }
  if(aStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
    stepNumber==1){
    HistoManager::getInstance().addTotalOptPhoton();
  }
  G4double time = aStep->GetPreStepPoint()->GetGlobalTime();
  if(aStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() &&
    preStepPVName=="physicConv" &&
    postStepPVName.find("World")!=std::string::npos){
    HistoManager::getInstance().addConvPhoton();
    HistoManager::getInstance().fillConvTime(time);
  }

  if(postStepLVName=="logicAPD"){HistoManager::getInstance().fillAPDHit(copyNo,edep,time,pdgid,trackid); }
  // if(time > 150.)return;

  if(postStepLVName=="logicCsI") {  HistoManager::getInstance().fillEcalHit(copyNo,edep,time,pdgid,trackid); }
}

 
void SteppingAction::Reset()
{
  //fEnergy = 0.;
}

G4double SteppingAction::BirksAttenuation(const G4Step* aStep)
{
 //Example of Birk attenuation law in organic scintillators.
 //adapted from Geant3 PHYS337. See MIN 80 (1970) 239-244
 //
 G4Material* material = aStep->GetTrack()->GetMaterial();
 G4double birk1       = material->GetIonisation()->GetBirksConstant();
 G4double destep      = aStep->GetTotalEnergyDeposit();
 G4double stepl       = aStep->GetStepLength();  
 G4double charge      = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
 //
 G4double response = destep;
 if (birk1*destep*stepl*charge != 0.)
   {
     response = destep/(1. + birk1*destep/stepl);
   }
 return response;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
