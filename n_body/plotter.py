from PyQt5 import QtWidgets, QtCore

import pyqtgraph.opengl as gl
import numpy as np

import sys


num_points = 500


class RealTime3DPlot(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.start = 0
        self.end = num_points

        # Create the main layout.
        self.layout = QtWidgets.QVBoxLayout()
        self.setLayout(self.layout)

        # Create the 3D view widget.
        self.view = gl.GLViewWidget()
        self.layout.addWidget(self.view)

        # Set the camera position.
        self.view.setCameraPosition(distance=100)

        # Create a scatter plot item.
        self.scatter = gl.GLScatterPlotItem()
        self.view.addItem(self.scatter)

        # Generate initial random data.
        self.x = np.random.rand()
        self.y = np.random.rand()
        self.z = np.random.rand()

        # Set the initial scatter plot data.
        self.scatter.setData(pos=np.column_stack((self.x, self.y, self.z)))

        # Set up a timer to update the plot every second.
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(1000)

    def update_plot(self):
        with open('./positions.txt', 'r') as file:
            lines = file.readlines()[self.start: self.end]
            file.close()

        self.start += num_points
        self.end += num_points

        self.x = np.array([])
        self.y = np.array([])
        self.z = np.array([])

        for line in lines:
            x, y, z = line[:-1].split(', ')
            self.x = np.append(self.x, x)
            self.y = np.append(self.y, y)
            self.z = np.append(self.z, z)

        # Update the scatter plot data.
        self.scatter.setData(pos=np.column_stack((self.x, self.y, self.z)))


if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = RealTime3DPlot()

    window.setWindowTitle('Real-Time 3D Scatter Plot with PyQtGraph')
    window.resize(800, 800)
    window.show()

    sys.exit(app.exec_())
