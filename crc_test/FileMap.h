#ifndef FileMap_h
#define FileMap_h

#include <windows.h>

#define CFILE_INIT hFile(INVALID_HANDLE_VALUE), hMap(0), lpMapView(0), dwSize(0)

class FileMap {

private:
	HANDLE hFile, hMap;
	LPVOID lpMapView;
	DWORD dwSize;


public:
	FileMap() : CFILE_INIT { }
	FileMap( LPCTSTR path, BOOL write = FALSE ) : CFILE_INIT
	{
		Open(path, write);
	}
	FileMap(LPCTSTR path, DWORD size) : CFILE_INIT
	{

		if (size)
			Create(path, size);
	}
	~FileMap()
	{
		Close();
	}

	operator BOOL()
	{
		return (hFile != INVALID_HANDLE_VALUE);
	}

	BOOL Open(LPCTSTR path, BOOL write)
	{

		return write ? ( OpenInternal( path, GENERIC_READ | GENERIC_WRITE, OPEN_EXISTING,
			PAGE_READWRITE, FILE_MAP_READ | FILE_MAP_WRITE ) ) :
		OpenInternal( path, GENERIC_READ, OPEN_EXISTING, PAGE_READONLY, FILE_MAP_READ );
	}

	BOOL Create(LPCTSTR path, DWORD size)
	{
		return OpenInternal(path, GENERIC_READ | GENERIC_WRITE, CREATE_ALWAYS,
			PAGE_READWRITE, FILE_MAP_WRITE, size);
	}

	BOOL Close(DWORD newsize = 0)
	{
		if (hMap)
			CloseHandle(hMap);

		hMap = 0;

		if (lpMapView)
			UnmapViewOfFile(lpMapView);

		lpMapView = 0;

		if (newsize > 0 && newsize < dwSize)
		{
			//переводим указатель
			DWORD result = SetFilePointer(hFile, newsize, 0, FILE_BEGIN);

			if (result != newsize)
				return FALSE;

			//устанавливаем новый конец файла
			if ( !SetEndOfFile(hFile) )
				return FALSE;
		}

		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);

		hFile = INVALID_HANDLE_VALUE;

		dwSize = 0;

		return TRUE;
	}

	inline  BYTE* Base() { return reinterpret_cast<BYTE*>(lpMapView); }

	inline  DWORD Size() { return dwSize;}

	inline BOOL OpenInternal(LPCTSTR path, DWORD dwAccess, DWORD dwCreation,
		DWORD flProtect, DWORD dwPageAccess,
		DWORD size = 0)
	{
		if (hFile != INVALID_HANDLE_VALUE)
			Close();

		hFile = CreateFile( path, dwAccess, FILE_SHARE_READ, 0, dwCreation,
			FILE_ATTRIBUTE_NORMAL, 0 );

		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		if (!size)
			size = GetFileSize(hFile, 0);

		hMap = CreateFileMapping(hFile, 0, flProtect, 0, size, 0);

		if (!hMap)
		{
			Close();
			return FALSE;
		}

		lpMapView = MapViewOfFile(hMap, dwPageAccess, 0, 0, size);

		if (!lpMapView)
		{
			Close();
			return FALSE;
		}

		dwSize = size;
		return TRUE;
	}

};


#endif