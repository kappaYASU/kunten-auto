//
//  utf_string.c
//  kunten
//
//  Created by 岡崎康浩 on 2023/05/28.
//

#include <stdio.h>
#include <string.h>
#include "oyakan.h"

#define S_KANJI_E   {0xF0,0x9F,0xAF,0xBF,'\0'}
#define E_KANJI_E   {0xF0,0xB1,0x8D,0x8B,'\0'}
#define S_KANJI_P   {0xEF,0xA3,0xBF,'\0'}
#define E_KANJI_P   {0xEF,0xAC,0x80,'\0'}
#define S_KANJI     {0xE3,0x8F,0xBF,'\0'}
#define E_KANJI     {0xE9,0xBF,0xAF,'\0'}
#define S_HIRA      {0xE3,0x81,0x80,'\0'}
#define E_HIRA      {0xE3,0x82,0xA0,'\0'}
#define S_KATA      {0xE3,0x82,0xA0,'\0'}
#define E_KATA      {0xE3,0x83,0xBA,'\0'}
#define S_ASCII     {0xEF,0xBC,0x80,'\0'}
#define E_ASCII     {0xEF,0xBD,0x9F,'\0'}

unsigned char kan_s[4] = S_KANJI;
unsigned char kan_e[4] = E_KANJI;
unsigned char kan_p_s[4] = S_KANJI_P;
unsigned char kan_p_e[4] = E_KANJI_P;
unsigned char kan_e_s[5] = S_KANJI_E;
unsigned char kan_e_e[5] = E_KANJI_E;
unsigned char hira_s[4] = S_HIRA;
unsigned char hira_e[4] = E_HIRA;
unsigned char kata_s[4] = S_KATA;
unsigned char kata_e[4] = E_KATA;
unsigned char ascii_s[4] = S_ASCII;
unsigned char ascii_e[4] = E_ASCII;

unsigned char temp_char[5];
unsigned char temp_buf_kun[2048];
unsigned char temp_error_str[512];
unsigned char temp_error_kun[2048];


char ending_chars[12] = {" })].,?\\\n\r\t"};
unsigned char* get_huku_josi(int i);

void           write_file_end_flag( int num );
/*
 temp_error_strの読み出し
 */
unsigned char* read_temp_error_str(void)
{
    return (unsigned char*)temp_error_str;
}
/*
 次の関数はtemp_error_kunへの読み書きをする。
 */
void    write_error_kun( unsigned char* s)
{
    strcpy((char*)temp_error_kun, (char*)s);
}
unsigned char* read_temp_error_kun( void )
{
    return (unsigned char*)temp_error_kun;
}
/*
 文字列strのバイト数を初めのバイトで判定する。戻り値は文字のバイト数。
 */
int one_character(unsigned char str)
{
    if(str < 0x80) return 1;
    if(str < 0xE0) return 2;
    if(str < 0xF0) return 3;
    return 4;
}

unsigned char* get_one_char( FILE* i_file, unsigned char* temp)
{
    unsigned char lit;
    int     code, i;
    lit = getc(i_file);
    if((signed char)lit == EOF) return NULL;
    code = one_character( lit );
    temp[0] = lit;
    for(i = 1; i < code; i++){
        lit = getc(i_file);
        temp[i] = lit;
    }
    temp[code] = '\0';
    return temp;
}
/*
 bpから一文字分（１〜４バイトまで）を切り出してtemp_charに納める。戻り値はtemp_charのポインター、
 なお操作を行った場合、bpはその文字を除いたものとなる。
 */
unsigned char* get_one_char_stream(unsigned char* bp)
{
    int code,i;
    temp_char[0] = '\0';
    code = one_character(*bp);
    for(i = 0; i < code; i++){
        temp_char[i] = *bp;
        bp++;
    }
    temp_char[i] = '\0';
    return temp_char;
}
/*
 kanindex_to_str(int start, int end)は、kan_indexに収められた漢字から白文の文字列を生成する。
 戻り値は、文字列 unsigned charポインタ、startは初めのkan_indexへのインデックス、endは最後のインデックス。
 endに０を指定するか、または、最大読み込み白文以上（１００以上）を指定すると、文末までの文字列とする。
 */
