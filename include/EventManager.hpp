#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "Event.hpp"
#include "ResourceManager.hpp"
#include <vector>

class EventManager {
    private:
        std::vector<Event> events;
        ResourceManager* resource_manager;
        std::string data_file;
        unsigned int next_id;
        
        bool hasConflict(const std::vector<unsigned int>& resource_ids, const std::string& date, 
                        const std::string& start_time, int duration_minutes,
                        unsigned int exclude_event_id = 0) const;
        
        bool timesOverlap(const std::string& start1, const std::string& end1,
                        const std::string& start2, const std::string& end2) const;
        
        std::string calculateEndTime(const std::string& start_time, int duration_minutes) const;
        
        std::string toLowercase(const std::string& str) const;
        
        bool isValidDate(const std::string& date) const;
        bool isValidTime(const std::string& time) const;

    public:
        EventManager(ResourceManager* res_manager);
        ~EventManager();
        
        void addEvent(const std::string& title, const std::string& date,
                     const std::string& start_time, int duration_minutes, 
                     const std::vector<unsigned int>& resource_ids);
        void viewAllEvents() const;
        void updateEvent(unsigned int id);
        void updateEventByTitle(const std::string& title);
        void deleteEvent(unsigned int id);
        void deleteEventByTitle(const std::string& title);
        
        std::vector<unsigned int> selectMultipleResourcesInteractive();
        
        int getEventCount() const;
        bool eventExists(unsigned int id) const;
        bool eventExistsByTitle(const std::string& title) const;
        Event* findEvent(unsigned int id);
        Event* findEventByTitle(const std::string& title);
        
        bool isResourceUsedByEvents(unsigned int resource_id) const;
        
        void saveToFile() const;
        void loadFromFile();

        void searchEventById(unsigned int id) const;
        void searchEventByTitle(const std::string& title) const;
};

#endif