#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <windows.h>
#include <conio.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>

const int consoleWidth = 1280;
const int consoleHeight = 766;
const int size = 12;
const int lineNoti = size * 2 + 1 + 15;

using namespace std;

mutex mtx;
bool isPlaying = false;
bool timeOut = false;
bool isOver = false;

struct BotMove {
    int x;
    int y;
};

struct PlayAgain {
    bool isPlayAgain;
    int x;
    int y;
};

struct TimeOut {
    bool team1;
    bool team2;
};

TimeOut timeOutGame = {false, false};

void playGame(bool isBotGame, PlayAgain playAgain);
int main();
bool inputInvaild(int board[size][size], int team, int x, int y);
int solveScore(int board[size][size], int team);
BotMove AI(int board[size][size]);

void resizeConsole(int width, int height) {
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, width, height, TRUE);
}

void gotoxy(int x, int y, string content = "") {
    lock_guard<mutex> lock(mtx);
    COORD coord;
    coord.X = x; 
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    cout << content;
}

void Nocursortype() {
    CONSOLE_CURSOR_INFO Info;
    Info.bVisible = FALSE;
    Info.dwSize = 20;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
}

string removeCodes(string str) {
    string result;
    bool code = false;

    for(int i = 0; i < str.length(); i++) {
        if (str[i] == '\x1b') {
            code = true;
        } else if (str[i] == 'm') {
            code = false;
        } else if (!code) {
            result += str[i];
        }
    }
    return result;
}

string printCenter(string content) {
    ostringstream result;
    ostringstream temp;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    temp << removeCodes(content);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int padding = (consoleWidth - temp.str().length()) / 2;
    result << string(padding, ' ') << content;
    temp.str("");
    return result.str();
}

int getConsoleWidth() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

string getSpace(int n) {
    string space = "";
    for (int i = 0; i < n; i++) {
        space += " ";
    }
    return space;
}

void endGame(int type, PlayAgain playAgain) {
    string yellow = "\x1b[33m", reset = "\x1b[0m";
    int play_again = 0;
    gotoxy(0, lineNoti + 1, printCenter("Press " + yellow + "Space" + reset + " to play again, " + yellow + "0" + reset + " to back to menu, " + yellow + "q" + reset + " to quit"));
    while (play_again == 0) {
        int n = getch();
        if (n == 32) {
            system("cls");
            if (type == 1) {
                playAgain.isPlayAgain = true;
                playGame(true, playAgain);
            } else if (type == 2) {
                playGame(false, playAgain);
            }
        } else if (n == 48) {
            system("cls");
            main();
        } else if (n == 113) {
            exit(0);
        }
    }
}

void printTime(int time) {
    int min = 0;
    int sec = 0;
    int clsw = getConsoleWidth();
	while (isPlaying) {
        ostringstream time;
        time << (min < 10 ? "0" : "") << min << ":" << (sec < 10 ? "0" : "") << sec;
        gotoxy(clsw / 2 - 2 + 1, 9, time.str());
        sec++;
        if (sec == 60) {
            min++;
            sec = 0;
        }
        if(min == 5) {
            break;
        }
		Sleep(1000);
	}
}

