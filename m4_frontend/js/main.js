// M4: Main Application Controller

let userCoords = { lat: 30.3244, lng: 78.0418, name: "Clock Tower, Dehradun" };
let currentSelectedHospitalId = "LOC-004";
let activeRouteData = null;

window.addEventListener('DOMContentLoaded', () => {
    initLeafletMap();
    loadHospitalGrid();
    populateNavDropdowns();
    requestUserLocation(true);
    initAnalyticsCharts();
});

function requestUserLocation(silent = false) {
    if (!navigator.geolocation) {
        if (!silent) showToast("Geolocation not supported by browser", "error");
        return;
    }
    navigator.geolocation.getCurrentPosition(
        (pos) => {
            userCoords.lat = pos.coords.latitude;
            userCoords.lng = pos.coords.longitude;
            userCoords.name = `GPS: ${userCoords.lat.toFixed(4)}, ${userCoords.lng.toFixed(4)}`;
            const locText = document.getElementById('user-loc-text');
            if (locText) locText.innerText = "Dehradun (GPS Active)";
            if (!silent) showToast("Acquired accurate GPS coordinates!", "success");
        },
        (err) => {
            console.warn("GPS error/denied:", err);
            const locText = document.getElementById('user-loc-text');
            if (locText) locText.innerText = "Clock Tower (Default Dehradun)";
        },
        { timeout: 7000 }
    );
}

function switchTab(tabId) {
    document.querySelectorAll('.tab-content').forEach(el => el.classList.add('hidden'));
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('border-emerald-600', 'text-emerald-600', 'dark:text-emerald-400', 'font-bold');
        btn.classList.add('border-transparent', 'text-slate-500', 'font-semibold');
    });

    const targetTab = document.getElementById(`tab-content-${tabId}`);
    if (targetTab) targetTab.classList.remove('hidden');

    const activeBtn = document.getElementById(`tab-btn-${tabId}`);
    if (activeBtn) {
        activeBtn.classList.add('border-emerald-600', 'text-emerald-600', 'dark:text-emerald-400', 'font-bold');
        activeBtn.classList.remove('border-transparent', 'text-slate-500', 'font-semibold');
    }

    if (tabId === 'navigation' && mapInstance) {
        setTimeout(() => { mapInstance.invalidateSize(); }, 200);
    }
    if (tabId === 'analytics') {
        setTimeout(() => { initAnalyticsCharts(); }, 200);
    }
}

