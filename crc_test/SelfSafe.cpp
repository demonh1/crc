#include "SelfSafe.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include "CrcTable.h"


class FileMap;

//void dbgPrint( int line, int idx, int size ) {
//    char str[512];
//    sprintf( str, "%d %d / %d\n", line, idx, size );
//    OutputDebugStringA(str);
//}

SelfSafe::SelfSafe( )
{ }

SelfSafe::SelfSafe(HMODULE hmod)
{
	newFile(hmod);
}

SelfSafe::SelfSafe(std::string fname)
{
	newFile(fname);
}


std::string SelfSafe::getErrStrOem() const
{ 
	DWORD len = errstrm.str().size();

	if (len == 0)  return " ";

	else
	{
		std::vector<char> oem;
		oem.reserve(errstrm.str().size() + 1);
		//dbgPrint( __LINE__, 1, oem.size() ); 

		::CharToOemBuffW((LPCWSTR)errstrm.str().c_str(), &oem[0], len);
		return std::string(oem.begin(), oem.begin() + len);
	}
}

void SelfSafe::newFile(HMODULE hmod){
	if ( targetFile )
		targetFile.Close();

	char FILENAME[ _MAX_PATH ];
	::GetModuleFileName(hmod, LPWSTR(FILENAME), sizeof(FILENAME) );


	fileName = FILENAME;
}


void SelfSafe::newFile(std::string fname){
	if ( targetFile )
		targetFile.Close();

	// проверяем на относительное имя
	char drive[_MAX_DRIVE], 
		dir[_MAX_DIR], 
		name[_MAX_FNAME],
		ext[_MAX_EXT], 
		current_dir[_MAX_PATH];

	_splitpath(fname.c_str(), drive, dir, name, ext);

	if (strlen(drive) == 0)
	{   
		::GetCurrentDirectory( sizeof (current_dir), (LPWSTR)current_dir );
		fileName = current_dir + std::string("\\") + fname;
	}
	else
	{       // задан полный путь к файлу
		fileName = fname;
	}
}


BOOL SelfSafe::checkCRC() {

	BYTE* crcpos = nullptr;

	BOOL res = FALSE;

	// ищем место, где в файле записана CRC
	if ( openFileAndFindCRCpos(&crcpos) )
	{
		// нашли, сравним то что записано в файле с
		// CRC посчитанной сейчас
		if ( *reinterpret_cast<DWORD*>(crcpos) == synCRC(crcpos) ) res = TRUE;

		else
		{
		
			errstrm.clear();
			errstrm.str(" ");
			errstrm << "File '" << fileName << "': wrong value CRC";
			res = FALSE;
		}
	}
	else
	{
		res = FALSE;
	}

	targetFile.Close();

	return res;

}

BOOL SelfSafe::writeCRC() {

	BYTE * crcpos = nullptr;

	// результат записи CRC
	BOOL ret = FALSE;

	// ищем место, куда в файл нужно записать CRC
	if ( openFileAndFindCRCpos(&crcpos, TRUE) )
	{
		*reinterpret_cast<DWORD*>(crcpos) = synCRC(crcpos);
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}

	targetFile.Close();

	return ret;
}

DWORD SelfSafe::synCRC(BYTE* crcpos) {
	
	DWORD crc = std::accumulate( targetFile.Base(),
		crcpos,
		(DWORD) 0, 
		CrcTable::updateCRC ); 

	
	return std::accumulate( crcpos + sizeof(DWORD),  
		targetFile.Base() + targetFile.Size(),  
		crc,          
		CrcTable::updateCRC );  
}

BOOL SelfSafe::openFileAndFindCRCpos( BYTE** crcpos, BOOL toWrite) {
	if (!targetFile.Open((LPCTSTR )fileName.c_str(),toWrite)) {

		errstrm.clear();
		errstrm.str(" ");
		errstrm << "Can not open file " << fileName << " ";

		return FALSE;
	}

	BYTE* file_end = targetFile.Base() + targetFile.Size();

	
	BYTE* label_start = std::search( targetFile.Base(),
		file_end,
		CrcTable::CrcData.label,
		CrcTable::CrcData.label + sizeof(CrcTable::CrcData.label) );

	if ( label_start == file_end )
	{
		errstrm.clear();
		errstrm.str(" ");
		errstrm << "In file '" << fileName 
			<< "' is not found storage CRC";
		return FALSE;
	}

	
	*crcpos = label_start + sizeof(CrcTable::CrcData.label);

	if ( ( *crcpos + sizeof(DWORD) ) > file_end )
	{
		errstrm.clear();
		errstrm.str(" ");
		errstrm << "Invalid CRC storage space in file '" 
			    << fileName << "'";
		return FALSE;
	}

	
	if ( std::search( label_start + sizeof(CrcTable::CrcData.label),
		file_end,
		CrcTable::CrcData.label,
		CrcTable::CrcData.label + sizeof(CrcTable::CrcData.label) ) != file_end )
	{
	
		errstrm.clear();
		errstrm.str( "" );
		errstrm << "In file '" << fileName
			    << "' found 2 places to store CRC, there should be one only ";
		return FALSE;
	}

	return TRUE;
}
