#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/koikoi.h"

int main(void) {

    playerID playerId;

    game koikoi;
    field ba;
    players player[GUEST];

    char string[SIZE];
    int round = 1;

    virtualTerminal();
    srand((unsigned int)time(NULL));
    
    printf("\nこいこいにようこそ。\n");

    for (int i = 0; i < GUEST; i++) {
        player[i].totalScore = 0;
    }

    while (round <= finish) {

        printf("\n試合を開始します。-%d月-\n", round);
        setValue(&ba, player, &koikoi);

        printf("\n順番を決めます。\n");
        decideTurn(player, &koikoi);

        printf("\n花札を配ります。\n");
        dealHanahuda(&ba, player, &koikoi);
 
        playerId = YOU;
        printHanafuda(&ba, player, &playerId);

        printf("\n始めます。\n");
        for (int i = 0; i < GUEST; i++) {
            playerId = i;
            judgeTefuda(player, &playerId, &koikoi);
            if (!player[playerId].hand) {
                printf("\n");
                printHandName(player, &playerId);
                printf("が成立しました。\n");
            }
        }
        while (player[YOU].hand && player[COM].hand && player[koikoi.order[0]].qTefuda) {
            for (int i = 0; i < GUEST; i++) {
                playerId = koikoi.order[i];
                selectTefuda(&ba, player, &playerId);
                playTefuda(&ba, player, &playerId);
                judgeHand(player, &playerId, &koikoi);
                if (!player[playerId].hand) {
                    printHanafuda(&ba, player, &playerId);
                    printf("\n");
                    printHandName(player, &playerId);
                    printf("が成立しました。\n");
                    judgeKoi(&ba, player, &playerId);
                    if (player[playerId].koi == YES) {
                        printf("\n");
                    } else if (player[playerId].koi == NO) {
                        break;
                    }
                }
                drawYamafuda(&ba, player, &playerId, &koikoi);
                judgeHand(player, &playerId, &koikoi);
                if (!player[playerId].hand) {
                    printHanafuda(&ba, player, &playerId);
                    printf("\n");
                    printHandName(player, &playerId);
                    printf("が成立しました。\n");
                    judgeKoi(&ba, player, &playerId);
                    if (player[playerId].koi == NO) {
                        break;
                    }
                }
            }
        }
        printf("\n終わりです。\n");

        judgeWinner(player, &koikoi);
        
        printf("\n得点を計算します。\n");
        calculateScore(player, &koikoi);

        printf("\n順位は以下の通りになりました。\n");
        decideRanking(player, &koikoi);

        if (round < finish) {
            printf("\n次の対局に進みます。");
            printf("\n何かキーを押してください。\n");
            scanf("%1s%*[^\n]", string); 
        }
        round++;
    }
    printf("\nありがとうございました。\n");

    return 0;
}