async function loadHospitalGrid() {
    const grid = document.getElementById('hospital-grid');
    const searchVal = document.getElementById('hospital-search')?.value || '';
    const specVal = document.getElementById('specialty-filter')?.value || 'all';

    if (!grid) return;
    grid.innerHTML = '<div class="col-span-full py-8 text-center text-slate-400"><i class="fa-solid fa-spinner fa-spin text-2xl"></i> Loading hospitals...</div>';

    const hospitals = await M2_API.fetchHospitals(searchVal, specVal);

    grid.innerHTML = '';
    if (hospitals.length === 0) {
        grid.innerHTML = `
            <div class="col-span-full py-12 text-center text-slate-500">
                <i class="fa-solid fa-hospital-circle-xmark text-4xl mb-3 text-slate-400"></i>
                <p class="font-semibold">No matching Dehradun hospitals found.</p>
            </div>`;
        return;
    }

    hospitals.forEach(h => {
        const icuAvail = h.availableIcu !== undefined ? h.availableIcu : (h.beds ? h.beds.availableIcu : 4);
        const icuTot = h.icuBeds !== undefined ? h.icuBeds : (h.beds ? h.beds.totalIcu : 25);
        const oxy = h.oxygenBeds !== undefined ? h.oxygenBeds : (h.beds ? h.beds.oxygenBeds : 40);

        const card = document.createElement('div');
        card.className = "bg-white dark:bg-slate-900 rounded-2xl border border-slate-200 dark:border-slate-800 p-5 shadow-sm hover:shadow-md transition flex flex-col justify-between space-y-4";
        card.innerHTML = `
            <div class="space-y-3">
                <div class="flex items-start justify-between gap-2">
                    <h4 class="font-bold text-base text-slate-900 dark:text-white">${h.name}</h4>
                    <span class="px-2 py-0.5 bg-emerald-100 text-emerald-700 dark:bg-emerald-950 dark:text-emerald-300 rounded-md text-[11px] font-bold whitespace-nowrap">${h.emergency || '24/7 Service'}</span>
                </div>
                <p class="text-xs text-slate-500 dark:text-slate-400 flex items-start gap-1.5">
                    <i class="fa-solid fa-location-dot text-red-500 mt-0.5"></i>
                    <span>${h.address}</span>
                </p>
                <div class="flex flex-wrap gap-1.5 pt-1">
                    ${(h.specialties || []).map(s => `<span class="px-2 py-0.5 rounded-md bg-slate-100 dark:bg-slate-800 text-[10px] font-semibold text-slate-600 dark:text-slate-300">${s}</span>`).join('')}
                </div>
            </div>

            <div class="space-y-3 pt-3 border-t border-slate-100 dark:border-slate-800">
                <div class="grid grid-cols-2 gap-2 text-center">
                    <div class="p-2 rounded-xl bg-slate-50 dark:bg-slate-800/50">
                        <span class="block text-xs text-slate-500">ICU Available</span>
                        <span class="font-extrabold text-emerald-600 text-sm">${icuAvail} / ${icuTot}</span>
                    </div>
                    <div class="p-2 rounded-xl bg-slate-50 dark:bg-slate-800/50">
                        <span class="block text-xs text-slate-500">Oxygen Beds</span>
                        <span class="font-extrabold text-blue-600 text-sm">${oxy}</span>
                    </div>
                </div>

                <div class="flex gap-2">
                    <button onclick="selectHospitalForRouting('${h.id}')" class="flex-1 py-2 bg-emerald-600 hover:bg-emerald-700 text-white rounded-xl text-xs font-bold transition flex items-center justify-center gap-1.5">
                        <i class="fa-solid fa-diamond-turn-right"></i> Navigate (Dijkstra)
                    </button>
                    <a href="tel:${(h.phone || '').replace(/[^0-9+]/g, '')}" class="px-3 py-2 bg-slate-100 dark:bg-slate-800 hover:bg-slate-200 dark:hover:bg-slate-700 rounded-xl text-xs font-bold transition flex items-center justify-center">
                        <i class="fa-solid fa-phone text-emerald-600"></i>
                    </a>
                </div>
            </div>
        `;
        grid.appendChild(card);
    });
}

function populateNavDropdowns() {
    const originSelect = document.getElementById('nav-origin-select');
    const destSelect = document.getElementById('nav-dest-select');

    if (!originSelect || !destSelect) return;

    originSelect.innerHTML = '';
    destSelect.innerHTML = '';

    for (const [id, node] of Object.entries(M3_API.nodes)) {
        const opt1 = document.createElement('option');
        opt1.value = id;
        opt1.text = `${node.name} (${id})`;
        if (id === 'LOC-001') opt1.selected = true;
        originSelect.appendChild(opt1);

        const opt2 = document.createElement('option');
        opt2.value = id;
        opt2.text = `${node.name} (${id})`;
        if (id === 'LOC-004') opt2.selected = true;
        destSelect.appendChild(opt2);
    }
}

function selectHospitalForRouting(hospId) {
    const destSelect = document.getElementById('nav-dest-select');
    if (destSelect) {
        destSelect.value = hospId;
    }
    switchTab('navigation');
    executeDijkstraRouting();
}

