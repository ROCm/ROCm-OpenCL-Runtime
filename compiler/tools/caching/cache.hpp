//-----------------------------------------------------------------------------
// Copyright (c) 2011 - 2015 Advanced Micro Devices, Inc.  All rights reserved.
//-----------------------------------------------------------------------------

// Code cache implementation.

#ifndef AMD_CACHE_H_
#define AMD_CACHE_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <alloca.h>
#else
#include <windows.h>
#include <shlobj.h>
#include <Lmcons.h>
#include <aclapi.h>
#include <filesystem>
#include <BaseTsd.h>
#endif

#if _WIN32
#define CloseFile CloseHandle
#define FileHandle HANDLE
#else
#define CloseFile close
#define FileHandle int
#endif

#define CACHING_MACHINERY_VERSION 1

/* String Cache File Contents (listed in order) */
// BUild options in text format
// srcSize1;
// Src data 1
// srcSize2
// Src data 2
// ......
// Dest data

class StringCache {
public:
  typedef size_t size_type;
#if defined(_WIN32) || defined(_WIN64)
  typedef SSIZE_T ssize_type;
#else
  typedef ssize_t ssize_type;
#endif // _WIN32 || _WIN64

  struct CachedData {
    const char *data;
    size_type dataSize;
  };

private:
  struct FileHeader {
    char AMD[4]; // 'AMD\0'
    unsigned int machineryVersion;
    unsigned int bitness;
    unsigned int srcNum;
    size_type buildOptSize;
    size_type dstSize;
  };

  struct IndexFile {
    unsigned int version;
    size_type cacheSize;
  };

  // TODO: the default cache size (512MB) might be changed later
  static const unsigned int KERNEL_CACHE_CAPACITY_DEFAULT = 512 * 1024 * 1024;
  unsigned int cacheStorageSize;
  unsigned int bitness;
  unsigned int version;
  unsigned int cacheVersion;
  size_type cacheSize;
  bool isCacheReady;
  bool isStrCached;
  std::string rootPath;
  std::string indexName;
  std::string errorMsg;
  std::string folderPostfix;

  // Helper functions
  char fileSeparator();
  bool pathExists(const std::string &path);
  bool createPath(const std::string &path);

  // Set the root path for the cache
  bool setRootPath(const std::string &chipName);

  // Setup cache tree structure
  bool setUpCacheFolders();

  // Get the cache version and size from the index file
  bool getCacheInfo();

  // Set the cache version and size in the index file
  bool setCacheInfo(unsigned int newVersion, StringCache::size_type newSize);

  // Compute hash value for chunks of data
  unsigned int computeHash(const StringCache::CachedData *data, unsigned int numData, const std::string &buildOpts);

  // Computes hash and file name from given data
  void makeFileName(const StringCache::CachedData *data, unsigned int numData,
                    const std::string &buildOpts, std::string &pathToFile);

  // Finds path to a file from a given hash value
  void getFilePathFromHash(unsigned int hash, std::string &pathToFile);

#if _WIN32
  // Get Sid of account
  std::unique_ptr<SID> getSid(TCHAR *userName);
#endif

  // Return detailed error message as string
  std::string getLastErrorMsg();

  // Read contents in cacheFile
  bool readFile(FileHandle cacheFile, void *buffer, StringCache::size_type size);

  // Write data to a file
  bool writeFile(FileHandle cacheFile, const void *buffer, StringCache::size_type sizeToWrite);
  bool writeFile(const std::string &fileName, const void *data, StringCache::size_type size, bool appendable);

  // Set file to only owner accessible
  bool setAccessPermission(const std::string &fileName, bool isFile = false);

  // Set up cache file structure
  bool cacheInit(const std::string &chipName);

  // Get cache entry corresponding to srcData, if it exists
  bool getCacheEntry_helper(const StringCache::CachedData *srcData, unsigned int srcNum,
                            const std::string &buildOpts, std::string &dstData);

  // Control cache test
  bool internalCacheTestSwitch();

  // Verify whether the file includes the right cache file header
  bool verifyFileHeader(StringCache::FileHeader &H, unsigned int bitness, unsigned int srcNum, const std::string &buildOpts);

  // Remove partially written file
  void removePartiallyWrittenFile(const std::string &fileName);

  // Log error message and close the file
  void logErrorCloseFile(const std::string &errorMsg, const FileHandle file);

public:
  StringCache(const std::string &deviceName, unsigned int b, unsigned int cacheVer = 0, std::string postfix = "") :
    cacheStorageSize(KERNEL_CACHE_CAPACITY_DEFAULT), bitness(b), version(0), cacheVersion(cacheVer), cacheSize(0), isStrCached(false) {
    folderPostfix = postfix;
    isCacheReady = cacheInit(deviceName);
    if (!isCacheReady) {
      appendLogToFile();
    }
  }

  // Wipe the cache folder structure
  bool wipeCacheStorage();

  void setCacheStorageSize(unsigned int storageSize) {
    cacheStorageSize = storageSize;
  }

  // Make cache entry corresponding to srcData, dstData, buildOpts
  bool makeCacheEntry(const StringCache::CachedData *srcData, unsigned int srcNum,
                      const std::string &buildOpts, const std::string &dstData);

  // Wrapper function for getCacheEntry
  bool getCacheEntry(bool isCachingOn, const StringCache::CachedData *srcData, unsigned int srcNum,
                     const std::string &buildOpts, std::string &dstData, const std::string &msg);

  // Log caching error messages for debugging the cache and/or detecting collisions
  void appendLogToFile(std::string extraMsg = "");

  bool isCached() const { return isStrCached; }
};
#endif // AMD_CACHE_H_
