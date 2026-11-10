#ifndef M1_MODEL_EMERGENCY_SEVERITY_H
#define M1_MODEL_EMERGENCY_SEVERITY_H

#include <string>

enum class EmergencySeverity {
    CRITICAL, // Cardiac arrest, massive trauma, unconsciousness (Immediate dispatch ALS)
    HIGH,     // Heavy bleeding, fractures, breathing difficulty
    MEDIUM,   // Moderate trauma, high fever, stable vitals
    LOW       // Non-life-threatening, outpatient transfer
};

inline std::string emergencySeverityToString(EmergencySeverity severity) {
    switch (severity) {
        case EmergencySeverity::CRITICAL: return "CRITICAL";
        case EmergencySeverity::HIGH: return "HIGH";
        case EmergencySeverity::MEDIUM: return "MEDIUM";
        case EmergencySeverity::LOW: return "LOW";
        default: return "MEDIUM";
    }
}

inline EmergencySeverity stringToEmergencySeverity(const std::string& str) {
    if (str == "CRITICAL") return EmergencySeverity::CRITICAL;
    if (str == "HIGH") return EmergencySeverity::HIGH;
    if (str == "LOW") return EmergencySeverity::LOW;
    return EmergencySeverity::MEDIUM;
}

#endif // M1_MODEL_EMERGENCY_SEVERITY_H