void runTime(int time, int team, bool isBot, PlayAgain playAgain) {
    int sizeBoard = 1 + 6 * size + 6;
    int sizeTime = sizeBoard / 2 - 12;
    string red = "\x1b[36m", blue = "\x1b[33m", reset = "\x1b[0m", yellow = "\x1b[43m", cyan = "\x1b[36m";
    char x = 'X';
    char o = 'O';
    ostringstream trai, phai;
    trai << "\x1b[36m" << (char)16 << "\x1b[0m";
    phai << "\x1b[33m" << (char)17 << "\x1b[0m";
    int timeSleep = (time * 1000) / sizeTime;
    int i = 0;
    string resetTrai = string(sizeTime, (char)16);
    string resetPhai = string(sizeTime, (char)17);
    if(team == 1) {
        gotoxy((getConsoleWidth() - 6 * size + 2) / 2, 8, resetTrai);
    }
    else {
        gotoxy(getConsoleWidth() / 2 + sizeBoard / 2 - 1 - sizeTime, 8, resetPhai);
    }
    while(timeOut) {
        Sleep(timeSleep);
        if(team == 1) {
            gotoxy(getConsoleWidth() / 2 - sizeBoard / 2 + 4 + i, 8, trai.str());
            i++;
            if(i == sizeTime || timeOutGame.team1 == true) {
                if(i == sizeTime) {
                    timeOut = false;
                    isOver = true;
                    INPUT ip;
                    ip.type = INPUT_KEYBOARD;
                    ip.ki.wScan = 0;
                    ip.ki.time = 0;
                    ip.ki.dwExtraInfo = 0;
                    ip.ki.wVk = VK_RETURN;
                    ip.ki.dwFlags = 0;
                    SendInput(1, &ip, sizeof(INPUT));
                    ip.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &ip, sizeof(INPUT));
                }
                break;
            }
        }
        else {
            gotoxy(getConsoleWidth() / 2 + sizeBoard / 2 - 2 - i, 8, phai.str());
            i++;
            if(i == sizeTime || timeOutGame.team2 == true) {
                if(i == sizeTime) {
                    timeOut = false;
                    isOver = true;
                    //gia lap phim enter
                    INPUT ip;
                    ip.type = INPUT_KEYBOARD;
                    ip.ki.wScan = 0;
                    ip.ki.time = 0;
                    ip.ki.dwExtraInfo = 0;
                    ip.ki.wVk = VK_RETURN;
                    ip.ki.dwFlags = 0;
                    SendInput(1, &ip, sizeof(INPUT));
                    ip.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &ip, sizeof(INPUT));
                }
                break;
            }
        }
    }
}

