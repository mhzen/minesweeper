#include <iostream>
#include <limits>
#include <fstream>

using namespace std;

// config
const int ROWS = 9;
const int COLS = 9;
const int MINES = 5;
const char HIDDEN = '#';
const char BOMB = '*';
const char EMPTY = '.';
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

// Kalkulasi angka berdasarkan mine disekitarnya
void calculateNumbers(char realBoard[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            // Jika itu mine, lewati
            if (realBoard[r][c] == BOMB) continue;

            int count = 0;
            // Cek 8 tetangganya
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int nr = r + i;
                    int nc = c + j;
                    if (isValid(nr, nc) && realBoard[nr][nc] == BOMB) {
                        count++;
                    }
                }
            }
            realBoard[r][c] = static_cast<char>(count + '0'); // Convert (static cast) int ke char: https://www.geeksforgeeks.org/cpp/cpp-program-for-int-to-char-conversion/
        }
    }
}

// Inisialisasi board dengan nilai default di config
void initBoards(char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            viewBoard[i][j] = HIDDEN; // ngikut config
            realBoard[i][j] = '0'; // Default nya 0 tetangga (Kalo bukan mine)
        }
    }
    placeMines(realBoard);
    calculateNumbers(realBoard);
}


// Fungsi loading dari file
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
        cout << "Save file berhasil dimuat." << endl;
        return true;
    } else {
        cout << "Save file tidak ditemukan." << endl;
        return false;
    }
}

// Fungsi saving ke file
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
        cout << "Game disimpan ke " << SAVEFILE << endl;
    } else {
        cout << "Error menyimpan save file." << endl;
    }
}

// Display board ke terminal
void printBoard(char viewBoard[ROWS][COLS]) {
    // Print header column
    cout << endl << "    "; // agar angka align di print whitespace itu

    // Printing angka di kolom
    for (int j = 1; j <= COLS; j++) {
        cout << j << " ";
        if (j < 10) cout << " "; // spacing
    }
    cout << endl;

    // garis2 di bawahnya angka
    cout << "   ";
    for (int j = 0; j < COLS; j++) cout << "---";
    cout << endl;

    for (int i = 0; i < ROWS; i++) {
        // Printing huruf di baris (A, B, C...) dan garis | di kananya
        // ini jujur gtw knp harus di konversi ke char (padahal keduanya udh char),
        // kalo gak di static_cast gitu malah jadi int gtw knp
        cout << static_cast<char>('A' + i) << " | ";

        // Printing viewboard nya itu sendiri
        for (int j = 0; j < COLS; j++) {
            cout << viewBoard[i][j] << "  ";
        }
        cout << endl;
    }
    cout << endl;
}

// Toggle flag di cell tertentu
void toggleFlag(int r, int c, char viewBoard[ROWS][COLS]) {
    if (viewBoard[r][c] == HIDDEN) {
        viewBoard[r][c] = 'F';
    } else if (viewBoard[r][c] == 'F') {
        viewBoard[r][c] = HIDDEN;
    } else {
        cout << "Tidak bisa menandai cell yang sudah dibuka." << endl;
    }
}

// Reveal rekursif untuk membuka area kosong
void revealCell(int r, int c, char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    // base case
    if (!isValid(r, c)) return;
    if (viewBoard[r][c] != HIDDEN) return; // sudah dibuka / di flag

    // reveal cell saat ini
    viewBoard[r][c] = realBoard[r][c];

    // Jika kosong, lanjut reveal tetangga
    if (realBoard[r][c] == '0') {
        // ganti 0 ke EMPTY
        viewBoard[r][c] = EMPTY;

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                revealCell(r + i, c + j, realBoard, viewBoard);
            }
        }
    }
}

