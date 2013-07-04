#pragma once

#include <sstream>
#include <cstring>
#include "FileMap.h"


class SelfSafe {

public:
        // конструктор
		 SelfSafe();
		 SelfSafe(HMODULE hmod); 
		 SelfSafe(std::string name); 
        
        void newFile(std:: string fname);
        void newFile(HMODULE hmod);

		~SelfSafe( ) { } ;

        // получить строку с сообщением об ошибке
         std::string SelfSafe::getErrStrAnsi() const  { return errstrm.str(); }
        // получить строку с сообщением об ошибке (для консольных приложений)
        std::string getErrStrOem() const;

        BOOL checkCRC();
        BOOL writeCRC();

protected:
        // имя проверяемого файла
        std::string fileName;
        // сообщение об ошибке
		std::stringstream errstrm;
        // обработываемый файл, отображенный в память
        FileMap targetFile;

        // найти место для CRC в файле
        BOOL openFileAndFindCRCpos(BYTE** crcpos, BOOL toWrite = FALSE);
        // посчитать CRC файла исключая собственно значение CRC в позиции crcpos
        DWORD synCRC (BYTE* crcpos);
};

