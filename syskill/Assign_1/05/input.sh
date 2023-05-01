#!/bin/bash

######################################
input_month=$1

monthnumber() {
 	month=$1
    	months="JanFebMarAprMayJunJulAugSepOctNovDec"
    	tmp=${months%%$month*}
    	month=${#tmp}
    	monthnumber=$((month/3+1))
    	printf "%02d\n" $monthnumber
}
arr=(`echo ${input_month}`);
month=$(monthnumber ${arr[0]})
#echo "$month"

#####################################


while IFS="," read -r firstName lastName birthDate
do
#        echo "Firstname: $firstName"
#        echo "Lastname: $lastName"
#        echo "Birth date: $birthDate"

        Year=$(echo birthDate | cut -c 1-4)
        Month=$(echo $birthDate | cut -c 5-6)
        Date=$(echo $birthDate | cut -c 6-7)

        echo "$Month"
done < guest.csv


#####################################

result=$( grep -e $month guest.csv) 
#result=$( grep -w $month guest.csv) 

if [ -n "$result" ]; then
    printf '%s\n' "$result"
else
    printf 'No match found for pattern'
fi
