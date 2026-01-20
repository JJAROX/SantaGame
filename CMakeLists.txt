cmake_minimum_required(VERSION 3.10)

# Nazwa Twojego projektu
project(SantaGame)

# Ustawiamy standard C++ na 17 (bezpieczny i nowoczesny)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Znajdujemy bibliotekę SFML zainstalowaną przez Homebrew
# Szukamy modułów: graphics, window, system (te są używane w twoim kodzie)
find_package(SFML 2.5 COMPONENTS graphics window system required)

# Dodajemy plik wykonywalny
add_executable(SantaGame main.cpp)

# Linkujemy biblioteki SFML do Twojego pliku wykonywalnego
target_link_libraries(SantaGame sfml-graphics sfml-window sfml-system)

# --- KOPIOWANIE ZASOBÓW (ASSETS) ---
# To jest kluczowe, aby gra widziała pliki png/ttf po uruchomieniu z folderu build.
# Kopiujemy wszystkie pliki .png, .jpg i .ttf do folderu, w którym powstaje plik wykonywalny.

file(GLOB ASSETS_FONTS "${CMAKE_CURRENT_SOURCE_DIR}/*.ttf")
file(GLOB ASSETS_IMGS_PNG "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
file(GLOB ASSETS_IMGS_JPG "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg")

file(COPY ${ASSETS_FONTS} ${ASSETS_IMGS_PNG} ${ASSETS_IMGS_JPG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
