#include <vector>
#include <string>

class HamArc {
private:
    std::vector<unsigned char> arcBytes;
    std::vector<std::string> fileNames;
    std::vector<uint64_t> fileSizes;
    char *arcName;

public:
    HamArc(char* archive_name);
    ~HamArc();

    void AppendFile(char* filename);
    void DeleteFile(char* filename);
    void ExtractFile(char* filename);
    void ShowFiles();
    void ExtractAll();
};