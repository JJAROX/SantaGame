cmake_minimum_required(VERSION 3.10)

# Nazwa Twojego projektu
project(SantaGame)

# Wymagany standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- KLUCZOWA ZMIANA: FetchContent ---
# Zamiast szukać biblioteki w systemie (find_package), pobieramy ją bezpośrednio.

include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.2  # Tu wymuszamy konkretną wersję!
)

# To sprawi, że SFML zostanie pobrany i dostępny dla Twojego projektu
FetchContent_MakeAvailable(sfml)

# Dodajemy plik wykonywalny
add_executable(SantaGame main.cpp)

# Linkujemy biblioteki (zwróć uwagę, że nazwy to teraz sfml-graphics itp., bez zmiennych)
target_link_libraries(SantaGame PRIVATE sfml-graphics sfml-window sfml-system)

# --- KOPIOWANIE ZASOBÓW (Fix dla Maca) ---
# Kopiuje obrazki do folderu, gdzie ląduje plik wykonywalny
file(GLOB ASSETS_FONTS "${CMAKE_CURRENT_SOURCE_DIR}/*.ttf")
file(GLOB ASSETS_IMGS_PNG "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
file(GLOB ASSETS_IMGS_JPG "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg")

file(COPY ${ASSETS_FONTS} ${ASSETS_IMGS_PNG} ${ASSETS_IMGS_JPG} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
