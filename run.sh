#!/bin/sh
echo 'Trying to kill last server instance'
kill -9 `cat current_pid`
wait `cat current_pid`
echo 'Done'
echo 'Building Server'
make -j 4
if [ $? -eq 0 ]
then
    ../validateMultiByte.exe ./
    if [ $? -eq 1 ]
    then
    echo 'Starting Server (no multibyte detected)'
    ./serv.exe &
    echo $! > current_pid
    else
    echo 'Multi Byte Char Detected Please Fix Before Continuing'
    fi
else
echo 'Compilation Failed'
fi