unsigned char* kanindex_to_str(int start, int end)
{
    int     i;
    char* temp_s;
    temp_s = (char*)temp_error_str;
    if(end == 0) end = 100;
    *temp_s = '\0';
    i = start;
    while(get_kan_index(i)!= NULL && i < end ){
        strcat(temp_s,(char*)get_kan_index(i)->oya);
        i++;
    }
    return (unsigned char*)temp_error_str;
}
/*
 strrtokは、文字列strの中で右から数えて最初に文字kが現れるところで、文字列を終えるようにする。
 */
char* strrtok(char* str, char k)
{
    char* temp_str;
    int   len;
    len = (int)strlen(str);
    temp_str = str + len;
    while(temp_str != str){
        if(*temp_str == k){
            *temp_str = '\0';
            return str;
        }
        temp_str--;
    }
    return str;
}
/*
 ending_processは、FILE streemからascii文字を読み取ってbufに移す。もしascii文字でなければ、
 FILEに押し戻す。戻り値は移した後のbufへのポインタ
 */
unsigned char* ending_process(FILE* i_file, unsigned char* buf)
{
    int     lit, ret;
    while((lit = getc(i_file)) != EOF && one_character((unsigned char)lit) == 1){
        if(strchr(ending_chars, lit) == NULL) break;
        *buf = (unsigned char)lit;
        buf++;
    }
    *buf = '\0';
    ret = ungetc(lit, i_file);
    if(ret == EOF){
        write_file_end_flag(1);
    }
    return buf;
}
/*
 ここはそれぞれ、ひらがな・カタカナ・読点、句点を判断し、そうである場合は、１、
 そうでない場合は０を返す。
 */
int is_utf_hira(unsigned char* kan)
{
    int ret, re;
    ret = strncmp((const char*)kan,(const char*)hira_s,3);
    re = strncmp((const char*)kan,(const char*)hira_e,3);
    if(ret>0 && re<0)return 1;
    return 0;
}
int is_utf_kata(unsigned char* kan)
{
    int ret, re;
    ret = strncmp((const char*)kan,(const char*)kata_s,3);
    re = strncmp((const char*)kan,(const char*)kata_e,3);
    if(ret>0 && re<0)return 1;
    return 0;
}
int is_utf_toten(unsigned char* toten)
{
    int ret;
    ret = strncmp((const char*)toten,"、",3);
    if(ret == 0) return 1;
    return 0;
}
int is_utf_hyph(unsigned char* cc)
{
    int ret;
    ret = strncmp((const char*)cc,"ー",3);
    if(ret == 0) return 1;
    ret = strncmp((const char*)cc,"―",3);
    if(ret == 0) return 1;
    ret = strncmp((const char*)cc,"—",3);
    if(ret == 0) return 1;
    ret = strncmp((const char*)cc,"–",3);
    if(ret == 0) return 1;
    ret = strncmp((const char*)cc,"‒",3);
    if(ret == 0) return 1;
    ret = strncmp((const char*)cc,"‑",3);
    if(ret == 0) return 1;
    ret = strncmp((const char*)cc,"‐",3);
    if(ret == 0) return 1;
    ret = strncmp((const char*)cc,"−",3);
    if(ret == 0) return 1;
    return 0;
}
int is_utf_kuten(unsigned char* kuten)
{
    int ret;
    ret = strncmp((const char*)kuten,"。",3);
    if(ret == 0) return 1;
    return 0;
}
/*
 「」かどうかを判断する。開き括弧は２、閉じ括弧は１、括弧でなければ0を返す。
 */
