#include <iostream>
#include <windows.h>
#include <ctime>
#include <conio.h>
#include <vector>

using namespace std;

constexpr int GAME_WIDTH = 50;
constexpr int GAME_HEIGHT = 30;
constexpr int SNAKE_INITIAL_LENGTH = 3;
constexpr int COLOR_SNAKE = 15;  // Bright White
constexpr int COLOR_APPLE = 12;  // Red
constexpr char CHAR_SNAKE = '*';
constexpr char CHAR_APPLE = 'o';

struct Position {
    int x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class Console {
public:
    static void initialize() {
        srand(static_cast<unsigned>(time(0)));
        system("mode con cols=51 lines=31");
        MoveWindow(GetConsoleWindow(), 50, 50, 2000, 2000, true);
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo = { sizeof(cursorInfo), false };
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);
    }

    static void drawBoundaries() {
        for (int row = 0; row < GAME_HEIGHT; ++row) {
            for (int col = 0; col < GAME_WIDTH; ++col) {
                char boundaryChar = getBoundaryChar(row, col);
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

private:
    static char getBoundaryChar(int row, int col) {
        if ((col == 0 || col == GAME_WIDTH - 1) && (row == 0 || row == GAME_HEIGHT - 1))
            return '+';
        if (col == 0 || col == GAME_WIDTH - 1)
            return '|';
        if (row == 0 || row == GAME_HEIGHT - 1)
            return '-';
        return ' ';
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
        do {
            newApple = { rand() % (GAME_WIDTH - 2) + 1, rand() % (GAME_HEIGHT - 2) + 1 };
        } while (isCollidingWithSnake(newApple));
        return newApple;
    }

    bool isCollidingWithSnake(const Position& pos) const {
        for (const auto& segment : snakeBody) {
            if (segment == pos) {
                return true;
            }
        }
        return false;
    }

    void processInput() {
        if (_kbhit()) {
            int keyPress = _getch();
            handleKeyPress(keyPress);
        }
    }

    void handleKeyPress(int keyPress) {
        if (keyPress == 224) {
            int arrowKey = _getch();
            switch (arrowKey) {
            case 72: setDirection(0, -1); break;  // Up
            case 80: setDirection(0, 1); break;   // Down
            case 75: setDirection(-1, 0); break;  // Left
            case 77: setDirection(1, 0); break;   // Right
            }
        }
        else if (keyPress == 27) {  // ESC
            isGameRunning = false;
        }
    }

    void setDirection(int dx, int dy) {
        if ((deltaX == 0 && dx != 0) || (deltaY == 0 && dy != 0)) {
            deltaX = dx;
            deltaY = dy;
        }
    }

    void updateGame() {
        Position nextHeadPosition = { snakeBody.front().x + deltaX, snakeBody.front().y + deltaY };
        if (isCollision(nextHeadPosition)) {
            isGameRunning = false;
            return;
        }

        snakeBody.insert(snakeBody.begin(), nextHeadPosition);
        if (nextHeadPosition == apple) {
            apple = generateNewApple();
            Console::displayCharacter(apple, CHAR_APPLE, COLOR_APPLE);
        }
        else {
            Console::displayCharacter(snakeBody.back(), ' ', COLOR_SNAKE);
            snakeBody.pop_back();
        }
        Console::displayCharacter(snakeBody.front(), CHAR_SNAKE, COLOR_SNAKE);
    }

    bool isCollision(const Position& pos) const {
        if (pos.x == 0 || pos.x == GAME_WIDTH - 1 || pos.y == 0 || pos.y == GAME_HEIGHT - 1)
            return true;
        return isCollidingWithSnake(pos);
    }

public:
    Game() {
        Console::initialize();
        Console::drawBoundaries();
        initializeSnake();
        apple = generateNewApple();
        Console::displayCharacter(apple, CHAR_APPLE, COLOR_APPLE);
        for (const auto& pos : snakeBody) {
            Console::displayCharacter(pos, CHAR_SNAKE, COLOR_SNAKE);
        }
    }

    void run() {
        while (isGameRunning) {
            Sleep(100);
            processInput();
            updateGame();
        }
        endGame();
    }

    void initializeSnake() {
        int startX = GAME_WIDTH / 2;
        int startY = GAME_HEIGHT / 2;
        for (int i = 0; i < SNAKE_INITIAL_LENGTH; ++i) {
            snakeBody.push_back({ startX - i, startY });
        }
    }

    void endGame() {
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
