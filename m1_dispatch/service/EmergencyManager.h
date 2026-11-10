#ifndef M1_SERVICE_EMERGENCY_MANAGER_H
#define M1_SERVICE_EMERGENCY_MANAGER_H

#include "../model/EmergencyRequest.h"
#include "TriageRuleEngine.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <optional>

class EmergencyManager {
private:
    std::unordered_map<std::string, EmergencyRequest> requests;
    std::vector<std::string> requestOrder;
    std::mutex managerMutex;
    int nextIdCounter;

public:
    EmergencyManager();

    std::string createEmergency(const std::string& caller,
                                const std::string& phone,
                                const std::string& locationDesc,
                                double lat,
                                double lng,
                                const std::string& condition = "General Emergency");

    bool updateStatus(const std::string& id, EmergencyStatus newStatus);
    bool advanceStatus(const std::string& id); // PENDING -> ASSIGNED -> DISPATCHED -> ARRIVED
    bool assignHospital(const std::string& id, const std::string& hospId);
    bool assignAmbulance(const std::string& id, const std::string& ambId);

    std::optional<EmergencyRequest> getEmergency(const std::string& id);
    std::vector<EmergencyRequest> getAllEmergencies();
    std::vector<EmergencyRequest> getByStatus(EmergencyStatus status);
    size_t count() const;
};

#endif // M1_SERVICE_EMERGENCY_MANAGER_H
