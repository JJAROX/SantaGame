#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

enum StanGry { MENU, ROZGRYWKA, POZIOMY, SKLEP, JAK_GRAC };

struct m
{
    float szerokosc;
    float wysokosc;
};
struct ostrzerzenie
{
    sf::Sprite wykrzyknik;
    sf::Clock Timer;
    bool wystrzelono = false;
};
struct zakres
{
    float start;
    float koniec;
};
struct Poziom
{
    sf::RectangleShape ramka;
    sf::Sprite obrazek;
    int numer_poziomu;
    bool odblokowany;
    int max_prezenty = 0; // rekord wrzuconych prezentów na tym poziomie
};
struct Powerup
{
    std::string nazwa; // np. "Spowolnienie fajerwerek", "Tarcza", "Punkty x2"
    float cena;        // koszt powerupa
    bool aktywny;      // czy jest aktualnie kupiony/włączony
};

// do wektorow(!!!dla kciuka!!!) - kazdy indeks to nowy poziom, jak chcesz zmienic predkosc jakiegos obiektu to tutaj
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

std::vector<float> predkosci_mikolaj = { 200.0f, 200.0f, 200.0f, 200.0f, 200.0f, 200.0f };
std::vector<float> predkosci_prezent_x = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
std::vector<float> predkosci_prezent_y = { 200.0f, 200.0f, 200.0f, 200.0f, 200.0f, 200.0f };
std::vector<float> predkosci_domek = { 100.0f, 100.0f, 300.0f, 100.0f, 100.0f, 100.0f };
std::vector<float> predkosci_fajerwerki_x = { 400.0f, 400.0f, 400.0f, 400.0f, 400.0f, 400.0f };
float predkosc_grincha = 250.f;
std::vector<std::vector<zakres>> zakresy_kominow_na_poziomy =
{
    {{0.23f, 0.39f}, {0.67f, 0.85f}, {0.73f, 0.92f}, {0.62f, 0.88f}},//poziom 1
    {{0.62f, 0.9f}, {0.62f, 0.9f}, {0.48f, 0.77f}, {0.0f, 0.0f}},//poziom 2
    {{0.2f, 0.5f}, {0.2f, 0.5f}, {0.2f, 0.5f}, {0.2f, 0.5f}},//poziom 3
};
std::vector<int> ilosc_fajerwerek = { 2, 2, 3, 3, 3 };
int wysokosc_hitboxa = 10.0f;

void setSpeed(int poziom, float& predkosc_mikolaj_y, float& predkosc_prezent_x, float& predkosc_prezent_y, float& predkosc_domek_x, float& predkosc_fajerwerek_x)
{
    int index = poziom - 1;
    if (index <= 5)
    {
        predkosc_mikolaj_y = predkosci_mikolaj[index];
        predkosc_prezent_x = predkosci_prezent_x[index];
        predkosc_prezent_y = predkosci_prezent_y[index];
        predkosc_domek_x = predkosci_domek[index];
        predkosc_fajerwerek_x = predkosci_fajerwerki_x[index];
    }
    else
    {
        std::cout << "za duzy index, cos jest nie tak z poziomami" << std::endl;
    }
}

// reset giery - zeruje tylko postęp rundy (wrzuconePrezenty), coiny zostają
void resetGierki(
    int& hp, int& wrzuconePrezenty, sf::Sprite& mikolaj,
    std::vector<sf::Sprite>& prezenty, std::vector<sf::Sprite>& domki,
    std::vector<sf::RectangleShape>& hitboxy, std::vector<bool>& trafione_kominy, std::vector<sf::Sprite>& fajerwerki,
    std::vector<ostrzerzenie>& ostrzerzenia, std::vector<sf::Sprite>& serca,
    float& tlo1_x, float& tlo2_x, float szerokosc_tla,
    StanGry& aktualnyStan, sf::Clock& clock,
    sf::Clock& cooldown_domku, sf::Clock& cooldown_fajerwerek,
    int szerokosc_okna, int wysokosc_okna, bool& unlockedLevelInfo)
{
    hp = 3;
    wrzuconePrezenty = 0; // zerowanko trafienia w tej rundzie
    unlockedLevelInfo = false;
    mikolaj.setPosition(szerokosc_okna / 8.0f, wysokosc_okna / 3.0f);
    prezenty.clear();
    domki.clear();
    hitboxy.clear();
    trafione_kominy.clear();
    fajerwerki.clear();
    ostrzerzenia.clear();
    serca.clear();
    tlo1_x = 0.0f;
    tlo2_x = szerokosc_tla;
    aktualnyStan = ROZGRYWKA;
    clock.restart();
    cooldown_domku.restart();
    cooldown_fajerwerek.restart();
}

float predkosc_mikolaj_y, predkosc_prezent_x, predkosc_prezent_y, predkosc_domek_x, predkosc_fajerwerek_x;

