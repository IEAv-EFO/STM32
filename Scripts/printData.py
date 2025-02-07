import serial

# Serial port setup. Adjust 'COM3' and 9600 to match your configuration.
ser = serial.Serial('COM3', 9600)

def print_data():
    try:
        serial_data = ser.readline().decode('ascii').strip()
        if serial_data:
            print(serial_data)
    except Exception as e:
        print("An error occurred:", e)

while True:
    print_data()
