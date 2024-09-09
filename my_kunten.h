//
//  my_kunten.h
//  kunten
//  これは、kunten.cにのみインクルードされる
//  Created by 岡崎康浩 on 2023/06/07.
//

#ifndef my_kunten_h
#define my_kunten_h
#define HAKU_BUF    1024
#define KUN_BUF     2048

unsigned char char_j[5];
unsigned char temp_comment_buf[19];
unsigned char temp_kun_char[5];
unsigned char temp_kan_char[5];
unsigned char temp_str[128];
unsigned char input_buf_haku[HAKU_BUF];
unsigned char input_buf_kun[KUN_BUF];
unsigned char temp_buf_bp[KUN_BUF];
unsigned char temp_char_kan[5];
unsigned char temp_char_kun[5];
int           kan_haku_flag;
int           read_num;
int           file_end_flag;
int           strata;
//int         read_flag;
int           start[7];
int           count_n[7];
int           line_property;
int           cont_flag;
int           cont_kun_flag;
int           yomi;
int           okuri;
int           kana_flag;
int           add;
int           kakko_kun_u;
int           kakko_u;
int           chofuku_n;
int           err_count;
int           non_done;
int           hogo_rev;
int           error_count,temp_error_point;
int           basic_error;
int           non_sai_okiji_count;



const unsigned char ichini[30][7] = {"一","二","三","四","五","六","七","八","九","十","十一","十二","十三","十四","十五","十六","十七","十八","十九","廿","廿一","廿二","廿三","廿四","廿五","廿六","廿七","廿八","廿九","丗"};
const unsigned char jouchu[3][4] = {"上","中","下"};
const unsigned char jouge[2][4] = {"上","下"};
const unsigned char kouotu[10][4] = {"甲","乙","丙","丁","戊","己","庚","辛","壬","癸"};
const unsigned char tenti[3][4] = {"天","地","人"};
const unsigned char tozai[4][4] = {"東","西","南","北"};
const unsigned char a_dan_s[10][4] = {"カ","ガ","サ","タ","ナ","ハ","バ","マ","ラ",""};
const unsigned char u_dan_s[14][4] = {"ク","グ","ス","ズ","ツ","ヅ","ヌ","フ","ブ","ム","ユ","ル","ウ",""};
const unsigned char u_dan_ss[22][4] = {"ル","ク","グ","ス","ズ","ツ","シ","ヅ","ヌ","フ","ブ","ム","キ","リ","ン","ユ","ウ","ヨ","ヤ","カ","ナ",""};//部分的に文終止を受けるので、ウ段ではないものも含んでいる。
const unsigned char min_u_dan_s[8][4] = {"ル","ク","グ","ス","ツ","フ","ブ",""};
const unsigned char rentai[15][4] = {"ル","ク","グ","ス","ズ","ツ","ヅ","ヌ","フ","ブ","ム","ウ","キ","シ",""};
const unsigned char e_dan_s[15][4] = {"ケ","ゲ","セ","ゼ","テ","デ","ネ","メ","カ","ヱ","ベ","エ","レ","ヘ",""};
const unsigned char i_dan_s[13][4] = {"シ","キ","チ","ヂ","ヒ","ミ","リ","イ","ジ","ギ","ビ","ニ",""};
const unsigned char ra_gyo_s[5][4] = {"ラ","リ","ル","レ",""};
const unsigned char noga[3][4] = {"ノ","ガ",""};
const unsigned char keiyo[5][4] = {"シ","ク","キ","ケ",""};
const unsigned char tokoro[3][4] = {"所","可",""};
const unsigned char kore[4] = {"之"};
const unsigned char hogo_josi[6][7] = {"ヲ","ニ","ヘ","ト","ヨリ",""};
const unsigned char hukujosi[4][7] = {"ハ","ノミ","モ",""};
const unsigned char shukaku[5][4] = {"ガ","ハ","モ","ゾ","ノ"};
const unsigned char sahen[7][4] = {"セ","シ","ス","ゼ","ジ","ズ",""};
const unsigned char to_sahen[4][7] = {"トセ","トシ","トス",""};
const unsigned char full_sahen[7][4] = {"セ","シ","ス","ゼ","ジ","ズ",""};
const unsigned char jodo[6][4] = {"ン","ム","ズ","ル","シ",""};
const unsigned char odan_josi_rentai[10][10] = {"コトヲ","モノヲ","コト","モノ","トモ","ト","ゾ","モ","ヲ","ノ"};
const unsigned char odan_josi_izen[3][7] = {"ドモ","ド","ト"};
const unsigned char te_odan_josi[3][7] = {"テモ","テゾ",""};
const unsigned char ka_gyo[5][4] = {"キ","ク","ケ","カ",""};
const unsigned char non_hendoku_suffix[14][4] = {"ア","オ","コ","ゴ","ソ","ゾ","ト","ド","ノ","ホ","モ","ロ","ヲ",""};
const unsigned char hitei_yomi[6][7] = {"ザラ","ザリ","ザル","ザレ","ズン","ズ"};
const char kun_mode[6] = {"fpkon"};//0=full 1=partial 2=kaeriten 3= okurigana-nomi 4=nasi
const char moji_size[6] = {"mlLhH"};//m=large,l=Large,L=LARGE,h=huge,H=Huge
const char line_feed_w[2][3] = {"W=","w="};
const char mode_char[] = {"fpkontmlLhHWw=0123456789."};


unsigned char   kana_temp_char[4];
oyakan*         kan_index[MAX_MOJI];
kaeriten*       kaeri_index[MAX_MOJI];

#endif /* my_kunten_h */