int is_utf_kakko(unsigned char* kakko)
{
    int ret;
    ret = strncmp((const char*)kakko,"「",3);
    if(ret == 0) return 2;
    ret = strncmp((const char*)kakko,"」",3);
    if(ret == 0) return 1;
    return 0;
}
/*
 is_utf_kakkoの半角｢｣｡､を通常の全角の「」。、に直す。
 */
unsigned char* han_yaku_normalize(unsigned char* temp)
{
    if(!strncmp((const char*)temp,"｢",3))
        strncpy((char*)temp,"「",3);
    if(!strncmp((const char*)temp,"｣",3))
        strncpy((char*)temp,"」",3);
    if(!strncmp((const char*)temp,"｡",3))
        strncpy((char*)temp,"。",3);
    if(!strncmp((const char*)temp,"､",3))
        strncpy((char*)temp,"、",3);
    return temp;
}
/*
 『』かどうかを判断する。開き括弧は２、閉じ括弧は１、括弧でなければ0を返す。
 */
int is_utf_wkakko(unsigned char* wkakko)
{
    int ret;
    ret = strncmp((const char*)wkakko,"『",3);
    if(ret == 0) return 2;
    ret = strncmp((const char*)wkakko,"』",3);
    if(ret == 0) return 1;
    return 0;
}
/*
 （）または、()を判定する。
 */
int is_utf_mkakko( unsigned char* mkakko)
{
    int   ret;
    ret = strncmp((const char*)mkakko,"（",3);
    if(ret == 0) return 2;
    ret = strncmp((const char*)mkakko,"）",3);
    if(ret == 0) return 1;
    return 0;
}
/*
 全角の英・記号を１バイトのアスキー記号に直す。引数は全角アスキー記号の３バイト文字、戻り値はアスキー記号
 入力が全角アスキー記号でなければ、空列を返す。*/
char b3_to_ascii(unsigned char* c_lit)
{
    unsigned char c1,c2;
    if(c_lit[0] < 0x80 ) return c_lit[0];
    if(c_lit[0] == 0xEF)
    {
        c1 = c_lit[1];
        c2 = c_lit[2];
        if(c1 == 0xBC && c2 < 0xC0)
        {
            return c2 - 0x60;
        }
        if(c1 == 0xBD && c2 < 0x9F)
        {
            return c2 - 0x20;
        }
    }
    return '\0';
}
/*
 ひらがなの文字列in_cをかたかなの文字列に変換する。戻り値はかたかな文字列へのポインタ
 */
unsigned char* hira_to_kata(unsigned char* in_c)
{
    unsigned char* tmp_c;
    tmp_c = in_c;
    while(*tmp_c != '\0'){
        if(is_utf_kata(tmp_c)){
            tmp_c = tmp_c + 3;
            continue;
        }
        if(tmp_c[1] == 0x81 && tmp_c[2] < 0xA0){
            tmp_c[1] = 0x82;
            tmp_c[2] = tmp_c[2] + 0x20;
        }
        if(tmp_c[1] == 0x81 && tmp_c[2] >0x9F){
            tmp_c[1] = 0x83;
            tmp_c[2] = tmp_c[2] - 0x20;
        }
        if(tmp_c[1] == 0x82 && tmp_c[2] < 0xA0){
            tmp_c[1] = 0x83;
            tmp_c[2] = tmp_c[2] + 0x20;
        }
        tmp_c = tmp_c + 3;
    }
    return in_c;
}
/*
 かたかなの文字列in_cをひらがなの文字列に変換する。戻り値はひらがな文字列へのポインタ
 */
