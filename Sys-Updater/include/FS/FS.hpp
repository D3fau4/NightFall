#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <switch.h>
#include <filesystem>

using namespace std;

extern FsFileSystem *fs;
extern FsFileSystem devices[4];

namespace FS {
    Result createdir(string path);
    bool checkdirexist(string path);
    Result DeleteDir(string path);
    Result createFile(string path);
};