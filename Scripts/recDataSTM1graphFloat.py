import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Slider, Button
from collections import deque
import time

# Initialize serial port
def init_serial(port, baudrate):
    ser = serial.Serial(port, baudrate)
    return ser

# Function to receive and process data
def receive_data(ser):
    line = ser.readline().strip().decode()
    line_filtered = ''.join(filter(lambda x: x.isdigit() or x in '.-', line))  # Filter numeric characters and dots and minus sign
    return line_filtered

# Global variables
max_data_points = 200  # Adjust as needed
data_points1 = deque(maxlen=max_data_points)
paused = False  # Variable to track pause/resume state

# Function to update the plot
def update(frame):
    global data_points1
    if not paused:
        line_data = receive_data(ser)
        if line_data:
            # Extract values from the received data
            try:
                value1 = float(line_data)
                data_points1.append(value1)  # Append instead of appendleft

                # Update the plots with the new data points
                line1.set_data(range(len(data_points1)), list(data_points1))  # Convert deque to list for plotting
                ax1.relim()
                ax1.autoscale_view()
            except ValueError:
                pass  # Ignore lines that can't be converted to float

    return line1,

# Function to toggle pause/resume
def toggle_pause(event):
    global paused
    paused = not paused
    button_pause.label.set_text('Resume' if paused else 'Pause')

# Function to update max_data_points
def update_max_data_points(val):
    global max_data_points, data_points1
    max_data_points = int(val)
    data_points1 = deque(data_points1, maxlen=max_data_points)

# Initialize serial port
port = input("Enter the communication port (default: COM3): ")
port = port if port else "COM3"  # Set default value if empty
baudrate = input("Enter the baudrate (default: 9600): ")
baudrate = int(baudrate) if baudrate else 9600  # Set default value if empty
max_data_points = input("Enter the max data points (default: 200): ")
max_data_points = int(max_data_points) if max_data_points else 200
update_interval = input("Enter the interval in milliseconds (default: 100): ")
update_interval = int(update_interval) if update_interval else 100
ser = init_serial(port, baudrate)

# Initialize plot
fig, ax1 = plt.subplots()
ax1.set_xlabel('Index')
ax1.set_ylabel('Value 1')
ax1.set_title('Data Plot 1')

# Initialize empty data points deque with a maximum length
data_points1 = deque(maxlen=max_data_points)

# Initialize lines for each plot
line1, = ax1.plot([], [])

# Create animation with a smaller interval for closer to real-time plotting
ani = FuncAnimation(fig, update, frames=None, interval=update_interval, blit=True, cache_frame_data=False)

# Create a slider to change max_data_points
slider_ax = plt.axes([0.93, 0.3, 0.03, 0.35], facecolor='red')  # Define slider position and size
slider = Slider(slider_ax, 'H', 2, 1000, valinit=max_data_points, color='lightgreen', orientation='vertical')
slider.on_changed(update_max_data_points)  # Set slider update action

# Create a button to pause/resume animation
button_pause_ax = plt.axes([0.8, 0.9, 0.1, 0.05])  # Define button position and size
button_pause = Button(button_pause_ax, 'Pause', color='lightblue', hovercolor='skyblue')
button_pause.on_clicked(toggle_pause)  # Set button click action

plt.show()

# Close serial port after plotting is finished
ser.close()
