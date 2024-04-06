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
