import setuptools

setuptools.setup(
    url="https://github.com/clovaai/CRAFT-pytorch",
    packages=setuptools.find_packages(),
    python_requires=">=3.6",
    install_requires=[
        "opencv-python",
        "scikit-image",
        "scipy",
    ],
)