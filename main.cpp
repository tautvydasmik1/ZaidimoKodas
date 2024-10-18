#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#define CLEAR_SCREEN "clear"
using namespace std;
void clearScreen();

// zaidejo ejimai
class Move {
public:
    char player;
    int eil, stulp;
    Move(char p, int e, int s) : player(p), eil(e), stulp(s) {}
};
// zaidimo lenta ir operacijos
class Board {
private:
    char board[8][8];
    vector<Move> moveHistory;
public:
    Board() {
        initialize();
    }
    void initialize() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = '.';
            }
        }
        // Pradines pozicijos
        board[3][3] = board[4][4] = 'W';
        board[3][4] = board[4][3] = 'B';
    }

    void print() {
        cout << "     1    2    3    4    5    6    7    8" << endl;
        cout << "   +----+----+----+----+--------+----+----+" << endl;
        for (int i = 0; i < 8; i++) {
            cout << i + 1 << "  |";
            for (int j = 0; j < 8; j++) {
                cout << " " << board[i][j] << "  |";
            }
            cout << endl;
            cout << "   +----+----+----+----+----+----+----+----+" << endl;
        }
        // Judesių trackeris
        if (!moveHistory.empty()) {
            cout << "\nAnkstesni 5 ėjimai :\n";
            int start = max(0, static_cast<int>(moveHistory.size()) - 5);
            for (int i = start; i < moveHistory.size(); i++) {
                cout << "Ėjimas " << i + 1 << ": žaidėjas " << moveHistory[i].player << " -> (" << moveHistory[i].eil + 1 << " " << moveHistory[i].stulp + 1 << ")\n";

            }
        }
    }

    void addMove(char player, int eil, int stulp) {
        moveHistory.push_back(Move(player, eil, stulp));
        board[eil][stulp] = player; // board atnaujinimas
    }

   void flipPieces(char player, int eil, int stulp) {
    char opponent = (player == 'B') ? 'W' : 'B';
    const int directions[8][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0},    // hor ir vert
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}   // istrizai
    };
    for (int d = 0; d < 8; d++) {
        int e = eil + directions[d][0];
        int s = stulp + directions[d][1];
        bool foundOpponent = false;

        while (e >= 0 && e < 8 && s >= 0 && s < 8 && board[e][s] == opponent) {
            foundOpponent = true;
            e += directions[d][0];
            s += directions[d][1];
        }
        if (foundOpponent && e >= 0 && e < 8 && s >= 0 && s < 8 && board[e][s] == player) {
            e = eil + directions[d][0];
            s = stulp + directions[d][1];
            while (board[e][s] == opponent) {
                board[e][s] = player;  // Flip
                cout << "Flipping (" << e + 1 << "," << s + 1 << ") from " << opponent << " to " << player << endl; // Debug output
                e += directions[d][0];
                s += directions[d][1];
            }
        }
    }
}

bool isValidMove(char player, int eil, int stulp) {
    if (eil < 0 || eil >= 8 || stulp < 0 || stulp >= 8 || board[eil][stulp] != '.') {
        return false;
    }
    char opponent = (player == 'B') ? 'W' : 'B';
    const int directions[8][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0},    // hor ir vert
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}   // istrizai
    };
    for (int d = 0; d < 8; d++) {
        int e = eil + directions[d][0];
        int s = stulp + directions[d][1];
        bool foundOpponent = false;

        while (e >= 0 && e < 8 && s >= 0 && s < 8) {
            if (board[e][s] == opponent) {
                foundOpponent = true;
            }
            else if (board[e][s] == player) {
                if (foundOpponent) {
                    return true; 
                }
                break;
            }
            else {
                break;
            }
            e += directions[d][0];
            s += directions[d][1];
        }
    }
    return false;
}


    bool hasValidMove(char player) {
        for (int eil = 0; eil < 8; eil++) {
            for (int stulp = 0; stulp < 8; stulp++) {
                if (isValidMove(player, eil, stulp)) {
                    return true;
                }
            }
        }
        return false;
    }
    char getPiece(int eil, int stulp) const {
        return board[eil][stulp];
    }
};

