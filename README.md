# RESQ DEHRADUN &mdash; Smart Emergency Response &amp; Assistance System
### Project-Based Learning (PBL) &bull; B.Tech Computer Science &amp; Engineering &bull; Graphic Era University (GEU), Dehradun

---

## 1. Project Overview &amp; Team Distribution

| Module | Module Name | Team Member | Student ID | Core Responsibilities |
| :--- | :--- | :--- | :--- | :--- |
| **M1** | **Emergency Dispatch** | Rajat Saxena (Lead) | `2510011033` | Incident reporting, urgency scoring, triage rule engine, status flow (`PENDING` &rarr; `ASSIGNED` &rarr; `DISPATCHED` &rarr; `ARRIVED`) |
| **M2** | **Healthcare Discovery** | Rishi Jain | `2510010332` | Hospital directory, bed &amp; ICU tracking, doctor registry, search &amp; filter by specialty |
| **M3** | **Navigation &amp; Routing Engine** | Anubhav Garg | `2510011405` | Road graph (20 nodes, 31 edges), Min-Heap Dijkstra, Haversine nearby search, multi-modal transport, REST API server |
| **M4** | **Frontend Dashboard** | Aryan Pundir | `2510010816` | Responsive HTML5/Tailwind UI, Leaflet.js interactive map, live ambulance animation, Chart.js analytics |

---

## 2. Deliverables Matrix (Matched with Evaluation Photo Sheet)

| Deliverable | Phase | Hindi Objective ("Kya karna hai?") | Implemented Files &amp; Classes | Status |
| :--- | :--- | :--- | :--- | :--- |
| **Location Model** | Phase 2 | *Locations ka data represent karne ke liye.* | `m3_navigation/model/Location.h`, `Location.cpp` | **VALIDATED** |
| **Road Graph Implementation** | Phase 2 | *Nodes aur edges independently demonstrate karne ke liye.* | `structure/LocationNode.*`, `structure/RoadEdge.*`, `structure/RoadGraph.*`, `service/RoadGraphBuilder.*` | **VALIDATED** |
| **Dijkstra / Shortest-Path** | Phase 2 | *Source se destination tak path calculate karne ke liye.* | `service/ShortestPathFinder.*`, `dto/RouteResponse.*` (Min-Heap $O((V+E)\log V)$) | **VALIDATED** |
| **Nearby-Hospital Search** | Phase 2 | *Agar independently implemented hai, toh demo karo.* | `service/NearbyServicesFinder.*`, `util/GeoUtil.*` (Haversine formula $R=6371\text{ km}$) | **VALIDATED** |
| **Standalone Driver/Demo** | Phase 2 | *Routing ko separately run karne ke liye.* | `main.cpp` (Demos a&ndash;e, interactive CLI &amp; daemon modes) | **VALIDATED** |
| **Unit Tests for Routing** | Phase 2 | *Pathfinding ke individual test cases.* | `test/test_main.cpp` (Dijkstra vs Bellman-Ford on 380 pairs, 100% green pass) | **VALIDATED** |
| **Common API Integration** | Phase 3 | *Routing ko M1/M2 aur dashboard se connect karne ke liye.* | `service/RestApiServer.*`, `include/httplib.h` (Endpoints: `/nearby`, `/route`, `/hospitals`, `/api/emergency-requests`) | **VALIDATED** |
| **End-to-End Navigation Tests** | Phase 3 | *Combined application mein routing verify karne ke liye.* | Full pipeline: CSV &rarr; In-memory Repository &rarr; RoadGraph &rarr; Dijkstra &rarr; REST API &rarr; Leaflet Map &rarr; Ambulance Tracker | **VALIDATED** |

---

## 3. System Architecture &amp; Directory Structure

