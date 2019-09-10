#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""The setup script."""

from setuptools import setup, find_packages
from distutils.core import Extension
from Cython.Build import cythonize

"""
# First create an Extension object with the appropriate name and sources.

ext = Extension(name="wrap_fib", sources=["cfib.c", "wrap_fib.pyx"])

# Use cythonize on the extension object.
setup(ext_modules=cythonize(ext))

"""
extensions = [
    Extension("hello", ["copi/hello.pyx"], libraries=["user32"]),
    Extension("fib", ["copi/fib.pyx"])
]

ext_modules=cythonize(extensions, language_level = 3)

with open('README.rst') as readme_file:
    readme = readme_file.read()

with open('HISTORY.rst') as history_file:
    history = history_file.read()

requirements = ["cython", "typing"]

setup_requirements = ['pytest-runner']

test_requirements =  ['pytest < 5.0', ]

setup(
    author="Christoph Schueler",
    author_email='cpu12.gems@googlemail.com',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Natural Language :: English',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
    ],
    description="copi is your friendly IOCP (I/O completion ports) micro framework, exclusively for Windows.",
    install_requires=requirements,
    license="BSD license",
    long_description=readme + '\n\n' + history,
    include_package_data=True,
    keywords='copi',
    name='copi',
    packages=find_packages(include=['copi']),
    setup_requires=setup_requirements,
    test_suite='tests',
    tests_require=test_requirements,
    ext_modules = ext_modules,
    url='https://github.com/christoph2/copi',
    version='0.1.0',
    zip_safe=False,
)
