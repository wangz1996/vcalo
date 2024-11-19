//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "VAnaManager.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

int main(int argc,char** argv) {
	if(argc!=2){
		std::cout<<"Wrong arguments! Use vana -h(--help) to view help message"<<std::endl;
		return 1;
	}
	std::string arg(argv[1]);
	VAnaManager *vana = new VAnaManager();
	vana->setInputFile(arg);
	if(!vana->run()){
		std::cout<<"Error in running vana"<<std::endl;
		std::cerr<<"Exiting"<<std::endl;
	}
	return 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo..... 

