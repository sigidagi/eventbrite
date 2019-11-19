#pragma once

#include "spdlog/spdlog.h"
#include <ev++.h>
#include <memory>
#include <fstream>
#include <list>
#include "nlohmann/json.hpp"
#include "configuration.hpp"

namespace spd = spdlog;
using namespace nlohmann;

//const std::string LOG_DIRECTORY = "/opt/matterhooks/logs";
//const std::string EVENT_DIRECTORY = "/opt/matterhooks/eventbrite";
//const std::string STORAGE_FILE = "events.json";
//const std::string CONF_FILE = "eventbrite.conf";

namespace brite {

struct Event {
    std::string id;
    std::string organizerId;
    std::string summary;
    std::string startTime;
    std::string endTime;
    std::string url;
    std::string logoLink;
    bool isFree = false;
};

class EventBrite {
    
    private:
        EventBrite();
        ~EventBrite();
        EventBrite(const EventBrite&) = delete;
        EventBrite& operator=(const EventBrite&) = delete;
        
        std::shared_ptr<spd::async_logger> initLog();
        void serialize(const std::string& filename, const json& j);
        void deserialize(const std::string& filename, json& j);
        void updateTimer(ev::timer& w, int);

        ev::default_loop loop_;
        Config_t conf_;

        ev::timer updateTimer_;
        std::list<Event> events_;
        std::shared_ptr<spd::async_logger> logger_ = nullptr;
        std::string storagePath_;
    public:
        static EventBrite* instance();
        ev::default_loop& loop();
};



} // namespace 
