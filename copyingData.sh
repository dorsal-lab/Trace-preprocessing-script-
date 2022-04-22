#!/bin/sh
tracesDirPath=$1
traceServerHome=$2

FILE=$tracesDirPath/metadata
cd $traceServerHome
mkdir -p archiveData/.tracing
if test -f "$FILE"; then
    cp -r $traceServerHome/Tracing/Traces$tracesDirPath $traceServerHome/archiveData
    cp -r $traceServerHome/Tracing/.tracing$tracesDirPath $traceServerHome/archiveData/.tracing
else    
    cp -r $traceServerHome/Tracing/Traces$tracesDirPath/. $traceServerHome/archiveData
    cp -r $traceServerHome/Tracing/.tracing$tracesDirPath/. $traceServerHome/archiveData/.tracing
fi