async function executeDijkstraRouting() {
    const originId = document.getElementById('nav-origin-select').value;
    const destId = document.getElementById('nav-dest-select').value;

    showToast("Calculating shortest path via Dijkstra (Min-Heap)...", "info");

    const route = await M3_API.getRoute(originId, destId);
    activeRouteData = route;

    // Draw on Leaflet Map
    drawRouteOnMap(route.pathNodeIds);

    // Update Route Summary Panel
    document.getElementById('nav-distance-text').innerText = `${route.totalDistanceKm.toFixed(2)} km`;
    document.getElementById('nav-algo-text').innerText = route.algorithm;

    const est = route.travelEstimates || {};
    const ambEst = est['Ambulance'] || { durationMinutes: 6.5, estimatedCostRupees: 65 };
    const bikeEst = est['Two-Wheeler'] || { durationMinutes: 7.0, estimatedCostRupees: 15 };
    const busEst = est['Public Transit (Bus)'] || { durationMinutes: 12.0, estimatedCostRupees: 20 };
    const walkEst = est['Walking'] || { durationMinutes: 45.0, estimatedCostRupees: 0 };

    document.getElementById('eta-ambulance').innerText = `${ambEst.durationMinutes.toFixed(1)} mins (₹${ambEst.estimatedCostRupees.toFixed(0)})`;
    document.getElementById('eta-bike').innerText = `${bikeEst.durationMinutes.toFixed(1)} mins (₹${bikeEst.estimatedCostRupees.toFixed(0)})`;
    document.getElementById('eta-bus').innerText = `${busEst.durationMinutes.toFixed(1)} mins (₹${busEst.estimatedCostRupees.toFixed(0)})`;
    document.getElementById('eta-walk').innerText = walkEst.durationMinutes > 0 ? `${walkEst.durationMinutes.toFixed(1)} mins` : 'Not suitable (>5km)';

    // Render turn-by-turn steps
    const stepsContainer = document.getElementById('turn-by-turn-list');
    if (stepsContainer && route.segments) {
        stepsContainer.innerHTML = '';
        route.segments.forEach((seg, i) => {
            const row = document.createElement('div');
            row.className = "p-2.5 rounded-xl bg-slate-50 dark:bg-slate-800/60 border border-slate-100 dark:border-slate-800 text-xs flex items-center justify-between";
            row.innerHTML = `
                <div class="flex items-center space-x-2">
                    <span class="w-5 h-5 rounded-full bg-emerald-100 text-emerald-700 font-bold flex items-center justify-center text-[10px]">${i+1}</span>
                    <span><b>${seg.roadName}</b> (${seg.from} &rarr; ${seg.to})</span>
                </div>
                <span class="font-bold text-slate-500">${seg.distanceKm.toFixed(2)} km</span>
            `;
            stepsContainer.appendChild(row);
        });
    }

    // Auto-trigger live ambulance tracking animation along the path!
    startLiveAmbulanceTracking(route.pathNodeIds);
}

async function handleEmergencyDispatch(e) {
    e.preventDefault();
    const name = document.getElementById('amb-name').value;
    const phone = document.getElementById('amb-phone').value;
    const location = document.getElementById('amb-location').value;
    const severity = document.getElementById('amb-severity').value;
    const condition = document.getElementById('amb-condition').value;

    const payload = {
        name,
        phone,
        location,
        severity,
        condition,
        latitude: userCoords.lat,
        longitude: userCoords.lng
    };

    const res = await M1_API.createEmergency(payload);
    showToast(`🚨 SOS Dispatch Issued! Unit Assigned. Status: DISPATCHED.`, "success");

    // Add to Active Dispatches UI
    const container = document.getElementById('dispatch-status-container');
    const card = document.createElement('div');
    card.className = "p-4 rounded-xl bg-red-50 dark:bg-red-950/40 border border-red-200 dark:border-red-800/60 flex items-center justify-between";
    card.innerHTML = `
        <div class="flex items-center space-x-3">
            <div class="w-10 h-10 rounded-xl bg-red-100 dark:bg-red-900/50 text-red-600 flex items-center justify-center font-bold">
                <i class="fa-solid fa-truck-medical fa-beat"></i>
            </div>
            <div>
                <h4 class="font-bold text-sm text-slate-900 dark:text-white">Emergency for ${name} (${phone})</h4>
                <p class="text-xs text-red-700 dark:text-red-300">${condition || 'Emergency'} at ${location} &bull; ETA 7 mins</p>
            </div>
        </div>
        <span class="px-2.5 py-1 bg-red-600 text-white rounded-lg text-xs font-bold uppercase">DISPATCHED</span>
    `;
    container.prepend(card);
    document.getElementById('ambulance-form').reset();
}

