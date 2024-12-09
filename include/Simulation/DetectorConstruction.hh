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
#include "G4LogicalSkinSurface.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4OpticalSurface.hh"
#include "globals.hh"
#include "SteppingAction.hh"
#include "G4UserLimits.hh"
#include "G4StepLimiterPhysics.hh"
#include "Config.hh"

using CLHEP::eV;
using CLHEP::cm;
using CLHEP::mm;
using CLHEP::um;
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
	void constructTracker();
	G4SubtractionSolid* constructSolidTiO2();
	G4SubtractionSolid* constructSolidConvTiO2();
	G4VSolid* constructECALShield();
	G4VSolid* constructCushion();
	G4SubtractionSolid* constructSolidSiliconeRubber();

	G4VSolid* constructSolidHex();
	void defineECALParameter();
	void defineECALMaterial();
	void defineConvParameter();

	void defineTrackerMaterial();
	void defineTrackerParameter();
	

	int nCryX;
	int nCryY;

	G4double CsI_MSL = 1.0* cm;
	G4double APD_MSL = 5.0* um;
	static constexpr double CsI_Z = 200.*mm;
	static constexpr double CsI_XY = 60. * mm;
	static constexpr double TiO2_XY = 61.2 * mm;
	static constexpr double TiO2_Z = 201.2 * mm;
	static constexpr double Polish_XY = 35. * mm;
	static constexpr double Polish_Z = (TiO2_Z - CsI_Z)/2.;

	static constexpr double APD_XY = 10. *mm;
	static constexpr double APD_Z = 10. *um;
	static constexpr double Cushion_XY = TiO2_XY;
	static constexpr double CushionInner_XY = 40. *mm;
	static constexpr double Cushion_Z = 1. *mm;

	static constexpr double Hole_Z = TiO2_Z;
	static constexpr double ECALShield_Z = TiO2_Z;
	G4double CryGap;
	G4double ECALShield_XY;
	G4double Hole_XY;

	static constexpr double Conv_XY = 150.*mm;
    static constexpr double Conv_Z = 15*mm;
    static constexpr double ConvAir_XY = 150.2*mm;
    static constexpr double ConvAir_Z = 15.2*mm;
    static constexpr double ConvTiO2_XY = 151.4*mm;
    static constexpr double ConvTiO2_Z = 16.4*mm;
    static constexpr double ConvPolish_XY = 15*mm;
    static constexpr double ConvPolish_Z = 0.6*mm;
    static constexpr double Conv_PosZ = -0.5*CsI_Z-160.*mm+0.5*ConvTiO2_Z; //-251.8*mm;

	//Tracker
	static constexpr double Hex_XY = 287. * mm;
	static constexpr double Hex_Z = 20. * mm;
	static constexpr double HexCell_XY = 25. * mm;
	static constexpr double HexCell_Z = Hex_Z;
	static constexpr double Tray_XY = Hex_XY;
	static constexpr double Tray_Z = Hex_Z;
	static constexpr double Tracker_XY = 285.*mm;
	static constexpr double Tracker_Z = 0.35*mm;
	static constexpr std::array<float, 6> TrackerPosZ = {
    -218.425, -213.125, -187.425,
    -182.125, -156.425, -151.125
	};

	G4Material* Vacuum;
	G4Material* CsI;
	G4Material* TiO2;
	G4Material* fAir;
	G4Material* Si;
	G4Material* Al;
	G4Material* Kapton;

	G4Material* carbonFiber;
	G4Material* siliconeRubber;
	G4Material* mat_Tray;

	//Optical Properties
	G4MaterialPropertiesTable *CsIMPT;
	G4MaterialPropertiesTable *TiO2MPT;
	G4MaterialPropertiesTable *APDMPT;
	G4OpticalSurface* CsISurface;
	G4OpticalSurface* CsI_Galactic_Surface;
	G4OpticalSurface* TiO2_Galactic_Surface;
	G4OpticalSurface* APD_Galactic_Surface;
	G4OpticalSurface* APD_CsI_Surface;
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
	static const G4int TiO2_NEntries=15;
	G4double TiO2_PEnergy[TiO2_NEntries] = {
    0.95372*eV,   // 1.30 μm
    1.03320*eV,   // 1.20 μm
    1.12713*eV,   // 1.10 μm
    1.23984*eV,   // 1.00 μm
    1.37760*eV,   // 0.90 μm
    1.45981*eV,   // 0.85 μm
    1.54980*eV,   // 0.80 μm
    1.65246*eV,   // 0.75 μm
    1.77120*eV,   // 0.70 μm
    1.90745*eV,   // 0.65 μm
    2.06640*eV,   // 0.60 μm
    2.25425*eV,   // 0.55 μm
    2.47968*eV,   // 0.50 μm
    2.75520*eV,   // 0.45 μm
    2.88195*eV    // 0.43 μm
	};
	G4double TiO2_RIndex[TiO2_NEntries] = {
    2.4626,  // 对应 1.30 μm
    2.4683,  // 对应 1.20 μm
    2.4757,  // 对应 1.10 μm
    2.4856,  // 对应 1.00 μm
    2.4995,  // 对应 0.90 μm
    2.5086,  // 对应 0.85 μm
    2.5197,  // 对应 0.80 μm
    2.5336,  // 对应 0.75 μm
    2.5512,  // 对应 0.70 μm
    2.5742,  // 对应 0.65 μm
    2.6049,  // 对应 0.60 μm
    2.6479,  // 对应 0.55 μm
    2.7114,  // 对应 0.50 μm
    2.8126,  // 对应 0.45 μm
    2.8717   // 对应 0.43 μm
	};
	G4double TiO2_Reflectivity[TiO2_NEntries];
	G4double TiO2_Transmission[TiO2_NEntries];
	G4double TiO2_AbsLength[TiO2_NEntries];

	static constexpr std::array<double,4> TrayPosZ = {-231.1, -200.1, -169.1, -138.1};
	G4double CsI_RIndex[CsI_NEntries];
	G4double CsI_AbsLength[CsI_NEntries];
	G4double CsI_Rayleigh[CsI_NEntries];
	G4double CsI_SpecularLobe[CsI_NEntries];
	G4double CsI_SpecularSpike[CsI_NEntries];
	G4double CsI_BackScatter[CsI_NEntries];
	G4double CsI_DiffuseLobe[CsI_NEntries];
	G4double CsI_Reflectivity[CsI_NEntries];

	static const std::unordered_map<std::string, G4SurfaceType> surfaceTypeMap;
	static const std::unordered_map<std::string, G4OpticalSurfaceModel> surfaceModelMap;
	static const std::unordered_map<std::string, G4OpticalSurfaceFinish> surfaceFinishMap;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

