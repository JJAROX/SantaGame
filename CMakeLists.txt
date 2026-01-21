cmake_minimum_required(VERSION 3.10)

# Nazwa Twojego projektu
project(SantaGame)

# Wymagany standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- POBIERANIE SFML 2.6.2 ---
include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.2
)

FetchContent_MakeAvailable(sfml)

# --- PLIK WYKONYWALNY ---
# ZMIANA: Wskazujemy, że plik jest w folderze src
add_executable(SantaGame src/main.cpp)

# Jeśli w przyszłości dodasz Game.cpp, zrób tak:
# add_executable(SantaGame src/main.cpp src/Game.cpp)

# --- LINKOWANIE ---
target_link_libraries(SantaGame PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# --- KOPIOWANIE ZASOBÓW ---
# Kopiujemy folder 'assets' z głównego katalogu do folderu build.
# Dzięki temu gra nadal będzie widzieć pliki jako "assets/..."
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
