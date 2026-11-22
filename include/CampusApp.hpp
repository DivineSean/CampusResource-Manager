#ifndef CAMPUSAPP_HPP
#define CAMPUSAPP_HPP

#include "Database.hpp"
#include <string>
#include <vector>

class CampusApp {
    private:
        Database _db;
        bool _running;

        bool isNumeric(const std::string& str) const;
        bool isValidDate(const std::string& date) const;
        bool isValidTime(const std::string& time) const;
        std::string toLower(const std::string& str) const;

        std::string calculateEndTime(const std::string& start, int duration) const;
        bool timesOverlap(const std::string& s1, const std::string& e1,
                          const std::string& s2, const std::string& e2) const;

        bool hasConflict(const std::vector<std::string>& resource_ids,
                         const std::string& date, const std::string& start_time,
                         int duration, const std::string& exclude_id = "") const;
        bool isResourceUsedByEvents(const std::string& resource_id) const;
        std::vector<std::string> selectResources();

        void displayResource(const Document& doc) const;
        void displayEvent(const Document& doc) const;

        void handleMainMenu();
        void handleResourceMenu();
        void handleEventMenu();
        void handleResourceSearch();
        void handleEventSearch();
        void displayDatabaseStatus() const;

    public:
        CampusApp();
        void run();
        ~CampusApp();
};

#endif
