//
//  utfstring.h
//  kunten
//
//  Created by 岡崎康浩 on 2023/05/28.
//

#ifndef utfstring_h
#define utfstring_h
int one_character(unsigned char str);
char b3_to_ascii(unsigned char* lit);
unsigned char* hira_to_kata(unsigned char* in_c);
unsigned char* kata_to_hira(unsigned char* in_c);
int is_utf_kanji(unsigned char* kan);
int is_utf_hira(unsigned char* kan);
int is_utf_kata(unsigned char* kan);
int is_utf_kakko(unsigned char* kakko);
int is_utf_wkakko(unsigned char* wkakko);
int is_utf_3b_ascii(unsigned char* kan);
int is_utf_hyph(unsigned char* cc);
int is_utf_angle(unsigned char* wkakko);
int is_utf_char( unsigned char* cc);
int is_utf_odori(unsigned char* odori);
int is_utf_nakaten(unsigned char* naka);
int is_utf_kan_or_yaku(unsigned char* cc);
int is_utf_mkakko( unsigned char* mkakko);
unsigned char* mb_strn(unsigned char* s1);
int b3_strrcmp( unsigned char* s1, unsigned char* s2);
int b3_strlcmp(unsigned char* s1, unsigned char* s2);
int b3_strstr(unsigned char* s1, unsigned char* s2);
unsigned char* strswp( unsigned char* s1, unsigned char* s2 );
int     kanji_cmp(unsigned char* kan1, unsigned char* kan2);
int     kana_cmp(unsigned char* kana1, unsigned char* kana2);
unsigned char* han_yaku_normalize(unsigned char* temp);
unsigned char* replace_kan(unsigned char* s1, unsigned char* kan1, unsigned char* kan2);
unsigned char* cut_from_string(unsigned char* s1,unsigned char* start_s,unsigned char* end_s);
unsigned char* mb_strstr(unsigned char* s1, unsigned char* s2);
unsigned char* next_kanji( unsigned char* stream );
unsigned char* del_kanji(unsigned char* kp);
unsigned char* skip_yomi( unsigned char* s1);
unsigned char* skip_kana( unsigned char* ip);
int     del_kana_posteria( unsigned char* kana_1, unsigned char* kana_2);
int     del_kana_posteria_list( unsigned char* kana_1, int max);
int     b3_strcmp_out_yaku(unsigned char* s1, unsigned char* s2, int mode);
void    init_buf( unsigned char* buf,int max);
char*   strstrchr(char* s1, char* s2);
unsigned char* ending_process(FILE* i_file, unsigned char* buf);
unsigned char* kanindex_to_str(int start, int end);
unsigned char* read_temp_error_str(void);
void            write_error_kun( unsigned char* s);
unsigned char*  read_temp_error_kun( void );
char*       strrtok(char* str, char k);


//unsigned char* del_yaku_buffer( unsigned char* buf);

#endif /* utfstring_h */
