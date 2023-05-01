#!/bin/bash

directory=$1

findsize_ls=$(ls -ld $directory |awk '{print $5'})
echo "$findsize_ls"

findsize_du=$(du -k $directory |awk '{print $1'})
echo "$findsize_du"

if [ $findsize_du -ge 1000000 ]
then echo $"High" ${directory} >> ~/ListOfBigDirs.txt

elif [ $findsize_du -lt 128000 ]
then echo $"Low"  ${directory} >> ~/ListOfBigDirs.txt

elif  [ $findsize_du -lt 1000000 ]
then echo $"Medium"\n

fi

# $ du file.txt
# 2469136 file.txt

# $ du -k file.txt
# 1234568 file.txt

# $ du -m file.txt
# 1206    file.txt

# $ du -g file.txt
# 2   file.txt

# $ du -h file.txt
# 1.2G    file.txt