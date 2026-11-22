#include "CampusApp.hpp"
#include "outils.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

CampusApp::CampusApp() : _db("campus"), _running(false) {
    _db.createCollection("resources");
    _db.createCollection("events");

    _db.getCollection("resources").createIndex("type");
    _db.getCollection("resources").createIndex("name");
    _db.getCollection("events").createIndex("title");
    _db.getCollection("events").createIndex("date");

    _db.load();
}

CampusApp::~CampusApp() {
    _db.save();
    std::cout << "\nDatabase saved. Goodbye!" << std::endl;
    sleepSeconds(1);
    clearScreen();
}

void CampusApp::run() {
    clearScreen();
    _running = true;
    while (_running)
        handleMainMenu();
}

bool CampusApp::isNumeric(const std::string& str) const {
    if (str.empty()) return false;
    for (char c : str)
        if (!std::isdigit(c)) return false;
    return true;
}

bool CampusApp::isValidDate(const std::string& date) const {
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(date[i])) return false;
    }
    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));
    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1) return false;
    int dim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) dim[1] = 29;
    return day <= dim[month - 1];
}

bool CampusApp::isValidTime(const std::string& time) const {
    if (time.length() != 5 || time[2] != ':') return false;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!std::isdigit(time[i])) return false;
    }
    int h = std::stoi(time.substr(0, 2));
    int m = std::stoi(time.substr(3, 2));
    return h >= 0 && h <= 23 && m >= 0 && m <= 59;
}

std::string CampusApp::toLower(const std::string& str) const {
    std::string result;
    for (char c : str)
        result += (c >= 'A' && c <= 'Z') ? static_cast<char>(c + 32) : c;
    return result;
}

std::string CampusApp::calculateEndTime(const std::string& start, int duration) const {
    int h = std::stoi(start.substr(0, 2));
    int m = std::stoi(start.substr(3, 2));
    m += duration;
    h += m / 60;
    m %= 60;
    h %= 24;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << h << ":"
        << std::setfill('0') << std::setw(2) << m;
    return oss.str();
}

bool CampusApp::timesOverlap(const std::string& s1, const std::string& e1,
                             const std::string& s2, const std::string& e2) const {
    return s1 < e2 && e1 > s2;
}

bool CampusApp::hasConflict(const std::vector<std::string>& resource_ids,
                            const std::string& date,
                            const std::string& start_time,
                            int duration,
                            const std::string& exclude_id) const {
    std::string end_time = calculateEndTime(start_time, duration);

    for (const auto& event : _db.getCollection("events").all()) {
        if (!exclude_id.empty() && event.id() == exclude_id) continue;
        if (!event.has("date") || event.get("date").asString() != date) continue;
        if (!event.has("resources") || !event.get("resources").isArray()) continue;

        bool shares = false;
        Value ev_resources = event.get("resources");
        for (const auto& res : ev_resources.asArray()) {
            if (!res.isObject()) continue;
            const auto& obj = res.asObject();
            auto it = obj.find("_id");
            if (it == obj.end()) continue;
            for (const auto& rid : resource_ids) {
                if (it->second.asString() == rid) { shares = true; break; }
            }
            if (shares) break;
        }
        if (!shares) continue;

        std::string ev_start = event.get("start_time").asString();
        std::string ev_end = calculateEndTime(ev_start, event.get("duration").asInt());
        if (timesOverlap(start_time, end_time, ev_start, ev_end))
            return true;
    }
    return false;
}

bool CampusApp::isResourceUsedByEvents(const std::string& resource_id) const {
    for (const auto& event : _db.getCollection("events").all()) {
        if (!event.has("resources") || !event.get("resources").isArray()) continue;
        Value ev_resources = event.get("resources");
        for (const auto& res : ev_resources.asArray()) {
            if (!res.isObject()) continue;
            const auto& obj = res.asObject();
            auto it = obj.find("_id");
            if (it != obj.end() && it->second.asString() == resource_id)
                return true;
        }
    }
    return false;
}

