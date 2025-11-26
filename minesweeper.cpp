#include <bits/stdc++.h>
using namespace std;

struct Cell {
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int neighbors = 0;
};

// Utilities
static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Convert a row letter (A-Z) to index (0-based). Returns -1 if invalid.
int rowFromLetter(char L) {
    if (!isalpha((unsigned char)L)) return -1;
    L = toupper((unsigned char)L);
    return L - 'A';
}

string letterFromRow(int r) {
    if (r < 0) return "?";
    char c = char('A' + r);
    return string(1, c);
}

// Parse coordinates like "12A" -> col 11, row 0
bool parseCoord(const string &token, int &row, int &col, int maxRows, int maxCols) {
    string t = trim(token);
    if (t.empty()) return false;
    // trailing letter(s) are row (we support single letter A..Z)
    int n = (int)t.size();
    int pos = n - 1;
    while (pos >= 0 && isalpha((unsigned char)t[pos])) pos--;
    if (pos == n - 1) return false; // no row letter
    string colStr = t.substr(0, pos + 1);
    string rowStr = t.substr(pos + 1);
    if (colStr.empty() || rowStr.empty()) return false;
    // only single letter row supported
    if (rowStr.size() != 1) return false;
    for (char ch : colStr) if (!isdigit((unsigned char)ch)) return false;
    int c = stoi(colStr) - 1; // convert to 0-based
    int r = rowFromLetter(rowStr[0]);
    if (r < 0) return false;
    if (r >= maxRows || c < 0 || c >= maxCols) return false;
    row = r; col = c;
    return true;
}

// Create an empty board
vector<vector<Cell>> createBoard(int rows, int cols) {
    return vector<vector<Cell>>(rows, vector<Cell>(cols));
}

// Place mines randomly
void placeMines(vector<vector<Cell>> &board, int mines, unsigned int seed = 0) {
    int rows = (int)board.size();
    int cols = (int)board[0].size();
    vector<int> idx(rows * cols);
    iota(idx.begin(), idx.end(), 0);
    std::mt19937 rng(seed ? seed : (unsigned)chrono::high_resolution_clock::now().time_since_epoch().count());
    shuffle(idx.begin(), idx.end(), rng);
    for (int i = 0; i < mines && i < (int)idx.size(); ++i) {
        int v = idx[i];
        int r = v / cols;
        int c = v % cols;
        board[r][c].mine = true;
    }
}

// Count neighbors for each cell
void computeNeighbors(vector<vector<Cell>> &board) {
    int rows = (int)board.size();
    int cols = (int)board[0].size();
    const int dr[8] = {-1,-1,-1,0,0,1,1,1};
    const int dc[8] = {-1,0,1,-1,1,-1,0,1};
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int cnt = 0;
            for (int k = 0; k < 8; ++k) {
                int nr = r + dr[k];
                int nc = c + dc[k];
                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                    if (board[nr][nc].mine) ++cnt;
                }
            }
            board[r][c].neighbors = cnt;
        }
    }
}

// Reveal using BFS for zeros
bool revealCell(vector<vector<Cell>> &board, int r, int c) {
    int rows = (int)board.size();
    int cols = (int)board[0].size();
    if (board[r][c].flagged || board[r][c].revealed) return true;
    board[r][c].revealed = true;
    if (board[r][c].mine) return false; // hit mine
    if (board[r][c].neighbors != 0) return true;
    // BFS to reveal neighbors
    queue<pair<int,int>> q;
    q.push({r,c});
    const int dr[8] = {-1,-1,-1,0,0,1,1,1};
    const int dc[8] = {-1,0,1,-1,1,-1,0,1};
    while (!q.empty()) {
        auto p = q.front(); q.pop();
        for (int k = 0; k < 8; ++k) {
            int nr = p.first + dr[k];
            int nc = p.second + dc[k];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                if (!board[nr][nc].revealed && !board[nr][nc].flagged) {
                    board[nr][nc].revealed = true;
                    if (board[nr][nc].neighbors == 0 && !board[nr][nc].mine) q.push({nr,nc});
                }
            }
        }
    }
    return true;
}

void toggleFlag(vector<vector<Cell>> &board, int r, int c) {
    if (board[r][c].revealed) return;
    board[r][c].flagged = !board[r][c].flagged;
}

bool checkWin(const vector<vector<Cell>> &board) {
    int rows = (int)board.size();
    int cols = (int)board[0].size();
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (!board[r][c].mine && !board[r][c].revealed) return false;
        }
    }
    return true;
}

// Print board with chess-like rows (A...) and numbered columns (1...)
void printBoard(const vector<vector<Cell>> &board, bool revealAll = false) {
    int rows = (int)board.size();
    int cols = (int)board[0].size();
    // column header
    cout << "   ";
    for (int c = 0; c < cols; ++c) {
        cout << setw(3) << (c + 1);
    }
    cout << '\n';
    for (int r = 0; r < rows; ++r) {
        cout << setw(3) << letterFromRow(r);
        for (int c = 0; c < cols; ++c) {
            const Cell &cell = board[r][c];
            char ch = '.';
            if (revealAll) {
                if (cell.mine) ch = '*';
                else if (cell.neighbors > 0) ch = char('0' + cell.neighbors);
                else ch = ' ';
            } else {
                if (cell.flagged) ch = 'F';
                else if (cell.revealed) {
                    if (cell.mine) ch = '*';
                    else if (cell.neighbors > 0) ch = char('0' + cell.neighbors);
                    else ch = ' ';
                } else ch = '.';
            }
            cout << setw(3) << ch;
        }
        cout << '\n';
    }
}

