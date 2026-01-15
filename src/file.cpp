#include "../include/file.hpp"
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define HIDE_SYMBOL __attribute__((visibility("hidden")))

struct HIDE_SYMBOL FHandleListEntry {
    size_t id;
    char* path;
};

std::vector<FHandleListEntry> fhandle_list;

HIDE_SYMBOL char isInit = 0;

void fs::init() {
    fhandle_list.reserve(4);

    isInit = 1;
}

HIDE_SYMBOL FHandle::FHandle(const char* path) {
    this->id = fhandle_list.size();
    fhandle_list.push_back(FHandleListEntry{.id=this->id, .path=(char*)path});
}

FHandle fs::gainHandle(SharedDArray<char> path) {
    return FHandle(path.get());
}

SharedDArray<char> readFile(FHandle path) {
    goto goto_fstart;

    goto_failure:
    return SharedDArray<char>((char*)0x0);

    goto_fstart:
    char* _path = nullptr;
    int fd;
    ssize_t n;
    size_t i = 0;
    for (FHandleListEntry p : fhandle_list) {
        if (p.id == path.get_id()) {
            _path = p.path;
        }
    }
    if (!_path) goto goto_failure;
    open(_path, O_RDONLY);
    if (fd < 0) goto goto_failure; 

    std::vector<char> buffer(4096);
    std::vector<char> _result;

    while ((n = read(fd, buffer.data(), buffer.size())) > 0) {
        _result.insert(_result.end(), buffer.begin(), buffer.begin() + n);
    }

    SharedDArray<char> result = std::shared_ptr<char[]>((char*)calloc(_result.size() + 1, 1));
    
    for (char n : _result) {
        result[i] = n;
        i++;
    }

    return result;
}
