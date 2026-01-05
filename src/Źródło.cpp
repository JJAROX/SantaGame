#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include <vector>
#include <algorithm>
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
    /*bool odblokowany;*/ // na razie komentuje po nwm czy dojdziemy do tego etapu ale mozna by bylo poziomy odblokowywac
};

//do wektorow(!!!dla kciuka!!!) - kazdy indeks to nowy poziom, jak chcesz zmienic predkosc jakiegos obiektu to tutaj
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

std::vector<float> predkosci_mikolaj = { 200.0f,200.0f,200.0f,200.0f,200.0f,200.0f };
std::vector<float> predkosci_prezent_x = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
std::vector<float> predkosci_prezent_y = { 200.0f,200.0f,200.0f,200.0f,200.0f,200.0f };
std::vector<float> predkosci_domek = { 100.0f,100.0f,100.0f,100.0f,100.0f,100.0f };
std::vector<float> predkosci_fajerwerki_x = { 400.0f,400.0f,400.0f,400.0f,400.0f,400.0f };
std::vector<zakres> zakresy_kominow =
{
    {0.20f, 0.33f}, // dom1
    {0.70f, 0.85f}, // dom2
    {0.75f, 0.91f}, // dom3
    {0.34f, 0.51f}, // dom4
    {0.76f, 0.94f}  // dom5
};
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
float predkosc_mikolaj_y, predkosc_prezent_x, predkosc_prezent_y, predkosc_domek_x, predkosc_fajerwerek_x;

enum StanGry { MENU, ROZGRYWKA, POZIOMY };

