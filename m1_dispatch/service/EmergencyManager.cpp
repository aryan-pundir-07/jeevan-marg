#include "EmergencyManager.h"
#include <iomanip>
#include <sstream>

EmergencyManager::EmergencyManager() : nextIdCounter(1001) {}

std::string EmergencyManager::createEmergency(
    const std::string& caller,
    const std::string& phone,
    const std::string& locationDesc,
    double lat,
    double lng,
    const std::string& condition) {

    std::lock_guard<std::mutex> lock(managerMutex);
    std::string id = "SOS-" + std::to_string(nextIdCounter++);

    TriageAssessment assessment = TriageRuleEngine::assessCondition(condition);
    EmergencyRequest req(id, caller, phone, locationDesc, lat, lng, assessment.severity);

    requests[id] = req;
    requestOrder.push_back(id);
    return id;
}

bool EmergencyManager::updateStatus(const std::string& id, EmergencyStatus newStatus) {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = requests.find(id);
    if (it == requests.end()) return false;
    it->second.setStatus(newStatus);
    return true;
}

bool EmergencyManager::advanceStatus(const std::string& id) {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = requests.find(id);
    if (it == requests.end()) return false;

    EmergencyStatus curr = it->second.getStatus();
    switch (curr) {
        case EmergencyStatus::PENDING:
            it->second.setStatus(EmergencyStatus::ASSIGNED);
            break;
        case EmergencyStatus::ASSIGNED:
            it->second.setStatus(EmergencyStatus::DISPATCHED);
            break;
        case EmergencyStatus::DISPATCHED:
            it->second.setStatus(EmergencyStatus::ARRIVED);
            break;
        case EmergencyStatus::ARRIVED:
            break; // Terminal state
    }
    return true;
}

bool EmergencyManager::assignHospital(const std::string& id, const std::string& hospId) {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = requests.find(id);
    if (it == requests.end()) return false;
    it->second.setAssignedHospital(hospId);
    return true;
}

bool EmergencyManager::assignAmbulance(const std::string& id, const std::string& ambId) {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = requests.find(id);
    if (it == requests.end()) return false;
    it->second.setAssignedAmbulance(ambId);
    if (it->second.getStatus() == EmergencyStatus::PENDING) {
        it->second.setStatus(EmergencyStatus::ASSIGNED);
    }
    return true;
}

std::optional<EmergencyRequest> EmergencyManager::getEmergency(const std::string& id) {
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = requests.find(id);
    if (it != requests.end()) return it->second;
    return std::nullopt;
}

std::vector<EmergencyRequest> EmergencyManager::getAllEmergencies() {
    std::lock_guard<std::mutex> lock(managerMutex);
    std::vector<EmergencyRequest> result;
    for (const auto& id : requestOrder) {
        result.push_back(requests[id]);
    }
    return result;
}

std::vector<EmergencyRequest> EmergencyManager::getByStatus(EmergencyStatus status) {
    std::lock_guard<std::mutex> lock(managerMutex);
    std::vector<EmergencyRequest> result;
    for (const auto& id : requestOrder) {
        if (requests[id].getStatus() == status) {
            result.push_back(requests[id]);
        }
    }
    return result;
}

size_t EmergencyManager::count() const {
    return requests.size();
}
