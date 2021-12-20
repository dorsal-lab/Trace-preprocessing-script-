#!/bin/sh
tracesDirPath=$1
traceServerHome=$2
cd $traceServerHome
mkdir -p archiveData/.tracing
echo Copying files ...
cp -r $traceServerHome/Tracing/.tracing$tracesDirPath/. $traceServerHome/archiveData/.tracing
cp -r $traceServerHome/Tracing/Traces$tracesDirPath/. $traceServerHome/archiveData
echo Files Copied.