std::vector<std::string> CampusApp::selectResources() {
    std::vector<std::string> selected;
    Collection& resources = _db.getCollection("resources");

    std::cout << "\n--- Available Resources ---" << std::endl;
    auto all = resources.all();
    if (all.empty()) {
        std::cout << "No resources in the database." << std::endl;
        return selected;
    }
    for (const auto& doc : all) {
        std::string status = doc.has("available") && doc.get("available").isBool()
            ? (doc.get("available").asBool() ? "Available" : "Unavailable")
            : "Unknown";
        std::cout << "  ID: " << doc.id()
                  << " | " << doc.get("name").asString()
                  << " (" << doc.get("type").asString() << ")"
                  << " - " << status << std::endl;
    }

    std::cout << "\nEnter resource IDs (comma-separated, e.g. 1,3,5): ";
    std::string ids_str;
    std::getline(std::cin, ids_str);

    std::stringstream ss(ids_str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        while (!token.empty() && token[0] == ' ') token = token.substr(1);
        while (!token.empty() && token.back() == ' ') token.pop_back();
        if (token.empty()) continue;

        if (resources.exists(token)) {
            Document* res = resources.find(token);
            if (res && res->has("available") && res->get("available").isBool()
                && !res->get("available").asBool()) {
                std::cout << "Warning: Resource ID " << token
                          << " is unavailable. Skipping." << std::endl;
                continue;
            }
            selected.push_back(token);
        } else {
            std::cout << "Warning: Resource ID " << token
                      << " not found. Skipping." << std::endl;
        }
    }

    if (selected.empty())
        std::cout << "No valid resources selected." << std::endl;
    else
        std::cout << "Selected " << selected.size() << " resource(s)." << std::endl;
    return selected;
}

void CampusApp::displayResource(const Document& doc) const {
    std::string status = "Unknown";
    if (doc.has("available") && doc.get("available").isBool())
        status = doc.get("available").asBool() ? "Available" : "Unavailable";

    std::cout << "=================================================" << std::endl;
    std::cout << "  ID:     " << doc.id() << std::endl;
    std::cout << "  Name:   " << doc.get("name").toDisplayString() << std::endl;
    std::cout << "  Type:   " << doc.get("type").toDisplayString() << std::endl;
    std::cout << "  Status: " << status << std::endl;
    std::cout << "=================================================" << std::endl;
}

void CampusApp::displayEvent(const Document& doc) const {
    int dur = doc.has("duration") ? doc.get("duration").asInt() : 0;
    std::string end = doc.has("start_time")
        ? calculateEndTime(doc.get("start_time").asString(), dur) : "??:??";

    std::cout << "=================================================" << std::endl;
    std::cout << "  ID:       " << doc.id() << std::endl;
    std::cout << "  Title:    " << doc.get("title").toDisplayString() << std::endl;
    std::cout << "  Date:     " << doc.get("date").toDisplayString() << std::endl;
    std::cout << "  Time:     " << doc.get("start_time").toDisplayString()
              << " - " << end << std::endl;
    std::cout << "  Duration: " << dur << " minutes" << std::endl;
    std::cout << "  Resources:" << std::endl;

    if (doc.has("resources") && doc.get("resources").isArray()) {
        Value res_val = doc.get("resources");
        for (const auto& res : res_val.asArray()) {
            if (!res.isObject()) continue;
            const auto& obj = res.asObject();
            std::string rname = obj.count("name")
                ? obj.at("name").asString() : "?";
            std::string rid = obj.count("_id")
                ? obj.at("_id").asString() : "?";
            std::string rtype = obj.count("type")
                ? obj.at("type").asString() : "?";
            std::cout << "    - " << rname
                      << " (ID: " << rid << ", Type: " << rtype << ")"
                      << std::endl;
        }
    } else {
        std::cout << "    (none)" << std::endl;
    }
    std::cout << "=================================================" << std::endl;
}

