#!/bin/bash
DIR=`pwd`
if [ -d jobs ];then
    rm -rf jobs;
fi
mkdir jobs
for ((i=10;i<=20;i++));do
    cp -r $DIR/temp $DIR/jobs/$i
    cd $DIR/jobs/$i
    sed -i "s:PATHHERE:$DIR/jobs/$i:g" $DIR/jobs/$i/run.sh
    sed -i "s:THICKNESS:$i:g" $DIR/jobs/$i/default.yaml
    condor_submit run.condor
    cd $DIR
done
