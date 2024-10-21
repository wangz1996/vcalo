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
#include "G4OpticalSurface.hh"
#include "globals.hh"
#include "SteppingAction.hh"
#include "Config.hh"

using CLHEP::eV;
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
  	Config *config;
    G4double fWorldSize;
	G4LogicalVolume* logicWorld;
	G4VPhysicalVolume* ConstructWorld(); 
	G4VPhysicalVolume* physiWorld; 
	G4VisAttributes* visAttributes;
	void constructECAL();
	void constructConverter();
	G4SubtractionSolid* constructSolidTiO2();
	G4SubtractionSolid* constructSolidConvTiO2();
	G4VSolid* constructECALShield();
	G4SubtractionSolid* constructSolidSiliconeRubber();
	void defineECALParameter();
	void defineECALMaterial();
	void defineConvParameter();
	

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
	G4double Hole_XY;
	G4double Hole_Z;
	G4double APD_XY;
	G4double APD_Z;
	G4double Conv_XY;
	G4double Conv_Z;
	G4double ConvTiO2_XY;
	G4double ConvTiO2_Z;
	G4double ConvPolish_XY;
	G4double ConvPolish_Z;

	G4Material* Vacuum;
	G4Material* CsI;
	G4Material* TiO2;

	G4Material* carbonFiber;
	G4Material* siliconeRubber;

	//Optical Properties
	G4MaterialPropertiesTable *CsIMPT;
	G4OpticalSurface* CsISurface;
	G4MaterialPropertiesTable* CsI_SurfaceMPT;
	static const G4int CsI_NEntries=20;
	G4double CsI_PEnergy[CsI_NEntries] = {
    1.55219*eV, 1.62123*eV, 1.70461*eV, 1.78240*eV, 1.86127*eV,
    1.91672*eV, 1.97201*eV, 2.02307*eV, 2.07292*eV, 2.12529*eV,
    2.17170*eV, 2.22473*eV, 2.27094*eV, 2.31910*eV, 2.38484*eV,
    2.49924*eV, 2.63158*eV, 2.78577*eV, 2.95112*eV, 3.31817*eV
	};

	G4double CsI_SlowComponentIntensity[CsI_NEntries] = {
    0.16787, 0.19185, 0.22782, 0.29736, 0.39808,
    0.49400, 0.60432, 0.73141, 0.82494, 0.91607,
    0.95683, 0.97842, 0.98321, 0.97362, 0.93525,
    0.80576, 0.57074, 0.35971, 0.19664, 0.02878
	};

	G4double CsI_RIndex[CsI_NEntries];
	G4double CsI_AbsLength[CsI_NEntries];	
	G4double CsI_SpecularLobe[CsI_NEntries];
	G4double CsI_DiffuseLobe[CsI_NEntries];
	G4double CsI_Reflectivity[CsI_NEntries];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