void printBoard(int board[size][size], int cursorX, int cursorY, int team, PlayAgain playAgain, int start = 1) {
    int select = 219;
    char x = 'X';
    char o = 'O';
    string red = "\x1b[36m", blue = "\x1b[33m", reset = "\x1b[0m", yellow = "\x1b[43m", cyan = "\x1b[36m";
    int sizeBoard = 1 + 6 * size + 6;
    int sizeTime = sizeBoard / 2 - 12;
    string resetTrai = string(sizeTime, (char)16);
    string resetPhai = string(sizeTime, (char)17);
    if(start == 0) {
        ostringstream output;
        string space = string((getConsoleWidth() - 47) / 2, ' ');
        string banner = space + "  ______      ___      .______        ______\n";
        banner += space + " /      |    /   \\     |   _  \\      /  __  \\\n";
        banner += space + "|  ,----'   /  ^  \\    |  |_)  |    |  |  |  |\n";
        banner += space + "|  |       /  /_\\  \\   |      /     |  |  |  |\n";
        banner += space + "|  `----. /  _____  \\  |  |\\  \\----.|  `--'  |\n";
        banner += space + " \\______|/__/     \\__\\ | _| `._____| \\______/\n";
        banner += space + "                                                 \n";
        cout << banner << endl;
        space = string((getConsoleWidth() - 6 * size + 2) / 2, ' ');
        int trai = 16, phai = 17, ngang = 22;
        ostringstream tiso;
        tiso << (char)ngang << cyan << playAgain.x << reset << ":" << cyan << playAgain.y << reset << (char)ngang;
        cout << " " << printCenter(string(sizeBoard / 2 - 12, (char)trai) + "  " + red + x + reset + "    " + tiso.str() + "    " + blue + o + reset + "  " + string(sizeBoard / 2 - 12, (char)phai));
        cout << endl << " " << printCenter("    " + getSpace(sizeBoard / 2 - 17) + "00:00" + getSpace(sizeBoard / 2 - 17) + "    ") << endl;
        int select = 219;
        int _GDoc = -70; _GDoc = (char)_GDoc;
        int _GNgang = -51;
        int _TT = -55;
        int _TP = -69;
        int _DT = -56; 
        int _DP = -68;
        int _GT = -53; 
        int _GD = -54;  
        int _VTT = -50; 
        int _NGT = -52; 
        int _NGP = -71; 
        output << space;
        output << (char)_TT;
        for(int i = 0; i < size - 1; i++) {
            output << string(5, (char)_GNgang) << (char)_GT;
        }
        output << string(5, (char)_GNgang) << (char)_TP << endl << space;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                output << (char)_GDoc << "  ";
                if(board[i][j] == 1 && i == cursorX && j == cursorY)
                    output << yellow << x << reset;
                else if(board[i][j] == 2 && i == cursorX && j == cursorY)
                    output << yellow << o << reset;
                else if (board[i][j] == 1)
                    output << red << x << reset;
                else if (board[i][j] == 2)
                    output << blue << o << reset;
                else if(i == cursorX && j == cursorY) 
                    output << (char)select;
                else
                    output << " ";
                output << "  ";
            }
            output << (char)_GDoc << endl << space;
            if (i < size - 1) {
                output << (char)_NGT;
                for (int j = 0; j < size - 1; j++) {
                    output << string(5, (char)_GNgang) << (char)_VTT;
                }
                output << string(5, (char)_GNgang) << (char)_NGP << endl << space;
            }
        }

        output << (char)_DT;
        for(int i = 0; i < size - 1; i++) {
            output << string(5, (char)_GNgang) << (char)_GD;
        }
        output << string(5, (char)_GNgang) << (char)_DP << endl << endl;
        output << endl << printCenter("Di chuyen den o can danh bang phim mui ten") << endl;
        cout << output.str();
    } else {
        int spaceLength = (getConsoleWidth() - 6 * size + 2) / 2;
        int sizeBoard = 1 + 6 * size + 6;
        string luot1 = team == 1 ? yellow + "Luot" + reset : "    ";
        string luot2 = team == 2 ? yellow + "Luot" + reset : "    ";
        if(team == 1) {
            gotoxy(getConsoleWidth() / 2 + sizeBoard / 2 - 14, 9, "    ");
            gotoxy(getConsoleWidth() / 2 - sizeBoard / 2 + 14, 9, luot1);
        }
        else {
            gotoxy(getConsoleWidth() / 2 - sizeBoard / 2 + 14, 9, "    ");
            gotoxy(getConsoleWidth() / 2 + sizeBoard / 2 - 14, 9, luot2);
        }
        int startX = spaceLength + 3;
        int startY = 11;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int xCoord = startX + (j * 6);
                int yCoord = startY + (i * 2);
                if(board[i][j] == 1 && i == cursorX && j == cursorY) {
                    gotoxy(xCoord, yCoord, yellow + x + reset);
                }
                else if(board[i][j] == 2 && i == cursorX && j == cursorY) {
                    gotoxy(xCoord, yCoord, yellow + o + reset);
                }
                else if (board[i][j] == 1) {
                    gotoxy(xCoord, yCoord, red + x + reset);
                }
                else if (board[i][j] == 2) {
                    gotoxy(xCoord, yCoord, blue + o + reset);
                }
                else if(i == cursorX && j == cursorY) {
                    string ch = string(1, (char)select);
                    gotoxy(xCoord, yCoord, ch);
                }
                else {
                    gotoxy(xCoord, yCoord, " ");
                }
                gotoxy(xCoord + 1, yCoord, "  ");
            }
        }
    }
}

bool inputInvaild(int board[size][size], int team, int x, int y) {
    if (x == -1 || y == -1) {
        return true;
    }
    if (x > size - 1 || x < 0 || y > size - 1 || y < 0) {
        return true;
    }
    if (board[x][y] == 0) {
        board[x][y] = team;
        return false;
    } else {
        return true;
    }
}

int checkWin(int board[size][size], int team) {
    int win = 0;
    int count = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 4; j++) {
            if (board[i][j] == team && board[i][j + 1] == team && board[i][j + 2] == team &&
                board[i][j + 3] == team && board[i][j + 4] == team) {
                win = 1;
            }
        }
    }

    for (int i = 0; i < size - 4; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == team && board[i + 1][j] == team && board[i + 2][j] == team &&
                board[i + 3][j] == team && board[i + 4][j] == team) {
                win = 1;
            }
        }
    }

    for (int i = 0; i < size - 4; i++) {
        for (int j = 0; j < size - 4; j++) {
            if (board[i][j] == team && board[i + 1][j + 1] == team && board[i + 2][j + 2] == team &&
                board[i + 3][j + 3] == team && board[i + 4][j + 4] == team) {
                win = 1;
            }
        }
    }

    for (int i = 0; i < size - 4; i++) {
        for (int j = 4; j < size; j++) {
            if (board[i][j] == team && board[i + 1][j - 1] == team && board[i + 2][j - 2] == team &&
                board[i + 3][j - 3] == team && board[i + 4][j - 4] == team) {
                win = 1;
            }
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0) {
                count++;
            }
        }
    }

    if (count == 0 && win != 1) {
        return 2;
    }

    return win;
}