unsigned char* kata_to_hira( unsigned char* in_c)
{
    unsigned char* tmp_c;
    tmp_c = in_c;
    while(*tmp_c != '\0'){
        if(is_utf_hira(tmp_c)){
            tmp_c = tmp_c + 3;
            continue;
        }
        if(tmp_c[1] == 0x82 && tmp_c[2] > 0xA0){
            tmp_c[1] = 0x81;
            tmp_c[2] = tmp_c[2] -(0xA0 - 0x80);
        }
        if(tmp_c[1] == 0x83 && tmp_c[2] < 0xA0){
            tmp_c[1] = 0x81;
            tmp_c[2] = tmp_c[2] + (0xA0 - 0x80);
        }
        if(tmp_c[1] == 0x83 && tmp_c[2] > 0x9F){
            tmp_c[1] = 0x82;
            tmp_c[2] = tmp_c[2] - (0xA0 - 0x80);
        }
        tmp_c = tmp_c + 3;

    }
    return in_c;
}
/*
 ここは漢字の文字列kanを読み込んでが３バイト漢字である場合は３、４バイト漢字である場合は４を返す。
 そうでない場合は０を返す。
 */
int is_utf_kanji(unsigned char* kan)
{
    int ret, re;
    if(kan[0] > 0xE2){
        ret = strncmp((const char*)kan,(const char*)kan_s,3);
        re = strncmp((const char*)kan,(const char*)kan_e,3);
        if(ret>0 && re<0)return 3;
        ret = strncmp((const char*)kan,(const char*)kan_p_s,3);
        re = strncmp((const char*)kan,(const char*)kan_p_e,3);
        if(ret>0 && re<0)return 3;
    }
    if(kan[0] == 0xF0){
        ret = strncmp((const char*)kan,(const char*)kan_e_s,4);
        re = strncmp((const char*)kan,(const char*)kan_e_e,4);
        if(ret>0 && re<0)return 4;
    }
    return 0;
}


/*
 ＜＞かどうかを判断する。開き括弧は２、閉じ括弧は１、括弧でなければ0を返す。
 */
int is_utf_angle(unsigned char* wkakko)
{
    int ret;
    ret = strncmp((const char*)wkakko,"＜",3);
    if(ret == 0) return 2;
    ret = strncmp((const char*)wkakko,"＞",3);
    if(ret == 0) return 1;
    return 0;
}
/*
 中点「・」かどうかを判断する。中点であれば、1そうでなければ０を返す。
 */
int is_utf_nakaten(unsigned char* naka)
{
    int ret;
    ret = strncmp((const char*)naka,"・",3);
    if(ret == 0)
    return 1;
    return 0;
}
/*
 踊り字「〻」かどうかを判断する。踊り字であれば、1そうでなければ０を返す。
 */
int is_utf_odori(unsigned char* odori)
{
    int ret;
    ret = strncmp((const char*)odori,"〻",3);
    if(ret == 0) return 1;
    return 0;
}
/*
 3バイトのアスキー文字（全角文字）かどうかを判断する。そうであれば１、そうでなければ０を返す。
 */
int is_utf_3b_ascii(unsigned char* kan)
{
    int ret, re;
    ret = strncmp((const char*)kan,(const char*)ascii_s,3);
    re = strncmp((const char*)kan,(const char*)ascii_e,3);
    if(ret>0 && re<0)return 1;
    return 0;
}
/*
 unsigned charである文字列を読み込み、句読点、括弧、中点、漢字、かな、ASCII, 全角ASCIIを判別する。
 戻り値は
 読点が11、
 句点が13
 括弧は「」の開き34、閉じ17、『』の開きが34、閉じが17となる。
 中点が8
 踊り字
 3byte漢字が3
 4byte漢字は4
 ひらがなが19
 かたかなが17
 ハイフンに相当する横棒が6
 全角ASCIIが5
 1バイトのASCII記号は1
 でそれ以外の場合は０である。
 */
