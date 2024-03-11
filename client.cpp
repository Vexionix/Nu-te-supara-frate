#include <SFML/Graphics.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <wait.h>
#include <arpa/inet.h>
#include <time.h>
#include <chrono>

#define SIZE 800
#define CIRCLE_SIZE SIZE / 11
#define CIRCLE_OUTLINE 2
#define p1color colors[player_id - 1]
#define p2color colors[player_id]
#define p3color colors[player_id + 1]
#define p4color colors[player_id + 2]

void draw_corner(sf::RenderWindow &window, float corner_x, float corner_y)
{

        sf::CircleShape corner(SIZE / 25.f);
        corner.setPosition(corner_x, corner_y);
        corner.setFillColor(sf::Color::Transparent);
        corner.setOutlineColor(sf::Color::White);
        corner.setOutlineThickness(CIRCLE_OUTLINE);

        window.draw(corner);

        corner.setPosition(corner.getPosition().x, corner.getPosition().y + CIRCLE_SIZE);
        window.draw(corner);

        corner.setPosition(corner.getPosition().x + CIRCLE_SIZE, corner.getPosition().y - CIRCLE_SIZE);
        window.draw(corner);

        corner.setPosition(corner.getPosition().x, corner.getPosition().y + CIRCLE_SIZE);
        window.draw(corner);
}

void draw_short_vertical_line(sf::RenderWindow &window, float corner_x, float corner_y)
{

        sf::CircleShape circle(SIZE / 25.f);
        circle.setPosition(corner_x, corner_y);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(sf::Color::White);
        circle.setOutlineThickness(CIRCLE_OUTLINE);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x, circle.getPosition().y + CIRCLE_SIZE);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x, circle.getPosition().y + CIRCLE_SIZE);
        window.draw(circle);
}

void draw_long_vertical_line(sf::RenderWindow &window, float corner_x, float corner_y, sf::Color fillColor)
{

        sf::CircleShape circle(SIZE / 25.f);
        circle.setPosition(corner_x, corner_y);
        circle.setFillColor(fillColor);
        circle.setOutlineColor(sf::Color::White);
        circle.setOutlineThickness(CIRCLE_OUTLINE);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x, circle.getPosition().y + CIRCLE_SIZE);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x, circle.getPosition().y + CIRCLE_SIZE);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x, circle.getPosition().y + CIRCLE_SIZE);
        window.draw(circle);
}

void draw_short_horizontal_line(sf::RenderWindow &window, float corner_x, float corner_y)
{

        sf::CircleShape circle(SIZE / 25.f);
        circle.setPosition(corner_x, corner_y);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(sf::Color::White);
        circle.setOutlineThickness(CIRCLE_OUTLINE);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x + CIRCLE_SIZE, circle.getPosition().y);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x + CIRCLE_SIZE, circle.getPosition().y);
        window.draw(circle);
}

void draw_long_horizontal_line(sf::RenderWindow &window, float corner_x, float corner_y, sf::Color fillColor)
{

        sf::CircleShape circle(SIZE / 25.f);
        circle.setPosition(corner_x, corner_y);
        circle.setFillColor(fillColor);
        circle.setOutlineColor(sf::Color::White);
        circle.setOutlineThickness(CIRCLE_OUTLINE);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x + CIRCLE_SIZE, circle.getPosition().y);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x + CIRCLE_SIZE, circle.getPosition().y);
        window.draw(circle);

        circle.setPosition(circle.getPosition().x + CIRCLE_SIZE, circle.getPosition().y);
        window.draw(circle);
}

void draw_start_circle(sf::RenderWindow &window, float corner_x, float corner_y, sf::Color fillColor)
{

        sf::CircleShape circle(SIZE / 25.f);
        circle.setPosition(corner_x, corner_y);
        circle.setFillColor(fillColor);
        circle.setOutlineColor(sf::Color::White);
        circle.setOutlineThickness(CIRCLE_OUTLINE);
        window.draw(circle);
}

