#include <array>
#include <bitset>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>

#ifdef _WIN32
#define WINDOWS_LEAN_AND_MEAN 1
#define WINDOWS_NO_MINMAX 1
#include <windows.h>
#endif

using namespace std;

struct piece {
    bitset<2> color;
    char piece;
    int worth;
};
piece emptypiece = {0b00, ' ', 0};

map<char, string> piecesmap = {
    {'r', "♖"}, {'n', "♘"}, {'b', "♗"}, {'q', "♕"}, {'k', "♔"}, {'p', "♙"},

    {'R', "♜"}, {'N', "♞"}, {'B', "♝"}, {'Q', "♛"}, {'K', "♚"}, {'P', "♟"},

    {' ', " "},
};

struct move {
    piece *piece;
    unsigned int fromrow : 3;
    unsigned int fromcol : 3;

    unsigned int torow : 3;
    unsigned int tocol : 3;
};

struct board {
    piece pieces[8][8]{
        {{0b01, 'r', 500},
         {0b01, 'n', 300},
         {0b01, 'b', 300},
         {0b01, 'k', 10000},
         {0b01, 'q', 900},
         {0b01, 'b', 300},
         {0b01, 'n', 300},
         {0b01, 'r', 500}},
        {{0b01, 'p', 100},
         {0b01, 'p', 100},
         {0b01, 'p', 100},
         {0b01, 'p', 100},
         {0b01, 'p', 100},
         {0b01, 'p', 100},
         {0b01, 'p', 100},
         {0b01, 'p', 100}},

        {{0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0}},
        {{0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0}},
        {{0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0}},
        {{0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0},
         {0b00, ' ', 0}},

        {{0b11, 'p', 100},
         {0b11, 'p', 100},
         {0b11, 'p', 100},
         {0b11, 'p', 100},
         {0b11, 'p', 100},
         {0b11, 'p', 100},
         {0b11, 'p', 100},
         {0b11, 'p', 100}},
        {{0b11, 'r', 500},
         {0b11, 'n', 300},
         {0b11, 'b', 300},
         {0b11, 'k', 10000},
         {0b11, 'q', 900},
         {0b11, 'b', 300},
         {0b11, 'n', 300},
         {0b11, 'r', 500}},
    };

    bool whosetomove = false;

    bool whoami = false;

    bitset<4> castling{"1111"};

    int eval() {
        int black = 0;
        int white = 0;

        for (piece(&row)[8] : pieces) {
            for (piece &piece : row) {
                if (piece.color[0] == 0)
                    continue;
                if (piece.color[1] == 0)
                    white += piece.worth;
                else if (piece.color[1] == 1)
                    black += piece.worth;
            }
        }
        cout << "White: " << white << " Black: " << black << endl;
        return white - black;
    }

    void move(string &move) {
        pieces[move[3] - '1'][move[2] - 'a'] = pieces[move[1] - '1'][move[0] - 'a'];
        pieces[move[1] - '1'][move[0] - 'a'] = emptypiece;

        cout << "Moving from " << move[0] << move[1] << " to " << move[2] << move[3] << endl;
    }

    void move(::move &move) {
        pieces[move.torow][move.tocol] = *move.piece;
        // pieces[move.fromrow][move.fromcol] = emptypiece;

        cout << "Moving from " << move.fromrow << " " << move.fromcol << " to " << move.torow << " " << move.tocol
             << endl;
    }

    void draw() {
        cout << "row\r\n↓";
        for (int i = 0; i < 9; i++) {
            cout << "\x1B[49m" << endl;
            if (i == 8) {
                cout << "\x1B[49m  \uFF10\uFF11\uFF12\uFF13\uFF14\uFF15\uFF16\uFF17 ← col  " << endl;
            } else {
                string numbs[8] = {"\uFF10", "\uFF11", "\uFF12", "\uFF13", "\uFF14", "\uFF15", "\uFF16", "\uFF17"};
                cout << "\x1B[49m" << numbs[7 - i];
                for (int k = 0; k < 8; ++k) {
                    piece &piece = pieces[7 - i][k];
                    cout << ((i % 2 ? !(k % 2) : (k % 2)) ? "\x1B[40m" : "\x1B[100m")                      // color
                         << piecesmap[(char)(piece.color[1] ? piece.piece : toupper(piece.piece))] << " "; // piece
                }
            }
        }
    }

    int calcworth(int row, int col) {
        return 0;
    }

    string toFen() {
        char fen[90];

        struct {
            unsigned int fenpos : 7;
            unsigned int empty : 3;
        } state = {0, 0};

        // for (piece(&row)[8] : pieces) {
        for (int i = 0; i < 8; i++) {
            piece(&row)[8] = pieces[7 - i];
            for (piece &piece : row) {
                cout << state.fenpos << " " << state.empty << endl;
                if (piece.color[0] == 0) {
                    state.empty++;
                    continue;
                }
                if (state.empty) {
                    fen[state.fenpos++] = state.empty + '0';
                    state.empty = 0;
                }
                if (piece.color[1] == 0) {
                    fen[state.fenpos++] = toupper(piece.piece);
                    continue;
                } else if (piece.color[1] == 1) {
                    fen[state.fenpos++] = tolower(piece.piece);
                    continue;
                }
            }
            if (state.empty) {
                fen[state.fenpos++] = state.empty + '0';
                state.empty = 0;
            }
            if (i < 7)
                fen[state.fenpos++] = '/';
            else
                fen[state.fenpos++] = ' ';
        }
        fen[state.fenpos++] = whosetomove ? 'b' : 'w';
        fen[state.fenpos++] = ' ';
        if (castling[0])
            fen[state.fenpos++] = 'K';
        if (castling[1])
            fen[state.fenpos++] = 'Q';
        if (castling[2])
            fen[state.fenpos++] = 'k';
        if (castling[3])
            fen[state.fenpos++] = 'q';
        if (fen[state.fenpos - 1] == ' ')
            fen[state.fenpos++] = '-';

        return fen;
    };

