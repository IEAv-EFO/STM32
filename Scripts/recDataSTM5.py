import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Initialize serial port
def init_serial(port, baudrate):
    ser = serial.Serial(port, baudrate)
    return ser

# Function to receive and process data
def receive_data(ser):
    line = ser.readline().strip().decode()
    line_filtered = ''.join(filter(lambda x: x.isdigit() or x == '.', line))
    return line_filtered

# Function to update the plot
def update(frame):
    global data_points
    line_data = receive_data(ser)
    if line_data:
        #print("Received:", line_data)  # Debug print statement
        value = float(line_data)
        data_points.append(value)
        if len(data_points) > max_data_points:
            data_points.pop(0)  # Remove the oldest data point
        line.set_data(range(len(data_points)), data_points)
        ax.relim()
        ax.autoscale_view()
    return line,

# Initialize serial port
port = input("Enter the communication port (default: COM4): ")
port = port if port else "COM4"  # Set default value if empty
baudrate = input("Enter the baudrate (default: 9600): ")
baudrate = int(baudrate) if baudrate else 9600  # Set default value if empty
ser = init_serial(port, baudrate)

# Initialize plot
fig, ax = plt.subplots()
ax.set_xlabel('Time (samples)')
ax.set_ylabel('Value')
ax.set_title('Real-Time Data Plot')

# Define the maximum number of data points to display
max_data_points = 100  # Adjust this value as needed

# Initialize empty data points list
data_points = []

# Initialize the line object
line, = ax.plot([], [], lw=2)

# Create animation
ani = FuncAnimation(fig, update, frames=None, interval=0.1, blit=True)

plt.show()

# Close serial port after plotting is finished
ser.close()
