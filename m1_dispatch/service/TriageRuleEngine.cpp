#include "TriageRuleEngine.h"
#include <algorithm>

TriageAssessment TriageRuleEngine::assessCondition(const std::string& symptom) {
    std::string s = symptom;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    TriageAssessment result;

    if (s.find("cardiac") != std::string::npos || s.find("heart") != std::string::npos ||
        s.find("chest pain") != std::string::npos || s.find("arrest") != std::string::npos) {
        result.severity = EmergencySeverity::CRITICAL;
        result.recommendedSpecialty = "Cardiology & Critical Care";
        result.requiredAmbulanceType = "ALS (Advanced Life Support)";
        result.clinicalProtocol = "Immediate AED, ECG telemetry, oxygen administration. Direct to cath-lab equipped hospital.";
        result.urgencyScore = 1;
    }
    else if (s.find("trauma") != std::string::npos || s.find("accident") != std::string::npos ||
             s.find("bleeding") != std::string::npos || s.find("fracture") != std::string::npos) {
        result.severity = EmergencySeverity::HIGH;
        result.recommendedSpecialty = "Trauma & Orthopedics";
        result.requiredAmbulanceType = "ALS (Advanced Life Support)";
        result.clinicalProtocol = "Spine immobilization, pressure hemorrhage control, rapid trauma triage.";
        result.urgencyScore = 2;
    }
    else if (s.find("breath") != std::string::npos || s.find("asthma") != std::string::npos ||
             s.find("respiratory") != std::string::npos) {
        result.severity = EmergencySeverity::HIGH;
        result.recommendedSpecialty = "Pulmonology & Emergency Wing";
        result.requiredAmbulanceType = "BLS with Oxygen";
        result.clinicalProtocol = "High flow nebulization / oxygen cannula, continuous SpO2 saturation monitoring.";
        result.urgencyScore = 2;
    }
    else {
        result.severity = EmergencySeverity::MEDIUM;
        result.recommendedSpecialty = "General Emergency";
        result.requiredAmbulanceType = "BLS (Basic Life Support)";
        result.clinicalProtocol = "Standard vital monitoring and prompt emergency facility transport.";
        result.urgencyScore = 3;
    }

    return result;
}
