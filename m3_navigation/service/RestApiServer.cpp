#ifndef WINVER
#define WINVER 0x0A00
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include "RestApiServer.h"
#include "../include/httplib.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>
#include <algorithm>

// In-memory data store for emergency dispatches and live ambulances
struct EmergencyRecord {
    std::string id;
    std::string callerName;
    std::string phone;
    std::string location;
    double lat;
    double lng;
    std::string severity;
    std::string status; // PENDING, ASSIGNED, DISPATCHED, ARRIVED
    std::string assignedHospitalId;
    std::string assignedAmbulanceId;
    std::string timestamp;
};

struct AmbulanceUnit {
    std::string id;
    std::string unitCode;
    std::string type; // ALS, BLS
    std::string driver;
    std::string phone;
    double lat;
    double lng;
    std::string status; // READY, DISPATCHED, EN_ROUTE, ON_SCENE
    std::string currentRouteHospitalId;
};

struct PatientLiveLocation {
    std::string emergencyId;
    std::string patientName;
    double lat;
    double lng;
    double accuracyMeters;
    std::string nearestNodeId;
    std::string timestamp;
};

static std::vector<EmergencyRecord> g_emergencies;
static std::vector<AmbulanceUnit> g_ambulances = {
    {"AMB-01", "Unit #DHN-108-01 (ALS)", "ALS", "Vikram Negi", "+91 98970 11001", 30.3244, 78.0418, "READY", ""},
    {"AMB-02", "Unit #DHN-108-02 (BLS)", "BLS", "Sanjay Rawat", "+91 98970 11002", 30.2872, 78.0069, "READY", ""},
    {"AMB-03", "Unit #DHN-108-03 (ALS)", "ALS", "Ramesh Joshi", "+91 98970 11003", 30.3340, 78.0180, "READY", ""},
    {"AMB-04", "Unit #DHN-108-04 (ALS)", "ALS", "Anil Chauhan", "+91 98970 11004", 30.3155, 78.0331, "READY", ""}
};
static std::vector<PatientLiveLocation> g_patientLocations;
static std::mutex g_stateMutex;

static void setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
}

static std::string buildJsonError(const std::string& errCode, const std::string& message) {
    std::ostringstream ss;
    ss << "{"
       << "\"success\":false,"
       << "\"status\":\"ERROR\","
       << "\"errorCode\":\"" << errCode << "\","
       << "\"message\":\"" << message << "\""
       << "}";
    return ss.str();
}

RestApiServer::RestApiServer(
    int p,
    std::shared_ptr<ShortestPathFinder> pf,
    std::shared_ptr<NearbyServicesFinder> nf,
    std::shared_ptr<TransportEstimator> te,
    std::shared_ptr<ILocationRepository> lr,
    std::shared_ptr<RoadGraph> rg,
    const std::string& h)
    : port(p), host(h), pathFinder(pf), nearbyFinder(nf),
      transportEstimator(te), locationRepo(lr), graph(rg),
      running(false) {
    server = std::make_unique<httplib::Server>();
    registerRoutes();
}

RestApiServer::~RestApiServer() {
    stop();
}