void playGame(bool isBotGame, PlayAgain playAgain) {
    isOver = false;
    int board[size][size] = {0};
    int team, start = 0;
    isBotGame ? team = 2 : team = 1;
    int n, x = size/ 2 - 1, y = size/ 2 - 1, win = 0, turnBot = 0;
    system("cls");
    printBoard(board, x, y, team, playAgain, start);
    int time = 0;
    int timeTurn = 5;
    int clsw = getConsoleWidth();
    isPlaying = true;
    thread timeThread(printTime, time);
    bool isMove = false;
    while (win == 0) {
        if(isOver) {
            if(team == 1) {
                playAgain.y++;
            }
            else {
                playAgain.x++;
            }
            isPlaying = false;
            timeThread.detach(); 
            gotoxy(0, lineNoti, printCenter("Het gio!"));
            if(team == 1) {
                timeOutGame.team1 = false;
                timeOutGame.team2 = false;
            }
            else {
                timeOutGame.team1 = false;
                timeOutGame.team2 = false;
            }
            endGame(isBotGame ? 1 : 2, playAgain);
            break;
        }
        if(!isMove) {
            timeOut = true;
            thread runTimeThread(runTime, timeTurn, team, isBotGame, playAgain);
            runTimeThread.detach();
        }
        if (!isBotGame || team == 1) {
            printBoard(board, x, y, team, playAgain, 1);
            n = getch();
            if (n == 224 || n == 0) {
                n = getch();
                if (n == 72 || n == 119) {
                    isMove = true;
                    if (x > 0)
                        x--;
                } else if (n == 80 || n == 115) { 
                    isMove = true;
                    if (x < size - 1)
                        x++;
                } else if (n == 75 || n == 97) {
                    isMove = true;
                    if (y > 0)
                        y--;
                } else if (n == 77 || n == 100) { 
                    isMove = true;
                    if (y < size - 1)
                        y++;
                }
                printBoard(board, x, y, team, playAgain, 1);
            }
            else if (n == 32) {
                if(isOver) {
                    continue;
                }
                if (inputInvaild(board, team, x, y)) {
                    printBoard(board, x, y, team, playAgain, 1);
                    cout << endl;
                    gotoxy(0, lineNoti, printCenter("Nuoc di khong hop le!"));
                    isMove = true;
                    continue;
                }
                BotMove botMove = AI(board);
                if(botMove.x == -1 && botMove.y == -1) {
                    playAgain.x++;
                    playAgain.y++;
                    isPlaying = false;
                    timeThread.detach();
                    gotoxy(0, lineNoti, printCenter("Hoa!"));
                    timeOutGame.team1 = false;
                    timeOutGame.team2 = false;
                    endGame(isBotGame ? 1 : 2, playAgain);
                    break;
                }
                if(team == 1) {
                    timeOutGame.team1 = true;
                    timeOutGame.team2 = false;
                }
                else {
                    timeOutGame.team1 = false;
                    timeOutGame.team2 = true;
                }
                isMove = false;
                timeOut = false;
                gotoxy(0, lineNoti, printCenter("                                    "));
                printBoard(board, x, y, team, playAgain, 1);
                win = checkWin(board, team);
                if (win == 1) {
                    cout << endl;
                    ostringstream text;
                    text << "Team " << team << " chien thang!";
                    team == 1 ? playAgain.x++ : playAgain.y++;
                    gotoxy(0, lineNoti, printCenter(text.str()));
                    break;
                } else if (win == 2) {
                    cout << endl;
                    gotoxy(0, lineNoti, printCenter("Hoa!"));
                    playAgain.x++;
                    playAgain.y++;
                    break;
                } else {
                    team = team == 1 ? 2 : 1;
                }
            }
        } else {
            if(team == 1) {
                timeOutGame.team1 = true;
                timeOutGame.team2 = false;
            }
            else {
                timeOutGame.team2 = true;
                timeOutGame.team1 = false;
            }
            if(turnBot == 0) {
                x = size / 2 - 1;
                y = size / 2 - 1;
                board[x][y] = 2;
                turnBot++;
                team = 1;
                printBoard(board, x, y, team, playAgain, 1);
                continue;
            }
            printBoard(board, x, y, team, playAgain, 1);
            timeOut = false;
            isMove = false;
            Sleep(1000);
            BotMove botMove = AI(board);
            if(botMove.x == -1 && botMove.y == -1) {
                playAgain.x++;
                playAgain.y++;
                isPlaying = false;
                timeThread.detach();
                gotoxy(0, lineNoti, printCenter("Hoa!"));
                timeOutGame.team1 = false;
                timeOutGame.team2 = false;
                endGame(1, playAgain);
                break;
            }
            x = botMove.x;
            y = botMove.y;
            board[x][y] = 2;
            printBoard(board, x, y, team, playAgain, 1);
            win = checkWin(board, team);
            if (win == 1) {
                cout << endl;
                ostringstream text;
                text << "Team " << team << (team == 2 ? " (AI)" : "") << " chien thang!";
                team == 1 ? playAgain.x++ : playAgain.y++;
                gotoxy(0, lineNoti, printCenter(text.str()));
                break;
            } else if (win == 2) {
                cout << endl;
                gotoxy(0, lineNoti, printCenter("Hoa!"));
                playAgain.x++;
                playAgain.y++;
                break;
            } else {
                team = team == 1 ? 2 : 1;
            }
        }
    }
    isPlaying = false;
    timeThread.detach(); 
    endGame(isBotGame ? 1 : 2, playAgain);
}

