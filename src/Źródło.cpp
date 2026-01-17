#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <string>

enum StanGry { MENU, ROZGRYWKA, POZIOMY, SKLEP };

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
std::vector<float> predkosci_domek = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f };
std::vector<float> predkosci_fajerwerki_x = { 400.0f, 400.0f, 400.0f, 400.0f, 400.0f, 400.0f };
std::vector<zakres> zakresy_kominow =
{
    {0.23f, 0.39f}, // dom1
    {0.67f, 0.85f}, // dom2
    {0.73f, 0.92f}, // dom3
    {0.62f, 0.88f}  // dom4
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
    std::vector<sf::RectangleShape>& hitboxy, std::vector<sf::Sprite>& fajerwerki,
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

    // kurwa potrzebuje poziomow XDD
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int poziom = 1;
    int coiny = 0;            // coiny ktore mozna wydac oddzielnie
    int wrzuconePrezenty = 0; // licznik trafień w aktualnej rundzie
    int hp = 3;
    bool unlockedLevelInfo = false; // komunikat o poziomie
    sf::Clock infoTimer;     // czas wyswietlania tego komunikatu

    // tla i skala
    std::vector<sf::Texture> tla;
    std::vector<std::string> nazwy_tel = { "Europa.png", "pustynia.png", "las.png", "menu.png" };
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
    tlo1.setTexture(tla[poziom - 1]);
    sf::Vector2u tloSize = tla[poziom - 1].getSize();
    tlo1.setScale((float)szerokosc_okna / tloSize.x, (float)wysokosc_okna / tloSize.y);
    tlo1.setPosition(0.0f, 0.0f);
    tlo2.setTexture(tla[poziom - 1]);
    tlo2.setScale((float)szerokosc_okna / tloSize.x, (float)wysokosc_okna / tloSize.y);
    float szerokosc_tla = tlo1.getGlobalBounds().width;
    tlo2.setPosition(szerokosc_tla, 0.0f);
    float tlo1_x = 0.0f, tlo2_x = szerokosc_tla;
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
        if (i < tla.size())
        {
            p.obrazek.setTexture(tla[i]);
            sf::Vector2u texSize = tla[i].getSize();
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

    // tekstura mikolaja
    sf::Texture tekstura_mikolaj;
    tekstura_mikolaj.loadFromFile("mikolajp1.png");

    sf::Font czcionka_game;
    czcionka_game.loadFromFile("ByteBounce.ttf");

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

    // przyciski w menu
    std::vector<std::string> nazwyPrzycisków = { "ZAGRAJ", "POZIOMY", "SKLEP", "WYJDZ" };
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
    std::vector<std::string> pliki_p2 = { "pustynia_dom1.png", "pustynia_dom2.png", "pustynia_dom3.png", "pustynia_dom4.png" };

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
    float cooldown_prezent = 1.0f;
    float cooldown_resp_domku = 3.5f;
    float cooldown_resp_fajerwerek = 5.0f;
    float czas_nietykalnosci = 1.0f;

    // mikolaj
    m k1 = { 365.0f, 155.0f };
    sf::Sprite mikolaj;
    mikolaj.setTexture(tekstura_mikolaj);
    sf::Vector2u texSize = tekstura_mikolaj.getSize();
    mikolaj.setPosition(szerokosc_okna / 8, wysokosc_okna / 3);
    float scaleX = k1.szerokosc / texSize.x;
    float scaleY = k1.wysokosc / texSize.y;
    mikolaj.setScale(scaleX, scaleY);

    // wiele prezentów
    std::vector<sf::Sprite> prezenty;

    //wiele domkow
    std::vector<sf::Sprite> domki;

    //hitboxy domkow
    std::vector<sf::RectangleShape> hitboxy;

    //wiele fajerwerek 
    std::vector<sf::Sprite> fajerwerki;

    //wykrzykniki
    std::vector<ostrzerzenie> ostrzerzenia;

    //serca dodawanie odejmowanie i punkty
    std::vector<sf::Sprite> serca;

    while (window.isOpen())
    {
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
                                resetGierki(hp, wrzuconePrezenty, mikolaj, prezenty, domki, hitboxy, fajerwerki, ostrzerzenia, serca, tlo1_x, tlo2_x, szerokosc_tla, aktualnyStan, clock, cooldown_domku, cooldown_fajerwerek, szerokosc_okna, wysokosc_okna, unlockedLevelInfo);
                            else if (i == 1)
                                aktualnyStan = POZIOMY;
                            else if (i == 2)
                                aktualnyStan = SKLEP;
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
                            tlo1.setTexture(tla[poziom - 1]);
                            tlo2.setTexture(tla[poziom - 1]);
                            setSpeed(poziom, predkosc_mikolaj_y, predkosc_prezent_x, predkosc_prezent_y, predkosc_domek_x, predkosc_fajerwerek_x);
                            resetGierki(hp, wrzuconePrezenty, mikolaj, prezenty, domki, hitboxy, fajerwerki, ostrzerzenia, serca, tlo1_x, tlo2_x, szerokosc_tla, aktualnyStan, clock, cooldown_domku, cooldown_fajerwerek, szerokosc_okna, wysokosc_okna, unlockedLevelInfo);
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

            tlo1_x -= predkosc_tla * dt;
            tlo2_x -= predkosc_tla * dt;
            if (tlo1_x + szerokosc_tla <= 0.0f)
                tlo1_x = tlo2_x + szerokosc_tla - 5.0f;
            if (tlo2_x + szerokosc_tla <= 0.0f)
                tlo2_x = tlo1_x + szerokosc_tla - 5.0f;
            tlo1.setPosition(tlo1_x, 0.0f);
            tlo2.setPosition(tlo2_x, 0.0f);
            // ruch mikolaja
            sf::Vector2f pos = mikolaj.getPosition();
            sf::FloatRect bounds = mikolaj.getGlobalBounds();
            sf::Vector2f newPos = pos;

            if (window.hasFocus())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    newPos.y -= predkosc_mikolaj_y * dt;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    newPos.y += predkosc_mikolaj_y * dt;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                    newPos.x -= predkosc_mikolaj_y * dt;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    newPos.x += predkosc_mikolaj_y * dt;
            }
            // ograniczenia
            if (newPos.x < 0)
                newPos.x = 0;
            if (newPos.x + bounds.width > szerokosc_okna)
                newPos.x = szerokosc_okna - bounds.width;
            if (newPos.y < 0)
                newPos.y = 0;
            if (newPos.y + bounds.height > wysokosc_okna * 0.6f)
                newPos.y = wysokosc_okna * 0.6f - bounds.height;

            mikolaj.setPosition(newPos);

            // tworzenie nowego prezentu + cooldown

            if (window.hasFocus())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldown.getElapsedTime().asSeconds() >= cooldown_prezent)
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

            // aktualizacja prezentow
            for (auto& p : prezenty)
            {
                p.move(predkosc_prezent_x * dt, predkosc_prezent_y * dt);
            }

            // usuwanie prezentow na dole okna
            prezenty.erase(std::remove_if(prezenty.begin(), prezenty.end(), [&](sf::Sprite& p)
                {
                    if (p.getPosition().y + p.getGlobalBounds().height >= wysokosc_okna) return true;
                    for (auto& h : hitboxy) {
                        if (p.getGlobalBounds().intersects(h.getGlobalBounds())) {
                            wrzuconePrezenty++; // trafienie w komin daje postęp
                            // szybki check czy odblokowales poizom
                            if (wrzuconePrezenty == poziom * 10) {
                                unlockedLevelInfo = true;
                                infoTimer.restart();
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
                if (domki.back().getPosition().x > szerokosc_okna - 350.0f) {
                    bezpiecznyOdstep = false;
                }
            }
            
            //tworzenie domku + cooldown + hitbox komina
            // respimy tylko gdy minął czas i jest miejsce (bezpiecznyOdstep)
            if (cooldown_domku.getElapsedTime().asSeconds() >= cooldown_resp_domku && bezpiecznyOdstep)
            {
                sf::Sprite domek;
                int index = rand() % 4;

                int indexPoziomu = poziom - 1;
                if (indexPoziomu >= 0 && indexPoziomu < tekstury_domkow_na_poziomy.size()) {
                    domek.setTexture(tekstury_domkow_na_poziomy[indexPoziomu][index]);

                    //skalowanie
                    sf::Vector2u domek_size = tekstury_domkow_na_poziomy[indexPoziomu][index].getSize();
                    domek.setScale(250.0f / domek_size.x, 250.0f / domek_size.y);
                    domek.setPosition(szerokosc_okna, wysokosc_okna - 220.0f);
                    domki.push_back(domek);

                    sf::RectangleShape hitbox;
                    float hitbox_start = zakresy_kominow[index % 4].start * (domek_size.x * (250.0f / domek_size.x));
                    float hitbox_end = zakresy_kominow[index % 4].koniec * (domek_size.x * (250.0f / domek_size.x));
                    hitbox.setSize(sf::Vector2f(hitbox_end - hitbox_start, wysokosc_hitboxa));
                    hitbox.setPosition(domek.getPosition().x + hitbox_start, domek.getPosition().y - 5.0f);
                    hitbox.setFillColor(sf::Color::Black);
                    hitboxy.push_back(hitbox);
                }

                cooldown_domku.restart();

                cooldown_resp_domku = 4.0f + 4.0f * (rand() / (float)RAND_MAX);
            }

            //aktualizacja wszystkich domkow
            for (auto& d : domki)
                d.move(-predkosc_domek_x * dt, 0);
            for (auto& h : hitboxy)
                h.move(-predkosc_domek_x * dt, 0);

            domki.erase(std::remove_if(domki.begin(), domki.end(), [&](sf::Sprite& d)
                { return d.getPosition().x + d.getGlobalBounds().width < 0; }),
                domki.end());
            hitboxy.erase(std::remove_if(hitboxy.begin(), hitboxy.end(), [&](sf::RectangleShape& h)
                { return h.getPosition().x + h.getSize().x < 0; }),
                hitboxy.end());

            //tworzenie wykrzyknikow do fajerwerek + cooldown
            if (cooldown_fajerwerek.getElapsedTime().asSeconds() >= cooldown_resp_fajerwerek)
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

            window.draw(tlo1);
            window.draw(tlo2);

            //odejmowanie hp podczas kolizji
            for (auto it = fajerwerki.begin(); it != fajerwerki.end();)
            {
                if (mikolaj.getGlobalBounds().intersects((*it).getGlobalBounds()))
                {
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

            //rysowanie wykrzyknikow
            for (auto& o : ostrzerzenia)
                window.draw(o.wykrzyknik);

            // rysowanie wszystkich fajerwerek
            for (auto& f : fajerwerki)
                window.draw(f);

            //for (auto& h : hitboxy)
              //window.draw(h);
            // rysowanie serc
            for (auto& s : serca)
                window.draw(s);
            if (newPos.y + bounds.height == wysokosc_okna * 0.6f)
                window.draw(napis_lot);
            window.draw(mikolaj);

            // ui coinuf i postepu
            tekst_coinow.setString("Coiny: " + std::to_string(coiny));
            window.draw(tekst_coinow);

            int cel = poziom * 10;
            int zostalo = cel - wrzuconePrezenty;
            if (zostalo < 0)
                zostalo = 0;
            tekst_celu.setString("Cel: " + std::to_string(zostalo) + " / " + std::to_string(cel));
            if (zostalo == 0)
                tekst_celu.setFillColor(sf::Color::Green);
            else
                tekst_celu.setFillColor(sf::Color::Cyan);
            window.draw(tekst_celu);

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
