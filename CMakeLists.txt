cmake_minimum_required(VERSION 3.21)
project(GestaoDeLoja LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# SQLite (amalgamation vendida em third_party/) -- compilada junto, vira parte
# do exe: zero dependencia de runtime para o banco.
add_library(sqlite3 STATIC third_party/sqlite/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/sqlite)

# Aplicacao desktop (WIN32 = sem janela de console no Windows).
add_executable(gestao-de-loja WIN32
    src/main.cpp
    src/Produto.cpp
    src/Util.cpp
    src/Loja.cpp
    src/Storage.cpp
    src/LoginDialog.cpp
    src/MainWindow.cpp
    include/LoginDialog.hpp   # listados p/ o AUTOMOC processar o Q_OBJECT
    include/MainWindow.hpp
)
target_include_directories(gestao-de-loja PRIVATE include)
target_link_libraries(gestao-de-loja PRIVATE Qt6::Widgets sqlite3)

# Teste headless do nucleo (sem Qt): regras de negocio + persistencia SQLite.
# So e adicionado quando o fonte do teste existe (ele nao e versionado).
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/selftest.cpp")
    add_executable(selftest
        tests/selftest.cpp
        src/Produto.cpp
        src/Util.cpp
        src/Loja.cpp
        src/Storage.cpp
    )
    target_include_directories(selftest PRIVATE include)
    target_link_libraries(selftest PRIVATE sqlite3)
endif()
