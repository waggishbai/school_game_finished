project('main', 'cpp', 'c')

ge = subproject('ge')
ge_dep = ge.get_variable('ge')
executable('main', 'tristage.cpp', dependencies: ge_dep)

