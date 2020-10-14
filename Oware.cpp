#include <iostream>
#include <iomanip>
#include <vector>
#include <Windows.h>
using namespace std;

int seeds[12] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };   // Initial state of the game
int score1 = 0;
int score2 = 0;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // This function is used to change the color of the text, using the ints below
const int RED = 12;
const int WHITE = 15;
const int GREEN = 10;

char houses[6] = { 'a', 'b', 'c', 'd', 'e', 'f' };

/**
Asks the user if they want to play against the AI. If so, asks the user if they want to play it on easy (random) or hard mode.
Otherwise, They'll play against someone else.
*/
void intro(int& AI, int& easy) {
    char res;
    cout << "Do you wish to play against the computer? (Y/N)" << endl;
    cin >> res;
    if (cin.eof()) exit(0);
    while (cin.fail()) {   // Makes sure the player inserts a valid character
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Please answer with Y or N" << endl;
        if (cin.eof()) exit(0);
    }
    while (int(res) != 78 && int(res) != 89 && int(res) != 121 && int(res) != 110) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Please answer with Y or N" << endl;
        cin >> res;
        if (cin.eof()) exit(0);
    }
    if (res == 'Y' || res == 'y') {
        AI = 1;
        char choice;
        cout << "Do you wish to play in hard mode? (Y/N)" << endl;
        cin.ignore(1000, '\n');
        cin >> choice;
        if (cin.eof()) exit(0);
        while (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please answer with Y or N" << endl;
            if (cin.eof()) exit(0);
        }
        while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n') {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please answer with Y or N" << endl;
            cin >> choice;
            if (cin.eof()) exit(0);
        }
        if (choice == 'Y' || choice == 'y') easy = 0;
        else easy = 1;
    }
    else AI = 0;
    cin.ignore(1000, '\n');         // This prevents any character in the buffer from being an answer to another question
}

/**
Draws the table of the game. Should be called each time there's an update in the seeds and scores.
You can change the color of text between RED, WHITE, GREEN or another int value of choice.
*/
void draw(int s1, int s2, int AI) {
    system("CLS");
    SetConsoleTextAttribute(h, WHITE);
    cout << endl << setw(15) << "" << setw(10) << 'f' << setw(10) << 'e' << setw(10) << 'd' << setw(10) << 'c' << setw(10) << 'b' << setw(10) << 'a' << endl << endl << setw(15) << "Player 1";
    SetConsoleTextAttribute(h, RED);
    cout << setw(10) << seeds[11] << setw(10) << seeds[10] << setw(10) << seeds[9] << setw(10) << seeds[8] << setw(10) << seeds[7] << setw(10) << seeds[6];
    SetConsoleTextAttribute(h, WHITE);
    if (AI== 0) cout << setw(15) << "Player 2" << endl << endl << setw(11) << s1 << setw(4) << "";
    else cout << setw(15) << "Computer" << endl << endl << setw(11) << s1 << setw(4) << "";
    SetConsoleTextAttribute(h, GREEN);
    cout << setw(10) << seeds[0] << setw(10) << seeds[1] << setw(10) << seeds[2] << setw(10) << seeds[3] << setw(10) << seeds[4] << setw(10) << seeds[5];
    SetConsoleTextAttribute(h, WHITE);
    cout << setw(11) << s2 << setw(4) << "" << endl << endl << setw(15) << "" << setw(10) << 'A' << setw(10) << 'B' << setw(10) << 'C' << setw(10) << 'D' << setw(10) << 'E' << setw(10) << 'F' << endl << endl;
}

/**
Checks all the indexes in the vector and updates the player's score accordingly.
The vector should only contain the index's of the last touched opponent's houses (empty if the turn ended in a player's house)
*/
void update_score(vector<int> checks, int& score, int seeds[], int player) {
    bool valid = false;                     // The play is invalid if the player would capture all of the opponent's seeds. If this happens, all the seeds remain in place
    int notempty = 0;
    if (player == 1) for (int i = 6; i < 12; ++i) {
        if (seeds[i] > 0) ++notempty;
    }
    else for (int i = 0; i < 6; ++i) {
        if (seeds[i] > 0) ++notempty;
    }
    if (notempty > checks.size()) valid = true;     // If there are more not empty houses than houses to check, the play is automatically valid
    else {
        for (int i = 0; i < checks.size(); ++i) {
            if (seeds[checks[i]] != 2 && seeds[checks[i]] != 3) valid = true;
        }
    }
    if (valid) {
        for (size_t i = 0; i < checks.size(); i++) {
            int index = checks[i];
            if (seeds[index] == 2 || seeds[index] == 3) {
                score += seeds[index];
                seeds[index] = 0;
            }
            else break;
        }
    }
}