// pagr zaidimo logika
class Game {
private:
    Board board;
    char currPlayer;

public:
    Game() {
        currPlayer = rand() % 2 == 0 ? 'B' : 'W'; //random zaidejas
    }
    void play() {
        cout << endl;
        cout << "Žaidėjas " << currPlayer << " Pradeda!" << endl;
        cout << "Norint pradėt žaidimą paspauskite ENTER :)";
        cin.ignore();
        cin.get();  //kad printintu kas pradeda

        bool gameEnd = false;
        while (!gameEnd) {
            clearScreen();
            board.print();

            if (!board.hasValidMove(currPlayer)) {
                cout << endl;
                cout << " Žaidėjas " << currPlayer << " neturi galiojančių judesių :(. Pereinama prie kito žaidėjo." << endl;
                cout << endl;
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
            int eil, stulp;
            bool validInput = false;

            while (!validInput) {
                cout << endl;
                cout << "Įveskite ėjimą (eil stulp) žaidėjui " << currPlayer << ": ";
                cout << endl;
                cin >> eil >> stulp;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << endl;
                    cout << "Error! Įveskite skaičius." << endl;
                    cout << endl;
                    continue;
                }
                eil--;
                stulp--;
                if (board.isValidMove(currPlayer, eil, stulp)) {
                    board.addMove(currPlayer, eil, stulp);
                    board.flipPieces(currPlayer, eil, stulp);

                    validInput = true;
                    currPlayer = (currPlayer == 'B') ? 'W' : 'B';
                }
                else {
                    cout << "Error! Netinkama įvestis :(." << endl;
                }
            }
            if (!board.hasValidMove('B') && !board.hasValidMove('W')) {
                gameEnd = true;
            }
        }
        clearScreen();
        board.print();
        Winner();
    }


    void Winner() {
        int blackScore = 0, whiteScore = 0;

        for (int eil = 0; eil < 8; eil++) {
            for (int stulp = 0; stulp < 8; stulp++) {
                if (board.getPiece(eil, stulp) == 'B') {
                    blackScore++;
                }
                else if (board.getPiece(eil, stulp) == 'W') {
                    whiteScore++;
                }
            }
        }

        cout << "\nGame over!\n";
        cout << "Galutiniai rezultatai:\n";
        cout << "Žaidėjas B (black): " << blackScore << " taškai\n";
        cout << "Žaidėjas W (white): " << whiteScore << " taškai\n";
        
        if (blackScore > whiteScore) {
            cout << "Žaidėjas B (black) laimėjo!" << endl;
        }
        else if (whiteScore > blackScore) {
            cout << "Žaidėjas W(white) laimėjo!" << endl;
        }
        else {
            cout << "Lygiosios!" << endl;
        }
    }
};
void clearScreen() {
    system(CLEAR_SCREEN);
}
void displayMenu() {
    int choice;
    bool exitGame = false;

    while (!exitGame) {
        cout << "\nWelcome to REVERSI!" << endl;
        cout << "1. Žaisti" << endl;
        cout << "2. Taisyklės" << endl;
        cout << "3. Išeiti" << endl;
        cout << endl;
        cout << "Įveskite pasirinkimą: ";
        cout << endl;

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "ERROR! :( Įveskite skaičių." << endl;
            continue;
        }
        switch (choice) {
        case 1: {
            Game game;
            game.play();
            break;
        }
        case 2:
            cout << "Taisyklės: " << endl;
            cout << endl;
            cout << "1. Tikslas yra užimti didžiąją dalį lentos savo spalvos diskais." << endl;
            cout << endl;
            cout << "2. Norint apversti kito žaidėjo diską, reikia jį suspausti tarp savo spalvos 2 diskų." << endl;
            cout << endl;
            cout << "3. Judėti galima horizontaliai, vertikaliai ir įstrižai" << endl;
            cout << endl;
            cout << "4. Žaidimas baigiasi, kai nė vienas žaidėjas nebeturi kur judėti. Žaidėjas su daugiausia savo spalvos diskų laimi." << endl;
            cout << endl;
            break;
        case 3:
            cout << "Iki..." << endl;
            exitGame = true;
            break;
        default:
            cout << "ERROR! Netinkama įvestis :(" << endl;
        }
    }
}
int main() {
    setlocale(LC_ALL, "lt_LT.UTF-8");
    srand(time(0)); // Seed
    displayMenu();
    return 0;
}
