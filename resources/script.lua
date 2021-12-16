
function setValue(appeui, deveui, name, value)
    worked, err = resiot_setnodevalue(appeui, deveui, name, value)

    if (not worked) then
        resiot_debug(string.format("Set Value Error %s \n", err))
    else
        resiot_debug(string.format("Tag %s set to %s \n", name, value))
    end
end

function parsePayload(appeui, deveui, payload)
    bytes, err = resiot_hexdecode(payload)

    -- Make sure format matches main.cpp
    --
    -- int16_t  - Accelerometer - x
    -- int16_t  - Accelerometer - y
    -- int16_t  - Accelerometer - z
    -- uint16_t - Accelerometer - flips
    -- uint16_t - Accelerometer - taps
    -- int16_t  - Brightness
    -- int16_t  - Soil moisture
    -- float    - Gps - lat
    -- float    - Gps - lon
    -- int16_t  - Temp
    -- int16_t  - Hum
    -- char     - Color

	setValue(appeui, deveui, "AccX",         resiot_ba2sintBE16(resiot_baslice(bytes, 0,   2)) / 1000.0)
	setValue(appeui, deveui, "AccY",         resiot_ba2sintBE16(resiot_baslice(bytes, 2,   4)) / 1000.0)
	setValue(appeui, deveui, "AccZ",         resiot_ba2sintBE16(resiot_baslice(bytes, 4,   6)) / 1000.0)
	setValue(appeui, deveui, "AccFlips",      resiot_ba2intBE16(resiot_baslice(bytes, 6,   8)))
	setValue(appeui, deveui, "AccTaps",       resiot_ba2intBE16(resiot_baslice(bytes, 8,  10)))
	setValue(appeui, deveui, "Brightness",   resiot_ba2sintBE16(resiot_baslice(bytes, 10, 12)) / 100.0)
	setValue(appeui, deveui, "SoilMoisture", resiot_ba2sintBE16(resiot_baslice(bytes, 12, 14)) / 100.0)
    lat =                                   resiot_ba2float32BE(resiot_baslice(bytes, 14, 18))
    lon =                                   resiot_ba2float32BE(resiot_baslice(bytes, 18, 22))
    setValue(appeui, deveui, "Temperature",  resiot_ba2sintBE16(resiot_baslice(bytes, 22, 24)) / 100.0)
    setValue(appeui, deveui, "Humidity",     resiot_ba2sintBE16(resiot_baslice(bytes, 24, 26)) / 100.0)
    colotChar =                                   resiot_ba2str(resiot_baslice(bytes, 26, 27))
    modeChar =                                    resiot_ba2str(resiot_baslice(bytes, 27, 28))

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