void draw_table(sf::RenderWindow &window, sf::Color P1, sf::Color P2, sf::Color P3, sf::Color P4)
{
        draw_corner(window, CIRCLE_OUTLINE + (SIZE / 25) / 4, CIRCLE_OUTLINE + (SIZE / 25) / 4);
        draw_corner(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 9, CIRCLE_OUTLINE + (SIZE / 25) / 4);
        draw_corner(window, CIRCLE_OUTLINE + (SIZE / 25) / 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 9);
        draw_corner(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 9, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 9);

        draw_short_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4);
        draw_short_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 10, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4);

        draw_short_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, CIRCLE_OUTLINE + (SIZE / 25) / 4);
        draw_short_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 10);

        draw_long_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, sf::Color::Transparent);
        draw_long_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, sf::Color::Transparent);
        draw_long_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, sf::Color::Transparent);
        draw_long_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, sf::Color::Transparent);

        draw_long_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE, sf::Color::Transparent);
        draw_long_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE, sf::Color::Transparent);
        draw_long_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, sf::Color::Transparent);
        draw_long_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, sf::Color::Transparent);

        draw_long_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 5, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, P1);
        draw_long_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 5, P2);
        draw_long_vertical_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 5, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE, P3);
        draw_long_horizontal_line(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 5, P4);

        draw_start_circle(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 10, P1);
        draw_start_circle(window, CIRCLE_OUTLINE + (SIZE / 25) / 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4, P2);
        draw_start_circle(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, CIRCLE_OUTLINE + (SIZE / 25) / 4, P3);
        draw_start_circle(window, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 10, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 6, P4);
}

void draw_arrows(sf::RenderWindow &window, sf::Sprite &arrowsprite, sf::Color P1, sf::Color P2, sf::Color P3, sf::Color P4)
{
        arrowsprite.setPosition(CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4 - CIRCLE_SIZE * (1.75 * 0.35 * SIZE / 800), SIZE - (SIZE / 25) / 4);
        arrowsprite.setRotation(270);
        arrowsprite.setColor(P1);
        window.draw(arrowsprite);

        arrowsprite.setPosition(CIRCLE_OUTLINE + (SIZE / 25) / 4, CIRCLE_OUTLINE + (SIZE / 25) / 4 + CIRCLE_SIZE * 4 - CIRCLE_SIZE * (1.75 * 0.35 * SIZE / 800));
        arrowsprite.setRotation(0);
        arrowsprite.setColor(P2);
        window.draw(arrowsprite);

        arrowsprite.setPosition(CIRCLE_SIZE * 7 + CIRCLE_SIZE * (1.75 * 0.35 * SIZE / 800), CIRCLE_OUTLINE + (SIZE / 25) / 4);
        arrowsprite.setRotation(90);
        arrowsprite.setColor(P3);
        window.draw(arrowsprite);

        arrowsprite.setPosition(SIZE - CIRCLE_OUTLINE - (SIZE / 25) / 4, CIRCLE_SIZE * 7 + CIRCLE_SIZE * (1.75 * 0.35 * SIZE / 800));
        arrowsprite.setRotation(180);
        arrowsprite.setColor(P4);
        window.draw(arrowsprite);
}

void draw_pawn(sf::RenderWindow &window, float x, float y, sf::Color P)
{
        sf::CircleShape pawn(SIZE / 50.f);
        pawn.setPosition(x + CIRCLE_OUTLINE + (SIZE / 25) / 2 + (SIZE / 50) / 2, y + CIRCLE_OUTLINE + (SIZE / 25) / 2 + (SIZE / 50) / 2);
        pawn.setFillColor(P);
        pawn.setOutlineColor(sf::Color::Black);
        pawn.setOutlineThickness(CIRCLE_OUTLINE);

        window.draw(pawn);
}

/* portul de conectare la server*/
int port;
int player_id;
char buf[100];
int table[11][11];
char warning_text_string[100];

