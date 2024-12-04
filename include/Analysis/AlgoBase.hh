#ifndef ALGOBASE_HH
#define ALGOBASE_HH
//Pure virtual base class of algorithms
#include "TFile.h"
#include "TTree.h"
#include <string>

template<class T>
class AlgoBase{
public:
    virtual int init() = 0;
    virtual int run() = 0;
    virtual std::string getName(){return m_name;}
    virtual ~AlgoBase() = default;

private:
    std::string m_name;
    std::unique_ptr<TFile> fFile;
    std::unique_ptr<TTree> fTree;
};

#endif