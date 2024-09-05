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
/// \file DBDecay/include/DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.hh 68017 2013-03-13 13:29:53Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "globals.hh"
#include "SteppingAction.hh"
#include "Config.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class Config;
class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    DetectorConstruction(Config *c);
   ~DetectorConstruction();

    virtual     
    G4VPhysicalVolume* Construct();
                        
    G4double GetWorldSize() {return fWorldSize;}; 
	G4VPhysicalVolume* GetphysiWorld()
	{
	    return this->physiWorld;
	}
    
  private:
    G4double fWorldSize;
	G4LogicalVolume* logicWorld;
	G4VPhysicalVolume* ConstructWorld(); 
	G4VPhysicalVolume* physiWorld; 
    G4VisAttributes* visAttributes;
	void ConstructECAL();
	G4SubtractionSolid* constructSolidTiO2();
	G4VSolid* constructECALShield();
	void defineECALParameter();
	void defineECALMaterial();
	Config *config;

	int nCryX;
	int nCryY;

	G4double CsI_XY;
	G4double CsI_Z;
	G4double TiO2_XY;
	G4double TiO2_Z;
	G4double Polish_XY;
	G4double Polish_Z;
	G4double CryGap;
	G4double ECALShield_XY;
	G4double ECALShield_Z;

	G4Material* Vacuum;
	G4Material* CsI;
	G4Material* TiO2;

	G4Material* carbonFiber;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