int main(int argc, char *argv[])
{
        if (SIZE < 800)
        {
                printf("Window too small.");
                return 0;
        }
        sf::RenderWindow window(sf::VideoMode(SIZE, SIZE + 150), "Nu te supara frate");

        sf::Texture bg;
        if (!bg.loadFromFile("assets/bg.jpg"))
                return EXIT_FAILURE;
        sf::Sprite bgsprite(bg);
        bgsprite.setScale(SIZE / bgsprite.getLocalBounds().width, (SIZE + 150) / bgsprite.getLocalBounds().height);

        sf::Texture dice_button;
        if (!dice_button.loadFromFile("assets/dice button.png"))
                return EXIT_FAILURE;
        sf::Sprite dice_button_sprite(dice_button);
        dice_button_sprite.setPosition(25, SIZE + 12.5);

        sf::Font font;
        if (!font.loadFromFile("assets/font.ttf"))
                return EXIT_FAILURE;

        sf::Text dice_result;
        dice_result.setFont(font);
        dice_result.setString("Dice result: ");
        dice_result.setCharacterSize(35);
        dice_result.setPosition(350, SIZE + 30);

        sf::Text turn_text;
        turn_text.setFont(font);
        turn_text.setString("Turn: ");
        turn_text.setCharacterSize(35);
        turn_text.setPosition(650, SIZE + 30);

        sf::Text warning_text;
        warning_text.setFont(font);
        warning_text.setString(warning_text_string);
        warning_text.setCharacterSize(25);
        warning_text.setPosition(25, SIZE + 112.5);

        sf::Texture arrow;
        if (!arrow.loadFromFile("assets/arrow3.png"))
                return EXIT_FAILURE;
        sf::Sprite arrowsprite(arrow);
        arrowsprite.setScale(0.35 * SIZE / 800, 0.30 * SIZE / 800);

        int sd;                    // descriptorul de socket
        struct sockaddr_in server; // structura folosita pentru conectare
        char msg[100];             // mesajul trimis

        /* exista toate argumentele in linia de comanda? */
        if (argc != 3)
        {
                printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
                return -1;
        }

        /* stabilim portul */
        port = atoi(argv[2]);

        /* cream socketul */
        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror("Eroare la socket().\n");
                return errno;
        }

        /* umplem structura folosita pentru realizarea conexiunii cu serverul */
        /* familia socket-ului */
        server.sin_family = AF_INET;
        /* adresa IP a serverului */
        server.sin_addr.s_addr = inet_addr(argv[1]);
        /* portul de conectare */
        server.sin_port = htons(port);

        /* ne conectam la server */
        if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
                perror("[client]Eroare la connect().\n");
                return errno;
        }
        bzero(buf, 100);
        if (read(sd, buf, 100) < 0)
        {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
        }

        if (strcmp(buf, "Serverul este plin.") == 0)
        {
                printf("[client] Serverul este plin. Executia se va opri.");
                return 0;
        }
        else
                player_id = buf[0] - '0';

        int turn;
        int ok;
        int isFocused = 0;

        char win_title[100] = "Nu te supara frate - P";
        strncat(win_title, buf, 1);
        window.setTitle(win_title);

        sf::Color colors[8] = {sf::Color::Blue, sf::Color::Red, sf::Color::Yellow, sf::Color::Green, sf::Color::Blue, sf::Color::Red, sf::Color::Yellow, sf::Color::Green};
        sf::Color P1 = p1color;
        sf::Color P2 = p2color;
        sf::Color P3 = p3color;
        sf::Color P4 = p4color;

        std::chrono::time_point start_time = std::chrono::high_resolution_clock::now();
        std::chrono::time_point current_time = std::chrono::high_resolution_clock::now();
        int elapsed_time;

        char game_state[100];
        int winner_id;

        while (window.isOpen())
        {
                current_time = std::chrono::high_resolution_clock::now();
                elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

                if (elapsed_time >= 2)
                {
                        bzero(warning_text_string, 100);
                        start_time = current_time;
                }

                warning_text.setString(warning_text_string);

                /* citirea raspunsului dat de server
                (apel blocant pina cind serverul raspunde) */

                if (read(sd, table, sizeof(int) * 11 * 11) < 0)
                {
                        perror("[client]Eroare la read() de la server.\n");
                        return errno;
                }

                if (read(sd, &turn, sizeof(int)) < 0)
                {
                        perror("[client]Eroare la read() de la server.\n");
                        return errno;
                }

                window.clear();
                window.draw(bgsprite);
                window.draw(dice_button_sprite);
                window.draw(dice_result);
                char turn_buffer[100] = "Turn: ";
                char turn_ch = turn + '0';
                strncat(turn_buffer, &turn_ch, 1);
                turn_text.setString(turn_buffer);
                window.draw(turn_text);
                window.draw(warning_text);

                draw_table(window, P1, P2, P3, P4);

                draw_arrows(window, arrowsprite, P1, P2, P3, P4);

                for (int i = 0; i < 11; i++)
                        for (int j = 0; j < 11; j++)
                                if (table[i][j] >= 1 && table[i][j] <= 4)
                                        draw_pawn(window, CIRCLE_SIZE * j, CIRCLE_SIZE * i, colors[table[i][j] - 1]);

                window.display();

                if (read(sd, game_state, 100) < 0)
                {
                        perror("[client]Eroare la read() de la server.\n");
                        return errno;
                }

                if (strcmp(game_state, "over") == 0)
                {
                        if (read(sd, &winner_id, sizeof(int)) < 0)
                        {
                                perror("[client]Eroare la read() de la server.\n");
                                return errno;
                        }
                        sleep(2);
                        sf::RenderWindow game_ended(sf::VideoMode(450, 75), "Game over!");
                        game_ended.setPosition(sf::Vector2i(sf::VideoMode::getDesktopMode().width / 2 - 225,
                                                            sf::VideoMode::getDesktopMode().height / 2 - 37.5));

                        sf::Text over_text;
                        over_text.setFont(font);
                        char over_text_string[100];
                        strcpy(over_text_string, "Player ");
                        char winner_id_c = winner_id + '0';
                        strncat(over_text_string, &winner_id_c, 1);
                        strcat(over_text_string, " has won the game!");
                        over_text.setString(over_text_string);
                        over_text.setCharacterSize(25);
                        over_text.setPosition(25, 25);
                        window.setActive(false);
                        while (game_ended.isOpen())
                        {
                                game_ended.requestFocus();
                                sf::Event end_event;
                                while (game_ended.pollEvent(end_event))
                                {
                                        if (end_event.type == sf::Event::Closed)
                                                game_ended.close();
                                }
                                game_ended.clear();
                                game_ended.draw(over_text);
                                game_ended.display();
                        }       
                        window.close();
                        close(sd);
                        return 0;
                }

                sf::Event event;
                while (1)
                {
                        ok = 0;
                        while (window.pollEvent(event))
                        {
                                switch (event.type)
                                {
                                case ::sf::Event::Closed:
                                        window.close();
                                        strcpy(msg, "stop");
                                        if (write(sd, msg, 100) <= 0)
                                        {
                                                perror("[client]Eroare la write() spre server.\n");
                                                return errno;
                                        }

                                        close(sd);
                                        return 0;
                                        break;
                                case ::sf::Event::MouseButtonPressed:
                                        if (event.mouseButton.button == sf::Mouse::Left)
                                        {
                                                if (event.mouseButton.x >= 25 && event.mouseButton.x <= 325 && event.mouseButton.y >= SIZE + 12.5 && event.mouseButton.y <= SIZE + 87.5)
                                                {
                                                        char check[100];
                                                        strcpy(msg, "dice");
                                                        if (write(sd, msg, 100) <= 0)
                                                        {
                                                                perror("[client]Eroare la write() spre server.\n");
                                                                return errno;
                                                        }
                                                        if (read(sd, check, 100) < 0)
                                                        {
                                                                perror("[client]Eroare la read() de la server.\n");
                                                                return errno;
                                                        }
                                                        if (strcmp(check, "ok") == 0)
                                                        {

                                                                char result[15] = "Dice result: ";
                                                                result[14] = '\0';
                                                                int rolled;
                                                                if (read(sd, &rolled, sizeof(int)) < 0)
                                                                {
                                                                        perror("[client]Eroare la read() de la server.\n");
                                                                        return errno;
                                                                }
                                                                result[13] = rolled + '0';
                                                                dice_result.setString(result);

                                                                char valid_moves[100];
                                                                if (read(sd, valid_moves, 100) < 0)
                                                                {
                                                                        perror("[client]Eroare la read() de la server.\n");
                                                                        return errno;
                                                                }

                                                                if (strcmp(valid_moves, "yes") == 0)
                                                                {

                                                                        current_time = std::chrono::high_resolution_clock::now();
                                                                        start_time = current_time;
                                                                        while (1)
                                                                        {
                                                                                current_time = std::chrono::high_resolution_clock::now();
                                                                                elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

                                                                                if (elapsed_time >= 2)
                                                                                {
                                                                                        bzero(warning_text_string, 100);
                                                                                        start_time = current_time;
                                                                                }

                                                                                int temp_ok = 0;
                                                                                isFocused = 0;
                                                                                char temp_check[100];
                                                                                bzero(temp_check, 100);
                                                                                while (window.pollEvent(event))
                                                                                {
                                                                                        window.clear();
                                                                                        window.draw(bgsprite);
                                                                                        window.draw(dice_button_sprite);
                                                                                        window.draw(dice_result);
                                                                                        char turn_buffer[100] = "Turn: ";
                                                                                        char turn_ch = turn + '0';
                                                                                        strncat(turn_buffer, &turn_ch, 1);
                                                                                        turn_text.setString(turn_buffer);
                                                                                        window.draw(turn_text);
                                                                                        warning_text.setString(warning_text_string);
                                                                                        window.draw(warning_text);

                                                                                        draw_table(window, P1, P2, P3, P4);

                                                                                        draw_arrows(window, arrowsprite, P1, P2, P3, P4);

                                                                                        for (int i = 0; i < 11; i++)
                                                                                                for (int j = 0; j < 11; j++)
                                                                                                        if (table[i][j] >= 1 && table[i][j] <= 4)
                                                                                                                draw_pawn(window, CIRCLE_SIZE * j, CIRCLE_SIZE * i, colors[table[i][j] - 1]);

                                                                                        window.display();

                                                                                        char dice_while_event[100];

                                                                                        switch (event.type)
                                                                                        {
                                                                                        case sf::Event::Closed:
                                                                                                window.close();
                                                                                                strcpy(dice_while_event, "stop");
                                                                                                if (write(sd, dice_while_event, 100) <= 0)
                                                                                                {
                                                                                                        perror("[client]Eroare la write() spre server.\n");
                                                                                                        return errno;
                                                                                                }
                                                                                                close(sd);
                                                                                                return 0;
                                                                                                break;
                                                                                        case sf::Event::MouseButtonPressed:
                                                                                                if (event.mouseButton.button == sf::Mouse::Left && event.mouseButton.x <= SIZE && event.mouseButton.y <= SIZE)
                                                                                                {
                                                                                                        int temp_x = event.mouseButton.x / 73;
                                                                                                        int temp_y = event.mouseButton.y / 73;

                                                                                                        strcpy(dice_while_event, "click");
                                                                                                        if (write(sd, dice_while_event, 100) <= 0)
                                                                                                        {
                                                                                                                perror("[client]Eroare la write() spre server.\n");
                                                                                                                return errno;
                                                                                                        }

                                                                                                        if (write(sd, &temp_x, sizeof(int)) <= 0)
                                                                                                        {
                                                                                                                perror("[client]Eroare la write() spre server.\n");
                                                                                                                return errno;
                                                                                                        }
                                                                                                        if (write(sd, &temp_y, sizeof(int)) <= 0)
                                                                                                        {
                                                                                                                perror("[client]Eroare la write() spre server.\n");
                                                                                                                return errno;
                                                                                                        }
                                                                                                        if (read(sd, temp_check, 100) <= 0)
                                                                                                        {
                                                                                                                perror("[client]Eroare la write() spre server.\n");
                                                                                                                return errno;
                                                                                                        }
                                                                                                        if (strcmp(temp_check, "ok") == 0)
                                                                                                                temp_ok = 1;
                                                                                                        else
                                                                                                                start_time = current_time, strcpy(warning_text_string, "Select a valid pawn move.");
                                                                                                }
                                                                                                else
                                                                                                        start_time = current_time, strcpy(warning_text_string, "Select a valid pawn move.");
                                                                                                break;
                                                                                        default:
                                                                                                break;
                                                                                        }
                                                                                }
                                                                                if (temp_ok == 1)
                                                                                {
                                                                                        isFocused = 0;
                                                                                        break;
                                                                                }
                                                                        }
                                                                }
                                                                else
                                                                        start_time = current_time, strcpy(warning_text_string, "No legal moves.");
                                                        }
                                                        else
                                                        {
                                                                strcpy(warning_text_string, "Warning: Invalid input or not your turn.");
                                                                start_time = current_time;
                                                        }
                                                }
                                                else
                                                {
                                                        strcpy(msg, ".");
                                                        if (write(sd, msg, 100) <= 0)
                                                        {
                                                                perror("[client]Eroare la write() spre server.\n");
                                                                return errno;
                                                        }
                                                }
                                                ok = 1;
                                        }
                                        break;

                                case sf::Event::LostFocus:
                                        isFocused = 0;
                                        break;
                                case sf::Event::GainedFocus:
                                        if (isFocused == 0)
                                        {
                                                isFocused = 1;
                                                strcpy(msg, "gained focus");
                                                if (write(sd, msg, 100) <= 0)
                                                {
                                                        perror("[client]Eroare la write() spre server.\n");
                                                        return errno;
                                                }
                                                ok = 1;
                                        }
                                        break;
                                default:
                                        break;
                                }
                        }
                        if (ok == 1)
                                break;
                }

                /* citirea mesajului */
                /* bzero (msg, 100);
                 read(0, msg, 100);
                 strtok(msg,"\n"); */
                /* trimiterea mesajului la server */
                /* if (write (sd, msg, 100) <= 0)
                {
                perror ("[client]Eroare la write() spre server.\n");
                return errno;
                }
                if (strcmp(msg, "stop") == 0){
                close(sd);
                return 0;
                } */
                /* afisam mesajul primit */
        }
}
