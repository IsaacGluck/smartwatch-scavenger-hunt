#!/bin/bash
#
# File: views_krag.sh
#
# Discription: file for running all
#
# usage: regress.sh dirname testfilename
# where dirname is the name of a directory containing test results
# where each testfilename is the name of a file containing bash command(s) for one test case
#
# Exit: 0 if dirname created successfully, or there were no differences from an existing dirname
#       1 if there were differences from an exitsing dirname, 2 if there were not sufficient arguments
#       3 if dirname is invalid, 4 if invalid file type


inputs=( $"$@" )


# check the number of arguments. if it is less than 2,
# the input is not sufficient
if [ ${#inputs[@]} -lt 2 ]
then
printf "usage: regress.sh dirname testfilename\n"
printf "not enought arguments\n"
exit 2
fi

dirname="$inputs"


# check if the dirname is a directory, if something by the name dirname exists
if find $PWD/$dirname &> /dev/null
then
if [ ! -d "$PWD/$dirname" ]
then
printf "usage: regress.sh dirname testfilename\n"
printf "invalid directory. $inputs is not a directory\n"
exit 3
fi
fi


# check if the files are valid - regular file and redable
for (( i=1; i<${#inputs[@]}; i++))
do
if [ ! -f "${inputs[$i]}" ] || [ ! -r "${inputs[$i]}" ]
then
printf "usage: regress.sh dirname testfilename\n"
printf "wrong file name or file type\n"
exit 4
fi
done


# produce backup files
date=$( date +%Y%m%d.%H%M%S )
echo $date
mkdir -p $PWD/$date

for (( i=1; i<${#inputs[@]}; i++))
do
cp -a "${inputs[$i]}" $date/"${inputs[$i]}".test
bash "${inputs[$i]}" 1> $date/"${inputs[$i]}".stdout 2> $date/"${inputs[$i]}".stderr
echo $? > $date/"${inputs[$i]}".status
done


# if dirname does not exist, output directory is changed to dirname
if [ ! -d "$dirname" ]
then
if mkdir -p $dirname
then
for file in $PWD/$date/*
do
mv $file $PWD/$dirname/$(basename $file)
done
rm -rf $PWD/$date
printf "saved test results in $dirname\n"
exit 0
else
printf "failed to save test results in $dirname; they remain in $date\n"
exit 3
fi

# when the dirname already existed, compare files in YYYYMMDD.HHMMSS to those in dirname
else
difference="0"       # is there a difference? 0: no 1: yes
echo "comparing $date with $dirname..."

# loop over all the files in $date and compare with files in $dirname
for file in $PWD/$date/*
do

# if there is a difference, print the message
if ! diff --brief $file $PWD/$dirname/$(basename $file) &>/dev/null
then
difference="1"

# does the file exist in the $dirname?
if find $PWD/$dirname/$(basename $file) &>/dev/null
then
printf "Files $dirname/$(basename $file) and $date/$(basename $file) differ\n"
else
printf "Only in $date: $(basename $file)\n"
fi
fi
done

# loop over all the files in $dirname and compare with files in @date
# there are cases where files are only in $dirname and not in $date
for file in $PWD/$dirname/*
do

# if there is a difference, print the message
if ! diff --brief $file $PWD/$date/$(basename $file) &>/dev/null
then
difference="1"

# does the file exist in the $dirname?
if ! find $PWD/$date/$(basename $file) &>/dev/null
then
printf "Only in $dirname: $(basename $file)\n"
fi
fi
done



# if no difference, exit with 0. otherwise, exit with 1
if [ $difference -eq "0" ]
then
printf "no differences.\n"
exit 0
fi
exit 1
fi
