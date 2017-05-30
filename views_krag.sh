#!/bin/bash
#
# File: views_krag.sh
#
# Discription:
#   Create the basis for running three commands
#   Then run the gameserver on port 2872
#
# usage: views_krag.sh
#
# Exit:
#   0 if no error
#   1 too mant arguments


# check the number of arguments. if it is not 0 error
if [ $# -ne 0 ]
then
printf "views_krag.sh\n"
printf "too many arguments\n"
exit 1
fi

dirname="./logs/"

# check if the dirname is a directory, if something by the name dirname exists
if find $PWD/$dirname &> /dev/null
then
if [ ! -d "$PWD/$dirname" ]
then
printf "'logs' that is not a directory exists\n"
exit 2
fi
fi


# make the directory if it does not exist

if [ ! -d "$dirname" ]
then
mkdir -p "$dirname"
fi

touch "$dirname"/gameserver.log
touch "$dirname"/guideagent.log
touch "$dirname"/fieldagents.log

make

cd ./game_server
./gameserver FEED ./kiff ./secret 2872
