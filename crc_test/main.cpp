#include <cstdlib>
#include <iostream>
#include "SelfSafe.h"


// test
int main (int argc, char* argv[]) {

	SelfSafe selfs;

	if (argc == 1){
		char name[_MAX_PATH];

		::GetModuleFileNameW(NULL,(LPWSTR) name, sizeof(name));

		selfs.newFile (name);

		if ( !selfs.checkCRC() ) 
		{
			std::cout << selfs.getErrStrOem() << std::endl;

			exit(1);
		}
		else
		{
			std::cout << "CRC in file " << name << " is cheked out!" << std::endl;
		}

	}

	else
	{
		selfs.newFile(argv[1]);

		std::cout << "Writing CRC into file " << argv[1] << "..." << std::endl;

		if ( !selfs.writeCRC() )
		{
			std::cout << selfs.getErrStrOem() << std::endl;

			exit(2);
		}
		else
		{
			std::cout << "CRC is written into file " << argv[1] << " !" << std::endl;
		}

		std::cout << "Checking CRC of " << argv[1] << "file..." << std::endl;

		if ( !selfs.checkCRC() )
		{
			std::cout << selfs.getErrStrOem() << std::endl;

			exit(3);
		}
		else
		{
			std::cout << "CRC in file " << argv[1] << " is cheñked out!" << std::endl;
		}
	}

	return 0;
}