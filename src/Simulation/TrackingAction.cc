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
/// \brief Implementation of the TrackingAction class
//
//
// $Id: TrackingAction.cc 78307 2013-12-11 10:55:57Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"

#include "HistoManager.hh"
//#include "Run.hh"
#include "EventAction.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(RunAction*runAct,EventAction* EA,Config *c)
  :G4UserTrackingAction(),
   fRun(runAct),fEvent(EA),
   fFullChain(true),config(c)
{
  fAbort_direction = c->conf["Global"]["Abort_direction"].as<int>();
  //fSteppingVerbose_Tracking = new SteppingVerbose();
}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::~TrackingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    // std::cout<<"TrackingAction::PreUserTrackingAction"<<std::endl;
    fEvent->fStepTag=0;
  /*
  Run* run 
    = static_cast<Run*>(G4RunManager::GetRunManager()->GetCurrentRun());
  */   
  if (__builtin_expect(track->GetParentID() == 0,0)){
    bool isAbort= (fAbort_direction > 0 && track->GetMomentum().z() > 0) || (fAbort_direction < 0 && track->GetMomentum().z() < 0);
    if (isAbort) {
      HistoManager::getInstance().setStatusCode(0);
      const_cast<G4Track*>(track)->SetTrackStatus(fStopAndKill);
      G4RunManager::GetRunManager()->AbortEvent();
    }
    HistoManager::getInstance().fillPrimary(track);
  }
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String name   = particle->GetParticleName();
  G4int ID      = track->GetTrackID();
  G4double Ekin = track->GetKineticEnergy();
  fParticleEnCode= particle->GetPDGEncoding();
  //-----------------update------------------------
}
  
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
    // std::cout<<"TrackingAction::PostUserTrackingAction"<<std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

