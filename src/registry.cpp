#include "registry.h"

#include "constants.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string.h>
#include <unistd.h>

// one time initialization of singleton
Registry* Registry::instance_ = new Registry();

Registry& Registry::instance()
{
    return *instance_;
}

std::string Registry::add_file(std::string filename)
{
    std::ofstream ofs(filename.c_str());
    if (!ofs) 
        return filename + FAIL_FILE_NOT_CREATED;
    else
        return filename + INFO_FILE_CREATED;
}

std::string Registry::seek_file(std::string filename, int index)
{
    std::map<std::string, int>::iterator it = seek_positions.find(filename);
    if (it == seek_positions.end()) {
        std::pair<std::string, int> entry = std::make_pair(filename, index);
        seek_positions.insert(entry);
    } else
        it->second = index;    

    std::stringstream ss;
    ss << index;    
    return filename + INFO_FILE_SEEKED + ss.str();
}

std::string Registry::read_file(std::string filename, int length)
{
    std::ifstream ifs(filename.c_str());

    //check for seek position
    std::map<std::string, int>::iterator it = seek_positions.find(filename);
    if (it != seek_positions.end())
        ifs.seekg(it->second);

    char buf[length + 1];
    memset(buf, 0, length + 1);
    ifs.read(buf, length);
    ifs.close();

    std::string data(buf);
    return data;
}

std::string Registry::write_file(std::string filename, std::string data)
{
    std::ofstream ofs(filename.c_str());
    
    //check for seek position
    std::map<std::string, int>::iterator it = seek_positions.find(filename);
    if (it != seek_positions.end())
        ofs.seekp(it->second);

    ofs.write(data.c_str(), data.size() + 1);
    ofs.close();

    return filename + INFO_FILE_WRITTEN;
}

std::string Registry::remove_file(std::string filename)
{
    if (std::remove(filename.c_str()) == 0)
        return filename + INFO_FILE_DELETED;
    
    return filename + FAIL_FILE_NOT_DELETED;
}

bool Registry::test_add_file(std::string filename, std::string& res)
{
    if (access(filename.c_str(), F_OK) == 0) {
        res = filename + FAIL_FILE_EXISTS;
        return true;
    }

    return true;
}

bool Registry::test_seek_file(std::string filename, int index,
    std::string& res)
{
    if (access(filename.c_str(), F_OK) != 0) {
        res = filename + FAIL_FILE_NOT_FOUND;
        return false;
    } 
    
    std::ifstream ifs(filename.c_str());
    ifs.seekg(0, ifs.end);
    if (index > ifs.tellg()) {
        res = filename + FAIL_SEEK_OOR;
        return false;
    }
    ifs.close();

    return true;
}

bool Registry::test_read_file(std::string filename, int length,
    std::string& res)
{
    if (access(filename.c_str(), F_OK) != 0) {
        res = filename + FAIL_FILE_NOT_FOUND;
        return false;
    } 
    if (access(filename.c_str(), R_OK) != 0) {
        res = filename + FAIL_FILE_NOT_READABLE;
        return false;
    }    

    return true;
}

bool Registry::test_write_file(std::string filename, std::string& res)
{
    if (access(filename.c_str(), F_OK) != 0) {
        res = filename + FAIL_FILE_NOT_FOUND;
        return false;
    } 
    if (access(filename.c_str(), W_OK) != 0) {
        res = filename + FAIL_FILE_NOT_WRITABLE;
        return false;
    }

    return true;
}

bool Registry::test_remove_file(std::string filename, std::string& res)
{
    return test_write_file(filename, res);
}

