
cd mongo-c-driver-1.19.0
cd build



sysOS=`uname -s`

if [ $sysOS == "Linux" ];then
cmake ..
make 
sudo make install

elif [ $sysOS == "Darwin" ];then
cmake ..
make 
fi


cp ./src/libbson/*.a  ../../lib/Debug/
cp ./src/libbson/*.a ../../lib/Release/
cp ./src/libbson/*.so ../../lib/Debug/
cp ./src/libbson/*.so ../../lib/Release/
cp ./src/libmongoc/*.a ../../lib/Debug/
cp ./src/libmongoc/*.a ../../lib/Release/
cp ./src/libmongoc/*.so ../../lib/Debug/
cp ./src/libmongoc/*.so ../../lib/Release/

cd ../../