void drawMenu(int selected) {
    ostringstream output;
    output << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
    string menu[4] = {"Choi voi may", "Choi 2 nguoi", "Huong dan", "Thoat"};
    output << printCenter("=============================================================") << endl;
    output << printCenter("=                  __  __ ___ _  _ _   _                    =") << endl;
    output << printCenter("=                  |  \\/  | __| \\| | | | |                  =") << endl;
    output << printCenter("=                  | |\\/| | _|| .` | |_| |                  =") << endl;
    output << printCenter("=                  |_|  |_|___|_|\\_|\\___/                   =") << endl;
    output << printCenter("=============================================================") << endl;
    for(int i = 0; i < 4; i++) {
        if(i == selected) {
            //tinh toan vi tri de in ra
            int space = 58 - menu[i].length();
            int spaceLeft = space / 2;
            int spaceRight = space - spaceLeft;
            output << printCenter("=" + getSpace(spaceLeft) + "\x1b[43m" + menu[i] + "\x1b[0m" + getSpace(spaceRight) + " =") << endl;
        } else {
            //tinh toan vi tri de in ra
            int space = 58 - menu[i].length();
            int spaceLeft = space / 2;
            int spaceRight = space - spaceLeft;
            output << printCenter("=" + getSpace(spaceLeft) + menu[i] + getSpace(spaceRight) + " =") << endl;
        }
    }
    output << printCenter("=============================================================") << endl;
    gotoxy(0, 0, output.str());
}

int scoreNum(int count) {
    if (count == 1) {
        return 10;
    } else if (count == 2) {
        return 100;
    } else if (count == 3) {
        return 1000;
    } else if (count == 4) {
        return 10000;
    } else if (count >= 5) {
        return 100000;
    }
    return 0;
}

int solveScore(int board[size][size], int team) {
    int ai = team;
    int player = team == 1 ? 2 : 1;
    int score = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 4; j++) {
            int count = 0;
            for (int k = 0; k < 5; k++) {
                if (board[i][j + k] == ai) {
                    count++;
                } else if (board[i][j + k] == player) {
                    count = 0;
                    break;
                }
            }
            score += scoreNum(count);
        }
    }

    for (int i = 0; i < size - 4; i++) {
        for (int j = 0; j < size; j++) {
            int count = 0;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j] == ai) {
                    count++;
                } else if (board[i + k][j] == player) {
                    count = 0;
                    break;
                }
            }
            score += scoreNum(count);
        }
    }

    for (int i = 0; i < size - 4; i++) {
        for (int j = 0; j < size - 4; j++) {
            int count = 0;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j + k] == ai) {
                    count++;
                } else if (board[i + k][j + k] == player) {
                    count = 0;
                    break;
                }
            }
            score += scoreNum(count);
        }
    }

    for (int i = 0; i < size - 4; i++) {
        for (int j = 4; j < size; j++) {
            int count = 0;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j - k] == ai) {
                    count++;
                } else if (board[i + k][j - k] == player) {
                    count = 0;
                    break;
                }
            }
            score += scoreNum(count);
        }
    }

    return score;
}

