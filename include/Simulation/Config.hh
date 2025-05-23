#ifndef CONFIG_HH
#define CONFIG_HH
#include "G4RunManager.hh"
#include "G4PhysicsListHelper.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4RadioactiveDecay.hh"
#include "DetectorConstruction.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "TList.h"
#include "TPolyLine3D.h"
// #include "PhysicsList.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "SteppingVerbose.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "TrackingAction.hh"
#include "QGSP_BERT.hh"
#include "G4GDMLParser.hh"
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4SystemOfUnits.hh"
#include <CLHEP/Units/SystemOfUnits.h>

#include "G4VisExecutive.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalPhoton.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4UIExecutive.hh"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include "yaml-cpp/yaml.h"
#include <filesystem>

class Config
{
public:
	Config();
	~Config();
	virtual void Print();
    virtual void Parse(const std::string &config_file);
    virtual int Run();
	bool IsLoad();
	virtual void SetupVisualization();
	YAML::Node conf;
private:
	G4UImanager* UI;
	G4long GetTimeNs()
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME,&ts);
		return ts.tv_sec*1000000000+ts.tv_nsec;
	}

};

#endif