/**
Calculates the choice that the AI makes in a given turn.
If the user chose to play on easy mode, it just returns a random house. Otherwise, it calculates the play with the highest immediate score.
*/
int AI_PLAY(int easy) {
    bool empty = true;
    for (int i = 0; i < 6; ++i) {
        if (seeds[i] != 0) {
            empty = false;
            break;
        }
    }
    if (easy == 1) {
        int num = rand() % 6 + 6;
        while (seeds[num] == 0) num = rand() % 6 + 6;   // Doesn't let the computer choose an empty house
        while (empty && (num + seeds[num] < 12)) num = rand() % 6 + 6; // Player's houses can't be empty
        return num;
     }
    else {
        int maxscore = -1;
        int choice;
        int scopy[12];
        vector<int> choices;
       for (int a = 6; a <= 11; ++a) {     // Tries every AI's house to know which has the highest immediate payoff
            if (seeds[a] == 0) continue;    // Doesn't let the computer choose an empty house
            if (empty) {
                if (a + seeds[a] < 12) continue;
            }
            for (int i = 0; i < 12; ++i) scopy[i] = seeds[i];
            int seed = scopy[a];
            int i = a + 1;
            int score = 0;
            vector<int> checks;
            while (seed > 0) {
                int index = i % 12;  // The %12 lets the game cycle through the houses
                ++i;
                if (index == 6) checks.clear();  // If the last seed drops at a player's house, the previous opponent's houses shouldn't be analyzed
                if (index == a) continue;  // Ignores the house that the player chosed
                ++scopy[index];
                if (index < 6) checks.insert(checks.begin(), index);  // If an opponent's house was changed, save its index. It's kept in reversed order
                --seed;
            }
            scopy[a] = 0;
            update_score(checks, score, scopy, 2);
            if (score > maxscore) {
                maxscore = score;
                choices.clear();
                choices.push_back(a);
             }
            else if (score == maxscore) {       // Keep the equal scores so the AI can chose one at random
                choices.push_back(a);
            }
       }
       choice = choices[rand() % choices.size()];
       return choice;
    }
}

/**
Asks the respective player for his move and calculates the distribution of the seeds.
If the AI is in play, gets its choice and distributes the seeds accordingly. (In this case, the player argument should be set to 3)
Also checks how many seeds go for the player's score and removes them from the board.
*/
void game(int player, int &score, int easy) {
    char move;
    int choice, seed, check;
    bool empty = true;
    bool valid = true;
    vector<int> checks;
    if (player < 3) {
        cout << "Player " << player << ", select the cell in order to make your move" << endl;
        cin >> move;
        if (cin.eof()) exit(0);
        while (cin.fail()) {                        // Makes sure the player inserts a valid character
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please insert a letter between A and F" << endl;
            if (cin.eof()) exit(0);
        }
        while (int(move) < 65 || int(move) > 102 || (int(move) > 70 && int(move) < 97)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please insert a letter between A and F" << endl;
            cin >> move;
            if (cin.eof()) exit(0);
        }
        if (player == 1) {
            for (int i = 6; i < 12; ++i) {
                if (seeds[i] != 0) {
                    empty = false;
                    break;
                }
            }
            if (int(move) >= 65 && int(move) <= 70) check = int(move) - 65;
            else check = int(move) - 97;
            if (empty) valid = (check + seeds[check] > 5);
            while (seeds[check] == 0 || !valid) {             // The player mustn't choose an empty house
                cin.clear();                                  // If the opponent has no seeds, the player must give him some
                cin.ignore(1000, '\n');                       // If not possible, the game is already over before this point
                if (seeds[check] == 0) cout << "You can't choose an empty house" << endl;
                else cout << "You must give your opponent some seeds" << endl;
                cin >> move;
                if (cin.eof()) exit(0);
                if (cin.fail()) cout << "Please insert a letter between A and F" << endl;
                while (int(move) < 65 || int(move) > 102 || (int(move) > 70 && int(move) < 97)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Please insert a letter between A and F" << endl;
                    cin >> move;
                    if (cin.eof()) exit(0);
                }
                if (int(move) >= 65 && int(move) <= 70) check = int(move) - 65;
                else check = int(move) - 97;
                if (empty) valid = (check + seeds[check] > 5);
            }
        }
        else {                                  // Same logic for Player2
            for (int i = 0; i < 6; ++i) {
                if (seeds[i] != 0) {
                    empty = false;
                    break;
                }
            }
            if (int(move) >= 65 && int(move) <= 70) check = int(move) - 59;
            else check = int(move) - 91;
            if (empty) valid = (check + seeds[check] > 11);
            while (seeds[check] == 0 || !valid) {
                cin.clear();
                cin.ignore(1000, '\n');
                if (seeds[check] == 0) cout << "You can't choose an empty house" << endl;
                else cout << "You must give your opponent some seeds" << endl;
                cin >> move;
                if (cin.eof()) exit(0);
                if (cin.fail()) cout << "Please insert a letter between A and F" << endl;
                while (int(move) < 65 || int(move) > 102 || (int(move) > 70 && int(move) < 97)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Please insert a letter between A and F" << endl;
                    cin >> move;
                    if (cin.eof()) exit(0);
                }
                if (int(move) >= 65 && int(move) <= 70) check = int(move) - 59;
                else check = int(move) - 91;
                if (empty) valid = (check + seeds[check] > 11);
            }
        }
        cin.ignore(1000, '\n');

        if (int(move) >= 65 && int(move) <= 70) {  // Player typing a char between A and F
            if (player == 1) {
                choice = int(move) - 65;
                seed = seeds[choice];
                int i = choice + 1;
                while (seed > 0) {
                    int index = i % 12;  // The %12 lets the game cycle through the houses
                    ++i;
                    if (index == 0) checks.clear();  // If the last seed drops at a player's house, the previous opponent's houses shouldn't be analyzed
                    if (index == choice) continue;  // Ignores the house that the player chosed
                    ++seeds[index];
                    if (index >= 6) checks.insert(checks.begin(), index);  // If an opponent's house was changed, save its index. It's kept in reversed order
                    --seed;
                }
                seeds[choice] = 0;
            }

            // Same logic, with different ASCII codes and players

            else {
                choice = int(move) - 59;
                seed = seeds[choice];
                int i = choice + 1;
                while (seed > 0) {
                    int index = i % 12;
                    ++i;
                    if (index == choice) continue;
                    if (index == 6) checks.clear();
                    if (index < 6) checks.insert(checks.begin(), index);
                    ++seeds[index];
                    --seed;
                }
                seeds[choice] = 0;
            }
        }
        else if (int(move) >= 97 && int(move) <= 102) {    // Player typing a char between a and f
            if (player == 1) {
                choice = int(move) - 97;
                seed = seeds[choice];
                int i = choice + 1;
                while (seed > 0) {
                    int index = i % 12;
                    ++i;
                    if (index == choice) continue;
                    ++seeds[index];
                    if (index == 0) checks.clear();
                    if (index >= 6) checks.insert(checks.begin(), index);
                    --seed;
                }
                seeds[choice] = 0;
            }
            else {
                choice = int(move) - 91;
                seed = seeds[choice];
                int i = choice + 1;
                while (seed > 0) {
                    int index = i % 12;
                    ++i;
                    if (index == choice) continue;
                    ++seeds[index];
                    if (index == 6) checks.clear();
                    if (index < 6) checks.insert(checks.begin(), index);
                    --seed;
                }
                seeds[choice] = 0;
            }
        }
    }
    else {
        choice = AI_PLAY(easy);
        cout << "The computer chooses house " << houses[choice-6] << endl;
        system("pause");
        seed = seeds[choice];
        int i = choice + 1;
        while (seed > 0) {
            int index = i % 12;
            ++i;
            if (index == choice) continue;
            if (index == 6) checks.clear();
            if (index < 6) checks.insert(checks.begin(), index);
            ++seeds[index];
            --seed;
        }
        seeds[choice] = 0;
    }
    update_score(checks, score, seeds, player);
}

