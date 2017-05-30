#!/bin/bash
#
# cgameserver.sh - test case for game_server
#
# usage: gameserver.sh
#
# input: n/a
# output: result of the test.
#
# exit:
#   0: succeed
#
#
# Kazuma Honjo, May 2017

# clean the file and start
make clean
# compile file
make

# if there are any input, it'll be an error
if [ $# -ne 0 ]
then
echo "usage: crawlertest.sh"
exit 1
fi

valgrind --leak-check=full ./gameserver FEED ./kiff ./secret 2872
