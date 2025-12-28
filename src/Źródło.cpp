#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include <vector>
#include <algorithm>

struct m {
    float szerokosc;
    float wysokosc;
};

int main() {
    srand(time(NULL));
    int szerokosc_okna = 1200;
    int wysokosc_okna = 900;
    sf::RenderWindow window(sf::VideoMode(szerokosc_okna, wysokosc_okna), "SantaGame");

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



    const float speed = 200;

    //czasy
    sf::Clock clock;
    sf::Clock cooldown;
    sf::Clock cooldown_domku;
    float cooldown_prezent = 1.0f;
    float cooldown_resp_domku = 3.5f;

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


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float dt = clock.restart().asSeconds();

        // ruch mikolaja
        sf::Vector2f pos = mikolaj.getPosition();
        sf::FloatRect bounds = mikolaj.getGlobalBounds();
        sf::Vector2f newPos = pos;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            newPos.y -= speed * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            newPos.y += speed * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            newPos.x -= speed * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            newPos.x += speed * dt;

        // ograniczenia
        if (newPos.x < 0)
            newPos.x = 0;

        if (newPos.x + bounds.width > szerokosc_okna)
            newPos.x = szerokosc_okna - bounds.width;

        if (newPos.y < 0)
            newPos.y = 0;

        if (newPos.y + bounds.height > wysokosc_okna)
            newPos.y = wysokosc_okna - bounds.height;

        mikolaj.setPosition(newPos);


        // tworzenie nowego prezentu + cooldown


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

        // aktualizacja prezentow

        for (auto& p : prezenty)
        {
            sf::Vector2f pos_p = p.getPosition();
            pos_p.y += 200.0f * dt;
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
            int index=rand()%3;
            domek.setTexture(tekstury_domkow[index]);//losowa grafika z vectora(indeksy 0-2)
            
            //skalowanie
            sf::Vector2u domek_size = tekstury_domkow[index].getSize();
            scaleX = 220.0f / domek_size.x;
            scaleY = 220.0f / domek_size.y;
            domek.setScale(scaleX, scaleY);

            domek.setPosition(szerokosc_okna,wysokosc_okna-220.0f);

            domki.push_back(domek);
            cooldown_domku.restart();
            cooldown_resp_domku = 3.0f + 3.0f * (rand() / (float)RAND_MAX);
        }

        //aktualizacja wszystkich domkow
        
        for (auto& d : domki)
        {
            sf::Vector2f pos_d = d.getPosition();
            pos_d.x -= 100.0f * dt;
            d.setPosition(pos_d);
        }

        domki.erase(
            std::remove_if(domki.begin(), domki.end(),
                [&](sf::Sprite& d) {
                    return d.getPosition().x + d.getGlobalBounds().width < 0;
                }),
            domki.end()
        );

        //koniec programu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
            window.close();

        window.clear(sf::Color::Black);

        // rysowanie wszystkich prezentów
        for (auto& p : prezenty)
            window.draw(p);

        // rysowanie wszystkich domkow
        for (auto& d : domki)
            window.draw(d);
        window.draw(mikolaj);
        window.display();
    }
}