int main()
{

    srand(time(NULL));
    int szerokosc_okna = 1200;
    int wysokosc_okna = 900;
    sf::RenderWindow window(sf::VideoMode(szerokosc_okna, wysokosc_okna), "SantaGame");

    // Głośność ogólna (0-100)
    float glosnosc_ogolna = 50.0f;

    sf::Music muzyka_tlo;
    muzyka_tlo.openFromFile("muzyka_tlo.ogg");
    muzyka_tlo.setLoop(true);
    muzyka_tlo.setVolume(glosnosc_ogolna * 0.2f);
    muzyka_tlo.play();

    sf::Music muzyka_boss;
    muzyka_boss.openFromFile("dzwiek_boss.ogg");
    muzyka_boss.setLoop(true);
    muzyka_boss.setVolume(glosnosc_ogolna * 0.06f);

    sf::SoundBuffer buffer_punkt;
    buffer_punkt.loadFromFile("dzwiek_punkt.ogg");
    sf::Sound dzwiek_punkt;
    dzwiek_punkt.setBuffer(buffer_punkt);
    dzwiek_punkt.setVolume(glosnosc_ogolna * 0.2f);

    sf::SoundBuffer buffer_odblokowanie;
    buffer_odblokowanie.loadFromFile("dzwiek_odblokowanie_poziomu.ogg");
    sf::Sound dzwiek_odblokowanie;
    dzwiek_odblokowanie.setBuffer(buffer_odblokowanie);
    dzwiek_odblokowanie.setVolume(glosnosc_ogolna * 0.2f);

    sf::SoundBuffer buffer_hitek;
    buffer_hitek.loadFromFile("dzwiek_hitek.ogg");
    sf::Sound dzwiek_hitek;
    dzwiek_hitek.setBuffer(buffer_hitek);
    dzwiek_hitek.setVolume(glosnosc_ogolna * 0.08f);

    sf::SoundBuffer buffer_hitek_grinch;
    buffer_hitek_grinch.loadFromFile("dzwiek_hitek_grinch.ogg");
    sf::Sound dzwiek_hitek_grinch;
    dzwiek_hitek_grinch.setBuffer(buffer_hitek_grinch);
    dzwiek_hitek_grinch.setVolume(glosnosc_ogolna * 0.08f);

    sf::SoundBuffer buffer_wygrana;
    buffer_wygrana.loadFromFile("dzwiek_wygrana.ogg");
    sf::Sound dzwiek_wygrana;
    dzwiek_wygrana.setBuffer(buffer_wygrana);
    dzwiek_wygrana.setVolume(glosnosc_ogolna * 0.2f);

    sf::SoundBuffer buffer_przegrana;
    buffer_przegrana.loadFromFile("dzwiek_przegrana.ogg");
    sf::Sound dzwiek_przegrana;
    dzwiek_przegrana.setBuffer(buffer_przegrana);
    dzwiek_przegrana.setVolume(glosnosc_ogolna * 0.2f);

    // kurwa potrzebuje poziomow XDD
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int poziom = 1;
    int coiny = 0;            // coiny ktore mozna wydac oddzielnie
    int wrzuconePrezenty = 0; // licznik trafień w aktualnej rundzie
    int hp = 3;
    int ilosc_trafien_w_grincha = 0;
    int ilosc_trafien_w_mikolaja = 0;
    bool unlockedLevelInfo = false; // komunikat o poziomie
    sf::Clock infoTimer;     // czas wyswietlania tego komunikatu
    bool aktualne_tlo_odwrocone = false;
    bool wygrana_poziom4 = false;
    sf::Clock wygranaTimer;
    bool przegrana_poziom4 = false;
    sf::Clock przegranaTimer;
    std::vector<bool> poprzedni_stan_odblokowania(4, false);

    // tla i skala
    std::vector<sf::Texture> tla;
    std::vector<std::string> nazwy_tel = { "Europa.png", "Europa_odw.png", "pustynia.png", "pustynia_odw.png", "las.png", "las_odw.png", "arena.jpg", "menu.png" };
    sf::Texture grafika;
    for (int i = 0; i < nazwy_tel.size(); i++)
    {
        grafika.loadFromFile(nazwy_tel[i]);
        tla.push_back(grafika);
    }
    /*
    grafika.loadFromFile("Ameryka.png");
    tla.push_back(grafika);
    grafika.loadFromFile("Azja.png");
    tla.push_back(grafika);
    grafika.loadFromFile("Biegun.png"); // O BOZE ALAAAAAAA ❤❤❤
    tla.push_back(grafika);*/

    sf::Sprite tlo1, tlo2, tlo_menu;
    int indeks_tla_start;
    if (poziom == 1)
        indeks_tla_start = 0;
    else if (poziom == 2)
        indeks_tla_start = 2;
    else if (poziom == 3)
        indeks_tla_start = 4;
    else if (poziom == 4)
        indeks_tla_start = 6;
    else
        indeks_tla_start = 0;
    tlo1.setTexture(tla[indeks_tla_start]);
    sf::Vector2u tloSize = tla[indeks_tla_start].getSize();
    tlo1.setScale(1200.0f / tloSize.x, 900.0f / tloSize.y);
    tlo1.setPosition(0.0f, 0.0f);
    tlo2.setTexture(tla[indeks_tla_start]);
    tlo2.setScale(1200.0f / tloSize.x, 900.0f / tloSize.y);
    float szerokosc_tla = 1200.0f;
    tlo2.setPosition(szerokosc_tla, 0.0f);
    float tlo1_x = 0.0f, tlo2_x = szerokosc_tla;
    aktualne_tlo_odwrocone = false;
    tlo_menu.setTexture(tla[tla.size() - 1]); // trzeba zrobic tak aby grafika pod menu byla zawsze ostatnia

    sf::Vector2u menuSize = tla[tla.size() - 1].getSize();
    tlo_menu.setScale((float)szerokosc_okna / menuSize.x, (float)wysokosc_okna / menuSize.y);

    setSpeed(poziom, predkosc_mikolaj_y, predkosc_prezent_x, predkosc_prezent_y, predkosc_domek_x, predkosc_fajerwerek_x);

    // Powerupy
    std::vector<Powerup> powerupy = {
        {"Spowolnienie fajerwerek", 30.0f, false},
        {"Tarcza", 25.0f, false},
        {"Coiny x2", 20.0f, false} };

    // Poziomy
    std::vector<Poziom> Poziomy;
    int kolumny = 3;
    int wiersze = 2;
    float margin = 50.f;
    float szerokosc_poziomu = 250.f;
    float wysokosc_poziomu = 180.f;

    for (int i = 0; i < 4; i++)
    {
        Poziom p;
        p.numer_poziomu = i + 1;
        p.odblokowany = (i == 0); // tylko pierwszy poziom odblokowany na start

        // ramka
        p.ramka.setSize(sf::Vector2f(szerokosc_poziomu, wysokosc_poziomu));
        p.ramka.setOutlineThickness(5.0f);
        p.ramka.setFillColor(sf::Color(50, 50, 50));

        // img pod poziom
        int indeks_ikony;
        if (i == 0)
            indeks_ikony = 0;
        else if (i == 1)
            indeks_ikony = 2;
        else if (i == 2)
            indeks_ikony = 4;
        else if (i == 3)
            indeks_ikony = 6;
        else
            indeks_ikony = 0;

        if (indeks_ikony < tla.size())
        {
            p.obrazek.setTexture(tla[indeks_ikony]);
            sf::Vector2u texSize = tla[indeks_ikony].getSize();
            p.obrazek.setScale(szerokosc_poziomu / texSize.x, wysokosc_poziomu / texSize.y);
        }

        // grid 
        int kol, rzad;
        if (i == 3)
        {
            kol = 1;
            rzad = 1;
        }
        else
        {
            kol = i % kolumny;
            rzad = i / kolumny;
        }

        float posX = (szerokosc_okna / 2.0f - (kolumny * (szerokosc_poziomu + margin)) / 2.0f) + kol * (szerokosc_poziomu + margin);
        float posY = (wysokosc_okna / 2.0f - (wiersze * (wysokosc_poziomu + margin)) / 2.0f) + rzad * (wysokosc_poziomu + margin);
        p.ramka.setPosition(posX, posY);
        p.obrazek.setPosition(posX, posY);
        Poziomy.push_back(p);
    }

    // inicjalizacja poprzedniego stanu odblokowania
    for (int i = 0; i < Poziomy.size(); i++)
    {
        poprzedni_stan_odblokowania[i] = Poziomy[i].odblokowany;
    }

    // tekstura mikolaja
    sf::Texture tekstura_mikolaj;
    tekstura_mikolaj.loadFromFile("mikolajp1.png");

    //czcionka game

    sf::Font czcionka_game;
    czcionka_game.loadFromFile("ByteBounce.ttf");

    //tekstura mikolaj minigierka

    sf::Texture tekstura_mikolajb;
    tekstura_mikolajb.loadFromFile("mikolajb.png");

    //tekstura grincha
    sf::Texture tekstura_grinch;
    tekstura_grinch.loadFromFile("grinch.png");


    // ui coinów 
    sf::Text tekst_coinow;
    tekst_coinow.setFont(czcionka_game);
    tekst_coinow.setCharacterSize(80);
    tekst_coinow.setFillColor(sf::Color::Yellow);
    tekst_coinow.setPosition(20.0f, -10.0f);
    tekst_coinow.setStyle(sf::Text::Bold);

    // ui celu (ile musi na ile trafic np: 10/10, 9/10 etc to gowno niebieskie czaisz???)
    sf::Text tekst_celu;
    tekst_celu.setFont(czcionka_game);
    tekst_celu.setCharacterSize(60);
    tekst_celu.setFillColor(sf::Color::Cyan);
    tekst_celu.setPosition(20.0f, 60.0f);

    sf::Text  tekst_trafien;
    tekst_trafien.setFont(czcionka_game);
    tekst_trafien.setCharacterSize(80);
    tekst_trafien.setFillColor(sf::Color::White);
    tekst_trafien.setPosition(szerokosc_okna / 2.0f, 130.0f);

    sf::Text tekst_wynik;
    tekst_wynik.setFont(czcionka_game);
    tekst_wynik.setString("TRAFIENIA MIKOLAJA : TRAFIENIA GRINCHA");
    tekst_wynik.setCharacterSize(60);
    tekst_wynik.setFillColor(sf::Color::White);
    tekst_wynik.setOutlineColor(sf::Color::Black);
    tekst_wynik.setOutlineThickness(1.0f);
    sf::FloatRect wynikBounds = tekst_wynik.getLocalBounds();
    tekst_wynik.setOrigin(wynikBounds.left + wynikBounds.width / 2.0f, wynikBounds.top + wynikBounds.height / 2.0f);
    tekst_wynik.setPosition(szerokosc_okna / 2.0f, 60.0f);

    // popup poziom odblokowany
    sf::Text tekst_info;
    tekst_info.setFont(czcionka_game);
    tekst_info.setString("POZIOM ODBLOKOWANY!");
    tekst_info.setCharacterSize(100);
    tekst_info.setFillColor(sf::Color::Green);
    tekst_info.setOutlineColor(sf::Color::Black);
    tekst_info.setOutlineThickness(3.0f);
    sf::FloatRect infoBounds = tekst_info.getLocalBounds();
    tekst_info.setOrigin(infoBounds.left + infoBounds.width / 2.0f, infoBounds.top + infoBounds.height / 2.0f);
    tekst_info.setPosition(szerokosc_okna / 2.0f, wysokosc_okna / 4.0f); // W górnej części ekranu

    sf::Text tekst_wygrana;
    tekst_wygrana.setFont(czcionka_game);
    tekst_wygrana.setString("WYGRANA");
    tekst_wygrana.setCharacterSize(120);
    tekst_wygrana.setFillColor(sf::Color::Green);
    tekst_wygrana.setOutlineColor(sf::Color::Black);
    tekst_wygrana.setOutlineThickness(5.0f);
    sf::FloatRect wygranaBounds = tekst_wygrana.getLocalBounds();
    tekst_wygrana.setOrigin(wygranaBounds.left + wygranaBounds.width / 2.0f, wygranaBounds.top + wygranaBounds.height / 2.0f);
    tekst_wygrana.setPosition(szerokosc_okna / 2.0f, wysokosc_okna / 2.0f - 60.0f);

    sf::Text tekst_pokonales;
    tekst_pokonales.setFont(czcionka_game);
    tekst_pokonales.setString("Pokonales grincha");
    tekst_pokonales.setCharacterSize(80);
    tekst_pokonales.setFillColor(sf::Color::White);
    tekst_pokonales.setOutlineColor(sf::Color::Black);
    tekst_pokonales.setOutlineThickness(3.0f);
    sf::FloatRect pokonalesBounds = tekst_pokonales.getLocalBounds();
    tekst_pokonales.setOrigin(pokonalesBounds.left + pokonalesBounds.width / 2.0f, pokonalesBounds.top + pokonalesBounds.height / 2.0f);
    tekst_pokonales.setPosition(szerokosc_okna / 2.0f, wysokosc_okna / 2.0f + 60.0f);

    sf::Text tekst_przegrana;
    tekst_przegrana.setFont(czcionka_game);
    tekst_przegrana.setString("PRZEGRANA");
    tekst_przegrana.setCharacterSize(120);
    tekst_przegrana.setFillColor(sf::Color::Red);
    tekst_przegrana.setOutlineColor(sf::Color::Black);
    tekst_przegrana.setOutlineThickness(5.0f);
    sf::FloatRect przegranaBounds = tekst_przegrana.getLocalBounds();
    tekst_przegrana.setOrigin(przegranaBounds.left + przegranaBounds.width / 2.0f, przegranaBounds.top + przegranaBounds.height / 2.0f);
    tekst_przegrana.setPosition(szerokosc_okna / 2.0f, wysokosc_okna / 2.0f - 60.0f);

    sf::Text tekst_grinch_pokonal;
    tekst_grinch_pokonal.setFont(czcionka_game);
    tekst_grinch_pokonal.setString("Grinch pokonal mikolaja");
    tekst_grinch_pokonal.setCharacterSize(80);
    tekst_grinch_pokonal.setFillColor(sf::Color::White);
    tekst_grinch_pokonal.setOutlineColor(sf::Color::Black);
    tekst_grinch_pokonal.setOutlineThickness(3.0f);
    sf::FloatRect grinchPokonalBounds = tekst_grinch_pokonal.getLocalBounds();
    tekst_grinch_pokonal.setOrigin(grinchPokonalBounds.left + grinchPokonalBounds.width / 2.0f, grinchPokonalBounds.top + grinchPokonalBounds.height / 2.0f);
    tekst_grinch_pokonal.setPosition(szerokosc_okna / 2.0f, wysokosc_okna / 2.0f + 60.0f);

    // przyciski w menu
    std::vector<std::string> nazwyPrzycisków = { "ZAGRAJ", "POZIOMY", "SKLEP", "JAK GRAC", "WYJDZ" };
    std::vector<sf::Text> przyciski;

    for (int i = 0; i < nazwyPrzycisków.size(); i++)
    {
        sf::Text p;
        p.setFont(czcionka_game);
        p.setString(nazwyPrzycisków[i]);
        p.setCharacterSize(50);
        p.setFillColor(sf::Color::White);

        // obramowanko
        p.setOutlineColor(sf::Color::Red);
        p.setOutlineThickness(2.0f);

        // wysrodkowanie tekstu (ktore jest rozjebane jak sie rozszerzy okienko, ale ogarniemy)
        sf::FloatRect bounds = p.getLocalBounds();
        p.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

        // ogolne wyliczanie pozycji w pionie, zeby byly zawsze mniej-wiecej wycentrowane
        float offsetY = (i - (nazwyPrzycisków.size() - 1) / 2.0f) * 130.0f;
        p.setPosition(szerokosc_okna / 2.0f, (wysokosc_okna / 2.0f) + offsetY);
        przyciski.push_back(p);
    }

    // przycisk w poziomy
    sf::Text poziomExit;
    poziomExit.setFont(czcionka_game);
    poziomExit.setString("WROC DO MENU");
    poziomExit.setCharacterSize(100);
    poziomExit.setFillColor(sf::Color::White);
    poziomExit.setOutlineColor(sf::Color::Red);
    poziomExit.setOutlineThickness(3.0f);
    sf::FloatRect szerokosc_exit = poziomExit.getLocalBounds();
    poziomExit.setOrigin(szerokosc_exit.left + szerokosc_exit.width / 2.0f, szerokosc_exit.top + szerokosc_exit.height / 2.0f);
    poziomExit.setPosition(szerokosc_okna * 0.5f, wysokosc_okna * 0.85f);

    // SKLEP UI
    // tytul "SKLEP" na gorze
    sf::Text sklepTytul;
    sklepTytul.setFont(czcionka_game);
    sklepTytul.setString("SKLEP");
    sklepTytul.setCharacterSize(80);
    sklepTytul.setFillColor(sf::Color::White);
    sklepTytul.setOutlineColor(sf::Color::Red);
    sklepTytul.setOutlineThickness(3.0f);
    {
        sf::FloatRect SklepTytul = sklepTytul.getLocalBounds();
        sklepTytul.setOrigin(SklepTytul.left + SklepTytul.width / 2.0f, SklepTytul.top + SklepTytul.height / 2.0f);
    }
    sklepTytul.setPosition(szerokosc_okna / 2.0f, 80.0f);

    // przyciski powerupow
    std::vector<sf::Text> przyciskiSklep;
    float sklepMarginY = 80.0f;

    // utworzenie pustych tekstow na powerupy (pozycjonowanie)
    for (int i = 0; i < powerupy.size(); ++i)
    {
        sf::Text textPowerup;
        textPowerup.setFont(czcionka_game);
        textPowerup.setCharacterSize(50);
        textPowerup.setFillColor(sf::Color::White);
        textPowerup.setOutlineColor(sf::Color::Red);
        textPowerup.setOutlineThickness(2.0f);
        float centerY = wysokosc_okna / 2.0f;
        float posY = centerY + (i - 1) * sklepMarginY; // i=1 -> srodek, 0 nad, 2 pod
        textPowerup.setPosition(szerokosc_okna / 2.0f, posY);
        przyciskiSklep.push_back(textPowerup);
    }

    // przycisk "Wyjdz do menu" na dole
    sf::Text sklepExit;
    sklepExit.setFont(czcionka_game);
    sklepExit.setString("WYJDZ DO MENU");
    sklepExit.setCharacterSize(60);
    sklepExit.setFillColor(sf::Color::White);
    sklepExit.setOutlineColor(sf::Color::Red);
    sklepExit.setOutlineThickness(3.0f);
    {
        sf::FloatRect exitBounds = sklepExit.getLocalBounds();
        sklepExit.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);
    }
    sklepExit.setPosition(szerokosc_okna / 2.0f, wysokosc_okna * 0.85f);

    // Przycisk powrotu dla ekranu "Jak grać"
    sf::Text jakGracExit;
    jakGracExit.setFont(czcionka_game);
    jakGracExit.setString("WROC DO MENU");
    jakGracExit.setCharacterSize(60);
    jakGracExit.setFillColor(sf::Color::White);
    jakGracExit.setOutlineColor(sf::Color::Red);
    jakGracExit.setOutlineThickness(3.0f);
    {
        sf::FloatRect exitBounds = jakGracExit.getLocalBounds();
        jakGracExit.setOrigin(exitBounds.left + exitBounds.width / 2.0f, exitBounds.top + exitBounds.height / 2.0f);
    }
    jakGracExit.setPosition(szerokosc_okna / 2.0f, wysokosc_okna - 150.0f);

    // Suwak głośności (zmienna już zdefiniowana wcześniej, używamy istniejącej)
    bool przeciaganie_suwaka = false;
    
    sf::RectangleShape suwak_tlo;
    suwak_tlo.setSize(sf::Vector2f(400.0f, 20.0f));
    suwak_tlo.setFillColor(sf::Color(100, 100, 100));
    suwak_tlo.setOutlineColor(sf::Color::White);
    suwak_tlo.setOutlineThickness(2.0f);
    suwak_tlo.setPosition(szerokosc_okna / 2.0f - 200.0f, wysokosc_okna - 200.0f);
    
    sf::RectangleShape suwak;
    suwak.setSize(sf::Vector2f(15.0f, 30.0f));
    suwak.setFillColor(sf::Color::Yellow);
    suwak.setOutlineColor(sf::Color::White);
    suwak.setOutlineThickness(2.0f);
    
    sf::Text tekst_glosnosc;
    tekst_glosnosc.setFont(czcionka_game);
    tekst_glosnosc.setString("GLOSNOSC");
    tekst_glosnosc.setCharacterSize(30);
    tekst_glosnosc.setFillColor(sf::Color::White);
    tekst_glosnosc.setOutlineColor(sf::Color::Black);
    tekst_glosnosc.setOutlineThickness(2.0f);
    sf::FloatRect glosnoscBounds = tekst_glosnosc.getLocalBounds();
    tekst_glosnosc.setOrigin(glosnoscBounds.left + glosnoscBounds.width / 2.0f, glosnoscBounds.top + glosnoscBounds.height / 2.0f);
    tekst_glosnosc.setPosition(szerokosc_okna / 2.0f, wysokosc_okna - 240.0f);

    // Teksty instrukcji
    std::vector<sf::Text> instrukcje;
    std::vector<std::string> tekstyInstrukcji = {
        "STEROWANIE W NORMALNEJ GRZE",
        "W/Strzalka w gore - lot do gory",
        "S/Strzalka w dol - lot w dol",
        "A/Strzalka w lewo - lot w lewo",
        "D/Strzalka w prawo - lot w prawo",
        "Spacja - rzut prezentem",
        "",
        "BOSS FIGHT",
        "W/Strzalka w gore - ruch do gory",
        "S/Strzalka w dol - ruch w dol",
        "Spacja - rzut sniezka"
    };
    
    for (size_t i = 0; i < tekstyInstrukcji.size(); i++)
    {
        sf::Text tekst;
        tekst.setFont(czcionka_game);
        tekst.setString(tekstyInstrukcji[i]);
        tekst.setCharacterSize(i == 0 || i == 7 ? 50 : 40);
        tekst.setFillColor(i == 0 || i == 7 ? sf::Color::Yellow : sf::Color::White);
        tekst.setOutlineColor(sf::Color::Black);
        tekst.setOutlineThickness(2.0f);
        
        sf::FloatRect bounds = tekst.getLocalBounds();
        tekst.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        tekst.setPosition(szerokosc_okna / 2.0f, 100.0f + i * 45.0f);
        instrukcje.push_back(tekst);
    }

    // helper do odswiezania napisow w sklepie
    auto odswiezTekstySklep = [&]()
        {
            for (int i = 0; i < powerupy.size(); ++i)
            {
                std::string label;
                if (powerupy[i].aktywny)
                    label = powerupy[i].nazwa + "  (ZAKUPIONO)";
                else
                    label = powerupy[i].nazwa + "  (" + std::to_string((int)powerupy[i].cena) + ")";
                przyciskiSklep[i].setString(label);
                sf::FloatRect b = przyciskiSklep[i].getLocalBounds();
                przyciskiSklep[i].setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            }
        };

    // powerupy sa tylko na jedna runde - na start wszystkie wylaczone w tekscie
    odswiezTekstySklep();

    auto resetPowerupyNaRunde = [&]()
        {
            for (auto& p : powerupy)
                p.aktywny = false;
            odswiezTekstySklep();
        };

    // ustawienie stanu gry na menu
    StanGry aktualnyStan = MENU;

    //napis przy lataniu za nisko
    sf::Text napis_lot;
    napis_lot.setFont(czcionka_game);
    napis_lot.setString("UWAZAJ LECISZ ZA NISKO");
    napis_lot.setCharacterSize(70);
    napis_lot.setFillColor(sf::Color::White);

    //ustawianie napisu na srodku
    sf::FloatRect textBounds = napis_lot.getLocalBounds();
    napis_lot.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    napis_lot.setPosition(szerokosc_okna / 2.0f, wysokosc_okna / 2.5f);

    //tekstura prezentu
    std::vector<std::string> obrazki_prezentow = { "prezent1.png", "prezent2.png", "prezent3.png", "prezent4.png", "prezent5.png", "prezent6.png", "prezent7.png", "prezent8.png" };
    std::vector<sf::Texture> tekstura_prezentu;
    for (auto& obrazek : obrazki_prezentow)
    {
        sf::Texture tekstura;
        tekstura.loadFromFile(obrazek);
        tekstura_prezentu.push_back(tekstura);
    }

    //sprawdzenie czy załadowało to chyba trzeba usunąć??? (działa bez tego bo sprawdziłem)
    /* {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        std::cout << buffer << std::endl;
        std::cout << "nie zaladowano tekstury prezentu" << std::endl;
        return 1;
    }*/

    // wektor wektorów - przechowuje zestawy tekstur dla każdego poziomu
    std::vector<std::vector<sf::Texture>> tekstury_domkow_na_poziomy;

    // pierwszy kontynent
    std::vector<std::string> pliki_p1 = { "dom1.png", "dom2.png", "dom3.png", "dom4.png" };

    // drugi kontynent
    std::vector<std::string> pliki_p2 = { "pustynia_dom1.png", "pustynia_dom2.png", "pustynia_dom3.png" };

    // trzeci kontynent
    std::vector<std::string> pliki_p3 = { "las_dom1.png", "las_dom2.png", "las_dom3.png", "las_dom4.png" };

    // czwarty kontynent ale wsm go nie ma i chyba tylko 3 robimy takze to sie wyjebie najwyzej
    std::vector<std::string> pliki_p4 = { "dom1.png", "dom2.png", "dom3.png", "dom4.png" };

    // funkcja do ładowania tekstur
    auto zaladujDomki = [&](std::vector<std::string> pliki)
        {
            std::vector<sf::Texture> temp_vec;
            for (auto& nazwa : pliki)
            {
                sf::Texture t;
                if (!t.loadFromFile(nazwa))
                {
                    std::cout << "Blad ladowania domku: " << nazwa << std::endl;
                }
                temp_vec.push_back(t);
            }
            tekstury_domkow_na_poziomy.push_back(temp_vec);
        };
    // ladowanie do glownego vectora
    zaladujDomki(pliki_p1); // indeks 0
    zaladujDomki(pliki_p2); // indeks 1
    zaladujDomki(pliki_p3); // indeks 2
    zaladujDomki(pliki_p4); // indeks 3

    //tekstura sniezki
    sf::Texture tekstura_sniezka;
    tekstura_sniezka.loadFromFile("sniezka.png");

    //tekstury fajerwerek
    std::vector<std::string> obrazki_fajerwerkow = { "fajerwerek1.png", "fajerwerek2.png", "fajerwerek3.png" };
    std::vector<sf::Texture> tekstury_fajerwerek;
    for (auto& obrazek : obrazki_fajerwerkow)
    {
        sf::Texture tekstura;
        tekstura.loadFromFile(obrazek);
        tekstury_fajerwerek.push_back(tekstura);
    }

    //tekstura wykrzyknika
    sf::Texture tekstura_wykrzyknik;
    tekstura_wykrzyknik.loadFromFile("wykrzyknik.png");

    //tekstury serc
    sf::Texture serce;
    sf::Texture puste_serce;
    serce.loadFromFile("serce_pelne.png");
    puste_serce.loadFromFile("serce_puste.png");

    //czasy
    sf::Clock clock;
    sf::Clock cooldown;
    sf::Clock cooldown_domku;
    sf::Clock cooldown_fajerwerek;
    sf::Clock cooldown_dmg;
    sf::Clock cooldown_sniezka;
    sf::Clock cooldown_grinch_fajerwerki;
    sf::Clock cooldown_grinch_kierunek;
    float cooldown_prezent = 1.0f;
    float cooldown_resp_domku = 3.5f;
    float cooldown_resp_fajerwerek = 5.0f;
    float czas_nietykalnosci = 1.0f;
    float cooldown_resp_sniezka = 2.0f;
    float cooldown_resp_sniezka_grinch = 1.5f;
    float czas_zmiany_kierunku_grinch = 7.0f + 3.0f * (rand() / (float)RAND_MAX);

    // mikolaj
    m k1 = { 365.0f, 155.0f };
    sf::Sprite mikolaj;
    mikolaj.setTexture(tekstura_mikolaj);
    sf::Vector2u texSize = tekstura_mikolaj.getSize();
    mikolaj.setPosition(szerokosc_okna / 8, wysokosc_okna / 3);
    float scaleX = k1.szerokosc / texSize.x;
    float scaleY = k1.wysokosc / texSize.y;
    mikolaj.setScale(scaleX, scaleY);

    // mikolaj mini gierka 2:3 skala
    m b1 = { 150.0f, 270.0f };
    sf::Sprite mikolajb;
    mikolajb.setTexture(tekstura_mikolajb);
    texSize = tekstura_mikolajb.getSize();
    mikolajb.setPosition(szerokosc_okna / 20, wysokosc_okna / 3);
    scaleX = b1.szerokosc / texSize.x;
    scaleY = b1.wysokosc / texSize.y;
    mikolajb.setScale(scaleX, scaleY);

    // grinch 3:2 skala
    m g1 = { 300.0f, 200.0f };
    sf::Sprite grinch;
    grinch.setTexture(tekstura_grinch);
    texSize = tekstura_grinch.getSize();
    grinch.setPosition(szerokosc_okna - 320.f, wysokosc_okna / 3);
    scaleX = g1.szerokosc / texSize.x;
    scaleY = g1.wysokosc / texSize.y;
    grinch.setScale(scaleX, scaleY);

    // wiele sniezek
    std::vector<sf::Sprite> sniezki;
    // wiele prezentów
    std::vector<sf::Sprite> prezenty;

    //wiele domkow
    std::vector<sf::Sprite> domki;

    //hitboxy domkow
    std::vector<sf::RectangleShape> hitboxy;
    std::vector<bool> trafione_kominy;

    //wiele fajerwerek 
    std::vector<sf::Sprite> fajerwerki;

    std::vector<sf::Sprite> fajerwerki_grinch;

    //wykrzykniki
    std::vector<ostrzerzenie> ostrzerzenia;

    //serca dodawanie odejmowanie i punkty
    std::vector<sf::Sprite> serca;

    while (window.isOpen())
    {
        if (window.hasFocus())
        {
            if (aktualnyStan == ROZGRYWKA && poziom == 4)
            {
                if (muzyka_tlo.getStatus() == sf::Music::Playing)
                    muzyka_tlo.pause();
                if (muzyka_boss.getStatus() != sf::Music::Playing)
                    muzyka_boss.play();
            }
            else
            {
                if (muzyka_boss.getStatus() == sf::Music::Playing)
                    muzyka_boss.stop();
                if (muzyka_tlo.getStatus() != sf::Music::Playing)
                    muzyka_tlo.play();
            }
        }
        else
        {
            if (muzyka_tlo.getStatus() == sf::Music::Playing)
                muzyka_tlo.pause();
            if (muzyka_boss.getStatus() == sf::Music::Playing)
                muzyka_boss.pause();
        }

        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (aktualnyStan == MENU)
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    for (int i = 0; i < przyciski.size(); i++)
                    {
                        if (przyciski[i].getGlobalBounds().contains(mousePosF))
                        {
                            if (i == 0) // pierwszy przycisk: ZAGRAJ
                            {
                                aktualne_tlo_odwrocone = false;
                                wygrana_poziom4 = false;
                                przegrana_poziom4 = false;
                                ilosc_trafien_w_grincha = 0;
                                ilosc_trafien_w_mikolaja = 0;
                                resetGierki(hp, wrzuconePrezenty, mikolaj, prezenty, domki, hitboxy, trafione_kominy, fajerwerki, ostrzerzenia, serca, tlo1_x, tlo2_x, szerokosc_tla, aktualnyStan, clock, cooldown_domku, cooldown_fajerwerek, szerokosc_okna, wysokosc_okna, unlockedLevelInfo);
                            }
                            else if (i == 1)
                                aktualnyStan = POZIOMY;
                            else if (i == 2)
                                aktualnyStan = SKLEP;
                            else if (i == 3)
                                aktualnyStan = JAK_GRAC;
                            else if (i == przyciski.size() - 1) // ostatni przycisk: WYJDZ
                                window.close();
                        }
                    }
                }
            }
            else if (aktualnyStan == POZIOMY)
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    if (poziomExit.getGlobalBounds().contains(mousePosF))
                        aktualnyStan = MENU;
                    for (auto& p : Poziomy)
                    {
                        if (p.ramka.getGlobalBounds().contains(mousePosF) && p.odblokowany)
                        {
                            poziom = p.numer_poziomu;
                            int indeks_tla;
                            if (poziom == 1)
                                indeks_tla = 0;
                            else if (poziom == 2)
                                indeks_tla = 2;
                            else if (poziom == 3)
                                indeks_tla = 4;
                            else if (poziom == 4)
                                indeks_tla = 6;
                            else
                                indeks_tla = 0;
                            tlo1.setTexture(tla[indeks_tla]);
                            tlo2.setTexture(tla[indeks_tla]);
                            sf::Vector2u tloSize = tla[indeks_tla].getSize();
                            tlo1.setScale(1200.0f / tloSize.x, 900.0f / tloSize.y);
                            tlo2.setScale(1200.0f / tloSize.x, 900.0f / tloSize.y);
                            szerokosc_tla = 1200.0f;
                            aktualne_tlo_odwrocone = false;
                            wygrana_poziom4 = false;
                            przegrana_poziom4 = false;
                            ilosc_trafien_w_grincha = 0;
                            ilosc_trafien_w_mikolaja = 0;
                            setSpeed(poziom, predkosc_mikolaj_y, predkosc_prezent_x, predkosc_prezent_y, predkosc_domek_x, predkosc_fajerwerek_x);
                            resetGierki(hp, wrzuconePrezenty, mikolaj, prezenty, domki, hitboxy, trafione_kominy, fajerwerki, ostrzerzenia, serca, tlo1_x, tlo2_x, szerokosc_tla, aktualnyStan, clock, cooldown_domku, cooldown_fajerwerek, szerokosc_okna, wysokosc_okna, unlockedLevelInfo);
                        }
                    }
                }
            }
            else if (aktualnyStan == SKLEP)
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    // klikniecie przyciskow powerupow
                    for (int i = 0; i < przyciskiSklep.size(); ++i)
                    {
                        if (przyciskiSklep[i].getGlobalBounds().contains(mousePosF))
                        {
                            if (!powerupy[i].aktywny && coiny >= powerupy[i].cena)
                            {
                                coiny -= static_cast<int>(powerupy[i].cena);
                                powerupy[i].aktywny = true;
                                odswiezTekstySklep();
                            }
                        }
                    }
                    // wyjscie do menu
                    if (sklepExit.getGlobalBounds().contains(mousePosF))
                        aktualnyStan = MENU;
                }
            }
            else if (aktualnyStan == JAK_GRAC)
            {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    if (jakGracExit.getGlobalBounds().contains(mousePosF))
                        aktualnyStan = MENU;
                    else if (suwak_tlo.getGlobalBounds().contains(mousePosF) || suwak.getGlobalBounds().contains(mousePosF))
                        przeciaganie_suwaka = true;
                }
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                {
                    przeciaganie_suwaka = false;
                }
            }
        }

        window.clear(sf::Color::Black);

        if (aktualnyStan == MENU)
        {
            // --- MENU ---
            window.draw(tlo_menu);
            for (auto& p : przyciski)
            {
                // hover na button
                if (p.getGlobalBounds().contains(mousePosF))
                {
                    p.setFillColor(sf::Color::Yellow);
                    p.setScale(1.7f, 1.7f);
                }
                else
                {
                    p.setFillColor(sf::Color::White);
                    p.setScale(1.5f, 1.5f);
                }
                window.draw(p);
            }
        }
        else if (aktualnyStan == POZIOMY)
        {   // --- POZIOMY ---

            // logika odblokowywania
            for (int i = 0; i < Poziomy.size(); i++)
            {
                if (i == 0)
                    Poziomy[i].odblokowany = true;
                else
                {
                    int wymagane = i * 10;
                    Poziomy[i].odblokowany = (Poziomy[i - 1].max_prezenty >= wymagane);
                }

                if (!poprzedni_stan_odblokowania[i] && Poziomy[i].odblokowany)
                {
                    dzwiek_odblokowanie.play();
                }

                poprzedni_stan_odblokowania[i] = Poziomy[i].odblokowany;

                if (Poziomy[i].odblokowany)
                {
                    Poziomy[i].ramka.setOutlineColor(sf::Color::White);
                    Poziomy[i].obrazek.setColor(sf::Color::White);
                }
                else
                {
                    Poziomy[i].ramka.setOutlineColor(sf::Color(128, 128, 128));
                    Poziomy[i].obrazek.setColor(sf::Color(100, 100, 100));
                }
            }

            window.draw(tlo_menu);
            for (auto& p : Poziomy)
            {
                // hover na ramkę
                if (p.odblokowany && p.ramka.getGlobalBounds().contains(mousePosF))
                    p.ramka.setOutlineColor(sf::Color::Yellow);
                window.draw(p.ramka);
                window.draw(p.obrazek);

                // tekst rekordu/wymagania
                sf::Text tekst_wyniku;
                tekst_wyniku.setFont(czcionka_game);
                std::string txt = "Rekord: " + std::to_string(p.max_prezenty);
                if (!p.odblokowany)
                    txt = "Wymaga: " + std::to_string(p.numer_poziomu * 10 - 10) + " (poprz.)";

                tekst_wyniku.setString(txt);
                tekst_wyniku.setCharacterSize(40);
                tekst_wyniku.setFillColor(p.odblokowany ? sf::Color::Green : sf::Color::Red);
                tekst_wyniku.setStyle(sf::Text::Bold);

                float tekst_x = p.ramka.getPosition().x + p.ramka.getSize().x / 2.0f;
                float tekst_y = p.ramka.getPosition().y + p.ramka.getSize().y + 25.0f;
                sf::FloatRect tekst_bounds = tekst_wyniku.getLocalBounds();
                tekst_wyniku.setOrigin(tekst_bounds.left + tekst_bounds.width / 2.0f, tekst_bounds.top + tekst_bounds.height / 2.0f);
                tekst_wyniku.setPosition(tekst_x, tekst_y);
                window.draw(tekst_wyniku);
            }
            if (poziomExit.getGlobalBounds().contains(mousePosF))
            {
                poziomExit.setFillColor(sf::Color::Yellow);
                poziomExit.setScale(1.1f, 1.1f);
            }
            else
            {
                poziomExit.setFillColor(sf::Color::White);
                poziomExit.setScale(1.0f, 1.0f);
            }
            window.draw(poziomExit);
        }
        else if (aktualnyStan == SKLEP)
        {
            // --- SKLEP ---
            window.draw(tlo_menu);


            // hover na przyciski powerupow
            for (auto& t : przyciskiSklep)
            {
                if (t.getGlobalBounds().contains(mousePosF))
                    t.setFillColor(sf::Color::Yellow);
                else
                    t.setFillColor(sf::Color::White);
                window.draw(t);
            }
            // hover na przycisk wyjscia
            if (sklepExit.getGlobalBounds().contains(mousePosF))
            {
                sklepExit.setFillColor(sf::Color::Yellow);
                sklepExit.setScale(1.2f, 1.2f);
            }
            else
            {
                sklepExit.setFillColor(sf::Color::White);
                sklepExit.setScale(1.0f, 1.0f);
            }
            window.draw(sklepTytul);
            window.draw(sklepExit);
            
            tekst_coinow.setString("Coiny: " + std::to_string(coiny));
            tekst_coinow.setPosition(szerokosc_okna - tekst_coinow.getGlobalBounds().width - 20.0f, 20.0f);
            window.draw(tekst_coinow);
        }
        else if (aktualnyStan == JAK_GRAC)
        {
            window.draw(tlo_menu);
            
            for (auto& tekst : instrukcje)
            {
                window.draw(tekst);
            }
            
            // Aktualizacja pozycji suwaka podczas przeciągania
            if (przeciaganie_suwaka)
            {
                float suwak_x = mousePosF.x;
                float suwak_min_x = suwak_tlo.getPosition().x;
                float suwak_max_x = suwak_tlo.getPosition().x + suwak_tlo.getSize().x;
                
                if (suwak_x < suwak_min_x)
                    suwak_x = suwak_min_x;
                if (suwak_x > suwak_max_x)
                    suwak_x = suwak_max_x;
                
                glosnosc_ogolna = ((suwak_x - suwak_min_x) / (suwak_max_x - suwak_min_x)) * 100.0f;
                
                // Aktualizacja głośności wszystkich dźwięków
                muzyka_tlo.setVolume(glosnosc_ogolna * 0.2f);
                muzyka_boss.setVolume(glosnosc_ogolna * 0.06f);
                dzwiek_punkt.setVolume(glosnosc_ogolna * 0.2f);
                dzwiek_odblokowanie.setVolume(glosnosc_ogolna * 0.2f);
                dzwiek_hitek.setVolume(glosnosc_ogolna * 0.08f);
                dzwiek_hitek_grinch.setVolume(glosnosc_ogolna * 0.08f);
                dzwiek_wygrana.setVolume(glosnosc_ogolna * 0.2f);
                dzwiek_przegrana.setVolume(glosnosc_ogolna * 0.2f);
            }
            
            // Aktualizacja pozycji suwaka na podstawie głośności
            float suwak_x = suwak_tlo.getPosition().x + (glosnosc_ogolna / 100.0f) * suwak_tlo.getSize().x;
            suwak.setPosition(suwak_x - suwak.getSize().x / 2.0f, suwak_tlo.getPosition().y - 5.0f);
            
            window.draw(tekst_glosnosc);
            window.draw(suwak_tlo);
            window.draw(suwak);
            
            if (jakGracExit.getGlobalBounds().contains(mousePosF))
            {
                jakGracExit.setFillColor(sf::Color::Yellow);
                jakGracExit.setScale(1.2f, 1.2f);
            }
            else
            {
                jakGracExit.setFillColor(sf::Color::White);
                jakGracExit.setScale(1.0f, 1.0f);
            }
            window.draw(jakGracExit);
        }
        else if (aktualnyStan == ROZGRYWKA)
        {
            // --- ROZGRYWKA ---

             //wyjscie do menu(pozdro Juras)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                int index_poziomu = poziom - 1;
                if (index_poziomu >= 0 && index_poziomu < Poziomy.size())
                {
                    if (wrzuconePrezenty > Poziomy[index_poziomu].max_prezenty)
                    {
                        Poziomy[index_poziomu].max_prezenty = wrzuconePrezenty;
                    }
                }

                resetPowerupyNaRunde();
                aktualnyStan = MENU;
            }
            float dt = clock.restart().asSeconds();
            //cala mechnika tla(ruch i zapetlanie)



            float predkosc_tla = 200.0f; //stala wartosc(tego nie zmieniamy w zaleznosci od poziomow)
            if (poziom == 4)
            {
                predkosc_tla = 0.f;
            }
            tlo1_x -= predkosc_tla * dt;
            tlo2_x -= predkosc_tla * dt;
            if (tlo1_x + szerokosc_tla <= 0.0f)
            {
                tlo1_x = tlo2_x + szerokosc_tla - 5.0f;
                if (poziom == 1 || poziom == 2 || poziom == 3)
                {
                    aktualne_tlo_odwrocone = !aktualne_tlo_odwrocone;
                    int indeks_tla;
                    if (poziom == 1)
                        indeks_tla = aktualne_tlo_odwrocone ? 1 : 0;
                    else if (poziom == 2)
                        indeks_tla = aktualne_tlo_odwrocone ? 3 : 2;
                    else if (poziom == 3)
                        indeks_tla = aktualne_tlo_odwrocone ? 5 : 4;
                    tlo1.setTexture(tla[indeks_tla]);
                    sf::Vector2u tloSize = tla[indeks_tla].getSize();
                    tlo1.setScale(1200.0f / tloSize.x, 900.0f / tloSize.y);
                    szerokosc_tla = 1200.0f;
                }
            }
            if (tlo2_x + szerokosc_tla <= 0.0f)
            {
                tlo2_x = tlo1_x + szerokosc_tla - 5.0f;
                if (poziom == 1 || poziom == 2 || poziom == 3)
                {
                    aktualne_tlo_odwrocone = !aktualne_tlo_odwrocone;
                    int indeks_tla;
                    if (poziom == 1)
                        indeks_tla = aktualne_tlo_odwrocone ? 1 : 0;
                    else if (poziom == 2)
                        indeks_tla = aktualne_tlo_odwrocone ? 3 : 2;
                    else if (poziom == 3)
                        indeks_tla = aktualne_tlo_odwrocone ? 5 : 4;
                    tlo2.setTexture(tla[indeks_tla]);
                    sf::Vector2u tloSize = tla[indeks_tla].getSize();
                    tlo2.setScale(1200.0f / tloSize.x, 900.0f / tloSize.y);
                    szerokosc_tla = 1200.0f;
                }
            }
            // ruch mikolaja

            sf::Vector2f pos = mikolaj.getPosition();
            sf::FloatRect bounds = mikolaj.getGlobalBounds();
            sf::Vector2f newPos = pos;
            float ograniczenie_wysokosc = mikolaj.getGlobalBounds().getSize().y;
            if (poziom == 4)
            {
                ograniczenie_wysokosc = mikolajb.getGlobalBounds().getSize().y;
                pos = mikolajb.getPosition();
                bounds = mikolajb.getGlobalBounds();
                newPos = pos;

            }
            if (window.hasFocus())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    newPos.y -= predkosc_mikolaj_y * dt;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    newPos.y += predkosc_mikolaj_y * dt;
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && poziom != 4)
                    newPos.x -= predkosc_mikolaj_y * dt;
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && poziom != 4)
                    newPos.x += predkosc_mikolaj_y * dt;
            }

            // ograniczenia
            int ograniczenie_x = 0, ograniczenie_y = 0;
            if (newPos.x < ograniczenie_x)
                newPos.x = ograniczenie_x;
            if (newPos.x + bounds.width > szerokosc_okna)
                newPos.x = szerokosc_okna - bounds.width;
            if (newPos.y < ograniczenie_y)
                newPos.y = ograniczenie_y;
            if (poziom != 4)
            {
                float max_y = wysokosc_okna * 0.65f;
                if (newPos.y + bounds.height > max_y)
                    newPos.y = max_y - bounds.height;
            }
            else
            {
                if (newPos.y + bounds.height > wysokosc_okna)
                    newPos.y = wysokosc_okna - ograniczenie_wysokosc;
            }

            poziom != 4 ? mikolaj.setPosition(newPos) : mikolajb.setPosition(newPos);
            //grinch ruch góra dół

            if (!wygrana_poziom4 && !przegrana_poziom4)
            {
                sf::Vector2f pos_grinch = grinch.getPosition();
                sf::FloatRect bounds_grinch = grinch.getGlobalBounds();
                sf::Vector2f newPos_grinch = pos_grinch;


                newPos_grinch.y += predkosc_grincha * dt;

                if (newPos_grinch.y <= ograniczenie_y)
                {
                    newPos_grinch.y = ograniczenie_y;
                    predkosc_grincha = 250.0f;
                }

                if (newPos_grinch.y + bounds_grinch.height >= wysokosc_okna)
                {
                    newPos_grinch.y = wysokosc_okna - bounds_grinch.height;
                    predkosc_grincha = -250.0f;
                }

                if (poziom == 4)
                {
                    if (cooldown_grinch_kierunek.getElapsedTime().asSeconds() >= czas_zmiany_kierunku_grinch)
                    {
                        predkosc_grincha = -predkosc_grincha;
                        czas_zmiany_kierunku_grinch = 7.0f + 3.0f * (rand() / (float)RAND_MAX);
                        cooldown_grinch_kierunek.restart();
                    }
                }
                grinch.setPosition(newPos_grinch);
            }

            if (poziom == 4 && !wygrana_poziom4 && !przegrana_poziom4)
            {
                if (cooldown_grinch_fajerwerki.getElapsedTime().asSeconds() >= cooldown_resp_sniezka_grinch)
                {
                    sf::Sprite fajerwerka;
                    fajerwerka.setTexture(tekstury_fajerwerek[2]);
                    sf::Vector2u fajerwerka_size = tekstury_fajerwerek[2].getSize();
                    fajerwerka.setScale(65.0f / fajerwerka_size.x, 40.0f / fajerwerka_size.y);
                    sf::FloatRect grinchBounds = grinch.getGlobalBounds();
                    fajerwerka.setPosition(grinchBounds.left + grinchBounds.width / 2.0f, grinchBounds.top + grinchBounds.height / 2.0f);
                    fajerwerki_grinch.push_back(fajerwerka);
                    cooldown_grinch_fajerwerki.restart();
                }
            }

            // tworzenie nowej sniezki + cooldown

            if (window.hasFocus())
            {
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldown_sniezka.getElapsedTime().asSeconds() >= cooldown_resp_sniezka) && poziom == 4)
                {
                    sf::Sprite nowa_sniezka;
                    nowa_sniezka.setTexture(tekstura_sniezka);
                    sf::Vector2u sniezka_size = tekstura_sniezka.getSize();
                    nowa_sniezka.setScale(45.0f / sniezka_size.x, 45.0f / sniezka_size.y);
                    nowa_sniezka.setPosition(pos.x, pos.y);
                    sniezki.push_back(nowa_sniezka);
                    cooldown_sniezka.restart();
                }
            }

            // tworzenie nowego prezentu + cooldown

            if (window.hasFocus())
            {
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldown.getElapsedTime().asSeconds() >= cooldown_prezent) && poziom != 4)
                {
                    sf::Sprite nowy;
                    int index = rand() % 8;
                    nowy.setTexture(tekstura_prezentu[index]);
                    sf::Vector2u prezent_Size = tekstura_prezentu[index].getSize();
                    nowy.setScale(70.0f / prezent_Size.x, 70.0f / prezent_Size.y);
                    nowy.setPosition(pos.x, pos.y + 53.0f);
                    prezenty.push_back(nowy);
                    cooldown.restart();
                }
            }
            //aktualizacja sniezek
            float predkosc_sniezka_x = 500.0f;
            for (auto& s : sniezki)
            {
                s.move(predkosc_sniezka_x * dt, 0.0f);
            }
            //usuwanie sniezek + sprawdzanie kolizji z grinchem

            sniezki.erase(std::remove_if(sniezki.begin(), sniezki.end(), [&](sf::Sprite& s)
                {
                    // sprawdzanie kolizji z grinchem
                    if (s.getGlobalBounds().intersects(grinch.getGlobalBounds()))
                    {
                        ilosc_trafien_w_grincha++;
                        dzwiek_hitek_grinch.play();
                        if (poziom == 4 && ilosc_trafien_w_grincha == 10 && !wygrana_poziom4)
                        {
                            wygrana_poziom4 = true;
                            wygranaTimer.restart();
                            muzyka_boss.stop();
                            dzwiek_wygrana.play();

                        }
                        return true; // usuń śnieżkę
                    }
                    // usuń jeśli poza ekranem
                    return s.getPosition().x + s.getGlobalBounds().width > szerokosc_okna;
                }),
                sniezki.end());

            // aktualizacja prezentow
            for (auto& p : prezenty)
            {
                p.move(predkosc_prezent_x * dt, predkosc_prezent_y * dt);
            }

            // usuwanie prezentow na dole okna
            prezenty.erase(std::remove_if(prezenty.begin(), prezenty.end(), [&](sf::Sprite& p)
                {
                    if (p.getPosition().y + p.getGlobalBounds().height >= wysokosc_okna) return true;
                    for (size_t i = 0; i < hitboxy.size(); i++) {
                        if (!trafione_kominy[i] && p.getGlobalBounds().intersects(hitboxy[i].getGlobalBounds())) {
                            trafione_kominy[i] = true;
                            wrzuconePrezenty++;
                            dzwiek_punkt.play();
                            if (wrzuconePrezenty == poziom * 10) {
                                unlockedLevelInfo = true;
                                infoTimer.restart();
                                dzwiek_odblokowanie.play();
                            }
                            // jesli powerup Punkty x2 aktywny, dodaj 2 punkty zamiast 1
                            if (powerupy[2].aktywny) coiny += 2; else coiny += 1; // i cyk coinik
                            return true;
                        }
                    }
                    return false; }),
                prezenty.end());

            // check czy ostatni domek odjechał wystarczająco daleko (zmienilem to bo byl problem z nowymi tekturami)
            bool bezpiecznyOdstep = true;
            if (!domki.empty()) {
                // jeśli ostatni dodany domek jest bliżej niż 350 pikseli od prawej krawędzi to jest waiting room na nastepny domek (chyba za dlugo ale to do testow)
                if (domki.back().getPosition().x > szerokosc_okna - 150.0f) {
                    bezpiecznyOdstep = false;
                }
            }

            //tworzenie domku + cooldown + hitbox komina
            // respimy tylko gdy minął czas i jest miejsce (bezpiecznyOdstep)
            if ((cooldown_domku.getElapsedTime().asSeconds() >= cooldown_resp_domku && bezpiecznyOdstep) && poziom != 4)
            {
                sf::Sprite domek;
                int indexPoziomu = poziom - 1;
                if (indexPoziomu >= 0 && indexPoziomu < tekstury_domkow_na_poziomy.size()) {
                    int liczba_tekstur = tekstury_domkow_na_poziomy[indexPoziomu].size();
                    int index = rand() % liczba_tekstur;

                    domek.setTexture(tekstury_domkow_na_poziomy[indexPoziomu][index]);

                    //skalowanie
                    sf::Vector2u domek_size = tekstury_domkow_na_poziomy[indexPoziomu][index].getSize();
                    domek.setScale(250.0f / domek_size.x, 250.0f / domek_size.y);
                    domek.setPosition(szerokosc_okna, wysokosc_okna - 230.0f);
                    domki.push_back(domek);

                    sf::RectangleShape hitbox;
                    int zakres_index = index;
                    if (zakres_index >= zakresy_kominow_na_poziomy[indexPoziomu].size())
                        zakres_index = zakresy_kominow_na_poziomy[indexPoziomu].size() - 1;
                    float hitbox_start = zakresy_kominow_na_poziomy[indexPoziomu][zakres_index].start * (domek_size.x * (250.0f / domek_size.x));
                    float hitbox_end = zakresy_kominow_na_poziomy[indexPoziomu][zakres_index].koniec * (domek_size.x * (250.0f / domek_size.x));
                    hitbox.setSize(sf::Vector2f(hitbox_end - hitbox_start, wysokosc_hitboxa));
                    hitbox.setPosition(domek.getPosition().x + hitbox_start, domek.getPosition().y - 5.0f);
                    hitbox.setFillColor(sf::Color::Black);
                    hitboxy.push_back(hitbox);
                    trafione_kominy.push_back(false);
                }

                cooldown_domku.restart();

                if (poziom == 1)
                    cooldown_resp_domku = 3.5f + 4.0f * (rand() / (float)RAND_MAX); // 4-8 sekund
                else if (poziom == 2)
                    cooldown_resp_domku = 2.5f + 3.0f * (rand() / (float)RAND_MAX); // 3-6 sekund
                else if (poziom == 3)
                    cooldown_resp_domku = 1.5f + 2.0f * (rand() / (float)RAND_MAX); // 2-4 sekundy
            }

            //aktualizacja wszystkich domkow
            for (auto& d : domki)
                d.move(-predkosc_domek_x * dt, 0);
            for (auto& h : hitboxy)
                h.move(-predkosc_domek_x * dt, 0);

            domki.erase(std::remove_if(domki.begin(), domki.end(), [&](sf::Sprite& d)
                { return d.getPosition().x + d.getGlobalBounds().width < 0; }),
                domki.end());

            auto it = hitboxy.begin();
            auto it_flag = trafione_kominy.begin();
            while (it != hitboxy.end()) {
                if (it->getPosition().x + it->getSize().x < 0) {
                    it = hitboxy.erase(it);
                    it_flag = trafione_kominy.erase(it_flag);
                }
                else {
                    ++it;
                    ++it_flag;
                }
            }

            //tworzenie wykrzyknikow do fajerwerek + cooldown
            if ((cooldown_fajerwerek.getElapsedTime().asSeconds() >= cooldown_resp_fajerwerek) && poziom != 4)
            {
                for (int i = 0; i < ilosc_fajerwerek[poziom - 1]; i++)
                {
                    float fajerwerka_y = rand() % 501 + 100;
                    ostrzerzenie o;
                    o.wykrzyknik.setTexture(tekstura_wykrzyknik);
                    o.wykrzyknik.setPosition(szerokosc_okna - 50, fajerwerka_y);
                    sf::Vector2u wykrzyknik_size = tekstura_wykrzyknik.getSize();
                    o.wykrzyknik.setScale(70.0f / wykrzyknik_size.x, 70.0f / wykrzyknik_size.y);
                    o.Timer.restart();
                    ostrzerzenia.push_back(o);
                }
                cooldown_fajerwerek.restart();
                cooldown_resp_fajerwerek = 10.0f + 5.0f * (rand() / (float)RAND_MAX);
            }

            //fajerwerki
            for (auto& o : ostrzerzenia)
            {
                if (o.Timer.getElapsedTime().asSeconds() >= 1.0f && !o.wystrzelono)
                {
                    sf::Sprite fajerwerka;
                    int index = rand() % 3;
                    fajerwerka.setTexture(tekstury_fajerwerek[index]);

                    //skalowanie
                    sf::Vector2u fajerwerka_size = tekstury_fajerwerek[index].getSize();
                    fajerwerka.setScale(65.0f / fajerwerka_size.x, 40.0f / fajerwerka_size.y);
                    fajerwerka.setPosition(szerokosc_okna + 1, o.wykrzyknik.getPosition().y);
                    fajerwerki.push_back(fajerwerka);
                    o.wystrzelono = true;
                }
            }
            ostrzerzenia.erase(std::remove_if(ostrzerzenia.begin(), ostrzerzenia.end(), [&](ostrzerzenie& o)
                { return o.Timer.getElapsedTime().asSeconds() >= 1.0f; }),
                ostrzerzenia.end());


            //aktualizacja wszystkich fajerwerek
            float f_speed = powerupy[0].aktywny ? predkosc_fajerwerek_x * 0.6f : predkosc_fajerwerek_x; // jesli aktywne spowolnienie fajerwerek - lecą 40% wolniej
            for (auto& f : fajerwerki)
                f.move(-f_speed * dt, 0);
            fajerwerki.erase(std::remove_if(fajerwerki.begin(), fajerwerki.end(), [&](sf::Sprite& f)
                { return f.getPosition().x + f.getGlobalBounds().width < 0; }),
                fajerwerki.end());

            float f_speed_grinch = predkosc_fajerwerek_x;
            for (auto& f : fajerwerki_grinch)
                f.move(-f_speed_grinch * dt, 0);
            fajerwerki_grinch.erase(std::remove_if(fajerwerki_grinch.begin(), fajerwerki_grinch.end(), [&](sf::Sprite& f)
                { return f.getPosition().x + f.getGlobalBounds().width < 0; }),
                fajerwerki_grinch.end());

            //rysowanie serc z odpowiednią teksturą
            serca.clear();
            for (int i = 0; i < 3; i++)
            {
                sf::Sprite s;
                if (hp >= i + 1)
                    s.setTexture(serce);
                else
                    s.setTexture(puste_serce);
                s.setScale(60.0f / serce.getSize().x, 60.0f / serce.getSize().y);
                s.setPosition(szerokosc_okna - (i + 1) * 70.0f, 20.0f);
                serca.push_back(s);
            }

            //koniec programu
            if (window.hasFocus())
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                    window.close();

            tlo1.setPosition(tlo1_x, 0.0f);
            tlo2.setPosition(tlo2_x, 0.0f);
            window.draw(tlo1);
            window.draw(tlo2);

            //odejmowanie hp podczas kolizji
            sf::FloatRect playerBounds = (poziom != 4 ? mikolaj : mikolajb).getGlobalBounds();
            for (auto it = fajerwerki.begin(); it != fajerwerki.end();)
            {
                if (playerBounds.intersects((*it).getGlobalBounds()))
                {
                    dzwiek_hitek.play();
                    if (cooldown_dmg.getElapsedTime().asSeconds() >= czas_nietykalnosci)
                    {
                        // jesli aktywna tarcza - usun tarcze, zostaw hp
                        if (powerupy[1].aktywny)
                        {
                            powerupy[1].aktywny = false;
                            odswiezTekstySklep();
                        }
                        else
                        {
                            hp--;
                        }
                        cooldown_dmg.restart();
                    }
                    it = fajerwerki.erase(it);
                }
                else
                    it++;
            }

            for (auto it = fajerwerki_grinch.begin(); it != fajerwerki_grinch.end();)
            {
                if (playerBounds.intersects((*it).getGlobalBounds()))
                {
                    ilosc_trafien_w_mikolaja++;
                    dzwiek_hitek.play();
                    if (poziom == 4 && ilosc_trafien_w_mikolaja == 10 && !przegrana_poziom4)
                    {
                        przegrana_poziom4 = true;
                        przegranaTimer.restart();
                        muzyka_boss.stop();
                        dzwiek_przegrana.play();

                    }
                    it = fajerwerki_grinch.erase(it);
                }
                else
                    it++;
            }

            if (hp == 0)
            {
                int index_poziomu = poziom - 1;

                // sprawdzam czy index jest poprawny żeby gra nie crashowała
                if (index_poziomu >= 0 && index_poziomu < Poziomy.size())
                {
                    if (wrzuconePrezenty > Poziomy[index_poziomu].max_prezenty)
                    {
                        Poziomy[index_poziomu].max_prezenty = wrzuconePrezenty;
                    }
                }
                else
                {
                    std::cout << "cos sie rozjebalo i zle zapisalo" << std::endl;
                }

                resetPowerupyNaRunde();
                aktualnyStan = MENU;
            }

            // rysowanie wszystkich prezentów
            for (auto& p : prezenty)
                window.draw(p);

            // rysowanie wszystkich domkow
            for (auto& d : domki)
                window.draw(d);
            // rysowanie wszystkich sniezek
            for (auto& s : sniezki)
                window.draw(s);
            //rysowanie wykrzyknikow
            for (auto& o : ostrzerzenia)
                window.draw(o.wykrzyknik);

            // rysowanie wszystkich fajerwerek
            for (auto& f : fajerwerki)
                window.draw(f);

            for (auto& f : fajerwerki_grinch)
                window.draw(f);

            //for (auto& h : hitboxy)
                //window.draw(h);

            if (poziom != 4)
            {
                for (auto& s : serca)
                    window.draw(s);
            }
            if (newPos.y + bounds.height == wysokosc_okna * 0.6f)
                window.draw(napis_lot);
            poziom != 4 ? window.draw(mikolaj) : window.draw(mikolajb);
            if (poziom == 4)
                window.draw(grinch);

            if (poziom == 4)
            {
                window.draw(tekst_wynik);
                tekst_trafien.setString(std::to_string(ilosc_trafien_w_grincha) + " : " + std::to_string(ilosc_trafien_w_mikolaja));
                tekst_trafien.setOutlineColor(sf::Color::Black);
                tekst_trafien.setOutlineThickness(1.0f);
                sf::FloatRect trafienBounds = tekst_trafien.getLocalBounds();
                tekst_trafien.setOrigin(trafienBounds.left + trafienBounds.width / 2.0f, trafienBounds.top + trafienBounds.height / 2.0f);
                window.draw(tekst_trafien);

                if (wygrana_poziom4)
                {
                    if (wygranaTimer.getElapsedTime().asSeconds() >= 5.0f)
                    {
                        resetPowerupyNaRunde();
                        aktualnyStan = MENU;
                        wygrana_poziom4 = false;
                        ilosc_trafien_w_grincha = 0;
                        ilosc_trafien_w_mikolaja = 0;
                    }
                    else
                    {
                        sf::RectangleShape tlo_wygrana;
                        tlo_wygrana.setSize(sf::Vector2f(szerokosc_okna, wysokosc_okna));
                        tlo_wygrana.setFillColor(sf::Color(0, 0, 0, 200));
                        tlo_wygrana.setPosition(0.0f, 0.0f);
                        window.draw(tlo_wygrana);
                        window.draw(tekst_wygrana);
                        window.draw(tekst_pokonales);
                    }
                }

                if (przegrana_poziom4)
                {
                    if (przegranaTimer.getElapsedTime().asSeconds() >= 5.0f)
                    {
                        resetPowerupyNaRunde();
                        aktualnyStan = MENU;
                        przegrana_poziom4 = false;
                        ilosc_trafien_w_grincha = 0;
                        ilosc_trafien_w_mikolaja = 0;
                    }
                    else
                    {
                        sf::RectangleShape tlo_przegrana;
                        tlo_przegrana.setSize(sf::Vector2f(szerokosc_okna, wysokosc_okna));
                        tlo_przegrana.setFillColor(sf::Color(0, 0, 0, 200));
                        tlo_przegrana.setPosition(0.0f, 0.0f);
                        window.draw(tlo_przegrana);
                        window.draw(tekst_przegrana);
                        window.draw(tekst_grinch_pokonal);
                    }
                }
            }

            if (poziom != 4)
            {
                tekst_coinow.setString("Coiny: " + std::to_string(coiny));
                tekst_coinow.setPosition(20.0f, -10.0f);
                window.draw(tekst_coinow);

                int cel = poziom * 10;
                tekst_celu.setString("Cel: " + std::to_string(wrzuconePrezenty) + " / " + std::to_string(cel));
                if (wrzuconePrezenty >= cel)
                    tekst_celu.setFillColor(sf::Color::Green);
                else
                    tekst_celu.setFillColor(sf::Color::Cyan);
                window.draw(tekst_celu);
            }

            // komunikacik o odblokowaniu poziomu zostal wyswietlony
            if (unlockedLevelInfo)
            {
                if (infoTimer.getElapsedTime().asSeconds() < 3.0f)
                {
                    window.draw(tekst_info);
                }
                else
                {
                    unlockedLevelInfo = false;
                }
            }
        }

        window.display();
    }
    return 0;
}
