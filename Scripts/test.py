import serial
import matplotlib.pyplot as plt

# Default values
default_com_port = 'COM4'
default_interval = 0.1
window_size = 100  # Number of data points to display in the plot

# Prompt user for COM port
com_port = input(f"Enter COM port (default: {default_com_port}): ").strip() or default_com_port

# Prompt user for update time interval
interval = input(f"Enter update time interval in seconds (default: {default_interval}): ").strip() or default_interval
interval = float(interval)

# Define upper and lower voltages (replace with actual values)
upper_voltage = 3.3  # Example: 3.3V
lower_voltage = 0.0  # Example: 0V

# Open serial port
ser = serial.Serial(com_port, 9600)

# Initialize lists to store data
duty_values = []
voltages = []

# Function to interpolate voltage based on duty cycle
def interpolate_voltage(duty):
    voltage_range = upper_voltage - lower_voltage
    voltage = lower_voltage + (voltage_range * duty / 100)
    return voltage

# Flag to indicate whether to continue plotting
plotting = True

# Read data from serial port
try:
    while plotting:
        # Read duty cycle value from serial port
        duty = ser.readline().decode().strip()
        duty = float(''.join(char for char in duty if char.isdigit() or char in '.-'))
        
        # Interpolate voltage based on duty cycle
        voltage = interpolate_voltage(duty)
        
        # Store data in lists
        duty_values.append(duty)
        voltages.append(voltage)
        
        # Plot PWM signal with limited window size
        if len(voltages) > window_size:
            plt.plot(voltages[-window_size:])
        else:
            plt.plot(voltages)
            
        plt.xlabel('Time')
        plt.ylabel('Voltage')
        plt.title('PWM Signal')
        plt.pause(interval)
        plt.clf()  # Clear plot for next iteration

except KeyboardInterrupt:
    # Close serial port
    ser.close()
    # Close plot
    plt.close()
