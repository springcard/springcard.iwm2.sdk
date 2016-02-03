@echo off
pushd %0\..
call i:\builder\JDK7u51_x64\setenv.cmd
rm *.class
make
popd
pause
