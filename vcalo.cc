//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "Config.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

int main(int argc,char** argv) {
 
	Config *config = new Config();
	for(int i=1;i<argc;i++)
	{
		if(std::string(argv[i])==std::string("-c") || std::string(argv[i])==std::string("--config"))
		{
			config->Parse(std::string(argv[i+1]));
		}
		else if(std::string(argv[i])==std::string("-p") || std::string(argv[i])==std::string("--print"))
		{
			config->Print();
			return 1;
		}
		else if(std::string(argv[i])==std::string("-h") || std::string(argv[i])==std::string("--help"))
		{
			std::cout<<"*************************************"<<std::endl;
			std::cout<<"It is a help message"<<std::endl;
			std::cout<<"*************************************"<<std::endl;
			std::cout<<"Use vcalo -p to produce a default.yaml"<<std::endl;
			std::cout<<"*************************************"<<std::endl;
			std::cout<<"Use vcalo -c default.yaml to load yaml"<<std::endl;
			std::cout<<"*************************************"<<std::endl;
			std::cout<<"end"<<std::endl;
			std::cout<<"*************************************"<<std::endl;
			return 1;
		}
		else
		{
		}
	}
	if(!config->IsLoad())
	{
		std::cout<<"No config loaded! Use calo -h(--help) to view help message"<<std::endl;
		throw "d";
	}
	else
	{
		config->Run();
	}
	return 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo..... 