// Save format: first line rows cols mines (mines optional), then each row a sequence of triplets "mrf" where m=1|0 mine, r=1|0 revealed, f=1|0 flagged
bool saveGame(const vector<vector<Cell>> &board, const string &filename, int minesCount = -1) {
    ofstream ofs(filename);
    if (!ofs) return false;
    int rows = (int)board.size();
    int cols = (int)board[0].size();
    ofs << rows << ' ' << cols << ' ' << minesCount << "\n";
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const Cell &cell = board[r][c];
            ofs << (cell.mine ? '1' : '0')
                << (cell.revealed ? '1' : '0')
                << (cell.flagged ? '1' : '0');
        }
        ofs << "\n";
    }
    return true;
}

bool loadGame(vector<vector<Cell>> &board, const string &filename) {
    ifstream ifs(filename);
    if (!ifs) return false;
    int rows, cols, minesCount;
    ifs >> rows >> cols >> minesCount;
    string line;
    getline(ifs, line); // consume endline
    vector<vector<Cell>> tmp = createBoard(rows, cols);
    for (int r = 0; r < rows; ++r) {
        if (!getline(ifs, line)) return false;
        int expected = cols * 3;
        if ((int)line.size() < expected) return false;
        for (int c = 0; c < cols; ++c) {
            int pos = c * 3;
            tmp[r][c].mine = (line[pos] == '1');
            tmp[r][c].revealed = (line[pos+1] == '1');
            tmp[r][c].flagged = (line[pos+2] == '1');
        }
    }
    computeNeighbors(tmp);
    board = std::move(tmp);
    return true;
}

// Main game loop (kept short; all logic lives in functions outside)
int main() {
    cout << "Minesweeper (no pointers) - simple CLI\n";
    cout << "Enter rows (A..Z as count) and columns (numbers). Max 26 rows. Example 9 9\n";
    cout << "Or enter 'L filename' to load a saved game.\n";
    string cmd;
    vector<vector<Cell>> board;
    int rows = 9, cols = 9, mines = 10;
    bool loaded = false;
    cout << "> ";
    if (!getline(cin, cmd)) return 0;
    cmd = trim(cmd);
    if (!cmd.empty() && (cmd[0] == 'L' || cmd[0] == 'l')) {
        string fn = trim(cmd.substr(1));
        if (fn.empty()) fn = "save.ms";
        if (loadGame(board, fn)) {
            rows = (int)board.size();
            cols = (int)board[0].size();
            loaded = true;
            cout << "Loaded game from " << fn << "\n";
        } else {
            cout << "Failed to load, starting new\n";
        }
    } else {
        if (!cmd.empty()) {
            // try parse two integers
            istringstream iss(cmd);
            if (!(iss >> rows >> cols)) { rows = 9; cols = 9; }
        }
        cout << "Enter mines count (suggested " << max(1, (rows*cols)/6) << ") or press Enter for default: ";
        string mline; getline(cin, mline);
        if (!trim(mline).empty()) mines = stoi(trim(mline));
        board = createBoard(rows, cols);
        placeMines(board, mines);
        computeNeighbors(board);
    }

    bool lost = false;
    while (true) {
        printBoard(board, false);
        if (checkWin(board)) {
            cout << "You win!\n";
            printBoard(board, true);
            break;
        }
        if (lost) {
            cout << "You hit a mine. Game over.\n";
            printBoard(board, true);
            break;
        }
        cout << "Commands: r <col><RowLetter> | f <col><RowLetter> | s [filename] | q\n";
        cout << "> ";
        string line;
        if (!getline(cin, line)) break;
        line = trim(line);
        if (line.empty()) continue;
        // split
        istringstream iss(line);
        string token; iss >> token;
        if (token.empty()) continue;
        char action = token[0];
        if (action == 'q' || action == 'Q') break;
        if ((action == 'r' || action == 'R' || action == 'f' || action == 'F')) {
            string coord;
            if (!(iss >> coord)) {
                cout << "No coordinate provided. Example: r 12A\n";
                continue;
            }
            int r,c;
            if (!parseCoord(coord, r, c, (int)board.size(), (int)board[0].size())) {
                cout << "Invalid coordinate. Use <col><RowLetter> like 12A and ensure inside board.\n";
                continue;
            }
            if (action == 'f' || action == 'F') {
                toggleFlag(board, r, c);
            } else {
                // reveal
                if (!revealCell(board, r, c)) {
                    lost = true;
                }
            }
        } else if (action == 's' || action == 'S') {
            string filename;
            if (!(iss >> filename)) filename = "save.ms";
            if (saveGame(board, filename, mines)) cout << "Saved to " << filename << "\n";
            else cout << "Failed to save to " << filename << "\n";
        } else {
            cout << "Unknown command: " << action << "\n";
        }
    }
    cout << "Bye.\n";
    return 0;
}
