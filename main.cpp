#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <thread>
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

    int enpassantrow : 4 = -2;
    int enpassantcol : 4 = -2;

    char promotion = ' ';

    bool castling : 1 = false;

    bool attacking : 1 = true;

    int fap[2]{-2, -2};

    int eval = 0;
};

struct board {
    piece pieces[8][8]{
        {{0b01, 'r', 500},
         {0b01, 'n', 300},
         {0b01, 'b', 300},
         {0b01, 'k', 100000},
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
         {0b11, 'k', 100000},
         {0b11, 'q', 900},
         {0b11, 'b', 300},
         {0b11, 'n', 300},
         {0b11, 'r', 500}},
    };

    bool whosetomove = false;

    bool whoami = false;

    bitset<4> castling{"1111"};

    int enpassant[2]{-2, -2};

    void move(string const &move) {
        ::move cmove;

        if (pieces[move[1] - '1'][move[0] - 'a'].piece == 'k' &&
            (move == "e1h1" || move == "e1a1" || move == "e8h8" || move == "e8a8")) {
            // castling
            cmove = {&pieces[move[1] - '1'][move[0] - 'a'],
                     static_cast<unsigned int>(move[1] - '1'),
                     static_cast<unsigned int>(move[0] - 'a'),
                     static_cast<unsigned int>(move[3] - '1'),
                     static_cast<unsigned int>(move[2] - 'a'),
                     -2,
                     -2,
                     ' ',
                     true};
        } else if (pieces[move[1] - '1'][move[0] - 'a'].piece == 'p' && move[0] != move[2] &&
                   pieces[move[3] - '1'][move[2] - 'a'].piece == ' ') { // (if capturing empty piece)
            // enpassant
            cmove = {&pieces[move[1] - '1'][move[0] - 'a'],
                     static_cast<unsigned int>(move[1] - '1'),
                     static_cast<unsigned int>(move[0] - 'a'),
                     static_cast<unsigned int>(move[3] - '1'),
                     static_cast<unsigned int>(move[2] - 'a'),
                     move[3] - '1',
                     move[2] - 'a',
                     ' ',
                     false};
        } else if (move.length() == 5) {
            // promotion
            cmove = {&pieces[move[1] - '1'][move[0] - 'a'],
                     static_cast<unsigned int>(move[1] - '1'),
                     static_cast<unsigned int>(move[0] - 'a'),
                     static_cast<unsigned int>(move[3] - '1'),
                     static_cast<unsigned int>(move[2] - 'a'),
                     -2,
                     -2,
                     move[4],
                     false};
        } else {
            // if not any of the weird cases, just move the piece
            cmove = {&pieces[move[1] - '1'][move[0] - 'a'], static_cast<unsigned int>(move[1] - '1'),
                     static_cast<unsigned int>(move[0] - 'a'), static_cast<unsigned int>(move[3] - '1'),
                     static_cast<unsigned int>(move[2] - 'a')};
        }

        this->move(cmove);
    }

    void move(::move const &move) {
        // auto revoke enpassantability
        enpassant[0] = -2;
        enpassant[1] = -2;

        // auto grant enpassantability
        if (move.piece->piece == 'p') {
            if (move.fromrow == 1 && move.torow == 3) {
                enpassant[0] = move.fromcol;
                enpassant[1] = move.fromrow + 1;
            } else if (move.fromrow == 6 && move.torow == 4) {
                enpassant[0] = move.fromcol;
                enpassant[1] = move.fromrow - 1;
            }
        }

        // auto revoke castling rights
        if (move.piece->piece == 'k') {
            if (move.piece->color[1] == 0) {
                castling[0] = 0;
                castling[1] = 0;
            } else {
                castling[2] = 0;
                castling[3] = 0;
            }
        } else if (move.piece->piece == 'r') {
            if (move.piece->color[1] == 0) {
                if (move.fromrow == 0 && move.fromcol == 0) {
                    // left white rook
                    castling[1] = 0;
                } else if (move.fromrow == 0 && move.fromcol == 7) {
                    // right white rook
                    castling[0] = 0;
                }
            } else {
                if (move.fromrow == 7 && move.fromcol == 0) {
                    // left black rook
                    castling[3] = 0;
                } else if (move.fromrow == 7 && move.fromcol == 7) {
                    // right black rook
                    castling[2] = 0;
                }
            }
        }

        if (move.castling) {
            // castling
            if (move.tocol == 7) {
                // kingside

                // move king
                pieces[move.torow][6] = *move.piece;
                pieces[move.fromrow][move.fromcol] = emptypiece;

                // move rook
                pieces[move.torow][5] = pieces[move.torow][7];
                pieces[move.torow][move.tocol] = emptypiece;
            } else if (move.tocol == 0) {
                // queenside

                // move king
                pieces[move.torow][2] = *move.piece;
                pieces[move.fromrow][move.fromcol] = emptypiece;

                // move rook
                pieces[move.torow][3] = pieces[move.torow][0];
                pieces[move.torow][move.tocol] = emptypiece;
            }
        } else if (move.enpassantcol != -2) {
            // enpassant

            // move pawn
            pieces[move.torow][move.tocol] = *move.piece;
            pieces[move.fromrow][move.fromcol] = emptypiece;

            // remove captured pawn
            pieces[move.enpassantrow == 5 ? 4 : 3][move.enpassantcol] = emptypiece;
        } else if (move.promotion != ' ') {
            // promotion

            // move pawn
            pieces[move.torow][move.tocol] = *move.piece;
            pieces[move.fromrow][move.fromcol] = emptypiece;

            // change piece.piece to requested promotion piece
            pieces[move.torow][move.tocol].piece = move.promotion;
        }

        else {
            // if not any of the weird cases, just move the piece
            pieces[move.torow][move.tocol] = *move.piece;
            pieces[move.fromrow][move.fromcol] = emptypiece;
        }

        // whosetomove = !whosetomove;

        // cout << "Moving from " << (char)(move.fromcol + 'a') << (char)(move.fromrow + '1') << "→"
        //      << (char)(move.tocol + 'a') << (char)(move.torow + '1') << endl;
    }

