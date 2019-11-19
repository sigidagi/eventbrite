#pragma once

#include <string>

namespace brite {

typedef struct Config {

    std::string LOG_DIRECTORY;
    std::string EVENT_DIRECTORY;
    std::string STORAGE_FILE;
    std::string SEARCH_URL;
    std::string WEBHOOK;
    std::string SECRET_KEY;
    unsigned int SEARCH_DAYS; // search events up to (in days).

} Config_t;

Config_t readConfiguration();

} // namespace
