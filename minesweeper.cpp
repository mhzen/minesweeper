#include <iostream>
#include <limits>
#include <fstream>

using namespace std;

// config
const int ROWS = 9;
const int COLS = 9;
const int MINES = 5;
const char HIDDEN = '#';
const char BOMB = '💣';
const char EMPTY = '.';
const string SAVEFILE = "minesweeper_save.txt";

// Menaruk mine secara random
void letakbom(char papanasli[ROWS][COLS]) {
    int minesPlaced = 0;
    while (minesPlaced < MINES) {
        int r = rand() % ROWS;
        int c = rand() % COLS;
        if (papanasli[r][c] != BOMB) {
            papanasli[r][c] = BOMB;
            minesPlaced++;
        }
    }
}

// Validasi Kordinat apakah ada didalam board
bool validasikoordinat(int r, int c) {
    return (r >= 0 && r < ROWS && c >= 0 && c < COLS);
}

// Kalkulasi angka berdasarkan mine disekitarnya
void hitungbom(char papanasli[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            // Jika itu mine, lewati
            if (papanasli[r][c] == BOMB) continue;

            int count = 0;
            // Cek 8 tetangganya
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int nr = r + i;
                    int nc = c + j;
                    if (validasikoordinat(nr, nc) && papanasli[nr][nc] == BOMB) {
                        count++;
                    }
                }
            }
            papanasli[r][c] = static_cast<char>(count + '0'); // Convert (static cast) int ke char: https://www.geeksforgeeks.org/cpp/cpp-program-for-int-to-char-conversion/
        }
    }
}

// Inisialisasi board dengan nilai default di config
void inisiasipapan(char papanasli[ROWS][COLS], char papantampilan[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            papantampilan[i][j] = HIDDEN; // ngikut config
            papanasli[i][j] = '0'; // Default nya 0 tetangga (Kalo bukan mine)
        }
    }
    letakbom(papanasli);
    hitungbom(papanasli);
}


// Fungsi loading dari file
bool lanjutkan(char papanasli[ROWS][COLS], char papantampilan[ROWS][COLS]) {
    ifstream inFile(SAVEFILE);
    if (inFile.is_open()) {
        // Load real board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                inFile >> papanasli[i][j];
            }
        }
        // Load view board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                inFile >> papantampilan[i][j];
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
void simpan(char papanasli[ROWS][COLS], char papantampilan[ROWS][COLS]) {
    ofstream outFile(SAVEFILE);
    if (outFile.is_open()) {
        // Save real board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                outFile << papanasli[i][j];
            }
            outFile << endl;
        }
        // Save view board
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                outFile << papantampilan[i][j];
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
void cetakpapan(char papantampilan[ROWS][COLS]) {
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

        // Printing papantampilan nya itu sendiri
        for (int j = 0; j < COLS; j++) {
            cout << papantampilan[i][j] << "  ";
        }
        cout << endl;
    }
    cout << endl;
}

// Toggle flag di cell tertentu
void bendera(int r, int c, char papantampilan[ROWS][COLS]) {
    if (papantampilan[r][c] == HIDDEN) {
        papantampilan[r][c] = 'F';
    } else if (papantampilan[r][c] == 'F') {
        papantampilan[r][c] = HIDDEN;
    } else {
        cout << "Tidak bisa menandai cell yang sudah dibuka." << endl;
    }
}

// Reveal rekursif untuk membuka area kosong
void membukacell(int r, int c, char papanasli[ROWS][COLS], char papantampilan[ROWS][COLS]) {
    // base case
    if (!validasikoordinat(r, c)) return;
    if (papantampilan[r][c] != HIDDEN) return; // sudah dibuka / di flag

    // reveal cell saat ini
    papantampilan[r][c] = papanasli[r][c];

    // Jika kosong, lanjut reveal tetangga
    if (papanasli[r][c] == '0') {
        // ganti 0 ke EMPTY
        papantampilan[r][c] = EMPTY;

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                membukacell(r + i, c + j, papanasli, papantampilan);
            }
        }
    }
}

// Cek kemenangan
bool cekkemenangan(char papanasli[ROWS][COLS], char papantampilan[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Jika ada satu saja yang bukan mine tapi masih hidden/flagged, berarti belum menang
            if (papanasli[i][j] != BOMB && (papantampilan[i][j] == HIDDEN || papantampilan[i][j] == 'F')) {
                return false;
                break; // Langsung keluar loop jika ketemu satu saja
            }
        }
    }
    return true;
}

// Bersihkan board saat game over (ganti '0' ke '.') di papanasli
void gantiangkanol(char papanasli[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (papanasli[i][j] == '0') {
                papanasli[i][j] = EMPTY;
            }
        }
    }
}

int main(){
    srand(time(0)); // Inisialisasi seed random
    char papanasli[ROWS][COLS]; // Yang diproses internal
    char papantampilan[ROWS][COLS]; // Yang diliat user
    bool gameOver = 0;
    char command; // Command dari input "r 12A" -> r nya
    int inputkolom; // 12 nya
    char inputbaris; // A nya
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
        loaded = lanjutkan(papanasli, papantampilan);
        if (!loaded) {
            cout << "Memulai game baru..." << endl;
        }
    }
    if (!loaded) {
        inisiasipapan(papanasli, papantampilan);
    }

    while (gameRunning) {
        cetakpapan(papantampilan);

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
            simpan(papanasli, papantampilan);
            continue;
        }
        if (command == 'q') {
            cout << "Quitting game." << endl;
            break;
        }

        // Baca kordinat
        cin >> inputkolom >> inputbaris;

        if (cin.fail()){
            cout << "Kordinat tidak valid! Masukkan angka (1-" << COLS << ") "
                 << "diikuti dengan huruf (A-" << char('A' + ROWS - 1) << ").";
            cout << endl << "----------------------------------------" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // Convert Inputs: Row 'A' -> 0, Col '1' -> 0

        int col = inputkolom - 1;

        int row = -1;
        // Handle lowercase or uppercase row input
        if (inputbaris >= 'A' && inputbaris <= 'Z') {
            row = inputbaris - 'A';
        } else if (inputbaris >= 'a' && inputbaris <= 'z') {
            row = inputbaris - 'a';
        }

        // Validate
        if (!validasikoordinat(row, col)) {
            cout << "Kordinat tidak valid! Coba lagi." << endl;
            continue;
        }

        if (command == 'f') {
            bendera(row, col, papantampilan);
        } else if (command == 'r') {
            // Cek jika membuka mine
            if (papanasli[row][col] == BOMB) {
                gantiangkanol(papanasli);
                cetakpapan(papanasli); // Tampilkan board asli saat game over
                cout << endl
                     << "BOOM! Anda terkena mine di " << inputkolom << inputbaris << "! (つ╥﹏╥)つ" << endl;
                cout << "Game Over." << endl;
                gameRunning = false;
            } else {
                membukacell(row, col, papanasli, papantampilan);
                if (cekkemenangan(papanasli, papantampilan)) {
                    cetakpapan(papantampilan);
                    cout << endl
                         << "CONGRATULATIONS! Anda berhasil membersihkan area mine! ദ്ദി ˉ͈̀꒳ˉ͈́ )✧" << endl;
                    gameRunning = false;
                }
            }
        }
    }
}
