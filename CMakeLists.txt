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

# Dodajemy plik wykonywalny
add_executable(SantaGame main.cpp)

# --- LINKOWANIE ---
# Ważne: sfml-audio jest tutaj kluczowe
target_link_libraries(SantaGame PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# --- KOPIOWANIE ZASOBÓW ---
# To jest jedyna linijka, której potrzebujesz do zasobów.
# Kopiuje folder "assets" z kodem źródłowym do folderu "build", gdzie powstaje gra.
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