/**
Checks if the game is over and prints the result if so.
If someone is out of seeds and it's impossible for the opponent to give them some, the opponent should capture all the seeds and the game ends.
*/
bool game_end(int& score1, int& score2, int AI) {
    bool empty1 = true;
    bool empty2 = true;
    for (int i = 0; i <= 5; ++i) if (seeds[i] != 0) empty1 = false;
    for (int i = 6; i <= 11; ++i) if (seeds[i] != 0) empty2 = false;
    if (empty1) {
        int rep = false;
        for (int i = 6; i <= 11; ++i) if (i + seeds[i] > 11) rep = true;
        if (!rep) {
            for (int i = 6; i <= 11; ++i) {
                score2 += seeds[i];
                seeds[i] = 0;
            }
            draw(score1, score2, AI);
            if (AI == 0) cout << "Player1 has run out of seeds and it's impossible to continue the game. Player2 captures all the remaining seeds" << endl;
            else cout << "Player1 has run out of seeds and it's impossible to continue the game. The computer captures all the remaining seeds" << endl;
        }
    }
    if (empty2) {
        int rep = false;
        for (int i = 0; i <= 5; ++i) if (i + seeds[i] > 5) rep = true;
        if (!rep) {
            for (int i = 0; i <= 5; ++i) {
                score1 += seeds[i];
                seeds[i] = 0;
            }
            draw(score1, score2, AI);
            if (AI == 0) cout << "Player2 has run out of seeds and it's impossible to continue the game. Player1 captures all the remaining seeds" << endl;
            else cout << "The computer has run out of seeds and it's impossible to continue the game. Player1 captures all the remaining seeds" << endl;
        }
    }


    if (score1 > 24) {
        cout << "Player 1 wins" << endl;
        return true;
    }
    else if (score2 > 24) {
        if (AI == 0) cout << "Player 2 wins" << endl;
        else cout << "The computer wins" << endl;
        return true;
    }
    else if (score1 == 24 && score2 == 24) {
        cout << "It's a draw" << endl;
        return true;
    }
    return false;
}

int main()
{
    int AI, easy;
    srand(time(NULL));
    intro(AI, easy);
    draw(score1, score2, AI);
    while (true) {
        game(1, score1, easy);  // Player1's move
        draw(score1, score2, AI);
        if (game_end(score1, score2, AI)) break;
        if (AI == 0) game(2, score2, easy);  //Player2's move
        else game(3, score2, easy); // AI's move
        draw(score1, score2, AI);
        if (game_end(score1, score2, AI)) break;
    }
}