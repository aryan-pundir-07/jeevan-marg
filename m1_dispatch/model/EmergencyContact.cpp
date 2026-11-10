#include "EmergencyContact.h"
#include <sstream>

EmergencyContact::EmergencyContact() : name(""), phone(""), relationship("Next of Kin") {}

EmergencyContact::EmergencyContact(const std::string& n, const std::string& p, const std::string& r)
    : name(n), phone(p), relationship(r) {}

std::string EmergencyContact::toJson() const {
    std::ostringstream ss;
    ss << "{\"name\":\"" << name << "\",\"phone\":\"" << phone << "\",\"relationship\":\"" << relationship << "\"}";
    return ss.str();
}
