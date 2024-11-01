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

void SteppingAction::UserSteppingAction(const G4Step* aStep) {
  // 获取 PreStep 和 PostStep 信息
  auto preStepPoint = aStep->GetPreStepPoint();
  auto postStepPoint = aStep->GetPostStepPoint();

  // 初始化 PreStep 和 PostStep 的物理/逻辑体名称
  std::string preStepPVName, postStepPVName, preStepLVName, postStepLVName;

  if (preStepPoint) {
    if (auto preVolume = preStepPoint->GetPhysicalVolume()) {
      preStepPVName = preVolume->GetName();
      preStepLVName = preVolume->GetLogicalVolume()->GetName();
    }
  }

  if (postStepPoint) {
    if (auto postVolume = postStepPoint->GetPhysicalVolume()) {
      postStepPVName = postVolume->GetName();
      postStepLVName = postVolume->GetLogicalVolume()->GetName();
    }
  }

  // 获取步骤相关的信息
  const G4VProcess *process;
  process = (postStepPoint) ? postStepPoint->GetProcessDefinedStep(): nullptr;
  auto processName = process ? process->GetProcessName() : "";
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4Track* track = aStep->GetTrack();
  G4double time = track->GetGlobalTime();
  G4int trackid = track->GetTrackID();
  G4int stepNumber = track->GetCurrentStepNumber();
  G4int pdgid = track->GetDefinition()->GetPDGEncoding();
  G4int copyNo = preStepPoint->GetPhysicalVolume()->GetCopyNo();
  auto particleDef = track->GetDefinition();

  // 处理Converter信息
  if (processName == "conv" && postStepLVName == "logicConv" && track->GetParentID() == 0) {
    HistoManager::getInstance().setConv();
    const std::vector<const G4Track*>* secondaries = aStep->GetSecondaryInCurrentStep();
    for(auto trk:*secondaries){
      if(trk->GetDefinition()->GetPDGEncoding() == 11){
        HistoManager::getInstance().fillConvElectron(trk);
      }
      else if(trk->GetDefinition()->GetPDGEncoding() == -11){
        HistoManager::getInstance().fillConvPositron(trk);
      }
    }
  }

  // 处理光子信息
  if (particleDef == G4OpticalPhoton::OpticalPhotonDefinition()) {
    if (stepNumber == 1) {
      HistoManager::getInstance().addTotalOptPhoton();
    }
    else if (preStepPVName == "physicConv" && postStepPVName.find("World") != std::string::npos) {
      HistoManager::getInstance().fillConvOptHit(time);
    }
    else if ((preStepPVName.find("World") != std::string::npos) && postStepLVName == "logicAPD") {
      // std::cout<<"APD detected"<<std::endl;
        HistoManager::getInstance().fillAPDOptHit(time);
    }
    else if ((preStepPVName.find("World") != std::string::npos) && postStepLVName == "logicConvAPD") {
      // std::cout<<"APD detected"<<std::endl;
        HistoManager::getInstance().fillConvOptHit(time);
    }
    else if (preStepLVName.find("logicCsI") != std::string::npos && postStepLVName == "World") {
      // std::cout<<"CsI to World"<<std::endl;
    }
  }

  // 处理能量沉积和命中
  if (postStepLVName == "logicAPD") {
    HistoManager::getInstance().fillAPDHit(copyNo, edep, time, pdgid, trackid);
  } else if (postStepLVName == "logicCsI") {
    HistoManager::getInstance().fillEcalHit(copyNo, edep, time, pdgid, trackid);
  }
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
