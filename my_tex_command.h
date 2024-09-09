//
//  my_tex_command.h
//  kunten
//
//  Created by 岡崎康浩 on 2023/06/26.
//

#ifndef my_tex_command_h
#define my_tex_command_h
typedef struct env_com{
    char        env_name[20];
    int         mod;
} env_com;
typedef struct items{
    unsigned char   item_str[128];
    int             head_com;
} items;
typedef struct  enum_com{
    char        env_mark[64];
    int         item_num;
    int         title;
    items*      each_item[10];
} enum_com;
typedef struct  title_style{
    int         moji_size;
    int         sub_moji_size;
} title_style;
typedef struct  daimon_style{
    int         moji_size;
    int         position;
} daimon_style;
int     shomon_style;
int     mondai_moji_size;
int     choice_question_default;
title_style     my_title;
daimon_style    my_daimon;
env_com         nest_env[10];
int             env_nest_num;
float raise_value[8] = {0,-0.6,0.6,-0.2,-0.12,0,0,0};
char moji_size_list[8][12] = {"\\scriptsize","\\small","\\normalsize","\\large","\\Large","\\LARGE","\\huge","\\Huge"};
unsigned char moji_size_kan[8][16] = {"より小","小","普通","やや大","より大","大","特大","最大"};
unsigned char kundoku_mode[6][16] = {"すべて","一部読み","返点のみ","仮名のみ","なし",""};
unsigned char label_form_index[10][16] = {"数字のみ","まる数字","英字小のみ","英字大のみ","括弧数字","括弧英字小","アイウ","丸アイウ"};
char label_list[10][17] = {"\\nsujitoi","\\marusujitoi","\\eimojitoi","\\Eimojitoi","\\kakkosujitoi","\\kakkoeijitoi","\\kaiutoi","","",""};
unsigned char style_index[15][16] = {"表題","大問","小問","選択縦並び","選択横並び","選択括弧付","段組","改行幅全体","改行幅一行","文字空け","下詰め","","","",""};
int     style_index_num[15] ={6,6,6,15,15,15,6,15,15,12,9,1,1,1,1};
unsigned char enum_form_index[10][16] = {"アイウ","丸数字","丸アイウ","漢数字","イロハ","","","","",""};
int     enum_form_num[10] ={9,9,12,9,9,0,0,0,0,0};
unsigned char enum_myitem_label[10][17] = {"\\myitemaiu","\\myitemmsuji","\\myitemmaiu","\\myitemksuji","\\myitemiroha","","","","",""};
unsigned char enum_item_label[10][7] = {"[kk","[ms","[mk","[ks","[ki","","","","",""};
unsigned char enum_shomon_index[5][16] = {"問一","問１","問1","",""};
unsigned char enum_shomon_label[5][31] = {"[label=問\\kansuji*]","[zs]","[label=問\\rensuji{\\arabic*}]","",""};

char moji_unit_list[4][4] = {"zw","\\zw","zh","\\zh"};
char kan_head_list[10][16] = {"\\KAN","\\CKAN","\\BKAN","\\BBKAN","\\NBKAN","\\BNBKAN","\\NKAN","\\BNKAN","",""};
char h_skip[2][7] = {"\\kern","\\hskip"};
char v_skip[7] ={"\\vskip"};
char space_con[] = {"spacing"};
char zatu_con[8][5] = {"\\CL","\\NG","","","","","",""};
char env_name[10][16] = {"spacing","description","enumerate","itemize","multicols","quote","minipage","tabular","",""};
//      0           1           2           3          4          5       6           7
unsigned char init_com_list[10][16] = {"KUNSTYLE","KANSIZE","LFWIDTH","LINEADJUST","STANDARDLABEL","KANHEAD","ENVCOMM","LABELFORM","NAKABOU","NAKAGURO"};
unsigned char init_mondai_list[10][16] = {"TITLESIZE","SUBTITLE","DAIMONSIZE","DAIMONPOS","MONDAISEZE","SHOMONFORM","ITEM_LABEL","","",""};
char head_index[6] = {"xbdwq"};
char label_index[21] = {"nsmsnenEkskekkhk"};
char moji_size_index[9] = {"tsnmlLhH"};
char line_adjust_com[15];
int  label_kind_index;
double  st_moji_width = 1.7;
double  st_adjust_dim;

enum_com*    init_enum(int index, unsigned char* line);

#endif /* my_tex_command_h */
