File Reader & Graph Plotter

This application allows users to read data from files and plot graphs using a custom Qt Widget Application. It is built with C++ and Qt Widgets for interactive visualization and data handling.

Features :-

Read and parse data from files.

Plot graphs using Qt's customizable plotting widgets.

User-friendly interface with adjustable settings.

Table of Contents

Installation :-

How to Run

Usage

Contributing

License

Installation

Prerequisites

Qt (tested on Qt 5.15.2 and higher)

Qt Creator or any C++ IDE that supports Qt

CMake or qmake (for building the project)

Steps to Set Up the Project:

Clone the repository:


bash

Copy code

git clone https://github.com/your-username/your-repo.git

Open the project in Qt Creator:


Launch Qt Creator.

Go to File -> Open File or Project... and select the .pro file or CMakeLists.txt file from the cloned repository.

Configure the project:


Select the appropriate Qt version and kit for your environment.

Set the build directory, if needed.

Build the project:


Click the Build button or use the shortcut (Ctrl + B).

Run the project:


Once the build is complete, click the Run button or use the shortcut (Ctrl + R).

How to Run

Running the application in Qt Creator:

Open the Project: After cloning, open the project in Qt Creator by selecting the .pro or CMakeLists.txt file.


Build the Application:


In Qt Creator, go to Build -> Build Project or press Ctrl + B.

Run the Application:


Go to Run -> Run Project or press Ctrl + R.

The main window will open where you can load a file and plot graphs.

Running the application from command line:

Navigate to the project's build directory:


bash

Copy code

cd /path/to/your/project/build

Run the application executable:


bash

Copy code

./your-application

File Input:

The application accepts specific file formats (e.g., CSV, TXT) for reading and plotting data.

Navigate through the file dialog in the application to open your data file.

Usage

Loading a File

Open the application.

Click the "Open File" button to select a data file.

Choose the file you want to load for plotting.

Plotting Graphs

Once the file is loaded, use the available options to customize your graph (e.g., select parameters to plot).

Click "Plot Graph" to generate the graph based on the selected data.

Example of File Data:

The file should contain columns of data that the application can parse and visualize, such as:


css

Copy code

Time, Speed, Energy

0, 0, 0

1, 10, 20

2, 20, 40

Contributing

Contributions are welcome! Please fork the repository and submit a pull request if you would like to contribute.


Fork the repository.

Create a new branch (git checkout -b feature/AmazingFeature).

Commit your changes (git commit -m 'Add AmazingFeature').

Push to the branch (git push origin feature/AmazingFeature).

Open a pull request.