int main() {
    srand(time(NULL));
    int szerokosc_okna = 1200;
    int wysokosc_okna = 900;
    sf::RenderWindow window(sf::VideoMode(szerokosc_okna, wysokosc_okna), "SantaGame");
    //kurwa potrzebuje poziomow XDD
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int poziom = 1;
    int punkty = 0;
    int hp = 3;
    //tla i skala
    std::vector<sf::Texture> tla;
    std::vector<std::string> nazwy_tel = { "Europa.png" , "Afryka.png" , "menu.png" };
    sf::Texture grafika;
    for (int i = 0; i < nazwy_tel.size(); i++) {
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
    tlo_menu.setScale(
        (float)szerokosc_okna / menuSize.x,
        (float)wysokosc_okna / menuSize.y
    );

    setSpeed(poziom, predkosc_mikolaj_y, predkosc_prezent_x, predkosc_prezent_y, predkosc_domek_x, predkosc_fajerwerek_x);

    // Poziomy
    std::vector<Poziom> Poziomy;
    int kolumny = 3;
    int wiersze = 2;
    float margin = 50.f;
    float szerokosc_poziomu = 250.f;
    float wysokosc_poziomu = 180.f;
    for (int i = 0; i < 6; i++) {
        Poziom p;
        p.numer_poziomu = i + 1;

        // ramka
        p.ramka.setSize(sf::Vector2f(szerokosc_poziomu, wysokosc_poziomu));
        p.ramka.setOutlineThickness(5.0f);
        p.ramka.setOutlineColor(sf::Color::White);
        p.ramka.setFillColor(sf::Color(50, 50, 50));

        // img pod poziom
        if (i < tla.size()) {
            p.obrazek.setTexture(tla[i]);
            sf::Vector2u texSize = tla[i].getSize();
            p.obrazek.setScale(szerokosc_poziomu / texSize.x, wysokosc_poziomu / texSize.y);
        }

        // grid 
        int kol = i % kolumny;
        int rzad = i / kolumny;
        float posX = (szerokosc_okna / 2.0f - (kolumny * (szerokosc_poziomu + margin)) / 2.0f) + kol * (szerokosc_poziomu + margin);
        float posY = (wysokosc_okna / 2.0f - (wiersze * (wysokosc_poziomu + margin)) / 2.0f) + rzad * (wysokosc_poziomu + margin);
        p.ramka.setPosition(posX, posY);
        p.obrazek.setPosition(posX, posY);
        Poziomy.push_back(p);
    }


    //tekstura mikolaja
    sf::Texture tekstura_mikolaj;
    if (!tekstura_mikolaj.loadFromFile("mikolajp1.png"))
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        std::cout << buffer << std::endl;
        std::cout << "nie zaladowano tekstury mikolaja" << std::endl;
        return 1;
    }
    sf::Font czcionka_arial;
    sf::Font czcionka_game;
    if (!czcionka_arial.loadFromFile("arial.ttf"))
    {
        std::cout << "blad ladowania czcionki" << std::endl;
        return 1;
    }
    if (!czcionka_game.loadFromFile("ByteBounce.ttf"))
    {
        std::cout << "blad ladowania czcionki" << std::endl;
        return 1;
    }
    //punkty
    sf::Text tekst_punktow;
    tekst_punktow.setFont(czcionka_arial);
    tekst_punktow.setCharacterSize(30);       // rozmiar czcionki
    tekst_punktow.setFillColor(sf::Color::Yellow); // kolor tekstu
    tekst_punktow.setPosition(20.0f, 20.0f);  // pozycja w lewym górnym rogu
    tekst_punktow.setStyle(sf::Text::Bold);

    // przyciski w menu
    std::vector<std::string> nazwyPrzycisków = { "ZAGRAJ", "POZIOMY", "WYJDZ" };
    std::vector<sf::Text> przyciski;

    for (int i = 0; i < nazwyPrzycisków.size(); i++) {
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

        float offsetY = i * 75.0f;
        p.setPosition(szerokosc_okna / 2.0f, (wysokosc_okna / 2.0f) + offsetY);

        przyciski.push_back(p);
    }

    // przycisk w poziomy 
    sf::Text poziomExit;
    poziomExit.setFont(czcionka_game);
    poziomExit.setString("WROC DO MENU");
    poziomExit.setCharacterSize(50);
    poziomExit.setFillColor(sf::Color::White);
    poziomExit.setOutlineColor(sf::Color::Red);
    poziomExit.setOutlineThickness(2.0f);
    sf::FloatRect bounds = poziomExit.getLocalBounds();
    poziomExit.setOrigin(0,0);
    poziomExit.setPosition(20.0f, 20.0f);

    // ustawienie stanu gry na menu

    StanGry aktualnyStan = MENU;

    //napis przy lataniu za nisko
    sf::Text napis_lot;
    napis_lot.setFont(czcionka_arial);
    napis_lot.setString("uwazaj, przypierdolisz komus w komin");
    napis_lot.setCharacterSize(30);
    napis_lot.setFillColor(sf::Color::White);

    //ustawianie napisu na srodku(giga chujnia)
    sf::FloatRect textBounds = napis_lot.getLocalBounds();
    napis_lot.setOrigin(
        textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f
    );

    napis_lot.setPosition(szerokosc_okna / 2.0f, wysokosc_okna / 2.5f);

    //napis przy kolizji mikolaja z fajerwerka
    sf::Text napis_kolizja;
    napis_kolizja.setFont(czcionka_arial);
    napis_kolizja.setString("no i chuj,\nswiat nie bedzie");
    napis_kolizja.setCharacterSize(30);
    napis_kolizja.setFillColor(sf::Color::Red);
    napis_kolizja.setPosition(szerokosc_okna - 300, 40);

    //tekstura prezentu
    sf::Texture tekstura_prezentu;
    if (!tekstura_prezentu.loadFromFile("prezent.png"))
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        std::cout << buffer << std::endl;
        std::cout << "nie zaladowano tekstury prezentu" << std::endl;
        return 1;
    }

    //tekstury domkow
    std::vector<sf::Texture> tekstury_domkow;
    sf::Texture t1;
    std::vector<std::string> obrazki_domkow = { "dom1.png", "dom2.png","dom3.png" ,"dom4.png" ,"dom5.png" };
    for (int i = 0; i < obrazki_domkow.size(); i++) {
        t1.loadFromFile(obrazki_domkow[i]);
        tekstury_domkow.push_back(t1);
    }


    //tekstury fajerwerek
    std::vector<std::string> obrazki_fajerwerkow = { "fajerwerek1.png", "fajerwerek2.png","fajerwerek3.png" };
    std::vector<sf::Texture> tekstury_fajerwerek;
    
    for(auto& obrazek : obrazki_fajerwerkow){
        sf::Texture tekstura;
        tekstura.loadFromFile(obrazek);
        tekstury_fajerwerek.push_back(tekstura);
    }
    /*f1.loadFromFile("fajerwerek1.png");
    f2.loadFromFile("fajerwerek2.png");
    f3.loadFromFile("fajerwerek3.png");
    tekstury_fajerwerek.push_back(f1);
    tekstury_fajerwerek.push_back(f2);
    tekstury_fajerwerek.push_back(f3);*/

    //tekstura wykrzyknika
    sf::Texture tekstura_wykrzyknik;
    if (!tekstura_wykrzyknik.loadFromFile("wykrzyknik.png"))
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        std::cout << buffer << std::endl;
        std::cout << "nie zaladowano tekstury wykrzyknika" << std::endl;
        return 1;
    }
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
    m k1 = { 300.0f, 120.0f };
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

    //serca
    std::vector<sf::Sprite> serca;

    while (window.isOpen())
    {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (aktualnyStan == MENU) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    for (int i = 0; i < przyciski.size(); i++) {
                        if (przyciski[i].getGlobalBounds().contains(mousePosF)) {
                            if (i == 0) { // pierwszy przycisk: ZAGRAJ
                                hp = 3;
                                punkty = 0;
                                mikolaj.setPosition(szerokosc_okna / 8, wysokosc_okna / 3);
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
                            else if (i == 1) {
                                aktualnyStan = POZIOMY;
                            }
                            else if (i == przyciski.size() - 1) { // ostatni przycisk: WYJDZ
                                window.close();
                            }
                        }
                    }
                }
            }
            else if (aktualnyStan == POZIOMY) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                        if (poziomExit.getGlobalBounds().contains(mousePosF)) {
                            aktualnyStan = MENU;
                        }
                }
            }
        }

        window.clear(sf::Color::Black);

        if (aktualnyStan == MENU) {
            // --- MENU ---

            window.draw(tlo_menu);

            for (auto& p : przyciski) {
                // hover na button
                if (p.getGlobalBounds().contains(mousePosF)) {
                    p.setFillColor(sf::Color::Yellow);
                    p.setScale(1.1f, 1.1f);
                }
                else {
                    p.setFillColor(sf::Color::White);
                    p.setScale(1.0f, 1.0f);
                }
                window.draw(p);
            }

        }
        else if (aktualnyStan == POZIOMY) {
            // --- POZIOMY ---

            window.draw(tlo_menu);
            for (auto& p : Poziomy) {
                // hover na ramkę
                if (p.ramka.getGlobalBounds().contains(mousePosF)) {
                    p.ramka.setOutlineColor(sf::Color::Yellow);
                }
                else {
                    p.ramka.setOutlineColor(sf::Color::White);
                }

                window.draw(p.ramka);
                window.draw(p.obrazek);
            }
            if (poziomExit.getGlobalBounds().contains(mousePosF)) {
                poziomExit.setFillColor(sf::Color::Yellow);
                poziomExit.setScale(1.1f, 1.1f);
            }
            else {
                poziomExit.setFillColor(sf::Color::White);
                poziomExit.setScale(1.0f, 1.0f);
            }
            window.draw(poziomExit);

        }
        else if (aktualnyStan == ROZGRYWKA) {
            // --- ROZGRYWKA ---

            //wyjscie do menu(pozdro Juras)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                aktualnyStan = MENU;
            }
            float dt = clock.restart().asSeconds();
            //cala mechnika tla(ruch i zapetlanie)

            float predkosc_tla = 200.0f;//stala wartosc(tego nie zmieniamy w zaleznosci od poziomow)

            tlo1_x -= predkosc_tla * dt;
            tlo2_x -= predkosc_tla * dt;
            if (tlo1_x + szerokosc_tla <= 0.0f)
            {
                tlo1_x = tlo2_x + szerokosc_tla - 5.0f;
            }
            if (tlo2_x + szerokosc_tla <= 0.0f)
            {
                tlo2_x = tlo1_x + szerokosc_tla - 5.0f;
            }
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
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) &&
                    cooldown.getElapsedTime().asSeconds() >= cooldown_prezent)
                {
                    sf::Sprite nowy;
                    nowy.setTexture(tekstura_prezentu);

                    sf::Vector2u prezent_Size = tekstura_prezentu.getSize();
                    float prezentScaleX = 50.0f / prezent_Size.x;
                    float prezentScaleY = 50.0f / prezent_Size.y;
                    nowy.setScale(prezentScaleX, prezentScaleY);

                    nowy.setPosition(mikolaj.getPosition());

                    prezenty.push_back(nowy);

                    cooldown.restart();
                }
            }

            // aktualizacja prezentow
            for (auto& p : prezenty)
            {
                sf::Vector2f pos_p = p.getPosition();
                pos_p.y += predkosc_prezent_y * dt;
                pos_p.x += predkosc_prezent_x * dt;
                p.setPosition(pos_p);
            }

            // usuwanie prezentow na dole okna
            prezenty.erase(
                std::remove_if(prezenty.begin(), prezenty.end(),
                    [&](sf::Sprite& p) {
                        if (p.getPosition().y + p.getGlobalBounds().height >= wysokosc_okna)
                        {
                            return true;
                        }
                        for (auto& h : hitboxy)
                        {
                            if (p.getGlobalBounds().intersects(h.getGlobalBounds()))
                            {
                                punkty += 1;
                                return true;
                            }
                        }
                        return false;
                    }),
                prezenty.end()
            );

            //tworzenie domku + cooldown + hitbox komina
            if (cooldown_domku.getElapsedTime().asSeconds() >= cooldown_resp_domku)
            {
                sf::Sprite domek;
                int index = rand() % 5;
                domek.setTexture(tekstury_domkow[index]);

                //skalowanie
                sf::Vector2u domek_size = tekstury_domkow[index].getSize();
                scaleX = 250.0f / domek_size.x;
                scaleY = 250.0f / domek_size.y;
                domek.setScale(scaleX, scaleY);

                domek.setPosition(szerokosc_okna, wysokosc_okna - 220.0f);

                domki.push_back(domek);

                sf::RectangleShape hitbox;
                float hitbox_start = zakresy_kominow[index % 5].start * (domek_size.x * scaleX);
                float hitbox_end = zakresy_kominow[index % 5].koniec * (domek_size.x * scaleX);

                hitbox.setSize(sf::Vector2f(hitbox_end - hitbox_start, wysokosc_hitboxa));
                hitbox.setPosition(domek.getPosition().x + hitbox_start, domek.getPosition().y - 5.0f);
                hitbox.setFillColor(sf::Color::Black);

                hitboxy.push_back(hitbox);

                cooldown_domku.restart();
                cooldown_resp_domku = 3.0f + 3.0f * (rand() / (float)RAND_MAX);
            }

            //aktualizacja wszystkich domkow
            for (auto& d : domki)
            {
                sf::Vector2f pos_d = d.getPosition();
                pos_d.x -= predkosc_domek_x * dt;
                d.setPosition(pos_d);
            }
            for (auto& h : hitboxy)
            {
                sf::Vector2f pos_h = h.getPosition();
                pos_h.x -= predkosc_domek_x * dt;
                h.setPosition(pos_h);
            }

            domki.erase(
                std::remove_if(domki.begin(), domki.end(),
                    [&](sf::Sprite& d) {
                        return d.getPosition().x + d.getGlobalBounds().width < 0;
                    }),
                domki.end()
            );
            hitboxy.erase(
                std::remove_if(hitboxy.begin(), hitboxy.end(),
                    [&](sf::RectangleShape& h) {
                        return h.getPosition().x + h.getSize().x < 0;
                    }),
                hitboxy.end()
            );

            //tworzenie wykrzyknikow do fajerwerek + cooldown
            if (cooldown_fajerwerek.getElapsedTime().asSeconds() >= cooldown_resp_fajerwerek)
            {
                for (int i = 0; i < 4; i++)
                {
                    float fajerwerka_y = rand() % 501 + 100;

                    ostrzerzenie o;
                    o.wykrzyknik.setTexture(tekstura_wykrzyknik);
                    o.wykrzyknik.setPosition(szerokosc_okna - 40, fajerwerka_y);

                    //skalowanie wykrzyknika
                    sf::Vector2u wykrzyknik_size = tekstura_wykrzyknik.getSize();
                    scaleX = 50.0f / wykrzyknik_size.x;
                    scaleY = 50.0f / wykrzyknik_size.y;
                    o.wykrzyknik.setScale(scaleX, scaleY);
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
                    scaleX = 30.0f / fajerwerka_size.x;
                    scaleY = 20.0f / fajerwerka_size.y;
                    fajerwerka.setScale(scaleX, scaleY);
                    fajerwerka.setPosition(szerokosc_okna + 1, o.wykrzyknik.getPosition().y);
                    fajerwerki.push_back(fajerwerka);
                    o.wystrzelono = true;
                }
            }

            ostrzerzenia.erase(
                std::remove_if(ostrzerzenia.begin(), ostrzerzenia.end(),
                    [&](ostrzerzenie& o) {
                        return o.Timer.getElapsedTime().asSeconds() >= 1.0f;
                    }),
                ostrzerzenia.end()
            );

            //aktualizacja wszystkich fajerwerek
            for (auto& f : fajerwerki)
            {
                sf::Vector2f pos_f = f.getPosition();
                pos_f.x += -predkosc_fajerwerek_x * dt;
                f.setPosition(pos_f);
            }

            fajerwerki.erase(
                std::remove_if(fajerwerki.begin(), fajerwerki.end(),
                    [&](sf::Sprite& f) {
                        return f.getPosition().x + f.getGlobalBounds().width < 0;
                    }),
                fajerwerki.end()
            );
            //rysowanie serc z odpowiednią teksturą
            serca.clear();
            for (int i = 0; i < 3; i++)
            {
                sf::Sprite s;
                if (hp >= i + 1)
                {
                    s.setTexture(serce);
                }
                else
                {
                    s.setTexture(puste_serce);
                }
                sf::Vector2u serceSize = serce.getSize();
                s.setScale(40.0f / serceSize.x, 40.0f / serceSize.y);
                s.setPosition(szerokosc_okna - (i + 1) * 50.0f, 20.0f);
                serca.push_back(s);
            }

            //koniec programu
            if (window.hasFocus())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                    window.close();
            }
            window.clear(sf::Color::Black);
            window.draw(tlo1);
            window.draw(tlo2);

            //odejmowanie hp podczas kolizji
            for (auto it = fajerwerki.begin(); it != fajerwerki.end();)
            {
                if (mikolaj.getGlobalBounds().intersects((*it).getGlobalBounds()))
                {
                    if (cooldown_dmg.getElapsedTime().asSeconds() >= czas_nietykalnosci)
                    {
                        hp--;
                        cooldown_dmg.restart();
                    }
                    it = fajerwerki.erase(it);
                }
                else
                {
                    it++;
                }
            }
            if (hp == 0)
            {
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
            //  window.draw(h);
            // rysowanie serc
            for (auto& s : serca)
                window.draw(s);
            if (newPos.y + bounds.height == wysokosc_okna * 0.6f)
                window.draw(napis_lot);
            window.draw(mikolaj);
            tekst_punktow.setString(std::to_string(punkty));
            window.draw(tekst_punktow);
        }


        window.display();
    }
}