    vector<::move> possibleMoves() {
        vector<::move> moves;

        // for (piece(&row)[8] : pieces) {
        for (int rownumb = 0; rownumb < 8; ++rownumb) {
            piece(&row)[8] = pieces[rownumb];
            for (int colnumb = 0; colnumb < 8; ++colnumb) {
                piece &piece = row[colnumb];

                // cout << whosetomove << endl;
                if (piece.color[0] == 0)
                    continue;
                if (piece.color[1] != whosetomove)
                    continue;

                switch (piece.piece) {
                case 'p': {
                    if (piece.color[1]) {
                        // black

                        // simply 1 forward
                        if (rownumb - 1 > -1 && pieces[rownumb - 1][colnumb].color[0] == 0) {
                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb + 1),
                                             static_cast<unsigned int>(colnumb)});
                        };
                        // 2 forward, but only if on starting row
                        if (rownumb == 6 && rownumb - 2 > -1 && pieces[rownumb - 2][colnumb].color[0] == 0) {
                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb + 2),
                                             static_cast<unsigned int>(colnumb)});
                        }
                    } else {
                        // white
                        if (rownumb + 1 < 8 && pieces[rownumb + 1][colnumb].color[0] == 0) {
                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb + 1),
                                             static_cast<unsigned int>(colnumb)});
                        };
                        if (rownumb == 1 && rownumb + 2 < 8 && pieces[rownumb + 2][colnumb].color[0] == 0) {
                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb + 2),
                                             static_cast<unsigned int>(colnumb)});
                        }
                    }
                    break;
                }
                case 'r': {

                    break;
                }
                case 'n': {

                    break;
                }
                case 'b': {

                    break;
                }
                case 'q': {

                    break;
                }
                case 'k': {

                    break;
                }
                }
            }
        }

        // moves.push_back({&pieces[1][0], 1, 0, 3, 0});

        return moves;
    }

    void loadfen(string fen) {
        // clear board
        for (piece(&row)[8] : pieces) {
            for (piece &piece : row) {
                piece = emptypiece;
            }
        }
        castling = bitset<4>{"0000"};

        int row = 7;
        int col = 0;
        int stage = 0;
        for (char &c : fen) {
            if (stage == 0) {
                if (c == '/') {
                    col = 0;
                    row--;
                    continue;
                }
                if (c > 96 && c < 123) {
                    // lowercase char, so it's black
                    pieces[row][col] = {0b11, (char)tolower(c), 0};
                    col++;
                    continue;
                }
                if (c > 64 && c < 91) {
                    // uppercase char, so it's white
                    pieces[row][col] = {0b01, (char)tolower(c), 0};
                    col++;
                    continue;
                }
                if (c > 47 && c < 58) {
                    // number, so skip that many squares
                    col += c - 48;
                    continue;
                }

                if (c == ' ') {
                    stage++;
                    continue;
                }
            } else if (stage == 1) {
                if (c == 'w') {
                    whosetomove = false;
                    continue;
                } else if (c == 'b') {
                    whosetomove = true;
                    continue;
                } else if (c == ' ') {
                    stage++;
                    continue;
                }
            } else if (stage == 2) {
                if (c == '-') {
                    continue;
                } else if (c == 'K') {
                    castling[0] = 1;
                    continue;
                } else if (c == 'Q') {
                    castling[1] = 1;
                    continue;
                } else if (c == 'k') {
                    castling[2] = 1;
                    continue;
                } else if (c == 'q') {
                    castling[3] = 1;
                    continue;
                } else if (c == ' ') {
                    stage++;
                    continue;
                }
            }
        }
    }
};

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    board board;

    set<string_view> args{argv + 1, argv + argc};
    if (args.contains("--black")) {
        cout << "I'm black" << endl;
        board.whoami = true;
    }

    // let's for now just assume I'm white (hmm, that sounds kinda racist)
    for (string input_move; true;) {
        getline(cin, input_move);
        // input_move = "e2e4";
        if (input_move.length() != 4) {
            exit(EXIT_FAILURE);
            // continue;
        }

        // board.move(input_move);
        // board.loadfen("r1b1kbnr/pp1qpppp/n1p5/3pP3/3P4/5N2/PPP2PPP/RNBQKB1R w KQkq - 3 5");
        board.loadfen("r1bk1bnr/pp1q1Ppp/n1p1p3/3p4/3P4/5N2/PPP2PPP/RNBQKB1R w KQ - 0 7");
        for (::move &move : board.possibleMoves()) {
            // cout << move.piece->piece << "  r" << move.fromrow << "c" << move.fromcol << " ("
            //      << (char)(move.fromcol + 'a') << (char)(move.fromrow + '1') << ")"
            //      << "  r" << move.torow << "c" << move.tocol << " (" << (char)(move.tocol + 'a')
            //      << (char)(move.torow + '1') << ")" << endl;
            cout
                << piecesmap[move.piece->color[1] ? (char)tolower(move.piece->piece) : (char)toupper(move.piece->piece)]
                << " " << (char)(move.fromcol + 'a') << (char)(move.fromrow + '1') << "→" << (char)(move.tocol + 'a')
                << (char)(move.torow + '1') << "\x1B[2m\x1B[90m              " << move.fromrow << move.fromcol << " "
                << move.torow << move.tocol << "\x1B[0m\x1B[49m" << endl;
        };
        cout << endl;

        // board.move(board.possibleMoves()[0]);

        board.draw();

        cout << board.eval() << endl;

        exit(EXIT_SUCCESS);
    }
}