void CampusApp::handleMainMenu() {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Campus Document Database System" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  1 | Resource Management" << std::endl;
    std::cout << "  2 | Event Management" << std::endl;
    std::cout << "  3 | Database Status" << std::endl;
    std::cout << "  4 | Exit" << std::endl;
    std::cout << std::endl << "Choose option (1-4): ";

    std::string input;
    std::getline(std::cin, input);

    if (!isNumeric(input)) {
        std::cout << "Please enter a number." << std::endl;
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
        clearScreen();
        return;
    }

    switch (std::stoi(input)) {
        case 1: clearScreen(); handleResourceMenu(); break;
        case 2: clearScreen(); handleEventMenu(); break;
        case 3:
            clearScreen();
            displayDatabaseStatus();
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
            clearScreen();
            break;
        case 4: _running = false; break;
        default:
            std::cout << "Invalid option." << std::endl;
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
            clearScreen();
    }
}

void CampusApp::handleResourceMenu() {
    Collection& resources = _db.getCollection("resources");
    bool in_menu = true;

    while (in_menu) {
        std::cout << "==================================================" << std::endl;
        std::cout << "  Resource Management" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "  1 | Add Resource" << std::endl;
        std::cout << "  2 | View All Resources" << std::endl;
        std::cout << "  3 | Update Resource" << std::endl;
        std::cout << "  4 | Delete Resource" << std::endl;
        std::cout << "  5 | Search Resources" << std::endl;
        std::cout << "  6 | Back to Main Menu" << std::endl;
        std::cout << std::endl << "Choose option (1-6): ";

        std::string input;
        std::getline(std::cin, input);
        if (!isNumeric(input)) {
            std::cout << "Please enter a number." << std::endl;
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
            clearScreen();
            continue;
        }

        switch (std::stoi(input)) {

        case 1: {
            std::cout << "\n=== ADD NEW RESOURCE ===" << std::endl;
            std::string name, type, avail_input;

            std::cout << "Enter resource name: ";
            std::getline(std::cin, name);
            if (name.empty()) {
                std::cout << "Error: name cannot be empty." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::cout << "Enter resource type (Room/Lab/Equipment/Other): ";
            std::getline(std::cin, type);
            if (type.empty()) {
                std::cout << "Error: type cannot be empty." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::cout << "Is this resource available? (y/n): ";
            std::getline(std::cin, avail_input);
            bool avail = (avail_input == "y" || avail_input == "Y");

            Document doc;
            doc.set("name", Value(name));
            doc.set("type", Value(type));
            doc.set("available", Value(avail));

            std::string id = resources.insert(doc);
            _db.save();

            std::cout << "\nDocument inserted into 'resources' collection:" << std::endl;
            displayResource(*resources.find(id));

            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 2: {
            auto all = resources.all();
            if (all.empty()) {
                std::cout << "\nNo resources in the database." << std::endl;
            } else {
                std::cout << "\n=== ALL RESOURCES (" << all.size()
                          << " documents) ===" << std::endl;
                for (const auto& doc : all) {
                    displayResource(doc);
                    std::cout << std::endl;
                }
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 3: {
            std::cout << "\n=== UPDATE RESOURCE ===" << std::endl;
            if (resources.count() == 0) {
                std::cout << "No resources to update." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }
            for (const auto& doc : resources.all())
                displayResource(doc);

            std::cout << "\nEnter resource ID to update: ";
            std::string id;
            std::getline(std::cin, id);

            Document* doc = resources.find(id);
            if (!doc) {
                std::cout << "Error: resource ID " << id << " not found." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::cout << "\nCurrent document:" << std::endl;
            displayResource(*doc);

            std::map<std::string, Value> updates;
            std::cout << "\nNew name (Enter to keep '" << doc->get("name").asString() << "'): ";
            std::string new_name;
            std::getline(std::cin, new_name);
            if (!new_name.empty()) updates["name"] = Value(new_name);

            std::cout << "New type (Enter to keep '" << doc->get("type").asString() << "'): ";
            std::string new_type;
            std::getline(std::cin, new_type);
            if (!new_type.empty()) updates["type"] = Value(new_type);

            std::cout << "Change availability? (y/N): ";
            std::string change_avail;
            std::getline(std::cin, change_avail);
            if (change_avail == "y" || change_avail == "Y") {
                std::cout << "Set as available? (y/n): ";
                std::string avail_in;
                std::getline(std::cin, avail_in);
                bool new_avail = (avail_in == "y" || avail_in == "Y");

                if (!new_avail && doc->get("available").asBool()
                    && isResourceUsedByEvents(id)) {
                    std::cout << "Error: cannot set unavailable while booked by events."
                              << std::endl;
                    std::cout << "\nPress Enter to continue..."; std::cin.get();
                    clearScreen(); break;
                }
                updates["available"] = Value(new_avail);
            }

            if (!updates.empty()) {
                resources.update(id, updates);
                _db.save();
                std::cout << "\nResource updated:" << std::endl;
                displayResource(*resources.find(id));
            } else {
                std::cout << "No changes made." << std::endl;
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 4: {
            std::cout << "\n=== DELETE RESOURCE ===" << std::endl;
            if (resources.count() == 0) {
                std::cout << "No resources to delete." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }
            for (const auto& doc : resources.all())
                displayResource(doc);

            std::cout << "\nEnter resource ID to delete: ";
            std::string id;
            std::getline(std::cin, id);

            if (!resources.exists(id)) {
                std::cout << "Error: resource ID " << id << " not found." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::cout << "Are you sure? (y/N): ";
            std::string confirm;
            std::getline(std::cin, confirm);

            if (confirm == "y" || confirm == "Y") {
                if (isResourceUsedByEvents(id)) {
                    std::cout << "Warning: this resource is embedded in events." << std::endl;
                    std::cout << "Delete anyway? (y/N): ";
                    std::string final_confirm;
                    std::getline(std::cin, final_confirm);
                    if (final_confirm != "y" && final_confirm != "Y") {
                        std::cout << "Cancelled." << std::endl;
                        std::cout << "\nPress Enter to continue..."; std::cin.get();
                        clearScreen(); break;
                    }
                }
                resources.remove(id);
                _db.save();
                std::cout << "Resource deleted." << std::endl;
            } else {
                std::cout << "Cancelled." << std::endl;
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 5:
            clearScreen();
            handleResourceSearch();
            break;

        case 6:
            clearScreen();
            in_menu = false;
            break;

        default:
            std::cout << "Invalid option." << std::endl;
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
        }
    }
}

void CampusApp::handleEventMenu() {
    Collection& events = _db.getCollection("events");
    Collection& resources = _db.getCollection("resources");
    bool in_menu = true;

    while (in_menu) {
        std::cout << "==================================================" << std::endl;
        std::cout << "  Event Management" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "  1 | Add Event" << std::endl;
        std::cout << "  2 | View All Events" << std::endl;
        std::cout << "  3 | Update Event" << std::endl;
        std::cout << "  4 | Delete Event" << std::endl;
        std::cout << "  5 | Search Events" << std::endl;
        std::cout << "  6 | Back to Main Menu" << std::endl;
        std::cout << std::endl << "Choose option (1-6): ";

        std::string input;
        std::getline(std::cin, input);
        if (!isNumeric(input)) {
            std::cout << "Please enter a number." << std::endl;
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen(); continue;
        }

        switch (std::stoi(input)) {

        case 1: {
            std::cout << "\n=== ADD NEW EVENT ===" << std::endl;
            if (resources.count() == 0) {
                std::cout << "No resources available. Add resources first." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::string title;
            std::cout << "Enter event title: ";
            std::getline(std::cin, title);
            if (title.empty()) {
                std::cout << "Error: title cannot be empty." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::string date;
            std::cout << "Enter date [YYYY-MM-DD]: ";
            std::getline(std::cin, date);
            if (!isValidDate(date)) {
                std::cout << "Error: invalid date." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::string start_time;
            std::cout << "Enter start time [HH:MM]: ";
            std::getline(std::cin, start_time);
            if (!isValidTime(start_time)) {
                std::cout << "Error: invalid time." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::string dur_str;
            std::cout << "Enter duration in minutes: ";
            std::getline(std::cin, dur_str);
            if (!isNumeric(dur_str)) {
                std::cout << "Error: duration must be a number." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }
            int duration = std::stoi(dur_str);
            if (duration <= 0 || duration > 1440) {
                std::cout << "Error: duration must be 1-1440." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            auto selected_ids = selectResources();
            if (selected_ids.empty()) {
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            if (hasConflict(selected_ids, date, start_time, duration)) {
                std::cout << "\nError: time conflict detected! Resource(s) already booked."
                          << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::vector<Value> embedded;
            for (const auto& rid : selected_ids) {
                Document* res = resources.find(rid);
                if (!res) continue;
                std::map<std::string, Value> embed;
                embed["_id"] = Value(rid);
                embed["name"] = res->get("name");
                embed["type"] = res->get("type");
                embedded.push_back(Value(embed));
            }

            Document event_doc;
            event_doc.set("title", Value(title));
            event_doc.set("date", Value(date));
            event_doc.set("start_time", Value(start_time));
            event_doc.set("duration", Value(duration));
            event_doc.set("resources", Value(embedded));

            std::string id = events.insert(event_doc);
            _db.save();

            std::cout << "\nEvent created with embedded resources:" << std::endl;
            displayEvent(*events.find(id));

            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 2: {
            auto all = events.all();
            if (all.empty()) {
                std::cout << "\nNo events in the database." << std::endl;
            } else {
                std::cout << "\n=== ALL EVENTS (" << all.size()
                          << " documents) ===" << std::endl;
                for (const auto& doc : all) {
                    displayEvent(doc);
                    std::cout << std::endl;
                }
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 3: {
            std::cout << "\n=== UPDATE EVENT ===" << std::endl;
            if (events.count() == 0) {
                std::cout << "No events to update." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }
            for (const auto& doc : events.all())
                displayEvent(doc);

            std::cout << "\nEnter event ID to update: ";
            std::string id;
            std::getline(std::cin, id);

            Document* doc = events.find(id);
            if (!doc) {
                std::cout << "Error: event ID " << id << " not found." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::map<std::string, Value> updates;

            std::cout << "\nNew title (Enter to keep '" << doc->get("title").asString() << "'): ";
            std::string new_title;
            std::getline(std::cin, new_title);
            if (!new_title.empty()) updates["title"] = Value(new_title);

            std::cout << "New date [YYYY-MM-DD] (Enter to keep '" << doc->get("date").asString() << "'): ";
            std::string new_date;
            std::getline(std::cin, new_date);
            if (!new_date.empty() && !isValidDate(new_date)) {
                std::cout << "Error: invalid date." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }
            if (!new_date.empty()) updates["date"] = Value(new_date);

            std::cout << "New start time [HH:MM] (Enter to keep '" << doc->get("start_time").asString() << "'): ";
            std::string new_time;
            std::getline(std::cin, new_time);
            if (!new_time.empty() && !isValidTime(new_time)) {
                std::cout << "Error: invalid time." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }
            if (!new_time.empty()) updates["start_time"] = Value(new_time);

            std::cout << "New duration (Enter to keep " << doc->get("duration").asInt() << "): ";
            std::string dur_str;
            std::getline(std::cin, dur_str);
            int new_dur = doc->get("duration").asInt();
            if (!dur_str.empty()) {
                if (!isNumeric(dur_str) || std::stoi(dur_str) <= 0 || std::stoi(dur_str) > 1440) {
                    std::cout << "Error: invalid duration." << std::endl;
                    std::cout << "\nPress Enter to continue..."; std::cin.get();
                    clearScreen(); break;
                }
                new_dur = std::stoi(dur_str);
                updates["duration"] = Value(new_dur);
            }

            std::cout << "Update resources? (y/N): ";
            std::string update_res;
            std::getline(std::cin, update_res);

            std::vector<std::string> check_ids;
            if (update_res == "y" || update_res == "Y") {
                check_ids = selectResources();
                if (check_ids.empty()) {
                    std::cout << "Keeping current resources." << std::endl;
                    if (doc->has("resources") && doc->get("resources").isArray()) {
                        Value cur_res = doc->get("resources");
                        for (const auto& r : cur_res.asArray()) {
                            if (r.isObject() && r.asObject().count("_id"))
                                check_ids.push_back(r.asObject().at("_id").asString());
                        }
                    }
                } else {
                    std::vector<Value> embedded;
                    for (const auto& rid : check_ids) {
                        Document* res = resources.find(rid);
                        if (!res) continue;
                        std::map<std::string, Value> embed;
                        embed["_id"] = Value(rid);
                        embed["name"] = res->get("name");
                        embed["type"] = res->get("type");
                        embedded.push_back(Value(embed));
                    }
                    updates["resources"] = Value(embedded);
                }
            } else {
                if (doc->has("resources") && doc->get("resources").isArray()) {
                    Value cur_res = doc->get("resources");
                    for (const auto& r : cur_res.asArray()) {
                        if (r.isObject() && r.asObject().count("_id"))
                            check_ids.push_back(r.asObject().at("_id").asString());
                    }
                }
            }

            std::string check_date = new_date.empty() ? doc->get("date").asString() : new_date;
            std::string check_time = new_time.empty() ? doc->get("start_time").asString() : new_time;

            if (hasConflict(check_ids, check_date, check_time, new_dur, id)) {
                std::cout << "Error: update would cause a time conflict." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            if (!updates.empty()) {
                events.update(id, updates);
                _db.save();
                std::cout << "\nEvent updated:" << std::endl;
                displayEvent(*events.find(id));
            } else {
                std::cout << "No changes made." << std::endl;
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 4: {
            std::cout << "\n=== DELETE EVENT ===" << std::endl;
            if (events.count() == 0) {
                std::cout << "No events to delete." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }
            for (const auto& doc : events.all())
                displayEvent(doc);

            std::cout << "\nEnter event ID to delete: ";
            std::string id;
            std::getline(std::cin, id);

            if (!events.exists(id)) {
                std::cout << "Error: event ID " << id << " not found." << std::endl;
                std::cout << "\nPress Enter to continue..."; std::cin.get();
                clearScreen(); break;
            }

            std::cout << "Are you sure? (y/N): ";
            std::string confirm;
            std::getline(std::cin, confirm);

            if (confirm == "y" || confirm == "Y") {
                events.remove(id);
                _db.save();
                std::cout << "Event deleted." << std::endl;
            } else {
                std::cout << "Cancelled." << std::endl;
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }

        case 5:
            clearScreen();
            handleEventSearch();
            break;

        case 6:
            clearScreen();
            in_menu = false;
            break;

        default:
            std::cout << "Invalid option." << std::endl;
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
        }
    }
}

void CampusApp::handleResourceSearch() {
    Collection& resources = _db.getCollection("resources");
    bool in_menu = true;

    while (in_menu) {
        std::cout << "==================================================" << std::endl;
        std::cout << "  Search Resources" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "  1 | Find by ID       (hash map O(1) lookup)" << std::endl;
        std::cout << "  2 | Filter by Type   (secondary index)" << std::endl;
        std::cout << "  3 | Search by Name   (case-insensitive scan)" << std::endl;
        std::cout << "  4 | Back" << std::endl;
        std::cout << std::endl << "Choose option (1-4): ";

        std::string input;
        std::getline(std::cin, input);
        if (!isNumeric(input)) {
            std::cout << "Please enter a number." << std::endl;
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen(); continue;
        }

        switch (std::stoi(input)) {
        case 1: {
            std::cout << "\nEnter resource ID: ";
            std::string id;
            std::getline(std::cin, id);
            Document* doc = resources.find(id);
            if (doc) {
                std::cout << "\n=== FOUND (hash map lookup) ===" << std::endl;
                displayResource(*doc);
            } else {
                std::cout << "No resource found with ID " << id << std::endl;
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }
        case 2: {
            std::cout << "\nEnter type to filter: ";
            std::string type;
            std::getline(std::cin, type);
            auto results = resources.filter("type", type);
            if (results.empty()) {
                std::cout << "No resources with type '" << type << "'" << std::endl;
            } else {
                std::cout << "\n=== FILTER RESULTS (" << results.size()
                          << " documents, via secondary index) ===" << std::endl;
                for (const auto& doc : results)
                    displayResource(doc);
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }
        case 3: {
            std::cout << "\nEnter name to search: ";
            std::string name;
            std::getline(std::cin, name);
            auto results = resources.search("name", name);
            if (results.empty()) {
                std::cout << "No resources matching '" << name << "'" << std::endl;
            } else {
                std::cout << "\n=== SEARCH RESULTS (" << results.size()
                          << " documents, full scan) ===" << std::endl;
                for (const auto& doc : results)
                    displayResource(doc);
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }
        case 4:
            clearScreen();
            in_menu = false;
            break;
        default:
            std::cout << "Invalid option." << std::endl;
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
        }
    }
}

void CampusApp::handleEventSearch() {
    Collection& events = _db.getCollection("events");
    bool in_menu = true;

    while (in_menu) {
        std::cout << "==================================================" << std::endl;
        std::cout << "  Search Events" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "  1 | Find by ID       (hash map O(1) lookup)" << std::endl;
        std::cout << "  2 | Filter by Date   (secondary index)" << std::endl;
        std::cout << "  3 | Search by Title  (case-insensitive scan)" << std::endl;
        std::cout << "  4 | Back" << std::endl;
        std::cout << std::endl << "Choose option (1-4): ";

        std::string input;
        std::getline(std::cin, input);
        if (!isNumeric(input)) {
            std::cout << "Please enter a number." << std::endl;
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen(); continue;
        }

        switch (std::stoi(input)) {
        case 1: {
            std::cout << "\nEnter event ID: ";
            std::string id;
            std::getline(std::cin, id);
            Document* doc = events.find(id);
            if (doc) {
                std::cout << "\n=== FOUND (hash map lookup) ===" << std::endl;
                displayEvent(*doc);
            } else {
                std::cout << "No event found with ID " << id << std::endl;
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }
        case 2: {
            std::cout << "\nEnter date [YYYY-MM-DD]: ";
            std::string date;
            std::getline(std::cin, date);
            auto results = events.filter("date", date);
            if (results.empty()) {
                std::cout << "No events on " << date << std::endl;
            } else {
                std::cout << "\n=== FILTER RESULTS (" << results.size()
                          << " documents, via secondary index) ===" << std::endl;
                for (const auto& doc : results)
                    displayEvent(doc);
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }
        case 3: {
            std::cout << "\nEnter title to search: ";
            std::string title;
            std::getline(std::cin, title);
            auto results = events.search("title", title);
            if (results.empty()) {
                std::cout << "No events matching '" << title << "'" << std::endl;
            } else {
                std::cout << "\n=== SEARCH RESULTS (" << results.size()
                          << " documents, full scan) ===" << std::endl;
                for (const auto& doc : results)
                    displayEvent(doc);
            }
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
            break;
        }
        case 4:
            clearScreen();
            in_menu = false;
            break;
        default:
            std::cout << "Invalid option." << std::endl;
            std::cout << "\nPress Enter to continue..."; std::cin.get();
            clearScreen();
        }
    }
}

void CampusApp::displayDatabaseStatus() const {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Database Status" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  Database: " << _db.name() << std::endl;
    std::cout << "  Data dir: " << _db.dataDir() << "/" << std::endl;
    std::cout << std::endl;
    std::cout << "  Collections:" << std::endl;

    for (const auto& cname : _db.collectionNames()) {
        const Collection& col = _db.getCollection(cname);
        std::cout << "    - " << cname << ": " << col.count() << " document(s)";

        auto idx_fields = col.indexedFields();
        if (!idx_fields.empty()) {
            std::cout << ", indexes: [";
            for (size_t i = 0; i < idx_fields.size(); i++) {
                if (i > 0) std::cout << ", ";
                std::cout << idx_fields[i];
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
}
