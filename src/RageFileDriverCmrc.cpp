/*
 * Ref: https://github.com/vector-of-bool/cmrc
 */

#include "global.h"
#include "RageFileDriverCmrc.h"

#include "RageUtil_FileDB.h"
#include "RageFile.h"
#include "RageLog.h"
#include "RageUtil.h"

using namespace cmrc;

// RageFileObjCmrc

RageFileObjCmrc::RageFileObjCmrc(cmrc::file file)
	:file(file)
{

}

int RageFileObjCmrc::ReadInternal(void* pBuffer, size_t iBytes)
{
	filePosition = min(filePosition, GetFileSize());
	iBytes = min(iBytes, (size_t)GetFileSize() - filePosition);
	if (iBytes == 0)
		return 0;

	memcpy(pBuffer, file.begin() + filePosition, iBytes);
	filePosition += iBytes;

	return iBytes;
}

int RageFileObjCmrc::GetFileSize() const
{
	return file.size();
}

int RageFileObjCmrc::SeekInternal(int offset)
{
	filePosition = clamp(offset, 0, GetFileSize());
	return filePosition;
}

// RageFileDriverCmrc

RageFileDriverCmrc::RageFileDriverCmrc(embedded_filesystem fs)
	:RageFileDriver(new NullFilenameDB),
	fs(fs)
{

}

RageFileBasic* RageFileDriverCmrc::Open(const RString& sPath, int mode, int& err)
{
	if (mode & RageFile::WRITE)
	{
		err = ERROR_WRITING_NOT_SUPPORTED;
		return nullptr;
	}

	RString resourcePath = "resources" + sPath;

	// All resource names are stored in lowercase
	std::for_each(resourcePath.begin(), resourcePath.end(), [](char& c) {
		c = ::tolower(c);
	});

	if (!fs.exists(resourcePath) || fs.is_directory(resourcePath))
	{
		err = ENOENT;
		return nullptr;
	}

	return new RageFileObjCmrc(fs.open(resourcePath));
}

/*
 * Copyright (c) 2003-2005 Glenn Maynard
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
