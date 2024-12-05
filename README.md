/cvmf# VLAST-P
Simulation Framework for VLAST-P

## Contact
Zhen Wang < wangz1996@sjtu.edu.cn >
Borong Peng < pbr7outsider@mail.ustc.edu.cn >
Renjun Wang < miaomaimi@mail.ustc.edu.cn >

## Environment
Use cvmfs
```
	source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/setup.sh
```

## Usage
Run simulation
```
	vcalo -c default.yaml
```

Print default config yaml
```
	vcalo -p
```

Print help message
```
	vcalo -h
```
## How to run :
         -  set enviorment 'source env.sh'
         - mkdir build
         - cd build && cmake .. && make -j10
         - ./vcalo   then    vcalo -h
         - vcalo -p
         - Modify the file default.yaml and then   vcalo -c default.yaml
         - root -l test.root
         - new TBrowser

