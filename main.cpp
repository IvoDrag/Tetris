#include <iostream>
#include <Windows.h>
#include <thread>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

wstring tetromino[7];

int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

int nScreenWidth = 80; // Console Screen Size X (columns)
int nScreenHeight = 30; // Console Screen Size Y (rows)

int rotation(int px, int py, int r) {
	switch (r % 4) {
	// 0 degrees rotation
	case 0: 
		return py * 4 + px;
		break;
	// 90 degrees rotation
	case 1: 
		return 12 + py - (px * 4);
		break;
	// 180 degrees rotation
	case 2:
		return 15 - (py * 4) - px;
		break;
	// 270 degrees rotation
	case 3:
		return 3 - py + (px * 4);
		break;
	}
	return 0;
}

bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
	for (int px = 0; px < 4; px++) {
		for (int py = 0; py < 4; py++) {
			// Get index into piece
			int pieceIndex = rotation(px, py, nRotation);
			
			// Get index into field (WITHOUT ROTXTION)
			int fieldIndex = (nPosY + py) * nFieldWidth + (nPosX + px);
			
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
					if (tetromino[nTetromino][pieceIndex] == L'X' && pField[fieldIndex] != 0) {
						return false; // COLISION !
					}
 				}
			}
		}
	}
}

int main() {

	// Create figures
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L".X..");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"..X.");
	tetromino[1].append(L"..X.");

	tetromino[2].append(L".X..");
	tetromino[2].append(L"..XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L"...X");
	tetromino[4].append(L"..XX");
	tetromino[4].append(L"....");

	tetromino[5].append(L".X..");
	tetromino[5].append(L".X..");
	tetromino[5].append(L".XX..");
	tetromino[5].append(L"....");

	tetromino[6].append(L".XX.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L"....");

	// Creating playing field! 
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++) {
		for (int y = 0; y < nFieldHeight; y++) {
			// Represent 9 as border or represent it as empty space(0)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
		}
	}

	// Creating Screen Buffer
	wchar_t* screen = new wchar_t[nScreenHeight * nScreenWidth];
	for (int r = 0; r < nScreenHeight * nScreenWidth; r++) screen[r] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	int nCurrentPiece = 6;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	// RIGHT ARROW, LEFT ARROW, DOWN ARROW, Z (TO ROTATE THE PIECE) !
	bool bKey[4];
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

	bool gameOver = false;
	while (!gameOver) {

		// GAME TIMING
		this_thread::sleep_for(50ms); // GAME TICK
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		// USER INPUT

		for (int k = 0; k < 4; k++) {						    // L   R   D    Z 
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x27\x28\x05A"[k])));
			// 0x8000 &
		}

		// GAME LOGIC

		if (bKey[0]) {
			if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) {
				nCurrentX = nCurrentX - 1;
			}
		}
		if (bKey[1]) {
			if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) {
				nCurrentX = nCurrentX + 1;
			}
		}

		if (bKey[2]) {
			if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
				nCurrentY = nCurrentY + 1;
			}
		}

		if (bKey[3]) {
			if (!bRotateHold && doesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) {
				nCurrentRotation = nCurrentRotation + 1;
				bRotateHold = true;
			}
		}
		else {
			bRotateHold = false;
		}

		if (bForceDown) {	
			nSpeedCounter = 0;
			nPieceCount++;
			if (nPieceCount % 50 == 0) {
				if (nSpeed >= 10) {
					nSpeed--;
				}
			}
			if (doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
				nCurrentY++; // SEE IF IT CAN FIT, FIT IN
			}
			else {
				// Lock the current piece in the field!

				for (int px = 0; px < 4; px++) {
					for (int py = 0; py < 4; py++) {
						if (tetromino[nCurrentPiece][rotation(px, py, nCurrentRotation)] == L'X') {
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
						}
					}
				}

				// Check if there is a line!

				for (int py = 0; py < 4; py++) {
					if (nCurrentY + py < nFieldHeight - 1) {
						bool bLine = true;
						for (int px = 1; px < nFieldWidth; px++) {
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
						}
						if (bLine) {
							//Remove line and set it to symbol (= which is number 8)
							for (int px = 1; px < nFieldWidth - 1; px++) {
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							}
							vLines.push_back(nCurrentY + py);
						}
					}
				}
				
				nScore += 25;
				if (!vLines.empty()) {
					nScore += (1 << vLines.size()) * 100;
				}

				// Choose next piece!

				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				srand(time(NULL));
				nCurrentPiece = rand() % 7;

				// if piece doesnt fit and it cant play anymore!

				gameOver = !doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
		}

		// Draw Field
		for (int x = 0; x < nFieldWidth; x++) {
			for (int y = 0; y < nFieldHeight; y++) {
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
			}
		}

		// DRAW CURRENT PIECE
		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {
				if (tetromino[nCurrentPiece][rotation(px, py, nCurrentRotation)] == L'X') {
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
				}
			}
		}

		// DRAW SCORE

		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

			if (!vLines.empty()) {
				//Display Frame
				WriteConsoleOutputCharacter(hConsole, screen, nScreenHeight * nScreenWidth, { 0,0 }, &dwBytesWritten);
				this_thread::sleep_for(100ms); //delay a bit

				for (auto& v : vLines) {
					for (int px = 1; px < nFieldWidth - 1; px++) {
						for (int py = v; py > 0; py--) {
							pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
							pField[px] = 0;
						}
					}
				}
				vLines.clear();
			}

			//Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenHeight* nScreenWidth, { 0,0 }, & dwBytesWritten);
	}

	CloseHandle(hConsole);
	cout << "GAME OVER! SCORE: " << nScore << endl;

	return 0;
}