```
PBL GEU/
├── m1_dispatch/                  # Rajat Saxena's Module (Emergency Dispatch)
│   ├── model/                    # EmergencyRequest, EmergencyContact, EmergencySeverity
│   ├── service/                  # EmergencyManager, TriageRuleEngine
│   ├── test/                     # test_m1.cpp (18 test assertions)
│   ├── main.cpp                  # Standalone CLI driver
│   └── Makefile
│
├── m2_healthcare/                # Rishi Jain's Module (Healthcare Discovery & Hospital Management)
│   ├── model/                    # Hospital, Department, BedAvailability, Doctor
│   ├── dto/                      # HospitalSearchQuery
│   ├── repository/               # HospitalRepository (In-memory hash table, Dehradun hospital seeds)
│   ├── service/                  # HealthcareService (Search by name/specialty, sort by ICU beds)
│   ├── test/                     # test_m2.cpp (18 test assertions)
│   ├── main.cpp                  # Standalone CLI driver
│   └── Makefile
│
├── m3_navigation/                # Anubhav Garg's Module (Navigation & Routing Engine)
│   ├── model/                    # Location, TransportOptions (Polymorphic: Ambulance, Bike, Bus, Walking)
│   ├── structure/                # LocationNode, RoadEdge, RoadGraph (Adjacency List)
│   ├── service/                  # ShortestPathFinder (Dijkstra), NearbyServicesFinder, RestApiServer
│   ├── repository/               # LocationRepository (Thread-safe hash map)
│   ├── datasource/               # CSV Parsers for locations, roads, transport options
│   ├── dto/                      # ApiResponse, RouteResponse, RouteSegment
│   ├── util/                     # GeoUtil (Haversine formula), JsonUtil
│   ├── include/                  # httplib.h (Single-header high performance REST API server)
│   ├── seed/                     # 20 real Dehradun locations & 31 connected roads
│   ├── database/                 # schema.sql (5 relational tables)
│   ├── test/                     # test_main.cpp (49 assertions, 380-pair Bellman-Ford validation)
│   ├── main.cpp                  # Standalone CLI demo & --serve HTTP daemon
│   └── Makefile
│
├── m4_frontend/                  # Aryan Pundir's Module (Interactive Dashboard & Map)
│   ├── index.html                # Responsive dashboard with Tailwind, Leaflet.js, OpenStreetMap
│   ├── css/style.css             # Siren glow, pulse animations, sleek scrollbars
│   └── js/
│       ├── main.js               # Tab management, toast notifications, UI event handling
│       ├── m1-api.js             # Emergency dispatch API client
│       ├── m2-api.js             # Hospital search & ICU bed query client
│       ├── m3-api.js             # Dijkstra route query & ambulance location client
│       ├── map.js                # Leaflet map, hospital markers, route polylines
│       ├── tracking.js           # Live animated ambulance moving along Dijkstra path
│       └── analytics.js          # Chart.js response time trends & bed utilization
│
├── Makefile                      # Root Makefile (make all, make test, make run, make serve)
├── run_server.bat                # 1-Click Windows script: launches C++ REST server & opens dashboard
├── run_demo.bat                  # 1-Click Windows script: runs standalone console demo
├── run_tests.bat                 # 1-Click Windows script: executes all 3 module test suites
└── README.md                     # Comprehensive documentation & Viva guide
```

---

## 4. Algorithmic &amp; Data Structures Complexity Summary

| Module / Operation | Data Structure Used | Algorithm | Time Complexity | Space Complexity |
| :--- | :--- | :--- | :--- | :--- |
| **Road Network** | `std::unordered_map<string, vector<RoadEdge>>` | Adjacency List Graph | $O(1)$ neighbor lookup | $O(V + E)$ |
| **Shortest Route** | `std::priority_queue` (Min-Heap) | Dijkstra's Algorithm | $O((V + E) \log V)$ | $O(V)$ |
| **Route Validation**| Dynamic Programming Table | Bellman-Ford (380 pairs) | $O(V \cdot E)$ | $O(V)$ |
| **Nearby Search** | `std::vector<pair<double, Location>>` | Haversine Formula + QuickSort | $O(N \log N)$ | $O(N)$ |
| **Location Cache** | `std::unordered_map<string, Location>` | Hash Table (Bucketed) | $O(1)$ average lookup | $O(N)$ |
| **Transport Modes**| Polymorphic Class Hierarchy | Strategy Pattern | $O(M)$ | $O(M)$ |

