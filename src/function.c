#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/koikoi.h"

//  Virtual Terminal機能を有効にする
void virtualTerminal( void ) {

    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;
    GetConsoleMode(stdOut, &consoleMode);
    SetConsoleMode(stdOut, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

//  エスケープシーケンスを標準出力する
void escapeSequence( char const *string, char const character ) {

    printf("\x1b[");
    printf(string);
    putchar(character);
}

//  RGBを指定して文字の色を変更する
void changeColorRGB( const int *code ) {

    char buf[64];

    snprintf(buf, 64, "38;2;%d;%d;%d", RGB[*code].r, RGB[*code].g, RGB[*code].b);
    escapeSequence(buf, 'm');
}

//  色情報をリセットする
void resetColor( void ) {

    escapeSequence("0", 'm');
}

//  カーソルを右に移動する
void moveCursorRight( const int column ) {

    char buf[8];

    snprintf(buf, 8, "%d", column);
    escapeSequence(buf, 'C');
}

//  値をセットする
void setValue( field *ba, players *player, game *koikoi ) {

    for (int i = 0; i < SIZE; i++) {
        ba->bafuda[i] = 0;
        ba->bMonth[i] = 0;
    }
    ba->qBafuda = 0;

    for (int i = 0; i < GUEST; i++) {
        for (int j = 0; j < DECK; j++) {
            player[i].motifuda[j] = 0;
        }
        for (int j = 0; j < TEFUDA; j++) {
            player[i].tefuda[j] = 0;
            player[i].tMonth[j] = 0;
        }
        for (int j = 0; j < 14; j++) {
            player[i].record[j] = 0;
        }
        player[i].flag = 0;
        player[i].qMotifuda = 0;
        player[i].qTefuda = 0;
        player[i].tane = 0;
        player[i].tan = 0;
        player[i].kasu = 0;
        player[i].hand = NOTYET;
        player[i].koi = NO;
    }

    for (int i = 0; i < DECK; i++) {
        koikoi->fuda[i] = 0;
    }
    for (int i = 0; i < GUEST; i++) {
        koikoi->order[i] = 0;
        koikoi->ranking[i] = 0;
    }
}

//  1~48を重複しないでランダムに生成する
int  generateNumber( game *koikoi ) {

    int N;

    do {
        N = rand() % 48 + 1;
    } while (koikoi->fuda[N]);
    koikoi->fuda[N]++;

    return N;
}

//  プレーヤーの名前を表示する
void printPlayerName( int playerId ) {

    for (int i = 0; name.string[playerId][i] != '\0'; i++) {
        printf("%c", name.string[playerId][i]);
    }
}

//  札の名前を表示する
void printFudaName( const int *cardId, const int *suit, int length ) {

    int column;

    if (length) {
        column = (length - hanafuda.length[*cardId]) / 2;
        if (column) {
            moveCursorRight(column);
        }
    }
    changeColorRGB(suit);
    for (int i = 0; hanafuda.string[*cardId][i] != '\0'; i++) {
        printf("%c", hanafuda.string[*cardId][i]);
    }
    resetColor();
    if (length) {
        if (column) {
            moveCursorRight(column);
        }
    }
}

//  順番を決める
void decideTurn( players *player, game *koikoi ) {

    static int count = 0;

    if (count) {

        koikoi->order[0] = koikoi->winner;
        koikoi->order[1] = koikoi->loser;

    } else {

        playerID priority;
        status decision;
        char sign = '"';
        int month[GUEST];
        int number[GUEST];
        int cardId;
        int suit;

        do {
            decision = DONE;
            for (int i = 0; i < GUEST; i++) {
                printPlayerName(i);
                printf(": %c", sign);
                cardId = generateNumber(koikoi);
                suit = (cardId - 1) % 12 + 1;
                printFudaName(&cardId, &suit, CHARACTER1);
                printf("%c を引きました。\n", sign);
                number[i] = cardId;
                month[i] = suit;
            }
            if (month[YOU] == month[COM] && hanafuda.point[number[YOU]] == hanafuda.point[number[COM]]) {
                decision = NOTYET;
                printf("\nもう1度やり直します。\n\n");
            }
        } while (decision);
        if (month[YOU] < month[COM]) {
            priority = YOU;
        } else if (month[YOU] > month[COM]) {
            priority = COM;
        } else {
            if (hanafuda.point[number[YOU]] > hanafuda.point[number[COM]]) {
                priority = YOU;
            } else if (hanafuda.point[number[YOU]] < hanafuda.point[number[COM]]) {
                priority = COM;
            }
        }
        if (!priority) {
            koikoi->order[0] = YOU;
            koikoi->order[1] = COM;
        } else {
            koikoi->order[0] = COM;
            koikoi->order[1] = YOU;
        }
        for (int i = 0; i < DECK; i++) {
            koikoi->fuda[i] = 0;
        }
    }
    count++;

    printf("\n順番は以下の通りになりました。\n");
    for (int i = 0; i < GUEST; i++) {
        printf("%d.", i + 1);
        printPlayerName(koikoi->order[i]);
        printf("\n");
    }
}

//  スートに変換する
void convertToSuit( int *card, int *suit, int start, int end ) {

    for (int i = start; i < end; i++) {
        suit[i] = 0;
    }
    for (int i = start; i < end; i++) {
        suit[i] = (card[i] - 1) % 12 + 1;
    }
}

//  花札を配る
void dealHanahuda( field *ba, players *player, game *koikoi ) {

    status deal;
    int suit[MONTH] = {0};

    while (1) {
        deal = DONE;
        for (int i = 0; i < BAFUDA; i++) {
            ba->bafuda[i] = generateNumber(koikoi);
            ba->qBafuda++;
        }
        convertToSuit(ba->bafuda, ba->bMonth, 0, ba->qBafuda);
        for (int i = 0; i < ba->qBafuda; i++) {
            suit[ba->bMonth[i]]++;
        }
        for (int i = 0; i < MONTH; i++) {
            if (suit[i] >= 4) {
                deal = NOTYET;
            }
            suit[i] = 0;
        }
        for (int i = 0; i < GUEST; i++) {
            for (int j = 0; j < TEFUDA; j++) {
                player[i].tefuda[j] = generateNumber(koikoi);
                player[i].qTefuda++;
            }
            convertToSuit(player[i].tefuda, player[i].tMonth, 0, player[i].qTefuda);
        }
        if (!deal) {
            break;
        }
        printf("\n札が4枚揃ったので配り直します。\n");
        setValue(ba, player, koikoi);
    }
}

//  ハイフンを表示する
void printHyphen( const int *count ) {

    for (int i = 0; i < *count; i++) {
        printf("-");
    }
}

//  持札の中で最も点数が高い札の通し番号を返す
int getCardId( players *player, int playerId, int *card ) {

    for (int i = 1; i < DECK; i++) {
        if (card[i] == playerId && hanafuda.point[i] == 20) {
            card[i] = -1;
            return i;
        }
    }
    for (int i = 1; i < DECK; i++) {
        if (card[i] == playerId && hanafuda.point[i] == 10) {
            card[i] = -1;
            return i;
        }
    }
    for (int i = 1; i < DECK; i++) {
        if (card[i] == playerId && hanafuda.point[i] == 5) {
            card[i] = -1;
            return i;
        }
    }
    for (int i = 1; i < DECK; i++) {
        if (card[i] == playerId && hanafuda.point[i] == 1) {
            card[i] = -1;
            return i;
        }
    }
    return 0;
}

//  場札、手札、持札を表示
void printHanafuda( field *ba, players *player, playerID *playerId ) {

    int card[DECK];
    int count;
    int maxLine;

    for (int i = 0; i < DECK; i++) {
        card[i] = -1;
    }
    for (int i = 0; i < GUEST; i++) {
        for (int j = 1; j < DECK; j++) {
            if (player[i].motifuda[j]) {
                card[j] = i;
            }
        }
    }
    maxLine = ba->qBafuda;
    for (int i = 0; i < GUEST; i++) {
        if (maxLine < player[i].qTefuda) {
            maxLine = player[i].qTefuda;
        }
    }
    for (int i = 0; i < GUEST; i++) {
        if (maxLine < player[i].qMotifuda) {
            maxLine = player[i].qMotifuda;
        }
    }

    printf("\n");
    count = (CHARACTER2 - 4) / 2;
    printHyphen(&count);
    printf("場札");
    printHyphen(&count);
    moveCursorRight(COLUMN);
    count = (CHARACTER3 - name.length[*playerId] - 6) / 2;
    printHyphen(&count);
    printPlayerName(*playerId);
    printf("の手札");
    printHyphen(&count);
    for (int i = 0; i < GUEST; i++) {
        moveCursorRight(COLUMN);
        count = (CHARACTER2 - name.length[i] - 6) / 2;
        printHyphen(&count);
        printPlayerName(i);
        printf("の持札");
        printHyphen(&count);
    }
    printf("\n");
    for (int line = 0; line < maxLine; line++) {
        if (line >= ba->qBafuda) {
            moveCursorRight(CHARACTER2);
        } else {
            printFudaName(&(ba->bafuda[line]), &(ba->bMonth[line]), CHARACTER2);
        }
        moveCursorRight(COLUMN);
        if (line >= player[*playerId].qTefuda) {
            moveCursorRight(CHARACTER3);
        } else {

            int cardId;
            int column;

            if (line <= 8) {
                printf(" ");
            }
            printf("%d.", line + 1);
            switch (*playerId) {
                case YOU:
                    cardId = player[*playerId].tefuda[line];
                    break;
                case COM:
                    cardId = 0;            
                    break;
                default:
                    break;
            }
            column = (CHARACTER3 - hanafuda.length[cardId]) / 2 - 3;
            if (column) {
                moveCursorRight(column);
            }
            switch (*playerId) {
                case YOU:
                    printFudaName(&cardId, &(player[*playerId].tMonth[line]), 0);
                    break;
                case COM:
                    printFudaName(&cardId, &cardId, 0);            
                    break;
                default:
                    break;
            }
            column = (CHARACTER3 - hanafuda.length[cardId]) / 2;
            moveCursorRight(column);
        }
        for (int i = 0; i < GUEST; i++) {
            moveCursorRight(COLUMN);
            if (line >= player[i].qMotifuda) {
                moveCursorRight(CHARACTER2);
            } else {

                int cardId;
                int suit;

                cardId = getCardId(player, i, card);
                suit = (cardId - 1) % 12 + 1;
                printFudaName(&cardId, &suit, CHARACTER2);
            }
        }
        printf("\n");
    }
}

//  手札を評価する
void judgeTefuda( players *player, playerID *playerId, game *koikoi ) {

    int suit[MONTH] = {0};
    int qTefuda = player[*playerId].qTefuda;

    for (int i = 0; i < qTefuda; i++) {
        suit[player[*playerId].tMonth[i]]++;
    }
    player[*playerId].hand = DONE;
    for (int i = 1; i < MONTH; i++) {
        if (suit[i] == 4) {
            player[*playerId].hand = NOTYET;
            break;
        } else if (suit[i] % 2) {
            player[*playerId].hand = NOTYET;
            break;
        }
    }
    if (player[*playerId].hand) {
        for (int i = 1; i < MONTH; i++) {
            if (suit[i] == 4) {
                player[*playerId].hand = DONE;
                player[*playerId].record[12] = 1; 
                break;
            } 
        }
    } else {
        player[*playerId].record[13] = 1;
    }
}

//  場に出す札を決める
void selectTefuda( field *ba, players *player, playerID *playerId ) {

    char sign = '"';
    int cardId = 100;
    int flag = -1;
    int input;
    int qBafuda = ba->qBafuda;
    int qTefuda = player[*playerId].qTefuda;

    printf("\n");
    printPlayerName(*playerId);
    printf("の番です。\n");

    switch (*playerId) {
        case YOU:
            printHanafuda(ba, player, playerId);
            printf("\n何枚目の札を場に出しますか? ");
            while (1) {
                scanf("%d", &input);
                if (input > 0 && input <= qTefuda) {
                    break;
                }
                printf("有効な値ではありません。");
            }
            input--;
            flag = input;
            break;
        case COM:
            for (int i = 0; i < qTefuda; i++) {
                for (int j = 0; j < qBafuda; j++) {
                    if (player[*playerId].tMonth[i] == ba->bMonth[j]) {
                        if (cardId > ba->bafuda[j]) {
                            cardId = ba->bafuda[j];
                            flag = i;
                        }
                    }
                }
            }
            if (flag == -1) {
                cardId = 0;
                for (int i = 0; i < qTefuda; i++) {
                    if (cardId < player[*playerId].tefuda[i]) {
                        cardId = player[*playerId].tefuda[i];
                        flag = i;
                    }
                }
            }
            printf("%c", sign);
            printFudaName(&(player[*playerId].tefuda[flag]), &(player[*playerId].tMonth[flag]), CHARACTER1);
            printf("%c を出しました。\n", sign);
            break;
        default:
            break;
    }
    player[*playerId].flag = flag;
}

//  札を移動する
void moveHanafuda( field *ba, players *player, playerID *playerId, int *cardId, int *suit ) {

    int select[3] = {0};
    int count = 0;
    int qBafuda = ba->qBafuda;
    int qTefuda = player[*playerId].qTefuda;
    int qMotifuda = player[*playerId].qMotifuda;

    for (int i = 0; i < qBafuda; i++) {
        if (*suit == ba->bMonth[i]) {
            select[count] = i;
            count++;
        }
    }
    
    if (count == 3) {

        int tmp[SIZE] = {0};
        int index = 0;

        for (int i = 0; i < count; i++) {
            player[*playerId].motifuda[ba->bafuda[select[i]]]++;
            qMotifuda++;
        }
        for (int i = 0; i < qBafuda; i++) {
            if (i != select[0] && i != select[1] && i != select[2]) {
                tmp[index] = ba->bafuda[i];
                index++;
            }
        }
        for (int i = 0; i < qBafuda; i++) {
            ba->bafuda[i] = tmp[i];
        }
        qBafuda -= count;
        player[*playerId].motifuda[*cardId]++;
        qMotifuda++;
        convertToSuit(ba->bafuda, ba->bMonth, 0, qBafuda);
    } else {

        int column;
        int input;
        int flag;

        if (count == 2) {
            if (*playerId == YOU) {
                printf("\nどの札に合わせますか?\n");
                for (int i = 0; i < count; i++) {
                    printf("%d.", i + 1);
                    column = (CHARACTER1 - hanafuda.length[ba->bafuda[select[i]]]) / 2;
                    if (column) {
                        moveCursorRight(column);
                    }
                    printFudaName(&(ba->bafuda[select[i]]), &(ba->bMonth[select[i]]), 0);              
                    printf("\n");
                }
                while (1) {
                    scanf("%d", &input);
                    if (input > 0 && input <= count) {
                        break;
                    }
                    printf("有効な値ではありません。");
                }
                input--;
                flag = select[input];
            } else if (*playerId == COM) {
                flag = select[0];
                if (ba->bafuda[flag] > ba->bafuda[select[1]]) {
                    flag = select[1];
                }
            }
        } else if (count == 1) {
            flag = select[0];
        }
        if (count) {  /* 場札と一致する手札がある場合 */
            player[*playerId].motifuda[ba->bafuda[flag]]++;
            qMotifuda++;
            player[*playerId].motifuda[*cardId]++;
            qMotifuda++;
            qBafuda--;
            for (int i = flag; i < qBafuda; i++) {
                ba->bafuda[i] = ba->bafuda[i + 1];
                ba->bMonth[i] = ba->bMonth[i + 1];
            }
            ba->bafuda[qBafuda] = 0;
            ba->bMonth[qBafuda] = 0;
        } else {  /* 場札と一致する手札が1枚もない場合 */
            ba->bafuda[qBafuda] = *cardId;
            ba->bMonth[qBafuda] = *suit;
            qBafuda++;
        }
    }
    ba->qBafuda = qBafuda;
    player[*playerId].qTefuda = qTefuda;
    player[*playerId].qMotifuda = qMotifuda;
}

//  選択した手札を場に出す
void playTefuda( field *ba, players *player, playerID *playerId ) {

    int flag = player[*playerId].flag;
    int qTefuda = player[*playerId].qTefuda;

    moveHanafuda(ba, player, playerId, &(player[*playerId].tefuda[flag]), &(player[*playerId].tMonth[flag]));

    qTefuda--;
    for (int i = flag; i < qTefuda; i++) {
        player[*playerId].tefuda[i] = player[*playerId].tefuda[i + 1];
        player[*playerId].tMonth[i] = player[*playerId].tMonth[i + 1];
    }
    player[*playerId].tefuda[qTefuda] = 0;
    player[*playerId].tMonth[qTefuda] = 0;

    player[*playerId].flag = -1;
    player[*playerId].qTefuda = qTefuda;
}

//  山札を場に出す
void drawYamafuda( field *ba, players *player, playerID *playerId, game *koikoi ) {

    char sign = '"';

    int cardId;
    int suit;

    cardId = generateNumber(koikoi);
    suit = (cardId - 1) % 12 + 1;
    printf("%c", sign);
    printFudaName(&cardId, &suit, CHARACTER1);
    printf("%c を引きました。\n", sign);

    moveHanafuda(ba, player, playerId, &cardId, &suit);
}

//  札を数える
void countHanafuda( players *player, playerID *playerId, int *kou, int *tane, int *tan, int *kasu ) {

    *kou = 0;
    *tane = 0;
    *tan = 0;
    *kasu = 0;

    for (int i = 1; i < DECK; i++) {
        if (player[*playerId].motifuda[i] && hanafuda.point[i] == 20) {
            (*kou)++;
        }
        if (player[*playerId].motifuda[i] && hanafuda.point[i] == 10) {
            (*tane)++;
        }
        if (player[*playerId].motifuda[i] && hanafuda.point[i] == 5) {
            (*tan)++;
        }
        if (player[*playerId].motifuda[i] && hanafuda.point[i] == 1) {
            (*kasu)++;
        }
    }
}

//  役を判定する
void judgeHand( players *player, playerID *playerId, game *koikoi ) {

    int kou;
    int tane;
    int tan;
    int kasu;

    countHanafuda(player, playerId, &kou, &tane, &tan, &kasu);

    switch (kou) {
        case 5:
            player[*playerId].record[0]++;
            if (player[*playerId].record[0] == 1) {
                player[*playerId].hand = DONE;
            }
            break;
        case 4:
            if (!player[*playerId].motifuda[11]) {
                player[*playerId].record[1]++;
                if (player[*playerId].record[1] == 1) {
                        player[*playerId].hand = DONE;
                }
            } else {
                player[*playerId].record[2]++;
                if (player[*playerId].record[2] == 1) {
                    player[*playerId].hand = DONE;
                }
            }
            break;
        case 3:
            if (!player[*playerId].motifuda[11]) {
                player[*playerId].record[3]++;
                if (player[*playerId].record[3] == 1) {
                        player[*playerId].hand = DONE;
                }
            }
            break;
        default:
            break;
    }
    if (player[*playerId].motifuda[13] && player[*playerId].motifuda[14] && player[*playerId].motifuda[15]) {
        player[*playerId].record[4]++;
        if (player[*playerId].record[4] == 1) {
            player[*playerId].hand = DONE;
        }
    }
    if (player[*playerId].motifuda[18] && player[*playerId].motifuda[21] && player[*playerId].motifuda[22]) {
        player[*playerId].record[5]++;
        if (player[*playerId].record[5] == 1) {
            player[*playerId].hand = DONE;
        }
    }
    if (player[*playerId].motifuda[3] && player[*playerId].motifuda[9]) {
        player[*playerId].record[6]++;
        if (player[*playerId].record[6] == 1) {
            player[*playerId].hand = DONE;
        }
    }
    if (player[*playerId].motifuda[8] && player[*playerId].motifuda[9]) {
        player[*playerId].record[7]++;
        if (player[*playerId].record[7] == 1) {
            player[*playerId].hand = DONE;
        }
    }
    if (player[*playerId].motifuda[6] && player[*playerId].motifuda[7] && player[*playerId].motifuda[10]) {
        player[*playerId].record[8]++;
        if (player[*playerId].record[8] == 1) {
            player[*playerId].hand = DONE;
        }
    }
    if (tane >= 5) {
        player[*playerId].record[9]++;
        if (tane > player[*playerId].tane) {
            player[*playerId].record[9] = 1;
            player[*playerId].tane = tane;
            player[*playerId].hand = DONE;
        }        
    }
    if (tan >= 5) {
        player[*playerId].record[10]++;
        if (tan > player[*playerId].tan) {
            player[*playerId].record[10] = 1;
            player[*playerId].tan = tan;
            player[*playerId].hand = DONE;
        }  
    }
    if (kasu >= 10) {
        player[*playerId].record[11]++;
        if (kasu > player[*playerId].kasu) {
            player[*playerId].record[11] = 1;
            player[*playerId].kasu = kasu;
            player[*playerId].hand = DONE;
        }
    }
}

//  役の名前を表示する
void printHandName( players *player, playerID *playerId ) {

    int code = 0;

    changeColorRGB(&code);
    if (player[*playerId].record[0] == 1) {
        printf("五光 ");
    }
    if (player[*playerId].record[1] == 1) {
        printf("四光 ");
    }
    if (player[*playerId].record[2] == 1) {
        printf("雨四光 ");
    }
    if (player[*playerId].record[3] == 1) {
        printf("三光 ");
    }
    if (player[*playerId].record[4] == 1) {
        printf("赤短 ");
    }
    if (player[*playerId].record[5] == 1) {
        printf("青短 ");
    }
    if (player[*playerId].record[6] == 1) {
        printf("花見酒 ");
    }
    if (player[*playerId].record[7] == 1) {
        printf("月見酒 ");
    }
    if (player[*playerId].record[8] == 1) {
        printf("猪鹿蝶 ");
    }
    if (player[*playerId].record[9] == 1) {
        printf("タネ ");
    }
    if (player[*playerId].record[10] == 1) {
        printf("タン ");
    }
    if (player[*playerId].record[11] == 1) {
        printf("カス ");
    }
    if (player[*playerId].record[12] == 1) {
        printf("手四 ");
    }
    if (player[*playerId].record[13] == 1) {
        printf("喰付 ");
    }
    resetColor();
}

//  入力を変換する
void convertInput( char *input, command *output ) {

    int column = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        column++;
    }
    switch (column) {
        case 1:
            if (input[0] == 'y') {
                *output = YES;
            } else if (input[0] == 'n') {
                *output = NO;
            } else {
                *output = INVALID;
            }
            break;
        default:
            *output = INVALID;
            break;
    }
}

//  こいこいか勝負かを決める
void judgeKoi( field *ba, players *player, playerID *playerId ) {

    if (player[*playerId].qTefuda) {
        switch (*playerId) {
            case YOU:
                {
                    char input[SIZE];
                    
                    printf("\nこいこいしますか? (Y/N) ");
                    while (1) {
                        scanf("%2s%*[^\n]", input);
                        getchar();
                        convertInput(input, &(player[*playerId].koi));
                        if (player[*playerId].koi == YES || player[*playerId].koi == NO) {
                            break;
                        }
                        printf("有効な値ではありません。");
                    }
                }
                break;
            case COM:
                {   
                    playerID rival = YOU;

                    int flag = 0;
                    int kou;
                    int tane;
                    int tan;
                    int kasu;

                    if (player[*playerId].qTefuda == 1) {
                        flag = 1;
                        for (int i = 0; i < ba->qBafuda; i++) {
                            if (player[*playerId].tMonth[0] == ba->bMonth[i]) {
                                flag = 0;
                                break;
                            }
                        }
                    }

                    countHanafuda(player, &rival, &kou, &tane, &tan, &kasu);

                    if (flag) {
                        player[*playerId].koi = NO;
                    } else if (kou >= 2 || tane >= 4 || tan >= 4 || kasu >= 9) {
                        player[*playerId].koi = NO;
                    } else {
                        player[*playerId].koi = YES;
                    }
                }
                break;
            default:
                break;
        }
    } else {
        player[*playerId].koi = NO;
    }

    if (player[*playerId].koi == NO) {
        printf("<<< 勝負 >>>\n");
    } else if (player[*playerId].koi == YES) {
        player[*playerId].hand = NOTYET;
        printf("<<< こいこい >>>\n");
    }
}

//  勝者を判定する
void judgeWinner( players *player, game *koikoi ) {

    if (player[YOU].hand == player[COM].hand) {
        koikoi->winner = koikoi->order[0];
    } else if (!player[YOU].hand) {
        koikoi->winner = YOU;
    } else if (!player[COM].hand) {
        koikoi->winner = COM;
    }

    if (koikoi->winner == YOU) {
        koikoi->loser = COM;
    } else {
        koikoi->loser = YOU;
    }
}

//  得点を計算する
void calculateScore( players *player, game *koikoi ) {

    int column;
    int kou;
    int tane;
    int tan;
    int kasu;

    countHanafuda(player, &(koikoi->winner), &kou, &tane, &tan, &kasu);

    for (int i = 0; i < GUEST; i++) {
        player[i].score = 0;
    }

    if (player[YOU].hand && player[COM].hand) {
        player[koikoi->winner].score += 6;
    } else {
        if (player[koikoi->winner].record[0]) {
            player[koikoi->winner].score += 15;
        } else if (player[koikoi->winner].record[1]) {
            player[koikoi->winner].score += 10;
        } else if (player[koikoi->winner].record[2]) {
            player[koikoi->winner].score += 8;
        } else if (player[koikoi->winner].record[3]) {
            player[koikoi->winner].score += 6;
        }
        if (player[koikoi->winner].record[4]) {
            player[koikoi->winner].score += 6;
        }
        if (player[koikoi->winner].record[5]) {
            player[koikoi->winner].score += 6;
        }
        if (player[koikoi->winner].record[6]) {
            player[koikoi->winner].score += 5;
        }
        if (player[koikoi->winner].record[7]) {
            player[koikoi->winner].score += 5;
        }
        if (player[koikoi->winner].record[8]) {
            player[koikoi->winner].score += 5;
        }
        if (player[koikoi->winner].record[9]) {
            player[koikoi->winner].score += (tane - 4);
        }
        if (player[koikoi->winner].record[10]) {
            player[koikoi->winner].score += (tan - 4);
        }
        if (player[koikoi->winner].record[11]) {
            player[koikoi->winner].score += (kasu - 9);
        }
        if (player[koikoi->winner].record[12]) {
            player[koikoi->winner].score += 6;
        }
        if (player[koikoi->winner].record[13]) {
            player[koikoi->winner].score += 6;
        }
        if (player[koikoi->winner].score >= 7) {
            player[koikoi->winner].score = player[koikoi->winner].score * 2;
        }
        if (player[koikoi->loser].koi == YES) {
            player[koikoi->winner].score = player[koikoi->winner].score * 2;
        }
    }
    player[koikoi->winner].totalScore += player[koikoi->winner].score;

    for (int i = 0; i < GUEST; i++) {
        printPlayerName(i);
        column = NAME - name.length[i];
        if (column) {
            moveCursorRight(column);
        }
        printf(": %d", player[i].score);
        printf("文\n");
    }
}

//  順位を決める
void decideRanking( players *player, game *koikoi ) {

    int column;
    int playerId;
    int rank;

    if (player[YOU].totalScore >= player[COM].totalScore) {
        koikoi->ranking[0] = YOU;
        koikoi->ranking[1] = COM;
    } else {
        koikoi->ranking[0] = COM;
        koikoi->ranking[1] = YOU;
    }

    for (int i = 0; i < GUEST; i++) {
        if (!i) {
            rank = i + 1;
            printf("%d位 ", rank);
        } else {
            if (player[koikoi->ranking[i]].totalScore == player[koikoi->ranking[i - 1]].totalScore) {
                printf("%d位 ", rank);
            } else {
                rank = i + 1;
                printf("%d位 ", rank);
            }
        }
        playerId = koikoi->ranking[i]; 
        printPlayerName(playerId);
        column = NAME - name.length[playerId];
        if (column) {
            moveCursorRight(column);
        }
        printf(" %d文\n", player[playerId].totalScore);
    }
}
