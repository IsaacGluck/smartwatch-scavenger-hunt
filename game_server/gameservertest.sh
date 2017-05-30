#!/bin/bash
#
# gameserver.sh - test case for game_server
#
# usage: gameserver.sh
#
# input: n/a
# output: result of the test.
#
# exit:
#   0: succeed
#   1: too many command line arguments
#
# Kazuma Honjo, May 2017

# clean the file and start
make clean
# compile file
make

# if there are any input, it'll be an error
if [ $# -ne 0 ]
then
echo "usage: gameserver.sh"
exit 1
fi

valgrind --leak-check=full ./gameserver FEED ../examples/krags ./secret 2872


echo "finished testing"
exit 0
