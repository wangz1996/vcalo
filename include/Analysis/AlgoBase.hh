#ifndef ALGOBASE_HH
#define ALGOBASE_HH
//Pure virtual base class of algorithms 
//only init() and run() need to be implemented
#include "TFile.h"
#include "TTree.h"
#include <string>

template<class T>
class AlgoBase{
public:
    AlgoBase(const std::string name){m_name=name;}
    virtual int init(TTree* tin,TTree* tout) = 0;
    virtual int run(const int& ientry) = 0;
    virtual std::string getName() const {return m_name;}
    virtual ~AlgoBase() = default;

protected:
    std::string m_name;
};

#endif