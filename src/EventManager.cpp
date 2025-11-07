#include "EventManager.hpp"

EventManager::EventManager(ResourceManager* res_manager) {
    resource_manager = res_manager;
    data_file = "data/events.txt";
    next_id = 1;
}

EventManager::~EventManager() {
}

std::string EventManager::calculateEndTime(const std::string& start_time, int duration_minutes) const {
    int hours = std::stoi(start_time.substr(0, 2));
    int minutes = std::stoi(start_time.substr(3, 2));
    
    minutes += duration_minutes;
    hours += minutes / 60;
    minutes = minutes % 60;
    hours = hours % 24;
    
    std::stringstream ss;
    ss << (hours < 10 ? "0" : "") << hours << ":" 
       << (minutes < 10 ? "0" : "") << minutes;
    
    return ss.str();
}

bool EventManager::timesOverlap(const std::string& start1, const std::string& end1,
                                const std::string& start2, const std::string& end2) const {
    return (start1 < end2) && (end1 > start2);
}

bool EventManager::hasConflict(const std::vector<unsigned int>& resource_ids, const std::string& date, 
                               const std::string& start_time, int duration_minutes,
                               unsigned int exclude_event_id) const {
    std::string end_time = calculateEndTime(start_time, duration_minutes);
    
    for (unsigned int r = 0; r < resource_ids.size(); r++) {
        unsigned int resource_id = resource_ids[r];
        
        for (unsigned int i = 0; i < events.size(); i++) {
            if (events[i].getId() == exclude_event_id) {
                continue;
            }
            
            std::vector<unsigned int> event_resources = events[i].getResourceIds();
            bool uses_same_resource = false;
            
            for (unsigned int j = 0; j < event_resources.size(); j++) {
                if (event_resources[j] == resource_id) {
                    uses_same_resource = true;
                    break;
                }
            }
            
            if (uses_same_resource && events[i].getDate() == date) {
                std::string existing_end = events[i].getEndTime();
                
                if (timesOverlap(start_time, end_time, 
                               events[i].getStartTime(), existing_end)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool EventManager::isResourceUsedByEvents(unsigned int resource_id) const {
    for (unsigned int i = 0; i < events.size(); i++) {
        std::vector<unsigned int> event_resources = events[i].getResourceIds();
        for (unsigned int j = 0; j < event_resources.size(); j++) {
            if (event_resources[j] == resource_id) {
                return true;
            }
        }
    }
    return false;
}

std::vector<unsigned int> EventManager::selectMultipleResourcesInteractive() {
    std::vector<unsigned int> selected_resources;
    
    std::cout << "\n--- Select Resources (you can select multiple) ---" << std::endl;
    std::cout << "1. View all resources and select" << std::endl;
    std::cout << "2. Search by name" << std::endl;
    std::cout << "3. Search by type" << std::endl;
    std::cout << "Enter your choice (1-3): ";
    
    std::string choice_str;
    std::getline(std::cin, choice_str);
    
    if (choice_str == "1") {
        resource_manager->viewAllResources();
    } else if (choice_str == "2") {
        std::cout << "Enter resource name to search: ";
        std::string search_term;
        std::getline(std::cin, search_term);
        
        if (!search_term.empty()) {
            resource_manager->searchResourceByName(search_term);
        }
    } else if (choice_str == "3") {
        std::cout << "Enter resource type to search: ";
        std::string search_term;
        std::getline(std::cin, search_term);
        
        if (!search_term.empty()) {
            resource_manager->searchResourceByType(search_term);
        }
    } else {
        std::cout << "Invalid choice!" << std::endl;
        return selected_resources;
    }
    
    std::cout << "\nEnter resource IDs separated by commas (e.g., 1,3,5): ";
    std::string ids_str;
    std::getline(std::cin, ids_str);
    
    std::stringstream ss(ids_str);
    std::string id_str;
    
    while (std::getline(ss, id_str, ',')) {
        while (!id_str.empty() && (id_str[0] == ' ' || id_str[0] == '\t')) {
            id_str = id_str.substr(1);
        }
        while (!id_str.empty() && (id_str[id_str.length()-1] == ' ' || id_str[id_str.length()-1] == '\t')) {
            id_str = id_str.substr(0, id_str.length()-1);
        }
        
        if (!id_str.empty()) {
            unsigned int id;
            std::stringstream id_ss(id_str);
            id_ss >> id;
            
            if (resource_manager->resourceExists(id)) {
                selected_resources.push_back(id);
            } else {
                std::cout << "Warning: Resource ID " << id << " does not exist. Skipping." << std::endl;
            }
        }
    }
    
    if (selected_resources.empty()) {
        std::cout << "No valid resources selected!" << std::endl;
    } else {
        std::cout << "Selected " << selected_resources.size() << " resource(s)." << std::endl;
    }
    
    return selected_resources;
}

void EventManager::addEvent(const std::string& title, const std::string& date,
                           const std::string& start_time, int duration_minutes, 
                           const std::vector<unsigned int>& resource_ids) {
    if (resource_ids.empty()) {
        std::cout << "Error: No resources selected!" << std::endl;
        return;
    }
    
    for (unsigned int i = 0; i < resource_ids.size(); i++) {
        if (!resource_manager->resourceExists(resource_ids[i])) {
            std::cout << "Error: Resource with ID " << resource_ids[i] << " does not exist!" << std::endl;
            return;
        }
        
        Resource* resource = resource_manager->findResource(resource_ids[i]);
        if (resource != NULL && !resource->isAvailable()) {
            std::cout << "Error: Resource '" << resource->getName() 
                      << "' is not available (under maintenance or out of service)." << std::endl;
            std::cout << "Please select only available resources." << std::endl;
            return;
        }
    }
    
    if (hasConflict(resource_ids, date, start_time, duration_minutes)) {
        std::cout << "Error: Time conflict detected! One or more resources are already booked at this time." << std::endl;
        std::cout << "Please choose a different time or different resources." << std::endl;
        return;
    }
    
    Event new_event(next_id, title, date, start_time, duration_minutes, resource_ids);
    events.push_back(new_event);
    
    std::cout << "Event '" << title << "' added successfully with ID: " << next_id << std::endl;
    std::cout << "Resources booked for " << date << " from " << start_time << " to " 
              << calculateEndTime(start_time, duration_minutes) << ":" << std::endl;
    
    for (unsigned int i = 0; i < resource_ids.size(); i++) {
        Resource* resource = resource_manager->findResource(resource_ids[i]);
        if (resource != NULL) {
            std::cout << "  - " << resource->getName() << " (ID: " << resource_ids[i] << ")" << std::endl;
        }
    }
    
    next_id++;
    saveToFile();
}

void EventManager::viewAllEvents() const {
    if (events.empty()) {
        std::cout << "\n=== NO EVENTS FOUND ===" << std::endl;
        std::cout << "No events have been scheduled yet." << std::endl;
        std::cout << "Use 'Add New Event' option to schedule events." << std::endl;
        return;
    }

    std::cout << "\n=== ALL EVENTS ===" << std::endl;
    std::cout << "Total Events: " << events.size() << std::endl;
    std::cout << std::endl;
    
    for (unsigned int i = 0; i < events.size(); i++) {
        events[i].displayInfo();
        
        std::vector<unsigned int> resource_ids = events[i].getResourceIds();
        std::cout << "Resources: ";
        if (resource_ids.empty()) {
            std::cout << "None";
        } else {
            for (unsigned int j = 0; j < resource_ids.size(); j++) {
                Resource* resource = resource_manager->findResource(resource_ids[j]);
                if (resource != NULL) {
                    std::cout << resource->getName();
                    if (j < resource_ids.size() - 1) {
                        std::cout << ", ";
                    }
                }
            }
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}

void EventManager::updateEvent(unsigned int id) {
    Event* event = findEvent(id);
    if (event == NULL) {
        std::cout << "Error: Event with ID " << id << " not found." << std::endl;
        return;
    }

    std::cout << "Current event information:" << std::endl;
    event->displayInfo();
    
    std::cout << "\nEnter new information (press Enter to keep current value):" << std::endl;
    
    std::cout << "New title (current: " << event->getTitle() << "): ";
    std::string new_title;
    std::getline(std::cin, new_title);
    if (!new_title.empty()) {
        event->setTitle(new_title);
    }
    
    std::cout << "New date [YYYY-MM-DD] (current: " << event->getDate() << "): ";
    std::string new_date;
    std::getline(std::cin, new_date);
    if (!new_date.empty() && !isValidDate(new_date)) {
        std::cout << "Error: Invalid date format! Update cancelled." << std::endl;
        return;
    }

    std::cout << "New start time [HH:MM] (current: " << event->getStartTime() << "): ";
    std::string new_start_time;
    std::getline(std::cin, new_start_time);
    if (!new_start_time.empty() && !isValidTime(new_start_time)) {
        std::cout << "Error: Invalid time format! Update cancelled." << std::endl;
        return;
    }
    
    std::cout << "New duration in minutes (current: " << event->getDurationMinutes() << "): ";
    std::string duration_str;
    std::getline(std::cin, duration_str);
    int new_duration = 0;

    if (!duration_str.empty()) {
        for (unsigned int i = 0; i < duration_str.length(); i++) {
            if (!std::isdigit(duration_str[i])) {
                std::cout << "Error: Duration must be a number! Update cancelled." << std::endl;
                return;
            }
        }
        new_duration = std::stoi(duration_str);
        if (new_duration <= 0 || new_duration > 1440) {
            std::cout << "Error: Duration must be between 1 and 1440 minutes! Update cancelled." << std::endl;
            return;
        }
    }

    std::cout << "Update resources? (y/N): ";
    std::string update_resources;
    std::getline(std::cin, update_resources);
    
    std::vector<unsigned int> new_resource_ids = event->getResourceIds();
    
    if (update_resources == "y" || update_resources == "Y") {
        new_resource_ids = selectMultipleResourcesInteractive();
        if (new_resource_ids.empty()) {
            std::cout << "Error: No valid resources selected. Keeping current resources." << std::endl;
            new_resource_ids = event->getResourceIds();
        }
    }
    
    for (unsigned int i = 0; i < new_resource_ids.size(); i++) {
        Resource* resource = resource_manager->findResource(new_resource_ids[i]);
        if (resource != NULL && !resource->isAvailable()) {
            std::cout << "Error: Resource '" << resource->getName() 
                      << "' is not available. Update cancelled." << std::endl;
            return;
        }
    }
    
    std::string check_date = new_date.empty() ? event->getDate() : new_date;
    std::string check_time = new_start_time.empty() ? event->getStartTime() : new_start_time;
    int check_duration = duration_str.empty() ? event->getDurationMinutes() : new_duration;
    
    if (hasConflict(new_resource_ids, check_date, check_time, check_duration, event->getId())) {
        std::cout << "Error: Update would cause a time conflict! Changes not saved." << std::endl;
        return;
    }
    
    if (!new_date.empty()) event->setDate(new_date);
    if (!new_start_time.empty()) event->setStartTime(new_start_time);
    if (!duration_str.empty()) event->setDurationMinutes(check_duration);
    event->setResourceIds(new_resource_ids);
    
    std::cout << "Event updated successfully!" << std::endl;
    saveToFile();
}

void EventManager::updateEventByTitle(const std::string& title) {
    Event* event = findEventByTitle(title);
    if (event == NULL) {
        std::cout << "Error: Event with title '" << title << "' not found." << std::endl;
        return;
    }
    updateEvent(event->getId());
}

void EventManager::deleteEvent(unsigned int id) {
    for (unsigned int i = 0; i < events.size(); i++) {
        if (events[i].getId() == id) {
            std::string event_title = events[i].getTitle();
            std::cout << "Deleting event: " << event_title << " (ID: " << id << ")" << std::endl;
            events.erase(events.begin() + i);
            std::cout << "Event deleted successfully." << std::endl;
            saveToFile();
            return;
        }
    }
    std::cout << "Error: Event with ID " << id << " not found." << std::endl;
}

void EventManager::deleteEventByTitle(const std::string& title) {
    Event* event = findEventByTitle(title);
    if (event == NULL) {
        std::cout << "Error: Event with title '" << title << "' not found." << std::endl;
        return;
    }
    unsigned int id = event->getId();
    deleteEvent(id);
}

std::string EventManager::toLowercase(const std::string& str) const {
    std::string result = str;
    for (unsigned int i = 0; i < result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + 32;
        }
    }
    return result;
}

bool EventManager::isValidDate(const std::string& date) const {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(date[i])) return false;
    }
    
    int year, month, day;
    std::stringstream(date.substr(0, 4)) >> year;
    std::stringstream(date.substr(5, 2)) >> month;
    std::stringstream(date.substr(8, 2)) >> day;
    
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    bool is_leap_year = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (is_leap_year) {
        days_in_month[1] = 29;
    }
    
    if (day > days_in_month[month - 1]) {
        return false;
    }
    
    return true;
}

bool EventManager::isValidTime(const std::string& time) const {
    if (time.length() != 5) return false;
    if (time[2] != ':') return false;
    
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!std::isdigit(time[i])) return false;
    }
    
    int hours, minutes;
    std::stringstream(time.substr(0, 2)) >> hours;
    std::stringstream(time.substr(3, 2)) >> minutes;
    
    if (hours < 0 || hours > 23) return false;
    if (minutes < 0 || minutes > 59) return false;
    
    return true;
}

int EventManager::getEventCount() const {
    return events.size();
}

bool EventManager::eventExists(unsigned int id) const {
    for (unsigned int i = 0; i < events.size(); i++) {
        if (events[i].getId() == id) {
            return true;
        }
    }
    return false;
}

bool EventManager::eventExistsByTitle(const std::string& title) const {
    std::string search_lower = toLowercase(title);
    for (unsigned int i = 0; i < events.size(); i++) {
        if (toLowercase(events[i].getTitle()) == search_lower) {
            return true;
        }
    }
    return false;
}

Event* EventManager::findEvent(unsigned int id) {
    for (unsigned int i = 0; i < events.size(); i++) {
        if (events[i].getId() == id) {
            return &events[i];
        }
    }
    return NULL;
}

Event* EventManager::findEventByTitle(const std::string& title) {
    std::string search_lower = toLowercase(title);
    for (unsigned int i = 0; i < events.size(); i++) {
        if (toLowercase(events[i].getTitle()) == search_lower) {
            return &events[i];
        }
    }
    return NULL;
}

void EventManager::saveToFile() const {
    std::ofstream file(data_file);
    
    if (!file.is_open()) {
        return;
    }
    
    for (unsigned int i = 0; i < events.size(); i++) {
        file << events[i].toString() << std::endl;
    }
    
    file.close();
}

void EventManager::loadFromFile() {
    std::ifstream file(data_file);
    
    if (!file.is_open()) {
        return;
    }
    
    std::vector<Event> temp_events;
    std::string line;
    unsigned int max_id = 0;
    
    while (std::getline(file, line)) {
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string part;
        
        while (std::getline(ss, part, '|')) {
            parts.push_back(part);
        }
        
        if (parts.size() != 6) {
            file.close();
            return;
        }
        
        unsigned int id;
        std::stringstream(parts[0]) >> id;
        
        int duration;
        std::stringstream(parts[4]) >> duration;
        
        std::vector<unsigned int> resource_ids;
        std::stringstream resource_ss(parts[5]);
        std::string resource_id_str;
        
        while (std::getline(resource_ss, resource_id_str, ',')) {
            unsigned int resource_id;
            std::stringstream id_ss(resource_id_str);
            id_ss >> resource_id;
            resource_ids.push_back(resource_id);
        }
        
        Event loaded_event(id, parts[1], parts[2], parts[3], duration, resource_ids);
        temp_events.push_back(loaded_event);
        
        max_id = (id > max_id) ? id : max_id;
    }
    
    file.close();
    events = temp_events;
    next_id = max_id + 1;
}

void EventManager::searchEventById(unsigned int id) const {
    Event* event = const_cast<EventManager*>(this)->findEvent(id);
    
    if (event == NULL) {
        std::cout << "No event found with ID " << id << std::endl;
    } else {
        std::cout << "\n=== SEARCH RESULTS ===" << std::endl;
        std::cout << "Found 1 event matching ID " << id << ":" << std::endl;
        event->displayInfo();
        
        std::vector<unsigned int> resource_ids = event->getResourceIds();
        std::cout << "Resources: ";
        if (resource_ids.empty()) {
            std::cout << "None";
        } else {
            for (unsigned int j = 0; j < resource_ids.size(); j++) {
                Resource* resource = resource_manager->findResource(resource_ids[j]);
                if (resource != NULL) {
                    std::cout << resource->getName();
                    if (j < resource_ids.size() - 1) {
                        std::cout << ", ";
                    }
                }
            }
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
}

void EventManager::searchEventByTitle(const std::string& title) const {
    std::vector<Event> found_events;
    std::string search_lower = toLowercase(title);
    
    for (unsigned int i = 0; i < events.size(); i++) {
        std::string title_lower = toLowercase(events[i].getTitle());
        
        if (title_lower.find(search_lower) != std::string::npos) {
            found_events.push_back(events[i]);
        }
    }
    
    if (found_events.empty()) {
        std::cout << "No events found matching title '" << title << "'" << std::endl;
    } else {
        std::cout << "\n=== SEARCH RESULTS ===" << std::endl;
        std::cout << "Found " << found_events.size() << " event(s) matching title '" << title << "':" << std::endl;
        for (unsigned int i = 0; i < found_events.size(); i++) {
            found_events[i].displayInfo();
            
            std::vector<unsigned int> resource_ids = found_events[i].getResourceIds();
            std::cout << "Resources: ";
            if (resource_ids.empty()) {
                std::cout << "None";
            } else {
                for (unsigned int j = 0; j < resource_ids.size(); j++) {
                    Resource* resource = resource_manager->findResource(resource_ids[j]);
                    if (resource != NULL) {
                        std::cout << resource->getName();
                        if (j < resource_ids.size() - 1) {
                            std::cout << ", ";
                        }
                    }
                }
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
}