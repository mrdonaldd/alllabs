#!/bin/bash


#Get value from key
input_month=$1

#Change month abbreviation to month number
if [ "$input_month" = "Jan" ]; 
then
  MON_NUM=01
elif [ "$input_month" = "Feb" ]; 
then
  MON_NUM=02
elif [ "$input_month" = "Mar" ]; 
then
  MON_NUM=03
elif [ "$input_month" = "Apr" ]; 
then
  MON_NUM=04
elif [ "$input_month" = "May" ]; 
then
  MON_NUM=05
elif [ "$input_month" = "Jun" ]; 
then
  MON_NUM=06
elif [ "$input_month" = "Jul" ]; 
then
  MON_NUM=07
elif [ "$input_month" = "Aug" ]; 
then
  MON_NUM=08
elif [ "$input_month" = "Sep" ]; 
then
  MON_NUM=09
elif [ "$input_month" = "Oct" ]; 
then
  MON_NUM=10
elif [ "$input_month" = "Nov" ]; 
then
  MON_NUM=11
elif [ "$input_month" = "Dec" ]; 
then
  MON_NUM=12
else
  MON_NUM=00
fi

#####################################


# while IFS="," read -r firstName lastName birthDate
# do
#        echo "Firstname: $firstName"
#        echo "Lastname: $lastName"
#        echo "Birth date: $birthDate"

#         Year=$(echo $birthDate | cut -c 1-4)
#         Month=$(echo $birthDate | cut -c 5-6)
#         Date=$(echo $birthDate | cut -c 6-7)

#         echo "$Month"
# done < guest.csv

#####################################

result=$( grep -e $MON_NUM guest.csv) 

if [ -n "$result" ]; then
    printf '%s\n' "$result"
else
    printf 'No match found for pattern'
fi


YMD=$(cat $2 | awk '{print $3'}) 
Year=$(echo $YMD | cut -c 1-4)
Month=$(echo $YMD | cut -c 5-6)
Date=$(echo $YMD | cut -c 6-7)

echo "$Month"

