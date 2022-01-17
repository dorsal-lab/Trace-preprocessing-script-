#!/bin/sh
traceServerHome=$1
cd $traceServerHome/archiveData
zip TracePackage_$(date +"%Y_%m_%d_%I_%M_%p").zip -r ./
echo Trace package created at:
echo $traceServerHome/archiveData
# Cleaning: 
# deleting directories
find -mindepth 1 -maxdepth 1 -type d -exec rm -r {} \;
# deleting files except the zip file
find . -type f ! -name '*.zip' -delete
