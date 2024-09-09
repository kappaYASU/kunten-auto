//
//  oyakan.h
//  kunten
//
//  Created by 岡崎康浩 on 2023/05/29.
//

#ifndef oyakan_h
#define oyakan_h

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
    long int            prop;
    int                 is_dosi;
    char                mod;
    int                 b_end;
    int                 interval;
    int                 kunten_mode;
    int                 gtest;
    long int            jufuku;
    struct kanaindex*   sai_yomi;
    int                 kun_jun;
    int                 sai;
} oyakan;

typedef struct kaeriten{
    int             code[7];
    int             juku;
    int             temp_ban;
} kaeriten;

typedef struct itai{
    unsigned char       jitai[5];
    struct itai*        chain;
    struct itai*        next_jitai;
} itai;
typedef struct kanakan{
    unsigned char       oya_kan[10];
    int                 jun;
    struct kanaindex*   yomi;
    struct kanakan*     next_kan;
} kanakan;
typedef struct kanaindex{
    unsigned char       kanayomi[10];
    unsigned char       kanaindex[25];
    unsigned char       mae_setu[10];
    int                 mae_setu_n;
    int                 kanayomi_n;
    int                 kanaindex_n;
    int                 ato;
    struct kanaindex*   next;
} kanaindex;
typedef struct saidoku{
    unsigned char       saikan[7];
    unsigned char       okuri[10];
    int                 okuri_n;
    struct kanaindex*   yomi_s;
    struct saidoku*     next_s;
} saidoku;
typedef struct kanatuki{
    unsigned char   hiramoji[16];
    int             mojisu;
    struct kanatuki *next;
} kanatuki;
typedef struct kanlist{
    unsigned char   listed_kan[5];
    int             attrib;
    struct kanatuki *okuri;
    struct kanlist  *next;
} kanlist;

typedef struct kan_chain_list{
    unsigned char listed_kan[5];
    struct kan_chain_list   *next;
}kan_chain_list;

typedef struct kanokuri{
    unsigned char   kanji[5];
    unsigned char   okurigana[61];
    unsigned char*  kan_point;
} kanokuri;
typedef struct kanokuri_index{
    struct kanokuri*       p_kanokuri;
    int                    p_index;
} kanokuri_index;
typedef struct comment{
    char            mark[3];
    unsigned char   comment[25];
} comment;
typedef struct mojimode{
    int         kun_mode;
    int         moji_size;
    double      lf_width;
} mojimode;


oyakan*     get_kan_index(int index);
kaeriten*   get_kaeri_index(int index);
int         read_verbose( void );


#define MAX_MOJI_LIST   100
#define LIST_PRIME      101

#endif /* oyakan_h */
