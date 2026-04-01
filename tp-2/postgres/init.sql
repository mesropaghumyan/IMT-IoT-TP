CREATE TABLE IF NOT EXISTS public.actuator_state (
                                                     id BIGSERIAL PRIMARY KEY,
                                                     device_id TEXT,
                                                     ts TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    vmc BOOLEAN,
    heating BOOLEAN,
    lighting BOOLEAN,
    cooling BOOLEAN
    );

CREATE TABLE IF NOT EXISTS public.telemetry (
                                                id BIGSERIAL PRIMARY KEY,
                                                device_id TEXT,
                                                ts TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    temperature REAL,
    humidity REAL,
    lux REAL,
    voc_index REAL
    );