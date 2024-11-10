#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <cassert>
#define CLEAR_SCREEN "clear"
#define board_size 8
using namespace std;
void clearScreen();

class GameComponent {
public:
    virtual void initialize() = 0;
    virtual void display() const = 0;
    virtual ~GameComponent() {}
};

class Move {
public:
    char player;
    int row, col;
    Move(char p, int r, int c) : player(p), row(r), col(c) {}
};
class Board : public GameComponent {
private:
    char board[board_size][board_size];
    vector<Move> history;
public:
    Board() {
        initialize();
    }
    void initialize() override {
        history.clear();
        for (int i = 0; i < board_size; i++) {
            for (int j = 0; j < board_size; j++) {
                board[i][j] = '.';
            }
        }
        board[3][3] = board[4][4] = 'W';
        board[3][4] = board[4][3] = 'B';
    }
    void display() const override {
        cout << "     1    2    3    4    5    6    7    8" << endl;
        cout << "   +----+----+----+----+----+----+----+----+" << endl;
        for (int i = 0; i < board_size; i++) {
            cout << i + 1 << "  |";
            for (int j = 0; j < board_size; j++) {
                cout << " " << board[i][j] << "  |";
            }
            cout << endl;
            cout << "   +----+----+----+----+----+----+----+----+" << endl;
        }
        if (!history.empty()) {
            cout << "\nRecent Moves:\n";
            int start = max(0, static_cast<int>(history.size()) - 5);
            for (int i = start; i < history.size(); i++) {
                cout << "Move " << i + 1 << ": Player " << history[i].player << " -> (" << history[i].row + 1 << ", " << history[i].col + 1 << ")\n";
            }
        }
    }
    void addMove(char player, int row, int col) {
        history.push_back(Move(player, row, col));
        board[row][col] = player;
    }

    void flipPieces(char player, int row, int col);
    bool isValidMove(char player, int row, int col);
    bool hasValidMove(char player);
    char getPiece(int row, int col) const {
        return board[row][col];
    }
};

