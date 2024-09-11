//
//  kunten.c
//  kunten
//
//  Created by 岡崎康浩 on 2023/04/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include "oyakan.h"
#include "kunten.h"
#include "utfstring.h"
#include "tex_command.h"
#include "my_math.h"
#include "my_kunten.h"

//#include "kunten_main.h"
int           kan_length;
int           process_error;
int           loop_error;
mojimode      haku_mojimode;
char          mode_buf[10];
int           read_haku_moji_mode( int n );
double        read_haku_moji_mode_width( void );
int           siyeki_on;
int           num_of_kaisi;

void    add_read_num( void )
{
    read_num++;
}

int     read_non_sai_okiji(void)
{
    return non_sai_okiji_count;
}


void kaeri_print(void)
{
    int     i;
    i = 0;
    while(kan_index[i]!= NULL)
    {
        printf("番号%02d:%s:訓順＝%d: 返点用 %02d:%02d:%02d:%02d:熟語 %d:返点: %s \n",i,kan_index[i]->oya,kan_index[i]->kun_jun,kaeri_index[i]->code[0],kaeri_index[i]->code[1],kaeri_index[i]->code[2],kaeri_index[i]->code[3],kaeri_index[i]->juku, kan_index[i]->kaeri_ten);
        i++;
    }
}
#ifdef DEBUG
int     debug_line;
int     every = 1;
int     debug_stop( int mark)
{
    if(mark == -1)return debug_line;
    debug_line = mark;
    return debug_line;
}
int     read_every( void )
{
    return every;
}


#endif
/*
 init_write_haku_moji_modeは、texcommand.iniからhaku_mojimode（漢文の書式を入れた構造体）
 構造体に数値を入れるためのもの、使用上各数値に負数を入れると、そこには数値は入らない。
 */
int     init_write_haku_moji_mode(int kun, int size, double width)
{
    if(kun > -1) {
        haku_mojimode.kun_mode = kun;
        return 1;
    }
    if(size > -1){
        haku_mojimode.moji_size = size;
        return 2;
    }
    if(width > 0){
        haku_mojimode.lf_width = width;
        return 3;
    }
    return 0;
}
int     read_haku_moji_mode( int n )
{
    switch(n){
        case 1: return haku_mojimode.kun_mode;
            break;
        case 2: return haku_mojimode.moji_size;
            break;
        default:
            return 0;
    }
}
double  read_haku_moji_mode_width( void )
{
    return haku_mojimode.lf_width;
}

int     write_haku_moji_mode(FILE* i_file, unsigned char* temp)
{
    int     read_flag,read_count;
    char    t_char;
    char*   t_bp;
    char*   ret_bp;
    double   t_width;
    read_flag = 1;
    read_count = 0;
    temp = get_one_char(i_file,temp);
    if(temp == NULL){
        file_end_flag = 1;
        return -1;
    }
    if(*temp != '<' && is_utf_angle(temp) != 2){
        if(is_utf_char(temp)== 34) kakko_u++;
        return one_character(*temp);
    }
    if(*temp > 0x80 && !is_utf_3b_ascii(temp)) return one_character(*temp);
    t_char = b3_to_ascii(temp);
    t_bp = mode_buf;
    while(t_char != '>'){
        temp = get_one_char(i_file, temp);
        if(*temp > 0x80 && !is_utf_3b_ascii(temp)){
            read_flag = 0;
            break;
        }
        t_char = b3_to_ascii(temp);
        if(strchr(mode_char,(int)t_char)== NULL){
            read_flag = 0;
            temp[0] = t_char;
            temp[1] = '\0';
            break;
        }
        *t_bp = t_char;
        t_bp++;
        read_count++;
    }
    *t_bp = '\0';
    if((t_bp = strchr(mode_buf,(int)'>'))!= NULL) {
        *t_bp = '\0';
        read_count = read_count - 1;
    }
    if(read_count > 0){
        t_bp = mode_buf;
        ret_bp = strstrchr((char*)kun_mode,t_bp);
        if(ret_bp != NULL){
            t_bp = (char*)kun_mode;
            haku_mojimode.kun_mode = (int)(ret_bp - t_bp);
        }
        t_bp = mode_buf;
        ret_bp = strstrchr((char*)moji_size,t_bp);
        if(ret_bp != NULL){
            t_bp = (char*)moji_size;
            haku_mojimode.moji_size = (int)(ret_bp - t_bp + 3);
        }
        ret_bp = strstr(mode_buf,line_feed_w[0]);
        if(ret_bp != NULL){
            sscanf(ret_bp, "W=%lf", &t_width);
            haku_mojimode.lf_width = t_width;
        }
        ret_bp = strstr(mode_buf,line_feed_w[1]);
        if(ret_bp != NULL){
            sscanf(ret_bp, "w=%lf", &t_width);
            haku_mojimode.lf_width = t_width;
        }
        return read_flag == 0? one_character(*temp) : 0;
    }
    return read_flag == 0? one_character(*temp) : 0;
}

int     get_kan_haku_flag( void )
{
    return kan_haku_flag;
}
void    write_kan_haku_flag( int num)
{
    kan_haku_flag = num;
}
void    write_read_num( int num ){
    read_num = num;
}
int     read_read_num( void )
{
    return read_num;
}
void    write_file_end_flag( int num )
{
    file_end_flag = num;
}
int  get_file_end_flag ( void )
{
    return file_end_flag;
}
void  init_kakko_u( void )
{
    kakko_u = 0;
}
unsigned char* get_huku_josi(int i)
{
    return (unsigned char*)hukujosi[i];
}
/*
 これは、kan_indexのoyakan構造体を引き渡すための関数です。引数はインデックスナンバーindex,
 戻り値はoyakan構造体です。
 */
oyakan* get_kan_index(int index)
{
    if(index < 0 || index > MAX_MOJI - 1) return NULL;
    return kan_index[index];
}
kaeriten* get_kaeri_index(int index)
{
    if(index < 0 || index > MAX_MOJI - 1) return NULL;
    return kaeri_index[index];
}
/*
 swap_index_kan_index(pair* swap) swapに示されたkan_indexの内容を交換する。
 typedef struct oyakan{
     unsigned char       oya[5];
     unsigned char       yomi[36];
     unsigned char       okuri[61];
     unsigned char       kaeri[36];
     unsigned char       kaeri_okuri[25];
     unsigned char       kaeri_ten[7];
     struct comment*     label;
     int                 okuri_n;
     int                 del_okuri;
     int                 kaeri_okuri_n;
     struct kanlist*     prop_p[9];
     long int                 prop;
     int                 is_dosi;
     char                mod;
     int                 b_end;
     int                 interval;
     int                 kunten_mode;
     int                 gtest;
     long int                 jufuku;
     struct kanaindex*   sai_yomi;
     int                 kun_jun;
     int                 sai;
 } oyakan;
 */

int    swap_kan_index(int id_1, int id_2)
{
    unsigned char       yomi[36];
    unsigned char       okuri[61];
    int                 okuri_n, del_okuri,kun_jun;
    if(id_1 < 0 || id_2 < 0) return -1;
    if(kan_index[id_1] == NULL || kan_index[id_2] == NULL) return -1;
    strcpy((char*)yomi, (char*)kan_index[id_1]->yomi);
    strcpy((char*)okuri, (char*)kan_index[id_1]->okuri);
    okuri_n = kan_index[id_1]->okuri_n;
    del_okuri = kan_index[id_1]->del_okuri;
    kun_jun = kan_index[id_1]->kun_jun;
    strcpy((char*)kan_index[id_1]->yomi, (char*)kan_index[id_2]->yomi);
    strcpy((char*)kan_index[id_1]->okuri, (char*)kan_index[id_2]->okuri);
    kan_index[id_1]->okuri_n = kan_index[id_2]->okuri_n;
    kan_index[id_1]->del_okuri = kan_index[id_2]->del_okuri;
    kan_index[id_1]->kun_jun = kan_index[id_2]->kun_jun;
    strcpy((char*)kan_index[id_2]->yomi, (char*)yomi);
    strcpy((char*)kan_index[id_2]->okuri, (char*)okuri);
    kan_index[id_2]->okuri_n = okuri_n;
    kan_index[id_2]->del_okuri = del_okuri;
    kan_index[id_2]->kun_jun = kun_jun;
    return id_1;
}
/*
 find_index_kanjiは、unsigned char* kan_bpの先頭にある漢字をkan_indexの中から探す。ただ探すだけで、何の条件もない。戻り値は、kan_index上のindex。探せなかったり、入力のkan_bpに問題があれば、-1を返す。引数のstartは、どの位置から探すかを指定する。０を指定すれば、最初から探す。
 */
int     find_kan_index(unsigned char* kan_str, int start)
{
    int   ret, index;
    unsigned char   temp_kanji[5];
    ret = is_utf_kanji(kan_str);
    if(ret < 3) return -1;
    strncpy((char*)temp_kanji, (char*)kan_str,ret);
    temp_kanji[ret] = '\0';
    index = start;
    while(kan_index[index]!=NULL){
        ret = kanji_cmp(kan_index[index]->oya, temp_kanji);
        if(ret == 0) return index;
        index++;
    }
    return -1;
}
/*
 kan_index[]上のindexで示されたoyakan構造体のoyaと、unsigned charのポインタの先頭漢字が一致するかどうかを調べる。一致すれば０、そうでなければ１を返す。
 */
int     kan_index_kan_identify(int index, unsigned char* kan_bp)
{
    int     ret, code;
    if(kan_index[index] == NULL) return 1;
    code = is_utf_kanji(kan_index[index]->oya);
    ret = strncmp((char*)kan_bp,(char*)(kan_index[index]->oya),code);
    return (ret == 0 ? 0 : 1);
}

/*
 訓読文から判断して、kan_index[i]とkan_index[i+1]が熟語になっているかどうかを判断する。引数は、訓読文のポインタ。
 */
/*already_readは、indexまでの漢字が全て読めているかどうかを確認する。全て読めていれば0、そうでなければ1が返される。*/
int     already_read( int index )
{
    int     i,ret;
    i = 0;
    //count = 0;
    ret = find_double_kanji(index, 1);
    if(ret > 0) return 1;
    while( i < index )
    {
        if(kan_index[i]->kun_jun == 0 && PID(kan_index[i]->prop, OKI_KANA) != 0) return 1;
        i++;
    }
    return 0;
}
/*
 find_double_kanjiは、重複している漢字を探す。現在のkan_index上のインデックスナンバー、戻り値はその漢字と重複している漢字の
 kan_index上のインデックスナンバー、できるだけ直近の重複漢字を探すため、後方の場合1, 前方の場合-1とする。
 また、markに0を入れると、文頭から検索する。
 重複している漢字がない場合は、-1を返す。
 */
int     find_double_kanji( int index, int mark)
{
    int         i, p;
    long int    q;
    q = kan_index[index]->jufuku;
    p = prime_simple_factorization(q, 2, MAX_PRIME);
    p = prime_num(p, MAX_PRIME,1);
    if(mark == 0){
        index = -1;
        mark = 1;
    }
        i = index + mark;
    while(i > -1 && kan_index[i] != NULL)
    {
        if(kan_index[i]->jufuku > 1 && kan_index[i]->kun_jun == 0 && kan_index[i]->gtest != -1){
            if(kan_index[i]->jufuku % p == 0) return i;
        }
        i = i + mark;
    }
    return -1;
}
/*
 int    is_dosi_conji(unsigned char* okuri) は、動詞に接続助詞がついたものかどうかを判定する。
 ただ、次のis_non_hendoku関数との組み合わせのみを考えたもので、接続助詞がオ段で終わるものだけに限っている。
 */
int    is_dosi_conj(unsigned char* okuri)
{
    int             len, len2,ret,i,j,p;
    unsigned char*   temp_str;
    len = (int)strlen((char*)okuri);
    i = 0;
    while(*te_odan_josi[i] != '\0'){
        ret = b3_strrcmp(okuri,(unsigned char*)te_odan_josi[i]);
        if(ret == 0) return 0;
        i++;
    }
    for(i = 0; i < 10; i++){
        ret = b3_strrcmp(okuri,(unsigned char*)odan_josi_rentai[i]);
        if(ret == 0){
            len2 = (int)strlen((char*)odan_josi_rentai[i]);
            temp_str = (len - len2 - 3) + okuri;
            j = 0;
            while(*u_dan_ss[j]!= '\0'){
                ret = b3_strlcmp(temp_str,(unsigned char*)u_dan_ss[j]);
                if(ret == 0) return 0;
                j++;
            }
        }
    }
    p = 0;
    while(p < 3){
        ret = b3_strrcmp(okuri,(unsigned char*)odan_josi_izen[p]);
        if(ret == 0){
            len2 = (int)strlen((char*)odan_josi_izen[p]);
            temp_str = (len - len2 - 3) + okuri;
            j = 0;
            while(*e_dan_s[j]!= '\0'){
                ret = b3_strlcmp(temp_str,(unsigned char*)e_dan_s[j]);
                if(ret == 0) return 0;
                j++;
            }
        }
        p++;
    }
    return 1;
}
/*
 is_non_hendokuは、補語をとる動詞（ないしは動詞もどき）でないことを判定する関数（ただし、動詞であることを確定する関数ではない）。引数は、当該の漢字とその送り仮名。動詞でないと判定できれば０、そうでなければ１を返す。
 */
int     is_non_hendoku(int index, unsigned char* okuri)
{
    int     ret, i;
    if(strcmp((char*)okuri,(char*)"ガ") == 0) return 0;
    if(strcmp((char*)okuri,(char*)"ノ") == 0) return 0;
    if(strcmp((char*)okuri,(char*)"ハ") == 0) return 0;
    if(strcmp((char*)okuri,(char*)"ヲ") == 0) return 0;
    if(strcmp((char*)okuri,(char*)"ニ") == 0) return 0;
    if(strcmp((char*)okuri,(char*)"ト") == 0) return 0;
    if(strcmp((char*)okuri,(char*)"ヘ") == 0) return 0;
    if(strcmp((char*)okuri,(char*)"ヨリ") == 0) return 0;
    if(b3_strrcmp(okuri, (unsigned char*)"ヲシテ")==0)
    return 0;
    for(i=0;i < 3;i++){
        if(strncmp((char*)okuri, (char*)to_sahen[i],6) == 0)return 1;
    }
    ret = is_dosi_conj(okuri);//
    if(ret == 0) return 1;
    for(i=0;i < 15;i++){
        if(b3_strrcmp(okuri,(unsigned char*)non_hendoku_suffix[i]) == 0) return 0;
    }
    return 1;
}
/*
 次の関数は、全ての重複が尽きているかどうかを確認する。全ての重複漢字が尽きていれば、０を返す。そうでなければ１以上を返す。
 その数は、現時点で読まれていない重複漢字の数を示す。したがって、１が返されば、indexの漢字以外に１つある意味、引数は、
 現在のindex。
 */
int     is_exhausted_dup_kan( int index )
{
    int    ret, temp_index,count;
    count = 0;
    temp_index = index;
    ret = 0;
    while( ret != -1){
        ret = find_double_kanji(temp_index,1);
        if(temp_index == index) count--;
        temp_index = ret;
        count++;
    }
    temp_index = index;
    while( ret != -1)
    {
        ret = find_double_kanji(temp_index,-1);
        if(temp_index == index) count--;
        temp_index = ret;
        count++;
    }
    return count;
}

/*
 訓読文ipの中からかな送り仮名部分を除き次の漢字のポインタを返す。
 */
unsigned char* skip_kana( unsigned char* ip)
{
    while(is_utf_kata(ip) || is_utf_hira(ip)){
        ip = ip+3;
    }
    return ip;
}
/*
 使役の助動詞として読むべき漢字かどうかを判定する。無関係である場合も含めて0を返す。明らかに使役の助動詞として読まないときのみ１を返す。引数は問題となる漢字へのkan_index上へのインデックスと送り仮名。
 */
int     is_siyeki_on(int index, unsigned char* okuri)
{
    int temp_i, alt_i;
    alt_i = -1;
    if(PID(kan_index[index]->prop, SIYEKI) != 0) return 0;
    if(b3_strrcmp(okuri, (unsigned char*)"ヲシテ")==0){
        if(kan_index[index]->jufuku > 1){
            alt_i = find_double_kanji(index, 1);
        }
        temp_i = index - 1;
        while(temp_i > -1){
            if(PID(kan_index[temp_i]->prop, SIYEKI) == 0){
                if(alt_i > -1){
                    kan_index[index]->kun_jun = -2;
                }
                return 0;
            }
            temp_i--;
        }
        return 1;
    }
    return 0;
}
/*
 「所」などの名詞を形成する特殊な助字を判別する。引数はkan_index上の置かれている位置の番号、妥当すれば、0
 そうでなければ、１を返す。
 */
int     is_tokoro( int index)
{
    int     i;
    for(i=0; i< 2; i++)
    {
        if(!kanji_cmp(kan_index[index]->oya,(unsigned char*)tokoro[i])) return 0;
    }
    return 1;
}
/*
順読のひらがなになっている助字が適正な位置にあるかを判定するための補助、問題となる助字の後に来るもので、最も読み順が小さいものを探す。その漢字のインデックスを返す。なお、後がない場合、120を返す。前がない場合は、−1を返す。
 */
int     post_joji( int  index)
{
    int     post_jun, i,post_pos, temp_jun;
    if(index == 0) return -1;
    i = index + 1;
    while(kan_index[i]!= NULL){
        if(kan_index[i]->kun_jun > 0) break;
        i = i + 1;
    }
    if(kan_index[i] == NULL){
        i = 0;
        post_jun = 1;
        post_pos = 0;
        while(kan_index[i]!= NULL){
            temp_jun = kan_index[i]->kun_jun;
            if(post_jun == 1 && temp_jun == 1) post_pos = i;
            if(post_jun < temp_jun){
                post_jun = temp_jun;
                post_pos = i;
            }
            i++;
        }
        return post_pos;
    }
    post_pos = i;
    post_jun = kan_index[i]->kun_jun;
    while(i > -1 && kan_index[i] != NULL)
    {
        if(i < index ) break;
        post_jun = kan_index[i]->kun_jun;
        i = preceeding_kan(post_jun,1);
    }
    return i;
}
/* 「之」は、構造助字として使われ「ノ」と読むか、置き字となる。その場合、訓読文には漢字は現れない。したがって、
 「之」が構造助字かどうかを文脈から判定する。構造助字と判定されれば0、でなければ1、判定不能であれば2を返す。
 また、なんらかのエラーが生じれば、9を返す。*/
