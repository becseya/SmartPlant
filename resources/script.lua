
function setValue(appeui, deveui, name, value)
    worked, err = resiot_setnodevalue(appeui, deveui, name, value)

    if (not worked) then
        resiot_debug(string.format("Set Value Error %s \n", err))
    else
        resiot_debug(string.format("Tag %s set to %s \n", name, value))
    end
end

function getAny(param, func, size)
    local retVal = func(resiot_baslice(param.bytes, param.position, param.position + size))
    param.position = param.position + size
    return retVal
end

function getInt16(param)
    return getAny(param, resiot_ba2sintBE16, 2)
end

function getUInt16(param)
    return getAny(param, resiot_ba2intBE16, 2)
end

function getFloat(param)
    return getAny(param, resiot_ba2float32BE, 4)
end

function getChar(param)
    return getAny(param, resiot_ba2str, 1)
end

function parsePayload(appeui, deveui, payload)
    bytes, err = resiot_hexdecode(payload)
    p = {bytes=bytes, position=0}

    -- Make sure format matches main.cpp

    setValue(appeui, deveui, "AccX", getInt16(p) / 1000.0)        -- int16_t  - Accelerometer - x
    setValue(appeui, deveui, "AccY", getInt16(p) / 1000.0)        -- int16_t  - Accelerometer - y
    setValue(appeui, deveui, "AccZ", getInt16(p) / 1000.0)        -- int16_t  - Accelerometer - z
    setValue(appeui, deveui, "AccFlips", getUInt16(p))            -- uint16_t - Accelerometer - flips
    setValue(appeui, deveui, "AccTaps", getUInt16(p))             -- uint16_t - Accelerometer - taps
    setValue(appeui, deveui, "Brightness", getInt16(p) / 100.0)   -- int16_t  - Brightness
    setValue(appeui, deveui, "SoilMoisture", getInt16(p) / 100.0) -- int16_t  - Soil moisture
    lat = getFloat(p)                                             -- float    - Gps - lat
    lon = getFloat(p)                                             -- float    - Gps - lon
    setValue(appeui, deveui, "Temperature", getInt16(p) / 100.0)  -- int16_t  - Temp
    setValue(appeui, deveui, "Humidity", getInt16(p) / 100.0)     -- int16_t  - Hum
    colotChar = getChar(p)                                        -- char     - Color
    modeChar = getChar(p)                                         -- char     - Mode

    if not (lat == 0 and lon == 0) then
        setValue(appeui, deveui, "Latitude", lat)
        setValue(appeui, deveui, "Longitude", lon)
        setValue(appeui, deveui, "GpsStatus", "Valid")
    else
        setValue(appeui, deveui, "GpsStatus", "No signal")
    end

      if colotChar == "R" then
        setValue(appeui, deveui, "Color", "Red")
    elseif colotChar == "G" then
        setValue(appeui, deveui, "Color", "Green")
    elseif colotChar == "B" then
        setValue(appeui, deveui, "Color", "Blue")
    elseif colotChar == "C" then
        setValue(appeui, deveui, "Color", "Clear")
    end

    if modeChar == "T" then
        setValue(appeui, deveui, "Mode", "Test")
    elseif modeChar == "N" then
        setValue(appeui, deveui, "Mode", "Normal")
    end
end

--Scene process starts here
Origin = resiot_startfrom()

if Origin == "Manual" then
    payload = "fff603a3000100230011001700434221a7eec06d5133086c22745254"
    appeui = "70b3d57ed000fcda"
    deveui = "8639323559379194"
else
    appeui = resiot_comm_getparam("appeui")
    deveui = resiot_comm_getparam("deveui")
    payload, err = resiot_getlastpayload(appeui, deveui)
end

parsePayload(appeui, deveui, payload)