// Cek kemenangan
bool checkWin(char realBoard[ROWS][COLS], char viewBoard[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Jika ada satu saja yang bukan mine tapi masih hidden/flagged, berarti belum menang
            if (realBoard[i][j] != BOMB && (viewBoard[i][j] == HIDDEN || viewBoard[i][j] == 'F')) {
                return false;
                break; // Langsung keluar loop jika ketemu satu saja
            }
        }
    }
    return true;
}

// Bersihkan board saat game over (ganti '0' ke '.') di realBoard
void cleanBoard(char realBoard[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (realBoard[i][j] == '0') {
                realBoard[i][j] = EMPTY;
            }
        }
    }
}

int main(){
    srand(time(0)); // Inisialisasi seed random
    char realBoard[ROWS][COLS]; // Yang diproses internal
    char viewBoard[ROWS][COLS]; // Yang diliat user
    bool gameOver = 0;
    char command; // Command dari input "r 12A" -> r nya
    int colInput; // 12 nya
    char rowInput; // A nya
    bool gameRunning = 1; // true terus sampai quit / menang / kalah

    int choice;
    do {
        cout << "Minesweeper Game" << endl;
        cout << "Apa yang ingin dilakukan?" << endl;
        cout << "1. New Game" << endl;
        cout << "2. Load Game" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (cin.fail()) {
            cout << "Input invalid! Masukkan angka." << endl;
            cin.clear(); // handle invalid input biar gak infinite loop: https://stackoverflow.com/questions/25020129/cin-ignorenumeric-limitsstreamsizemax-n
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }
    } while (choice != 1 && choice != 2);
    // ngeloop terus sampe inputnya bener

    bool loaded = false;
    if (choice == 2) {
        loaded = loadGame(realBoard, viewBoard);
        if (!loaded) {
            cout << "Memulai game baru..." << endl;
        }
    }
    if (!loaded) {
        initBoards(realBoard, viewBoard);
    }

    while (gameRunning) {
        printBoard(viewBoard);

        cout << endl << "Commands: 'r' (reveal), 'f' (flag), 's' (save), 'q' (quit)" << endl;
        cout << "Format: [cmd] [col][row] (cth., 'r 5C' atau 'f 10A')" << endl;
        cout << "Masukkan command: ";
        cin >> command;

        cout << endl << "----------------------------------------" << endl;

        if (command != 'r' && command != 'f' && command != 's' && command != 'q' ){
            cout << "Command tidak valid!" << endl;
            cout << "----------------------------------------" << endl;
            continue;
        }

        // kalo s/q cuman diambil satu karakter awal gak peduli lanjutanya
        if (command == 's') {
            saveGame(realBoard, viewBoard);
            continue;
        }
        if (command == 'q') {
            cout << "Quitting game." << endl;
            break;
        }

        // Baca kordinat
        cin >> colInput >> rowInput;

        if (cin.fail()){
            cout << "Kordinat tidak valid! Masukkan angka (1-" << COLS << ") "
                 << "diikuti dengan huruf (A-" << char('A' + ROWS - 1) << ").";
            cout << endl << "----------------------------------------" << endl;
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
            cout << "Kordinat tidak valid! Coba lagi." << endl;
            continue;
        }

        if (command == 'f') {
            toggleFlag(row, col, viewBoard);
        } else if (command == 'r') {
            // Cek jika membuka mine
            if (realBoard[row][col] == BOMB) {
                cleanBoard(realBoard);
                printBoard(realBoard); // Tampilkan board asli saat game over
                cout << endl
                     << "BOOM! Anda terkena mine di " << colInput << rowInput << "! (つ╥﹏╥)つ" << endl;
                cout << "Game Over." << endl;
                gameRunning = false;
            } else {
                revealCell(row, col, realBoard, viewBoard);
                if (checkWin(realBoard, viewBoard)) {
                    printBoard(viewBoard);
                    cout << endl
                         << "CONGRATULATIONS! Anda berhasil membersihkan area mine! ദ്ദി ˉ͈̀꒳ˉ͈́ )✧" << endl;
                    gameRunning = false;
                }
            }
        }
    }
}