void Board::flipPieces(char player, int row, int col) {
    char opponent = (player == 'B') ? 'W' : 'B';
    const int directions[8][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for (int d = 0; d < 8; d++) {
        int r = row + directions[d][0];
        int c = col + directions[d][1];
        vector<pair<int, int>> toFlip;

        while (r >= 0 && r < board_size && c >= 0 && c < board_size && board[r][c] == opponent) {
            toFlip.push_back({ r, c });
            r += directions[d][0];
            c += directions[d][1];
        }
        if (!toFlip.empty() && r >= 0 && r < board_size && c >= 0 && c < board_size && board[r][c] == player) {
            for (auto& pos : toFlip) {
                board[pos.first][pos.second] = player;
            }
        }
    }
}

bool Board::isValidMove(char player, int row, int col) {
    if (row < 0 || row >= board_size || col < 0 || col >= board_size || board[row][col] != '.') {
        return false;
    }
    char opponent = (player == 'B') ? 'W' : 'B';
    const int directions[8][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for (int d = 0; d < 8; d++) {
        int r = row + directions[d][0];
        int c = col + directions[d][1];
        bool foundOpponent = false;

        while (r >= 0 && r < board_size && c >= 0 && c < board_size) {
            if (board[r][c] == opponent) {
                foundOpponent = true;
            }
            else if (board[r][c] == player) {
                if (foundOpponent) {
                    return true;
                }
                break;
            }
            else {
                break;
            }
            r += directions[d][0];
            c += directions[d][1];
        }
    }
    return false;
}
bool Board::hasValidMove(char player) {
    for (int row = 0; row < board_size; row++) {
        for (int col = 0; col < board_size; col++) {
            if (isValidMove(player, row, col)) {
                return true;
            }
        }
    }
    return false;
}
class Game {
private:
    static Game* instance;
    Board board;
    char currPlayer;

    Game() {
        currPlayer = rand() % 2 == 0 ? 'B' : 'W';
    }

public:
    static Game* getInstance() {
        if (instance == nullptr) {
            instance = new Game();
        }
        return instance;
    }
    void play();
    void determineWinner();
    Board& getBoard() { return board; }
};

Game* Game::instance = nullptr;

void Game::play() {
    board.initialize();
    cout << "\nPlayer " << currPlayer << " starts!" << endl;
    cout << "Press ENTER to begin.";
    cin.ignore();
    cin.get();

    bool gameEnd = false;
    while (!gameEnd) {
        clearScreen();
        board.display();

        if (!board.hasValidMove(currPlayer)) {
            cout << "\nPlayer " << currPlayer << " has no valid moves. Switching to another player." << endl;
            cin.ignore();
            cin.get();

            char opponent = (currPlayer == 'B') ? 'W' : 'B';
            if (board.hasValidMove(opponent)) {
                currPlayer = opponent;
                continue;
            }
            else {
                gameEnd = true;
                break;
            }
        }
        int row, col;
        bool validInput = false;

        while (!validInput) {
            cout << "Enter move (row col) for player " << currPlayer << " (or type -1 to quit): ";
            cin >> row;
            if (row == -1) {
                cout << "Player " << currPlayer << " has chosen to quit. Ending the game." << endl;
                return;
            }
            cin >> col;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\nError! Please enter numbers" << endl;
                continue;
            }
            if (row < 1 || row > board_size || col < 1 || col > board_size) {
                cout << "Error! Please enter values between 1 and " << board_size << "." << endl;
                continue;
            }
            row--;
            col--;
            if (board.isValidMove(currPlayer, row, col)) {
                board.addMove(currPlayer, row, col);
                board.flipPieces(currPlayer, row, col);
                validInput = true;
                currPlayer = (currPlayer == 'B') ? 'W' : 'B';
            }
            else {
                cout << "Error! Invalid input." << endl;
            }
        }
        if (!board.hasValidMove('B') && !board.hasValidMove('W')) {
            gameEnd = true;
        }
    }
    clearScreen();
    board.display();
    determineWinner();
}
void Game::determineWinner() {
    int blackS = 0, WhiteS = 0;

    for (int row = 0; row <board_size;row++) {
        for (int col = 0; col < board_size;col++) {
            if (board.getPiece(row, col)=='B') {
                blackS++;
            }
            else if (board.getPiece(row, col) =='W') {
                WhiteS++;
            }
        }
    }

    cout << "\nGame over!\n";
    cout << "Final Scores:\n";
    cout << "Player B (Black): " << blackS << " points\n";
    cout << "Player W (White): " << WhiteS << " points\n";

    if (blackS > WhiteS) {
        cout << "Player B  wins!!!" << endl;
    }
    else if (WhiteS > blackS) {
        cout << "Player W wins!!!" << endl;
    }
    else {
        cout << "Tie!!!" << endl;
    }
}

void clearScreen() {
    system(CLEAR_SCREEN);
}

void displayMenu() {
    int choice;
    bool exitGame = false;

    while (!exitGame) {
        cout << "\nWelcome to REVERSI!" << endl;
        cout << endl;
        cout << "1. Play" << endl;
        cout << "2. Rules" << endl;
        cout << "3. Exit" << endl;
        cout << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error! Please enter a number." << endl;
            continue;
        }
        switch (choice) {
        case 1: {
            Game* game = Game::getInstance();
            game->play();
            break;
        }
        case 2:
            cout << endl;
            cout << "Rules: " << endl;
            cout << endl;
            cout << "1. The goal is to occupy most of the board with your color discs." << endl;
            cout << "2. To flip an opponent's disc, it must be trapped between two of your own discs." << endl;
            cout << "3. Moves can be made horizontally, vertically, and diagonally." << endl;
            cout << "4. The game ends when neither player has any valid moves." << endl;
            cout << endl;
            break;
        case 3:
            cout << "Goodbye!" << endl;
            exitGame = true;
            break;
        default:
            cout << "Error! Invalid choice." << endl;
        }
    }
}
void runUnitTests();

int main() {
    srand(time(0));
    runUnitTests();
    displayMenu();
    return 0;
}


void runUnitTests() {
    Game* game = Game::getInstance();
    Board& board = game->getBoard();

    // Test 1:
    board.initialize();
    assert(board.isValidMove('B', 2, 3) == true);
    assert(board.isValidMove('W', 5, 4) == false);

    // Test 2:
    board.initialize();
    board.addMove('B', 2, 3);
    board.flipPieces('B', 2, 3);
    assert(board.getPiece(3, 3) == 'B');

    // Test 3:
    board.initialize();
    for (int i = 0; i < board_size; ++i) {
        for (int j = 0; j < board_size; ++j) {
            board.addMove('B', i, j);
        }
    }
    assert(!board.hasValidMove('W') && !board.hasValidMove('B'));

    cout << "All tests passed :)" << endl;
}
