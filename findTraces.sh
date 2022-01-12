#!/bin/sh

#The above finds all files below the given directory "traces_folder" that are regular files (-type f) 
#and have  "metadata" as a  name (-name 'metadata'). Next, sed removes  the file name  from the path, 
#leaving just the directory name. Then, the list of directories  is sorted (sort) and  duplicates are 
#removed with (uniq). The result is written then into a text file using (tee).
traces_folder=$1
find $traces_folder -type f -name 'metadata' | sed -r 's|/[^/]+$||' |sort |uniq | tee $traces_folder/file.txt > /dev/null



