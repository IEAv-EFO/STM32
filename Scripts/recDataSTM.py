import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Slider
from collections import deque
from datetime import datetime
from scipy.signal import find_peaks

# Initialize serial port
def init_serial(port, baudrate):
    ser = serial.Serial(port, baudrate)
    return ser

# Function to receive and process data
def receive_data(ser):
    line = ser.readline().strip().decode()
    # Filter out non-numeric characters
    # line_filtered = ''.join(filter(lambda x: x.isdigit() or x in '.-', line))
    line_filtered = ''.join(filter(lambda x: x.isdigit(), line))
    return line_filtered

# Function to calculate frequency from periods
def calculate_frequency(periods):
    if len(periods) == 0:
        return 0.0
    avg_period = sum(periods) / len(periods)
    return 1 / avg_period

# Global variables for frequency calculation
# last_time = None
# frequency = 0.0
# peaks = deque(maxlen=10)  # Store timestamps of peaks
# periods = deque(maxlen=10)  # Store periods between peaks
# min_peak_distance = 1.0  # Minimum time difference between peaks in seconds

# Function to update the plot
def update(frame):
    global data_points, last_time, frequency, peaks, periods
    # Receive data
    line_data = receive_data(ser)
    if line_data:
        value = int(line_data)
        data_points.append(value)
        
        #current_time = datetime.now()
        
        # Find peaks in the data
        # peak_indices, _ = find_peaks(data_points, distance=min_peak_distance * 1000)  # distance in samples
        
        # Check if there are new peaks
        # if peak_indices.size > 0:  # Check if the array has any elements
            # new_peak_time = current_time
            
            # # Calculate period and update frequency
            # if last_time is not None:
                # time_diff = (new_peak_time - last_time).total_seconds()  # Time difference in seconds
                # periods.append(time_diff)
                # frequency = calculate_frequency(periods)
                
            # last_time = new_peak_time
            
            # # Store peak timestamps
            # peaks.append(last_time)
        
        # Update the plot with the new data points
        line.set_data(range(len(data_points)), data_points)
        ax.relim()
        ax.autoscale_view()
        
        # Update frequency text
        # freq_text.set_text(f'Data Points Frequency: {frequency:.2f} Hz')
    
    return line,

# Function to update max_data_points
def update_max_data_points(val):
    global max_data_points, data_points
    max_data_points = int(val)
    data_points = deque(data_points, maxlen=max_data_points)

# Initialize serial port
port = input("Enter the communication port (default: COM3): ")
port = port if port else "COM3"  # Set default value if empty
baudrate = input("Enter the baudrate (default: 9600): ")
baudrate = int(baudrate) if baudrate else 9600  # Set default value if empty
max_data_points = input("Enter the max data points (default: 1000): ")
max_data_points = int(max_data_points) if max_data_points else 1000
ser = init_serial(port, baudrate)

# Initialize plot
fig, ax = plt.subplots()
ax.set_xlabel('Index')
ax.set_ylabel('Value')
ax.set_title('\"Real-Time\" Data Plot')

# Set the y-axis limit to 4.0
ax.set_ylim(0, 4.0)

# Initialize empty data points deque with a maximum length
# max_data_points = 100  # Adjust this value as needed
data_points = deque(maxlen=max_data_points)
line, = ax.plot([], [])

# Place to show the frequency on the plot
freq_text = ax.text(0.02, 0.95, '', transform=ax.transAxes)

# Create animation with a smaller interval for closer to real-time plotting
ani = FuncAnimation(fig, update, frames=None, interval=0.1, cache_frame_data=False)

# Create a slider to change max_data_points
slider_ax = plt.axes([0.93, 0.3, 0.03, 0.35], facecolor='red')  # Define slider position and size
slider = Slider(slider_ax, 'H', 2, 5000, valinit=max_data_points, color='lightgreen', orientation='vertical')
slider.on_changed(update_max_data_points)  # Set slider update action

plt.show()

# Close serial port after plotting is finished
ser.close()
