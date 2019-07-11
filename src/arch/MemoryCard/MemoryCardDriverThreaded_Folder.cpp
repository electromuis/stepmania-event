#include "global.h"
#include "MemoryCardDriverThreaded_Folder.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "PlayerNumber.h"
#include "MemoryCardManager.h"

#include <limits.h>
#include <stdlib.h>
#include <bitset>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

static int g_currentSerial = 0;

MemoryCardDriverThreaded_Folder::MemoryCardDriverThreaded_Folder()
{
}

MemoryCardDriverThreaded_Folder::~MemoryCardDriverThreaded_Folder()
{
}

time_t MemoryCardDriverThreaded_Folder::FolderTime(RString path)
{
	if (path.empty()) {
		return NULL;
	}

	const char *pathname = path.c_str();
	struct stat info;

	int statRC = stat( pathname, &info );
	if( statRC != 0 )
	{
		if (errno == ENOENT)  { return NULL; } // something along the path does not exist
		if (errno == ENOTDIR) { return NULL; } // something in path prefix is not a dir
		return NULL;
	}

	if( info.st_mode & S_IFDIR ) {
		return info.st_ctime;
	}

	return NULL;
}

bool MemoryCardDriverThreaded_Folder::TestWrite( UsbStorageDevice* pDevice )
{
	//TODO

	return true;
}

void MemoryCardDriverThreaded_Folder::ReadActivePlayerTimes()
{
	FOREACH_PlayerNumber( p )
	{
		const RString folder = MEMCARDMAN->m_sMemoryCardOsMountPoint[p];
		m_NewDevices[p] = FolderTime(folder);
	}
}

bool MemoryCardDriverThreaded_Folder::USBStorageDevicesChanged()
{
	ReadActivePlayerTimes();

	FOREACH_PlayerNumber(p)
	{
		if (m_NewDevices[p] != m_LastDevices[p]) {
			LOG->Trace("CHANGEEEE");
			return true;
		}
	}

	return false;
}

void MemoryCardDriverThreaded_Folder::GetUSBStorageDevices( vector<UsbStorageDevice>& vDevicesOut )
{
	LOG->Trace( "GetUSBStorageDevices" );
	vDevicesOut.clear();

	FOREACH_PlayerNumber( p )
	{	
		if (m_NewDevices[p] != NULL) {
			UsbStorageDevice usbd;
			g_currentSerial++;
			usbd.sSerial = StringConversion::ToString(g_currentSerial);
			usbd.sSysPath = MEMCARDMAN->m_sMemoryCardOsMountPoint[p];
			usbd.sOsMountDir = MEMCARDMAN->m_sMemoryCardOsMountPoint[p];

			vDevicesOut.push_back(usbd);
		}
		
		m_LastDevices[p] = m_NewDevices[p];
	}
}

bool MemoryCardDriverThreaded_Folder::Mount( UsbStorageDevice* pDevice )
{
	return true;
}

void MemoryCardDriverThreaded_Folder::Unmount( UsbStorageDevice* pDevice )
{
	return;
}

/*
 * (c) 2018-2019 Electromuis
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