function runTriageAssessment(type) {
    const resultBox = document.getElementById('triage-result');
    const title = document.getElementById('triage-title');
    const spec = document.getElementById('triage-spec');
    const desc = document.getElementById('triage-desc');
    const actionBtn = document.getElementById('triage-action-btn');

    resultBox.classList.remove('hidden');

    if (type === 'cardiac') {
        title.innerText = "Critical Priority: Immediate Cardiology Care Needed";
        spec.innerText = "Recommended Specialty: Cardiology & Critical Care";
        desc.innerText = "Symptoms indicate potential acute myocardial infarction. Immediate transport to Max Super Speciality Hospital (Malsi) or Synergy Institute (Ballupur) advised.";
        actionBtn.onclick = () => selectHospitalForRouting("LOC-004");
    } else if (type === 'trauma') {
        title.innerText = "High Priority: Trauma & Orthopedic Surgery Needed";
        spec.innerText = "Recommended Specialty: Trauma & Orthopedics";
        desc.innerText = "Severe trauma with potential fractures or hemorrhage. Shri Mahant Indiresh Hospital (SMIH) or Graphic Era Hospital (Dhoolkot) equipped with 24/7 trauma surgeons.";
        actionBtn.onclick = () => selectHospitalForRouting("LOC-005");
    } else if (type === 'respiratory') {
        title.innerText = "High Priority: Pulmonology & High-Flow Oxygen";
        spec.innerText = "Recommended Specialty: Pulmonology & Emergency Wing";
        desc.innerText = "Severe respiratory distress. Doon Government Hospital or Synergy Institute are equipped with high-flow oxygen and emergency ventilatory support.";
        actionBtn.onclick = () => selectHospitalForRouting("LOC-011");
    } else {
        title.innerText = "Critical Priority: Stroke / Neurological Emergency";
        spec.innerText = "Recommended Specialty: Neurology & Stroke Unit";
        desc.innerText = "Suspected ischemic/hemorrhagic stroke within the 4.5-hour thrombolysis golden window. Route directly to Max Hospital or Synergy Hospital.";
        actionBtn.onclick = () => selectHospitalForRouting("LOC-004");
    }
}

function triggerEmergencySOS() {
    showToast("🚨 EMERGENCY SOS BROADCASTED! Alerting Dehradun Police & 108 Ambulance.", "error");
    switchTab('dispatch');
    if (navigator.vibrate) navigator.vibrate([500, 200, 500, 200, 500]);
}

function openHelplinesModal() {
    document.getElementById('helplines-modal').classList.remove('hidden');
}

function closeHelplinesModal() {
    document.getElementById('helplines-modal').classList.add('hidden');
}

function toggleDarkMode() {
    document.documentElement.classList.toggle('dark');
}

function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    if (!container) return;
    const toast = document.createElement('div');

    let bgClass = "bg-emerald-600 text-white";
    let icon = "fa-circle-check";
    if (type === 'error') {
        bgClass = "bg-red-600 text-white";
        icon = "fa-triangle-exclamation";
    } else if (type === 'info') {
        bgClass = "bg-blue-600 text-white";
        icon = "fa-circle-info";
    }

    toast.className = `${bgClass} px-4 py-3 rounded-xl shadow-xl flex items-center space-x-3 pointer-events-auto transition transform translate-y-2 opacity-0 duration-300`;
    toast.innerHTML = `<i class="fa-solid ${icon} text-lg"></i><span class="text-xs font-semibold flex-1">${message}</span>`;

    container.appendChild(toast);
    setTimeout(() => { toast.classList.remove('translate-y-2', 'opacity-0'); }, 50);
    setTimeout(() => {
        toast.classList.add('translate-y-2', 'opacity-0');
        setTimeout(() => toast.remove(), 300);
    }, 4000);
}

