import os

from distutils.core import setup
from distutils.extension import Extension

try:
    import Cython
    USE_CYTHON = bool(os.environ.get("USE_CYTHON"))
except ImportError:
    USE_CYTHON = False


ext = ".pyx" if USE_CYTHON else ".c"

extensions = [Extension("fibonacci", ["fibonacci" + ext])]

if USE_CYTHON:
    from Cython.Build import cythonize
    extensions = cythonize(extensions)

setup(
    name="fibonacci",
    ext_modules=extensions,
    extras_requires={
        "with-cython": ["cython==0.23.4"]
    }
)
