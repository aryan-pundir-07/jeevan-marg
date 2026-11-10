// M1: Emergency Dispatch API Client (Rajat Saxena)

// Auto-detect: use local backend when running on localhost, else demo/offline mode on Netlify
const IS_LOCAL = window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1';
const API_BASE = IS_LOCAL ? 'http://localhost:8080/api' : null; // null = offline demo mode

const M1_API = {
    baseUrl: API_BASE,

    async createEmergency(data) {
        try {
            const res = await fetch(`${this.baseUrl}/emergency-requests`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            });
            if (res.ok) {
                return await res.json();
            }
        } catch (e) {
            console.warn('[M1] Server offline or unreachable, using local fallback:', e);
        }
        // Fallback simulation
        return {
            success: true,
            status: "OK",
            data: {
                emergencyId: "SOS-" + Math.floor(1000 + Math.random() * 9000),
                status: "DISPATCHED",
                assignedAmbulance: "AMB-01",
                etaMinutes: 7.5
            }
        };
    },

    async getEmergencies() {
        try {
            const res = await fetch(`${this.baseUrl}/emergency-requests`);
            if (res.ok) {
                const json = await res.json();
                return json.data || [];
            }
        } catch (e) {
            console.warn('[M1] Fetch emergencies fallback:', e);
        }
        return [];
    },

    async updateStatus(id, newStatus) {
        try {
            const res = await fetch(`${this.baseUrl}/emergencies/${id}/status`, {
                method: 'PATCH',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ status: newStatus })
            });
            return res.ok;
        } catch (e) {
            console.warn('[M1] Update status fallback:', e);
            return true;
        }
    },

    async syncPatientLocation(patientData) {
        try {
            const res = await fetch(`${this.baseUrl}/patient-location`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(patientData)
            });
            if (res.ok) {
                return await res.json();
            }
        } catch (e) {
            console.warn('[M1] Sync patient location fallback:', e);
        }
        return {
            success: true,
            status: "OK",
            data: {
                emergencyId: patientData.emergencyId || "PATIENT-LIVE-01",
                latitude: patientData.latitude,
                longitude: patientData.longitude,
                accuracyMeters: patientData.accuracy || 5.0,
                nearestGraphNodeId: "LOC-001"
            }
        };
    },

    async getPatientLocations() {
        try {
            const res = await fetch(`${this.baseUrl}/patient-location`);
            if (res.ok) {
                const json = await res.json();
                return json.data || [];
            }
        } catch (e) {
            console.warn('[M1] Fetch patient locations fallback:', e);
        }
        return [];
    }
};

window.M1_API = M1_API;
