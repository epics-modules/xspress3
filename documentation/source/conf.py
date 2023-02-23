#
# xspress3epics documentation build configuration file

import sys, os

release = '3.2'
project = 'xspress3epics'
html_title = html_short_title = 'Xspress3 Epics Module'

authors = 'Matthew Newville, Tom Griffin'
copyright = 'Public Domain'

extensions = ['sphinx.ext.autodoc',
              'sphinx.ext.mathjax',
              'sphinx.ext.extlinks',
              'sphinx.ext.napoleon']

todo_include_todos = True

templates_path = ['_templates']
source_suffix = '.rst'
source_encoding = 'utf-8'
master_doc = 'index'
today_fmt = '%Y-%B-%d'

exclude_trees = ['_build']

add_function_parentheses = True
add_module_names = False
pygments_style = 'sphinx'

# html themes: 'default', 'sphinxdoc',  'alabaster', 'agogo', 'nature', 'pyramid'
html_theme = 'pyramid'

html_static_path = ['_static']
html_last_updated_fmt = '%Y-%B-%d'
html_show_sourcelink = True
htmlhelp_basename = 'xspress3epics_doc'
