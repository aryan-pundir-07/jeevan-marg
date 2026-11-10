#ifndef M1_SERVICE_TRIAGE_RULE_ENGINE_H
#define M1_SERVICE_TRIAGE_RULE_ENGINE_H

#include "../model/EmergencySeverity.h"
#include <string>

struct TriageAssessment {
    EmergencySeverity severity;
    std::string recommendedSpecialty;
    std::string requiredAmbulanceType; // ALS or BLS
    std::string clinicalProtocol;
    int urgencyScore; // 1 (Highest) to 4 (Lowest)
};

class TriageRuleEngine {
public:
    static TriageAssessment assessCondition(const std::string& symptomOrCondition);
};

#endif // M1_SERVICE_TRIAGE_RULE_ENGINE_H