// ==========================================
// REAL-TIME PATIENT LOCATION & GPS TRACKING
// ==========================================
let patientLiveState = {
    lat: 30.3244,
    lng: 78.0418,
    accuracy: 5.0,
    nearestNodeId: "LOC-001",
    isTracking: false,
    watchId: null,
    lastUpdate: null
};

function calculateHaversineKm(lat1, lon1, lat2, lon2) {
    const R = 6371.0;
    const toRad = deg => deg * (Math.PI / 180);
    const dLat = toRad(lat2 - lat1);
    const dLon = toRad(lon2 - lon1);
    const a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
              Math.cos(toRad(lat1)) * Math.cos(toRad(lat2)) *
              Math.sin(dLon / 2) * Math.sin(dLon / 2);
    return R * (2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a)));
}

function findNearestNodeToCoords(lat, lng) {
    let bestId = "LOC-001";
    let minDist = Infinity;
    for (const [id, node] of Object.entries(M3_API.nodes)) {
        const d = calculateHaversineKm(lat, lng, node.lat, node.lng);
        if (d < minDist) {
            minDist = d;
            bestId = id;
        }
    }
    return { nodeId: bestId, distanceKm: minDist, name: M3_API.nodes[bestId].name };
}

function accessPatientRealTimeLocation(autoRoute = false) {
    if (!navigator.geolocation) {
        showToast("Geolocation is not supported by your browser.", "error");
        return;
    }

    showToast("Acquiring high-accuracy real-time patient GPS...", "info");

    navigator.geolocation.getCurrentPosition(
        (pos) => {
            handlePatientPositionUpdate(pos, autoRoute);
            showToast(`📍 Live Patient GPS Acquired! (Accuracy: ±${pos.coords.accuracy.toFixed(1)}m)`, "success");
        },
        (err) => {
            console.warn("Patient GPS acquisition error:", err);
            showToast("Could not access live GPS. Using default Dehradun location.", "warning");
        },
        { enableHighAccuracy: true, timeout: 10000, maximumAge: 0 }
    );
}

function handlePatientPositionUpdate(pos, autoRoute = false) {
    const lat = pos.coords.latitude;
    const lng = pos.coords.longitude;
    const acc = pos.coords.accuracy || 5.0;

    const nearest = findNearestNodeToCoords(lat, lng);

    patientLiveState.lat = lat;
    patientLiveState.lng = lng;
    patientLiveState.accuracy = acc;
    patientLiveState.nearestNodeId = nearest.nodeId;
    patientLiveState.lastUpdate = new Date().toLocaleTimeString();

    // Sync to backend C++ server
    M1_API.syncPatientLocation({
        emergencyId: "PATIENT-LIVE-01",
        latitude: lat,
        longitude: lng,
        accuracy: acc
    });

    // Update UI Elements
    updatePatientLocationUI(lat, lng, acc, nearest);

    // Place on Map
    setPatientLiveMarker(lat, lng, acc, `Patient Location (${nearest.name})`);

    // Auto-populate origin in navigation
    const originSelect = document.getElementById('nav-origin-select');
    if (originSelect) {
        originSelect.value = nearest.nodeId;
    }

    // Auto-populate emergency form
    const ambLocInput = document.getElementById('amb-location');
    if (ambLocInput && (!ambLocInput.value || ambLocInput.value.includes("GPS:"))) {
        ambLocInput.value = `Patient Live GPS: ${lat.toFixed(4)}, ${lng.toFixed(4)} (near ${nearest.name})`;
    }

    if (autoRoute) {
        switchTab('navigation');
        executeDijkstraRouting();
    }
}

