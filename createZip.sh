#!/bin/sh
traceServerHome=$1
cd $traceServerHome/archiveData
zip -q TracePackage_$(date +"%Y_%m_%d_%I_%M_%p").zip -r ./
# Cleaning: 
# deleting directories and their simlinks
find -mindepth 1 -maxdepth 1 -type d -exec rm -r {} \;
find -mindepth 1 -maxdepth 1 -type l -delete
# deleting files except the zip file
find . -type f ! -name '*.zip' -delete
