#include "EmergencyRequest.h"
#include <sstream>
#include <iomanip>

std::string emergencyStatusToString(EmergencyStatus status) {
    switch (status) {
        case EmergencyStatus::PENDING: return "PENDING";
        case EmergencyStatus::ASSIGNED: return "ASSIGNED";
        case EmergencyStatus::DISPATCHED: return "DISPATCHED";
        case EmergencyStatus::ARRIVED: return "ARRIVED";
        default: return "PENDING";
    }
}

EmergencyStatus stringToEmergencyStatus(const std::string& str) {
    if (str == "ASSIGNED") return EmergencyStatus::ASSIGNED;
    if (str == "DISPATCHED") return EmergencyStatus::DISPATCHED;
    if (str == "ARRIVED") return EmergencyStatus::ARRIVED;
    return EmergencyStatus::PENDING;
}

EmergencyRequest::EmergencyRequest()
    : emergencyId(""), callerName(""), callerPhone(""), locationDescription(""),
      latitude(0.0), longitude(0.0), severity(EmergencySeverity::MEDIUM),
      status(EmergencyStatus::PENDING), assignedHospitalId(""), assignedAmbulanceId("") {}

EmergencyRequest::EmergencyRequest(const std::string& id,
                                   const std::string& caller,
                                   const std::string& phone,
                                   const std::string& locDesc,
                                   double lat,
                                   double lng,
                                   EmergencySeverity sev)
    : emergencyId(id), callerName(caller), callerPhone(phone), locationDescription(locDesc),
      latitude(lat), longitude(lng), severity(sev), status(EmergencyStatus::PENDING),
      assignedHospitalId(""), assignedAmbulanceId("") {}

std::string EmergencyRequest::toJson() const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(5);
    ss << "{"
       << "\"id\":\"" << emergencyId << "\","
       << "\"callerName\":\"" << callerName << "\","
       << "\"callerPhone\":\"" << callerPhone << "\","
       << "\"location\":\"" << locationDescription << "\","
       << "\"latitude\":" << latitude << ","
       << "\"longitude\":" << longitude << ","
       << "\"severity\":\"" << emergencySeverityToString(severity) << "\","
       << "\"status\":\"" << emergencyStatusToString(status) << "\","
       << "\"assignedHospital\":\"" << assignedHospitalId << "\","
       << "\"assignedAmbulance\":\"" << assignedAmbulanceId << "\","
       << "\"contacts\":[";
    for (size_t i = 0; i < contacts.size(); ++i) {
        ss << contacts[i].toJson();
        if (i + 1 < contacts.size()) ss << ",";
    }
    ss << "]}";
    return ss.str();
}
