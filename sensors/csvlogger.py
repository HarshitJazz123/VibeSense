import serial
import csv
import time

# Apna COM port confirm kar (Device Manager se)
PORT = 'COM18'  # apna actual port daal
BAUD = 115200
DURATION_SECONDS = 1800  # 30 minute
LABEL = 'motor_A_overload2.0 '  # HAR CONDITION KE LIYE YEH BADALNA

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)  # connection stabilize hone do

filename = f'{LABEL}.csv'
with open(filename, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['timestamp', 'raw_line'])  # header
    
    start_time = time.time()
    print(f"Recording {LABEL} for {DURATION_SECONDS} seconds...")
    
    while time.time() - start_time < DURATION_SECONDS:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            writer.writerow([time.time(), line])
            print(line)  # terminal mein bhi dikhega
    
print(f"Done! Saved to {filename}")
ser.close()