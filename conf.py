#!/usr/bin/python
# Configuration file for generating docs on readthedocs.org
import subprocess, os
from recommonmark.parser import CommonMarkParser

# Generate Doxygen files
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

if read_the_docs_build:

    subprocess.call('doxygen Doxyfile.rtfd', shell=True)
    subprocess.call('ls -R', shell=True)
    subprocess.call('mv html _build/doxygen-html', shell=True)
    subprocess.call('mv latex _build/doxygen-latex', shell=True)

# Configure markdown support
source_parsers = {
    '.md': CommonMarkParser,
}

source_suffix = ['.rst', '.md']
