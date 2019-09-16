#include <vector>
#include <string>

using namespace std;

//return 1 if successful, -1 if failed
int ListFiles(const string& directory, const string& file_extension, vector<string>& file_list);
void CreateDirectory(const string& directory);
