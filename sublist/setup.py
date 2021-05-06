from distutils.core import setup, Extension

setup(
    name="sublist",
    version="1.0",
    ext_modules=[
        Extension("sublist", ["sublist.c"]),
    ]
)
