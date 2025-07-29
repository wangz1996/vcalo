# VLAST-P
Simulation and Analysis Framework for VLAST-P

## Contact
Zhen Wang < wangz1996@sjtu.edu.cn, wangz1996@ustc.edu.cn, zhenw@cern.ch >

Jiaxuan Wang < wjx1019@mail.ustc.edu.cn >

Borong Peng < pbr7outsider@mail.ustc.edu.cn >

Renjun Wang < miaomaimi@mail.ustc.edu.cn >
## Environment
Using cvmfs
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

Run analysis (test.root as the simulation output should be in the directory)
```
	vana
```

