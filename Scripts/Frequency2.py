import serial
import time

# Initialize serial port
def init_serial(port, baudrate):
    ser = serial.Serial(port, baudrate)
    return ser

# Function to receive and process data
def receive_data(ser):
    line = ser.readline().strip().decode()
    return line

# Initialize serial port
port = input("Enter the communication port (default: COM3): ")
port = port if port else "COM3"  # Set default value if empty
baudrate = input("Enter the baudrate (default: 115200): ")
baudrate = int(baudrate) if baudrate else 115200  # Set default value if empty
ser = init_serial(port, baudrate)

# Variables for frequency calculation
last_transition_time = None
frequency_samples = []
average_denominator = input("Enter the average denominator (default: 10): ")
average_denominator = int(average_denominator) if average_denominator else 10

try:
    while True:
        line_data = receive_data(ser)
        if line_data:
            # Extract value from the received data
            value1 = int(line_data)  # Ensure the data is an integer

            #current_time = time.time()
            current_time = time.perf_counter()

            if last_transition_time is not None:
                period = current_time - last_transition_time
                if period > 0:
                    frequency = 1 / period
                    frequency_samples.append(frequency)
                    if len(frequency_samples) >= average_denominator:
                        average_frequency = sum(frequency_samples) / len(frequency_samples)
                        print('Average Frequency ({} samples): {:.2f} Hz'.format(len(frequency_samples), average_frequency))
                        frequency_samples = []
            last_transition_time = current_time

except KeyboardInterrupt:
    print("Measurement stopped by user.")

finally:
    # Close serial port
    ser.close()
