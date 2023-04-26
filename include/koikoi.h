#ifndef INCLUDE_KOIKOI
#define INCLUDE_KOIKOI

#define DECK 49
#define MONTH 13

#define GUEST 2
#define BAFUDA 8
#define TEFUDA 8

#define CHARACTER1 14    //  必ず14以上の値とする
#define CHARACTER2 16
#define CHARACTER3 20  //  必ず20以上の値とする
#define COLUMN 2
#define NAME 6
#define SIZE 32

typedef enum PlayerID {
    YOU, COM
} playerID;

typedef enum Command {
    YES, NO, INVALID
} command;

typedef enum status {
    DONE, NOTYET
} status;

typedef struct Field {
    int bafuda[SIZE]; //  場札の通し番号保存用配列
    int bMonth[SIZE]; //  場札のスート保存用配列
    int qBafuda;      //  場札の枚数
} field;

typedef struct Players {
    int motifuda[DECK]; //  持札の通し番号保存用配列
    int tefuda[TEFUDA]; //  手札の通し番号保存用配列
    int tMonth[TEFUDA]; //  手札のスート保存用配列
    int record[14];
    int flag;
    int qMotifuda;  //  持札の枚数
    int qTefuda;    //  手札の枚数
    int tane;
    int tan;
    int kasu;
    int score;      //  合計点
    int totalScore; //  最終合計点
    status hand;
    command koi;
} players;

typedef struct Game {
    int fuda[DECK];
    playerID order[GUEST];
    playerID ranking[GUEST];
    playerID winner;
    playerID loser;
} game;

typedef struct PlayerName {
    char string[GUEST][SIZE];
    int length[GUEST];
} playerName;

typedef struct Card {
    char string[DECK][SIZE]; //  花札の名前
    int length[DECK];        //  名前の長さ
    int point[DECK];         //  花札の点数
} card;

typedef struct Color {
    int r;
    int g;
    int b;
} color;

void virtualTerminal(void);
void escapeSequence(char const *string, char const character);
void changeColorRGB(const int *code);
void resetColor(void);
void moveCursorRight(const int column);
void setValue(field *ba, players *player, game *koikoi);
int  generateNumber(game *koikoi);
void printPlayerName(int playerId);
void printFudaName(const int *cardId, const int *suit, int length);
void decideTurn(players *player, game *koikoi);
void convertToSuit(int *card, int *suit, int start, int end);
void dealHanahuda(field *ba, players *player, game *koikoi);
void printHyphen (const int *count);
int getCardId( players *player, int playerId, int *card );
void printHanafuda(field *ba, players *player, playerID *playerId);
void judgeTefuda(players *player, playerID *playerId, game *koikoi);
void selectTefuda(field *ba, players *player, playerID *playerId);
void moveHanafuda(field *ba, players *player, playerID *playerId, int *cardId, int *suit);
void playTefuda(field *ba, players *player, playerID *playerId);
void drawYamafuda(field *ba, players *player, playerID *playerId, game *koikoi);
void countHanafuda(players *player, playerID *playerId, int *kou, int *tane, int *tan, int *kasu);
void judgeHand(players *player, playerID *playerId, game *koikoi);
void printHandName(players *player, playerID *playerId);
void convertInput(char *input, command *output);
void judgeKoi(field *ba, players *player, playerID *playerId);
void judgeWinner(players *player, game *koikoi);
void calculateScore(players *player, game *koikoi);
void decideRanking(players *player, game *koikoi);

extern const card hanafuda;
extern const playerName name;
extern const color RGB[MONTH];

extern const int finish;

#endif//INCLUDE_KOIKOI