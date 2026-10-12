project('ge', 'cpp')

geIncDirs = include_directories('./include')

geSrc = ['src/render.cpp', 'src/window.cpp']


cmake = import('cmake')

assimp_proj = cmake.subproject('assimp')
assimp_dep = assimp_proj.dependency('assimp')

glfw_proj = cmake.subproject('glfw')
glfw_dep = glfw_proj.dependency('glfw')

glad_proj = subproject('glad')
glad_dep = glad_proj.get_variable('glad')

glm_opts = cmake.subproject_options()
glm_opts.add_cmake_defines({
    'CMAKE_POSITION_INDEPENDENT_CODE' : true
})

glm_proj = cmake.subproject('glm', options: glm_opts)
glm_dep = glm_proj.dependency('glm')


geDeps = [glfw_dep, glad_dep, glm_dep, assimp_dep]
geLib = library('ge', geSrc, include_directories: geIncDirs, dependencies: geDeps)


ge = declare_dependency(link_with: geLib, include_directories: geIncDirs)
