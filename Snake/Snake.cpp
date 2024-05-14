#include <iostream>
#include <windows.h>
#include <ctime>
#include <conio.h>
#include <vector>

using namespace std;

const int GAME_WIDTH = 50;
const int GAME_HEIGHT = 30;
const int SNAKE_INITIAL_LENGTH = 3;
const int COLOR_SNAKE = 15;  // Bright White
const int COLOR_APPLE = 12;  // Red
const char CHAR_SNAKE_HEAD = '*';
const char CHAR_APPLE = 'o';

struct Position {
    int x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class Console {
public:
    static void initialize() {
        srand(time(0));
        system("mode con cols=51 lines=31");
        MoveWindow(GetConsoleWindow(), 50, 50, 2000, 2000, true);
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo = { sizeof(cursorInfo), false };
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);
    }

    static void drawBoundaries() {
        for (int row = 0; row < GAME_HEIGHT; row++) {
            for (int col = 0; col < GAME_WIDTH; col++) {
                char boundaryChar = ' ';
                if (col == 0 || col == GAME_WIDTH - 1) boundaryChar = '|';
                if (row == 0 || row == GAME_HEIGHT - 1) boundaryChar = '-';
                if ((col == 0 || col == GAME_WIDTH - 1) && (row == 0 || row == GAME_HEIGHT - 1))
                    boundaryChar = '+';
                cout << boundaryChar;
            }
            cout << endl;
        }
    }

    static void displayCharacter(Position pos, char character, int color) {
        COORD coord = { short(pos.x), short(pos.y) };
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(consoleHandle, coord);
        SetConsoleTextAttribute(consoleHandle, color);
        putchar(character);
    }

    static void clearScreen() {
        system("cls");
    }

    static void pause() {
        system("pause");
    }
};

class Game {
private:
    vector<Position> snakeBody;
    Position apple;
    int deltaX = 1;
    int deltaY = 0;
    bool isGameRunning = true;

    Position generateNewApple() {
        Position newApple;
        bool isColliding;
        do {
            isColliding = false;
            newApple.x = rand() % (GAME_WIDTH - 2) + 1;
            newApple.y = rand() % (GAME_HEIGHT - 2) + 1;
            for (const auto& segment : snakeBody) {
                if (segment == newApple) {
                    isColliding = true;
                    break;
                }
            }
        } while (isColliding);
        return newApple;
    }

    void processInput() {
        if (_kbhit()) {
            int keyPress = _getch();
            switch (keyPress) {
            case 224:  // Handle arrow keys
                switch (_getch()) {
                case 72: if (deltaY == 0) { deltaY = -1; deltaX = 0; } break; // Up
                case 80: if (deltaY == 0) { deltaY = 1; deltaX = 0; } break;  // Down
                case 75: if (deltaX == 0) { deltaY = 0; deltaX = -1; } break; // Left
                case 77: if (deltaX == 0) { deltaY = 0; deltaX = 1; } break;  // Right
                }
                break;
            case 27:  // ESC 
                isGameRunning = false;
                break;
            }
        }
    }

    void updateGame() {
        Position nextHeadPosition = { snakeBody.front().x + deltaX, snakeBody.front().y + deltaY };
        if (nextHeadPosition.x == 0 || nextHeadPosition.x == GAME_WIDTH - 1 || nextHeadPosition.y == 0 || nextHeadPosition.y == GAME_HEIGHT - 1) {
            isGameRunning = false;  // Snake hits the boundary
            return;
        }

        for (const auto& segment : snakeBody) {
            if (nextHeadPosition == segment) {
                isGameRunning = false;  // Collision with itself
                return;
            }
        }

        snakeBody.insert(snakeBody.begin(), nextHeadPosition);
        if (nextHeadPosition == apple) {  // Eat the apple
            apple = generateNewApple();
            Console::displayCharacter(apple, CHAR_APPLE, COLOR_APPLE);
        }
        else {
            Console::displayCharacter(snakeBody.back(), ' ', COLOR_SNAKE);  // Clear last segment
            snakeBody.pop_back();
        }
        Console::displayCharacter(snakeBody.front(), CHAR_SNAKE_HEAD, COLOR_SNAKE);  // Redraw snake
    }

public:
    Game() {
        Console::initialize();
        Console::drawBoundaries();
        snakeBody = { {GAME_WIDTH / 2, GAME_HEIGHT / 2} };
        for (int i = 1; i < SNAKE_INITIAL_LENGTH; i++) {
            snakeBody.push_back({ GAME_WIDTH / 2 - i, GAME_HEIGHT / 2 });
        }
        apple = generateNewApple();
        Console::displayCharacter(apple, CHAR_APPLE, COLOR_APPLE);
        for (const auto& pos : snakeBody) {
            Console::displayCharacter(pos, CHAR_SNAKE_HEAD, COLOR_SNAKE);
        }
    }

    void run() {
        while (isGameRunning) {
            Sleep(100);
            processInput();
            updateGame();
        }
        Console::clearScreen();
        cout << "GAME OVER\n";
        Console::pause();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
