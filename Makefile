fs_win:
	swig -c++ -python BigInt.i
	C:\TDM-GCC-64\bin\g++ -c BigInt.cpp
	C:\TDM-GCC-64\bin\g++ -c bigInt_wrap.cxx -IC:\Python27\include
	C:\TDM-GCC-64\bin\g++  -shared BigInt.o BigInt_wrap.o -o _BigInt.pyd
fs_lin:	
	swig -c++ -python BigInt.i
	g++ -fPIC -c BigInt.cpp
	g++ -fPIC -c BigInt_wrap.cxx -I/usr/include/python2.7
	g++ -shared BigInt.o BigInt_wrap.o -o _BigInt.so
	rm BigInt.o BigInt_wrap.cxx BigInt_wrap.o
	
	