int     is_kore_joji( int index, kanokuri* temp_kan_kore,int kun_jun )
{
    int     temp_kun_jun, temp_kun_jun_pre;
    int     temp_index, temp_index_pre, ret;
    kanlist* temp_list;
    
    if(PID(kan_index[index]->prop, KORE) != 0) return 1;
    if(kan_index[index]->kun_jun == -2) return 0;
    temp_kun_jun_pre = immediate_kun(index, -1);
    temp_kun_jun = immediate_kun(index, 1);
    if(temp_kun_jun_pre == 0) return 1;
    if(temp_kun_jun == 0) return 1;
    if(temp_kun_jun < temp_kun_jun_pre) return 1;
//  The following four lines are hindering correct kutou.
//    if(temp_kun_jun < kun_jun - 1) {
//        kan_index[index]->kun_jun = -2;
//        return 0;
//    }//
    temp_index_pre = kunjun_around_index(index, temp_kun_jun, -1);
    temp_index = kunjun_around_index(index, temp_kun_jun_pre, 1);
    
    if(temp_index_pre < 0 && temp_index < 0)return 1;
    if(kan_index[temp_index_pre]->kun_jun+1 != kan_index[temp_index]->kun_jun) return 1;
    temp_list = identify_kanlist(kan_index[index]->oya,find_root_okikana());
    if(temp_list == NULL) return 1;
    ret = kanlist_confirm(temp_list, kan_index[temp_index_pre]->okuri, 3);
    if( ret == 0) {
        kan_index[index]->kun_jun = -2;
        return 0;
    }
    return 1;
}
int is_kore_joji_simple( int index )
{
    int     temp_i;
    if(PID(kan_index[index]->prop, KORE))return index;
    temp_i = find_double_kanji(index, 1);
    if(temp_i > index && kan_index[temp_i]!= NULL) return temp_i;
    return index;
}
/*
hendoku_property_identifyは、kan_index上の漢字AとBが返読の関係にあるかを判定する。引数はkan_index上のインデックスナンバー
 と送り仮名、戻り値は、特殊な返読文字の性質にあたるEXISTなどの定数。どれにも当たらなければ、GENERAL(つまり、すべての
 各助詞に当てはまるものを）を返す。
 #define EXIST       30// 補語が、助詞なし、連体形、・・「こと」「もの」E (助詞なし２、連体形３、「こと」「モノ」５
 #define COPULA      77// 名詞＋ノ、連体形＋ガ C　・・名詞＋ノ　７　連体形＋ガ　11、
 #define PROVERB     26//　連用形（イ段、エ段、接尾辞なし）A　連用形 13
 #define SPECIAL1    15//　連体形、連体形＋コト S
 #define SPECIAL2    3// 連体形 T
 */
int hendoku_property_identify(int index, unsigned char* kana, unsigned char* bp)
{
    int             ret, t_index, ret_attrib;
    long int        q;
    kanokuri*       temp_kanokuri;
    kanokuri_index* temp_kanokuri_index;
    kanlist*        temp_list;
    unsigned char*  temp_kana;
//    unsigned char*  temp_bp;
    q = kan_index[index]->prop;
    ret_attrib = 1;
    t_index = index;
    if(!is_tokoro( index )) return RENTAI;
    if(PID(q,HENDOKU) == 0){
        temp_list = (kanlist*)kan_index[index]->prop_p[1];
        if(temp_list != NULL){
            ret = kanlist_confirm( temp_list, kana, 2);
            if(ret == 0) {
                ret_attrib = ret_attrib * temp_list->attrib;
            }
        }
    }
    if(PID(q, HENDOKU2) == 0){
        temp_list = (kanlist*)kan_index[index]->prop_p[5];
        if(temp_list != NULL){
            ret = kanlist_confirm( temp_list, kana, 2);
            if(ret == 0) {
                ret_attrib = ret_attrib * temp_list->attrib;
            }
        }
    }
    if(PID(q , HENDOKU3) == 0){
        temp_list = (kanlist*)kan_index[index]->prop_p[6];
        if(temp_list != NULL){
            ret = kanlist_confirm( temp_list, kana, 2);
            if(ret == 0) {
                ret_attrib = ret_attrib * temp_list->attrib;
            }
        }
    }
    if(PID(q, HENDOKU4) == 0){
        temp_list = (kanlist*)kan_index[index]->prop_p[7];
        if(temp_list != NULL){
            ret = kanlist_confirm( temp_list, kana, 2);
            if(ret == 0) {
                ret_attrib = ret_attrib * temp_list->attrib;
            }
        }
    }
    if(PID(q, HENDOKU5) == 0){
        temp_list = (kanlist*)kan_index[index]->prop_p[8];
        if(temp_list != NULL){
            ret = kanlist_confirm( temp_list, kana, 2);
            if(ret == 0) {
                ret_attrib = ret_attrib * temp_list->attrib;
            }
        }
    }
    if(ret_attrib > 1) return ret_attrib;
    if(PID(q, NONHENDOKU) == 0){
        temp_list = (kanlist*)kan_index[index]->prop_p[4];
        if(temp_list != NULL){
            ret = kanlist_confirm(temp_list, kana, 3);
            if(ret == 0) return 199;
        }
    }
    if(*kana == '\0'){
        temp_kana = bp;
 //       temp_bp = temp_buf_bp;
        temp_kanokuri_index = trace_juku(index, temp_kana);
        temp_kanokuri = temp_kanokuri_index->p_kanokuri;
        ret = is_non_hendoku(t_index,temp_kanokuri->okurigana);
        free(temp_kanokuri);
        free(temp_kanokuri_index);
    }
    else{
        ret = is_non_hendoku(index,kana);
    }
    return((ret==0)? 199: GENERAL);
}
/*
 daisi_identifyは、代詞と見られる漢字の現在のkan_index上の位置と、訓読文の文字列から、それが
 代詞かどうかを判定する。代詞と判定されれば0、そうでなければ1を返す。
 */
int     daisi_identify(int  index )
{
    int         hitei_i;
    if(PID(kan_index[index]->prop, DAISI) != 0) return 1;
    hitei_i = index - 1;
    while(hitei_i > -1)
    {
        if(PID(kan_index[hitei_i]->prop, HITEI) == 0 ) {
            if(kan_index[hitei_i]->kun_jun == 0 || kanji_cmp(kan_index[hitei_i]->oya, (unsigned char*)"未") == 0)
            return 0;
        }
        hitei_i--;
    }
    return 1;
}
/*
 副詞を確定させる hukusi_property_identifyは、kan_index上の漢字Aが副詞かどうかを確定させる。引数は、
 hendoku_property_identifyとおなじであるが、送り仮名は完全一致で、戻り値は妥当な場合当該の性質を示す数、そうでない場合１
 */
int hukusi_property_identify(int index, unsigned char* kana)
{
    int             ret;
    long int        q;
    kanlist*        temp_list;
    q = kan_index[index]->prop;
    if(PID(q,HUKUSI) != 0) return 0;
    temp_list = (kanlist*)kan_index[index]->prop_p[0];
    ret = kanlist_confirm( temp_list, kana, 1);
    if(ret == 1) {
        if(kan_index[index]->jufuku == 0){
            q = -q;
            q = q^HUKUSI;
//            printf("kan_index[%d]->prop = %ld \n", index, -q);
            kan_index[index]->prop = -q;
        }
        return 0;
    }
    return temp_list->attrib;
}
/*
 int    is_conj_josi( int index, int len, unsigned char* okuri )
 indexで示される漢字が動詞扱いになっているかを探るindexは対象の漢字を特定するインデックス。
 lenは、対象となる助詞の長さ、okuriは対象となる感じの送り仮名。接続助詞と判断されれば０、単なる
 格助詞であると判断されれば１を返す。modeは１を指定すると連体形、２を指定すると連体形または文末。
 例えば、「と」が受けるような形となる。
 */
int    is_conj_josi( int index, unsigned char* josi, unsigned char* okuri,int mode )
{
    int               len1, len2,i;
    unsigned char*    temp_str;
    len1 = (int)strlen((char*)josi);
    len2 = (int)strlen((char*)okuri);
    if(len1 == len2) return 1;
    if(kan_index[index]->is_dosi > 0) return 0;
    temp_str = okuri;
    temp_str = temp_str + (len2 - len1 - 3);
    i = 0;
    switch(mode){
        case 2:
            while(*rentai[i] != '\0')
            {
                if(!strncmp((char*)temp_str,(char*)u_dan_ss[i],3)) return 0;
                i++;
            }
            break;
        case 1:
        default:
            while(*rentai[i] != '\0')
            {
                if(!strncmp((char*)temp_str,(char*)rentai[i],3)) return 0;
                i++;
            }
    }
    return 1;
}
/*
int mode_judge(int mode)は、
 */
int     mode_judge( int mode )
{
    int     temp_i, temp_p;
    if(mode < 6 ) return mode;
    if(mode == 199) return 19;
    if(mode % RENYO == 0) return 5;
    temp_i = 1;
    temp_p = 1;
    while(temp_i > 0){
        temp_i = prime_simple_factorization((long int)mode, temp_i, 42);
        if(temp_i < 0 )break;
        temp_p = prime_num(temp_i,42,0);
        temp_i++;
    }
    if(temp_p > 22) return 2;
    return 3;
}
 
/*
 返読の対象となるような、補語かどうかを確定する。引数は送り仮名とKan_index上のその漢字の位置、戻り値は対象となる語の性格、
 #define MEISI       2// 助詞なし。
 #define RENTAI      3// 連体形
 #define KOTO        5//「コト」「モノ」
 #define POSTNO      7// 名詞＋ノ
 #define RENTAIGA    11// 連体形＋ガ
 #define RENYO       13// 連用形
 #define RENTAIHAMO  17// 連体形＋ハ／モ
 #define POSTNI      23//  N
 #define POSTTO      29//  T
 #define POSTYORI    31//  Y
 #define POSTWO      37// O
 #define POSTHE      41
 {"ヲ","ニ","ヘ","ト","ヨリ",""}
 どれにも該当しなければ、１を返す。
 これは、全てに対応するようにするには、modeに30、助詞だけでよければ２、存在詞、同動詞の特殊な返読文字に対応するのでよければ３、
 「難」「易」などの連用形を受けるものだけでよければ、５を入れる。その対象とするものを組み合わせる場合には、それぞれの項目をかけ
 合わせた数を入れる。
 */
int     hogo_property( int index, unsigned char* okuri_str, int mode)
{
    int i, temp_prop, ret, ori_mode;
    temp_prop = 1;
    ori_mode = mode;
    mode = mode_judge( mode );
    switch(mode){
        case 2:
            if(index > -1){
                temp_prop = hukusi_property_identify(index, okuri_str);
                if(temp_prop > 0) return 109;
            }
            if(!b3_strrcmp(okuri_str,(unsigned char*)"コト")) return KOTO;
            for(i = 0;i<5;i++){
                if(!b3_strrcmp(okuri_str,(unsigned char*)hogo_josi[i])){
                    switch(i){
                        case 0: 
                            if(non_done == 2){
                           // if(non_done > 0 && (prime_num(non_done + 2,MAX_PRIME,0) * hogo_rev) % 5 == 0){
                                ret = is_conj_josi(index, (unsigned char*)hogo_josi[0], okuri_str,1);
                                return ret == 0 ? 109 : POSTWO;
                            }
                            hogo_rev = hogo_rev * 5;
                            return POSTWO;
                            break;
                        case 1:
                            if(non_done == 1){
                          //  if(non_done > 0 && (prime_num(non_done + 2,MAX_PRIME,0) * hogo_rev) % 7 == 0){
                                ret = is_conj_josi(index, (unsigned char*)hogo_josi[1], okuri_str,1);
                                return ret == 0 ? 109 : POSTNI;
                            }
                            hogo_rev = hogo_rev * 7;
                            return POSTNI;
                            break;
                        case 2: 
                            if(kan_index[index]->is_dosi > 0) return 109;
                            return POSTHE;
                            break;
                        case 3:
                            if(non_done == 3){
                            //if(non_done > 0 && (prime_num(non_done + 2,MAX_PRIME,0) * hogo_rev) % 11 == 0){
                                ret = is_conj_josi(index, (unsigned char*)hogo_josi[3], okuri_str,2);
                                return ret == 0 ? 109 : POSTTO;
                            }
                            hogo_rev = hogo_rev * 11;
                            return POSTTO;
                            break;
                        case 4: return POSTYORI;
                            break;
                    }
                }
            }
            if(ori_mode % 3 != 0 && ori_mode % 5 != 0) return 2;
        case 3:
            if(*okuri_str == '\0') return MEISI;
            if(!abb_included_cmp(okuri_str,(unsigned char*)"",'m',(unsigned char*)"",3)) return RENTAI;
            if(!b3_strrcmp(okuri_str,(unsigned char*)"コト")) return KOTO;
            if(!b3_strrcmp(okuri_str,(unsigned char*)"モノ")) return KOTO;
            if(!kana_cmp(okuri_str,(unsigned char*)"クノ")) return POSTNO;
            if(!kana_cmp(okuri_str,(unsigned char*)"ノ")) return POSTNO;
            if(!kana_cmp(okuri_str,(unsigned char*)"ガ")) return POSTGA;
            if(!abb_included_cmp(okuri_str,(unsigned char*)"",'m',(unsigned char*)"ガ",3)) return RENTAIGA;
            if(!abb_included_cmp(okuri_str,(unsigned char*)"",'q',(unsigned char*)"ガ",3)) return RENTAIGA;
            if(!abb_included_cmp(okuri_str,(unsigned char*)"",'m',(unsigned char*)"ハ",3)) return RENTAIHAMO;
            if(!abb_included_cmp(okuri_str,(unsigned char*)"",'m',(unsigned char*)"モ",3)) return RENTAIHAMO;
            if(ori_mode % 5 != 0) return 2;
        case 5:
            if(!abb_included_cmp(okuri_str,(unsigned char*)"",'i',(unsigned char*)"",3)) return RENYO;
            if(!abb_included_cmp(okuri_str,(unsigned char*)"",'e',(unsigned char*)"",3)) return RENYO;
        default:
            return 2;
    }
}
/*
kaisi_identifyは、現在の漢字のkan_index上の対するに対するインデックスナンバーと送り仮名から、対応する前置詞を探し、
同定する。前置詞なので、上に遡って探す。見つかった場合は、０を返す。見つからなかった場合は、hogo_propertyが返した値をそのまま返す。
 #define WITH        53// W
 #define FROM        59// F
 #define YORITO      3127// Q 31*37
 #define DISYORI     3599// G
 #define POSTNI      23//  N
 #define POSTTO      29//  T
 #define POSTYORI    31//  Y
 #define POSTWO      37// O
 #define POSTHE      41
 */
int     tuduki_kaisi( unsigned char* kan, unsigned char* tuduki)
{
    int num_on_index, num_on_kun, i,len;
    unsigned char*  temp_bp;
    if(num_of_kaisi > -1) return num_of_kaisi;
    num_on_index = 0;
    num_on_kun = 0;
    len = (int)strlen((char*)kan);
    i = 0;
    while(kan_index[i] != NULL)
    {
        if(!strcmp((char*)kan_index[i]->oya,(char*)kan)){
            num_on_index++;
        }
        i++;
    }
    temp_bp = tuduki;
    while(temp_bp != NULL)
    {
        temp_bp = (unsigned char*)strstr((char*)temp_bp, (char*)kan);
        if(temp_bp != NULL){
            num_on_kun++;
            temp_bp = temp_bp + len;
        }
    }
    return num_on_index - num_on_kun;
}

int     double_kaisi_identify(int index, int attrib, int ori_index)
{
    int         temp_i;
    kanlist*    temp_kanlist;
    temp_i = index - 1;
    while(temp_i > -1)
    {
        if(PID(kan_index[temp_i]->prop, KAISI) == 0 && (kan_index[temp_i]->kun_jun == -2 - ori_index || kan_index[temp_i]->kun_jun == 0)){
            temp_kanlist = (kanlist*)kan_index[temp_i]->prop_p[3];
            if(temp_kanlist->attrib % attrib == 0) return 0;
        }
        temp_i--;
    }
    return 1;
}
int     kaisi_identify(int index, unsigned char* temp_josi,unsigned char* tuduki)
{
    int            temp_property,i,len,temp_attrib,ret,ret_d;
    kanlist*       temp_kanlist;
    unsigned char  temp_okuri[61];
    ret =  1;
    ret_d  =  0;
    temp_property = hogo_property(index, temp_josi, 2);
    if(temp_property == 2){
        strcpy((char*)temp_okuri,(char*)temp_josi);
        i = 0;
        while(*hukujosi[i] != '\0'){
            if(!b3_strrcmp(temp_okuri, (unsigned char*)hukujosi[i])) break;
            i++;
        }
        len = (int)strlen((char*)temp_okuri);
        len = len - (int)strlen((char*)hukujosi[i]);
        temp_okuri[len] = '\0';
        temp_property = hogo_property(index, temp_okuri, 2);
    }
    i = index - 1;
    while(i > -1){
        if((kan_index[i]->kun_jun == 0 || kan_index[i]->kun_jun == -2-index) && (PID(kan_index[i]->prop, KAISI)) == 0){
//        if(kan_index[i]->kun_jun == 0 && (kan_index[i]->prop % KAISI) == 0){
            temp_kanlist = (kanlist*)kan_index[i]->prop_p[3];
            temp_attrib = temp_kanlist->attrib;
            ret = temp_attrib % temp_property;
            ret_d = double_kaisi_identify(i, temp_property,i);
            break;
            }
        i--;
    }
    num_of_kaisi = tuduki_kaisi(kan_index[index]->oya, tuduki);
    if(ret == 0 && ret_d == 1 && num_of_kaisi > 0){
        kan_index[i]->kun_jun = -2 - index;
        num_of_kaisi--;
        return 0;
    }
    return temp_property;
}
/*
 ある漢字がkanlistに挙げられたものであるとき、送り仮名等を考慮してkanlistに挙げられた性質を持つものであるかを確定する。
 引数は、kanlistであるtemp_list、送り仮名であるchar_okuri、完全一致、前方一致、後方一致、部分一致を指定する１〜４整数であり、
 確定されれば０、できなければ１を返す。
 */
