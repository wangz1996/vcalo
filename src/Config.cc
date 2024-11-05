#include "Config.hh"

using namespace std;

Config::Config()
{}
Config::~Config()
{}

void Config::Parse(const std::string &config_file)
{
	UI = G4UImanager::GetUIpointer();
	conf = YAML::LoadFile(config_file);

	if(conf["Project"].IsDefined())
	{
		std::cout<<"Config file loaded sucessfully"<<std::endl;
	}
	else
	{
		throw config_file;
	}
}

bool Config::IsLoad()
{
	return conf["Project"].IsDefined();
}

int Config::Run()
{
	//Initialize the singleton HistoManager
	HistoManager::getInstance().book(conf["Global"]["output"].as<std::string>().c_str(),conf["Global"]["savegeo"].as<bool>());
	HistoManager::getInstance().bindConfig(this);
	//choose the Random engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
	if(conf["Global"]["useseed"].as<bool>())
	{
		CLHEP::HepRandom::setTheSeed(conf["Global"]["seed"].as<int>());
	}
	else
	{
		CLHEP::HepRandom::setTheSeed(this->GetTimeNs());
	}
	CLHEP::HepRandom::showEngineStatus();
	std::cout<<"seed: "<<CLHEP::HepRandom::getTheSeed()<<std::endl;

	// Construct the default run manager
	//my Verbose output class
	G4VSteppingVerbose::SetInstance(new SteppingVerbose);
	G4RunManager* runManager = new G4RunManager;


	// set mandatory initialization classes
	//
	DetectorConstruction* detector = new DetectorConstruction(this);
	if(conf["Global"]["savegeo"].as<bool>())
	{
		G4GDMLParser parser;
		if(std::filesystem::exists("vcalo.gdml")){
			std::remove("vcalo.gdml");
		}
		parser.Write("vcalo.gdml",detector->Construct());
	}
	runManager->SetUserInitialization(detector);

	G4VModularPhysicsList* physics = new FTFP_BERT();
	physics->ReplacePhysics(new G4EmStandardPhysics_option4());
	if(conf["Global"]["optical"].as<bool>()){
		physics->RegisterPhysics(new G4OpticalPhysics());
	}
	runManager->SetUserInitialization(physics);

	//SteppingVerbose* stepV = new SteppingVerbose();

	PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector, this);
	runManager->SetUserAction(primary);

	RunAction* runAction = new RunAction(primary,this);
	runManager->SetUserAction(runAction);

	EventAction* eventAction = new EventAction(this);
	runManager->SetUserAction(eventAction);

	TrackingAction* trackingAction = new TrackingAction(runAction,eventAction,this);
	runManager->SetUserAction(trackingAction);

	SteppingAction* steppingAction = new SteppingAction(detector,eventAction);
	runManager->SetUserAction(steppingAction);

	runManager->SetVerboseLevel(conf["Verbose"]["run"].as<int>());
	G4String command = "/control/execute ";

	UI->ApplyCommand(G4String("/control/verbose ")+G4String(conf["Verbose"]["control"].as<std::string>()));
	UI->ApplyCommand(G4String("/tracking/verbose ")+G4String(conf["Verbose"]["tracking"].as<std::string>()));
	UI->ApplyCommand(G4String("/event/verbose ")+G4String(conf["Verbose"]["event"].as<std::string>()));

	G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();
	if(conf["Global"]["vis"].as<bool>()){
		G4UIExecutive* uie = new G4UIExecutive(1,nullptr,"Qt");
		SetupVisualization();
		uie->SessionStart();
		// delete uie;
	}
	//Initialize G4 kernel
	runManager->Initialize();
	if(conf["Global"]["usemac"].as<bool>()){
		if(!ifstream(conf["Global"]["mac"].as<std::string>()).good()){
			std::cout<<"Macro file not found!"<<std::endl;
			throw conf["Global"]["mac"].as<std::string>().c_str();
		}
		std::cout<<"Executing macro file..."<<std::endl;
		UI->ExecuteMacroFile(conf["Global"]["mac"].as<std::string>().c_str());
	}

	runManager->BeamOn(conf["Global"]["beamon"].as<int>());
	
	// job termination
	//
	delete runManager;
	//if(access("vcalo.gdml",F_OK) == 0)std::remove("vcalo.gdml");

	return 1;
}

void Config::SetupVisualization()
{
	UI->ApplyCommand("/vis/viewer/set/autoRefresh false");
	UI->ApplyCommand("/vis/open OGL");
	UI->ApplyCommand("/vis/drawVolume");
	UI->ApplyCommand("/vis/viewer/set/viewpointTheta 45");
	UI->ApplyCommand("/vis/viewer/set/viewpointPhi 45");
	UI->ApplyCommand("/vis/viewer/set/style wireframe");
	UI->ApplyCommand("/vis/viewer/set/background white");
	UI->ApplyCommand("/vis/scene/add/trajectories smooth");
	UI->ApplyCommand("/vis/scene/add/hits");
	UI->ApplyCommand("/vis/scene/add/axes 0 0 1 0.2");
	UI->ApplyCommand("/vis/scene/add/axes 1 0 0 0.2");
	UI->ApplyCommand("/vis/scene/add/axes 0 1 0 0.2");
	UI->ApplyCommand("/vis/scene/add/axes 0 0 0 0.2");
	UI->ApplyCommand("/vis/scene/add/text 0 0 0 0.1 \"Vlast-CALO\" red");
	UI->ApplyCommand("/vis/scene/add/text 0 0 0 0.1 \"Zhen Wang\" red");
	UI->ApplyCommand("/vis/scene/add/text 0 0 0 0.1 \"wangz1996@sjtu.edu.cn\" red");
	return;
}

void Config::Print()
{
	ofstream fout("./default.yaml");
	fout<<"#Project information"<<endl;
	fout<<"Project: Vlast-CALO"<<endl;
	fout<<"Contact: Zhen Wang < wangz1996@sjtu.edu.cn >"<<endl;
	fout<<"\n"<<endl;
	fout<<"Global:"<<endl;
	fout<<"        useseed: False # default: system time in nano-second"<<endl;
	fout<<"        seed: 1234"<<endl;
	fout<<"\n"<<endl;
	fout<<"        usemac: False # Currently not applicable"<<endl;
	fout<<"        mac: ./vis.mac"<<endl;
	fout<<"\n"<<endl;
	fout<<"        output: ./test.root # Output root file name"<<endl;
	fout<<"        beamon: 1"<<endl;
	fout<<"        savegeo: True"<<endl;
	fout<<"        optical: True"<<endl;
	fout<<"\n"<<endl;
	fout<<"#Construct Calorimeter"<<endl;
	fout<<"ECAL:"<<endl;
	fout<<"        ECALShield: True"<<endl;
	fout<<"        build: True"<<endl;
	fout<<"\n"<<endl;
	fout<<"#Particle source setup"<<endl;
	fout<<"Source:"<<endl;
	fout<<"        particle: mu-"<<endl;
	fout<<"        energy: 3000  # Unit is in MeV"<<endl;
	fout<<"        position: [ 0., 0., -100.]   # Unit is in cm"<<endl;
	fout<<"        direction: [ 0., 0., 1.]"<<endl;
	fout<<"\n"<<endl;
	fout<<"#Verbose"<<endl;
	fout<<"Verbose:"<<endl;
	fout<<"        run: 0"<<endl;
	fout<<"        control: 0"<<endl;
	fout<<"        event: 0"<<endl;
	fout<<"        tracking: 0"<<endl;
	fout.close();

}