void RestApiServer::registerRoutes() {
    // 0. Global OPTIONS handler for CORS preflight
    server->Options(".*", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        res.status = 200;
    });

    // 1. Health check
    server->Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        res.set_content("{\"status\":\"UP\",\"system\":\"RESQ Dehradun Navigation Engine\",\"version\":\"1.0.0\"}", "application/json");
    });

    // 2. GET /nearby?lat=...&lng=...&radius=...&type=...
    server->Get("/nearby", [this](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);

        if (!req.has_param("lat") || !req.has_param("lng")) {
            res.status = 400;
            res.set_content(buildJsonError("VALIDATION_ERROR", "Missing required parameters: lat and lng"), "application/json");
            return;
        }

        try {
            double lat = std::stod(req.get_param_value("lat"));
            double lng = std::stod(req.get_param_value("lng"));
            double radius = req.has_param("radius") ? std::stod(req.get_param_value("radius")) : 5.0;

            LocationType filterType = LocationType::UNKNOWN;
            if (req.has_param("type")) {
                filterType = stringToLocationType(req.get_param_value("type"));
            }

            auto nearbyResult = nearbyFinder->findNearby(lat, lng, radius, filterType);

            if (!nearbyResult.isSuccess()) {
                if (nearbyResult.getErrorCode() == "NO_RESULTS") {
                    res.status = 200; // Returns empty list with 200
                    res.set_content("{\"success\":true,\"status\":\"NO_RESULTS\",\"message\":\"" + nearbyResult.getMessage() + "\",\"data\":[]}", "application/json");
                } else {
                    res.status = 400;
                    res.set_content(buildJsonError(nearbyResult.getErrorCode(), nearbyResult.getMessage()), "application/json");
                }
                return;
            }

            std::ostringstream ss;
            ss << "{\"success\":true,\"status\":\"OK\",\"message\":\"" << nearbyResult.getMessage() << "\",\"data\":"
               << NearbyServicesFinder::resultsToJson(nearbyResult.getData()) << "}";
            res.set_content(ss.str(), "application/json");

        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(buildJsonError("VALIDATION_ERROR", std::string("Invalid parameter format: ") + e.what()), "application/json");
        }
    });

    // 3. GET /route?from=...&to=...&mode=...
    server->Get("/route", [this](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);

        if (!req.has_param("from") || !req.has_param("to")) {
            res.status = 400;
            res.set_content(buildJsonError("VALIDATION_ERROR", "Missing required query parameters: 'from' and 'to'"), "application/json");
            return;
        }

        std::string from = req.get_param_value("from");
        std::string to = req.get_param_value("to");

        // If coordinates provided instead of IDs, resolve to nearest graph nodes
        if (req.has_param("fromLat") && req.has_param("fromLng")) {
            try {
                double fLat = std::stod(req.get_param_value("fromLat"));
                double fLng = std::stod(req.get_param_value("fromLng"));
                from = graph->findNearestNodeId(fLat, fLng);
            } catch (...) {}
        }
        if (req.has_param("toLat") && req.has_param("toLng")) {
            try {
                double tLat = std::stod(req.get_param_value("toLat"));
                double tLng = std::stod(req.get_param_value("toLng"));
                to = graph->findNearestNodeId(tLat, tLng);
            } catch (...) {}
        }

        auto routeResult = pathFinder->findShortestRoute(from, to);

        if (!routeResult.isSuccess()) {
            res.status = (routeResult.getErrorCode() == "NOT_FOUND") ? 404 : 400;
            res.set_content(buildJsonError(routeResult.getErrorCode(), routeResult.getMessage()), "application/json");
            return;
        }

        RouteResponse route = routeResult.getData();
        if (transportEstimator) {
            transportEstimator->estimateAll(route);
        }

        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"message\":\"Shortest path computed successfully\",\"data\":"
           << route.toJson() << "}";
        res.set_content(ss.str(), "application/json");
    });

    // 4. GET /hospitals and /api/hospitals
    auto handleHospitals = [this](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        auto hospitals = locationRepo->findByType(LocationType::HOSPITAL);
        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"count\":" << hospitals.size() << ",\"data\":[";
        for (size_t i = 0; i < hospitals.size(); ++i) {
            ss << hospitals[i].toJson();
            if (i + 1 < hospitals.size()) ss << ",";
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    };
    server->Get("/hospitals", handleHospitals);
    server->Get("/api/hospitals", handleHospitals);

    // 5. GET /api/hospitals/search?query=...&specialty=...
    server->Get("/api/hospitals/search", [this](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::string query = req.has_param("query") ? req.get_param_value("query") : "";
        std::string specialty = req.has_param("specialty") ? req.get_param_value("specialty") : "";

        std::transform(query.begin(), query.end(), query.begin(), ::tolower);
        std::transform(specialty.begin(), specialty.end(), specialty.begin(), ::tolower);

        auto hospitals = locationRepo->findByType(LocationType::HOSPITAL);
        std::vector<Location> matched;

        for (const auto& h : hospitals) {
            std::string nameLower = h.getName();
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            std::string addrLower = h.getAddress();
            std::transform(addrLower.begin(), addrLower.end(), addrLower.begin(), ::tolower);

            bool matchesQuery = query.empty() ||
                                (nameLower.find(query) != std::string::npos) ||
                                (addrLower.find(query) != std::string::npos);

            if (matchesQuery) {
                matched.push_back(h);
            }
        }

        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"count\":" << matched.size() << ",\"data\":[";
        for (size_t i = 0; i < matched.size(); ++i) {
            ss << matched[i].toJson();
            if (i + 1 < matched.size()) ss << ",";
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    });

    // 6. Emergency Dispatch: POST /api/emergency-requests
    server->Post("/api/emergency-requests", [this](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::lock_guard<std::mutex> lock(g_stateMutex);

        std::string body = req.body;
        // Simple JSON extractor for essential fields
        auto extractStr = [&body](const std::string& key) -> std::string {
            size_t pos = body.find("\"" + key + "\"");
            if (pos == std::string::npos) return "";
            size_t colon = body.find(":", pos);
            if (colon == std::string::npos) return "";
            size_t q1 = body.find("\"", colon);
            if (q1 == std::string::npos) return "";
            size_t q2 = body.find("\"", q1 + 1);
            if (q2 == std::string::npos) return "";
            return body.substr(q1 + 1, q2 - q1 - 1);
        };
        auto extractNum = [&body](const std::string& key, double defVal) -> double {
            size_t pos = body.find("\"" + key + "\"");
            if (pos == std::string::npos) return defVal;
            size_t colon = body.find(":", pos);
            if (colon == std::string::npos) return defVal;
            try {
                return std::stod(body.substr(colon + 1));
            } catch (...) { return defVal; }
        };

        std::string name = extractStr("name");
        std::string phone = extractStr("phone");
        std::string location = extractStr("location");
        std::string severity = extractStr("severity");
        if (severity.empty()) severity = "HIGH";
        if (name.empty()) name = "Emergency Caller";
        if (location.empty()) location = "Clock Tower, Dehradun";

        double lat = extractNum("latitude", 30.3244);
        double lng = extractNum("longitude", 78.0418);

        std::string id = "SOS-" + std::to_string(1001 + g_emergencies.size());
        std::string assignedAmb = "AMB-0" + std::to_string(1 + (g_emergencies.size() % 4));

        EmergencyRecord rec;
        rec.id = id;
        rec.callerName = name;
        rec.phone = phone;
        rec.location = location;
        rec.lat = lat;
        rec.lng = lng;
        rec.severity = severity;
        rec.status = "DISPATCHED";
        rec.assignedHospitalId = "LOC-011"; // Doon Hospital default nearest
        rec.assignedAmbulanceId = assignedAmb;
        rec.timestamp = "Just Now";

        g_emergencies.push_back(rec);

        // Update assigned ambulance status
        for (auto& amb : g_ambulances) {
            if (amb.id == assignedAmb) {
                amb.status = "DISPATCHED";
                amb.currentRouteHospitalId = rec.assignedHospitalId;
                break;
            }
        }

        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"message\":\"Emergency request created successfully\","
           << "\"data\":{"
           << "\"emergencyId\":\"" << id << "\","
           << "\"status\":\"DISPATCHED\","
           << "\"assignedAmbulance\":\"" << assignedAmb << "\","
           << "\"etaMinutes\":7.5"
           << "}}";
        res.set_content(ss.str(), "application/json");
    });

    // 7. GET /api/emergency-requests
    server->Get("/api/emergency-requests", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        std::lock_guard<std::mutex> lock(g_stateMutex);
        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"count\":" << g_emergencies.size() << ",\"data\":[";
        for (size_t i = 0; i < g_emergencies.size(); ++i) {
            const auto& e = g_emergencies[i];
            ss << "{"
               << "\"id\":\"" << e.id << "\","
               << "\"name\":\"" << e.callerName << "\","
               << "\"phone\":\"" << e.phone << "\","
               << "\"location\":\"" << e.location << "\","
               << "\"latitude\":" << e.lat << ","
               << "\"longitude\":" << e.lng << ","
               << "\"severity\":\"" << e.severity << "\","
               << "\"status\":\"" << e.status << "\","
               << "\"assignedAmbulanceId\":\"" << e.assignedAmbulanceId << "\""
               << "}";
            if (i + 1 < g_emergencies.size()) ss << ",";
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    });

    // 8. PATCH /api/emergencies/:id/status
    server->Patch(R"(/api/emergencies/([^/]+)/status)", [](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::string emId = req.matches[1];
        std::lock_guard<std::mutex> lock(g_stateMutex);

        std::string newStatus = "ARRIVED";
        if (req.body.find("ASSIGNED") != std::string::npos) newStatus = "ASSIGNED";
        else if (req.body.find("DISPATCHED") != std::string::npos) newStatus = "DISPATCHED";
        else if (req.body.find("ARRIVED") != std::string::npos) newStatus = "ARRIVED";

        bool found = false;
        for (auto& e : g_emergencies) {
            if (e.id == emId) {
                e.status = newStatus;
                found = true;
                break;
            }
        }

        if (found) {
            res.set_content("{\"success\":true,\"message\":\"Status updated to " + newStatus + "\"}", "application/json");
        } else {
            res.status = 404;
            res.set_content(buildJsonError("NOT_FOUND", "Emergency ID not found: " + emId), "application/json");
        }
    });

    // 9. GET /ambulance-location and /api/ambulances
    auto handleAmbulances = [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        std::lock_guard<std::mutex> lock(g_stateMutex);
        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"data\":[";
        for (size_t i = 0; i < g_ambulances.size(); ++i) {
            const auto& a = g_ambulances[i];
            ss << "{"
               << "\"id\":\"" << a.id << "\","
               << "\"unitCode\":\"" << a.unitCode << "\","
               << "\"type\":\"" << a.type << "\","
               << "\"driver\":\"" << a.driver << "\","
               << "\"phone\":\"" << a.phone << "\","
               << "\"latitude\":" << a.lat << ","
               << "\"longitude\":" << a.lng << ","
               << "\"status\":\"" << a.status << "\""
               << "}";
            if (i + 1 < g_ambulances.size()) ss << ",";
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    };
    server->Get("/ambulance-location", handleAmbulances);
    server->Get("/api/ambulances", handleAmbulances);

    // 10. Real-Time Patient Location Tracking: POST /api/patient-location
    server->Post("/api/patient-location", [this](const httplib::Request& req, httplib::Response& res) {
        setCorsHeaders(res);
        std::lock_guard<std::mutex> lock(g_stateMutex);

        std::string body = req.body;
        auto extractStr = [&body](const std::string& key) -> std::string {
            size_t pos = body.find("\"" + key + "\"");
            if (pos == std::string::npos) return "";
            size_t colon = body.find(":", pos);
            if (colon == std::string::npos) return "";
            size_t q1 = body.find("\"", colon);
            if (q1 == std::string::npos) return "";
            size_t q2 = body.find("\"", q1 + 1);
            if (q2 == std::string::npos) return "";
            return body.substr(q1 + 1, q2 - q1 - 1);
        };
        auto extractNum = [&body](const std::string& key, double defVal) -> double {
            size_t pos = body.find("\"" + key + "\"");
            if (pos == std::string::npos) return defVal;
            size_t colon = body.find(":", pos);
            if (colon == std::string::npos) return defVal;
            try {
                return std::stod(body.substr(colon + 1));
            } catch (...) { return defVal; }
        };

        std::string emId = extractStr("emergencyId");
        if (emId.empty()) emId = "PATIENT-LIVE-01";
        std::string name = extractStr("patientName");
        if (name.empty()) name = "Patient (Live GPS)";
        double lat = extractNum("latitude", 30.3244);
        double lng = extractNum("longitude", 78.0418);
        double accuracy = extractNum("accuracy", 5.0);

        std::string nearestNode = graph ? graph->findNearestNodeId(lat, lng) : "LOC-001";

        bool updated = false;
        for (auto& p : g_patientLocations) {
            if (p.emergencyId == emId) {
                p.lat = lat;
                p.lng = lng;
                p.accuracyMeters = accuracy;
                p.nearestNodeId = nearestNode;
                p.timestamp = "Live";
                updated = true;
                break;
            }
        }
        if (!updated) {
            g_patientLocations.push_back({emId, name, lat, lng, accuracy, nearestNode, "Live"});
        }

        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"message\":\"Real-time patient location updated\","
           << "\"data\":{"
           << "\"emergencyId\":\"" << emId << "\","
           << "\"latitude\":" << lat << ","
           << "\"longitude\":" << lng << ","
           << "\"accuracyMeters\":" << accuracy << ","
           << "\"nearestGraphNodeId\":\"" << nearestNode << "\""
           << "}}";
        res.set_content(ss.str(), "application/json");
    });

    // GET /api/patient-location
    server->Get("/api/patient-location", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        std::lock_guard<std::mutex> lock(g_stateMutex);
        std::ostringstream ss;
        ss << "{\"success\":true,\"status\":\"OK\",\"count\":" << g_patientLocations.size() << ",\"data\":[";
        for (size_t i = 0; i < g_patientLocations.size(); ++i) {
            const auto& p = g_patientLocations[i];
            ss << "{"
               << "\"emergencyId\":\"" << p.emergencyId << "\","
               << "\"patientName\":\"" << p.patientName << "\","
               << "\"latitude\":" << p.lat << ","
               << "\"longitude\":" << p.lng << ","
               << "\"accuracyMeters\":" << p.accuracyMeters << ","
               << "\"nearestGraphNodeId\":\"" << p.nearestNodeId << "\""
               << "}";
            if (i + 1 < g_patientLocations.size()) ss << ",";
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    });

    // 11. GET /api/analytics
    server->Get("/api/analytics", [this](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        size_t totalLocations = locationRepo->count();
        size_t totalHospitals = locationRepo->findByType(LocationType::HOSPITAL).size();
        size_t totalEdges = graph ? graph->getEdgeCount() : 0;

        std::ostringstream ss;
        ss << "{"
           << "\"success\":true,"
           << "\"data\":{"
           << "\"totalLocations\":" << totalLocations << ","
           << "\"totalHospitals\":" << totalHospitals << ","
           << "\"totalRoadEdges\":" << totalEdges << ","
           << "\"activeAmbulances\":4,"
           << "\"avgResponseTimeMinutes\":8.4,"
           << "\"totalDispatches\":" << (12 + g_emergencies.size()) << ","
           << "\"systemStatus\":\"ONLINE\""
           << "}}";
        res.set_content(ss.str(), "application/json");
    });

    // 11. Static File Serving for Frontend (m4_frontend)
    // Looks for m4_frontend in current directory or parent directory
    server->set_mount_point("/", "./m4_frontend");
    server->set_mount_point("/", "../m4_frontend");
}

void RestApiServer::start(bool blocking) {
    if (running.load()) return;
    running.store(true);

    std::cout << "\n========================================================" << std::endl;
    std::cout << " [RESQ DEHRADUN] REST API Server Starting..." << std::endl;
    std::cout << " Listening on: http://localhost:" << port << std::endl;
    std::cout << " Endpoints: /nearby , /route , /hospitals , /api/emergency-requests" << std::endl;
    std::cout << " Dashboard: http://localhost:" << port << "/index.html" << std::endl;
    std::cout << "========================================================\n" << std::endl;

    if (blocking) {
        server->listen(host.c_str(), port);
        running.store(false);
    } else {
        serverThread = std::thread([this]() {
            server->listen(host.c_str(), port);
            running.store(false);
        });
    }
}

void RestApiServer::stop() {
    if (running.load()) {
        server->stop();
        if (serverThread.joinable()) {
            serverThread.join();
        }
        running.store(false);
        std::cout << "[RESQ DEHRADUN] REST API Server stopped." << std::endl;
    }
}
