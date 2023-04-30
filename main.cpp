#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

struct piece {
    int color;
    char piece;
    int worth;
};

struct board {
    piece pieces[8][8]{
        {{1, 'r', 500},
         {1, 'n', 300},
         {1, 'b', 300},
         {1, 'q', 900},
         {1, 'k', 10000},
         {1, 'b', 300},
         {1, 'n', 300},
         {1, 'r', 500}},
        {{1, 'p', 100},
         {1, 'p', 100},
         {1, 'p', 100},
         {1, 'p', 100},
         {1, 'p', 100},
         {1, 'p', 100},
         {1, 'p', 100},
         {1, 'p', 100}},

        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},
        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},
        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},
        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},

        {{0, 'p', 100},
         {0, 'p', 100},
         {0, 'p', 100},
         {0, 'p', 100},
         {0, 'p', 100},
         {0, 'p', 100},
         {0, 'p', 100},
         {0, 'p', 100}},
        {{0, 'r', 500},
         {0, 'n', 300},
         {0, 'b', 300},
         {0, 'q', 900},
         {0, 'k', 10000},
         {0, 'b', 300},
         {0, 'n', 300},
         {0, 'r', 500}},
    };

    int eval() {
        int black = 0;
        int white = 0;

        cout << endl;
        for (int i = 0; i < 8; i++) {
            piece(&row)[8] = pieces[i];
            for (int k = 0; k < 8; ++k) {
                cout << row[k].piece << " ";
            }
            cout << endl;
            for (int j = 0; j < 8; ++j) {
                piece piece = pieces[i][j];

                if (piece.color == 0)
                    white += piece.worth;
                if (piece.color == 1)
                    black += piece.worth;
            }
        }

        cout << "White: " << white << endl;
        cout << "Black: " << black << endl;

        return white - black;
    }
};

main(int argc, char *argv[]) {
    set<string_view> args{argv + 1, argv + argc};
    bool black = args.contains("--black");
    bool white = !black;
    if (black)
        cout << "I'm black" << endl;

    board board;

    // let's for now just assume I'm white (hmm, that sounds kinda racist)
    for (string input_move; true;) {
        getline(cin, input_move);
        if (input_move.length() != 4) {
            cout << "Invalid move of size " << input_move.length() << endl;
            exit(EXIT_FAILURE);
        }

        cout << "eval: " << board.eval() << endl;
        cout << input_move << endl;
    }
}