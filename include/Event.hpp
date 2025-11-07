#ifndef EVENT_HPP
#define EVENT_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

class Event {
    private:
        unsigned int id;
        std::string title;
        std::string date;              
        std::string start_time;        
        int duration_minutes;
        std::vector<unsigned int> resource_ids;  

    public:
        Event();
        Event(unsigned int id, const std::string& title, const std::string& date,
              const std::string& start_time, int duration_minutes, 
              const std::vector<unsigned int>& resource_ids);  
        ~Event();

        unsigned int getId() const;
        std::string getTitle() const;
        std::string getDate() const;
        std::string getStartTime() const;
        int getDurationMinutes() const;
        std::vector<unsigned int> getResourceIds() const;  
        void setTitle(const std::string& title);
        void setDate(const std::string& date);
        void setStartTime(const std::string& start_time);
        void setDurationMinutes(int duration_minutes);
        void setResourceIds(const std::vector<unsigned int>& resource_ids);  
        void addResourceId(unsigned int resource_id); 
        void removeResourceId(unsigned int resource_id); 
        void displayInfo() const;
        std::string toString() const;
        std::string getEndTime() const;
};

#endif