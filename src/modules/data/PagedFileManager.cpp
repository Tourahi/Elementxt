#include <cstdio>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>

#include "modules/data/PagedFIleManager.hpp"

Data::PagedFileManager* Data::PagedFileManager::_pfm = NULL;

Data::PagedFileManager* Data::PagedFileManager::instance()
{
  if (!_pfm)
    _pfm = new Data::PagedFileManager();

  return _pfm;
}

Data::PagedFileManager::~PagedFileManager()
{
  delete _pfm;
}


bool Data::PagedFileManager::fileExists(const std::string &fn)
{
  struct stat sb;
  return stat(fn.c_str(), &sb) == 0;
}


Data::Status Data::PagedFileManager::createFile(const std::string &fn)
{
  if (fileExists(fn))
    return PFM_FILE_EXISTS;

  FILE *pFile = fopen(fn.c_str(), "wb");

  if (pFile == NULL)
    return PFM_OPEN_FAILED;

  fclose(pFile);
  return SUCCESS;
}


Data::Status Data::PagedFileManager::purgeFile(const std::string &fn)
{
  if (remove(fn.c_str()) != 0)
    return PFM_OPEN_FAILED;

  return SUCCESS;
}

Data::Status Data::PagedFileManager::openFile(const std::string &fn, Data::FileHandle &fileH)
{
  if (fileH.getfd() != nullptr)
    return PFM_HANDLE_IN_USE;

  if (!fileExists(fn))
    return PFM_FILE_DN_EXIST;

  FILE *pFile;
  pFile = fopen(fn.c_str(), "rb+");

  if (pFile == nullptr)
    return PFM_OPEN_FAILED;

  fileH.setfd(pFile);

  return SUCCESS;
}


Data::Status Data::PagedFileManager::closeFile(Data::FileHandle &fileH)
{
  FILE *pFile = fileH.getfd();

  if (pFile == nullptr)
    return 1;

  fclose(pFile);

  fileH.setfd(nullptr);

  return SUCCESS;
}

Data::FileHandle::FileHandle()
{
  readCtr = 0;
  writeCtr = 0;
  appendCtr = 0;

  _fd = nullptr;
}


Data::FileHandle::~FileHandle()
{
  _fd = nullptr;
}

void Data::FileHandle::setfd(FILE* fd)
{
  _fd = fd;
}

FILE *Data::FileHandle::getfd()
{
  return _fd;
}


unsigned Data::FileHandle::getNumberOfPages()
{
  struct stat sb;
  if (fstat(fileno(_fd), &sb) != 0)
    return 0;

  return sb.st_size / PAGE_SIZE;
}

Data::Status Data::FileHandle::readPage(Data::PageNum pageNum, void *data)
{
  if (getNumberOfPages() < pageNum)
    return FH_PAGE_DN_EXIST;

  if (fseek(_fd, PAGE_SIZE * pageNum, SEEK_SET))
    return FH_SEEK_FAILED;

  if (fread(data, 1, PAGE_SIZE, _fd) != PAGE_SIZE)
    return FH_READ_FAILED;

  readCtr++;

  return SUCCESS;
}

Data::Status Data::FileHandle::writePage(Data::PageNum pageNum, const void *data)
{
  if (getNumberOfPages() < pageNum)
    return FH_PAGE_DN_EXIST;

  if (fseek(_fd, PAGE_SIZE * pageNum, SEEK_SET))
    return FH_SEEK_FAILED;

  if (fwrite(data, 1,PAGE_SIZE, _fd) == PAGE_SIZE)
  {
    fflush(_fd);
    writeCtr++;
    return SUCCESS;
  }

  return FH_WRITE_FAILED;
}

Data::Status Data::FileHandle::appendPage(const void *data)
{
  if (fseek(_fd, 0, SEEK_END))
    return FH_SEEK_FAILED;

  if (fwrite(data, 1,PAGE_SIZE, _fd) == PAGE_SIZE)
  {
    fflush(_fd);
    appendCtr++;
    return SUCCESS;
  }

  return FH_WRITE_FAILED; 
}
