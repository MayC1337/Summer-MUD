#include "Location.h"
#include <iostream>
#include <utility>

Location::Location(std::string name, std::string description, LocationType type)
    : name_(std::move(name)), description_(std::move(description)), type_(type) {}

void Location::enter() const {
    std::cout << "你来到【" << name_ << "】。" << description_ << "\n";
}

void Location::show() const {
    std::cout << name_ << " - " << description_ << "\n";
}