---

## 5. How to Compile, Run and Test

### Option A: Using 1-Click Windows Batch Scripts
- **Start the Full System (Backend REST Server + Frontend UI):**
  Double click `run_server.bat` (Starts server on `http://localhost:8080` and opens dashboard in your browser).
- **Run Standalone Phase 2 Demos:**
  Double click `run_demo.bat`.
- **Run All Automated Test Suites:**
  Double click `run_tests.bat`.

### Option B: Using GNU Make / Terminal
```bash
# 1. Compile all 3 backend modules
make all

# 2. Run all unit tests (M1, M2, M3)
make test

# 3. Run standalone demonstration suite (Phase 2 Demo)
make run

# 4. Run HTTP REST API server on port 8080 (Phase 3 Integration)
make serve
```

---

## 6. Viva Voce Questions &amp; Model Answers for Examination

### Q1: Why did you choose an Adjacency List instead of an Adjacency Matrix for RoadGraph?
> **Answer:** Road networks are sparse graphs where each intersection connects on average to only 3 to 5 roads. An adjacency matrix consumes $O(V^2)$ memory regardless of edge density (wasteful when $|V|$ is large), whereas an adjacency list takes only $O(V + E)$ space and enables $O(\text{deg}(u))$ neighbor traversal during Dijkstra's priority queue exploration.

### Q2: Why did you cross-validate Dijkstra with Bellman-Ford on all 380 pairs?
> **Answer:** Dijkstra is a greedy algorithm utilizing a Min-Heap with $O((V+E)\log V)$ time complexity, whereas Bellman-Ford is a dynamic programming approach with $O(V \cdot E)$ time complexity. By executing both algorithms across all $20 \times 19 = 380$ ordered pairs in our 20-node Dehradun network, we mathematically proved that our Dijkstra implementation produces the optimal shortest path for 100% of routes.

### Q3: What formula is used to find nearby hospitals, and why not simple Euclidean distance?
> **Answer:** We implemented the **Haversine formula** in `util/GeoUtil.cpp` ($R = 6371.0\text{ km}$). Euclidean distance ($\sqrt{\Delta x^2 + \Delta y^2}$) assumes a flat 2D plane. Because the Earth is an oblate spheroid and longitude lines converge toward higher latitudes, Haversine uses spherical great-circle trigonometry to calculate exact surface distances in kilometers across GPS coordinates.

### Q4: How does the system adjust travel time for different road types and transport options?
> **Answer:** We implemented a polymorphic class hierarchy (`TransportOption`) utilizing the Strategy Pattern. Each transport mode (`AmbulanceOption`, `TwoWheelerOption`, `PublicTransitOption`, `WalkingOption`) overrides `getSpeedForRoadType(RoadType)`. For example, ambulances travel at 75 km/h on Highways, 45 km/h on City Streets, and 30 km/h on Hill Roads (such as the Mussoorie Diversion Road). Furthermore, walking is marked as unsuitable if the distance exceeds 5 km.

### Q5: How do the four modules communicate with each other?
> **Answer:** M3 hosts a multi-threaded REST API server (`RestApiServer.cpp`) using `httplib.h` on port 8080 with Cross-Origin Resource Sharing (CORS) enabled. M1 and M2 services are integrated into the REST layer, exposing endpoints `/nearby`, `/route`, `/hospitals`, and `/api/emergency-requests`. The M4 HTML5/JS frontend queries these endpoints asynchronously via the JavaScript `Fetch API`, rendering live routes on a Leaflet.js OpenStreetMap canvas.
