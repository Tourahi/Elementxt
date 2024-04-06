#pragma once

#include <string>
#include <climits>

#define SUCCESS 0


#define PFM_FILE_EXISTS   1
#define PFM_OPEN_FAILED   2
#define PFM_REMOVE_FAILED 3
#define PFM_HANDLE_IN_USE 4
#define PFM_FILE_DN_EXIST 5
#define PFM_FILE_NOT_OPEN 6

#define FH_PAGE_DN_EXIST  1
#define FH_SEEK_FAILED    2
#define FH_READ_FAILED    3
#define FH_WRITE_FAILED   4

#define PAGE_SIZE 4096


namespace Data {

  typedef unsigned PageNum;
  typedef int Status;
  typedef char Byte;
  

  class FileHandle;

  /* Singleton */
  class PagedFileManager
  {
  public:
    static PagedFileManager* instance();

    Status createFile(const std::string &fn);
    Status purgeFile(const std::string &fn);
    Status openFile(const std::string &fn, FileHandle &fileH);
    Status closeFile(FileHandle &fileH);

  protected:
    PagedFileManager() = default;
    ~PagedFileManager();

  private:
    static PagedFileManager *_pfm;

    bool fileExists(const std::string &fn);

  };

  class FileHandle
  {

  public:
    
    unsigned readCtr;
    unsigned writeCtr;
    unsigned appendCtr;

    FileHandle();
    ~FileHandle();

    Status readPage(PageNum PageNum, void *data);
    Status writePage(PageNum PageNum, const void *data);
    Status appendPage(const void *data);
    unsigned getNumberOfPages();

    friend class PagedFileManager;

  private:

    FILE *_fd;

    void setfd(FILE *fd);
    FILE *getfd();

  };

}

