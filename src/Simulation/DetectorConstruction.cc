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
/// \file radioactivedecay/rdecay01/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.cc 78307 2013-12-11 10:55:57Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction(Config *c)
 : G4VUserDetectorConstruction(),
   config(c)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    visAttributes = new G4VisAttributes(G4Colour(0.9,0.0,0.0));
    visAttributes -> SetVisibility(false);
	physiWorld = ConstructWorld();
    // define materials and parameters 
    // some of the materials are used by converter
    defineConvParameter();
    defineACDParameter();
    defineECALParameter();
    defineECALMaterial();
    defineTrackerParameter();
	defineTrackerMaterial();
    if(config->conf["Converter"]["build"].as<bool>()){
        constructConverter();
    }
	if(config->conf["ECAL"]["build"].as<bool>())
	{
		constructECAL();
	}
    if(config->conf["ECAL"]["ECALShield"].as<bool>()){
        constructECALShield();
    }
    if(config->conf["Tracker"]["build"].as<bool>()){
        constructTracker();
    }
    if(config->conf["ACD"]["build"].as<bool>()){
        constructACD();
    }
    return physiWorld;
}
G4VPhysicalVolume* DetectorConstruction::ConstructWorld()
{
    G4double a, z, density;
    G4int nelements;
    auto N = new G4Element("Nitrogen", "N", z = 7, a = 14.01 * g / mole);
    auto O = new G4Element("Oxygen", "O", z = 8, a = 16.00 * g / mole);
    fAir = new G4Material("Air", density = 1.29 * mg / cm3, nelements = 2);
    fAir->AddElement(N, 70. * perCent);
    fAir->AddElement(O, 30. * perCent);
    // Air
    std::vector<G4double> photonEnergy = {
    2.034 * eV, 2.068 * eV, 2.103 * eV, 2.139 * eV, 2.177 * eV, 2.216 * eV,
    2.256 * eV, 2.298 * eV, 2.341 * eV, 2.386 * eV, 2.433 * eV, 2.481 * eV,
    2.532 * eV, 2.585 * eV, 2.640 * eV, 2.697 * eV, 2.757 * eV, 2.820 * eV,
    2.885 * eV, 2.954 * eV, 3.026 * eV, 3.102 * eV, 3.181 * eV, 3.265 * eV,
    3.353 * eV, 3.446 * eV, 3.545 * eV, 3.649 * eV, 3.760 * eV, 3.877 * eV,
    4.002 * eV, 4.136 * eV
    };
    std::vector<G4double> AirRIndex = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                             1.0, 1.0, 1.0, 1.0 };

    Vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    auto AirMPT = new G4MaterialPropertiesTable();
    AirMPT->AddProperty("RINDEX", photonEnergy, AirRIndex);

    // G4cout << "Air G4MaterialPropertiesTable:" << G4endl;
    // AirMPT->DumpTable();

    fAir->SetMaterialPropertiesTable(AirMPT);
    Vacuum->SetMaterialPropertiesTable(AirMPT);
    G4bool checkOverlaps = false;

    // Full sphere shape
    G4double solidWorld_rmax = 500*cm;
    G4Orb*
        solidWorld = new G4Orb("World",                          // its name
                solidWorld_rmax);                // its size 

    //G4LogicalVolume*                         
        logicWorld = new G4LogicalVolume(solidWorld,             //its solid
                Vacuum,                    //its material
                "World");               //its name
    G4VPhysicalVolume*                                   
        physiWorld = new G4PVPlacement(0,                      //no rotation
                G4ThreeVector(),        //at (0,0,0)
                logicWorld,             //its logical volume
                "World",                //its name
                0,                      //its mother  volume
                false,                  //no boolean operation
                0,
                checkOverlaps);                     //copy number
    logicWorld ->SetVisAttributes(visAttributes);
	return physiWorld;
}
