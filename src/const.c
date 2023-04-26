#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/koikoi.h"

const card hanafuda = { { "****", "松に鶴"  , "梅に鶯"  , "桜に幕"  , "藤に不如帰", "菖蒲に八橋", "牡丹に蝶"  , "萩に猪"  , "芒に月"  , "菊に盃"  , "紅葉に鹿"  , "柳に小野道風", "桐に鳳凰",  \
                                  "松に赤短", "梅に赤短", "桜に赤短", "藤に短冊"  , "菖蒲に赤短", "牡丹に青短", "荻に短冊", "芒に雁"  , "菊に青短", "紅葉に青短", "柳に燕"      , "桐のカス",  \
                                  "松のカス", "梅のカス", "桜のカス", "藤のカス"  , "菖蒲のカス", "牡丹のカス", "荻のカス", "芒のカス", "菊のカス", "紅葉のカス", "柳に短冊"    , "桐のカス",  \
                                  "松のカス", "梅のカス", "桜のカス", "藤のカス"  , "菖蒲のカス", "牡丹のカス", "荻のカス", "芒のカス", "菊のカス", "紅葉のカス", "柳のカス"    , "桐のカス" },\

                        { 4,  6,  6,  6, 10, 10,  8,  6,  6,  6,  8, 12,  8,  \
                              8,  8,  8,  8, 10, 10,  8,  6,  8, 10,  6,  8,  \
                              8,  8,  8,  8, 10, 10,  8,  8,  8, 10,  8,  8,  \
                              8,  8,  8,  8, 10, 10,  8,  8,  8, 10,  8,  8 },\
                                                                              \
                        { 0, 20, 10, 20, 10, 10, 10, 10, 20, 10, 10, 20, 20,  \
                              5,  5,  5,  5,  5,  5,  5, 10,  5,  5, 10,  1,  \
                              1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  5,  1,  \
                              1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1 } };

const playerName name = { { "あなた", "あいて" },\

                          { 6, 6 } };

const color RGB[MONTH] = { { 250, 250, 250 },\
                           {  15,  90,  20 },\
                           { 170,  60, 130 },\
                           { 245, 160, 240 },\
                           { 205, 100, 255 },\
                           { 110,  80, 250 },\
                           { 230,  80, 110 },\
                           { 240,  90, 230 },\
                           { 230, 200, 160 },\
                           { 240, 220,  30 },\
                           { 200,  90,  10 },\
                           { 140, 185, 130 },\
                           { 210, 150, 254 } };

const int finish = 3;