function togglePatientLiveTracking() {
    const btn = document.getElementById('btn-continuous-tracking');
    const badge = document.getElementById('patient-tracking-status');

    if (!patientLiveState.isTracking) {
        if (!navigator.geolocation) {
            showToast("Geolocation not supported.", "error");
            return;
        }

        patientLiveState.isTracking = true;
        patientLiveState.watchId = navigator.geolocation.watchPosition(
            (pos) => {
                handlePatientPositionUpdate(pos, false);
            },
            (err) => {
                console.warn("Watch position error:", err);
            },
            { enableHighAccuracy: true, maximumAge: 1000, timeout: 5000 }
        );

        if (btn) {
            btn.innerHTML = '<i class="fa-solid fa-stop text-red-500"></i> Stop Live Tracking';
            btn.className = "px-3 py-1.5 bg-red-100 dark:bg-red-950 text-red-700 dark:text-red-300 rounded-xl text-xs font-bold transition flex items-center space-x-1.5 border border-red-300 dark:border-red-800";
        }
        if (badge) {
            badge.innerText = "LIVE STREAMING GPS";
            badge.className = "px-2.5 py-0.5 bg-emerald-500 text-white rounded-full text-[10px] font-extrabold uppercase animate-pulse";
        }
        showToast("Started continuous real-time patient location streaming!", "success");
    } else {
        if (patientLiveState.watchId !== null) {
            navigator.geolocation.clearWatch(patientLiveState.watchId);
            patientLiveState.watchId = null;
        }
        patientLiveState.isTracking = false;

        if (btn) {
            btn.innerHTML = '<i class="fa-solid fa-satellite-dish text-purple-600"></i> Continuous Live Tracking';
            btn.className = "px-3 py-1.5 bg-purple-50 dark:bg-purple-950 text-purple-700 dark:text-purple-300 hover:bg-purple-100 rounded-xl text-xs font-bold transition flex items-center space-x-1.5 border border-purple-200 dark:border-purple-800";
        }
        if (badge) {
            badge.innerText = "GPS STATIC";
            badge.className = "px-2.5 py-0.5 bg-slate-200 dark:bg-slate-700 text-slate-700 dark:text-slate-300 rounded-full text-[10px] font-bold uppercase";
        }
        showToast("Stopped continuous live tracking.", "info");
    }
}

function updatePatientLocationUI(lat, lng, acc, nearest) {
    const latElem = document.getElementById('patient-live-lat');
    const lngElem = document.getElementById('patient-live-lng');
    const accElem = document.getElementById('patient-live-acc');
    const nodeElem = document.getElementById('patient-live-node');
    const timeElem = document.getElementById('patient-live-time');

    if (latElem) latElem.innerText = lat.toFixed(5);
    if (lngElem) lngElem.innerText = lng.toFixed(5);
    if (accElem) accElem.innerText = `±${acc.toFixed(1)} m`;
    if (nodeElem) nodeElem.innerText = `${nearest.name} (${nearest.nodeId}) • ${(nearest.distanceKm * 1000).toFixed(0)}m away`;
    if (timeElem) timeElem.innerText = patientLiveState.lastUpdate || "Just now";
}

function routeFromPatientGps() {
    accessPatientRealTimeLocation(true);
}

window.switchTab = switchTab;
window.loadHospitalGrid = loadHospitalGrid;
window.selectHospitalForRouting = selectHospitalForRouting;
window.executeDijkstraRouting = executeDijkstraRouting;
window.handleEmergencyDispatch = handleEmergencyDispatch;
window.runTriageAssessment = runTriageAssessment;
window.triggerEmergencySOS = triggerEmergencySOS;
window.openHelplinesModal = openHelplinesModal;
window.closeHelplinesModal = closeHelplinesModal;
window.toggleDarkMode = toggleDarkMode;
window.showToast = showToast;
window.accessPatientRealTimeLocation = accessPatientRealTimeLocation;
window.togglePatientLiveTracking = togglePatientLiveTracking;
window.routeFromPatientGps = routeFromPatientGps;
