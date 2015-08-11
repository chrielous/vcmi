/*
 * CZipSaver.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
 
#include "StdInc.h"
#include "CZipSaver.h"

///CZipOutputStream
CZipOutputStream::CZipOutputStream(zipFile archive, const std::string & archiveFilename):
	handle(archive)
{
	//zip_fileinfo fileInfo;
	
	zipOpenNewFileInZip(handle,
                       archiveFilename.c_str(),
                       nullptr,//todo: use fileInfo,
                       nullptr,
                       0,
                       nullptr,
                       0,
                       "",
                       Z_DEFLATED,
                       Z_DEFAULT_COMPRESSION);
}

CZipOutputStream::~CZipOutputStream()
{
	zipCloseFileInZip(handle);
}


si64 CZipOutputStream::write(const ui8 * data, si64 size)
{
	int ret = zipWriteInFileInZip(handle, (const void*)data, (unsigned)size);
	
	if (ret == ZIP_OK)
		return size;
	else
		return 0;
}

///CZipSaver
CZipSaver::CZipSaver(std::shared_ptr<CIOApi> api, const std::string & path):
	ioApi(api),
	zipApi(ioApi->getApiStructure()),
	handle(nullptr)	
{
	
	
	handle = zipOpen2_64(path.c_str(), APPEND_STATUS_CREATE, nullptr, &zipApi);
	
	if (handle == nullptr)
		throw new std::runtime_error("Failed to create archive");
}

CZipSaver::~CZipSaver()
{
	if(handle != nullptr)
		zipClose(handle, nullptr);
}

std::unique_ptr<COutputStream> CZipSaver::addFile(const std::string & archiveFilename)
{
	if(activeStream != nullptr)
		throw new std::runtime_error("CZipSaver::addFile: stream already opened");
	
	std::unique_ptr<COutputStream> stream(new CZipOutputStream(handle, archiveFilename));
	
	activeStream = stream.get();
	
	return stream;
}
