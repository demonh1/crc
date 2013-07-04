#pragma once

#include <sstream>
#include <cstring>
#include "FileMap.h"


class SelfSafe {

public:
        // �����������
		 SelfSafe();
		 SelfSafe(HMODULE hmod); 
		 SelfSafe(std::string name); 
        
        void newFile(std:: string fname);
        void newFile(HMODULE hmod);

		~SelfSafe( ) { } ;

        // �������� ������ � ���������� �� ������
         std::string SelfSafe::getErrStrAnsi() const  { return errstrm.str(); }
        // �������� ������ � ���������� �� ������ (��� ���������� ����������)
        std::string getErrStrOem() const;

        BOOL checkCRC();
        BOOL writeCRC();

protected:
        // ��� ������������ �����
        std::string fileName;
        // ��������� �� ������
		std::stringstream errstrm;
        // �������������� ����, ������������ � ������
        FileMap targetFile;

        // ����� ����� ��� CRC � �����
        BOOL openFileAndFindCRCpos(BYTE** crcpos, BOOL toWrite = FALSE);
        // ��������� CRC ����� �������� ���������� �������� CRC � ������� crcpos
        DWORD synCRC (BYTE* crcpos);
};

