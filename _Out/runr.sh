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

./NFServer -d Plugin=Plugin.xml Server=MasterServer ID=3

sleep 1

./NFServer -d Plugin=Plugin.xml Server=WorldServer ID=7

sleep 1

./NFServer -d Plugin=Plugin.xml Server=DBServer ID=8

sleep 1

./NFServer -d Plugin=Plugin.xml Server=LoginServer ID=4

sleep 1

./NFServer -d Plugin=Plugin.xml Server=GameServer ID=16001

sleep 1

./NFServer -d Plugin=Plugin.xml Server=ProxyServer ID=5

sleep 1

./NFServer -d Plugin=Plugin.xml Server=TestServer ID=6

sleep 5

sudo ufw disable

ps -A|grep NF
