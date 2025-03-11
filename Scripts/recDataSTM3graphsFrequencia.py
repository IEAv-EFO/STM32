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
    line_filtered = ''.join(filter(lambda x: x.isdigit() or x in '\t', line))  # Filter numeric characters and tabs
    return line_filtered.split('\t')  # Split data by tabs

# Global variables
max_data_points = 300  # Adjust as needed
data_points1 = deque(maxlen=max_data_points)
data_points2 = deque(maxlen=max_data_points)
data_points3 = deque(maxlen=max_data_points)
paused = False  # Variable to track pause/resume state
last_value1 = None
last_transition_time = None
frequencies = deque(maxlen=10)
avg_frequency_text = None  # Variable to hold the text object for average frequency display

# Function to update the plot
def update(frame):
    global data_points1, data_points2, data_points3, last_value1, last_transition_time, avg_frequency_text
    if not paused:
        line_data = receive_data(ser)
        if line_data:
            # Extract values from the received data
            value1, value2, value3 = map(int, line_data)
            data_points1.append(value1)  # Append instead of appendleft
            data_points2.append(value2)
            data_points3.append(value3)

            # Update the plots with the new data points
            line1.set_data(range(len(data_points1)), list(data_points1))  # Convert deque to list for plotting
            line2.set_data(range(len(data_points2)), list(data_points2))
            line3.set_data(range(len(data_points3)), list(data_points3))
            ax1.relim()
            ax1.autoscale_view()
            ax2.relim()
            ax2.autoscale_view()
            ax3.relim()
            ax3.autoscale_view()

            current_time = time.time()
            if last_value1 is not None and last_value1 != value1:
                if last_transition_time is not None and last_transition_time != current_time:
                    period = (current_time - last_transition_time) * 2
                    if period != 0:
                        frequency = 1 / period
                        frequencies.append(frequency)
                        if len(frequencies) == frequencies.maxlen:
                          avg_frequency = sum(frequencies) / len(frequencies)
                          avg_frequency_text.set_text('Average Frequency: {:.2f} Hz'.format(avg_frequency))
            last_value1 = value1
            last_transition_time = current_time

    return line1, line2, line3, avg_frequency_text

# Function to toggle pause/resume
def toggle_pause(event):
    global paused
    paused = not paused
    button_pause.label.set_text('Resume' if paused else 'Pause')

# Function to update max_data_points
def update_max_data_points(val):
    global max_data_points, data_points1, data_points2, data_points3
    max_data_points = int(val)
    data_points1 = deque(data_points1, maxlen=max_data_points)
    data_points2 = deque(data_points2, maxlen=max_data_points)
    data_points3 = deque(data_points3, maxlen=max_data_points)

# Initialize serial port
port = input("Enter the communication port (default: COM3): ")
port = port if port else "COM3"  # Set default value if empty
baudrate = input("Enter the baudrate (default: 9600): ")
baudrate = int(baudrate) if baudrate else 9600  # Set default value if empty
max_data_points = input("Enter the max data points (default: 1000): ")
max_data_points = int(max_data_points) if max_data_points else 300
ser = init_serial(port, baudrate)

# Initialize plot
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(8, 6))
ax1.set_xlabel('Index')
ax1.set_ylabel('Value 1')
ax1.set_title('Data Plot 1')
ax2.set_xlabel('Index')
ax2.set_ylabel('Value 2')
ax2.set_title('Data Plot 2')
ax3.set_xlabel('Index')
ax3.set_ylabel('Value 3')
ax3.set_title('Data Plot 3')

# Initialize empty data points deque with a maximum length
data_points1 = deque(maxlen=max_data_points)
data_points2 = deque(maxlen=max_data_points)
data_points3 = deque(maxlen=max_data_points)

# Initialize lines for each plot
line1, = ax1.plot([], [])
line2, = ax2.plot([], [])
line3, = ax3.plot([], [])

# Create text for average frequency display
avg_frequency_text = ax1.text(0.985, 0.8, '', transform=ax1.transAxes, ha='right', va='bottom', color='red')  # Adjust position and color

# Create animation with a smaller interval for closer to real-time plotting
ani = FuncAnimation(fig, update, frames=None, interval=0.1, blit=True)

# Create a slider to change max_data_points
slider_ax = plt.axes([0.93, 0.3, 0.03, 0.35], facecolor='red')  # Define slider position and size
slider = Slider(slider_ax, 'H', 2, 1000, valinit=max_data_points, color='lightgreen', orientation='vertical')
slider.on_changed(update_max_data_points)  # Set slider update action

# Create a button to pause/resume animation
button_pause_ax = plt.axes([0.8, 0.9, 0.1, 0.05])  # Define button position and size
button_pause = Button(button_pause_ax, 'Pause', color='lightblue', hovercolor='skyblue')
button_pause.on_clicked(toggle_pause)  # Set button click action

# Adjust vertical spacing between subplots
plt.subplots_adjust(hspace=0.8)

plt.show()

# Close serial port after plotting is finished
ser.close()
