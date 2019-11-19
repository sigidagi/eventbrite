#include "configuration.hpp"
#include <fstream>

#include "nlohmann/json.hpp"

namespace brite {

Config_t readConfiguration() {

    using json = nlohmann::json;
    Config_t conf;

    std::string file = "/etc/eventbrite.conf";
    std::ifstream is(file.c_str());
    if (!is.good()) {
        throw std::string("Can't open file: " + file);
    }
    try {
        json ajson;
        is >> ajson;
        conf.LOG_DIRECTORY = ajson.at("LOG_DIRECTORY").get<std::string>();
        conf.EVENT_DIRECTORY = ajson.at("EVENT_DIRECTORY").get<std::string>();
        conf.STORAGE_FILE = ajson.at("STORAGE_FILE").get<std::string>();
        conf.SEARCH_URL = ajson.at("SEARCH_URL").get<std::string>();
        conf.SECRET_KEY= ajson.at("SECRET_KEY").get<std::string>();
        conf.SEARCH_DAYS = ajson.at("SEARCH_DAYS").get<unsigned int>() * 24 * 60 * 60; // convert days to seconds.
        conf.WEBHOOK = ajson.at("WEBHOOK").get<std::string>();
    }
    catch(std::exception& e) {
        throw std::string(e.what());
    }

    return conf;
}

} // namespace brite
