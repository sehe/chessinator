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
        {{1, 'r', 5}, {1, 'n', 3}, {1, 'b', 3}, {1, 'q', 9}, {1, 'k', 100}, {1, 'b', 3}, {1, 'n', 3}, {1, 'r', 5}},
        {{1, 'p', 1}, {1, 'p', 1}, {1, 'p', 1}, {1, 'p', 1}, {1, 'p', 100}, {1, 'p', 1}, {1, 'p', 1}, {1, 'p', 1}},

        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 000}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},
        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 000}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},
        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 000}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},
        {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 000}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}},

        {{0, 'p', 1}, {0, 'p', 1}, {0, 'p', 1}, {0, 'p', 1}, {0, 'p', 100}, {0, 'p', 1}, {0, 'p', 1}, {0, 'p', 1}},
        {{0, 'r', 5}, {0, 'n', 3}, {0, 'b', 3}, {0, 'q', 9}, {0, 'k', 100}, {0, 'b', 3}, {0, 'n', 3}, {0, 'r', 5}},
    };
};

main(int argc, char *argv[]) {
    set<string_view> args{argv + 1, argv + argc};
    bool black = args.contains("--black");
    bool white = !black;

    board board;

    // let's for now just assume I'm white (hmm, that sounds kinda racist)
    while (true) {
        string input_move;
        getline(cin, input_move);
        cout << input_move << endl;
    }
}