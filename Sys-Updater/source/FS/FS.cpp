#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <sys/stat.h>
#include <unistd.h>
#include "FS/FS.hpp"

namespace FS
{
	Result createdir(string path)
	{
		if (checkdirexist(path) == false)
		{
			mkdir(path.c_str(), 0777);
			return 0;
		}
		return 1;
	}

	Result DeleteDir(string path){
		std::filesystem::remove_all(path);
		rmdir(path.c_str());
		return 0;
	}
	
	bool checkdirexist(string path)
	{
		bool exist = false;
		DIR *dir = opendir(path.c_str());
		if (dir)
		{
			closedir(dir);
			exist = true;
		}
		return exist;
	}

	Result createFile(string path)
	{
		FILE *f;
		f = fopen(path.c_str(), "w");
		if (f)
		{
			fclose(f);
			return 0;
		}
		return 1;
	}
} // namespace FS