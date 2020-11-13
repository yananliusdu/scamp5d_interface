# commandline to use:
# pip install setuptools
# python setup.py build
# python setup.py install
#

from distutils.core import setup, Extension

source_files = ['scampmodule.cpp', 'scampmodule_packet_switch.cpp']

ext_module_1 = Extension(
    name = 'scamp', 
    sources = source_files, 
    include_dirs = ['../scamp5d_interface'], 
    library_dirs = ['../scamp5d_interface/bin/Release_for_Python/'],
    libraries = ['scamp5d_interface','uv','usb-1.0'],
    extra_compile_args = ['-O2'],
    extra_linker_args = ['-pthread'],
    language='c++'
    )
    
setup(name = 'scamp', version = '1.0', author='Jianing Chen', ext_modules = [ext_module_1])
