cmake_minimum_required(VERSION 3.10)

# Nazwa Twojego projektu
project(SantaGame)

# Wymagany standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- POBIERANIE SFML 2.6.2 ---
# To zapewnia, że masz tę samą wersję co na Windowsie
include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.2
)

FetchContent_MakeAvailable(sfml)

# Dodajemy plik wykonywalny (Twój kod)
add_executable(SantaGame main.cpp)

# --- LINKOWANIE ---
# TU ZMIANA: Dodano sfml-audio
target_link_libraries(SantaGame PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# --- KOPIOWANIE ZASOBÓW (Fix dla Maca) ---
# Kopiujemy czcionki, obrazki ORAZ pliki dźwiękowe do folderu z grą

# 1. Znajdź pliki
file(GLOB ASSETS_FONTS "${CMAKE_CURRENT_SOURCE_DIR}/*.ttf")
file(GLOB ASSETS_IMGS_PNG "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
file(GLOB ASSETS_IMGS_JPG "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg")
# Nowe: Szukamy plików audio (.wav, .ogg, .mp3)
file(GLOB ASSETS_AUDIO "${CMAKE_CURRENT_SOURCE_DIR}/*.wav" "${CMAKE_CURRENT_SOURCE_DIR}/*.ogg" "${CMAKE_CURRENT_SOURCE_DIR}/*.mp3")

# 2. Skopiuj je
file(COPY ${ASSETS_FONTS} ${ASSETS_IMGS_PNG} ${ASSETS_IMGS_JPG} ${ASSETS_AUDIO} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
