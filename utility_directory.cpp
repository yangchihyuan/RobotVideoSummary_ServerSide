#include "utility_directory.hpp"

#include <dirent.h>
#include <string.h>  //this is a C head file for strlen()
#include <sys/stat.h>       //for S_IRWXU

int ListFiles(const string& directory, const string& file_extension, vector<string>& file_list)
{
    DIR *dir;
    struct dirent *ent;
    int ext_length = file_extension.length();
    if ((dir = opendir(directory.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            int string_length = strlen(ent->d_name);
            if(string_length > ext_length)
                if( strcmp(ent->d_name+string_length-ext_length, file_extension.c_str()) == 0)  //==0 means equals
                { 
                    file_list.push_back(string(ent->d_name));       //ent->d_name is char[256]
                }
        }
        closedir (dir);
        return 1;
    } else {
        /* could not open directory */
        return -1;
    }
}

void CreateDirectory(const string& directory)
{
    DIR* dir = opendir(directory.c_str() );
    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
        /* Directory does not exist. */
        mkdir(directory.c_str(), S_IRWXU|S_IRWXG);
    }
}