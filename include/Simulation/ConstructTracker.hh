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

#ifndef ConstructTracker_h
#define ConstructTracker_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4OpticalSurface.hh"
#include "globals.hh"
#include "SteppingAction.hh"
#include "Config.hh"

class HexArrayParam : public G4VPVParameterisation {
public:
    HexArrayParam(G4double radius, G4int rows, G4int cols)
        : radius_(radius), rows_(rows), cols_(cols) {}

    void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const override {
        G4int row = copyNo / cols_;
        G4int col = copyNo % cols_;
        
        // 计算 hexcell 的横向位置
        G4double xPos = col * 2 * radius_ * cos(M_PI / 3);
        G4double yPos = row * 1.5 * radius_;
        
        // 偶数行向右偏移半个单元
        if (row % 2 == 1) {
            xPos += radius_ * cos(M_PI / 3);
        }
        
        physVol->SetTranslation(G4ThreeVector(xPos, yPos, 0));
    }

private:
    G4double radius_;
    G4int rows_;
    G4int cols_;
};

#endif // ConstructTracker_h