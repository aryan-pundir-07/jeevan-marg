#ifndef M1_MODEL_EMERGENCY_CONTACT_H
#define M1_MODEL_EMERGENCY_CONTACT_H

#include <string>

class EmergencyContact {
private:
    std::string name;
    std::string phone;
    std::string relationship;

public:
    EmergencyContact();
    EmergencyContact(const std::string& name, const std::string& phone, const std::string& relation = "Next of Kin");

    const std::string& getName() const { return name; }
    const std::string& getPhone() const { return phone; }
    const std::string& getRelationship() const { return relationship; }

    std::string toJson() const;
};

#endif // M1_MODEL_EMERGENCY_CONTACT_H
