#include <iostream>
#include <limits>
#include <fstream>

using namespace std;

// config
const int ROWS = 9;
const int COLS = 9;
const int MINES = 10;
const char HIDDEN = '#';
const char BOMB = '💣';
const string SAVEFILE = "minesweeper_save.txt";

// Menaruk mine secara random
void placeMines(char realBoard[ROWS][COLS]) {
    int minesPlaced = 0;
    while (minesPlaced < MINES) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (realBoard[r][c] != BOMB) {
            realBoard[r][c] = BOMB;
            minesPlaced++;
        }
    }
}

// Validasi Kordinat apakah ada didalam board
bool isValid(int r, int c) {
    return (r >= 0 && r < ROWS && c >= 0 && c < COLS);
}

// Calculate numbers based on surrounding mines
void calculateNumbers(char realBoard[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            // If it's a mine, skip it
            if (realBoard[r][c] == BOMB) continue;

            int count = 0;
            // Check all 8 neighbors
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int nr = r + i;
                    int nc = c + j;
                    if (isValid(nr, nc) && realBoard[nr][nc] == BOMB) {
                        count++;
                    }
                }
            }
            realBoard[r][c] = count + '0'; // Convert int to char
        }
    }
}

// Initialize boards with default values
void initBoards(char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            viewBoard[i][j] = HIDDEN; // '.' represents a covered cell
            realBoard[i][j] = '0'; // Default to 0 neighbors
        }
    }
    placeMines(realBoard);
    calculateNumbers(realBoard);
}

// Load state from file
bool loadGame(char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    ifstream inFile(SAVEFILE);
    if (inFile.is_open()) {
        // Load real board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                inFile >> realBoard[i][j];
            }
        }
        // Load view board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                inFile >> viewBoard[i][j];
            }
        }
        inFile.close();
        cout << "Game loaded successfully." << endl;
        return true;
    } else {
        cout << "No save file found." << endl;
        return false;
    }
}

// Save state to file
void saveGame(char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    ofstream outFile(SAVEFILE);
    if (outFile.is_open()) {
        // Save real board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                outFile << realBoard[i][j];
            }
            outFile << endl;
        }
        // Save view board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                outFile << viewBoard[i][j];
            }
            outFile << endl;
        }
        outFile.close();
        cout << "Game saved to " << SAVEFILE << endl;
    } else {
        cout << "Error saving file." << endl;
    }
}

// Display the board to the console
void printBoard(char viewBoard[ROWS][COLS]) {
    // Print column headers
    cout << "\n    ";
    for (int j = 1; j <= COLS; j++) {
        cout << j << " ";
        if (j < 10) cout << " "; // Alignment spacing
    }
    cout << endl;

    cout << "   ";
    for (int j = 0; j < COLS; j++) cout << "---";
    cout << endl;

    for (int i = 0; i < ROWS; i++) {
        // Print Row Header (A, B, C...)
        cout << (char)('A' + i) << " | ";

        for (int j = 0; j < COLS; j++) {
            cout << viewBoard[i][j] << "  ";
        }
        cout << endl;
    }
    cout << endl;
}

// Place or remove a flag
void toggleFlag(int r, int c, char viewBoard[ROWS][COLS]) {
    if (viewBoard[r][c] == HIDDEN) {
        viewBoard[r][c] = 'F';
    } else if (viewBoard[r][c] == 'F') {
        viewBoard[r][c] = HIDDEN;
    } else {
        cout << "Cannot flag a revealed cell." << endl;
    }
}

// Recursive flood fill to reveal empty areas
void revealCell(int r, int c, char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    // Base cases for recursion
    if (!isValid(r, c)) return;
    if (viewBoard[r][c] != HIDDEN) return; // Already revealed or flagged

    // Reveal the current cell
    viewBoard[r][c] = realBoard[r][c];

    // If it's a '0', recursively reveal neighbors
    if (realBoard[r][c] == '0') {
        // Change '0' to space ' ' for cleaner look on board
        viewBoard[r][c] = ' ';

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                revealCell(r + i, c + j, realBoard, viewBoard);
            }
        }
    }
}

// Check if the player won
bool checkWin(char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // If a cell is NOT a mine, but it is still hidden ('.') or flagged ('F'),
            // the game is not won yet.
            if (realBoard[i][j] != BOMB && (viewBoard[i][j] == HIDDEN || viewBoard[i][j] == 'F')) {
                return false;
            }
        }
    }
    return true;
}

int main(){
    srand(time(0));
    char realBoard[ROWS][COLS]; // Yang diproses internal
    char viewBoard[ROWS][COLS]; // Yang diliat user
    bool gameOver = 0;
    char command; // Command dari input "r 12A" -> r nya
    int colInput; //
    char rowInput;
    bool gameRunning = 1;

    int choice;
    do {
        cout << "Minesweeper Game" << endl;
        cout << "Apa yang ingin dilakukan?" << endl;
        cout << "1. New Game" << endl;
        cout << "2. Load Game" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (cin.fail()) {
            cout << "Invalid input! Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }
    } while (choice != 1 && choice != 2);
    // ngeloop sampe inputnya bener

    if (choice == 2 && !loadGame(realBoard, viewBoard)){
        cout << "Starting new game instead..." << endl;
    }
    initBoards(realBoard, viewBoard);

    while (gameRunning) {
        printBoard(viewBoard);

        cout << endl << "Commands: 'r' (reveal), 'f' (flag), 's' (save), 'q' (quit)" << endl;
        cout << "Format: [cmd] [col][row] (e.g., 'r 5C' or 'f 10A')" << endl;
        cout << "Enter command: ";
        cin >> command;

        cout << endl << "----------------------------------------" << endl;

        // kalo s/q cuman diambil satu karakter gak peduli lanjutanya
        if (command == 's') {
            saveGame(realBoard, viewBoard);
            continue;
        }
        if (command == 'q') {
            cout << "Quitting game." << endl;
            break;
        }

        // Read coordinates
        cin >> colInput >> rowInput;

        if (cin.fail()){
            cout << "Kordinat tidak valid! Masukkan angka (1-" << COLS << ") "
                 << "diikuti dengan huruf (A-" << char('A' + ROWS - 1) << ").";
            cout << endl << "----------------------------------------" << endl;
            // cout << "Invalid coordinates! Please enter a letter (A-" << char('A' + ROWS - 1)
            //      << ") followed by a number (1-" << COLS << ")." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // Convert Inputs: Row 'A' -> 0, Col '1' -> 0

        int col = colInput - 1;

        int row = -1;
        // Handle lowercase or uppercase row input
        if (rowInput >= 'A' && rowInput <= 'Z') {
            row = rowInput - 'A';
        } else if (rowInput >= 'a' && rowInput <= 'z') {
            row = rowInput - 'a';
        }

        // Validate
        if (!isValid(row, col)) {
            cout << "Invalid coordinates! Try again." << endl;
            continue;
        }

        if (command == 'f') {
            toggleFlag(row, col, viewBoard);
        } else if (command == 'r') {
            // Check if user hit a mine
            if (realBoard[row][col] == BOMB) {
                printBoard(realBoard); // Show current state
                cout << endl << "BOOM! You hit a mine at " << colInput << rowInput << "!" << endl;
                cout << "Game Over." << endl;
                gameRunning = false;
            } else {
                revealCell(row, col, realBoard, viewBoard);
                if (checkWin(realBoard, viewBoard)) {
                    printBoard(viewBoard);
                    cout << endl << "CONGRATULATIONS! You cleared the minefield!" << endl;
                    gameRunning = false;
                }
            }
        }
    }
}