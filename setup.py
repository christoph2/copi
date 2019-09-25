#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""The setup script."""

from setuptools import setup, find_packages, Extension
from Cython.Build import cythonize

options = ["/Ox"] # ["/Ox", "/GS-", "/EHsc"]

extensions = [
    Extension(name = "copi.copi", sources = ["copi/copi.pyx", "copi/_iocp.cpp",
            "copi/_socket.cpp", "copi/_exceptions.cpp", "copi/_locks.cpp", "copi/_wsock.cpp",
            "copi/_file.cpp", "copi/_mmap.cpp", "copi/_helper.cpp"
        ],
        libraries = ["ws2_32", "kernel32"], extra_compile_args = options),
]

ext_modules=cythonize(extensions, language_level = 3, annotate = True)

with open('README.rst') as readme_file:
    readme = readme_file.read()

with open('HISTORY.rst') as history_file:
    history = history_file.read()

requirements = ["typing"]

setup_requirements = ['cython', 'pytest-runner']

test_requirements =  ['pytest < 5.0', ]

setup(
    author="Christoph Schueler",
    author_email='cpu12.gems@googlemail.com',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: Microsoft :: Windows',
        'Natural Language :: English',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Topic :: Software Development'
    ],
    description="copi is your friendly IOCP (I/O completion ports) micro framework, exclusively for Windows.",
    install_requires=requirements,
    license="BSD license",
    long_description=readme + '\n\n' + history,
    include_package_data=True,
    keywords=['iocp ', 'Windows'],
    platforms=['Windows'],
    name='copi',
    packages=find_packages(include=['copi']),
    setup_requires=setup_requirements,
    test_suite='tests',
    tests_require=test_requirements,
    ext_modules = ext_modules,
    url='https://github.com/christoph2/copi',
    version='0.1.18',
    zip_safe=False,
)