int is_utf_char( unsigned char* cc)
{
    int ret;
    ret = one_character( *cc );
    if( ret == 1 ) return 1;
    if( ret == 2 ) return 0;
    if(*cc == 0xE3 && *(cc+1) == 0x80 && *(cc+2) == 0x80) return 0;
    ret = is_utf_kanji(cc);
    if(ret > 0)
    {
        return ret;
    }
    if(is_utf_kata(cc))return 41;
    if(is_utf_hira(cc))return 43;
    if(is_utf_3b_ascii(cc))return 5;
    if(is_utf_toten(cc))return 11;
    if(is_utf_kuten(cc))return 13;
    if((ret = is_utf_kakko(cc)) > 0)
    {
        return 17 * ret;
    }
    if((ret = is_utf_wkakko(cc)) > 0)
    {
        return 17 * ret;
    }
    if(is_utf_nakaten(cc))return 8;
    if(is_utf_odori(cc))return 10;
    if(is_utf_hyph(cc)) return 6;
    return 0;
}
/*
 漢字もしくは約物を判定し、漢字の場合は漢字のバイト数を、
 約物の場合は、２を返す。それ以外の場合は０を返す。
 */
int is_utf_kan_or_yaku(unsigned char* cc)
{
    int     ret;
    ret = is_utf_kanji( cc );
    if( ret != 0) return ret;
    if(is_utf_toten(cc))return 2;
    if(is_utf_kuten(cc))return 2;
    if(is_utf_nakaten(cc))return 5;
    if((ret = is_utf_kakko(cc)) > 0)
    {
        return 2;
    }
    if((ret = is_utf_wkakko(cc)) > 0)
    {
        return 2;
    }
    return 0;
}
/*
 ３バイトまたは４バイト文字１文字分だけs1から切り出して、temp_charに保存する。
 なお、３バイト文字（但し、日本語・漢字ではないものの一部が含まれる）でなければ、
 空列にして返す。
 */
unsigned char* mb_strn(unsigned char* s1)
{
    if(*s1 < 0xE3)temp_char[0] = '\0';
    temp_char[0] = *s1;
    temp_char[1] = *(s1 + 1);
    temp_char[2] = *(s1 + 2);
    if(*s1 > 0xEF){
        temp_char[3] = *(s1 + 3);
        temp_char[4] = '\0';
    }
    else
    {
        temp_char[3] = '\0';
    }
    return temp_char;
}
/*
 int is_r_yaku(unsigned char* s, int mode)
 モードで示された約物がsの最後に来るかどうかを判定する。modeに入れる数は、is_utf_charで
 */
int is_r_yaku(unsigned char* s)
{
    int             len, ret;
    unsigned char* temp_s;
    len = (int)strlen((char*)s);
    if(len < 3) return 0;
    temp_s = s + len - 3;
    if(*temp_s != 0xE3 ) return 1;
    if(is_utf_toten(temp_s))return 11;
    if(is_utf_kuten(temp_s))return 13;
    if((ret = is_utf_kakko(temp_s)) > 0)
    {
        return 17 * ret;
    }
    if((ret = is_utf_wkakko(temp_s)) > 0)
    {
        return 17 * ret;
    }
    if(is_utf_nakaten(temp_s))return 8;
    if(is_utf_hyph(temp_s)) return 6;
    return 0;
}
/*
 int b3_strcmp_out_yaku(unsigned char* s1, unsigned char* s2, int mode)
 modeで示された約物を除いた右端から、文字列を比較する。modeがあることを除けば、b3_strrcmpと同じである。
 modeは
 0 = 通常のb3_strrcmpと同じ。
 1 = s1の読点を除いて比較。
 2 = s1の句点を除いて比較。
 3 = s1の句点・読点を除いて比較。
 */
int b3_strcmp_out_yaku(unsigned char* s1, unsigned char* s2, int mode)
{
    int len, len2, ret;
    unsigned char* temp_p;
    unsigned char* temp_p2;
    len = (int)strlen((char*)s1);
    len2 = (int)strlen((char*)s2);
    if(len == 0 && len2 == 0) return 0;
    if(len == 0 || len2 == 0) return 1;
    temp_p = s1 + len;
    temp_p2 = s2 + len2;
    ret = is_r_yaku(s1);
    switch(mode){
        case 1:
            if(ret == 13)
                ret = 3;
            break;
        case 2:
            if(ret == 11)
                ret = 3;
            break;
        case 3:
            if(ret == 13 || ret == 11)
                ret = 3;
            break;
        default:
            ret = 0;
    }
    temp_p = temp_p - ret;
    while(temp_p2 != s2)
    {
        if(*temp_p == *temp_p2 )
        {
            temp_p = temp_p - 1;
            temp_p2 = temp_p2 -1;
        }
        else
        {
            return 1;
        }
    }
 return 0 + ret;
}
/*
３バイト文字列s1の右端が３バイト文字列s2であるかを判定する。右端がs2であれば、０、そうでなければ１を返す。
 */
