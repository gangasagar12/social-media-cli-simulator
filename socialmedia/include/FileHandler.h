#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>

class FileHandler {
public:
    static void saveBinary(const std::string &filename, const std::vector<std::string> &lines);
    static std::vector<std::string> loadBinary(const std::string &filename);
    
private:
    static std::vector<std::string> loadTextBackup(const std::string &filename);
};

#endif // FILEHANDLER_H