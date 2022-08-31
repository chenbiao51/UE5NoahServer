
cd mongo-cxx-driver-r3.6.7
cd build

sysOS=`uname -s`

if [ $sysOS == "Linux" ];then
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
sudo make 
sudo make install


elif [ $sysOS == "Darwin" ];then
cmake ..
make 
fi


# cp ./src/bsoncxx/*.a  ../../lib/Debug/
# cp ./src/bsoncxx/*.a ../../lib/Release/
cp ./src/bsoncxx/*.so ../../lib/Debug/
cp ./src/bsoncxx/*.so ../../lib/Release/
# cp ./src/mongocxx/*.a ../../lib/Debug/
# cp ./src/mongocxx/*.a ../../lib/Release/
cp ./src/mongocxx/*.so ../../lib/Debug/
cp ./src/mongocxx/*.so ../../lib/Release/

cd ../../
