import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Slider, Button, TextBox
from collections import deque
import time

# Initialize serial port
def init_serial(port, baudrate):
    ser = serial.Serial(port, baudrate)
    return ser

# Function to receive and process data
def receive_data(ser):
    line = ser.readline().strip().decode()
    # Filter out non-numeric characters (e.g., null bytes)
    numeric_part = ''.join(char for char in line if char.isdigit() or char in '.-')
    return numeric_part

# Function to update the plot
def update(frame):
    global data_points, data_points_time, ax, last_received_value, is_paused, last_time
    if not is_paused:
        # Append the last received value to the data points
        current_time = time.time()
        elapsed_time = (current_time - last_time)
        last_time = current_time
        data_points.append(last_received_value)
        data_points_time.append(data_points_time[-1] + elapsed_time if data_points_time else 0)
    
        # Update the plot with the new data points
        ax.clear()
        ax.plot(data_points_time, data_points, marker='.')  # Plot with markers
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Volts')
        ax.set_title('\"Real-Time\" Data Plot')
        ax.set_ylim(0, 4.0)  # Fix y-axis limit to 4.0
        ax.grid(True)  # Enable grid lines
        
        # Add vertical grid lines
        ax.xaxis.set_major_locator(plt.MultipleLocator(grid_interval))
        ax.grid(which='major', color='gray', linestyle='--')

    return ax,

# Function to update max_data_points
def update_max_data_points(val):
    global max_data_points, data_points, data_points_time
    max_data_points = int(val)
    data_points = deque(data_points, maxlen=max_data_points)
    data_points_time = deque(data_points_time, maxlen=max_data_points)

# Function to update the received value
def update_received_value():
    global last_received_value
    line_data = receive_data(ser)
    if line_data:
        try:
            last_received_value = float(line_data)
        except ValueError:
            pass  # Skip if the received data cannot be converted to float

# Function to pause or resume the animation
def pause_resume(event):
    global is_paused
    is_paused = not is_paused
    if is_paused:
        button.label.set_text('Resume')
    else:
        button.label.set_text('Pause')

# Function to update grid interval
def update_grid_interval(text):
    global grid_interval
    try:
        grid_interval = float(text)
    except ValueError:
        pass



# Function to move the plot left without changing grid intervals
def move_grid_left(event):
    for line in ax.lines:
        x_data = line.get_xdata()
        line.set_xdata(x_data - step_size)
    ax.figure.canvas.draw_idle()  # Redraw the plot

# Function to move the plot right without changing grid intervals
def move_grid_right(event):
    for line in ax.lines:
        x_data = line.get_xdata()
        line.set_xdata(x_data + step_size)
    ax.figure.canvas.draw_idle()  # Redraw the plot

# Ask user for COM port, baud rate, and update interval
default_com_port = "COM5"
default_baudrate = 9600
default_interval = 0.001  # Microsecond
default_grid_interval = 0.2 # Second
default_step_size = 0.01 # Second

com_port = input(f"Enter the COM port (default: {default_com_port}): ") or default_com_port
baudrate = int(input(f"Enter the baud rate (default: {default_baudrate}): ") or default_baudrate)
update_interval = float(input(f"Enter the update interval in milliseconds (default: {default_interval}): ") or default_interval)
grid_interval = float(input(f"Enter the grid interval in seconds: {default_grid_interval}): ") or default_grid_interval)
step_size = float(input(f"Enter the step size for plot positioning in seconds: {default_step_size}): ") or default_step_size)

# Initialize serial port
ser = init_serial(com_port, baudrate)

# Initialize plot
fig, ax = plt.subplots()
data_points = deque(maxlen=100)
data_points_time = deque(maxlen=100)
last_received_value = 0.0
is_paused = False
last_time = time.time()

# Create animation with the specified update interval and disable frame data caching
ani = FuncAnimation(fig, update, frames=None, interval=update_interval, blit=False, cache_frame_data=False)

# Create a slider to change max_data_points
max_data_points = 100  # Set default value
slider_ax = plt.axes([0.93, 0.3, 0.03, 0.35], facecolor='red')  # Define slider position and size
slider = Slider(slider_ax, 'H', 2, 30, valinit=max_data_points, color='lightgreen', orientation='vertical')
slider.on_changed(update_max_data_points)  # Set slider update action

# Create a button to pause/resume the animation
button_ax = plt.axes([0.85, 0.015, 0.1, 0.04])
button = Button(button_ax, 'Pause')
button.on_clicked(pause_resume)

# Create a text box to input grid interval
grid_interval_text_ax = plt.axes([0.74, 0.015, 0.1, 0.04])
grid_interval_text = TextBox(grid_interval_text_ax, 'Grid (s) ', initial=str(grid_interval))
grid_interval_text.on_submit(update_grid_interval)



# Create buttons to move the plot left and right
move_left_button_ax = plt.axes([0.11, 0.015, 0.1, 0.04])
move_left_button = Button(move_left_button_ax, '<<')
move_left_button.on_clicked(move_grid_left)

move_right_button_ax = plt.axes([0.21, 0.015, 0.1, 0.04])
move_right_button = Button(move_right_button_ax, '>>')
move_right_button.on_clicked(move_grid_right)

# Function to continuously update the received value
def update_serial_data():
    while True:
        update_received_value()

# Start a thread to continuously update the received value
import threading
serial_thread = threading.Thread(target=update_serial_data)
serial_thread.daemon = True
serial_thread.start()

plt.show()
