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
  rnd.SetSeed(0);
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
    fCS = fGParticleSource->GetCurrentSource();
    fenedist = fCS->GetEneDist();
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
    par_mass = particle->GetPDGMass()/1000.; // GeV
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
    else if (postype == "Sphere"){
      fCS->GetPosDist()->SetPosDisType("Surface");
      fCS->GetPosDist()->SetPosDisShape("Sphere");
      fCS->GetPosDist()->SetRadius(config->conf["Source"]["SphereRadius"].as<float>() * mm);
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
  //If using spectrum
  use_spec = config->conf["Global"]["usespec"].as<bool>();
  if(use_spec){
    specfile = config->conf["Global"]["specfile"].as<std::string>();
    spechist = config->conf["Global"]["spechist"].as<std::string>();
    TFile *file=TFile::Open(TString(specfile),"READ");
    if(!file){
      std::cerr<<"Cannot open spectrum file: "<<specfile<<std::endl;
      exit(1);
    }
    hspec = (TH1D*)file->Get(TString(spechist));
    if(!hspec){
      std::cerr<<"Cannot find spectrum histogram: "<<spechist<<std::endl;
      exit(1);
    }
    hspec->SetDirectory(0);
    file->Close();
    delete file;
  }
  use_spherespec = config->conf["Global"]["usespherespec"].as<bool>();
  if(use_spherespec){
    std::cout<<"Using sphere spectrum for inputs"<<std::endl;
    pc = new PointCloud();
    spec_anglefile = config->conf["Source"]["spec_anglefile"].as<std::string>();
    spec_anglehist = config->conf["Source"]["spec_anglehist"].as<std::string>();
    std::ifstream file(spec_anglefile);
    if(!file){
      std::cerr<<"Cannot open spectrum angle file: "<<spec_anglefile<<std::endl;
    }
    int tmp_index;float tmp_theta,tmp_arc;
    while(file>>tmp_index>>tmp_theta>>tmp_arc){
      pc->addPoint(tmp_index,tmp_theta,tmp_arc);
      umap_index_thetaarc[tmp_index] = std::pair<float,float>(tmp_theta,tmp_arc);
      sphere_count++;
    }
    file.close();
    pc->buildTree();


    TFile *fhist = TFile::Open(TString(spec_anglehist),"READ");
    if(!fhist){
      std::cerr<<"Cannot open spectrum angle histogram: "<<spec_anglehist<<std::endl;
    }
    for(int i=0;i<sphere_count;i++){
      auto h = (TH1D*)fhist->Get(TString::Format("h%d",i));
      h->SetDirectory(0);
      umap_index_anglehist[i] = h;
    }
    fhist->Close();
  }
  random_center = config->conf["Source"]["random_center"].as<std::vector<int>>();
  x0_range = config->conf["Source"]["x0_range"].as<std::vector<float>>();
  y0_range = config->conf["Source"]["y0_range"].as<std::vector<float>>();
  z0_range = config->conf["Source"]["z0_range"].as<std::vector<float>>();
  if(random_center.size()!=3){
    std::cerr<<"random_center must be a vector of 3 numbers"<<std::endl;
  }
  spec_R = config->conf["Source"]["spec_R"].as<float>();
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
  if (use_gps)
  {
    if(use_spec){
      double momentum = hspec->GetRandom(); // GeV
    double energy = sqrt(momentum * momentum + par_mass * par_mass) - par_mass; // GeV
    fenedist->SetMonoEnergy(energy * GeV);
    }
    if(use_spherespec){
      float ex = -999.; float ez = -999.; float ey = -285.;
      float x,y,z,ux,uy,uz,x0,y0,z0;
      int tmp_index;
      while(abs(ex)>150. || abs(ez)>150.){
      tmp_index = rnd.Integer(sphere_count);
      float theta = umap_index_thetaarc[tmp_index].first;
      float arc = umap_index_thetaarc[tmp_index].second;  
      x0 = rnd.Uniform(x0_range[0],x0_range[1]);
      y0 = rnd.Uniform(z0_range[0],z0_range[1]);
      z0 = rnd.Uniform(y0_range[0],y0_range[1]);
      ux = -sin(theta) * cos(arc);
      uy = -sin(theta) * sin(arc);
      uz = -cos(theta);
      float su = sqrt(ux * ux + uy * uy + uz * uz);
      ux /= su; uy /= su; uz /= su;
      x = x0 - spec_R * ux;
      y = y0 - spec_R * uy;
      z = z0 - spec_R * uz;
      //Equivalent xyz at ez=-285
      float eR = (y0-ey)/uy;
      ex = x0 - eR * ux;
      ez = z0 - eR * uz;
      }
      float nx = x0-x; float ny = y0-y; float nz = z0-z;
      float sn = sqrt(nx*nx+ny*ny+nz*nz);
      nx /= sn; ny /= sn; nz /= sn;
      // std::cout<<x<<" "<<y<<" "<<z<<" "<<ux<<" "<<uy<<" "<<uz<<" "<<x0<<" "<<y0<<" "<<z0<<std::endl;
      // std::cout<<nx<<" "<<ny<<" "<<nz<<std::endl;
      double momentum = umap_index_anglehist[tmp_index]->GetRandom(); // GeV
      double energy = sqrt(momentum * momentum + par_mass * par_mass) - par_mass; // GeV
      fCS = fGParticleSource->GetCurrentSource();
      fCS->GetPosDist()->SetCentreCoords(G4ThreeVector(x*mm,z*mm,y*mm));
      fCS->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(nx,nz,ny));
      fenedist->SetMonoEnergy(energy * GeV);
    }
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

double PrimaryGeneratorAction::wrapTo2Pi(double angle) {
    double result = fmod(angle, 2.*TMath::Pi());
    if (result < 0) {
        result += 2.*TMath::Pi();
    }
    return 360.*result/(2.*TMath::Pi());
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
