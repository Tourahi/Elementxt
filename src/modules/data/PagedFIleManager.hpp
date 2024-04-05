#pragma once

#include <string>
#include <climits>

#define SUCCESS 0


#define PFM_FILE_EXISTS 0
#define PFM_OPEN_FAILED 0

typedef unsigned PageNum;
typedef int Status;
typedef char Byte;

class FileHandle;

/* Singleton */
class PagedFileManager
{
public:
  static PagedFileManager* instance();

protected:
  PagedFileManager();
  ~PagedFileManager();

private:
  static PagedFileManager *_pfm;

};