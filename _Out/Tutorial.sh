#!/bin/bash

export LC_ALL="C"
echo "ulimit -c unlimited" >> /etc/profile


sysOS=`uname -s`
if [ $sysOS == "Darwin" ];then
    echo "I'm MacOS"
    export DYLD_LIBRARY_PATH=.:$DYLD_LIBRARY_PATH
elif [ $sysOS == "Linux" ];then
    echo "I'm Linux"
    export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
else
    echo "Other OS: $sysOS"
fi


cd Release

chmod -R 777  NFServer

./NFServer -d Plugin=Tutorial.xml Server=NFGameServer ID=6



ps -A|grep NF