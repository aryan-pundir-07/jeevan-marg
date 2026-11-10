-- RESQ Dehradun / Ambulance Management Database Schema
-- Defines the 5 core tables matching the Phase-II Architecture

CREATE TABLE IF NOT EXISTS locations (
    location_id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    type VARCHAR(30) NOT NULL,
    address TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS roads (
    road_id INTEGER PRIMARY KEY AUTOINCREMENT,
    from_node_id VARCHAR(20) NOT NULL,
    to_node_id VARCHAR(20) NOT NULL,
    distance_km REAL NOT NULL,
    road_name VARCHAR(100) NOT NULL,
    road_type VARCHAR(30) NOT NULL,
    FOREIGN KEY(from_node_id) REFERENCES locations(location_id),
    FOREIGN KEY(to_node_id) REFERENCES locations(location_id)
);

CREATE TABLE IF NOT EXISTS hospitals (
    hospital_id VARCHAR(20) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    address TEXT NOT NULL,
    phone VARCHAR(30) NOT NULL,
    emergency_service VARCHAR(50) NOT NULL,
    total_beds INTEGER NOT NULL,
    icu_beds INTEGER NOT NULL,
    available_icu INTEGER NOT NULL,
    oxygen_beds INTEGER NOT NULL,
    specialties TEXT NOT NULL, -- comma-separated list
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    rating REAL DEFAULT 4.5
);

CREATE TABLE IF NOT EXISTS emergencies (
    emergency_id VARCHAR(30) PRIMARY KEY,
    caller_name VARCHAR(100) NOT NULL,
    caller_phone VARCHAR(20) NOT NULL,
    pickup_location VARCHAR(150) NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    severity VARCHAR(20) NOT NULL, -- CRITICAL, HIGH, MEDIUM, LOW
    status VARCHAR(20) NOT NULL,   -- PENDING, ASSIGNED, DISPATCHED, ARRIVED
    assigned_hospital_id VARCHAR(20),
    assigned_ambulance_id VARCHAR(20),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS ambulances (
    ambulance_id VARCHAR(20) PRIMARY KEY,
    unit_code VARCHAR(30) NOT NULL,
    type VARCHAR(20) NOT NULL, -- ALS (Advanced Life Support), BLS (Basic Life Support)
    driver_name VARCHAR(100) NOT NULL,
    driver_phone VARCHAR(20) NOT NULL,
    station_location_id VARCHAR(20) NOT NULL,
    current_lat REAL NOT NULL,
    current_lng REAL NOT NULL,
    status VARCHAR(20) NOT NULL, -- READY, EN_ROUTE, ON_SCENE, RETURNING, MAINTENANCE
    assigned_emergency_id VARCHAR(30)
);