int b3_strrcmp( unsigned char* s1, unsigned char* s2)
{
    int len, len2;
    unsigned char* temp_p;
    unsigned char* temp_p2;
    len = (int)strlen((char*)s1);
    len2 = (int)strlen((char*)s2);
    if(len == 0 && len2 == 0) return 0;
    if(len == 0 || len2 == 0) return 1;
    temp_p = s1 + len;
    temp_p2 = s2 + len2;
    while(temp_p2 != s2)
    {
        if(*temp_p == *temp_p2 )
        {
            temp_p = temp_p - 1;
            temp_p2 = temp_p2 -1;
        }
        else
        {
            return 1;
        }
    }
 return 0;
}
/*
３バイト文字列s1の左端が３バイト文字列s2であるかを判定する。左端がs2であれば、０、そうでなければ１を返す。
 */
int b3_strlcmp(unsigned char* s1, unsigned char* s2)
{
    int i;
    if(s1 == NULL || s2 == NULL) return 1;
    if( *s1 == '\0' && *s2 == '\0') return 0;
    if( *s2 == '\0' || *s1 == '\0') return 1;
    else
    {
        i = 0;
        while(*(s2+i) != '\0')
        {
            if(*(s1+i) != *(s2+i))return 1;
            i = i + 1;
        }
        return 0;
    }
}
/*
 strstrを多バイト版にしたもの。多バイトコードの文字列s1の中から、同じく多バイトコード
 のs2をさがす。見つかれば、s1の中の見つかった場所のポインターを返し、見つからなければ
 NULLを返す。
 */
unsigned char* mb_strstr(unsigned char* s1, unsigned char* s2)
{
    int i, j, k, code;
    if( s2 == NULL || s1 == NULL||*s1 == '\0' || *s2 == '\0') return NULL;
    i = 0;
    while(*(s1 + i) != '\0')
    {
        code = one_character(*s1);
        if(*(s1 + i) == *(s2))
        {
            j = 1;
            k = i+1;
            while(*(s1+k) == *(s2+j)){
                k++;
                j++;
            }
            if(*(s2 + j) == '\0')return s1 + i;
        }
        i = i + code;
    }
    return NULL;
}
/*
 strstrを３バイト版にしたもの。多バイトコードの文字列s1の中から、同じく３バイトコード
 のs2をさがす。見つかれば、0、見つからなければ1を返す。
 */
int      b3_strstr(unsigned char* s1, unsigned char* s2)
{
    int i, j, k;
    if( s2 == NULL || s1 == NULL) return 1;
    if(*s1 == '\0' && *s2 == '\0') return 0;
    if(*s1 == '\0' || *s2 == '\0') return 1;
    i = 0;
    while(*(s1 + i) != '\0')
    {
        if(*(s1 + i) == *(s2))
        {
            j = 1;
            k = i+1;
            while(*(s1+k) == *(s2+j)){
                k++;
                j++;
            }
            if(*(s2 + j) == '\0')return 0;
        }
        i = i + 3;
    }
    return 1;
}

/*
 s1とs2の内容を入れ替える。戻り値はs1へのポインター。ただし、返された時には、s2の内容も書き変わっている。
 */
unsigned char* strswp( unsigned char* s1, unsigned char* s2 )
{
    unsigned char tmp[100];
    strcpy((char*)tmp,(char*)s1);
    strcpy((char*)s1,(char*)s2);
    strcpy((char*)s2,(char*)tmp);
    return s1;
}

