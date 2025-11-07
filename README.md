# Campus Resource & Event Management System

A comprehensive C++ application for managing campus resources (rooms, equipment, labs) and scheduling events with automatic conflict detection and data persistence.

## Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Class Relationships](#class-relationships)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [How to Run](#how-to-run)
- [Usage Guide](#usage-guide)
- [Data Storage](#data-storage)
- [Project Structure](#project-structure)

## Overview

This system helps educational institutions manage their resources and events efficiently. It prevents double-booking, tracks resource availability, and maintains persistent data storage.

## System Architecture

The application follows Object-Oriented Programming principles with clear separation of concerns:
```
CampusSystem (Main Controller)
    |
    |--- ResourceManager (Manages all resources)
    |        |
    |        |--- Resource (Individual resource objects)
    |
    |--- EventManager (Manages all events)
             |
             |--- Event (Individual event objects)
             |
             |--- Uses ResourceManager to validate bookings
```

### Flow of Control
```
main.cpp
    |
    v
CampusSystem Constructor
    |
    |--- Auto-loads data from files
    |--- Initializes ResourceManager
    |--- Initializes EventManager
    |
    v
CampusSystem::run()
    |
    |--- Display Main Menu
    |--- Handle User Input
    |       |
    |       |--- Resource Management
    |       |       |
    |       |       |--- Add/View/Update/Delete/Search Resources
    |       |       |--- Auto-saves after each modification
    |       |
    |       |--- Event Management
    |       |       |
    |       |       |--- Add/View/Update/Delete/Search Events
    |       |       |--- Checks resource availability
    |       |       |--- Detects time conflicts
    |       |       |--- Auto-saves after each modification
    |       |
    |       |--- View System Status
    |       |
    |       |--- Exit
    |
    v
CampusSystem Destructor
    |
    |--- Display goodbye message
    |--- Clear screen
```

## Class Relationships

### 1. Resource Class
Represents a single campus resource.

**Attributes:**
- id: Unique identifier
- name: Resource name
- type: Resource type (Room, Projector, Lab)
- is_available: Availability status

**Key Methods:**
- getId(), getName(), getType(), isAvailable()
- setName(), setType(), setAvailability()
- displayInfo(), toString()

### 2. ResourceManager Class
Manages all resources in the system.

**Attributes:**
- resources: Vector of Resource objects
- data_file: Path to storage file
- next_id: Counter for generating unique IDs

**Key Methods:**
- addResource() - Creates new resource and auto-saves
- viewAllResources() - Displays all resources
- updateResource() - Modifies resource and auto-saves
- deleteResource() - Removes resource and auto-saves
- searchResourceById(), searchResourceByName(), searchResourceByType()
- saveToFile() - Writes data to disk
- loadFromFile() - Reads data from disk

### 3. Event Class
Represents a single scheduled event.

**Attributes:**
- id: Unique identifier
- title: Event name
- date: Event date (YYYY-MM-DD)
- start_time: Start time (HH:MM)
- duration_minutes: Duration in minutes
- resource_ids: List of resource IDs used by this event

**Key Methods:**
- getId(), getTitle(), getDate(), getStartTime(), getDurationMinutes()
- setTitle(), setDate(), setStartTime(), setDurationMinutes()
- getResourceIds(), setResourceIds()
- getEndTime() - Calculates end time
- displayInfo(), toString()

### 4. EventManager Class
Manages all events and validates resource bookings.

**Attributes:**
- events: Vector of Event objects
- resource_manager: Pointer to ResourceManager
- data_file: Path to storage file
- next_id: Counter for generating unique IDs

**Key Methods:**
- addEvent() - Creates event, validates resources, checks conflicts, auto-saves
- viewAllEvents() - Displays all events with resource names
- updateEvent() - Modifies event, revalidates, auto-saves
- deleteEvent() - Removes event and auto-saves
- searchEventById(), searchEventByTitle()
- hasConflict() - Checks for time and resource conflicts
- isResourceUsedByEvents() - Checks if resource is in use
- saveToFile(), loadFromFile()

### 5. CampusSystem Class
Main controller that coordinates all operations.

**Attributes:**
- resource_manager: ResourceManager instance
- event_manager: EventManager instance
- is_running: System state flag

**Key Methods:**
- Constructor: Auto-loads data on startup
- run() - Main program loop
- handleMainMenu() - Processes main menu choices
- handleResourceMenu() - Handles resource operations
- handleEventMenu() - Handles event operations
- displaySystemStatus() - Shows system statistics
- Destructor: Displays exit message and cleans up

### 6. Utility Functions (outils.hpp/cpp)
Helper functions for UI and system operations.

**Functions:**
- clearScreen() - Clears console
- sleepSeconds() - Pauses execution
- systemMainMenu() - Displays main menu
- systemResourceMenu() - Displays resource menu
- systemEventMenu() - Displays event menu
- systemResourceSearchMenu() - Displays resource search menu
- systemEventSearchMenu() - Displays event search menu

## Features

### Resource Management
- Add resources with availability status
- View all resources with details
- Update resource information
- Delete resources (with warnings if in use)
- Search by ID, name, or type
- Automatic data persistence

### Event Management
- Schedule events with multiple resources
- View all events with resource details
- Update event information
- Delete events
- Search by ID or title
- Automatic conflict detection
- Prevents booking unavailable resources
- Automatic data persistence

### Data Validation
- Date format validation (YYYY-MM-DD)
- Time format validation (HH:MM)
- Numeric input validation
- Empty input prevention
- Duplicate prevention

### User Experience
- Error recovery - re-enter only invalid fields
- Clear error messages
- Confirmation prompts for deletions
- Auto-save after every modification
- Auto-load on startup
- Clean exit with goodbye message

## Prerequisites

To compile and run this program, you need:

1. C++ Compiler with C++17 support or higher
   - GCC 4.8+ (Linux/Mac)
   - Clang 3.3+ (Mac/Linux)
   - Visual Studio 2015+

2. Make (for using Makefile)

## How to Run

#### Compile the program:
```bash
make
```

#### Run the program:

Linux/Mac:
```bash
./campus_system
```

#### Clean compiled files:
```bash
make clean
```
## Usage Guide

### Starting the Program

When you run the program:
1. It automatically loads existing data from files
2. Press Enter to see the main menu
3. Choose options by entering numbers

### Main Menu Options
```
1. Resource Management - Manage campus resources
2. Event Management - Schedule and manage events
3. View System Status - See statistics
4. Exit - Close the program
```

### Adding a Resource

1. Select "Resource Management" from main menu
2. Choose "Add New Resource"
3. Enter resource name (example: Room 101)
4. Enter resource type (example: Classroom)
5. Set availability (y for available, n for unavailable)
6. Resource is automatically saved

### Adding an Event

1. Select "Event Management" from main menu
2. Choose "Add New Event"
3. Enter event title (example: Math Lecture)
4. Enter date in YYYY-MM-DD format (example: 2025-12-25)
5. Enter start time in HH:MM format (example: 14:30)
6. Enter duration in minutes (example: 90)
7. Select resources:
   - View all resources OR search by name/type
   - Enter resource IDs separated by commas (example: 1,3,5)
8. System checks for conflicts and availability
9. Event is automatically saved if valid

### Updating Resources or Events

You can update using either:
- ID number (example: 1)
- Name or title (example: Room 101)

### Searching

Resources can be searched by:
- ID
- Name (partial match supported)
- Type (partial match supported)

Events can be searched by:
- ID
- Title (partial match supported)

### Error Recovery

If you make a mistake:
1. System shows an error message
2. Press Enter
3. You only re-enter the field that had an error
4. You don't need to start over

### Exiting the Program

1. Choose "Exit" from main menu
2. System displays goodbye message
3. Screen clears after 1 second
4. All data is already saved

## Data Storage

### File Locations

Data is stored in the `data` directory:
```
data/
    resources.txt  - Resource data
    events.txt     - Event data
```

### Data Format

**resources.txt format:**
```
ID|Name|Type|IsAvailable
1|Room 101|Classroom|1
2|Projector A|Equipment|0
```

**events.txt format:**
```
ID|Title|Date|StartTime|Duration|ResourceIDs
1|Math Lecture|2025-12-25|14:30|90|1,2
```

Note: Do not manually edit these files while the program is running.

### Automatic Data Management

- Data loads automatically when program starts
- Data saves automatically after every add, update, or delete
- No need to manually save
- Data persists between program sessions

## Project Structure
```
campus-system/
│
├── main.cpp                 - Program entry point
├── CampusSystem.hpp         - Main controller header
├── CampusSystem.cpp         - Main controller implementation
│
├── Resource.hpp             - Resource class header
├── Resource.cpp             - Resource class implementation
├── ResourceManager.hpp      - Resource manager header
├── ResourceManager.cpp      - Resource manager implementation
│
├── Event.hpp                - Event class header
├── Event.cpp                - Event class implementation
├── EventManager.hpp         - Event manager header
├── EventManager.cpp         - Event manager implementation
│
├── outils.hpp               - Utility functions header
├── outils.cpp               - Utility functions implementation
│
├── data/                    - Data storage directory
│   ├── resources.txt        - Resource data file
│   └── events.txt           - Event data file
│
└── README.md                - This file
```

## Troubleshooting

### Problem: Program won't compile

**Solution:** Make sure you have a C++11 compatible compiler installed and use the `-std=c++11` flag.

### Problem: Cannot find data directory

**Solution:** Create a `data` folder in the same directory as your executable file.

### Problem: Changes are not saved

**Solution:** Check that the program has write permissions in the `data` directory.

### Problem: Data files are corrupted

**Solution:** Delete `resources.txt` and `events.txt` from the `data` folder and restart the program.

## Important Notes

1. Always run the program from the directory containing the executable
2. Do not manually edit data files while program is running
3. The program automatically prevents conflicts and validates all inputs
4. Resources can only be set to unavailable if they are not being used by any events
5. All modifications are automatically saved
