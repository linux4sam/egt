typedef struct
{
    std::string name;
    int offset;
    int len;
} eraw_st;

eraw_st offset_table[] =
{
    {"bkgrd", 0, 324282},    //0
    {"path1087", 324282, 856},    //1
    {"path1083", 325138, 1082},    //2
    {"path1091", 326220, 1046},    //3
    {"path1095", 327266, 1442},    //4
    {"path1079", 328708, 1426},    //5
    {"path1099", 330134, 1482},    //6
    {"path1075", 331616, 1458},    //7
    {"path1103", 333074, 1468},    //8
    {"path1071", 334542, 1498},    //9
    {"path1107", 336040, 1510},    //10
    {"path1067", 337550, 1464},    //11
    {"path1115", 339014, 1480},    //12
    {"path1111", 340494, 1518},    //13
    {"path1063", 342012, 1498},    //14
    {"path1059", 343510, 1520},    //15
    {"path1119", 345030, 1534},    //16
    {"path1055", 346564, 1444},    //17
    {"path1123", 348008, 1506},    //18
    {"path1051", 349514, 1488},    //19
    {"path1127", 351002, 1484},    //20
    {"path1047", 352486, 1530},    //21
    {"path1131", 354016, 1512},    //22
    {"path1043", 355528, 1472},    //23
    {"path1135", 357000, 1494},    //24
    {"path1039", 358494, 1496},    //25
    {"path1139", 359990, 1504},    //26
    {"path1035", 361494, 1506},    //27
    {"path1143", 363000, 1498},    //28
    {"path1031", 364498, 1486},    //29
    {"path1147", 365984, 1452},    //30
    {"path1027", 367436, 1464},    //31
    {"path1151", 368900, 1474},    //32
    {"path1023", 370374, 1456},    //33
    {"path1155", 371830, 1470},    //34
    {"path1019", 373300, 1442},    //35
    {"path1159", 374742, 1440},    //36
    {"path1015", 376182, 1454},    //37
    {"path1163", 377636, 1454},    //38
    {"path1011", 379090, 1410},    //39
    {"path1167", 380500, 1430},    //40
    {"path1007", 381930, 1420},    //41
    {"path1171", 383350, 1404},    //42
    {"path1003", 384754, 1374},    //43
    {"path1175", 386128, 1416},    //44
    {"path999", 387544, 1386},    //45
    {"path1179", 388930, 1384},    //46
    {"path995", 390314, 1354},    //47
    {"path1183", 391668, 1350},    //48
    {"path991", 393018, 1362},    //49
    {"path1187", 394380, 1268},    //50
    {"path987", 395648, 1342},    //51
    {"path1191", 396990, 1294},    //52
    {"path983", 398284, 1304},    //53
    {"path1195", 399588, 1288},    //54
    {"path979", 400876, 1256},    //55
    {"path1199", 402132, 1242},    //56
    {"path975", 403374, 1236},    //57
    {"path1203", 404610, 1238},    //58
    {"path971", 405848, 1218},    //59
    {"path1207", 407066, 1210},    //60
    {"path967", 408276, 1208},    //61
    {"path1211", 409484, 1170},    //62
    {"path963", 410654, 1180},    //63
    {"path1215", 411834, 1146},    //64
    {"path959", 412980, 1146},    //65
    {"path1219", 414126, 1110},    //66
    {"path955", 415236, 1096},    //67
    {"path1223", 416332, 1108},    //68
    {"path951", 417440, 1066},    //69
    {"path1227", 418506, 1058},    //70
    {"path947", 419564, 1056},    //71
    {"path1231", 420620, 992},    //72
    {"path943", 421612, 988},    //73
    {"path1235", 422600, 998},    //74
    {"path939", 423598, 966},    //75
    {"path1239", 424564, 946},    //76
    {"path935", 425510, 922},    //77
    {"path1243", 426432, 898},    //78
    {"path931", 427330, 886},    //79
    {"path1247", 428216, 860},    //80
    {"path927", 429076, 864},    //81
    {"path1251", 429940, 824},    //82
    {"path923", 430764, 840},    //83
    {"path1255", 431604, 786},    //84
    {"path919", 432390, 782},    //85
    {"path1259", 433172, 756},    //86
    {"path915", 433928, 720},    //87
    {"path1263", 434648, 708},    //88
    {"path911", 435356, 686},    //89
    {"path1267", 436042, 620},    //90
    {"path907", 436662, 654},    //91
    {"path1271", 437316, 550},    //92
    {"path903", 437866, 618},    //93
    {"path1275", 438484, 518},    //94
    {"path899", 439002, 520},    //95
    {"path1279", 439522, 432},    //96
    {"path895", 439954, 500},    //97
    {"path1283", 440454, 372},    //98
    {"path891", 440826, 446},    //99
    {"path1287", 441272, 486},    //100
    {"path887", 441758, 432},    //101
    {"path1291", 442190, 520},    //102
    {"path883", 442710, 490},    //103
    {"path1295", 443200, 592},    //104
    {"path879", 443792, 592},    //105
    {"path1299", 444384, 664},    //106
    {"path875", 445048, 670},    //107
    {"path1303", 445718, 652},    //108
    {"path871", 446370, 694},    //109
    {"path1307", 447064, 730},    //110
    {"path867", 447794, 722},    //111
    {"path1311", 448516, 778},    //112
    {"path863", 449294, 738},    //113
    {"path859", 450032, 826},    //114
    {"path1315", 450858, 810},    //115
    {"path1319", 451668, 840},    //116
    {"path855", 452508, 886},    //117
    {"path1323", 453394, 790},    //118
    {"path851", 454184, 904},    //119
    {"path1327", 455088, 940},    //120
    {"path847", 456028, 930},    //121
    {"fuelr", 456958, 798},    //122
    {"rect948", 457756, 1512},    //123
    {"rect950", 459268, 2238},    //124
    {"rect952", 461506, 2974},    //125
    {"rect954", 464480, 3720},    //126
    {"rect956", 468200, 4478},    //127
    {"rect958", 472678, 5182},    //128
    {"fuelrect", 477860, 5640},    //129
    {"rect971", 483500, 930},    //130
    {"rect973", 484430, 1830},    //131
    {"rect975", 486260, 2658},    //132
    {"rect977", 488918, 3616},    //133
    {"rect979", 492534, 4410},    //134
    {"tempw", 496944, 5258},    //135
    {"temprect", 502202, 5908},    //136
    {"mainspeed", 508110, 12660},    //137
    {"text995", 520770, 4490},    //138
    {"text1031", 525260, 7990},    //139
    {"spd18", 533250, 13248},    //140
    {"g1", 546498, 13248},    //141
    {"text995-2", 559746, 4548},    //142
    {"text1031-4", 564294, 8610},    //143
    {"text1367", 572904, 3998},    //144
    {"lbar", 576902, 1662},    //145
    {"bar0", 578564, 970},    //146
    {"bar5", 579534, 676},    //147
    {"rbar", 580210, 1638},    //148
    {"rbar0", 581848, 952},    //149
    {"rbar5", 582800, 676},    //150
    {"lspeed", 583476, 1038},    //151
    {"text1309", 584514, 406},    //152
    {"text1345", 584920, 538},    //153
    {"rspeed", 585458, 1040},    //154
    {"text1385", 586498, 470},    //155
    {"text1421", 586968, 550},    //156
    {"temp", 587518, 1490},    //157
    {"text1487", 589008, 666},    //158
    {"text1503", 589674, 768},    //159
    {"mute", 590442, 1060},    //160
    {"left_blink", 591502, 1194},    //161
    {"takeoff", 592696, 1354},    //162
    {"farlight", 594050, 890},    //163
    {"vsc", 594940, 822},    //164
    {"calling", 595762, 1282},    //165
    {"bat", 597044, 896},    //166
    {"egoil", 597940, 928},    //167
    {"hazards", 598868, 1480},    //168
    {"snow", 600348, 1720},    //169
    {"abs", 602068, 1710},    //170
    {"wifi", 603778, 1590},    //171
    {"bt", 605368, 1322},    //172
    {"right_blink", 606690, 1264},    //173
    {"engine", 607954, 1388},    //174
    {"callname", 609342, 3620},    //175
    {"callnum", 612962, 9048},    //176
    {"eng5", 622010, 1362},    //177
    {"tc", 623372, 946},    //178
    {"spd18-9", 624318, 10886},    //179
    {"g2", 635204, 10886},    //180
    {"g3", 646090, 10886},    //181
    {"text995-2-5", 656976, 5288},    //182
    {"text1031-4-7", 662264, 5326},    //183
    {"spd61", 667590, 12484},    //184
    {"g2-3", 680074, 12484},    //185
    {"g3-2", 692558, 12484},    //186
    {"g4", 705042, 12484},    //187
    {"text995-2-5-3", 717526, 8224},    //188
    {"text1031-4-7-0", 725750, 4288},    //189
    {"spd99", 730038, 16064},    //190
    {"g2-3-5", 746102, 16064},    //191
    {"g3-2-2", 762166, 16064},    //192
    {"g4-3", 778230, 16064},    //193
    {"g6", 794294, 16064},    //194
    {"text995-2-5-3-1", 810358, 8208},    //195
    {"text1031-4-7-0-9", 818566, 7804},    //196
};