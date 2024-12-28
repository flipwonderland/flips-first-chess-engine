#include "util.h"
#include <string>


static enum piece {

    none = 0b00000,
    king = 0b00001,
    pawn = 0b00010,
    knight = 0b00011,
    bishop = 0b00100,
    rook = 0b00101,
    queen = 0b00110,
    white = 0b01000,
    black = 0b10000,

};

int square[8][8] = {};
std::string startingFenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";


/*
there should be 3 layers,
fen > bitboard > square[][]
fen is the starting layer, which converts to the middle layer
the middle layer is the most efficient, as it needs to access the board millions of times per second
then when the calulations are done, it should give a square[0-7][0-7] for the renderer to show to the player

*/

internal int fenToBoard(std::string fenString){
int y = 0;
int x = 0;
int skips = 0;
for (int piece = 0; piece <= (fenString.length() - 1); piece++) {
    char fenPart = fenString[piece];
    switch (fenPart) {
    case('1'):
    case('2'):
    case('3'):
    case('4'):
    case('5'):
    case('6'):
    case('7'):
    case('8')://this is to rectify the char int being the same value as an int
        switch (fenPart) {
        case('1'):
            skips = 1;
            break;
        case('2'):
            skips = 2;
            break;
        case('3'):
            skips = 3;
            break;
        case('4'):
            skips = 4;
            break;
        case('5'):
            skips = 5;
            break;
        case('6'):
            skips = 6;
            break;
        case('7'):
            skips = 7;
            break;
        case('8'):
            skips = 8;
            break;
        }
        
        for (; skips != 0; skips--) {
            square[y][x] = piece::none;
            x++;
        }
        skips = 0;
        break;
    case('/'):
        y++;
        x = 0;
        break;
    case('K'):
        square[y][x] = piece::white && piece::king;
        x++;
        break;
    case('P'):
        square[y][x] = piece::white && piece::pawn;
        x++;
        break;
    case('N'):
        square[y][x] = piece::white && piece::knight;
        x++;
        break;
    case('B'):
        square[y][x] = piece::white && piece::bishop;
        x++;
        break;
    case('R'):
        square[y][x] = piece::white && piece::rook;
        x++;
        break;
    case('Q'):
        square[y][x] = piece::white && piece::queen;
        x++;
        break;
    case('k'):
        square[y][x] = piece::black && piece::king;
        x++;
        break;
    case('p'):
        square[y][x] = piece::black && piece::pawn;
        x++;
        break;
    case('n'):
        square[y][x] = piece::black && piece::knight;
        x++;
        break;
    case('b'):
        square[y][x] = piece::black && piece::bishop;
        x++;
        break;
    case('r'):
        square[y][x] = piece::black && piece::rook;
        x++;
        break;
    case('q'):
        square[y][x] = piece::black && piece::queen;
        x++;
        break;
    }
    }

}

internal int
arrayToFen(int square) {

}

internal int
fenToBitBoard(const char fenString) {

}

internal int
bitBoardToFen(const char fenString /*fix later */ ) {

}


static int
internal_board(int pieceFile, int pieceRank) {

    square[0][0] = white | bishop;
    square[7][6] = black | queen;
    square[0][7] = black | knight;

    return square[pieceFile][pieceRank];
}