BotMove AI(int board[size][size]) {
    int maxScoreAI = 0, maxScorePlayer = 0;
    int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    BotMove botMove;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0) {
                board[i][j] = 2;
                int score = solveScore(board, 2);
                if (score > maxScoreAI) {
                    maxScoreAI = score;
                    x1 = i;
                    y1 = j;
                }
                board[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0) {
                board[i][j] = 1;
                int score = solveScore(board, 1);
                if (score > maxScorePlayer) {
                    maxScorePlayer = score;
                    x2 = i;
                    y2 = j;
                }
                board[i][j] = 0;
            }
        }
    }

    if (maxScoreAI >= 100000) {
        botMove.x = x1;
        botMove.y = y1;
        return botMove;
    }
    
    if (maxScoreAI >= maxScorePlayer) {
        botMove.x = x1;
        botMove.y = y1;
        return botMove;
    } else {
        botMove.x = x2;
        botMove.y = y2;
        return botMove;
    }
}

int main() {
    resizeConsole(consoleWidth, consoleHeight);
    Nocursortype();
    int selectedItem = 0;
    int menuSize = 4;
    char key;
    system("cls");
    do {
        drawMenu(selectedItem);

        key = _getch(); 
        if (key == 72) { 
            selectedItem = (selectedItem - 1 + menuSize) % menuSize;
        } else if (key == 80) {
            selectedItem = (selectedItem + 1) % menuSize;
        }
    } while (key != 32); 

    system("cls");
    Nocursortype();
    PlayAgain playAgain;
    string yellow = "\x1b[33m", reset = "\x1b[0m", blue = "\x1b[34m";
    playAgain.isPlayAgain = false;
    playAgain.x = 0;
    playAgain.y = 0;
    switch (selectedItem) {
        case 0: {
            playGame(true, playAgain);
            break;
        }
        case 1: {
            playGame(false, playAgain);
            break;
        }
        case 2: {
            int Left = 17, Right = 16, Up = 30, Down = 31, Space = 32;
            cout << endl << endl << endl << endl;
            cout << printCenter("=============================================================\n");
            cout << printCenter("=                          HUONG DAN                        =\n");
            cout << printCenter("=============================================================\n");
            cout << endl;
            cout << printCenter(yellow + (char)Up + reset + "\n");
            cout << printCenter(yellow + (char)Left + reset + " " + yellow + (char)Down + reset + " " + yellow + (char)Right + reset + "\n");
            cout << printCenter("Di chuyen: " + yellow + "Phim mui ten" + reset + "\n");
            cout << printCenter("Chon: " + yellow + "Space" + reset + "\n");
            cout << endl;
            cout << printCenter("=============================================================\n");
            cout << endl;
            cout << printCenter("Moi ben lan luot danh vao cac" + yellow + " o trong" + reset + " tren ban co.\n");
            cout << printCenter("Hay co gang tao thanh" + yellow + " 5 quan cung mau" + reset + " lien tiep.\n");
            cout << printCenter("Phat hien chien thuat cua doi phuong de ngan chan ho.\n");
            cout << endl;
            cout << printCenter(yellow + "Chuc ban choi vui ve!" + reset + "\n");
            cout << printCenter(yellow + "X" + reset + "-" + blue + "O" + reset + "-" + yellow + "X" + reset + "-" + blue + "O" + reset + "-" + yellow + "X" + reset + "-" + blue + "O" + reset + "-" + yellow + "X" + reset + "-" + blue + "O" + reset + "-" + yellow + "X" + reset + "-" + blue + "O" + reset + "\n");
            cout << endl;
            cout << printCenter("=============================================================\n");
            cout << printCenter("Press " + yellow + "Space" + reset + " to back menu, " + yellow + "q" + reset + " to quit.\n");
            cout << printCenter("=============================================================\n");

            while (true) {
                int n = getch();
                if (n == Space) {
                    system("cls");
                    main();
                } else if (n == 113) {
                    exit(0);
                }
            }
            break;
        }
        case 3: {
            exit(0);
            break;
        }
        default: {
            break;
        }
    }
    return 0;
}
