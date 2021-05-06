from distutils.core import setup, Extension

setup(
    name="custom3",
    version="1.0",
    ext_modules=[
        Extension("custom3", ["custom3.c"]),
    ]
)