int     kanlist_confirm( kanlist* temp_list, unsigned char* okuri_char, int p)
{
    int     ret;
    kanatuki* temp_kana;
    temp_kana = temp_list->okuri;
    ret = 1;
    while( temp_kana != NULL)
    {
        switch(p){
            case 1: ret = kana_cmp(okuri_char, temp_kana->hiramoji);
                break;
            case 2: ret = b3_strlcmp(okuri_char, temp_kana->hiramoji);
                break;
            case 3: ret = b3_strrcmp(okuri_char, temp_kana->hiramoji);
                break;
            case 4: ret = b3_strstr(okuri_char, temp_kana->hiramoji);
        }
        if(ret == 0) return ret; //temp_list->attrib;
        temp_kana = temp_kana->next;
    }
    return 1;
}
/*
 identify_kanlist( unsigned char* kan, kanlist* temp_list)は、kanがtemp_listの中のどの漢字に相当するかを
 見つけ出し確定する。戻り値は、該当のkan_listへのポインタ。該当するものがなければ、ヌルを返す。
 */
kanlist*  identify_kanlist( unsigned char* kan, kanlist* temp_list)
{
    kanlist* temp;
    temp = temp_list;
    while( temp != NULL)
    {
        if(!kanji_cmp(kan, temp->listed_kan)){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}
/*
 preceeding_kanは現在読まれている漢字の前の漢字を探す。引数は現在の読み順t_junで戻り値は前に
 読まれた漢字のkan_indexにおけるインデックス番号。なお、そういった漢字がない場合は-1を返す。
 なお、modeに−1を入れると、直後の順の漢字を探すことになる。
 */
int     preceeding_kan( int t_jun, int mode)
{
    int     i, temp_jun;
    if(t_jun < 1 + mode) return -1;
    temp_jun = t_jun - mode;
    i = 0;
    while(kan_index[i] != NULL)
    {
        if(kan_index[i]->kun_jun == temp_jun) return i;
        i++;
    }
    return -1;
}

/*
 succeeding_charは、訓読文の文字列bpから次にくる漢字とその送り仮名をとりだす。戻り値は
 漢字とその送り仮名の構造体kanokuriへのポインタ。
 */
kanokuri*  succeeding_kan(unsigned char* bp)
{
    unsigned char*  temp_bp1;
    unsigned char*  temp_bp2;
    kanokuri*       temp_kan;
    process_error = 0;
    temp_bp1 = bp;
    temp_bp1 = del_kanji(temp_bp1);
    temp_bp1 = next_kanji(temp_bp1);
    if(temp_bp1 == NULL) return NULL;
        temp_kan = malloc(sizeof(kanokuri));
        if(temp_kan == NULL){
            printf("処理のためのメモリが足りません。\n");
            process_error++;
            return NULL;
        }
    temp_kan->kan_point = temp_bp1;
    temp_bp2 = temp_bp1;
    temp_bp2 = del_kanji(temp_bp2);
    cut_from_string(temp_kan->kanji, temp_bp1, temp_bp2);
    temp_bp1 = temp_bp2;
    temp_bp1 = skip_yomi(temp_bp1);
    temp_bp2 = temp_bp1;
    temp_bp2 = skip_kana(temp_bp2);
    cut_from_string(temp_kan->okurigana, temp_bp1, temp_bp2);
    return temp_kan;
}
/*
 find_jukugo(int index, unsigned char* kun)は、kunの中に、熟語に相当するものがあるか、それが白文の中でも熟語となっているかを判断する。
 引数は、当該漢字のindex、書き下し文の文字列に対するポインタ。返り値は、indexの漢字が問題の熟語であるか、熟語がなければ０、他に熟語があって、indexの漢字が該当しない場合１以上を返す。
 */
int     find_jukugo(int index, unsigned char* kun)
{
    int             temp_i;
    int             len, ret;
    char*           temp_bp;
    unsigned char   temp_kan[9];
    ret = 0;
    temp_i = index - 1;
    if(temp_i > -1) 
    {
        strcpy((char*)temp_kan, (char*)kan_index[temp_i]->oya);
        strcat((char*)temp_kan,(char*)kan_index[temp_i+1]->oya);
        len = (int)strlen((char*)temp_kan);
        temp_bp = (char*) kun;
        temp_bp = strstr(temp_bp, (char*)temp_kan);
        while(temp_bp != NULL)
        {
            ret += 100;
            temp_bp += len;
            temp_bp = strstr(temp_bp, (char*)temp_kan);
        }
    }
    temp_i = index;
    if(kan_index[index+1] != NULL)
    {
        strcpy((char*)temp_kan, (char*)kan_index[temp_i]->oya);
        strcat((char*)temp_kan,(char*)kan_index[temp_i+1]->oya);
        len = (int)strlen((char*)temp_kan);
        temp_bp = (char*) kun;
        temp_bp = strstr(temp_bp, (char*)temp_kan);
        while(temp_bp != NULL)
        {
            ret += 1;
            temp_bp += len;
            temp_bp = strstr(temp_bp, (char*)temp_kan);
        }
    }
    return ret;
}
/*
hendoku_point_identifyは、二重補語かどうかを判定する時などに用いる。kan_index上のインデックスナンバーと訓読文のポインタ
を引数とし、戻り値は、無事に返読できた場合は０、そうでない場合は１が返る。ただし、どういった経路で返読されているかは、temp_indexに
保存されており、次のこの関数が呼ばれるときに初期化される。
*/
int hendoku_point_identify( int index_main, int index, unsigned char* bp)
{
    kanokuri*   temp_kan1;
    unsigned char* temp_okuri[22];
    unsigned char* temp_bp;
    int         temp_i, kan_i, ret, hogo_ret,mode,temp_jun, unread;
    temp_i = 0;
    temp_jun = 1;
    init_temp_index_list();
    write_temp_list(temp_i,0,index);
    write_temp_list(temp_i,1,temp_jun);
    temp_bp = bp;
    temp_kan1 = NULL;
    kan_i = -1;
    while( temp_bp != NULL && *temp_bp != '\0' )
    {
        if(temp_kan1 != NULL){
            strcpy((char*)temp_okuri,(char*)temp_kan1->okurigana);
            free(temp_kan1);
        }
        temp_kan1 = succeeding_kan(temp_bp);
        if(temp_kan1 == NULL) return 1;
        temp_bp = temp_kan1->kan_point;
        kan_i = find_kan(temp_bp);
        if(kan_i == index_main)kan_i = find_double_kanji(index_main,1);
        if(kan_i == -1) {
            free(temp_kan1);
            return 1;
        }
        temp_i++;
        write_temp_list(temp_i,0,kan_i);
        temp_jun++;
        write_temp_list(temp_i,1,temp_jun);
        if(-1 < kan_i && kan_i < index_main) break;
    }
    if(kan_i > -1 && kan_index[kan_i]->kun_jun != 0){
        free(temp_kan1);
        return 1;
    }
    if(kan_i < 0) return 1;
    ret = hendoku_property_identify(kan_i,temp_kan1->okurigana,bp);
    free(temp_kan1);
    if(ret == PROVERB){
        mode = 5;
    }
    else{
        mode = 6;
    }
    kan_i = read_temp_list(temp_i-1,0);
    hogo_ret = hogo_property(temp_i,(unsigned char*)temp_okuri,mode);
    unread = unread_char_list( index, MAX_MOJI);
    if(ret % hogo_ret == 0 && unread == 0) return 0;
    return 1;
}
/*ある漢文訓読文からkanokuri構造体を作る。引数はunsigned char文字列へのポインタ*/
kanokuri* make_kanokuri(unsigned char* bp)
{
    kanokuri*       temp_kan;
    unsigned char*  bp_temp_s;
    unsigned char*  bp_temp_e;
    bp_temp_s = bp;
    bp_temp_e = bp;
    if(*bp_temp_e == '\0') return NULL;
        temp_kan = malloc(sizeof(kanokuri));
        if(temp_kan == NULL){
            printf("処理のためのメモリが足りません。\n");
            return NULL;
        }
    temp_kan->kan_point = bp;
    bp_temp_s = del_kanji(bp_temp_s);
    cut_from_string(temp_kan->kanji, bp,bp_temp_s);
    bp_temp_s = skip_yomi(bp_temp_s);
    bp_temp_e = bp_temp_s;
    bp_temp_e = skip_kana(bp_temp_e);
    cut_from_string(temp_kan->okurigana, bp_temp_s,bp_temp_e);
    return temp_kan;
}
/*
 訓読文上で熟語と見られる漢字がKan_index上でも熟語となっているかどうかを判定する。訓読文上だけで熟語に見えたものには1、
 他に同様な熟語がkan_index上にある場合には-2、見かけにも合致しないものは、−1を返す。合致するものには0を返す。
 */
int     juku_before_identify(int index, kanokuri* kan_temp_1, unsigned char* bp)
{
    kanokuri*      kan_temp_2;
    unsigned char* temp_bp;
    int            temp_i, temp_ret;
    long long int  ret;
    
    temp_ret = 0;
    if(*kan_temp_1->okurigana != '\0') return -1;
    temp_bp = bp;
    kan_temp_2 = succeeding_kan ( temp_bp );
    if(kan_temp_1 == NULL || kan_temp_2 == NULL) {
        free(kan_temp_2);
        return -9;
    }
    temp_i = find_kan(kan_temp_2->kan_point);
    if(temp_i == index){
        temp_i = find_double_kanji(index,1);
    }
    if(temp_i < 0 ){
        free(kan_temp_2);
        return -9;
    }
    if(temp_i == index + 1){
            temp_ret = 1;
        }
        else{
            temp_ret = -2;
        }
    ret = exist_juku(kan_temp_1->kanji,kan_temp_2->kanji,0);
    free(kan_temp_2);
    if(ret < LIST_PRIME && (int)ret == index) return 0;
    return (ret < 0 ? -1 : temp_ret);
}
/*
 訓読文上で熟語と見られる漢字がKan_index上でも熟語となっているかどうかを判定する。訓読文上だけで熟語に見えたものには1、
 他に同様な熟語がkan_index上にある場合には-2、見かけにも合致しないものは、−1を返す。合致するものには0を返す。
 */
int     juku_after_identify(int index, int kun_jun_t, kanokuri* temp_kan, unsigned char* bp)
{
    int             pre;
    int             temp_ret;
    long long int   ret;
    
    pre = preceeding_kan( kun_jun_t,1 );
    if(pre == -1) {
        return -9;
    }
    if(kan_index[pre]->okuri_n != 0) {
        return -1;
    }
    else{
        if(pre == index-1){
             temp_ret = 1;
        }
        else{
             temp_ret = - 2;
        }
    }
    ret = exist_juku(kan_index[pre]->oya,mb_strn(bp),1);
    if(ret < LIST_PRIME && (int)ret == index) {
        return 0;
    }
    return (ret < 0 ? -1 : temp_ret);
}
/*
 訓読文上で熟語と見られる漢字Xと漢字YがKan_index上でも熟語となっているものがあるかを判定する。引数は、二つの漢字の unsigned char*および前接熟語か、後接熟語かを判断するint postがある。postは、１ならば、後接熟語、０ならば前接熟語ということになる。,返り値は、熟語とみなせるものがあればそのkan_index上のインデックス番号、そうでなければ−1である。
 */
long long int     exist_juku( unsigned char* kan1, unsigned char* kan2,int post)
{
    int         i;
    long long int    ret;
    i = 0;
    ret = 1;
    while(kan_index[i] != NULL)
    {
        if(!itai_kanji_comp(kan_index[i]->oya,kan1)&& kan_index[i+1] != NULL && kan_index[i+1]->kun_jun == 0){
            if(post){
                if(kan_index[i]->kun_jun > 0 && kan_index[i]->okuri_n == 0 && !itai_kanji_comp(kan_index[i+1]->oya, kan2))
                    ret = ret*LIST_PRIME + i + 1;
            }
            else{
               if(kan_index[i]->kun_jun == 0 && !itai_kanji_comp(kan_index[i+1]->oya, kan2))
                   ret = ret*LIST_PRIME + i;
            }
        }
        i++;
    }
    if(ret == 1){
        return -1;
    }
    return ret;
}
/*
 int is_real_succeeding_kan(int pre_index, int suc_index, unsigned char* suc_bp, int jun)
 は、後続漢字が本当に後続漢字かを判定する。本当に後続漢字であることと判定すれば、suc_indexをそのまま、そうでなければ、
 次に候補となる漢字を呼び出す。うまく見つからなければ、-1を返す。
 */
int  is_real_succeeding_kan(int pre_index, int index, kanokuri* suc_bp, int jun, int juku)
{
    int temp_index;
    long long int ret;
    kanokuri*   kan_2;
    if(kan_index[index]->jufuku == 0) return index;
    if(pre_index == index || PID(kan_index[index]->prop, OKI_KANA) == 0){
        ret = juku_before_identify(index, suc_bp, suc_bp->kan_point);
        if(ret == 1) return index;
        if(ret == -2){
            kan_2 = succeeding_kan(suc_bp->kan_point);
            ret = (int)exist_juku(suc_bp->kanji, kan_2->kanji, 0);
            while(1){
                ret = ret % LIST_PRIME;
                if(ret < LIST_PRIME) {
                    free(kan_2);
                    return (int)ret;}
                ret = ret / LIST_PRIME;
            }
        }
        if(pre_index == index){
            temp_index = find_double_kanji(index, 1);
            if(temp_index < 0) temp_index = find_double_kanji(index, -1);
        }
        else{
            temp_index = index;
        }
        return temp_index;
    }
    return index;//試しに
}
/*
 find_kanはset_kanjiでkan_index[x]にセットされた漢字の中から、文字列kanを探す。戻り値はkan_indexのインデックス番号
 そういった漢字がない場合は-1を返す。*/
int     find_kan(unsigned char* bp)
{
    itai*           temp_itai;
    itai*           temp_root;
    unsigned char*  kan;
    int     i;
    kan = mb_strn( bp );
    if(!is_utf_kanji(kan)) return -1;
    i = 0;
    while(kan_index[i]!= NULL){
        if(!kanji_cmp(kan_index[i]->oya,kan) && kan_index[i]->kun_jun == 0 ){
            return i;
        }
        i++;
    }
    temp_root = find_itai(kan);
    if(temp_root != NULL){
        temp_itai = temp_root->chain;
        while(temp_itai != temp_root){
            i = 0;
            while(kan_index[i]!= NULL){
                if(!kanji_cmp(kan_index[i]->oya,temp_itai->jitai)&& kan_index[i]->kun_jun == 0){
                    bp = replace_kan(bp,kan,kan_index[i]->oya);
                    return i;
                }
                i++;
            }
            temp_itai = temp_itai->chain;
        }
        
    }
    return -1;
}
/*
 duplicate_caseは現在獲得されたindexの漢字よりもよりふさわしいものがあるかを判定し、
 見つかった場合は、そのインデックス番号を返す。どうしても見つからない場合は、-1を返す。
 引数は、与えられたindex、訓順kun_jun,現在読んでいる訓読文へのポインターkun_pである。
 */
int     duplicate_case( int index, int kun_jun, unsigned char* kun_p)
{
    int     ret, second_ret, temp_index;

    ret = 1;
    temp_index = index;
    loop_error = 0;
    while( ret > -1 && loop_error < 5){
        ret = duplicated_kan(temp_index, kun_jun, kun_p);
        if(ret == 0 ) {
            kan_index[temp_index]->gtest = 2;
            return temp_index;
        }
        if(non_done > 3 && ret != -1 && kan_index[temp_index]->gtest == 1){
            return temp_index;
        }
        else{
            kan_index[temp_index]->gtest = -ret;
        }
        second_ret = find_double_kanji(temp_index,1);
        if(second_ret < 0){
            basic_error++;
            if(basic_error > 100) return -1;
            second_ret = find_double_kanji(temp_index,0);
            if(second_ret < 0) return -1;
        }
        temp_index = second_ret;
    }
    return -1;
}
/*
 kanokuri_index* trace_juku(int index, unsigned char* bp)　は、現在熟語になっていると思われる最後のindexとその漢字を先頭とする訓読文を受け取って、熟語となっている熟語であると判断される語の最後のkan_index上のindexを含んだkanokuri_index構造体へのポインタを返す。
 */
kanokuri_index* trace_juku(int index, unsigned char* bp)
{
    kanokuri_index* temp_kindex;
    kanokuri*       temp_kanokuri;
    unsigned char*  temp_bp;
    int             temp_i, ret;
    temp_i = index;
    ret = 0;
    temp_bp = bp;
    temp_kanokuri = NULL;
    while(*temp_bp != '\0' && kan_index[temp_i] != NULL)
    {   temp_kanokuri = make_kanokuri(temp_bp);
        if(*(temp_kanokuri->okurigana) != '\0') break;
        temp_bp = del_kanji(temp_bp);
        temp_bp = next_kanji(temp_bp);
        if(temp_bp == NULL) break;
        temp_i++;
        if(itai_kanji_comp(kan_index[temp_i]->oya,mb_strn(temp_bp)) != 0){
            break;}
        free(temp_kanokuri);
    }
    temp_kindex = malloc(sizeof(kanokuri_index));
    if(temp_kindex == NULL) return NULL;
    temp_kindex->p_index = temp_i;
    temp_kindex->p_kanokuri = temp_kanokuri;
    return temp_kindex;
}
/*
 pre_connect_match()は、前の語との繋がりを判定する。connect_matchが本体だが、その前提となる処理を行う。
 引数は、判定すべきkan_index上のindex,現在の訓順jun,そして現在の漢字のkanokuri構造体へのポインタである。戻り値は、connect_matchが返す妥当０と不当１であるが、前処理の途中で問題を生じた場合、-1を返すようになっている。
 */
int     pre_connect_match(int index, int jun, kanokuri* kan_t, int target)
{
    int             pre_kan, ret;
    unsigned char*  pre_kan_okuri;
    pre_kan = preceeding_kan(jun, 1);
    if(pre_kan < 0) return -1;
    pre_kan_okuri = kan_index[pre_kan]->okuri;
    ret = connect_match(pre_kan, pre_kan_okuri,index, kan_t,kan_t->kan_point,1);
    return ret;
}
/*
 post_connect_match()は、後の語との繋がりを判定する。connect_matchが本体だが、其の前提となる処理を行う。引数は、pre_connect_matchと同じようにしている。
 */
int     post_connect_match(int index, int jun, kanokuri* kan_t, int target)
{
    kanokuri*   temp_suc;
    int         temp_suc_i, ret,possible_alt;
    temp_suc = succeeding_kan(kan_t->kan_point);
    if(temp_suc == NULL) return -1;
    temp_suc_i = find_kan(temp_suc->kan_point);
    if(temp_suc_i < 0) return -1;
    temp_suc_i = is_real_succeeding_kan(target, temp_suc_i, temp_suc, jun, 0);
    if(temp_suc_i < 0) return -1;
    if(kan_index[temp_suc_i]->jufuku > 0){
        possible_alt = find_double_kanji(temp_suc_i,1);
        if(possible_alt > -1 ){
            if(!is_kore_joji(temp_suc_i, temp_suc, jun)) temp_suc_i = possible_alt;
            if(is_siyeki_on(temp_suc_i, temp_suc->okurigana))temp_suc_i = possible_alt;
        }
    }
    ret = connect_match(index, kan_t->okurigana,temp_suc_i,temp_suc,temp_suc->kan_point,0);
    free(temp_suc);
    return ret;
}
/*
 int is_kaisi(int index,unsigned char* okuri)は、duplicated_kanの中で、それが介詞かどうかを判断する。介詞と判断されれば０、そうでなければ１を返す。
 */
int      is_kaisi(int index,unsigned char* okuri,unsigned char* tuduki)
{
    int     temp_index, ret;
    if(PID(kan_index[index]->prop, KAISI) != 0) return 1;
    temp_index = find_double_kanji(index, 1);
    if(temp_index < 0) return 1;
    ret = kaisi_identify(temp_index, okuri,tuduki);
    return ret == 0 ? 0 : 1;
    
}
/* duplicated_kanは、重複している漢字のX1が訓読文の文脈に妥当かどうかを判断する
 引数は、重複している漢字のkan_index上の位置index、訓順、kun_jun_t, 訓読文へのポインタkun_bp、
 妥当と判断すれば0、妥当でないと判断されれば、1を返す。*/
int     duplicated_kan( int index, int kun_jun_t, unsigned char* kun_bp)
{
    kanokuri*       temp_kan;
    kanokuri*       t_temp_kan;
    kanokuri_index* t_juku;
    unsigned char*  next_kan_point;
    int         juku_ret_pre, juku_ret_post,begin,last,ret,final;
    int         t_index, t_pre_index;
    if(kan_index[index]->kun_jun < -1) return 1;
    juku_ret_pre = -1;
    juku_ret_post = -1;
    temp_kan = make_kanokuri( kun_bp );
#ifdef DEBUG_4
    if(debug_line)
        printf("STOP!\n");
#endif
    if(PID(kan_index[index]->prop,KORE) == 0){
        if(!is_kore_joji(index,temp_kan,kun_jun_t)){
            free(temp_kan);
            return 1;
        }
    }
    if(!is_kaisi(index,temp_kan->okurigana,temp_kan->kan_point)) return 1;
    ret = is_siyeki_on(index, temp_kan->okurigana);
    if(ret != 0 ) return 1;
    ret = find_jukugo(index, kun_bp);
    if(ret != 0 ) return 1;
    begin = (kun_jun_t == 1 ? 1 : 0);
    if(!begin){juku_ret_post = juku_after_identify(index,kun_jun_t,temp_kan, kun_bp);}
    next_kan_point = del_kanji(kun_bp);
    next_kan_point = next_kanji(next_kan_point);
    last = (next_kan_point == NULL ? 1 : 0);
    if(!last){juku_ret_pre = juku_before_identify( index, temp_kan, kun_bp );}
    ret = evaluate_two_number(begin, last);
    final = 0;
    switch(ret){
        case 1:
            t_index = index;
            t_temp_kan = temp_kan;
            t_juku = NULL;
            switch(juku_ret_pre){
                case 0: free(temp_kan);
                    final = 0; //return 0;
                    break;
                case 1:
                case -1:
                    final = 3 * post_connect_match(index, kun_jun_t, temp_kan,index);
                    if(t_juku != NULL){
                        free(t_juku->p_kanokuri);
                        free(t_juku);
                    }
                    free(temp_kan);
                    break; //return final;
                default:
                    free(temp_kan);
                    final = 1; //return 1;
            }
            return final;
            break;
        case 2:switch(juku_ret_post){
            case 1:
            case 0:
                free(temp_kan);
                final = 0; //return 0;
                break;
            case -1:
                final = 2 * pre_connect_match(index, kun_jun_t, temp_kan,index);
                t_index = find_double_kanji(index,1);
                if(t_index > -1){
                    t_pre_index = preceeding_kan(kun_jun_t, 1);
                    if(t_pre_index > index){
                        ret = pre_connect_match(t_index, kun_jun_t, temp_kan,t_index);
                        if(ret == 0){
                            final = 1;
                        }
                    }
                }
                free(temp_kan);
                break; //return final;
            default:
                free(temp_kan);
                final = 1; //return 1;
            }
            return final;
            break;
        case 0: ret = judge_juku_ret(juku_ret_post, juku_ret_pre);
            switch(ret){
                case 0:
                    free(temp_kan);
                    final = 0;//return 0;
                    break;
                case 5:
                    free(temp_kan);
                    final = 1; //return 1;
                    break;
                case 2:
                case 4:
                    final = 3 * post_connect_match(index, kun_jun_t, temp_kan,index);
                    if( ret == 2){
                        final = final +2 * pre_connect_match(index, kun_jun_t, temp_kan,index);
                    }
                    free(temp_kan);
                    break;
                case 3:
                case 1:
                    final = 3 * post_connect_match(index, kun_jun_t, temp_kan,index);
                    if(ret == 1){
                        final = final + 2 * pre_connect_match(index, kun_jun_t, temp_kan,index);
                    }
                    free(temp_kan);
                    break;
            }
            return final;
            break;
    }
    free(temp_kan);
    return 1;
}
/* これは、ある漢字、kan_1とkan_2が返読の関係にあるかどうかを判定する。kan_1,kan_2はkan_index上の位置を示す
 int型整数、okuri1とokuri2はkan_1,kan_2の送り仮名を表す文字列、kan_1とkan_2の大小関係（前後関係）が訓読文の文法に
 あっていると判定できれば0、文法にあっていないと思われれば1を返す。*/
int     connect_match( int kan_1, unsigned char* okuri1, int kan_2, kanokuri* kan_okuri2, unsigned char* bp, int juku)
{
    int     ret, ret_read, ret_hogo, next_kan, ret_connect, temp_i;
    unsigned char* okuri2;
    unsigned char  temp_okuri1[64];
    if(kan_1 == -1) return 0;
    if(kan_1 == -9) return 1;
    if(kan_okuri2 == NULL && process_error == 0) return 0;
    okuri2 = kan_okuri2->okurigana;
    strcpy((char*)temp_okuri1,(char*)okuri1);
    ret = hukusi_property_identify(kan_1,temp_okuri1);
    if( ret > 0 ){
        if(kan_1 < kan_2) return 0;
    }
hantei:    ret = hendoku_property_identify( kan_2,okuri2,bp);
    switch( ret ){
        case POSTHE:
        case POSTNI:
        case POSTTO:
        case POSTWO:
        case POSTWONI:
        case POSTYORI:
        case GENERAL:
            ret_hogo = hogo_property(kan_1,temp_okuri1,2);
            break;
        case SPECIAL1:
        case SPECIAL_COPULA:
        case COPULA:
        case RENTAI:
        case EXIST:
            ret_hogo = hogo_property(kan_1,temp_okuri1,3);
            break;
        case PROVERB:
            ret_hogo = hogo_property(kan_1,temp_okuri1,5);// mode = 5;
            break;
        default:
            ret_hogo = hogo_property(kan_1,temp_okuri1,ret);
    }
    ret_connect = ret % ret_hogo;
    if( ret_connect == 0){
        if(kan_1 > kan_2){
            if(kan_index[kan_2]->jufuku == 0){
                kan_index[kan_2]->is_dosi = ret_hogo;
                return 0;
            }
            else{
                temp_i = find_double_kanji(kan_2, 1);
                if(temp_i == -1) {
                    //kan_index[kan_2]->is_dosi = 1;
                    return 0;
                }else{
                    if(kan_1 > temp_i && juku) return 1;
                    kan_index[kan_2]->is_dosi = ret_hogo;
                    return 0;
                }
            }
        }
        else{ //without this source, An error occurs
            ret = kaisi_identify(kan_1, okuri1,bp);
           if(ret == 0) return 0;
        }
        if(del_kana_posteria_list(temp_okuri1,3)) goto hantei;
        ret = hogo_property(kan_2,okuri2,2);
        if(GENERAL % ret == 0) {
            next_kan = find_double_kanji(kan_1, 1);
            if(next_kan == -1){
                kan_index[kan_2]->is_dosi = 1;
                return 0;
            }
            else{
                return 1;
            }
        }
        if(GENERAL % ret_hogo == 0){
            if(kan_1 + 1 == kan_2){
                ret = kaisi_identify(kan_1,temp_okuri1,bp);
                if(ret == 0) {
                    kan_index[kan_2]->is_dosi = ret_hogo;
                    return 0;}
                ret = daisi_identify(kan_1);
                if(ret == 0) {
                    kan_index[kan_2]->is_dosi = ret_hogo;
                    return 0;}
            }
            ret_read = already_read(kan_1);
            ret = hendoku_point_identify(kan_1,kan_2,bp);
            if(ret == 0 && ret_read == 0) {
                kan_index[kan_2]->is_dosi = 1;
                return 0;}
            return 1;
        }
    }
    else{
        if(kan_1 < kan_2) {
         //   kan_index[kan_2]->is_dosi = 1;
            return 0;
        }
        if(del_kana_posteria_list(temp_okuri1,3)) goto hantei;
    }
    if( kan_1 < kan_2 ){
        kan_index[kan_2]->is_dosi = 1;
        return 0; }
    return 1;
}

/*--------------------------------------------------------------------------------------*/
/*
 一文の中で重複している漢字すべてをマークする。なお、別々の文字の重複には別のマークがつけられる。
 戻り値は、何種の文字が重複しているかという数字である。ただし、この関数はkan_indexにすべての漢字が
 セットできた後に適用できる。
 */
int choufuku( void )
{
    int             index, i, mark,mark_prime;
    long int                jufuku_n;
    unsigned char*                   temp_kan;
    index = 0;
    mark =  2;
    jufuku_n = 1;
    while(kan_index[index]!= NULL){
        if(kan_index[index]->kun_jun > -1 && kan_index[index]->jufuku < 0){
            mark++;
            i = index + 1;
            mark_prime =  prime_num(mark,MAX_PRIME,1);
            jufuku_n = jufuku_n * mark_prime;
            kan_index[index]->jufuku = jufuku_n;
            temp_kan = kan_index[index]->oya;
            while(kan_index[i] != NULL){
                if(!kanji_cmp(kan_index[i]->oya,temp_kan)){
                    jufuku_n = jufuku_n * mark_prime;
                    kan_index[i]->jufuku = jufuku_n;
                }
                i++;
            }
            jufuku_n = 1;
        }
        index++;
    }
    return mark-1;
}

/*
 length_kan_indexはkan_indexの長さを測る。引数はなし、戻り値は文の最後のインデックスナンバー
 */
int     length_kan_index( void )
{
    int     num;
    num = 0;
    while(kan_index[num] != NULL){
        num++;
    }
    return num - 1;
}
/*
 ア段（四段未然）、ウ段（動詞終止）、エ段、ラ行（ラ変型）の文字を取り出す。キーとなる文字xとその順を読み込んで具体的なカナを返す。
 */
unsigned char* hojo(char x, int p_temp)
{
    switch(x)
    {
        case 'a':
            strcpy((char*)kana_temp_char,(char*)a_dan_s[p_temp]);
            break;
        case 'i':
            strcpy((char*)kana_temp_char,(char*)i_dan_s[p_temp]);
            break;
        case 'u':
            strcpy((char*)kana_temp_char,(char*)u_dan_s[p_temp]);
            break;
        case 'e':
            strcpy((char*)kana_temp_char,(char*)e_dan_s[p_temp]);
            break;
        case 'r':
            strcpy((char*)kana_temp_char,(char*)ra_gyo_s[p_temp]);
            break;
        case 'k':
            strcpy((char*)kana_temp_char,(char*)keiyo[p_temp]);
            break;
        case 's':
            strcpy((char*)kana_temp_char,(char*)sahen[p_temp]);
            break;
        case 'm':
            strcpy((char*)kana_temp_char,(char*)rentai[p_temp]);
            break;
        case 'g':
            strcpy((char*)kana_temp_char,(char*)ka_gyo[p_temp]);
            break;
        case 'q':
            strcpy((char*)kana_temp_char,(char*)jodo[p_temp]);
            break;
        default:
            kana_temp_char[0] = '\0';
            
    }
    return kana_temp_char;
}
/*
 特定の音節群xをその中に含んでいるような文字列scの同定を行う。s1の中にscを一定の条件、完全一致、前方一致、
 後方一致、部分一致を指定する１〜４整数で指定している。一致すれば0、しなければ1を返す。
 */
int     abb_included_cmp(unsigned char* s1, unsigned char* sc_p, char x, unsigned char* sc_s, int p)
{
    unsigned char  s_str[14];
    int            temp_p, ret;
    temp_p = 0;
    ret = 1;
    hojo(x,temp_p);
    while(*kana_temp_char != '\0')
    {
        sprintf((char*)s_str,"%s%s%s",(char*)sc_p,(char*)kana_temp_char,(char*)sc_s);
        switch(p){
            case 1: ret = kana_cmp(s1, s_str);
                break;
            case 2: ret = b3_strlcmp(s1, s_str);
                break;
            case 3: ret = b3_strrcmp(s1, s_str);
                break;
            case 4: ret = b3_strstr(s1, s_str);
        }
        if(ret == 0) return ret;
        temp_p++;
        hojo(x,temp_p);
    }
    return ret;
}
/*
 $x$を一文字のカタカナで置き換える。置き換えられなければならない元の文字列はs1、置き換えるカタカナは、s2で、それらを引数とし、s1を戻り値とするが、s1は、この関数ですでに$x$を置き換えられた文字列となっている。
 */
unsigned char* dl_replace( unsigned char* s1, unsigned char* s2)
{
    unsigned char* pp;
    pp = s1;
    while( *pp != '\0')
    {
        if(*pp == '$')
        {
            *pp = s2[0];
            pp++;
            *pp = s2[1];
            pp++;
            *pp = s2[2];
        }
        pp++;
    }
    return s1;
}


/*
 redup_kanaindexはkanaindexのori_kanaにある$x$を具体的なカタカナに置き換え、新しいkanaindexを複製します。戻り値はkanaindexのポインターです。
 */
kanaindex* redup_kanaindex(kanaindex* ori_kana, unsigned char* real, int f)
{
    kanaindex* temp_kanaindex;
    temp_kanaindex = malloc(sizeof(kanaindex));
    if(temp_kanaindex == NULL)
    {
        printf("メモリが確保できません。\n");
        return NULL;
    }
    temp_kanaindex->ato = ori_kana->ato;
    temp_kanaindex->kanaindex_n = ori_kana->kanaindex_n;
    temp_kanaindex->kanayomi_n = ori_kana->kanayomi_n;
    temp_kanaindex->mae_setu_n = ori_kana->mae_setu_n;
    if(ori_kana->kanaindex_n > 0)
    {
        strncpy((char*)temp_kanaindex->kanaindex,(char*)ori_kana->kanaindex,ori_kana->kanaindex_n+1);
    }
    if(ori_kana->kanayomi_n > 0)
    {
        strncpy((char*)temp_kanaindex->kanayomi,(char*)ori_kana->kanayomi,ori_kana->kanayomi_n+1);
    }
    if(ori_kana->mae_setu_n > 0)
    {
        strncpy((char*)temp_kanaindex->mae_setu,(char*)ori_kana->mae_setu,ori_kana->mae_setu_n+1);
    }
    else{
        ori_kana->mae_setu[0] = '\0';
    }
    temp_kanaindex->next = ori_kana;
    if(f == 1)
    {
        dl_replace(ori_kana->mae_setu, real);
        dl_replace(ori_kana->kanaindex, real);
    }
    if(f == 2)
    {
        dl_replace(ori_kana->kanaindex, real);
    }
    return temp_kanaindex;
}
/*
 もとの$x$を含んだkanaindexをhojoをもちいて$x$を具体的カナで置き換えていきます。flagの値によって前接部、後接部を区別し、a,bは、それぞれ前接部・後接部の置き換えるべき文字列の
    キーとなる文字を指定している。
 */
kanaindex* expand_kana(kanaindex* ori_kana, char a, char b, int flag)
{
    unsigned char* temp_c;
    kanaindex* temp_kana;
    kanaindex* final;
    int         temp_p1,temp_p2;
    int         test;
    temp_p1 = 0;
    temp_p2 = 0;
    temp_kana = ori_kana;
    if(flag == 1)
    {
        temp_c = hojo(a, temp_p1);
        while(*temp_c != '\0')
        {
            temp_kana = redup_kanaindex(temp_kana,temp_c,flag);
            temp_p1++;
            temp_c = hojo(a, temp_p1);
        }
    }
    if(flag == 2)
    {
        temp_c = hojo(b, temp_p2);
        test = strcmp((char*)temp_c,"");
    while(*temp_c != '\0')
        {
            temp_kana = redup_kanaindex(temp_kana,temp_c,flag);
            temp_p1++;
            temp_c = hojo(b, temp_p1);
            test = strcmp((char*)temp_c,"");
        }
    }
    final = temp_kana->next;
    free(temp_kana);
    return final;
}
/*
 もとの$x$を含んだkanatukiをhojoをもちいて$x$を具体的カナで置き換えていきます。ccはhojoに渡す
    キーとなる文字、cpはkanatukiに渡された文字数。$x$を一つのカナで置き換えるので、文字数は変わらない。
 */
kanatuki* expand_kana_abb(kanatuki* ori_kana, unsigned char cc, int cp)
{
    unsigned char temp_char[13];
    unsigned char* temp_kana;
    kanatuki*       temp_okuri;
    kanatuki*       next_okuri;
    int         pp;
    temp_okuri = NULL;
    strcpy((char*)temp_char, (char*)ori_kana->hiramoji);
    pp = 0;
    temp_kana = hojo(cc, pp);
    dl_replace(ori_kana->hiramoji, temp_kana);
    next_okuri = ori_kana;
    while(1){
        if(*hojo(cc,pp + 1) == '\0') break;
        temp_okuri = malloc(sizeof(kanatuki));
        if(temp_okuri == NULL){
            printf("メモリが確保できません");
            return NULL;
        }
        strcpy((char*)temp_okuri->hiramoji,(char*)temp_char);
        pp++;
        temp_kana = hojo(cc, pp);
        dl_replace(temp_okuri->hiramoji, temp_kana);
        temp_okuri->mojisu = cp;
        temp_okuri->next = next_okuri;
        next_okuri = temp_okuri;
    }
    if(temp_okuri != NULL) return temp_okuri;
    return NULL;
}




/*
 送り仮名が特殊（仮名にすべき漢字の読みにまぎらわしいもの）である漢字を、送り仮名とともに当該の用法であるかを確認する。
 引数は確認すべき漢字のkan_indexでの配列iと特殊な送り仮名のデータリストroot(実体はroot_okuri)、戻り値は一致した場合、
 0,一致しなかった場合1。
 */
int okuri_identify(int i, kanlist *root)
{
    kanatuki* tmp;
    kanlist* o_tmp;
    o_tmp = root;
    while(o_tmp != NULL)
    {
        if(!kanji_cmp(kan_index[i]->oya,o_tmp->listed_kan)){
            tmp = o_tmp->okuri;
            while(tmp != NULL){
                if(!b3_strlcmp(kan_index[i]->okuri,tmp->hiramoji)){
                    kan_index[i]->prop = tmp->mojisu;
                    return 0;
                }
                tmp = tmp->next;
            }
            return 1;
        }
        o_tmp = o_tmp->next;
    }
    return 1;
}
/*
 kan_index[i]にセットされた漢字の中から再読文字を探し、oyakan構造体のsaiを１にする。
 引数は再読文字のデータベースsaidoku構造体へのポインタ。
 */
void saidoku_identify( saidoku* char_sai_ori )
{
    int i, len, i_tmp, flag;
    saidoku* char_sai;
    //unsigned char* c_tmp;
    i = 0;
    while(kan_index[i] != NULL){
        char_sai = char_sai_ori;
        i_tmp = i;
        if(is_utf_kanji(kan_index[i]->oya) == 0){
            i++;
            continue;
        }
    while(char_sai != NULL){
        len = (int)strlen((char*)char_sai->saikan);
        flag = b3_strlcmp(char_sai->saikan,kan_index[i]->oya);
        if(flag == 0){
            if(len > 3 && b3_strrcmp(char_sai->saikan,kan_index[i+1]->oya) == 0){
                kan_index[i_tmp]->sai = 1;
                i_tmp = i_tmp + 1;
                }
            if(strncmp((char*)kan_index[i_tmp]->okuri,(char*)char_sai->okuri,char_sai->okuri_n) == 0){
                kan_index[i_tmp]->sai = 1;
                strswp(kan_index[i_tmp]->kaeri_okuri,kan_index[i_tmp]->okuri);
                strswp(kan_index[i_tmp]->kaeri,kan_index[i_tmp]->yomi);
                kan_index[i_tmp]->kaeri_okuri_n = kan_index[i_tmp]->okuri_n;
                kan_index[i_tmp]->okuri_n = 0;
                if(len > 3 && i_tmp >= 0){
                    strswp(kan_index[i_tmp-1]->kaeri_okuri,kan_index[i_tmp-1]->okuri);
                    strswp(kan_index[i_tmp-1]->kaeri,kan_index[i_tmp-1]->yomi);
                    kan_index[i_tmp-1]->kaeri_okuri_n = kan_index[i_tmp-1]->okuri_n;
                    kan_index[i_tmp-1]->okuri_n = 0;
                }
                kan_index[i_tmp]->sai_yomi = char_sai->yomi_s;
                break;
            }
        }
        char_sai = char_sai->next_s;
    }
        i = i_tmp + 1;
    }
}
/*
 １漢字の文字列kanを読み込んで異体字の構造体itaiへのポインターを返す。その漢字の異体字が
 itaiji.tblにない文字であれば、NULLを返す。
 */
itai* find_itai(unsigned char* kan)
{
    itai* temp;
    int     r;
    temp = find_root_itai();
    while(temp != NULL)
    {
        r = kanji_cmp(temp->jitai,kan);
        if(r == 0) return temp;
        temp = temp->next_jitai;
    }
    return NULL;
}
/*
itai_kanji_compは、 kanji_cmpのkan1,kan2を異体字の領域まで広げて同一性を判定する。
 */
int     itai_kanji_comp(unsigned char* kan1, unsigned char* kan2)
{
    itai* temp;
    itai* const_chain;
    if(kanji_cmp(kan1,kan2) == 0) return 0;
    temp = find_root_itai();
    temp = find_itai(kan1);
    if(temp == NULL) return 1;
    const_chain = temp;
    temp = temp->chain;
    while( const_chain != temp){
        if(kanji_cmp(kan2, temp->jitai) == 0) return 0;
        temp = temp->chain;
    }
    return 1;
}
/*
 kan_indexおよび、それに対応して返点を打つためのデータkaeri_indexのメモリを解放する。
 */
void index_free( void )
{
    int i = 0;
    while(i < MAX_MOJI){
        if(kan_index[i]!= NULL) free(kan_index[i]);
        if(kaeri_index[i] != NULL) free(kaeri_index[i]);
        i++;
    }
}
/*
 kan_indexおよび、それに対応して返点を打つためのデータkaeri_indexの紐付けを切り離し初期化する。
 */
void    index_init( void )
{
    int i;
    for(i=0;i < MAX_MOJI;i++){
        kan_index[i] = NULL;
        kaeri_index[i] = NULL;
    }
}

/*
 訓読文にある送り・読み仮名をそれぞれ対応する漢字のkan_index[n]のokuri,yomiに当て
 はめる。引数は、訓読文のポインタとkan_indexの漢字対応するインデックスindexで、戻り値は
 読み仮名、送り仮名を読み取った後の訓読文へのポインタ。
 */
unsigned char* kun_kana(unsigned char* bp, int index)
{
    int okuri_c,yomi,sakun;
    okuri_c  = 0;
    yomi = 0;
    sakun = 0;
    while(is_utf_kanji(bp)==0 && *bp != '\0'){
        if(*bp<0x80){
            if(*bp == '(') yomi = 1;
            if(*bp == ')') yomi = 0;
            if(*bp == '[') sakun = 1;
            if(*bp == ']') sakun = 0;
            bp++;
            continue;}
        if(*bp<0xDF && *bp>0xC1)bp = bp + 2;
        if(yomi == 1 && is_utf_hira(bp)>0 ){
            strncat((char*)kan_index[index]->yomi,(char*)bp,3);
 //           printf("kan_index[%d].yomi = %s\n",index,kan_index[index]->yomi);
 //           printf("kan_index[%d].okuri = %s\n",index,kan_index[index]->okuri);
            bp = bp + 3;
            continue;
        }
        if(sakun == 1 && is_utf_hira(bp)>0){
            strncat((char*)kan_index[index]->kaeri,(char*)bp,3);
            bp = bp + 3;
            continue;
        }
        if(is_utf_kata(bp)>0){
            strncat((char*)kan_index[index]->okuri,(char*)bp,3);
            okuri_c= okuri_c + 3;
            bp = bp + 3;
            continue;
        }
        if(is_utf_odori(bp)>0){
            strncat((char*)kan_index[index]->okuri,(char*)bp,3);
            okuri_c= okuri_c + 3;
            bp = bp + 3;
            continue;
        }
        if(strncmp((char*)bp,"。",3)==0){
            bp = bp+3;
            if(*bp != '\0') continue;
            kan_index[index]->okuri[okuri_c] = '\0';
            kan_index[index]->okuri_n = okuri_c;
            return bp;
        }
        if(!is_utf_kanji(bp)){
            bp = bp + 3;
        }
    }
    kan_index[index]->okuri[okuri_c] = '\0';
    kan_index[index]->okuri_n = okuri_c;
    return bp;
}
/*---------------------以下 返点を打つための一連の関数--------------------------*/
/*
 この後のinit_code, f_or_b, reten, utu_kaeri, prepare_codeは返点を打つための一連の関数。
 */
void init_code(void)
{
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;
    start[3] = 0;
    start[4] = 0;
    start[5] = 0;
    start[6] = 0;
    count_n[0] = 0;
    count_n[1] = 0;
    count_n[2] = 0;
    count_n[3] = 0;
    count_n[4] = 0;
    count_n[5] = 0;
    count_n[6] = 0;
}
int f_or_b(int ind, int temp)
{
    int q;
    q = ind+1;
    while(kan_index[q] != NULL){
    if(kan_index[q]->kun_jun  < 1 )
    {
        q++;
        continue;
    }
    if(kan_index[q]->kun_jun > temp) return 0;
    if(kan_index[q]->kun_jun < temp) return 1;
        q++;
    }
    return 0;
}
/*
 kaeri_index[i]->code[0]が１である場合に、レ点を打つ。
 */
void reten( void )
{
    int i;
    i = 0;
    while(kaeri_index[i] != NULL)
    {
        if(kaeri_index[i]->code[0] == 1){
            strcpy((char*)kan_index[i]->kaeri_ten,"レ");
        }
        i++;
    }
}
/*
kan_index[i]にある漢字に対し、
 その漢字の返点の層s、その層での順序count、一文の中で最高層strat、関数制御用のadd
 から実際にkan_index[i]->kaeri_tenに実際の返点を書き込む。戻り値は関数制御用のadd。
 */
int utu_kaeri(int i,int s,int count, int strat, int add)
{
    int j, level,c, ret;
    j = i;
    level = strat - s + add;
    ret = 0;
    switch(level)
    {
        case 1:
            if(j > 1 && kan_index[j-1]->kun_jun == -1){
                c = 2;
                while(kan_index[j-c]->kun_jun == -1){
                    c++;
                }
                if(kaeri_index[j-c]->code[s] != 0)
                {
                    kaeri_index[j]->code[s] = 0;
                    kaeri_index[j-c]->code[0] = 1;
                    break;
                }
            }
            c = 1;
            strcpy((char*)kan_index[j]->kaeri_ten,(char*)ichini[0]);
            if(kaeri_index[j]->code[0] == 1){
                strcpy((char*)kan_index[j]->kaeri_ten,"\\IR");
               kaeri_index[j]->code[0] = 0;
            }
            while(j > -1 && kaeri_index[j]->code[s]!= 1){
                if(kaeri_index[j]->code[s] != 0)
                {
                    strcpy((char*)kan_index[j]->kaeri_ten,(char*)ichini[c]);
                    c++;
                }
                j--;
            }
            strcpy((char*)kan_index[j]->kaeri_ten,(char*)ichini[c]);
            break;
        case 2:
            c = 1;
            if(count == 2){
                strcpy((char*)kan_index[j]->kaeri_ten,(char*)jouge[0]);
                if(kaeri_index[j]->code[0] == 1){
                    strcpy((char*)kan_index[j]->kaeri_ten,"\\JR");
                 kaeri_index[j]->code[0] = 0;
                }
                while(j > -1 && kaeri_index[j]->code[s]!= 1){
                    if(kaeri_index[j]->code[s] != 0)
                    {
                        strcpy((char*)kan_index[j]->kaeri_ten,(char*)jouge[c]);
                        c++;
                    }
                    j--;
                }
                strcpy((char*)kan_index[j]->kaeri_ten,(char*)jouge[c]);
                break;
            }
            if(count == 3){
                strcpy((char*)kan_index[j]->kaeri_ten,(char*)jouchu[0]);
                if(kaeri_index[j]->code[0] == 1){
                    strcpy((char*)kan_index[j]->kaeri_ten,"\\JR");
                  kaeri_index[j]->code[0] = 0;
                }
                while(j > -1 && kaeri_index[j]->code[s]!= 1){
                    if(kaeri_index[j]->code[s] != 0)
                    {
                        strcpy((char*)kan_index[j]->kaeri_ten,(char*)jouchu[c]);
                        c++;
                    }
                    j--;
                }
                strcpy((char*)kan_index[j]->kaeri_ten,(char*)jouchu[c]);
                break;
            }
            ret = 1;
        case 3:
            c = 1;
            strcpy((char*)kan_index[j]->kaeri_ten,(char*)kouotu[0]);
            if(kaeri_index[j]->code[0] == 1){
                strcpy((char*)kan_index[j]->kaeri_ten,"\\KR");
                kaeri_index[j]->code[0] = 0;
            }
            while(j > -1 && kaeri_index[j]->code[s]!= 1){
                if(kaeri_index[j]->code[s] != 0)
                {
                    strcpy((char*)kan_index[j]->kaeri_ten,(char*)kouotu[c]);
                    c++;
                }
                j--;
            }
            strcpy((char*)kan_index[j]->kaeri_ten,(char*)kouotu[c]);
            break;
        case 4:
            c = 1;
            strcpy((char*)kan_index[j]->kaeri_ten,(char*)tenti[0]);
            if(kaeri_index[j]->code[0] == 1){
                strcpy((char*)kan_index[j]->kaeri_ten,"\\TR");
               // kaeri_index[j]->code[0] = 0;
            }
            while(j > -1 && kaeri_index[j]->code[s]!= 1){
                if(kaeri_index[j]->code[s] != 0)
                {
                    strcpy((char*)kan_index[j]->kaeri_ten,(char*)tenti[c]);
                    c++;
                }
                j--;
            }
            strcpy((char*)kan_index[j]->kaeri_ten,(char*)tenti[c]);
            break;
            
        case 5:
            
        case 6:
            
        default:
            wc_print(返り点が打てません\n);
            printf("返り点が打てません\n");
            break;
    }
    if(ret == 1 || add == 1) ret = 1;
    return ret;
}
/*
 joji_injukuは中棒で繋がれるべき漢字群の中に書き下しの中に現れない助字が含まれている場合に対応する。想定する助字は「而」などである。
 */
int     joji_injuku( int i)
{
    if(kan_index[i]->kun_jun - kan_index[i-1]->kun_jun == 1) return 1;
    if(kan_index[i-1]->kun_jun == 0){
        if(kan_index[i]->kun_jun - kan_index[i-2]->kun_jun == 1){
            kaeri_index[i-1]->juku = 1;
            return 1;
        }
    }
    if(kan_index[i]->kun_jun == 0 && kaeri_index[i]->juku == 1)
    {
        return 1;
    }
    return 0;
}
/*
 kan_index[x]につけられたoyakan->kun_kunからkaeri_index[x]に返点を
 打つためのデータをセットし、実際に返点を書き込む関数utu_kaeriを呼び出す。
 */
void prepear_code( void )
{
    int        s, i, j, adv;
    int        reten_flag;
    i = 0;
    s = 0;
    reten_flag = 0;
    strata = 1;
    add = 0;
    init_code();
    while(kan_index[i] != NULL)
    {
        adv = 1;
        while (kan_index[i+adv] != NULL && kan_index[i+adv]->kun_jun == -1){
            adv++;
        }
        if(kan_index[i+adv] != NULL && kan_index[i+adv]->kun_jun > 0 && kan_index[i]->kun_jun == kan_index[i+adv]->kun_jun + 1)
        {
            kaeri_index[i]->code[0] = 1;
            reten_flag = 1;
        }
    
        if(kan_index[i]->kun_jun != -1 && kan_index[i]->kun_jun == start[s]-1){
            if(f_or_b(i,kan_index[i]->kun_jun) && reten_flag == 0){
                j = i;
                while(j> 0 && joji_injuku(j) == 1 && j >= 0){
                    //kaeri_index[j]->juku = 1;
                    kaeri_index[j-1]->juku = 1;
                    j--;
                }
                start[s] = kan_index[j]->kun_jun;
                count_n[s] = count_n[s] + 1;
                kaeri_index[j]->code[s] = count_n[s];
            }
            else
            {
                add = utu_kaeri(i,s,count_n[s]+1,strata,add);
                start[s] = 0;
                count_n[s] = count_n[s] + 1;
                kaeri_index[i]->code[s] = count_n[s];
                count_n[s] = 0;
                s = s -1;
                if(s == 0){
                    strata = 1;
                }
            }
            reten_flag = 0;
            i++;
            continue;
        }
        if(f_or_b(i,kan_index[i]->kun_jun) && reten_flag == 0)
        {
            j = i;
            while(j> 0 && kan_index[j]->kun_jun - kan_index[j-1]->kun_jun == 1 && j >= 0){
                //kaeri_index[j]->juku = 1;
                kaeri_index[j-1]->juku = 1;
                j--;
            }
            s = s + 1;
            if(s >= strata) strata = s + 1;
            start[s] = kan_index[j]->kun_jun;
            count_n[s] = count_n[s] + 1;
            kaeri_index[j]->code[s] = count_n[s];
        }
        reten_flag = 0;
        i++;
        }
    if(read_verbose() > 1){
        kaeri_print();
    }
    reten();
}
/*------------------------------以上が返点を打つための関数-----------------------------*/
/*
 int okiji_kaisi_identify( int index, int max )は、indexが、介詞であるばあい、それに基づく送り仮名を持った
 漢字を見つけ、それが返読されるとすれば、その漢字のindexを、そうでなければ-1を返す。
 */
int okiji_kaisi_identify( int index, int max )
{
    kanlist*    temp_list;
    kanatuki*   temp_okuri;
    int temp_index, temp_suc_index,temp_kun_jun, count;
    count = 0;
    if(index < 0 )return -1;
    if(kan_index[index]!= NULL && kan_index[index]->kun_jun == 0 && PID(kan_index[index]->prop, KAISI) == 0){
        temp_list = (kanlist*)kan_index[index]->prop_p[3];
        if(temp_list->attrib % HIDDEN_PREPOSITION != 0) return -1;
        temp_okuri = temp_list->okuri;
        temp_index = immediate_index(index, 1);
        if(temp_index < 0) return -1;
        temp_kun_jun = kan_index[temp_index]->kun_jun;
        while(temp_index > -1 && kan_index[temp_index] != NULL && count < max){
            if(kan_index[temp_index]->okuri_n == 0){
                temp_index++;
                continue;
            }
            while(temp_okuri != NULL){
                if(!b3_strrcmp(kan_index[temp_index]->okuri, temp_okuri->hiramoji)){
                    temp_suc_index = preceeding_kan(kan_index[temp_index]->kun_jun, -1);
                    if(temp_suc_index > 0 && temp_suc_index < temp_index){
                        kan_index[temp_index]->prop = temp_okuri->mojisu;
                        return temp_index;
                    }
                    else{
                        return -1;
                    }
                }
                temp_okuri = temp_okuri->next;
            }
            temp_index = immediate_index(temp_index, 1);
            if(temp_index < 0) return -1;
            if(kan_index[temp_index]->kun_jun != temp_kun_jun + 1)return -1;
            temp_kun_jun = kan_index[temp_index]->kun_jun;
            count++;
        }
        return -1;
    }
    return -1;
}
/*
 ある文字kan_index[kan_i]が置字である可能性を判定する。これもその文字に対応する
 kanaindex構造体のkanayomi_nが０場合に呼び出される。引数はkanaindex構造体のポインタ
 漢字の位置を示すkan_iそして順読か返読かをしめすr_or_lである。置字と判断すれば、０。
 そうでなければ、１を返す。
 */
int okiji_identify(kanaindex* source_okiji, int kan_i,int r_or_l)
{
    static int ip, course;
    course = 1;
    if(r_or_l == -1) course = -1;
    ip = kan_i + course;
    while(ip >= 0 && kan_index[ip] != NULL){
        if(kan_index[ip]->okuri_n < source_okiji->kanaindex_n)
        {
            ip = ip + course;
            continue;
        }
        if(source_okiji->ato == -1)
        {
            if(!b3_strrcmp(kan_index[ip]->okuri,source_okiji->kanaindex))
                return 0;
        }
        if(!b3_strlcmp(kan_index[ip]->okuri,source_okiji->kanaindex)) return 0;
        ip = ip + course;
    }
    return 1;
}
/*
 カナにすべき漢字を確定したところで、読みの順序を整える。
 */
void reordering(int ind, int course)
{
    int jun, end_jun, temp_ind, end_ind;
    jun = kan_index[ind]->kun_jun;
    end_jun = jun + course -1;
    end_ind = ind + course -1;
    temp_ind = 0;
    while(kan_index[temp_ind] != NULL){
        if(temp_ind < ind || end_ind <temp_ind){
            if(kan_index[temp_ind]->kun_jun >= jun){
           kan_index[temp_ind]->kun_jun = kan_index[temp_ind]->kun_jun + course;
                   // temp_ind = temp_ind + course;
        }
        }
      temp_ind++;
    }
}
/*
 再読文字によって読む順序が変わった場合に順序を整える。再読文字は再読した場合の順序とする。
 */
void saidoku_reordering(int jun, int jun2, int jun_n)
{
    int i,q;
    i = 0;
    q = jun + jun_n -1;
    while(kan_index[i] != NULL)
    {
        if(q < kan_index[i]->kun_jun && kan_index[i]->kun_jun < jun2){
            kan_index[i]->kun_jun = kan_index[i]->kun_jun - jun_n;
        }
        i++;
    }
}
/*
 再読文字を使った文では処理の便宜上、kaeri_okuriとokuri、kaeriとyomiを入れ替えているので本来のものに戻す。それぞれ、kan_indexにある文字ごとに操作する。引数、戻り値はなし。
 */
void refine_saidoku( void )
{
    int i,q;
    i= 0;
    while(kan_index[i] != NULL)
    {
        if(kan_index[i]->sai == 1){
            strswp(kan_index[i]->kaeri,kan_index[i]->yomi);
            strswp(kan_index[i]->kaeri_okuri,kan_index[i]->okuri);
            q = kan_index[i]->kaeri_okuri_n;
            kan_index[i]->kaeri_okuri_n = kan_index[i]->okuri_n;
            kan_index[i]->okuri_n = q;
            }
        i++;
    }
}
/*
 踊り字になっているものが漢字に対応しているかを探す。引数は、対応する漢字のインデックスiのみ、返り値は対応するものがあれば0,そうでなければ1となる。
 */
int odori_identify(int i)
{
    unsigned char* temp;
    int             temp_n;
    if(i < 1) return 1;
    if(kanji_cmp(kan_index[i]->oya,kan_index[i-1]->oya)) return 1;
    if(!is_utf_odori(kan_index[i-1]->okuri)) return 1;
    temp = kan_index[i-1]->okuri + 3;
    strcpy((char*)kan_index[i-1]->okuri,"");
    temp_n = kan_index[i-1]->okuri_n - 3;
    strcpy((char*)kan_index[i]->okuri,(char*)temp);
    kan_index[i]->okuri_n = temp_n;
    kan_index[i]->kun_jun = kan_index[i-1]->kun_jun + 1;
    reordering(i,1);
    return 0;
}
/*
 かな読み重複　int find_hitei( int index) そのかな読みをする漢字が、kan_index[x]（送り仮名を取ってくるところ）との間に、他にないかを探す。
 */
int find_hitei( int index )
{
    int i, j, ret, aim, count;
    unsigned char*  temp_i;
    unsigned char*  temp_okuri;
    char*  final;
    if(PID(kan_index[index]->prop, HITEI) != 0) return index;
    i = index + 1;
    ret = 0;
    count = 0;
    while(kan_index[i] != NULL)
    {
        if(kan_index[i]->kun_jun > 0 && kan_index[i]->okuri_n > 2){
            temp_i = NULL;
            temp_okuri = kan_index[i]->okuri;
            j = 0;
            if(okuri_identify(i, find_root_okuri()) == 0)
                temp_okuri = temp_okuri + kan_index[i]->prop;
            if(*temp_okuri == '\0') goto next_i;
            while(j < 7) {
                temp_i = (unsigned char*)strstr((char*)temp_okuri,(char*)hitei_yomi[j]);
                if(temp_i != NULL) {
                    count = (int)strlen((char*)hitei_yomi[i]);
                    temp_i = temp_i + count;
                    break;
                }
                j++;
            }
            if(temp_i == NULL) {
                goto next_i;
            }
 //           if(okuri_identify(i, find_root_okuri()) == 0) goto next_i;
            if(*temp_i == '\0') return index;
            j = 0;
            while( j < 7){
                final = strstr((char*)temp_i,(char*)hitei_yomi[j]);
                    if(final != NULL){
//                        if(okuri_identify(i, find_root_okuri()) != 0){
                            goto next_step;
//                        }
                    }
                j++;
            }
            if(temp_i == NULL) return index;
        }
    next_i:        i++;
    }
next_step:    aim = i;
    i = index + 1;
    while(kan_index[i] != NULL && i < aim){
        if(kan_index[i]->kun_jun == 0 && PID(kan_index[i]->prop, HITEI) == 0){
            ret = 1;
            break;
        }
        i++;
    }
    return (ret==1?i:index);
}
/*
 distribute_yomi_in_jukugoは、熟語の先頭インデックス。熟語の漢字数、読みを受け取って、各漢字に振り分ける
 */
void    distribute_yomi_in_jukugo(int index, int num, unsigned char* yomi)
{
    int yomi_num, diff,i;
    yomi_num = (int)strlen((char*)yomi);
    i = 0;
    if(num > 1){
        diff = num - (yomi_num / 3);
        switch(diff){
            case 0: while(i < num){
                strncpy((char*)kan_index[index + i]->yomi,(char*)yomi,3);
                kaeri_index[index + i]->juku = 1;
                yomi = yomi + 3;
                i++;
            }
                kaeri_index[index + i - 1]->juku = 0;
                break;
            case 1: while(i < num -1){
                strncpy((char*)kan_index[index + i]->okuri,(char*)yomi,3);
                kaeri_index[index + i]->juku = 1;
                yomi = yomi + 3;
                i++;
            }
                break;
            default:
                strcpy((char*)kan_index[index]->yomi,(char*)yomi);
        }
    }
    else{
        strcpy((char*)kan_index[index]->yomi,(char*)yomi);
    }
}
/*
 ある漢字kan_index[kan_i]に対しする読みや送り仮名（前接するカナ）もふくめてのデータ
 source_okurig(kanaindex構造体)のポインタを受け取り、他のkan_index[ip]という
 他の漢字の送り仮名から対応するものを探す。対応するものが見つかれば、０を返す。見つからなければ、-１を返す。また、それ以外にふさわしいものがあれば、１を返す。kan_nは
 複合的にカナ読みをするもの、複合再読文字（六朝以降よくみられる複合助動詞に多い）などの文字数を表す。r_or_lはカナに
 すべき文字が順読か返読かを示す。−1の時は順読、−2は返読である。
 */
int kana_to_kan(kanaindex* source_okurig, int kan_i,int kan_n, int r_or_l)
{
    static int ip, course,res;
    unsigned char* temp;
    unsigned char* temp_1;
    unsigned char* temp_okuri;
    int            temp_okuri_n, ret, con_i,post_jundoku;
    kanaindex*  okurig;
    ret    = 0;
    course = 1;
    post_jundoku = -1;
    con_i = kan_i;
    if(r_or_l > -2) course = -1;
    if(course == 1)
    {
        ip = kan_i + kan_n;
    }
    else
    {
        ip = kan_i + course;
    }
    if(r_or_l == -1)
    {
        post_jundoku = post_joji(kan_i);
        //printf("kan_i = %d: post_jundoku = %d  \n",kan_i, post_jundoku);
    }
    while(ip > -1 && kan_index[ip] != NULL){
        if(r_or_l == -1 && kan_index[ip]->kun_jun < 0) break;
            okurig = source_okurig;
        while(okurig != NULL){
            if(r_or_l == -1){//ここに判定
                if(post_jundoku != ip){
                    //printf("post_jun= %d kan_index[%d]\n",post_jundoku, ip);
                    break;
                }
            }
            if(okurig->kanayomi_n == 0)
            {
                if(!okiji_identify(okurig, kan_i,r_or_l))
                    return -1;
            }
            temp = NULL;
            temp_okuri_n = kan_index[ip]->okuri_n;
            temp_okuri = kan_index[ip]->okuri;
            if(kan_index[ip]->prop > 1){
                temp_okuri_n = temp_okuri_n - (int)kan_index[ip]->prop;
                temp_okuri = temp_okuri + (int)kan_index[ip]->prop;
            }
            if(temp_okuri_n == 0)break;
            if(temp_okuri_n < okurig->kanaindex_n){
                okurig = okurig->next;
                continue;
            }
            temp = (unsigned char*)strstr((char*)temp_okuri,(char*)okurig->kanaindex);
            if(temp != NULL){
                if(okurig->mae_setu_n == -1)
                {
                if(temp == temp_okuri)
                    {
                        if(okurig->ato == -1){
                            if(kan_index[ip]->okuri_n != okurig->kanaindex_n || kan_index[ip]->del_okuri > 0)
                            {
                                okurig = okurig->next;
                                continue;
                            }
                        }
                        if(yomi < 2)
                        distribute_yomi_in_jukugo(kan_i,kan_n,kata_to_hira(okurig->kanayomi));
                        temp_1 = temp + okurig->kanayomi_n;
                        strcpy((char*)kan_index[kan_i+kan_n-1]->okuri,(char*)temp_1);
                        *temp = '\0';
                        kan_index[kan_i+kan_n-1]->okuri_n = (int)strlen((char*)temp_1);
                        kan_index[ip]->okuri_n = (int)strlen((char*)kan_index[ip]->okuri);
                        kan_index[ip]->del_okuri = temp_okuri_n - kan_index[ip]->okuri_n;
                        if(kan_index[kan_i]->kun_jun > 0)
                            saidoku_reordering(kan_index[kan_i]->kun_jun,kan_index[ip]->kun_jun + 1, kan_n);
                        kan_index[kan_i]->kun_jun = kan_index[ip]->kun_jun + 1;
                        if(kan_n > 1)kan_index[kan_i+1]->kun_jun = kan_index[kan_i]->kun_jun+1;
                        if(kan_n > 2)kan_index[kan_i+2]->kun_jun = kan_index[kan_i+1]->kun_jun+1;
                        reordering(kan_i,kan_n);
                        return ret;
                    }
                }
                else
                {
                    if(okurig->ato == -1){
                        res = b3_strrcmp(kan_index[ip]->okuri,okurig->kanaindex);
                        if(res != 0 || kan_index[ip]->del_okuri > 0)
                        {
                            okurig = okurig->next;
                            continue;
                        }
                        else{
                            //ここに判定
                           // if(course == -1){
                           //     printf("ato: target= %d; ip = //kan_index[%d]\n",post_jundoku, ip);
                            //}//
                            if(yomi < 2)
                                strcpy((char*)kan_index[kan_i]->yomi,(char*)kata_to_hira(okurig->kanayomi));
                            kan_index[ip]->okuri_n = kan_index[ip]->okuri_n - okurig->kanayomi_n;
                            *(kan_index[ip]->okuri + kan_index[ip]->okuri_n) = '\0';
                            kan_index[ip]->del_okuri = okurig->kanayomi_n;
                            if(kan_index[kan_i]->kun_jun > 0)
                                saidoku_reordering(kan_index[kan_i]->kun_jun,kan_index[ip]->kun_jun + 1, kan_n);
                            kan_index[kan_i]->kun_jun = kan_index[ip]->kun_jun + 1;
                            if(kan_n > 1)kan_index[kan_i+1]->kun_jun = kan_index[kan_i]->kun_jun+1;
                            if(kan_n > 2)kan_index[kan_i+2]->kun_jun = kan_index[kan_i+1]->kun_jun+1;
                            if(kan_index[kan_i]->sai == 0)
                            reordering(kan_i,kan_n);
                            return ret;
                        }
                    }
                    if(yomi < 2)
                    distribute_yomi_in_jukugo(kan_i,kan_n,kata_to_hira(okurig->kanayomi));
                    temp_1 = temp + okurig->kanayomi_n + okurig->mae_setu_n;
                    strcpy((char*)kan_index[kan_i+kan_n-1]->okuri,(char*)temp_1);
                    temp = temp + okurig->mae_setu_n;
                    *temp = '\0';
                    kan_index[kan_i+kan_n-1]->okuri_n = (int)strlen((char*)temp_1);
                    kan_index[ip]->okuri_n = (int)strlen((char*)kan_index[ip]->okuri);
                    kan_index[ip]->del_okuri = temp_okuri_n - kan_index[ip]->okuri_n;
                    if(kan_index[kan_i]->kun_jun > 0)
                        saidoku_reordering(kan_index[kan_i]->kun_jun,kan_index[ip]->kun_jun + 1, kan_n);
                    kan_index[kan_i]->kun_jun = kan_index[ip]->kun_jun + 1;
                    if(kan_n > 1)kan_index[kan_i+1]->kun_jun = kan_index[kan_i]->kun_jun+1;
                    if(kan_n > 2)kan_index[kan_i+2]->kun_jun = kan_index[kan_i+1]->kun_jun+1;
                    if(kan_index[kan_i]->sai == 0)
                    reordering(kan_i,kan_n);
                    return ret;
                }
            }
        next_okurig:   okurig = okurig->next;
        }
    next_ip:  ip = ip + course;
    }
    return -1;
}
/*
 void   caution(int index, unsigned char* message); は、indexの文字が置字、または再読文字でないことを示す。
 */
void   caution(unsigned char* kan, unsigned char* message)
{
    wc_print(第);
    wd_print(bun_line_read()+1);
    wc_print(文の);
    wv_print(kan);
    wv_print(message);
    wc_print(\n);
    printf("第%d文の%s%s\n",bun_line_read() + 1, kan,message);
}
/*
kan_index[0] - [n]にある漢字の中で訓読文の漢字に対応していないもの、再読文字を探し、
 それをカナにすべき漢字のデータベースと照らし合わし、該当の漢字があれば、その読みを他の
 漢字の送り仮名の中から探す。引数は、カナにすべき漢字のデータベースkanakan構造体への
 ポインタ。なお、実際に対応づけるのは、kana_to_kan関数である。
 */
int      kanakan_identify(kanakan* kana_s)
{
    unsigned char kan[10];
    unsigned char kan_t[10];
    int         okiji_col[15];
    int         okiji_count;
    int         non_saidoku_count;
    int         find_okiji,oki_i;
    kanakan* kana;
    int   w_av_flag, w_av_f_flag, double_read;
    int   len, i, i_tmp, t_len, result, temp_i,conv_i,sub;
    okiji_count = 0;
    non_saidoku_count = 0;
    for(i = 0;i < 15; i++){
        okiji_col[i] = -1;
    }
    i = 0;
    w_av_flag = 0;
    w_av_f_flag = 0;
    while(kan_index[i] != NULL){
        okuri_identify(i, find_root_okuri());
        okiji_kaisi_identify(i, 9);
        i++;
    }
    i = 0;
    while(kan_index[i] != NULL)
    {
        len = 3;
        double_read = 0;
        if(kan_index[i]->kun_jun == -1){
            i = i + 1;
            continue;
        }
        if(kan_index[i]->kun_jun > 0 && kan_index[i]->sai == 0)
        {
            i = i + 1;
            continue;
        }
        strcpy((char*)kan, "");
        temp_i = find_hitei(i);
        conv_i = i;
        if(i < temp_i){
            double_read = 1;
            i = temp_i;
        }
        if(kan_index[i+1] != NULL){
            if(kan_index[i]->sai == 1 && kan_index[i+1]->kun_jun == 0){
                w_av_flag = 1;
                i = i + 1;
            }
        }
        if(kan_index[i]->sai == 1 && w_av_flag == 0){
            i_tmp = 1;
            result = 1;
            strcpy((char*)kan,(char*)kan_index[i]->oya);
            if(kan_index[i+1]->sai == 1)
            {
                i_tmp = i_tmp + 1;
                strcat((char*)kan, (char*)kan_index[i+1]->oya);
            }
            result = kana_to_kan(kan_index[i+i_tmp-1]->sai_yomi,i,i_tmp,-2);
         //   if(result == 0) break;
            if(result == -1)
            {
                strcpy((char*)kan_index[i+i_tmp-1]->okuri,(char*)kan_index[i+i_tmp-1]->kaeri_okuri);
                kan_index[i+i_tmp-1]->okuri_n = kan_index[i+i_tmp-1]->kaeri_okuri_n;
                kan_index[i+i_tmp-1]->sai = 0;
                strcpy((char*)kan_index[i+i_tmp-1]->kaeri_okuri,"");
                strcpy((char*)kan_index[i+i_tmp-1]->kaeri,"");
                caution(kan_index[i]->oya, (unsigned char*)"は再読文字ではないようです");
                non_saidoku_count++;
            }
            else{
                kan_index[i]->prop = 1;
            }
            i = i + i_tmp;
            continue;
        }
        if(w_av_f_flag == 1)
        {
            w_av_f_flag = 0;
            if(kan_index[i+2] != NULL && kan_index[i+2]->kun_jun == 0)i = i + 2;
        }
        if(kan_index[i]->kun_jun == 0){
            kana = kana_s;
            result = 1;
            strcpy((char*)kan,(char*)kan_index[i]->oya);
            if(kan_index[i+1]!= NULL)
            {
                if(kan_index[i+1]->kun_jun == 0) strcat((char*)kan,(char*)kan_index[i+1]->oya);
            }
            if(kan_index[i+2] != NULL){
            if(kan_index[i+2]->kun_jun == 0) strcat((char*)kan,(char*)kan_index[i+2]->oya);
            }
            while(kana != NULL){
                if(kana->jun < 0) {
                    len = (int)strlen((const char*)kana->oya_kan);
                    if(strncmp((char*)kan,(char*)kana->oya_kan,len) == 0){
                        kan_index[i]->sai_yomi = kana->yomi;
                        t_len = len/3;
                        result = kana_to_kan(kan_index[i]->sai_yomi,i,t_len,kana->jun);
                        if(result == 0) break;
                    }
                }
                kana = kana->next_kan;
            }
        break_loop:            if(result == -1) {
            i++;
            continue;}
            if(result != 0) result = odori_identify(i);
            if(result != 0){
                kana = kana_s;
                while(kana != NULL){
                    if(kana->jun > -2 && strncmp((char*)kan,(char*)kana->oya_kan,len) == 0){
                        result = 0;
                        break;
                    }
                    kana = kana->next_kan;
                }
                if(result != 0){
                    caution(kan_index[i]->oya,(unsigned char*)"は置字だと思います");
                    okiji_col[okiji_count] = i;
                    okiji_count++;
                }
            }
            else{
                kan_index[i]->prop = 1;
            }
        }
        if(w_av_flag == 1)
        {
            i = i - 2;
            w_av_flag = 0;
            w_av_f_flag = 1;
        }
        if(double_read == 1){
            i = conv_i;
        }
        else{
            len = len/3;
            i = i + len;
        }
    }
    i = kan_length;
    sub = 0;
    while(i > -1){
        if(kan_index[i]->kun_jun == 0){
            kana = kana_s;
            result = 1;
            sub = 0;
            strcpy((char*)kan,(char*)kan_index[i]->oya);
            if(i - 1 > -1)
            {
                if(kan_index[i-1]->kun_jun == 0) {
                    strcpy((char*)kan_t,(char*)kan_index[i-1]->oya);
                    strcat((char*)kan_t,(char*)kan);
                    if(i - 2 > -1)
                    {
                        if(kan_index[i-2]->kun_jun == 0){
                            strcpy((char*)kan,(char*)kan_index[i-2]->oya);
                            strcat((char*)kan,(char*)kan_t);
                        }
                        else{
                            strcpy((char*)kan,(char*)kan_t);
                        }
                    }
                }
            }
            while(kana != NULL){
                if(kana->jun > -1){
                    if(b3_strrcmp(kan,kana->oya_kan) == 0){
                    len = (int)strlen((const char*)kana->oya_kan);
                        t_len = len/3;
                        sub = -1 * (t_len - 1);
                        kan_index[i+sub]->sai_yomi = kana->yomi;
                        result = kana_to_kan(kan_index[i+sub]->sai_yomi,i+sub,t_len,-1);
                        if(result == 0) break;
                    }
                }
                kana = kana->next_kan;
            }
            if(result != 0){
                find_okiji = 0;
                for(oki_i = 0; oki_i < okiji_count; oki_i++){
                    if(okiji_col[oki_i] == i){
                        find_okiji = 1;
                        break;
                    }
                }
                if(find_okiji == 0){
                    caution(kan_index[i]->oya,(unsigned char*)"は置字だと思います");
                    okiji_count++;
                }
            }
        }
        i = i - 1 + sub;
    }
    return non_saidoku_count + okiji_count;
}
/*
 
 */
void    clear_set_kanji(void)
{
    int i;
    i = 0;
    while(kan_index[i]!=NULL)
    {
        kan_index[i]->okuri_n = 0;
        strcpy((char*)kan_index[i]->okuri,"");
        strcpy((char*)kan_index[i]->yomi,"");
        if(kan_index[i]->kun_jun > 0) kan_index[i]->kun_jun = 0;
        i++;
    }
}
/*
 次のset_kanjiはread_buf_kanで読み込まれた漢字をkan_indexに納めていく。戻り値はkan_index[0]への
 ポインタである。
 */
oyakan* set_kanji(unsigned char* stream)
{
    int             i, com_flag;
    oyakan*         kanji_1;
    kaeriten*       kaeri;
    kanlist*        temp_list;
    kanakan*        temp_kanakan;
    kan_chain_list* temp_kan_chain;
    comment*        temp_comment;
    unsigned char   temp_label[20];
    unsigned char*  bp;
    unsigned char*  temp_bp;
    unsigned char*  j_char;
    char         *  k_mode;
    int             jun, juf_n, code_n, line_flag;
    long int        temp_prop;
    char            line_mode;
    chofuku_n = 0;
    jun = 0;
    com_flag = 0;
    bp = stream;
    line_flag = 0;
    line_mode = 'x';
    //space = 1;
    temp_comment = NULL;
    index_init();
    if(stream == NULL) return NULL;
    while(*bp != '\0')
    {
        j_char = get_one_char_stream( bp );
        if(j_char[0]== '('){
            bp++;
            temp_comment = malloc(sizeof(comment));
            i = 0;
            while(*bp != ')'){
                temp_label[i] = *bp;
                bp++;
                i++;
            }
            temp_label[i] = '\0';
            if(temp_label[0] < 0x7B && temp_label[0] > 0x40){
                if(temp_label[1] < 0x78 && temp_label[1] > 0x40){
                    strncpy(temp_comment->mark,(char*)temp_label,2);
                    temp_comment->mark[2] = '\0';
                    j_char = temp_label;
                    j_char = j_char + 2;
                }
                else{
                    set_label_form(temp_comment->mark);
                    j_char = temp_label;
                }
            }
            else
            {
                set_label_form(temp_comment->mark);
                j_char = temp_label;
            }
            strcpy((char*)temp_comment->comment, (char*)j_char);
            continue;
        }
        if(*j_char == '{'){
            bp++;
            if(*bp < 0x7B && *bp > 0x40){
                line_mode = *bp;
                bp++;
            }
            else{
                line_mode = 'b';
            }
            continue;
        }
        if(*j_char == '}'){
            line_mode = 'x';
            if(jun > 0){
                kan_index[jun - 1]->b_end = 1;
            }
            bp++;
            continue;
        }
        if(*j_char == '\n'){
            if(jun > 0){
                kan_index[jun-1]->interval = kan_index[jun-1]->interval * 3;
            }
            bp++;
            continue;
        }
        if(*j_char == '\t'){
            if(jun > 0){
                kan_index[jun-1]->interval = kan_index[jun-1]->interval * 5;
            }
            bp++;
            continue;
        }
        if(*j_char == '-'){
            if(jun > 0){
                kaeri_index[jun-1]->juku = 1;
            }
            bp++;
            continue;
        }
        if(*j_char == 0x20){
            if(jun > 0){
                //space = space * 2;
                kan_index[jun-1]->interval = kan_index[jun-1]->interval * 2;
            }
            bp++;
            continue;
        }
        if(j_char[0] == 'x' || j_char[0] == 'X'){
            if(jun > 0){
                kan_index[jun-1]->gtest = 10001;
            }
            bp++;
            continue;
        }
        if((k_mode = strchr((const char*)kun_mode,*j_char)) != NULL)
        {
            if(jun > 0)kan_index[jun-1]->kunten_mode = (int)(k_mode-kun_mode);
            bp++;
            continue;
        }
        if((code_n = one_character(*j_char)) < 3) {
            bp = bp + code_n;
            continue;
        }
        code_n = is_utf_kan_or_yaku(j_char);
        if(code_n == 0) {
            code_n = one_character(*j_char);
            bp = bp + code_n;
            continue;
        }
        if(code_n > 1 && code_n < 6){
            //space = 1;
            kanji_1 = (oyakan*)malloc(sizeof(oyakan));
            if(kanji_1 == NULL){
                printf("メモリが足りません。\n");
                return NULL;
            }
            kaeri = (kaeriten*)malloc(sizeof(kaeriten));
            if(kaeri == NULL){
                printf("メモリが足りません。\n");
                return NULL;
            }
            if(code_n == 5){
                strncpy((char*)kanji_1->oya,get_con_seq(1),3);
                kanji_1->oya[3] = '\0';
                kanji_1->kun_jun = -1;
                code_n = 3;
            }
            else{
                if(code_n == 2){
                    strncpy((char*)kanji_1->oya, (char*)j_char,3);
                    kanji_1->oya[3] = '\0';
                    kanji_1->kun_jun = -1;
                    code_n = 3;
                }
                else{
                    strncpy((char*)kanji_1->oya, (char*)j_char,code_n);
                    kanji_1->oya[code_n] = '\0';
                    kanji_1->kun_jun = 0;
                }
            }
            kaeri->juku = 0;
            kaeri->temp_ban = 0;
            kaeri->code[0] = 0;
            kaeri->code[1] = 0;
            kaeri->code[2] = 0;
            kaeri->code[3] = 0;
            kaeri->code[4] = 0;
            kaeri->code[5] = 0;
            kaeri->code[6] = 0;
            juf_n = 0;
            if(kanji_1->kun_jun == 0){
                temp_bp = bp + code_n;
                while((temp_bp = mb_strstr(temp_bp, j_char)) != NULL){
                    juf_n--;
                    temp_bp = temp_bp + code_n;
                }
            }
            kanji_1->jufuku = juf_n;
            strcpy((char*)kanji_1->yomi,"");
            strcpy((char*)kanji_1->okuri, "");
            strcpy((char*)kanji_1->kaeri_okuri, "");
            strcpy((char*)kanji_1->kaeri, "");
            strcpy((char*)kanji_1->kaeri_ten, "");
            kanji_1->label = temp_comment;
            kanji_1->prop_p[0] = NULL;
            kanji_1->prop_p[1] = NULL;
            kanji_1->prop_p[2] = NULL;
            kanji_1->prop_p[3] = NULL;
            kanji_1->prop_p[4] = NULL;
            kanji_1->prop_p[5] = NULL;
            kanji_1->prop_p[6] = NULL;
            kanji_1->kaeri_okuri_n = 0;
            kanji_1->okuri_n = 0;
            kanji_1->del_okuri = 0;
            kanji_1->interval = 1;
            kanji_1->is_dosi = 0;
            kanji_1->gtest = 0;
            kanji_1->kunten_mode = read_haku_moji_mode(1);
            kanji_1->sai_yomi = NULL;
            kanji_1->mod = line_mode;
            kanji_1->b_end = 0;
            kanji_1->sai = 0;
            temp_prop = 0;
            temp_list = find_fukusi();
            while(temp_list != NULL){//副詞で「ニ」などを送るものを判定します。
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[0] = temp_list;
                    temp_prop = temp_prop | HUKUSI;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_hendoku_moji();
            while(temp_list != NULL){//通常の動詞以外の返読文字を判定します。
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[1] = temp_list;
                    temp_prop = temp_prop | HENDOKU;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_hendoku_moji2();
            //同じ返読文字を別の読み方で用いる場合、訓読文の文法的な性質が異なるための補助的なタグ
            while(temp_list != NULL){
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[5] = temp_list;
                    temp_prop = temp_prop | HENDOKU2;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_hendoku_moji3();
            //同じ返読文字を別の読み方で用いる場合、訓読文の文法的な性質が異なるための補助的なタグ
            while(temp_list != NULL){
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[6] = temp_list;
                    temp_prop = temp_prop | HENDOKU3;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_hendoku_moji4();
            //同じ返読文字を別の読み方で用いる場合、訓読文の文法的な性質が異なるための補助的なタグ
            while(temp_list != NULL){
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[7] = temp_list;
                    temp_prop = temp_prop | HENDOKU4;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_hendoku_moji5();
            //同じ返読文字を別の読み方で用いる場合、訓読文の文法的な性質が異なるための補助的なタグ
            while(temp_list != NULL){
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[8] = temp_list;
                    temp_prop = temp_prop | HENDOKU5;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_root_daisi();
            while(temp_list != NULL){//代詞かどうかを判定します。
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[2] = temp_list;
                    temp_prop = temp_prop | DAISI;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_root_kaisi();
            while(temp_list != NULL){//特定の介詞を判定します。
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[3] = temp_list;
                    temp_prop = temp_prop | KAISI;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_list = find_no_hendoku();
            while(temp_list != NULL){//返読文字と紛らわしい漢字を登録しています。
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    kanji_1->prop_p[4] = temp_list;
                    temp_prop = temp_prop | NONHENDOKU;
                    break;
                }
                temp_list = temp_list->next;
            }
            temp_kan_chain = find_hitei_hukusi();
            while(temp_kan_chain != NULL)
            {
                if(!kanji_cmp(kanji_1->oya, temp_kan_chain->listed_kan)){
                    temp_prop = temp_prop | HITEI;
                    break;
                }
                temp_kan_chain = temp_kan_chain->next;
            }
            temp_kanakan = find_root_kanakan();
            while(temp_kanakan != NULL){// ここでは、訓読文に漢字として現れない可能性のある語をマークします。
                if(!kanji_cmp(kanji_1->oya, temp_kanakan->oya_kan)){
                    temp_prop = temp_prop | OKI_KANA;
                    break;
                }
                temp_kanakan = temp_kanakan->next_kan;
            }
            temp_kan_chain = find_siyeki();
            while(temp_kan_chain != NULL){
                if(!kanji_cmp(kanji_1->oya,temp_kan_chain->listed_kan)){
                    temp_prop = temp_prop | SIYEKI;
                    break;
                }
                temp_kan_chain = temp_kan_chain->next;
            }
            temp_list = find_root_okikana();
            while(temp_list != NULL)
                {//文中で他の語に挟まれて、置き字やひらがなになる漢字を判定します。
                if(!kanji_cmp(kanji_1->oya,temp_list->listed_kan)){
                    temp_prop = temp_prop | KORE;
                    break;
                }
                temp_list = temp_list->next;
            }
            if(temp_prop == 0) temp_prop = 1;
            kanji_1->prop = -temp_prop;
            temp_comment = NULL;
            kan_index[jun]      = kanji_1;
            kaeri_index[jun]    = kaeri;
            jun++;
            bp = bp + code_n;
        }

    }
    chofuku_n = choufuku();
    kan_length = length_kan_index();
    return kan_index[0];
}
/*
 白文ファイルを読み込んでバッファに蓄える、引数は白文ファイルへのファイルポインタ、戻り値はバッファへのポインタ
 */
unsigned char* read_buf_kan( FILE* i_file, FILE* o_file)
{
    unsigned char* temp;
    unsigned char* bp_s;
    unsigned char* bp_e;
    //int         read_num;
    char        temp_ascii;
    int   flag, ku_flag,label_flag,line_end,kan_num;
    ku_flag = 0;
    label_flag = 0;
    kan_num = 0;
    line_end = 1;
    bp_s = input_buf_haku;
    bp_e = input_buf_haku;
    init_buf(bp_s, HAKU_BUF);
    if(cont_flag > 0){
        if(temp_char_kan[0] == '(')label_flag = 1;
        strncpy((char*)bp_e,(char*)temp_char_kan,cont_flag);
        bp_e = bp_e + cont_flag;
        cont_flag = 0;
    }
    temp_char_kan[0] = '\0';
    while( 1 ){
        if(kan_haku_flag == 0){
            temp = get_one_char( i_file, temp_char_kan);
            if(temp == NULL) {
                if(read_read_num() == 0)
                {
                    wc_print(訓点を打つべき白文がありません\n\n);
                    wc_print(白文のはじまりには(全角でも半角でも)を終わりに>書き込んでください\n);
                    printf("訓点を打つべき白文がありません。白文のはじまりには、\n");
                    printf("<(全角でも半角でも)を、終わりに>書き込んでください。\n");
                }
                file_end_flag = 1;
                return NULL;
            }
            flag = is_utf_angle(temp);
            if(*temp == '<' || flag == 2){
                kan_haku_flag = 1;
                add_read_num();
                temp_char_kan[0] = '\0';
                bp_s = input_buf_haku;
                bp_e = input_buf_haku;
                init_buf(bp_s, HAKU_BUF);
                cont_flag = write_haku_moji_mode(i_file, temp_char_kan);
                opening_kan(o_file);
                return NULL;
                //continue;
            }
            line_end = read_text(temp, line_end);
            if(line_end == 1){
                interpret_tex(o_file, get_ori_line());
            }
            temp_char_kan[0] = '\0';
            continue;
        }
        temp = get_one_char( i_file, temp_char_kan);
        if(temp == NULL) {
            if(bp_e == bp_s){
                kan_haku_flag = 0;
                file_end_flag = 1;
                return NULL;
            }
            else{
                file_end_flag = 1;
                break;
            }
        }
        temp = han_yaku_normalize(temp);
        flag = is_utf_char( temp );
        if(flag == 0){
            temp_char_kan[0] = '\0';
            continue;
        }
        if(flag > 40){
            if(label_flag){
                strncpy((char*)bp_e, (char*)temp,3);
                bp_e = bp_e + 3;
            }
            temp_char_kan[0] = '\0';
            continue;
        }
        if(flag == 1 || flag == 5){
            temp_ascii = '\0';
            temp_ascii = b3_to_ascii(temp);
            temp_char_kan[0] = '\0';
            switch(temp_ascii){
                case '>':
                    kan_haku_flag = 0;
                    goto  end;
                    break;
                case '\n': if(input_buf_haku == bp_e) break;
                    break;
                case '(':
                    if(ku_flag == 1){
                        cont_flag = 1;
                        strcpy((char*)temp_char_kan,"(");
                        return bp_s;
                    }
                    label_flag = 1;
                    break;
                case ')': label_flag = 0;
                    break;
                case '{':
                    if(ku_flag == 1){
                        cont_flag = 1;
                        strcpy((char*)temp_char_kan,"{");
                        return bp_s;
                    }
                default: break;
            }
            *bp_e = temp_ascii;
            bp_e++;
            continue;
        }
        if(flag == 6){
            *bp_e = '-';
            temp_char_kan[0] = '\0';
            bp_e++;
            continue;
        }
        if(flag == 34 ){//括弧を開く
            kakko_u++;
            strncpy((char*)bp_e, (char*)temp,3);
            temp_char_kan[0] = '\0';
            bp_e = bp_e + 3;
            continue;
        }
        if(flag == 17 ){//括弧を閉じる
            kakko_u--;
            strncpy((char*)bp_e, (char*)temp,3);
            temp_char_kan[0] = '\0';
            bp_e = bp_e + 3;
            if(ku_flag == 1){
                break;
            }
            continue;
        }
        if(flag == 13){//句点の処理
            ku_flag = 1;
            strncpy((char*)bp_e, (char*)temp,3);
            temp_char_kan[0] = '\0';
            bp_e = bp_e + 3;
            if(kakko_u == 0){
                break;
            }
            continue;
        }
        if(flag > 6 && flag < 12){//句点以外の約物の処理
            strncpy((char*)bp_e, (char*)temp,3);
            temp_char_kan[0] = '\0';
            bp_e = bp_e + 3;
            continue;
        }
        if(flag > 2 && flag < 5){
            kan_num++;
            if(ku_flag == 1){
                cont_flag = flag;
                break;
            }
            else{
                strncpy((char*)bp_e, (char*)temp,flag);
                bp_e = bp_e + flag;
                continue;
            }
        }
    }
end:    if(kan_num == 0) return 0;
        bp_e = ending_process(i_file, bp_e);
        if(bp_e == NULL) return NULL;
        return bp_s;
}
/*
 書き下し文ファイルを読み込んでバッファに蓄える、引数は書き下し文ファイルへのファイルポインタ、戻り値はバッファへのポインタ
 */
unsigned char* read_buf_kun( FILE* i_file)
{
    unsigned char* temp;
    unsigned char* bp_s;
    unsigned char* bp_e;
    unsigned char        temp_ascii;
    int         flag, ku_flag;
    int         end_kakko_flag,yomi_flag,sakun_flag;
    ku_flag = 0;
    yomi_flag = 0;
    sakun_flag = 0;
    end_kakko_flag = 0;
    init_buf((unsigned char*)input_buf_kun, KUN_BUF);
    bp_s = input_buf_kun;
    bp_e = input_buf_kun;
    if(cont_kun_flag > 0){
        strncpy((char*)bp_s,(char*)temp_char_kun,cont_kun_flag);
        bp_e = bp_e + cont_kun_flag;
        cont_kun_flag = 0;
    }
    temp_char_kun[0] = '\0';
    while( 1 )
    {
        temp = get_one_char( i_file, temp_char_kun);
        if(temp == NULL){
            if(bp_s != bp_e){
                break;
            }
            else{
                return NULL;
            }
        }
        temp = han_yaku_normalize(temp);
        flag = is_utf_char( temp );
        if(flag == 0){
            temp_char_kun[0] = '\0';
            continue;
        }
        if(flag == 1 || flag == 5){//ASCII記号の処理（全角も含めて）
            temp_ascii = '\0';
            temp_ascii = b3_to_ascii(temp);
            temp_char_kun[0] = '\0';
            switch(temp_ascii){
                case '(': yomi_flag = 1;
                    *bp_e = temp_ascii;
                    bp_e++;
                    break;
                case ')': yomi_flag = 0;
                    *bp_e = temp_ascii;
                    bp_e++;
                    break;
                case '[': sakun_flag = 1;
                    *bp_e = temp_ascii;
                    bp_e++;
                    break;
                case ']': sakun_flag = 0;
                    *bp_e = temp_ascii;
                    bp_e++;
                    break;
                default:
                    break;
            }
            continue;
        }
        if(flag < 10 && flag > 5){//句読点、括弧以外の約物の処理
            //strncpy((char*)bp_e,(char*)temp,3);
            temp_char_kun[0] = '\0';
            //bp_e = bp_e + 3;
            continue;
            }
        if(flag == 34){//括弧を開く
            kakko_kun_u++;
            //strncpy((char*)bp_e, (char*)temp,3);
            temp_char_kun[0] = '\0';
            //bp_e = bp_e + 3;
            continue;
        }
        if(flag == 17 ){//括弧を閉じる
            kakko_kun_u--;
            if(ku_flag) end_kakko_flag = 1;
            //strncpy((char*)bp_e, (char*)temp,3);
            temp_char_kun[0] = '\0';
            //bp_e = bp_e + 3;
            continue;
        }
        if(flag == 41){ //カタカナの場合
            if(yomi_flag || sakun_flag){
                temp = kata_to_hira(temp);
            }
        }
        if(flag == 10){
            strncpy((char*)bp_e, (char*)temp,3);
            bp_e = bp_e + 3;
            continue;
        }
        if(flag == 43){//平仮名の場合
            if(!yomi_flag && !sakun_flag){
                temp = hira_to_kata(temp);
            }
            if(ku_flag){
                if(end_kakko_flag){
                    strncpy((char*)bp_e, (char*)temp,3);
                    temp_char_kun[0] = '\0';
                    bp_e = bp_e + 3;
                    continue;
                }
                else
                {
                    cont_kun_flag = 3;
                    break;
                }
            }
            else{
                strncpy((char*)bp_e, (char*)temp,3);
                temp_char_kun[0] = '\0';
                bp_e = bp_e + 3;
                continue;
            }
        }
        if(flag > 2 && flag < 5){
            if(ku_flag){
                if(end_kakko_flag){
                strncpy((char*)bp_e, (char*)temp,flag);
                temp_char_kun[0] = '\0';
                bp_e = bp_e + flag;
                continue;
                }
                else
                {
               // strncpy((char*)temp_char_kun,(char*)temp,flag);
               // temp_char_kun[flag] = '\0';
                cont_kun_flag = flag;
                break;
                }
            }
            else{
            strncpy((char*)bp_e, (char*)temp,flag);
            temp_char_kun[0] = '\0';
            bp_e = bp_e + flag;
            continue;
            }
        }
        if(flag == 13){
            //strncpy((char*)bp_e, (char*)temp,3);
            temp_char_kun[0] = '\0';
            //bp_e = bp_e + 3;
            if(!kakko_kun_u) break;
            if(end_kakko_flag) break;
            ku_flag = 1;
            continue;
        }
        if(flag == 11) {
            if(ku_flag == 1) break;
        }
    }
end:
        *bp_e = '\0';
        return bp_s;
    
}
/*
 gtestに書き込みを行う。
 */
void write_gtest(unsigned char* kun)
{
    int temp_jun,temp_index,ret;
    unsigned char*      temp_kun;
    temp_jun = 1;
    temp_index = 0;
    temp_kun = kun;
    while(kan_index[temp_index] != NULL)
    {
        if(kan_index[temp_index]->kun_jun == 1){
            break;
        }
        temp_index++;
    }
    while(temp_kun != NULL && temp_index > -1){
        ret = duplicated_kan(temp_index, temp_jun, temp_kun);
        if(ret == 0){
            kan_index[temp_index]->gtest = 444;
        }
        else{
            kan_index[temp_index]->gtest = -90 - ret;
        }
        temp_index = preceeding_kan(temp_jun, -1);
        temp_kun = del_kanji(temp_kun);
        temp_kun = next_kanji(temp_kun);
        temp_jun++;
    }
}

/*
 kan_indexの訓順を0とする。
 */
void    kunjun_clear(void)
{
    int     i, len;
    i = 0;
    while(kan_index[i] != NULL)
    {
        if(kan_index[i]->kun_jun > -1){
            kan_index[i]->kun_jun = 0;
        }
        len = (int)strlen((char*)kan_index[i]->yomi);
        if(len > 0) strcpy((char*)kan_index[i]->yomi,"");
        i++;
    }
}

/*
 int haku_kuten_error( void )は白文の句点の位置がおかしいこと判断する。
 
 */
int     haku_kuten_error( void )
{
    int               i, kutou;
    unsigned char*    haku_string;
    kutou = kan_length - 1;
    i = kutou;
    while(i > 0 && kan_index[i]->kun_jun < 1) i--;
    if(kutou - i > 3){
        haku_string = kanindex_to_str(i, 0);
        return 1;
    }
    else{
        return 0;
    }
}
/*
 仮においたkun_junの値を０に直す。
 */
void    kunjun_setup(void)
{
    int     i;
    i = 0;
    while(kan_index[i] != NULL)
    {
        if(kan_index[i]->kun_jun < -1)
            kan_index[i]->kun_jun = 0;
        i++;
    }
}
/*
 次の関数は、assign_kan_indexは訓読文に現れる漢字をkan_index上の漢字に当てはめる。引数は訓読文の文字列に対するポインタ。
 戻り値はエラーの数。
 */

int     assign_kan_index( unsigned char* kun_ori )
{
    int     errors, error_jun, temp_index, ret_index, temp_jun, code,i;
    int     ori_hogo_rev, penalty;
    int     debug_ret;
//    int     sw1, sw2;
    pair*   swap;
//    oyakan* temp_kan;
    // static int     temp_error_point;
    int     j_flag;
    unsigned char* kun;
    num_of_kaisi = -1;
    siyeki_on = 0;
    errors = 0;
    error_jun = 0;
    temp_jun = 1;
    j_flag = 0;
    if(hogo_rev != 1){
        ori_hogo_rev = hogo_rev;
    }
    else{
        ori_hogo_rev = 0;
    }
    kun = kun_ori;
    while( kun != NULL && *kun != '\0')
    {
        kun = next_kanji(kun);
        if(kun == NULL || *kun == '\0') break;
        code = is_utf_kanji(kun);
        temp_index = find_kan( kun );
        if(kun != NULL && temp_index < 0){
            write_error_kun(kun);
            if(read_kuten_error()){
                return -1;
            }
            else{
                return -9;
            }
        }
        if(kan_index[temp_index]->jufuku > 4 && kan_index[temp_index]->gtest < 10000 && kan_index[temp_index]->jufuku < MAX_PRIME){
            j_flag++;
            if(err_count > 0 && !is_exhausted_dup_kan(temp_index)){
                ret_index = temp_index;
            }
            else
            {
                ret_index = duplicate_case(temp_index,temp_jun,kun);
                if(ret_index < 0){
                    if(temp_error_point == temp_index){
                        basic_error++;
                        error_count++;
                    }
                    else{
                        temp_error_point = temp_index;
                        error_count = 0;
                    }
                    if(error_count > 4) return 1;
                    if(read_verbose() > 0)
                    printf("error_count = %d\n",error_count);
                    errors++;
                    printf("\x1b[31m");
                    printf("第%d文で、",bun_line_read()+1);
                    printf("次の漢字に問題が生じています:%s\x1b[0m\n",kan_index[temp_index]->oya);
                    wc_print(第);
                    wd_print(bun_line_read()+1);
                    wc_print(文で次の漢字に問題が生じています: );
                    wv_print(kan_index[temp_index]->oya);
                    wc_print(\n);
                    i = 0;
                    while(kan_index[i]!= NULL)
                    {
                        if(i == temp_index){
                            wc_print(<<);
                            wv_print(kan_index[i]->oya);
                            wc_print(>>);
                            printf("\x1b[31m%s \x1b[0m",kan_index[i]->oya);
                        }
                        else{
                            wv_print(kan_index[i]->oya);
                            printf("%s",kan_index[i]->oya);
                        }
                        i++;
                    }
                    wc_print(\n);
                    printf("\n");
                    kun = kun + code;
                    kun = next_kanji(kun);
                    continue;
                }
                if(ret_index == temp_index && is_exhausted_dup_kan(temp_index) > 0){
                    ret_index = find_double_kanji(temp_index, 1);
                    swap_jufuku_kan_index(temp_index, ret_index);
                }
                else{
                    temp_index = ret_index;
                }
            }

        }
       if(g_test_read()){
           if(non_done == 0){
               debug_ret = duplicated_kan(temp_index, temp_jun, kun);
               if(debug_ret == 0){
                   kan_index[temp_index]->gtest = 444;
               }
               else{
                   kan_index[temp_index]->gtest = -90 - debug_ret;
               }
           }
           else{
               kan_index[temp_index]->gtest = 999;
           }
        }
        kan_index[temp_index]->kun_jun = temp_jun;
        temp_jun++;
        kun = kun + code;
        kun = kun_kana(kun, temp_index);
    }
    if(ori_hogo_rev > 0){
        hogo_rev = ori_hogo_rev;
    }
    write_kuten_error(haku_kuten_error());
    if(chofuku_n > 1) {
        if(!errors){
            error_jun = check_kunjun();
            if(read_verbose() > 0){
                printf("Is the order of kanji-reading possible from the point of assignment of kaeriten? \n'0' is OK, '1' is impossible. In this case %d \n", error_jun);
            }
            debug_ret = 0;
            while(error_jun != 0 && debug_ret < 3){
                printf("Error %d\n", error_jun);
                erronious_point_in_kunjun();
                swap = swap_pair_return();
                if(read_verbose() > 0){
                    print_dubious_kaeri();
                }
                wc_print(読み方の順が理論的にあり得ない順となっています\n);
                wc_print(このままでは返点が打てないので漢字);
                wv_print(kan_index[swap->post]->oya);
                wc_print(の第);
                wd_print(swap->post);
                wc_print(と第);
                wd_print(swap->pre);
                wc_print(番を逆にします\n);
                printf("読み方の順が理論的にあり得ない順となっています。\n");
                printf("このままでは、返点が打てないので、漢字%sの%d番と%d番を逆にします。\n",kan_index[swap->post]->oya, swap->post, swap->pre);
                swap_kan_index(swap->post, swap->pre);
                error_jun = check_kunjun();
                debug_ret++;
            }
        }
    }
    penalty = penalty_count();
    if(penalty < 0)
    {
        caution(kanindex_to_str(0, 0),(unsigned char*)"\nという白文の語順の確定に大きな問題があるようです\n");
        wc_print(打たれた訓点をよく確認してください おかしい場合 おかしい訓点(返点)が打たれた漢字にローマ字の'x'をつけて\n);
        wc_print(再度このプログラムを通してみてください\n);
        printf("打たれた訓点をよく確認してください おかしい場合 おかしい訓点(返点)が打たれた漢字にローマ字の'x'をつけて\n");
        printf("再度このプログラムを通してみてください\n");
        if(read_verbose()>0)
        printf("Penalty = %d: So many problems occur in the sentence No. %d\n", penalty, bun_line_read() + 1);
    }
    else{
        if(read_verbose()>0)
        printf("Penalty = %d: Not so many problems occur in the sentence No. %d\n", penalty, bun_line_read() + 1);
    }
    return errors;
}
/*
 force_swapping(int chofuku,int error)は、chofuku_nと現在のerrorを受け取って、errorの
 数を返す。全てが交換不可能になれば、chofuku_nの数をerrorの数として返す。
 */
int force_swapping(int chofuku,int error, int pre_error)
{
    int     prime_index,i,temp_prime_num,ret_chofuku,temp_chofuku;
    if(error < pre_error + 1) return error;
    prime_index = 3;
    temp_chofuku = chofuku + 3;
    while(prime_index < temp_chofuku){
        i = 0;
        temp_prime_num = prime_num(prime_index,49,1);
        while(kan_index[i]!= NULL){
            if(kan_index[i]->jufuku == temp_prime_num){
                ret_chofuku = find_double_kanji(i, 1);
                if(ret_chofuku < 0) break;
                swap_jufuku_kan_index(i, ret_chofuku);
                error++;
                return error;
            }
            i++;
        }
        prime_index++;
    }
    return chofuku;
}
/*
 以下が、これらを以上を統合して訓点を完成させる関数である。引数は、訓読文の文字列へのポインタ。戻り値はエラーの数。
 最終的にこれが0であれば、一応の
 */
int     process_kan(unsigned char* kun )
{
    int ret, count,f_chofuku,temp_chofuku;
    non_done = 0;
    hogo_rev = 1;
    basic_error = 0;
    error_count = 0;
    temp_error_point = -1;
start:
    ret = 0;
    err_count = 0;
    count = 0;
    if(chofuku_n > 4){
        temp_chofuku = 4;
    }
    else{
        temp_chofuku = chofuku_n;
    }
    f_chofuku = factorial(temp_chofuku-1);
    while(err_count < f_chofuku)
    {
        if(basic_error > 10000){
            wc_print(問題が生じていてこのプログラムは中止します\n);
            printf("問題が生じていてこのプログラムは中止します\n");
            return 1;
        }
        ret = assign_kan_index(kun);
        if(ret < 0){
            if(ret < -1){
                caution(kanindex_to_str(0, 0),(unsigned char*)"\nという白文に対する訓読文の句点が一致していないか\n訓読文と白文で違う漢字が使われている可能性があります\n確認してい下さい");
                wc_print(訓読文は以下の通りです\n);
                wv_print(read_temp_error_kun());
                wc_print(\n);
                printf("訓読文は以下の通りです。\n");
                printf("%s\n",read_temp_error_kun());
            }
            else{
                caution(read_temp_error_str(),(unsigned char*)"\nという白文に句点が一致していないか\n訓読文と白文で違う漢字が使われている可能性があります\n");
                wc_print(対応する訓読文は次のものです\n);
                wv_print(read_temp_error_kun());
                wc_print(\n確認してみてください\n);
                printf("対応する訓読文は以下のものです。\n%s\n",read_temp_error_kun());
            }
            wc_print(ここで処理を中断します\n);
            printf("ここで処理を中断します\n");
            return ret;
        }
        if(ret == 100) break;
        if(ret != 0 && non_done > 3){
            if(temp_error_point > -1){
                printf("次の漢字が最終的にはまりませんでした: %s \n", kan_index[temp_error_point]->oya);
                temp_error_point = is_kore_joji_simple(temp_error_point);
                kan_index[temp_error_point]->gtest = 1;
            }
            caution(kanindex_to_str(0, 0),(unsigned char*)"\nという白文の訓点が解析できませんでした。\n");
            if(non_done < 20){
                non_done++;
                kunjun_clear();
                goto start;
            }
            else{
                return ret;
            }//return ret;
        }
        if(ret == 0) {
            if(err_count > 0){
                wd_print(err_count);
                wc_print(回目に成功しました\n);
                printf("%d回目に成功しました\n",err_count);
            }
            if(non_done > 3)
            {
                wd_print(temp_error_point);
                wc_print(番目の漢字<<);
                printf("%d番目の漢字<<",temp_error_point);
                if(temp_error_point > -1 && kan_index[temp_error_point] != NULL)
                {
                    wv_print(kan_index[temp_error_point]->oya);
                    printf("%s",kan_index[temp_error_point]->oya);
                }
                wc_print(>>を当てはめました\n);
                printf(">>を当てはめました\n");
            }
                break;
        }
        err_count++;
        clear_set_kanji();
        err_count = force_swapping(chofuku_n, err_count, ret);
    }
    if(ret != 0 ){
        if(non_done < 4){
            if(non_done == 0){
                org_file_message_with_int((unsigned char*)"temp_error_point = ", temp_error_point, (unsigned char*)"");
                out_put_kan_index(kanindex_to_str(0, 0), kun, bun_line_read()+1);
            }
            error_count = 0;
            temp_error_point = -1;
            non_done = non_done + 1;
            goto start;
        }
        else{
            wc_print(訓点が妥当ではないかもしれません\n);
            printf("訓点が妥当ではないかもしれません\n");
        }
    }
    if(read_verbose() > 0){
        kaeri_print();//For Debug
    }
    kunjun_setup();
    saidoku_identify(find_root_saidoku());
    non_sai_okiji_count = kanakan_identify(find_root_kanakan());
    refine_saidoku();
    prepear_code();
//#ifdef DEBUG_4
//#endif
    return ret;
}
