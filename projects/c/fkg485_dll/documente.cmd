@echo off
pushd %0\..

rm -Rf doc
mkdir doc

rm -Rf tmp
mkdir tmp

copy inc\*.h tmp
copy src\*.c tmp

robodoc.exe --src ./tmp --doc ./doc --multidoc --html --index --toc --folds

rm -Rf tmp

pause