/*
 漢字kan1とkan2が同じ漢字かどうかを確かめる。同じであれば0、同じでなければ1を返す。
 */
int     kanji_cmp(unsigned char* kan1, unsigned char* kan2)
{
    int k1_res, i;
    if(kan1 == NULL || kan2 == NULL) return 1;
    if(*kan1 != *kan2) return 1;
    k1_res = is_utf_kanji(kan1);
    if(k1_res < 3) return 1;
    for(i = 1;i<k1_res;i++){
        if(*(kan1+i)!= *(kan2+i))
            return 1;
    }
    return 0;
}
/*
 かなkana1とkana2が同じ文字列かどうかを確かめる。同じであれば0、同じでなければ1を返す。なお、
 この関数は、原則としてひらがな、またはカタカナの文字列に関してのみ判定する。そうでない場合、例えば漢字同士の比較などについては、最初から1を返す。
 */
int     kana_cmp(unsigned char* kana1, unsigned char* kana2)
{
    int i;
    if(kana1 == NULL || kana2 == NULL) return 1;
    if(*kana1 != 0xE3 || *kana2 != 0xE3) return 1;
    i = 0;
    while(*(kana1+i) != '\0')
    {
        if(*(kana1+i)!= *(kana2+i)) return 1;
        i++;
    }
    if(*(kana2+i) == '\0') return 0;
    return 1;
}

/*
文字列s1の中にある漢字kan1をkan2で置き換える。返り値は漢字が置き換えられた文字列s1
 */
unsigned char* replace_kan(unsigned char* s1, unsigned char* kan1, unsigned char* kan2)
{
    int             code1, code2, i;
    unsigned char* ori_bp;
    unsigned char* temp_bp;
    ori_bp = s1;
    temp_bp = temp_buf_kun;
    code1 = is_utf_kanji(kan1);
    code2 = is_utf_kanji(kan2);
    while(*ori_bp != '\0'){
        if(kanji_cmp(ori_bp,kan1)== 0){
            for(i = 0;i<code2;i++){
                *temp_bp = *kan2;
                temp_bp++;
                kan2++;
            }
            kan2 = kan2 - code2;
            ori_bp = ori_bp + code1;
        }
        *temp_bp = *ori_bp;
        temp_bp++;
        ori_bp++;
    }
    *temp_bp = '\0';
    strcpy((char*)s1,(char*)temp_buf_kun);
    return s1;
}
/*
 start_sで始まり、end_sの直前で終わる文字列をs1に格納する。戻り値はs1へのポインタ
 */
unsigned char* cut_from_string(unsigned char* s1,unsigned char* start_s,unsigned char* end_s){
    unsigned char* temp1;
    unsigned char* temp2;
    temp1 = start_s;
    temp2 = s1;
    while(temp1 != end_s){
        *temp2 = *temp1;
        temp1++;
        temp2++;
    }
    *temp2 = '\0';
    return s1;
}
/*
 streamにある次の漢字を探す。漢字があれば、その先頭のポインタを返す。次の漢字がなければ、NULLを返す。
 */
unsigned char* next_kanji( unsigned char* stream )
{
    unsigned char* temp_st;
    int            byte_num;
    temp_st = stream;
    while(*temp_st != '\0')
    {
        byte_num = one_character(*temp_st);
        if(byte_num < 3){
            temp_st = temp_st + byte_num;
            continue;
        }
        if(is_utf_kanji(temp_st) > 0) return temp_st;
        temp_st = temp_st + byte_num;
    }
    return NULL;
}
/*
 ストリームkpから漢字を一字抜き出す。ストリームの先頭が漢字でなければ、kpをそのまま返す。
 */
unsigned char* del_kanji(unsigned char* kp)
{
    int ret;
    ret = is_utf_kanji(kp);
    kp = kp + ret;
    return kp;
}

/*
 訓読文s1の中から()で囲まれた読みの部分を除き次の漢字のポインタを返す。
 */
