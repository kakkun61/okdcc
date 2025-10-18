# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

project = 'okdcc'
copyright = u'2025, Kazuki Okamoto (岡本和樹)'
author = u'Kazuki Okamoto (岡本和樹)'

extensions = ['breathe']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'sphinx_rtd_theme'

# Read the Docs' recommended options
# https://docs.readthedocs.com/platform/stable/guides/manage-translations-sphinx.html#create-translatable-files
gettext_uuid = True
gettext_compact = False

# Options for breath extension
breathe_default_project = "okdcc"
breathe_projects = {"okdcc": "../.build/okdcc/doc/doxygen/en/xml"}
