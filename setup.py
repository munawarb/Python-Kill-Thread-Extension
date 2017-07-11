from distutils.core import setup, Extension

threader = Extension('threader',
	sources = ['threader.c'])

setup (name = 'Threader',
	version = '1.0',
	description = 'External thread killing',
	ext_modules = [threader])