unsigned char* skip_yomi( unsigned char* s1)
{
    unsigned char* temp_bp;
    int             ret, bracket;
    temp_bp = s1;
start:    if(*temp_bp != '(' && *temp_bp != '[') return temp_bp;
    bracket = 0;
    if(*temp_bp == '(') bracket = 1;
    if(*temp_bp == '[') bracket = 2;
    temp_bp = temp_bp + 1;
    switch(bracket){
        case 1:
            while(*temp_bp != ')')
            {
                ret = is_utf_hira(temp_bp);
                if(ret == 1)
                {
                    temp_bp = temp_bp + 3;
                }
                else
                {
                    return s1;
                }
            }
            temp_bp = temp_bp + 1;
            goto start;
        case 2:
            while(*temp_bp != ']')
            {
                ret = is_utf_hira(temp_bp);
                if(ret == 1)
                {
                    temp_bp = temp_bp + 3;
                }
                else
                {
                    return s1;
                }
            }
            temp_bp = temp_bp + 1;
            goto start;
        default:
            return temp_bp;
    }
    return temp_bp;
}
/*
 del_kana_posteria()は、kana1とkana2を比較し、kana2がkana1の最後部と一致した場合、
 そのkana2の部分をkana1から除く。ただし、ここでは、kana1は書き換えられる。書き換えが行われていれば書き換えが行われた文字数が、
 なにもなされなければ０を返す。
 */
int del_kana_posteria( unsigned char* kana_1, unsigned char* kana_2)
{
    int     len1, len2, temp_p;
    unsigned char*      temp_kana;
    len1 = (int)strlen((const char*)kana_1);
    len2 = (int)strlen((const char*)kana_2);
    if( len1 < len2) return 0;
    temp_p = len1 - len2;
    temp_kana = kana_1 + temp_p;
    if(!kana_cmp(temp_kana,kana_2)){
        *temp_kana = '\0';
        return len2;
    }
    return 0;
}
/*
 del_kana_posteria_list()は、kana1とkana2[]を比較し、kana2[]のいずれかがkana1の最後部と一致した場合、
 そのkan2の部分をkan1から除く。ただし、ここでは、kan1は書き換えられる。書き換えが行われていれば書き換えが行われた文字数が、
 なにもなされなければ０を返す。これは、del_kana_posteria()のkanaリスト版である。
 */
int del_kana_posteria_list( unsigned char* kana_1, int max)
{
    int     len1, len2, temp_p,i;
    unsigned char*      temp_kana;
    unsigned char*      temp_huku;
    len1 = (int)strlen((const char*)kana_1);
    if( len1 < 6) return 0;
    i = 0;
    while(i < max && (temp_huku = get_huku_josi(i)) != NULL)
    {
        len2 = (int)strlen((const char*)temp_huku);
        temp_p = len1 - len2;
        temp_kana = kana_1 + temp_p;
        if(!kana_cmp(temp_kana,temp_huku)){
            *temp_kana = '\0';
            return len2;
        }
        i++;
    }
    return 0;
}

/*
 init_buf( unsigned char* buf,int max)は、bufの内容をmaxまですべて０で埋める。
 */
void  init_buf( unsigned char* buf,int max)
{
    unsigned char*  temp_bp;
    int                   i;
    temp_bp = buf;
    for(i=0;i<max;i++)
    {
        *buf = '\0';
        buf++;
    }
    
}

/*
 ある文字列s2の中の文字がs1の中にあるかを調べる。あれば、s1の中のその文字へのポインタ、なければNULLを返す。
 ただし、s2の中に同じ文字がないことを前提としている。
 */
char*   strstrchr(char* s1, char* s2)
{
    char* temp_bp;
    char* ret_bp;
    temp_bp = s2;
    while(*temp_bp != '\0'){
        ret_bp = strchr(s1,*temp_bp);
        if(ret_bp != NULL) return ret_bp;
        temp_bp++;
    }
    return NULL;
}
