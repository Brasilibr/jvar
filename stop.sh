#!/bin/sh
echo 'Trying to kill last server instance'
kill -9 `cat current_pid`
wait `cat current_pid`
echo 'Done'
