from setuptools import setup, Extension
import numpy as np

bezier_module = Extension(
    'bezier',
    sources=['bezier.c'],
    include_dirs=[np.get_include()]
)

setup(
    name='bezier',
    version='0.1',
    description='Bézier curve generator as a C extension',
    ext_modules=[bezier_module],
    install_requires=['numpy'],
)
