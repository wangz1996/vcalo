#!/bin/bash
vcalo -c default.yaml
root -l -b -q save.cxx
scp ECALShield.json zhenw@lxplus.cern.ch:/eos/user/z/zhenw/website/time_range
