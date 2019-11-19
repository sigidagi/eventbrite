#include "eventbrite.hpp" 
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <future>
#include <sstream>
#include <algorithm>
#include "util.hpp"

#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
#include "curlpp/Exception.hpp"

// curl -H "Accept:application/json" -H "Authorization:Bearer YAFY5RPXHFV5I7XOTDGC" https://www.eventbriteapi.com/v3/events/search/?location.address=denmark--aalborg

namespace brite {

inline bool exist(const json& j, const std::string& key) {
    if (j.find(key) != j.end()) return true; else return false;
}

void to_json(json& aj, const Event& e)
{
    auto j = json {
        {"id", e.id},
        {"organizerId", e.organizerId},
        {"summary", e.summary},
        {"startTime", e.startTime},
        {"endTime", e.endTime},
        {"url", e.url},
        {"logoLink", e.logoLink},
        {"isFree", e.isFree}
    };

    aj.update(j);
}

void from_json(const json& j, Event& e)
{
    j.at("id").get_to(e.id);
    j.at("organizerId").get_to(e.organizerId);
    j.at("summary").get_to(e.summary);
    j.at("startTime").get_to(e.startTime);
    j.at("endTime").get_to(e.endTime);
    j.at("url").get_to(e.url);
    j.at("logoLink").get_to(e.logoLink);
    j.at("isFree").get_to(e.isFree);
}

std::future<std::string> invoke(const std::string& url, const std::string& key, const std::string& body = "") {
    return std::async(std::launch::async,
            [](const std::string& url, const std::string& key, const std::string& body) mutable {
                std::list<std::string> header;
                if (!key.empty()) {
                    header.push_back("Accept:application/json"); 
                    std::string auth = "Authorization:Bearer " + key;
                    header.push_back(auth); 
                }
                else {
                    header.push_back("Content-Type:application/json");
                }

                curlpp::Cleanup clean;
                curlpp::Easy r;
                r.setOpt(new curlpp::options::Url(url));
                r.setOpt(new curlpp::options::HttpHeader(header));
                //r.setOpt(new curlpp::options::CustomRequest{"GET"});
                    
                if (!body.empty()) {
                    r.setOpt(new curlpp::options::PostFields(body));
                    r.setOpt(new curlpp::options::PostFieldSize(body.length()));
                }

                std::ostringstream response;
                r.setOpt(new curlpp::options::WriteStream(&response));

                r.perform();
                return response.str();
            }, url, key, body);
}


struct SortByTime {
  bool operator() (const Event& ev1, const Event& ev2) { 
    return dateTimeToTimestamp(ev1.startTime) < dateTimeToTimestamp(ev2.startTime); 
  }
} SortByTime_t;

EventBrite::EventBrite()
{
    try {
        conf_ = readConfiguration();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << "\n";
        throw e.what(); // terminate program!
    }

    logger_ = initLog();
    logger_->info("Started EventBrite ++++++++");
    
    storagePath_ = conf_.EVENT_DIRECTORY + "/" + conf_.STORAGE_FILE;

    json ajson;
    deserialize(storagePath_, ajson);
    for (json::iterator it = ajson.begin(); it != ajson.end(); ++it) {
        Event ev = *it;
        if (dateTimeToTimestamp(ev.startTime) >= timestamp())
            events_.push_back(std::move(ev));
    }
   
    // update every 30 min.
    updateTimer_.set(1._ms, 30._min);
    updateTimer_.set<EventBrite, &EventBrite::updateTimer>(this);
    updateTimer_.start();
}

EventBrite::~EventBrite()
{
}

EventBrite* EventBrite::instance()
{
	static EventBrite instance_;
	return &instance_;
}

void EventBrite::serialize(const std::string& filename, const json& j)
{
    std::ofstream os(filename.c_str(), std::ios::out);
    if(!os) {
        logger_->warn("Cannot open file: '" + filename + "', file does not exist. Creating file.."); 
        os.open(filename);
    }
    os << j.dump();
    os.close();
}

void EventBrite::deserialize(const std::string& filename, json& j)
{
    std::ifstream is(filename.c_str());
    if (!is.good()) {
        logger_->warn("Cannot open file: '" + filename + "' for deserialization."); 
        return;
    }
    is >> j;
    is.close();
}

std::shared_ptr<spdlog::async_logger> EventBrite::initLog()
{
    // ------------------- Logger ------------------------
    auto stdout_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(conf_.LOG_DIRECTORY + "/eventb_info.log", 1024*1024*10, 3);

    std::vector<spdlog::sink_ptr> sinks {stdout_sink, rotating_sink};
    int queue_size = 1024;
    auto logger = std::make_shared<spdlog::async_logger>("log", sinks.begin(), sinks.end(), queue_size);

    //logger->set_level(spdlog::level::info); // Set global log level to info - debug() will not be desplayed.
    logger->set_level(spdlog::level::trace); // Set specific logger's log level

    spdlog::register_logger(logger);
    return logger;
}

void EventBrite::updateTimer(ev::timer& w, int)
{
    logger_->info("It's time to pull data from eventbrite.com!"); 
    std::cout << std::endl;
    
    // Check if some events expiried, then remove from list.
    std::list<Event>::const_iterator it = events_.begin();
    while (it != std::end(events_)) {
        if (timestamp() > dateTimeToTimestamp(it->startTime)) {
            logger_->info("Event with id " + it->id + " has been expired - removing from list.");
            it = events_.erase(it);
        }
        else {
            ++it;
        }
    }
    
    std::vector<Event> newEvents;
    unsigned int nevents{0};
    try {
        auto response = invoke(conf_.SEARCH_URL, conf_.SECRET_KEY);      
        auto root = json::parse(response.get());
        
        if (exist(root, "error")) {
            logger_->error("Eventbrite says: " + root["error_description"].get<std::string>() + 
                           ", code: " + std::to_string(root["status_code"].get<int>()));
            return;
        }

        for (auto& event : root["events"]) {
            ++nevents;
            Event ev{};    
            for (auto& [key, value] : event.items()) {

                if (key == "id") {
                    ev.id = value;
                }
                if (key == "organizer_id") {
                    ev.organizerId = value;
                }
                else if (key == "summary") {
                    ev.summary = value;     
                }
                else if (key == "start" && exist(value, "local")) {
                    ev.startTime = value.at("local");
                }
                else if (key == "end" && exist(value, "local")) {
                    ev.endTime = value.at("local");
                }
                else if (key == "logo" && exist(value, "url")) {
                    ev.logoLink = value.at("url");
                }
                else if (key == "is_free") {
                    ev.isFree = value;
                }
                else if (key == "url") {
                    ev.url = value;
                }
                else {
                }
            }

            auto it = std::find_if(std::begin(events_), std::end(events_), 
                                   [&ev](const Event& e) { return e.id == ev.id; });
            
            if (it == std::end(events_) && 
                dateTimeToTimestamp(ev.startTime) > timestamp() &&                      // not include past events.
                dateTimeToTimestamp(ev.startTime) < (timestamp() + conf_.SEARCH_DAYS))  // less than 2 month
            {
                logger_->info("New event is found. Event id: " + ev.id);
                newEvents.push_back(std::move(ev));
            }
        }

        logger_->info("Got events: " + std::to_string(nevents) + ". New events: " + std::to_string(newEvents.size()));

        // Send to webhook and serialize new events.
        if (!newEvents.empty()) {
            // 
            std::sort(std::begin(newEvents), std::end(newEvents), SortByTime_t);
            // Send to webhook
            for (const auto& ev : newEvents) {
                json ajson;
                ajson["username"] = "eventbrite";
                ajson["icon_url"] = "https://dagilis.me/images/eventbrite.png";
                
                std::string text;
                if (!ev.logoLink.empty()) {
                    text += "[![Logo Link](" + ev.logoLink + ")](" + ev.url + ")\n\n"; 
                }
                text += ev.summary + "\n\n";
                text += "**Nuoroda**: " + ev.url + " \n\n";
                text += "**Pradžia**:  _" + ev.startTime +  "_\n **Pabaiga**: _" + ev.endTime + "_\n\n";
                if (ev.isFree) {
                    text += "_Renginys nemokamas_";
                }
                else {
                    text += "_Renginys mokamas_";
                }

                ajson["text"] = text;
                invoke(conf_.WEBHOOK, "", ajson.dump());
            }

            std::copy(std::begin(newEvents), std::end(newEvents), std::back_inserter(events_));
            // Serialize all events
            json jarr;    
            for (auto& e : events_) 
                jarr.push_back(e); 
            serialize(storagePath_, jarr);
        }
    }
    catch (curlpp::LogicError & e) {
        logger_->error("Logic error: " + std::string(e.what()));
    }
    catch (curlpp::RuntimeError & e) {
        logger_->error("Runtime error: " + std::string(e.what()));
    }
    catch (json::exception& e) {
        logger_->warn("Response is not a json!");
    }
    catch (std::exception& e) {
        logger_->error("Something went wrong: " + std::string(e.what()));
    }
}

ev::default_loop& EventBrite::loop()
{
    return loop_;
}

} // namespace brite