    void actuallyMove(::move const &move) {
        cout << (char)(move.fromcol + 'a') << (char)(move.fromrow + '1') << (char)(move.tocol + 'a')
             << (char)(move.torow + '1');

        if (move.promotion != ' ') {
            cout << (char)toupper(move.promotion);
        }

        this->move(move);
        toggleWhoseToMove();

        cout << endl;
    }

    void toggleWhoseToMove() {
        whosetomove = !whosetomove;
    };

    void draw() {
        cerr << "row\r\n↓";
        for (int i = 0; i < 9; i++) {
            cerr << "\x1B[49m" << endl;
            if (i == 8) {
                // cerr << "\x1B[49m  \uFF10\uFF11\uFF12\uFF13\uFF14\uFF15\uFF16\uFF17 ← col  " << endl;
                cerr << "\x1B[49m  \uFF41\uFF42\uFF43\uFF44\uFF45\uFF46\uFF47\uFF48 ← col  " << endl;
            } else {
                // string numbs[8] = {"\uFF10", "\uFF11", "\uFF12", "\uFF13", "\uFF14", "\uFF15", "\uFF16", "\uFF17"};
                string numbs[8] = {"\uFF11", "\uFF12", "\uFF13", "\uFF14", "\uFF15", "\uFF16", "\uFF17", "\uFF18"};
                cerr << "\x1B[49m" << numbs[7 - i];
                for (int k = 0; k < 8; ++k) {
                    piece &piece = pieces[7 - i][k];
                    cerr << ((i % 2 ? !(k % 2) : (k % 2)) ? "\x1B[40m" : "\x1B[100m")                      // color
                         << piecesmap[(char)(piece.color[1] ? piece.piece : toupper(piece.piece))] << " "; // piece
                }
            }
        }
    }

    // int calcworth(int row, int col) {
    //     return 0;
    // }

    // warning: highly inefficient function, it was more or less only meant for testing, don't even try to read it,
    // it'll definitely hurt your eyes
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

