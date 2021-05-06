from distutils.core import setup, Extension

setup(
    name="custom4",
    version="1.0",
    ext_modules=[
        Extension("custom4", ["custom4.c"]),
    ]
)
