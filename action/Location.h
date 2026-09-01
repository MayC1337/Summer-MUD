#pragma once

#include <string>

enum class LocationType {
    Library,
    Gym,
    GameHall,
    Shop,
    Home
};

class Location {
private:
    std::string name_;
    std::string description_;
    LocationType type_;

public:
    Location(std::string name, std::string description, LocationType type);

    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    LocationType getType() const { return type_; }

    void enter() const;
    void show() const;
};
