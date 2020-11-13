
# Scamp Python Interface Module

First of all, install Python3 (64-bit) and Pip3. Then install Setuptools module for python: 

    pip3 install setuptools


## For Windows & Visual Studio

Use the following command line to build and install the python module:

    python3 setup.py install

The "setup.py" script can be modified to link with the 32-bit version of "scamp5d_interface.lib"
if the build is for 32-bit Python.


## For Linux & GCC 

First, build "scamp5d_interface" library in CodeBlocks using "Release_for_Python" configuration.
Then, run "setup_gcc.py" to build and install the module:

    python3 setup_gcc.py install


## Example
The "test.py" script is an example of a python app with GUI. It needs some other dependencies, 
such as "Tkinter", "Image", "PIL.TkImage". Install them first before running "test.py".