    vector<::move> possibleMoves(const vector<tuple<int, int>> &selectPieces = {{-2, -2}}, bool select = false) {
        vector<::move> movesm;
        vector<::move> moveso;
        int kingrow = -2;
        int kingcol = -2;

        // normal piece movement
        for (int rownumb = 0; rownumb < 8; ++rownumb) {
            piece(&row)[8] = pieces[rownumb];
            for (int colnumb = 0; colnumb < 8; ++colnumb) {
                if (select && !(count_if(selectPieces.begin(), selectPieces.end(), [&](const tuple<int, int> &t) {
                        return get<0>(t) == rownumb && get<1>(t) == colnumb;
                    })))
                    continue;

                piece &piece = row[colnumb];

                // cout << whosetomove << endl;
                if (piece.color[0] == 0)
                    continue;

                vector<::move> &moves = (piece.color[1] != whosetomove) ? moveso : movesm;
                bool imo = piece.color[1] != whosetomove;
                // bool imo = false;

                // if (piece.color[1] != whosetomove)
                // continue;

                switch (piece.piece) {
                case 'p': {
                    if (piece.color[1]) {
                        // black

                        // simply 1 forward
                        if (rownumb - 1 > -1 && pieces[rownumb - 1][colnumb].color[0] == 0) {
                            if (rownumb - 1 == 0) {
                                // promotion
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'q', false, false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'n', false, false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'b', false, false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'r', false, false});
                            } else {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, ' ', false, false});
                            }

                            // 2 forward, but only if on starting row & not going through another piece
                            if (rownumb == 6 && rownumb - 2 > -1 && pieces[rownumb - 2][colnumb].color[0] == 0) {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 2),
                                                 static_cast<unsigned int>(colnumb), -2, -2, ' ', false, false});
                            }
                        };

                        // capturing, left or right
                        if (rownumb - 1 > -1 && colnumb - 1 > -1 &&
                            ((pieces[rownumb - 1][colnumb - 1].color[0] == 1 &&
                              pieces[rownumb - 1][colnumb - 1].color[1] != piece.color[1]) ||
                             imo)) {
                            if (rownumb - 1 == 0) {
                                // promotion
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'q', false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'n', false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'b', false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'r', false});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb - 1), static_cast<unsigned int>(colnumb - 1)});
                            }
                        }
                        if (rownumb - 1 > -1 && colnumb + 1 < 8 &&
                            ((pieces[rownumb - 1][colnumb + 1].color[0] == 1 &&
                              pieces[rownumb - 1][colnumb + 1].color[1] != piece.color[1]) ||
                             imo)) {
                            if (rownumb - 1 == 0) {
                                // promotion
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb - 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'q', false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'n', false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'b', false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'r', false});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb - 1), static_cast<unsigned int>(colnumb + 1)});
                            }
                        }

                        // en passant
                        if (rownumb == 3 && ((colnumb - 1) == enpassant[0] || (colnumb + 1) == enpassant[0])) {
                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(2),
                                             static_cast<unsigned int>(enpassant[0]), 3, enpassant[0]});
                        }
                    } else {
                        // white

                        // moving, simply 1 forward
                        if (rownumb + 1 < 8 && pieces[rownumb + 1][colnumb].color[0] == 0) {
                            if (rownumb + 1 == 7) {
                                // promotion
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'q', false, false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'n', false, false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'b', false, false});

                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, 'r', false, false});
                            } else {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb), -2, -2, ' ', false, false});
                            }

                            // 2 forward, but only if on starting row & not going through another piece
                            if (rownumb == 1 && rownumb + 2 < 8 && pieces[rownumb + 2][colnumb].color[0] == 0) {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 2),
                                                 static_cast<unsigned int>(colnumb), -2, -2, ' ', false, false});
                            }
                        };

                        // capturing, left or right
                        if (rownumb + 1 < 8 && colnumb - 1 > -1 &&
                            ((pieces[rownumb + 1][colnumb - 1].color[0] == 1 &&
                              pieces[rownumb + 1][colnumb - 1].color[1] != piece.color[1]) ||
                             imo)) {
                            // left
                            if (rownumb == 6) {
                                // promotion
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'q', false});
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'n', false});
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'b', false});
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb - 1), -2, -2, 'r', false});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb + 1), static_cast<unsigned int>(colnumb - 1)});
                            }
                        };
                        if (rownumb + 1 < 8 && colnumb + 1 < 8 &&
                            ((pieces[rownumb + 1][colnumb + 1].color[0] == 1 &&
                              pieces[rownumb + 1][colnumb + 1].color[1] != piece.color[1]) ||
                             imo)) {
                            // right
                            if (rownumb == 6) {
                                // promotion
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'q', false});
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'n', false});
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'b', false});
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb),
                                                 static_cast<unsigned int>(rownumb + 1),
                                                 static_cast<unsigned int>(colnumb + 1), -2, -2, 'r', false});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb + 1), static_cast<unsigned int>(colnumb + 1)});
                            };
                        };

                        // en passant
                        if (rownumb == 4 && ((colnumb - 1) == enpassant[0] || (colnumb + 1) == enpassant[0])) {
                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(5),
                                             static_cast<unsigned int>(enpassant[0]), 4, enpassant[0]});
                        }
                    }
                    break;
                }
                case 'k': {
                    if (!imo) {
                        kingrow = rownumb;
                        kingcol = colnumb;
                    }
                }
                case 'q':
                case 'r': {
                    // left
                    for (int i = colnumb - 1; i > -1; i--) {
                        if (imo) {
                            if (pieces[rownumb][i].color == 0b00) {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(i)});
                            } else {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(i), .fap = {rownumb, i}});

                                if (pieces[rownumb][i].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[rownumb][i].color[0] == 0b1 /* kenobi */ &&
                                pieces[rownumb][i].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(i)});

                            if (pieces[rownumb][i].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    // right
                    for (int i = colnumb + 1; i < 8; i++) {
                        if (imo) {
                            if (pieces[rownumb][i].color == 0b00) {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(i)});
                            } else {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(i), .fap = {rownumb, i}});

                                if (pieces[rownumb][i].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[rownumb][i].color[0] == 0b1 /* kenobi */ &&
                                pieces[rownumb][i].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(i)});

                            if (pieces[rownumb][i].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    // up
                    for (int i = rownumb + 1; i < 8; i++) {
                        if (imo) {
                            if (pieces[i][colnumb].color == 0b00) {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(i),
                                                 static_cast<unsigned int>(colnumb)});
                            } else {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(i),
                                                 static_cast<unsigned int>(colnumb), .fap = {i, colnumb}});

                                if (pieces[i][colnumb].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[i][colnumb].color[0] == 0b1 /* kenobi */ &&
                                pieces[i][colnumb].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(i),
                                             static_cast<unsigned int>(colnumb)});

                            if (pieces[i][colnumb].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    // down
                    for (int i = rownumb - 1; i > -1; i--) {
                        if (imo) {
                            if (pieces[i][colnumb].color == 0b00) {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(i),
                                                 static_cast<unsigned int>(colnumb)});
                            } else {
                                moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                                 static_cast<unsigned int>(colnumb), static_cast<unsigned int>(i),
                                                 static_cast<unsigned int>(colnumb), .fap = {i, colnumb}});

                                if (pieces[i][colnumb].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[i][colnumb].color[0] == 0b1 /* kenobi */ &&
                                pieces[i][colnumb].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(i),
                                             static_cast<unsigned int>(colnumb)});

                            if (pieces[i][colnumb].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    if (piece.piece == 'r')
                        break;

                    [[fallthrough]];
                }
                case 'b': {
                    // left up
                    for (int i = 1; i <= min(7 - rownumb, colnumb); i++) {
                        if (imo) {
                            if (pieces[rownumb + i][colnumb - i].color == 0b00) {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb + i), static_cast<unsigned int>(colnumb - i)});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb + i), static_cast<unsigned int>(colnumb - i),
                                     .fap = {rownumb + i, colnumb - i}});

                                if (pieces[rownumb + i][colnumb - i].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[rownumb + i][colnumb - i].color[0] == 0b1 /* kenobi */ &&
                                pieces[rownumb + i][colnumb - i].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb + i),
                                             static_cast<unsigned int>(colnumb - i)});

                            if (pieces[rownumb + i][colnumb - i].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    // left down
                    for (int i = 1; i <= min(rownumb, colnumb); i++) {
                        if (imo) {
                            if (pieces[rownumb - i][colnumb - i].color == 0b00) {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb - i), static_cast<unsigned int>(colnumb - i)});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb - i), static_cast<unsigned int>(colnumb - i),
                                     .fap = {rownumb - i, colnumb - i}});

                                if (pieces[rownumb - i][colnumb - i].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[rownumb - i][colnumb - i].color[0] == 0b1 /* kenobi */ &&
                                pieces[rownumb - i][colnumb - i].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb - i),
                                             static_cast<unsigned int>(colnumb - i)});

                            if (pieces[rownumb - i][colnumb - i].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    // right up
                    for (int i = 1; i <= min(7 - rownumb, 7 - colnumb); i++) {
                        if (imo) {
                            if (pieces[rownumb + i][colnumb + i].color == 0b00) {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb + i), static_cast<unsigned int>(colnumb + i)});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb + i), static_cast<unsigned int>(colnumb + i),
                                     .fap = {rownumb + i, colnumb + i}});

                                if (pieces[rownumb + i][colnumb + i].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[rownumb + i][colnumb + i].color[0] == 0b1 /* kenobi */ &&
                                pieces[rownumb + i][colnumb + i].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb + i),
                                             static_cast<unsigned int>(colnumb + i)});

                            if (pieces[rownumb + i][colnumb + i].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    // right down
                    for (int i = 1; i <= min(rownumb, 7 - colnumb); i++) {
                        if (imo) {
                            if (pieces[rownumb - i][colnumb + i].color == 0b00) {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb - i), static_cast<unsigned int>(colnumb + i)});
                            } else {
                                moves.push_back(
                                    {&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                     static_cast<unsigned int>(rownumb - i), static_cast<unsigned int>(colnumb + i),
                                     .fap = {rownumb - i, colnumb + i}});

                                if (pieces[rownumb - i][colnumb + i].piece != 'k')
                                    break;
                            }
                        } else {
                            if (pieces[rownumb - i][colnumb + i].color[0] == 0b1 /* kenobi */ &&
                                pieces[rownumb - i][colnumb + i].color[1] == piece.color[1])
                                break;

                            moves.push_back({&piece, static_cast<unsigned int>(rownumb),
                                             static_cast<unsigned int>(colnumb), static_cast<unsigned int>(rownumb - i),
                                             static_cast<unsigned int>(colnumb + i)});

                            if (pieces[rownumb - i][colnumb + i].color[0] == 0b1)
                                break;
                        }

                        if (piece.piece == 'k')
                            break;
                    }

                    break;
                }
                case 'n': {
                    //  _   (rownumb + 2, colnumb - 1)
                    //   |
                    //   |
                    if (rownumb + 2 < 8 && colnumb - 1 > -1 &&
                        (pieces[rownumb + 2][colnumb - 1].color[0] == 0b0 ||
                         pieces[rownumb + 2][colnumb - 1].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb + 2),
                                         static_cast<unsigned int>(colnumb - 1)});

                    //  _   (rownumb + 2, colnumb + 1)
                    // |
                    // |
                    if (rownumb + 2 < 8 && colnumb + 1 < 8 &&
                        (pieces[rownumb + 2][colnumb + 1].color[0] == 0b0 ||
                         pieces[rownumb + 2][colnumb + 1].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb + 2),
                                         static_cast<unsigned int>(colnumb + 1)});

                    // |__ (colnumb - 2, rownumb + 1)
                    if (colnumb - 2 > -1 && rownumb + 1 < 8 &&
                        (pieces[rownumb + 1][colnumb - 2].color[0] == 0b0 ||
                         pieces[rownumb + 1][colnumb - 2].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb + 1),
                                         static_cast<unsigned int>(colnumb - 2)});

                    //  __ (colnumb - 2, rownumb - 1)
                    // |
                    if (colnumb - 2 > -1 && rownumb - 1 > -1 &&
                        (pieces[rownumb - 1][colnumb - 2].color[0] == 0b0 ||
                         pieces[rownumb - 1][colnumb - 2].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb - 1),
                                         static_cast<unsigned int>(colnumb - 2)});

                    // |
                    // |__ (colnumb + 1, rownumb - 2)
                    if (colnumb + 1 < 8 && rownumb - 2 > -1 &&
                        (pieces[rownumb - 2][colnumb + 1].color[0] == 0b0 ||
                         pieces[rownumb - 2][colnumb + 1].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb - 2),
                                         static_cast<unsigned int>(colnumb + 1)});

                    // |
                    // |
                    //_  (colnumb - 1, rownumb - 2)
                    if (colnumb - 1 > -1 && rownumb - 2 > -1 &&
                        (pieces[rownumb - 2][colnumb - 1].color[0] == 0b0 ||
                         pieces[rownumb - 2][colnumb - 1].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb - 2),
                                         static_cast<unsigned int>(colnumb - 1)});

                    // __| (colnumb + 2, rownumb + 1)
                    if (colnumb + 2 < 8 && rownumb + 1 < 8 &&
                        (pieces[rownumb + 1][colnumb + 2].color[0] == 0b0 ||
                         pieces[rownumb + 1][colnumb + 2].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb + 1),
                                         static_cast<unsigned int>(colnumb + 2)});

                    // __
                    //   | (colnumb + 2, rownumb - 1)
                    if (colnumb + 2 < 8 && rownumb - 1 > -1 &&
                        (pieces[rownumb - 1][colnumb + 2].color[0] == 0b0 ||
                         pieces[rownumb - 1][colnumb + 2].color[1] != piece.color[1] || imo))
                        moves.push_back({&piece, static_cast<unsigned int>(rownumb), static_cast<unsigned int>(colnumb),
                                         static_cast<unsigned int>(rownumb - 1),
                                         static_cast<unsigned int>(colnumb + 2)});
                    break;
                }
                }
            }
        }
        if (select)
            return moveso;

        vector<::move> &moves = movesm;
        // castling
        if (whosetomove) {
            // black
            if (castling[2]) {
                // black kingside
                if (pieces[7][5].color[0] == 0b0 && pieces[7][6].color[0] == 0b0) {
                    // castling might be possible

                    // check if moves in between are legal
                    bool isLegal = true;
                    for (::move &move : moveso) {
                        if (move.torow == 7 && move.tocol == 4) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 7 && move.tocol == 5) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 7 && move.tocol == 6) {
                            isLegal = false;
                            break;
                        }
                    }

                    if (isLegal)
                        moves.push_back({&pieces[7][4], static_cast<unsigned int>(7), static_cast<unsigned int>(4),
                                         static_cast<unsigned int>(7), static_cast<unsigned int>(7), -2, -2, ' ',
                                         true});
                }
            }
            if (castling[3]) {
                // black queenside
                if (pieces[7][1].color[0] == 0b0 && pieces[7][2].color[0] == 0b0 && pieces[7][3].color[0] == 0b0) {
                    // castling might be possible

                    // check if moves in between are legal
                    bool isLegal = true;
                    for (::move &move : moveso) {
                        if (move.torow == 7 && move.tocol == 4) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 7 && move.tocol == 3) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 7 && move.tocol == 2) {
                            isLegal = false;
                            break;
                        }
                    }

                    if (isLegal)
                        moves.push_back({&pieces[7][4], static_cast<unsigned int>(7), static_cast<unsigned int>(4),
                                         static_cast<unsigned int>(7), static_cast<unsigned int>(2), -2, -2, ' ',
                                         true});
                }
            }
        } else {
            // white
            if (castling[0]) {
                // white kingside
                if (pieces[0][5].color[0] == 0b0 && pieces[0][6].color[0] == 0b0) {
                    // castling might be possible

                    // check if moves in between are legal
                    bool isLegal = true;
                    for (::move &move : moveso) {
                        if (move.torow == 0 && move.tocol == 4) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 0 && move.tocol == 5) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 0 && move.tocol == 6) {
                            isLegal = false;
                            break;
                        }
                    }

                    if (isLegal)
                        moves.push_back({&pieces[0][4], static_cast<unsigned int>(0), static_cast<unsigned int>(4),
                                         static_cast<unsigned int>(0), static_cast<unsigned int>(7), -2, -2, ' ',
                                         true});
                }
            }
            if (castling[1]) {
                // white queenside
                if (pieces[0][1].color[0] == 0b0 && pieces[0][2].color[0] == 0b0 && pieces[0][3].color[0] == 0b0) {
                    // castling might be possible

                    // check if moves in between are legal
                    bool isLegal = true;
                    for (::move &move : moveso) {
                        if (move.torow == 0 && move.tocol == 4) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 0 && move.tocol == 3) {
                            isLegal = false;
                            break;
                        }
                        if (move.torow == 0 && move.tocol == 2) {
                            isLegal = false;
                            break;
                        }
                    }

                    if (isLegal)
                        moves.push_back({&pieces[0][4], static_cast<unsigned int>(0), static_cast<unsigned int>(4),
                                         static_cast<unsigned int>(0), static_cast<unsigned int>(0), -2, -2, ' ',
                                         true});
                }
            }
        }

        // legality check
        bool kingIsInCheck = false;
        vector<tuple<int, int>> toselectPieces;
        if (kingcol != -2) {
            for (::move &move : moveso) {
                if (move.attacking && move.torow == kingrow && move.tocol == kingcol) {
                    toselectPieces.push_back({static_cast<int>(move.fromrow), static_cast<int>(move.fromcol)});

                    kingIsInCheck = true;
                    // break;
                }
            }
        } else {
            // cerr << "king not found!" << endl;
        }

        auto it = remove_if(moves.begin(), moves.end(), [&](::move &move) {
            if (kingIsInCheck) {
                if (move.piece->piece == 'k') {
                    bool isIllegal = false;
                    for (::move &movea : moveso) {
                        if (movea.attacking && movea.torow == move.torow && movea.tocol == move.tocol) {
                            isIllegal = true;
                            break;
                        }
                    }

                    return isIllegal;
                } else {
                    // make copy of board
                    board boardc = *this;
                    boardc.move(move);

                    vector<::move> possibleMoves = boardc.possibleMoves(toselectPieces, true);

                    bool isIllegal = false;
                    // cout << possibleMoves.size() << endl;
                    for (::move &move : possibleMoves) {
                        if (move.attacking && move.torow == kingrow && move.tocol == kingcol) {
                            isIllegal = true;
                            break;
                        }
                    };

                    return isIllegal;
                }
            } else {
                if (move.piece->piece == 'k') {
                    bool isIllegal = false;
                    for (::move &movea : moveso) {
                        if (movea.attacking && movea.torow == move.torow && movea.tocol == move.tocol) {
                            isIllegal = true;
                            break;
                        }
                    }

                    return isIllegal;
                } else {
                    // make copy of board
                    board boardc = *this;
                    boardc.move(move);

                    vector<tuple<int, int>> fappers;
                    for (::move &movea : moveso) {
                        if (movea.fap[0] == move.fromrow && movea.fap[1] == move.fromcol) {
                            fappers.push_back({static_cast<int>(movea.fromrow), static_cast<int>(movea.fromcol)});
                        }
                    }
                    // cout << fappers.size() << endl;

                    vector<::move> possibleMoves = boardc.possibleMoves(fappers, true);

                    bool isIllegal = false;
                    // cout << possibleMoves.size() << endl << endl;
                    for (::move &move : possibleMoves) {
                        if (move.attacking && move.torow == kingrow && move.tocol == kingcol) {
                            isIllegal = true;
                            break;
                        }
                    };

                    return isIllegal;
                }
            }
        });
        moves.erase(it, moves.end());

        for (::move &move : moves) {
            board boardc = *this;
            boardc.move(move);

            // int eval = 0;

            // for every piece on the board
            for (::piece(&row)[8] : boardc.pieces) {
                for (::piece &piece : row) {
                    if (piece.color[1] == 0b1) {
                        // black
                        if (piece.piece == 'p') {
                            move.eval += -100;
                        }
                        if (piece.piece == 'n') {
                            move.eval += -305;
                        }
                        if (piece.piece == 'b') {
                            move.eval += -333;
                        }
                        if (piece.piece == 'r') {
                            move.eval += -563;
                        }
                        if (piece.piece == 'q') {
                            move.eval += -950;
                        }
                        if (piece.piece == 'k') {
                            move.eval += -100000;
                        }
                    } else {
                        // white
                        if (piece.piece == 'p') {
                            move.eval += 100;
                        }
                        if (piece.piece == 'n') {
                            move.eval += 305;
                        }
                        if (piece.piece == 'b') {
                            move.eval += 333;
                        }
                        if (piece.piece == 'r') {
                            move.eval += 563;
                        }
                        if (piece.piece == 'q') {
                            move.eval += 950;
                        }
                        if (piece.piece == 'k') {
                            move.eval += 100000;
                        }
                    }
                }
            }

            if (whoami)
                move.eval = -1 * move.eval;

            // move.eval = eval;
        }

        // if (size(moves) == 0)
        // throw runtime_error("no moves");

        return moves;
    }

    int simpleEval() {
        int eval = 0;
        for (::piece(&row)[8] : pieces) {
            for (::piece &piece : row) {
                if (piece.color[1] == 0b1) {
                    // black
                    if (piece.piece == 'p') {
                        eval += -100;
                    }
                    if (piece.piece == 'n') {
                        eval += -305;
                    }
                    if (piece.piece == 'b') {
                        eval += -333;
                    }
                    if (piece.piece == 'r') {
                        eval += -563;
                    }
                    if (piece.piece == 'q') {
                        eval += -950;
                    }
                    if (piece.piece == 'k') {
                        eval += -100000;
                    }
                } else {
                    // white
                    if (piece.piece == 'p') {
                        eval += 100;
                    }
                    if (piece.piece == 'n') {
                        eval += 305;
                    }
                    if (piece.piece == 'b') {
                        eval += 333;
                    }
                    if (piece.piece == 'r') {
                        eval += 563;
                    }
                    if (piece.piece == 'q') {
                        eval += 950;
                    }
                    if (piece.piece == 'k') {
                        eval += 100000;
                    }
                }
            }
        }

        if (whoami)
            eval = -1 * eval;

        return eval;
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
                    // pieces[7 - row][col] = {0b01, (char)tolower(c), 0};
                    col++;
                    continue;
                }
                if (c > 64 && c < 91) {
                    // uppercase char, so it's white
                    pieces[row][col] = {0b01, (char)tolower(c), 0};
                    // pieces[7 - row][col] = {0b11, (char)tolower(c), 0};
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
            } else if (stage == 3) {
                if (c == '-') {
                    continue;
                } else if (c > '`' && c < '{') {
                    enpassant[0] = c - 'a';
                } else if (c > '/' && c < ':') {
                    enpassant[1] = c - '1';
                } else if (c == ' ') {
                    stage++;
                    // cout << enpassant[0] << enpassant[1] << endl;
                    continue;
                }
            }
        }
    }

    // ::move findMove() {
    //     int counter = 0;

    //     vector<::move> moves = possibleMoves();

    //     for (::move &move : moves) {
    //         move.eval = findMoveDepth(3, move, counter).eval;
    //     }

    //     cout << "evaluated " << counter << " positions" << endl;

    //     sort(moves.begin(), moves.end(), [](const ::move &a, const ::move &b) { return a.eval > b.eval; });

    //     for (::move &move : moves) {
    //         // board.printMove(move);
    //         cerr
    //             << piecesmap[move.piece->color[1] ? (char)tolower(move.piece->piece) :
    //             (char)toupper(move.piece->piece)]
    //             << " " << (char)(move.fromcol + 'a') << (char)(move.fromrow + '1') << "→" << (char)(move.tocol + 'a')
    //             << (char)(move.torow + '1') << "\x1B[0;32m"
    //             << (string(1, move.promotion) != " "
    //                     ? string(1, move.promotion)
    //                     : (move.enpassantrow != -2
    //                            ? "*"
    //                            : (move.castling
    //                                   ? (move.piece->color[1] ? "\x1B[0m" + piecesmap['r'] : "\x1B[0m" +
    //                                   piecesmap['R']) : "")))
    //             << (move.fap[0] != -2 ? "\x1B[90m•" : (move.attacking ? "\x1B[37m•" : ""))
    //             << (move.attacking || move.fap[0] != -2
    //                     ? piecesmap[move.piece->color[1] ? (char)tolower(pieces[move.torow][move.tocol].piece)
    //                                                      : (char)toupper(pieces[move.torow][move.tocol].piece)]
    //                     : "")
    //             << "\x1B[0m"
    //             << "\x1B[2m\x1B[90m  \t   " /* << move.fromrow << move.fromcol << " " << move.torow << move.tocol */
    //             << move.eval << "\x1B[0m\x1B[49m" << endl;
    //     };

    //     return moves[0];
    // }

    // ::move findMoveDepth(int depth, ::move const &move, int &counter) {
    //     if (depth == 0) {
    //         return move;
    //     }

    //     board boardc = *this;
    //     boardc.move(move);
    //     vector<::move> moves = boardc.possibleMoves();

    //     for (::move &move : moves) {
    //         board boardc2 = *this;
    //         boardc2.move(move);
    //         counter++;
    //         move.eval = boardc2.findMoveDepth(depth - 1, move, counter).eval;
    //     }

    //     sort(moves.begin(), moves.end(), [](const ::move &a, const ::move &b) { return a.eval > b.eval; });

    //     return moves[0];
    // }

    void printMoveList(vector<::move> &moves) {
        for (::move &move : moves) {
            // board.printMove(move);
            cerr
                << piecesmap[move.piece->color[1] ? (char)tolower(move.piece->piece) : (char)toupper(move.piece->piece)]
                << " " << (char)(move.fromcol + 'a') << (char)(move.fromrow + '1') << "→" << (char)(move.tocol + 'a')
                << (char)(move.torow + '1') << "\x1B[0;32m"
                << (string(1, move.promotion) != " "
                        ? string(1, move.promotion)
                        : (move.enpassantrow != -2
                               ? "*"
                               : (move.castling
                                      ? (move.piece->color[1] ? "\x1B[0m" + piecesmap['r'] : "\x1B[0m" + piecesmap['R'])
                                      : "")))
                << (move.fap[0] != -2 ? "\x1B[90m•" : (move.attacking ? "\x1B[37m•" : ""))
                << (move.attacking || move.fap[0] != -2
                        ? piecesmap[move.piece->color[1] ? (char)tolower(pieces[move.torow][move.tocol].piece)
                                                         : (char)toupper(pieces[move.torow][move.tocol].piece)]
                        : "")
                << "\x1B[0m"
                << "\x1B[2m\x1B[90m  \t   " /* << move.fromrow << move.fromcol << " " << move.torow << move.tocol */
                << move.eval << "\x1B[0m\x1B[49m" << endl;
        };
    }

    ::move findMoveDepth(int depth, atomic<int> &counter) {
        bool iamoriginal = counter == 0;

        // list moves
        vector<::move> moves = possibleMoves();

        // wait for all threads to finish
        if (iamoriginal) {
            vector<thread *> threads;
            // for (::move &move : moves) {
            for (int i = 0; i < moves.size(); i++) {
                ::move move = moves[i];

                board *boardb = new board(*this);
                board &boardc = *boardb;

                boardc.move(move);
                counter++;

                boardc.toggleWhoseToMove();

                thread *t = new thread{([&, boardb, i]() {
                    board boardc = *boardb;

                    int oe = boardc.findMoveDepth(depth - 1, counter).eval;
                    if (oe != 696969)
                        moves[i].eval = oe;
                })};

                threads.push_back(t);
            }

            cout << threads.size() << endl;
            for (thread *t : threads) {
                (*t).join();
            }

            // desctruct everything properly

            // something?
        } else {
            // for every move, think of opponent response
            for (::move &move : moves) {
                board boardc = *this;
                boardc.move(move);
                counter++;

                boardc.toggleWhoseToMove();

                if (depth > 0) {
                    int oe = boardc.findMoveDepth(depth - 1, counter).eval;
                    if (oe != 696969)
                        move.eval = oe;
                }
            }
        }

        // sort moves by eval
        sort(
            moves.begin(), moves.end(),
            whoami == whosetomove ? [](const ::move &a, const ::move &b) { return a.eval > b.eval; }
                                  : [](const ::move &a, const ::move &b) { return a.eval < b.eval; });

        if (iamoriginal) {
            printMoveList(moves);
        }

        if (moves.size() == 0) {
            return {.eval = 696969};
        }

        return moves[0];
    }
};

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    board board;

    // board.loadfen("1rbk2nr/p2q1Ppp/2p1p3/Pp1p2Np/n1PP4/3B2bR/1P3P1P/RNBQK2R w QK b6 4 13");

    set<string_view> args{argv + 1, argv + argc};
    if (args.contains("--black")) {
        // cout << "I'm black" << endl;
        board.whoami = true;
    } else {
        board.actuallyMove({&board.pieces[1][4], 1, 4, 3, 4, -2, -2, ' ', false, false, {-2, -2}, 0});
        board.draw();
    }

    // let's for now just assume I'm white (hmm, that sounds kinda racist)
    for (string input_move; true;) {
        getline(cin, input_move);
        // input_move = "e2e4";
        if (input_move.length() != 4 && input_move.length() != 5) {
            // exit(EXIT_FAILURE);
            continue;
        }

        board.move(input_move);
        board.toggleWhoseToMove();

        // example with possibilities for pawns to go sideways, forward, promote (also capturing promotion),
        // castle (not the pawns), en passant, discovery checks

        // to be added: field attacked preventing castling, checkmate, stalemate, 50 move rule, 3 fold repetition
        // field attacked preventing piece to move to it, and being in check, forced to resolve that issue first
        // board.loadfen("1rbk2nr/p2q1Ppp/2p1p3/Pp1p2N1/n1PP4/3B2bR/1P3PPP/RNBQK3 w Q b6 4 13");
        // board.loadfen("1rbk2nr/p2q1Ppp/2p1p3/Pp1p2N1/n1PP4/3B1b1R/1P3PPP/RNBQK3 w Q b6 4 13");
        // board.loadfen("1rbk2nr/p2q1Ppp/2p1p3/Pp1p2Np/n1PP4/3B2bR/1P3P1P/RNBQK2R w QK b6 4 13");

        // both legality check and definition of check check
        // board.loadfen("2R5/8/3k4/R4q2/8/B1K5/8/4R3 b - - 0 1");

        // fap, but not too often
        // board.loadfen("8/8/3k4/5q2/2N2r2/2K5/8/8 b - - 0 1");

        // board.move("e1h1");

        // move move
        // board.loadfen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
        // board.loadfen("r3k2r/pppppppp/8/P7/8/8/1PPPPPPP/R3K2R b KQkq - 0 1");

        // castling legality check
        // board.loadfen("8/3k4/8/5q2/1B6/7B/8/1R2K2R w K - 0 1");

        // implementing bishops
        // board.loadfen("8/8/2B1B3/8/2B5/8/8/8 w - b6 4 13");

        // implementing knights
        // board.loadfen("8/8/1N6/N2N4/2n5/8/8/8 w - - 4 13");

        // implementing kings
        // board.loadfen("8/8/4k3/2qP4/1RK5/8/8/8 w - - 4 13");

        // relatively complicated legality check
        // board.loadfen("k7/8/8/2rr1n2/4b3/4p3/3K4/2r5 w - - 2 2");
        // board.loadfen("8/4k3/8/2q5/1B6/4B3/5K2/1R6 w - - 0 1");
        // board.loadfen("1k6/8/8/3r1b2/8/3K1Q2/8/8 w - - 1 1");
        // board.move(input_move);
        // board.toggleWhoseToMove();

        // board.move("a5b6");

        // vector<::move> possibleMoves = board.possibleMoves();
        // for (::move &move : possibleMoves) {
        //     // board.printMove(move);
        //     cerr
        //         << piecesmap[move.piece->color[1] ? (char)tolower(move.piece->piece) :
        //         (char)toupper(move.piece->piece)]
        //         << " " << (char)(move.fromcol + 'a') << (char)(move.fromrow + '1') << "→" << (char)(move.tocol + 'a')
        //         << (char)(move.torow + '1') << "\x1B[0;32m"
        //         << (string(1, move.promotion) != " "
        //                 ? string(1, move.promotion)
        //                 : (move.enpassantrow != -2
        //                        ? "*"
        //                        : (move.castling
        //                               ? (move.piece->color[1] ? "\x1B[0m" + piecesmap['r'] : "\x1B[0m" +
        //                               piecesmap['R']) : "")))
        //         << (move.fap[0] != -2 ? "\x1B[90m•" : (move.attacking ? "\x1B[37m•" : ""))
        //         << (move.attacking || move.fap[0] != -2
        //                 ? piecesmap[move.piece->color[1] ? (char)tolower(board.pieces[move.torow][move.tocol].piece)
        //                                                  : (char)toupper(board.pieces[move.torow][move.tocol].piece)]
        //                 : "")
        //         << "\x1B[0m"
        //         << "\x1B[2m\x1B[90m  \t   " /* << move.fromrow << move.fromcol << " " << move.torow << move.tocol */
        //         << move.eval << "\x1B[0m\x1B[49m" << endl;
        // };

        // amout of possible moves
        // cerr << possibleMoves.size() << endl << endl;

        atomic<int> counter = 0;
        auto start = chrono::high_resolution_clock::now();
        ::move bestMove = board.findMoveDepth(4, counter);
        board.actuallyMove(bestMove);
        cerr << counter / 1000 / (duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start)).count()
             << "K positions per second" << endl
             << "(" << counter << " total)" << endl;

        board.draw();

        // cout << board.eval() << endl;

        exit(EXIT_SUCCESS);
    }
}