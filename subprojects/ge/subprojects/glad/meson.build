project('glad', 'c')

gladInc = include_directories('include')

gladLib = library('glad', 'src/glad.c', include_directories: gladInc)

glad = declare_dependency(link_with: gladLib, include_directories: gladInc)
