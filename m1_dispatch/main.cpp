#include "model/EmergencyRequest.h"
#include "service/EmergencyManager.h"
#include "service/TriageRuleEngine.h"
#include <iostream>

int main() {
    std::cout << "\n========================================================================\n";
    std::cout << "  JEEVAN MARG -- MODULE 1: EMERGENCY DISPATCH & TRIAGE MANAGER\n";
    std::cout << "  Lead: Rajat Saxena (2510011033) | Graphic Era University\n";
    std::cout << "========================================================================\n\n";

    EmergencyManager manager;

    std::cout << "[Step 1] Incoming Emergency Call #1:\n";
    std::string id1 = manager.createEmergency("Pooja Rawat", "+91 94120 12345",
                                              "Clock Tower Market", 30.3244, 78.0418,
                                              "Severe chest pain and collapse");
    auto req1 = manager.getEmergency(id1);
    std::cout << "  Created Emergency: " << id1 << " | Severity: " << emergencySeverityToString(req1->getSeverity())
              << " | Status: " << emergencyStatusToString(req1->getStatus()) << "\n";

    std::cout << "\n[Step 2] Assigning ALS Ambulance Unit #DHN-108-01:\n";
    manager.assignAmbulance(id1, "AMB-01");
    std::cout << "  Status updated: " << emergencyStatusToString(manager.getEmergency(id1)->getStatus()) << "\n";

    std::cout << "\n[Step 3] Dispatching to Doon Government Hospital (LOC-011):\n";
    manager.assignHospital(id1, "LOC-011");
    manager.advanceStatus(id1);
    std::cout << "  Status updated: " << emergencyStatusToString(manager.getEmergency(id1)->getStatus()) << "\n";

    std::cout << "\n[Step 4] Ambulance Arrived on Scene:\n";
    manager.advanceStatus(id1);
    std::cout << "  Status updated: " << emergencyStatusToString(manager.getEmergency(id1)->getStatus()) << "\n";

    std::cout << "\nEmergency Record JSON:\n" << manager.getEmergency(id1)->toJson() << "\n\n";
    return 0;
}
