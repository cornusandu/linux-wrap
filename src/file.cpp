#include "../include/file.hpp"
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <csetjmp>
#include <cstring>
#include <string>
#include <unordered_map>

#define HIDE_SYMBOL __attribute__((visibility("hidden")))

struct FHandle {
    size_t id;
};

struct HIDE_SYMBOL FHandleListEntry {
    size_t id;
    char* path;
    ssize_t fd = -1;
};

std::vector<FHandleListEntry> fhandle_list;
std::unordered_map<std::string, std::shared_ptr<FHandle>> table;

static QWORD handle_id = 0;

HIDE_SYMBOL char isInit = 0;

void fs::init() {
    fhandle_list.reserve(4);

    isInit = 1;
}

std::shared_ptr<FHandle> &fs::gainHandle(const char* path) {
    auto& ptr = table[std::string(path)];
    if (ptr) {
        return ptr;
    }

    if (fhandle_list.size() > 128) {  // Start removing unnecessary "tomb" items
        for (QWORD i = 0; i < fhandle_list.size(); ) {
            if (fhandle_list[i].path == nullptr && fhandle_list[i].fd < 0) {
                fhandle_list.erase(fhandle_list.begin() + i);
            } else {
                ++i;
            }
        }
    }

    for (FHandleListEntry &entry : fhandle_list) {
        if (entry.path == nullptr) continue;
        if (strcmp(entry.path, path) == 0) {
            FHandle *handle = new FHandle{.id=entry.id};
            table[std::string(entry.path)] = std::shared_ptr<FHandle>(handle, [](FHandle* ptr) {
                for (FHandleListEntry &entry : fhandle_list) {
                    if (entry.id == ptr->id && entry.fd >= 0) {
                        close(entry.fd);
                        entry.fd = -1;
                    }
                }
                delete ptr;
            });
            return table[std::string(entry.path)];
        }
    }

    QWORD id = handle_id;
    handle_id++;
    FHandle *handle = new FHandle{.id=id};
    table[std::string(path)] = std::shared_ptr<FHandle>(handle, [](FHandle* ptr) {
        if (fhandle_list.empty()) return;
        for (FHandleListEntry &entry : fhandle_list) {
            if (entry.id == ptr->id && entry.fd >= 0) {
                close(entry.fd);
                free(entry.path);
                entry.path = nullptr;
                entry.fd = -1;
            }
        }
        delete ptr;
    });
    char* path2 = (char*)malloc(strlen(path)+1);
    memcpy(path2, path, strlen(path)+1);
    fhandle_list.push_back(FHandleListEntry{.id=id, .path = path2, .fd=open(path2, O_RDWR)});
    return table[std::string(path)];
}

DArray<char> readFile(std::shared_ptr<FHandle> path);
