#!/usr/bin/python
# Configuration file for generating docs on readthedocs.org
import subprocess, os
from recommonmark.parser import CommonMarkParser

# Generate Doxygen files
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

# Run the doxygen build
if read_the_docs_build:

    subprocess.call('doxygen Doxyfile.rtfd', shell=True)

# Configure breathe support
breathe_projects = {
    "fire-framework":"xml/",
    }

# Configure markdown support
source_parsers = {
    '.md': CommonMarkParser,
}

source_suffix = ['.rst', '.md']
extensions = [ "breathe" ]
