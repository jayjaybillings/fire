#!/usr/bin/python
# Configuration file for generating docs on readthedocs.org
import subprocess, os

read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

if read_the_docs_build:

    subprocess.call('doxygen Doxyfile.rtfd', shell=True)