#include "defs.h"


u64 generatePositionKey(const boardStructure* position) {

	int square = 0;
	u64 finalKey = 0ULL;
	int piece = empty;

	//pieces
	for (; square < BRD_SQ_NUM; square++) {
		piece = position->pieces[square];
		if (piece != noSquare && piece != empty && piece != offBoard) {
#ifdef PERFORMANCEMODE
			finalKey ^= pieceKeys[piece][square];
#endif
#ifdef DEBUG
			if (piece >= wK && piece <= bQ) {
				finalKey ^= pieceKeys[piece][square];
			}
			else {
				std::cout << "error, invalid piece in the position key generator \n";
			}
#endif
		}
	}

	if (position->side == white) {
		finalKey ^= sideKey;
	}

	if (position->enPassant != noSquare) {
#ifdef DEBUG
		if (position->enPassant >= 0 && position->enPassant < BRD_SQ_NUM) {
			if (!squareOnBoard(position->enPassant)) {
				std::cout << "en passant position not on board in keygen\n";
			}
			else if (ranksBoard[position->enPassant] != rank3 && ranksBoard[position->enPassant] != rank6) {
				std::cout << "en passant not on the 3rd or 6th ranks in keygen\n";
			}
			else {
				finalKey ^= pieceKeys[empty][position->enPassant];
			}

		}
#endif
#ifdef PERFORMANCEMODE
		finalKey ^= pieceKeys[empty][position->enPassant];
#endif
	}


#ifdef DEBUG
	if (position->castlePermission >= 0 && position->castlePermission <= 15) {
		finalKey ^= castleKeys[position->castlePermission];
	}
	else {
		std::cout << "error, invalid castling permissions given to the position key generator \n";
	}
#endif
#ifdef PERFORMANCEMODE
	finalKey ^= castleKeys[position->castlePermission];
#endif

	return finalKey;
}



