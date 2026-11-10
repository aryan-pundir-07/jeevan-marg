#ifndef M1_MODEL_EMERGENCY_REQUEST_H
#define M1_MODEL_EMERGENCY_REQUEST_H

#include "EmergencySeverity.h"
#include "EmergencyContact.h"
#include <string>
#include <vector>

enum class EmergencyStatus {
    PENDING,
    ASSIGNED,
    DISPATCHED,
    ARRIVED
};

std::string emergencyStatusToString(EmergencyStatus status);
EmergencyStatus stringToEmergencyStatus(const std::string& str);

class EmergencyRequest {
private:
    std::string emergencyId;
    std::string callerName;
    std::string callerPhone;
    std::string locationDescription;
    double latitude;
    double longitude;
    EmergencySeverity severity;
    EmergencyStatus status;
    std::string assignedHospitalId;
    std::string assignedAmbulanceId;
    std::vector<EmergencyContact> contacts;
    std::string createdAt;

public:
    EmergencyRequest();
    EmergencyRequest(const std::string& id,
                     const std::string& caller,
                     const std::string& phone,
                     const std::string& locDesc,
                     double lat,
                     double lng,
                     EmergencySeverity sev);

    const std::string& getId() const { return emergencyId; }
    const std::string& getCallerName() const { return callerName; }
    const std::string& getCallerPhone() const { return callerPhone; }
    const std::string& getLocationDescription() const { return locationDescription; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
    EmergencySeverity getSeverity() const { return severity; }
    EmergencyStatus getStatus() const { return status; }
    const std::string& getAssignedHospitalId() const { return assignedHospitalId; }
    const std::string& getAssignedAmbulanceId() const { return assignedAmbulanceId; }
    const std::vector<EmergencyContact>& getContacts() const { return contacts; }

    void setStatus(EmergencyStatus s) { status = s; }
    void setAssignedHospital(const std::string& hospId) { assignedHospitalId = hospId; }
    void setAssignedAmbulance(const std::string& ambId) { assignedAmbulanceId = ambId; }
    void addContact(const EmergencyContact& contact) { contacts.push_back(contact); }

    std::string toJson() const;
};

#endif // M1_MODEL_EMERGENCY_REQUEST_H
