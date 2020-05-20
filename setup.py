from setuptools import setup, find_packages

# read version
about_info = {}
with open("version.txt", "r") as v:
    exec(v.read(), about_info)

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name="h2o_craft",
    version=about_info["version"],
    description="H2O.ai port of Pytorch implementation of CRAFT text detector",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://www.h2o.ai",
    author="H2O.ai",
    author_email="team@h2o.ai",
    license="MIT License",
    classifiers=[
        "Development Status :: 1 - Planning",
        "Environment :: Other Environment",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Image Recognition",
    ],
    keywords="",
    packages=find_packages(),
    python_requires="~=3.6",
    install_requires=[
        "certifi==2020.4.5.1",
        "chardet==3.0.4",
        "cycler==0.10.0",
        "decorator==4.4.2",
        "future==0.18.2",
        "idna==2.9",
        "imagecodecs==2020.2.18",
        "imageio==2.8.0",
        "kiwisolver==1.2.0",
        "matplotlib==3.2.1",
        "networkx==2.4",
        "numpy==1.18.4",
        "opencv-python==4.2.0.34",
        "pillow==7.1.2",
        "pyparsing==2.4.7",
        "python-dateutil==2.8.1",
        "pywavelets==1.1.1",
        "requests==2.23.0",
        "scikit-image==0.17.2",
        "scipy==1.4.1",
        "six==1.14.0",
        "tifffile==2020.5.11",
        "torch==1.5.0",
        "torchvision==0.6.0",
        "urllib3==1.25.9",
    ],
    dependency_links=[],
    # package_data={},
    # data_files=[],
    # entry_points={},
)
