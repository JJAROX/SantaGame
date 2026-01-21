# 🎅 Santa's Delivery Rush

Zabawna gra platformowo-zręcznościowa stworzona w C++ z wykorzystaniem biblioteki SFML. Pomóż Mikołajowi nadrobić zaległości i dostarczyć je dzieciom na całym świecie, unikając przy tym złośliwych pułapek Grincha!

## 📖 Fabuła
Mikołaj nieco zbyt mocno "odpoczywał" w swojej fabryce prezentów. Czas ucieka, a sanie są puste! Leć i roznoś prezenty dzieciom do kominów po różnych miejsach na świecie. Uważaj – Grinch nie śpi i zrobi wszystko, by zepsuć te święta.

## 🎮 Mechanika Gry
Gra podzielona jest na dwa główne etapy:

* **Podróż Saniami (Platformówka/Side-scroller):**
    * **Cel:** Zrzucaj prezenty prosto do kominów.
    * **Przeszkody:** Unikaj fajerwerków i śnieżek rzucanych przez Grincha.
    * **Sabotaż:** Grinch podłożył bomby do Twoich sań! Jeśli wrzucisz je do komina – tracisz punkty.
    * **Kontratak:** Możesz zrzucić bombę prosto na Grincha, aby zyskać czasową odporność na fajerwerki.
    * **Postęp:** Każdy kontynent to nowy poziom z rosnącym poziomem trudności.
* **Walka z Grinchem (Ostateczny poziom):** Zręcznościowe walka z zielonym niszczycielem świąt.

## 🛠 Technologie
* **Język:** C++17
* **Grafika i Dźwięk:** SFML (Simple and Fast Multimedia Library)
* **System Budowania:** CMake
* **Architektura:** State Pattern (Zarządzanie stanami gry)

## 🚀 Wymagania
* Kompilator C++ (GCC, Clang lub MSVC)
* CMake (wersja 3.10 lub nowsza)
* Biblioteka SFML (Projekt skonfigurowany jest tak, aby pobrać ją automatycznie przy budowaniu przez CMake)

## 📥 Jak pobrać i uruchomić

### Opcja 1: Przez Terminal (Zalecane dla macOS/Linux)

1.  **Pobierz kod:** Sklonuj repozytorium lub pobierz i rozpakuj plik ZIP.
2.  **Otwórz terminal** w głównym folderze projektu.
3.  **Wpisz poniższe komendy**, aby zbudować grę:

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
    *(Uwaga: Na Windowsie zamiast `make` użyj `cmake --build .`)*

4.  **Uruchom grę:**
    * **macOS/Linux:** `./SantaGame`
    * **Windows:** `.\Debug\SantaGame.exe` (ścieżka może się różnić zależnie od kompilatora)

### Opcja 2: Visual Studio Code (Wygodne)

1.  Otwórz folder z grą w **VS Code**.
2.  Upewnij się, że masz zainstalowane rozszerzenia **C/C++** oraz **CMake Tools**.
3.  Zezwól na konfigurację projektu (VS Code zapyta o to automatycznie po otwarciu folderu).
4.  Wybierz zestaw kompilatora (Kit) na dolnym pasku (np. Clang na Macu).
5.  Kliknij przycisk **Build** na dolnym pasku, a następnie ikonę **Play (▶)**, aby zagrać.

### ⚠️ Rozwiązywanie problemów na Windows

Bedzie najprawdopodobniej potrzebna instalacja Cmake'a i GIT'a
Jeśli po uruchomieniu pliku `SantaGame.exe` nic się nie dzieje (konsola znika) lub pojawia się błąd o braku plików `.dll`, musisz ręcznie skopiować biblioteki do folderu z grą.

1.  Wejdź do folderu, gdzie zbudowała się gra (zazwyczaj `build/Debug`).
2.  Upewnij się, że znajdują się tam następujące pliki (jeśli nie, poszukaj ich w folderze `build` i skopiuj):
    * `sfml-graphics-d-2.dll`
    * `sfml-window-d-2.dll`
    * `sfml-system-d-2.dll`
    * `sfml-audio-d-2.dll`
    * `openal32.dll` (Kluczowe dla dźwięku!)
3.  Upewnij się, że w tym samym folderze znajduje się folder **`assets`** (z obrazkami i dźwiękami).
