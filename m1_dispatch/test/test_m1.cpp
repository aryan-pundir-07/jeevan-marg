#include "../model/EmergencyRequest.h"
#include "../model/EmergencyContact.h"
#include "../service/TriageRuleEngine.h"
#include "../service/EmergencyManager.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>

static int g_passCount = 0;
static int g_totalCount = 0;

#define M1_ASSERT(cond, msg) do { \
    g_totalCount++; \
    if (cond) { \
        g_passCount++; \
    } else { \
        std::cerr << "  [FAIL] Assertion failed: " << msg << " (" << __LINE__ << ")\n"; \
    } \
} while(0)

void testEmergencyModels() {
    std::cout << "[M1 Test] Testing Emergency Models...\n";
    EmergencyRequest req("SOS-TEST", "Aarav Sharma", "+91 98765 43210", "Clock Tower", 30.3244, 78.0418, EmergencySeverity::CRITICAL);

    M1_ASSERT(req.getId() == "SOS-TEST", "ID matches");
    M1_ASSERT(req.getCallerName() == "Aarav Sharma", "Caller name matches");
    M1_ASSERT(req.getStatus() == EmergencyStatus::PENDING, "Default status is PENDING");
    M1_ASSERT(req.getSeverity() == EmergencySeverity::CRITICAL, "Severity is CRITICAL");

    req.setStatus(EmergencyStatus::ASSIGNED);
    M1_ASSERT(req.getStatus() == EmergencyStatus::ASSIGNED, "Status updated to ASSIGNED");

    EmergencyContact contact("Sunita Sharma", "+91 98765 00000", "Mother");
    req.addContact(contact);
    M1_ASSERT(req.getContacts().size() == 1, "Contact added to emergency request");
}

void testTriageRuleEngine() {
    std::cout << "[M1 Test] Testing Triage Rule Engine...\n";

    auto cardiac = TriageRuleEngine::assessCondition("Patient has severe chest pain and left arm numbness");
    M1_ASSERT(cardiac.severity == EmergencySeverity::CRITICAL, "Cardiac event assessed as CRITICAL");
    M1_ASSERT(cardiac.urgencyScore == 1, "Cardiac urgency score is 1 (Highest)");
    M1_ASSERT(cardiac.requiredAmbulanceType.find("ALS") != std::string::npos, "Cardiac requires ALS ambulance");

    auto trauma = TriageRuleEngine::assessCondition("Road accident at Ballupur, heavy bleeding");
    M1_ASSERT(trauma.severity == EmergencySeverity::HIGH, "Trauma event assessed as HIGH");
    M1_ASSERT(trauma.urgencyScore == 2, "Trauma urgency score is 2");

    auto mild = TriageRuleEngine::assessCondition("Minor ankle sprain");
    M1_ASSERT(mild.severity == EmergencySeverity::MEDIUM, "Mild condition assessed as MEDIUM");
}

void testStatusWorkflow() {
    std::cout << "[M1 Test] Testing Status Progression (PENDING -> ASSIGNED -> DISPATCHED -> ARRIVED)...\n";
    EmergencyManager mgr;

    std::string id = mgr.createEmergency("Rohan Verma", "+91 99999 11111", "ISBT Dehradun", 30.2872, 78.0069, "Cardiac arrest");
    auto reqOpt = mgr.getEmergency(id);
    M1_ASSERT(reqOpt.has_value(), "Emergency found in manager");
    M1_ASSERT(reqOpt->getStatus() == EmergencyStatus::PENDING, "Initial status is PENDING");

    // Advance 1: ASSIGNED
    mgr.advanceStatus(id);
    M1_ASSERT(mgr.getEmergency(id)->getStatus() == EmergencyStatus::ASSIGNED, "Status transitioned to ASSIGNED");

    // Advance 2: DISPATCHED
    mgr.advanceStatus(id);
    M1_ASSERT(mgr.getEmergency(id)->getStatus() == EmergencyStatus::DISPATCHED, "Status transitioned to DISPATCHED");

    // Advance 3: ARRIVED
    mgr.advanceStatus(id);
    M1_ASSERT(mgr.getEmergency(id)->getStatus() == EmergencyStatus::ARRIVED, "Status transitioned to ARRIVED");
}

void testConcurrentCreation() {
    std::cout << "[M1 Test] Testing Thread Safety with Concurrent Requests...\n";
    EmergencyManager mgr;
    std::vector<std::thread> workers;

    for (int i = 0; i < 10; ++i) {
        workers.emplace_back([&mgr, i]() {
            mgr.createEmergency("Caller " + std::to_string(i), "+91 98000 0000" + std::to_string(i),
                                "Location " + std::to_string(i), 30.3, 78.0);
        });
    }

    for (auto& t : workers) {
        t.join();
    }

    M1_ASSERT(mgr.count() == 10, "All 10 concurrent emergency requests created without race condition");
}

int main() {
    std::cout << "\n========================================================================\n";
    std::cout << "  MODULE 1 (EMERGENCY DISPATCH) TEST SUITE - RAJAT SAXENA\n";
    std::cout << "========================================================================\n\n";

    testEmergencyModels();
    testTriageRuleEngine();
    testStatusWorkflow();
    testConcurrentCreation();

    std::cout << "\n========================================================================\n";
    std::cout << "  M1 TEST SUMMARY: " << g_passCount << " / " << g_totalCount << " PASSED (";
    if (g_passCount == g_totalCount) {
        std::cout << "100% SUCCESS)\n";
    } else {
        std::cout << "FAILURES OCCURRED)\n";
    }
    std::cout << "========================================================================\n\n";

    return (g_passCount == g_totalCount) ? 0 : 1;
}
