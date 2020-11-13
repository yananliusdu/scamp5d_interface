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
    library_dirs = ['../x64/Release/'], # for a 32-bit build, it's '../Release/'
    libraries = ['scamp5d_interface','kernel32','user32','advapi32'],
    extra_compile_args = ['/O2'],
    extra_link_args = ['/LTCG:OFF'],
    language='c++'
    )
    
setup(name = 'scamp', version = '1.0', author='Jianing Chen', ext_modules = [ext_module_1])
