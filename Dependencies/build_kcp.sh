


sysOS=`uname -s`

if [ $sysOS == "Linux" ];then
cd kcp
cmake ./
make 


cp ./ikcp.h ../vcpkg/installed/x64-linux/include/

cp ./*.a ../lib/Debug/
cp ./*.a ../lib/Release/

cd ../

fi