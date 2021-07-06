#!/usr/bin/env python
from setuptools import setup
from craft import __version__

long_description = open('README.md').read()

requirements = [
    'opencv-python==3.4.2.17',
    'scikit-image==0.14.2'
]

setup(
    # Metadata
    name='craft',
    version=__version__,
    author='Baek, Youngmin and Lee, Bado and Han, Dongyoon and Yun, Sangdoo and Lee, Hwalsuk',
    url='https://github.com/clovaai/CRAFT-pytorch',
    description='Character Region Awareness for Text Detection Toolkit',
    long_description=long_description,
    license='MIT',
    # Package info
    packages=['craft'],
    zip_safe=True,
    include_package_data=True,
    install_requires=requirements,
)
