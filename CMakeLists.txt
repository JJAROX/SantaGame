cmake_minimum_required(VERSION 3.10)
project(SantaGame)

# Standard C++
set(CMAKE_CXX_STANDARD 17)

# Dodanie ścieżki do plików nagłówkowych, żeby nie pisać długich include'ów
include_directories(include)

# Automatyczne znalezienie wszystkich plików .cpp w podfolderach
file(GLOB_RECURSE SOURCES "src/*.cpp")

# SFML - szukanie bibliotek
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Budowanie pliku wykonywalnego
add_executable(SantaGame ${SOURCES})

# Linkowanie bibliotek SFML
target_link_libraries(SantaGame sfml-graphics sfml-window sfml-system sfml-audio)