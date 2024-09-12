#!/bin/bash
/bin/time -v -p vcalo -c default.yaml #> log 2>&1
#root -l -b -q save.cxx
#scp vcalo.json zhenw@lxplus.cern.ch:/eos/user/z/zhenw/website/time_range
