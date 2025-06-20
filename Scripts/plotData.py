from pyqtgraph.Qt import QtWidgets, QtCore
import pyqtgraph as pg
import serial

class MainWindow(QtWidgets.QMainWindow):
    class SerialPlotter(QtCore.QObject):
        data_updated = QtCore.pyqtSignal(list)
        duty_cycle_updated = QtCore.pyqtSignal(float)

        def __init__(self, parent=None):
            super().__init__(parent)
            self.data = []
            self.last_data_points = 60
            self.high_samples = 0  # Track the number of high samples
            self.total_samples = 0  # Track the total number of samples
            self.duty_cycle_update_interval = 50
            self.duty_cycle_window = []  # Store recent duty cycle values
            self.duty_cycle_window_size = 10  # Number of values to average
            self.timer = QtCore.QTimer(self)
            self.timer.timeout.connect(self.calculate_duty_cycle)
            self.timer.start(self.duty_cycle_update_interval)
            self.paused = False

        def read_serial(self):
            try:
                while ser.in_waiting:
                    serial_data = ser.readline().decode('utf-8').strip()
                    if serial_data:
                        filtered_data = ''.join(char for char in serial_data if char.isdigit())
                        if filtered_data:
                            value = int(filtered_data)
                            self.data.append(value)
                            self.total_samples += 1
                            self.high_samples += value
                if not self.paused:
                    self.data_updated.emit(self.data[-self.last_data_points:])
            except Exception as e:
                print("An error occurred in read_serial:", e)

        def calculate_duty_cycle(self):
            if self.total_samples > 0:
                duty_cycle = (self.high_samples / self.total_samples) * 100
                self.duty_cycle_window.append(duty_cycle)
                if len(self.duty_cycle_window) > self.duty_cycle_window_size:
                    self.duty_cycle_window.pop(0)  # Remove oldest value
                duty_cycle_avg = sum(self.duty_cycle_window) / len(self.duty_cycle_window)
                self.duty_cycle_updated.emit(duty_cycle_avg)
            # Reset counters after each calculation
            self.high_samples = 0
            self.total_samples = 0

    def __init__(self):
        super().__init__()

        # Create application window
        self.setWindowTitle("NOT SO REALTIME DATA PLOTTER")

        # Create a plot window
        self.plot_window = pg.GraphicsLayoutWidget(show=True)
        self.plot = self.plot_window.addPlot(title="Realtime Data")
        self.plot.showGrid(x=True, y=True)  # Show grid lines

        self.curve = self.plot.plot(pen='y')

        # Serial port setup. Adjust 'COM3' and 9600 to match your configuration.
        self.default_com_port = 'COM3'
        self.default_baudrate = 9600

        self.com_port_label = QtWidgets.QLabel("COM Port:")
        self.com_port_edit = QtWidgets.QLineEdit(self.default_com_port)
        self.baudrate_label = QtWidgets.QLabel("Baudrate:")
        self.baudrate_edit = QtWidgets.QLineEdit(str(self.default_baudrate))

        # Spinboxes for last data points, timer interval, and duty cycle update interval
        self.last_data_points_label = QtWidgets.QLabel("Last Data Points:")
        self.last_data_points_spinbox = QtWidgets.QSpinBox()
        self.last_data_points_spinbox.setMinimum(1)
        self.last_data_points_spinbox.setMaximum(1000)
        self.last_data_points_spinbox.setValue(60)

        self.timer_interval_label = QtWidgets.QLabel("Timer Interval:")
        self.timer_interval_spinbox = QtWidgets.QSpinBox()
        self.timer_interval_spinbox.setMinimum(10)
        self.timer_interval_spinbox.setMaximum(1000)
        self.timer_interval_spinbox.setValue(100)
        self.timer_interval_spinbox.setSuffix(" ms")

        # Spinboxes for x and y axis intervals
        self.x_axis_interval_label = QtWidgets.QLabel("X Grid Interval:")
        self.x_axis_interval_spinbox = QtWidgets.QDoubleSpinBox()
        self.x_axis_interval_spinbox.setMinimum(0.1)
        self.x_axis_interval_spinbox.setMaximum(1000.0)
        self.x_axis_interval_spinbox.setValue(1.0)

        self.y_axis_interval_label = QtWidgets.QLabel("Y Grid Interval:")
        self.y_axis_interval_spinbox = QtWidgets.QDoubleSpinBox()
        self.y_axis_interval_spinbox.setMinimum(0.1)
        self.y_axis_interval_spinbox.setMaximum(1000.0)
        self.y_axis_interval_spinbox.setValue(1.0)

        # Set locale to use '.' as the decimal separator
        self.x_axis_interval_spinbox.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedStates))
        self.y_axis_interval_spinbox.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedStates))

        # Connect valueChanged signals
        self.x_axis_interval_spinbox.valueChanged.connect(self.update_x_axis_interval)

        # Y-axis limit input fields
        self.y_axis_lower_limit_label = QtWidgets.QLabel("Y Axis Lower Limit:")
        self.y_axis_lower_limit_edit = QtWidgets.QLineEdit(str(0))
        self.y_axis_upper_limit_label = QtWidgets.QLabel("Y Axis Upper Limit:")
        self.y_axis_upper_limit_edit = QtWidgets.QLineEdit(str(10))

        # Connect textChanged signals
        self.y_axis_lower_limit_edit.textChanged.connect(self.update_y_axis_limits)
        self.y_axis_upper_limit_edit.textChanged.connect(self.update_y_axis_limits)

        # Duty Cycle display
        self.duty_cycle_label = QtWidgets.QLabel("Duty Cycle: 0.00%")

        # Button to pause/resume animation
        self.pause_button = QtWidgets.QPushButton("Pause")
        self.pause_button.setCheckable(True)
        self.pause_button.setChecked(False)  # Start animation running by default
        self.pause_button.clicked.connect(self.toggle_animation)

        # Buttons to move x-axis labels left or right
        self.move_x_left_button = QtWidgets.QPushButton("Move X Left")
        self.move_x_right_button = QtWidgets.QPushButton("Move X Right")
        self.move_x_left_button.clicked.connect(self.move_x_axis_left)
        self.move_x_right_button.clicked.connect(self.move_x_axis_right)

        # Button to toggle grid lines
        self.toggle_grid_button = QtWidgets.QPushButton("Toggle Grid")
        self.toggle_grid_button.setCheckable(True)
        self.toggle_grid_button.setChecked(True)  # Grid lines shown by default
        self.toggle_grid_button.clicked.connect(self.toggle_grid)

        # Layout setup for control panel
        control_layout = QtWidgets.QVBoxLayout()
        control_layout.addWidget(self.com_port_label)
        control_layout.addWidget(self.com_port_edit)
        control_layout.addWidget(self.baudrate_label)
        control_layout.addWidget(self.baudrate_edit)
        control_layout.addWidget(self.last_data_points_label)
        control_layout.addWidget(self.last_data_points_spinbox)
        control_layout.addWidget(self.timer_interval_label)
        control_layout.addWidget(self.timer_interval_spinbox)
        control_layout.addWidget(self.x_axis_interval_label)
        control_layout.addWidget(self.x_axis_interval_spinbox)
        control_layout.addWidget(self.y_axis_interval_label)
        control_layout.addWidget(self.y_axis_interval_spinbox)
        control_layout.addWidget(self.y_axis_lower_limit_label)
        control_layout.addWidget(self.y_axis_lower_limit_edit)
        control_layout.addWidget(self.y_axis_upper_limit_label)
        control_layout.addWidget(self.y_axis_upper_limit_edit)
        control_layout.addStretch(1)  # Add stretchable space
        #control_layout.addWidget(self.duty_cycle_label)
        control_layout.addWidget(self.pause_button)
        control_layout.addWidget(self.move_x_left_button)
        control_layout.addWidget(self.move_x_right_button)
        control_layout.addWidget(self.toggle_grid_button)

        # Group control widgets into a frame
        control_frame = QtWidgets.QFrame()
        control_frame.setLayout(control_layout)
        control_frame.setFrameStyle(QtWidgets.QFrame.Panel | QtWidgets.QFrame.Raised)

        # Layout setup for main window
        main_layout = QtWidgets.QHBoxLayout()
        main_layout.addWidget(self.plot_window)
        main_layout.addWidget(control_frame)

        central_widget = QtWidgets.QWidget()
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)

        # Serial plotter instance
        self.serial_plotter = self.SerialPlotter()

        # Connect signals and slots
        self.com_port_edit.textChanged.connect(self.update_serial_connection)
        self.baudrate_edit.textChanged.connect(self.update_serial_connection)
        self.last_data_points_spinbox.valueChanged.connect(self.update_last_data_points)
        self.timer_interval_spinbox.valueChanged.connect(self.update_timer_interval)
        self.x_axis_interval_spinbox.valueChanged.connect(self.update_x_axis_interval)
        self.y_axis_interval_spinbox.valueChanged.connect(self.update_y_axis_interval)
        self.serial_plotter.duty_cycle_updated.connect(self.update_duty_cycle)
        self.serial_plotter.data_updated.connect(self.update_plot)

        # Initially establish serial connection
        self.update_serial_connection()

        # Start the timer to read serial data
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.serial_plotter.read_serial)
        self.timer.start(100)  # Assuming a default interval of 100 ms for now

        # Initialize y-axis limits flag
        self.y_axis_limits_set = False

        # Initialize grid lines to be shown
        self.show_grid = True

    def update_serial_connection(self):
        global ser
        com_port = self.com_port_edit.text()
        baudrate = int(self.baudrate_edit.text())
        ser = serial.Serial(com_port, baudrate, timeout=0.1)

    def update_last_data_points(self, points):
        self.serial_plotter.last_data_points = points

    def update_timer_interval(self, interval):
        self.timer.setInterval(interval)

    def update_duty_cycle_interval(self, interval):
        self.serial_plotter.timer.setInterval(interval)

    def update_x_axis_interval(self, interval):
        axis = self.plot.getAxis('bottom')
        ticks = [(i * interval, str(i * interval)) for i in range(int(axis.range[0] // interval), int(axis.range[1] // interval) + 1)]
        axis.setTicks([ticks, []])  # Set x-axis interval

    def update_y_axis_interval(self, interval):
        axis = self.plot.getAxis('left')
        ticks = [(i * interval, str(i * interval)) for i in range(int(axis.range[0] // interval), int(axis.range[1] // interval) + 1)]
        axis.setTicks([ticks, []])  # Set y-axis interval

    def update_y_axis_limits(self):
        try:
            lower_limit = float(self.y_axis_lower_limit_edit.text())
            upper_limit = float(self.y_axis_upper_limit_edit.text())
            self.plot.setYRange(lower_limit, upper_limit)
        except ValueError:
            pass  # Ignore non-numeric input

    def update_duty_cycle(self, duty_cycle):
        self.duty_cycle_label.setText(f"Duty Cycle: {duty_cycle:.2f}%")
        # Update plot title to include duty cycle
        self.plot.setTitle(f"Realtime Data\u00A0\u00A0\u00A0\u00A0---\u00A0\u00A0\u00A0\u00A0Duty Cycle: {duty_cycle:.2f}%")


    def update_plot(self, data):
        self.curve.setData(data)

        if data:
            max_data = max(data)
            max_grid = int(max_data) + 1  # Ensure max_data aligns with the next integer value
            if not self.y_axis_limits_set:
                self.plot.setYRange(0, max_grid, padding=0)  # Set y-axis range with zero padding
                self.plot.getAxis('left').setGrid(1)  # Set y-axis grid to 1
                self.y_axis_limits_set = True

    def toggle_animation(self):
        self.serial_plotter.paused = not self.serial_plotter.paused
        if self.serial_plotter.paused:
            self.pause_button.setText("Resume")
        else:
            self.pause_button.setText("Pause")

    def move_x_axis_left(self):
        lower, upper = self.plot.getAxis('bottom').range
        self.plot.getAxis('bottom').setRange(lower - 1, upper - 1)

    def move_x_axis_right(self):
        lower, upper = self.plot.getAxis('bottom').range
        self.plot.getAxis('bottom').setRange(lower + 1, upper + 1)

    def toggle_grid(self):
        self.show_grid = not self.show_grid
        self.plot.showGrid(x=self.show_grid, y=self.show_grid)  # Toggle grid lines visibility

if __name__ == '__main__':
    import sys
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
