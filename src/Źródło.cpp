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

//do wektorow(!!!dla kciuka!!!) - kazdy indeks to nowy poziom, jak chcesz zmienic predkosc jakiegos obiektu to tutaj
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

std::vector<float> predkosci_mikolaj = { 200.0f,200.0f,200.0f,200.0f,200.0f,200.0f};
std::vector<float> predkosci_prezent_x = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
std::vector<float> predkosci_prezent_y = { 200.0f,200.0f,200.0f,200.0f,200.0f,200.0f};
std::vector<float> predkosci_domek = { 100.0f,100.0f,100.0f,100.0f,100.0f,100.0f};
std::vector<float> predkosci_fajerwerki_x = { 400.0f,400.0f,400.0f,400.0f,400.0f,400.0f};

void setSpeed(int poziom, float& predkosc_mikolaj_y, float& predkosc_prezent_x, float& predkosc_prezent_y, float& predkosc_domek_x, float& predkosc_fajerwerek_x)
{
    int index = poziom - 1;
    if(index<=5)
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

int main() {
    srand(time(NULL));
    int szerokosc_okna = 1200;
    int wysokosc_okna = 900;
    sf::RenderWindow window(sf::VideoMode(szerokosc_okna, wysokosc_okna), "SantaGame");

    //kurwa potrzebuje poziomow XDD
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int poziom = 1;
    setSpeed(poziom, predkosc_mikolaj_y, predkosc_prezent_x, predkosc_prezent_y, predkosc_domek_x, predkosc_fajerwerek_x);
    //tekstura mikolaja
    sf::Texture tekstura_mikolaj;
    if (!tekstura_mikolaj.loadFromFile("mikolaj.png"))
    {
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        std::cout << buffer << std::endl;
        std::cout << "nie zaladowano tekstury mikolaja" << std::endl;
        return 1;
    }
    sf::Font czcionka_arial;
    if (!czcionka_arial.loadFromFile("arial.ttf"))
    {
        std::cout << "blad ladowania czcionki" << std::endl;
        return 1;
    }
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
    sf::Texture t1, t2, t3;
    t1.loadFromFile("domek1.png");
    t2.loadFromFile("domek2.png");
    t3.loadFromFile("domek3.png");

    tekstury_domkow.push_back(t1);
    tekstury_domkow.push_back(t2);
    tekstury_domkow.push_back(t3);

    //tekstury fajerwerek
    std::vector<sf::Texture> tekstury_fajerwerek;
    sf::Texture f1, f2, f3;
    f1.loadFromFile("fajerwerek1.png");
    f2.loadFromFile("fajerwerek2.png");
    f3.loadFromFile("fajerwerek3.png");
    tekstury_fajerwerek.push_back(f1);
    tekstury_fajerwerek.push_back(f2);
    tekstury_fajerwerek.push_back(f3);

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

    //czasy
    sf::Clock clock;
    sf::Clock cooldown;
    sf::Clock cooldown_domku;
    sf::Clock cooldown_fajerwerek;
    float cooldown_prezent = 1.0f;
    float cooldown_resp_domku = 3.5f;
    float cooldown_resp_fajerwerek = 5.0f;

    // mikolaj
    m k1 = { 400.0f, 120.0f };
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

    //wiele fajerwerek 
    std::vector<sf::Sprite> fajerwerki;

    //wykrzykniki
    std::vector<ostrzerzenie> ostrzerzenia;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();
        ////////////////

        // ruch mikolaja
        sf::Vector2f pos = mikolaj.getPosition();
        sf::FloatRect bounds = mikolaj.getGlobalBounds();
        sf::Vector2f newPos = pos;

        //predkosc_mikolaj_y = 200.0f;

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
        //predkosc_prezent_x = 0.0f;
        //predkosc_prezent_y = 200.0f;
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
                    return p.getPosition().y + p.getGlobalBounds().height >= wysokosc_okna;
                }),
            prezenty.end()
        );


        //tworzenie domku + cooldown
        if (cooldown_domku.getElapsedTime().asSeconds() >= cooldown_resp_domku)
        {
            sf::Sprite domek;
            int index = rand() % 3;
            domek.setTexture(tekstury_domkow[index]);//losowa grafika z vectora(indeksy 0-2)

            //skalowanie
            sf::Vector2u domek_size = tekstury_domkow[index].getSize();
            scaleX = 220.0f / domek_size.x;
            scaleY = 220.0f / domek_size.y;
            domek.setScale(scaleX, scaleY);

            domek.setPosition(szerokosc_okna, wysokosc_okna - 220.0f);

            domki.push_back(domek);
            cooldown_domku.restart();
            cooldown_resp_domku = 3.0f + 3.0f * (rand() / (float)RAND_MAX);
        }

        //aktualizacja wszystkich domkow
        //predkosc_domek_x = 100.0f;
        for (auto& d : domki)
        {
            sf::Vector2f pos_d = d.getPosition();
            pos_d.x -= predkosc_domek_x * dt;
            d.setPosition(pos_d);
        }

        domki.erase(
            std::remove_if(domki.begin(), domki.end(),
                [&](sf::Sprite& d) {
                    return d.getPosition().x + d.getGlobalBounds().width < 0;
                }),
            domki.end()
        );



        //tworzenie wykrzyknikow do fajerwerek + cooldown
        if (cooldown_fajerwerek.getElapsedTime().asSeconds() >= cooldown_resp_fajerwerek)
        {
            for (int i = 0;i < 4;i++)
            {
                float fajerwerka_y = rand() % 501 + 100;

                ostrzerzenie o;
                o.wykrzyknik.setTexture(tekstura_wykrzyknik);
                o.wykrzyknik.setPosition(szerokosc_okna - 40, fajerwerka_y);

                //skalowanie wykrzyknika
                sf::Vector2u wykrzyknik_size = tekstura_wykrzyknik.getSize();
                scaleX = 30.0f / wykrzyknik_size.x;
                scaleY = 30.0f / wykrzyknik_size.y;
                o.wykrzyknik.setScale(scaleX, scaleY);
                o.Timer.restart();

                ostrzerzenia.push_back(o);




            }
            cooldown_fajerwerek.restart();
            cooldown_resp_fajerwerek = 10.0f + 5.0f * (rand() / (float)RAND_MAX); //losowy czas między 10-15s
        }
        //fajerwerki
        for (auto& o : ostrzerzenia)
        {
            if (o.Timer.getElapsedTime().asSeconds() >= 1.0f && !o.wystrzelono) // sekunde po pokazaniu leca fajerwerki
            {
                sf::Sprite fajerwerka;
                int index = rand() % 3;
                fajerwerka.setTexture(tekstury_fajerwerek[index]);//losowa grafika z vectora(indeksy 0-2)

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
        //predkosc_fajerwerek_x = 400.0f;
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

        //koniec programu
        if (window.hasFocus())
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
                window.close();
        }
        window.clear(sf::Color::Black);
        //wyswietlanie napisu podczas kolizji
        for (auto& f : fajerwerki) {
            if (mikolaj.getGlobalBounds().intersects(f.getGlobalBounds())) {
                window.draw(napis_kolizja);
                break; // nie ma sensu sprawdzać dalej 
            }
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
        if (newPos.y + bounds.height == wysokosc_okna * 0.6f)
            window.draw(napis_lot);
        window.draw(mikolaj);
        window.display();
    }
}

