//
//  kunten.h
//  kunten
//
//  Created by 岡崎康浩 on 2023/05/28.
//
#ifndef kunten_h
#define kunten_h

#undef  WINDOWS
#undef  DEBUG
//#define DEBUG_3

#define MAX_DIR             128
#define MEISI               2// 助詞なし。
#define RENTAI              3// 連体形
#define KOTO                5//「コト」「モノ」
#define POSTNO              7// 名詞＋ノ
#define POSTGA              7// 名詞＋ガ
#define RENTAIGA            11// 連体形＋ガ
#define RENYO               13// 連用形
#define RENTAIHAMO          17// 連体形に「ハ」「モ」がついた形。
#define EXIST               6630// 補語が、助詞なし、連体形、・・「こと」「もの」E (助詞なし２、連体形３、「こと」「モノ」５
// 連体形に「ハ」「モ」がついた形 17 さらに連用形の場合もあり得るので、13 2*3*5*17*13
#define COPULA              77// 名詞＋ノ、連体形＋ガ C　・・名詞＋ノ　７　連体形＋ガ　11
#define SPECIAL_COPULA      1771// COPULA 77に否定形の場合「ニ」23を取ることが可能である。77*23=1771
#define PROVERB             26//　連用形（イ段、エ段、接尾辞なし）A　連用形 13
#define SPECIAL1            15//　連体形、連体形＋コト S
#define SPECIAL2            3// 連体形 T
#define PRONOM              -1//
#define POSTNI              23//  N
#define POSTTO              29//  T
#define POSTYORI            31//  Y
#define POSTWO              37// O 847757 = 23*29*31*41
#define POSTHE              41//
#define YORINI              713//  M 23*31
#define POSTWONI            851//
#define GENERAL             1160579333 // 23*29*31*41*37 = 1160579333
#define PREPOSITION         53
#define HIDDEN_PREPOSITION  59
#define WITH                1537// W 53*23
#define FROM                1643// F 53*31
#define YORITO              47647// Q 53*29*31
#define DISYORI             1829// G 59*31
//
 #define HUKUSI              128 //3
 #define HENDOKU             256  //5
 #define DAISI               512  //7
 #define KAISI               1024//11
 #define SIYEKI              2048//13
 #define NONHENDOKU          4096//17
 #define HENDOKU2            8192//19
 #define HENDOKU3            16384//23
 #define HENDOKU4            32768//29
 #define HENDOKU5            65536 //131072..262144..524288//31
 #define HITEI               1048576//2097152..4194304..8388608//53
 #define OKI_KANA            64
 #define KORE                8388608//8388672
 /*
#define HUKUSI              3
#define HENDOKU             5
#define DAISI               7
#define KAISI               11
#define SIYEKI              13
#define NONHENDOKU          17
#define HENDOKU2            19
#define HENDOKU3            23
#define HENDOKU4            29
#define HENDOKU5            31
#define HITEI               43
#define KORE                1024
#define OKI_KANA            4
*///
#define MAX_PRIME           49
#define MAX_MOJI            100

void           index_free( void );
void           index_init( void );
void           refine_saidoku( void );
unsigned char* hojo(char x, int p_temp);
unsigned char* dl_replace( unsigned char* s1, unsigned char* s2);
int            one_character(unsigned char str);
unsigned char* get_one_char( FILE* i_file, unsigned char* temp);
unsigned char* get_one_char_stream(unsigned char* bp);
kanaindex*     redup_kanaindex(kanaindex* ori_kana, unsigned char* real, int f);
kanaindex*     expand_kana(kanaindex* ori_kana, char a, char b, int flag);
kanatuki*      expand_kana_abb(kanatuki* ori_kana, unsigned char cc, int cp);
int            okuri_identify(int i, kanlist *root);
int            abb_included_cmp(unsigned char* s1, unsigned char* sc_p, char x, unsigned char* sc_s, int p);
int            duplicated_kan( int index, int kun_jun_t, unsigned char* kun_bp);
void           saidoku_identify( saidoku* char_sai_ori );
int            choufuku( void );
itai*          find_itai(unsigned char* kan);
int            okiji_identify(kanaindex* source_okiji, int kan_i,int r_or_l);
int            odori_identify(int i);
int            kanlist_confirm( kanlist* temp_list, unsigned char* okuri_char, int p);
int            kanakan_identify(kanakan* kana_s);
void           reordering(int ind, int course);
void           saidoku_reordering(int jun, int jun2, int jun_n);
int            kana_to_kan(kanaindex* source_okurig, int kan_i,int kan_n, int r_or_l);
kanokuri*      succeeding_kan(unsigned char* bp);
kanlist*       identify_kanlist( unsigned char* kan, kanlist* temp_list);
int            preceeding_kan( int t_jun, int mode);
int            is_kore_joji( int index, kanokuri* temp_kan_kore,int kun_jun );
int            is_kaisi(int index,unsigned char* okuri,unsigned char* tuduki);
int            find_kan(unsigned char* bp);
int            hogo_property( int index, unsigned char* okuri_str, int mode);
int hendoku_property_identify(int index, unsigned char* kana, unsigned char* bp);
int            hendoku_point_identify( int index_main, int index, unsigned char* bp);
int            hukusi_property_identify(int index, unsigned char* kana);
long long int  exist_juku( unsigned char* kan1, unsigned char* kan2,int post);
int            find_double_kanji( int index, int mark);
int            length_kan_index( void );
int            for_variant_form( unsigned char* kun_bp);
int            juku_before_identify(int index, kanokuri* kan_temp_1, unsigned char* bp);
int            juku_after_identify(int index, int kun_jun_t, kanokuri* temp_kan, unsigned char* bp);
int            connect_match( int kan_1, unsigned char* okuri1, int kan_2, kanokuri* kan_okuri2, unsigned char* bp, int juku);
int            duplicate_case( int index, int kun_jun, unsigned char* kun_p);
int            assign_kan_index( unsigned char* kun );
int            force_swapping(int chofuku,int error, int pre_error);
int            is_real_succeeding_kan(int pre_index, int index, kanokuri* suc_bp, int jun, int juku);
int            okiji_kaisi_identify( int index, int max );
int            is_siyeki_on(int index, unsigned char* okuri);
int            itai_kanji_comp(unsigned char* kan1, unsigned char* kan2);
int            pre_connect_match(int index, int jun, kanokuri* kan_t, int target);
int            post_connect_match(int index, int jun, kanokuri* kan_t, int target);
int            is_conj_josi( int index, unsigned char* josi, unsigned char* okuri, int mode);
int            stop_infinite_loop(int error, int temp_i, int pre_temp_i);
itai*          find_root_itai( void );
kanlist*       find_hendoku_moji( void );
kanlist*       find_hendoku_moji2(void);
kanlist*       find_hendoku_moji3(void);
kanlist*       find_hendoku_moji4(void);
kanlist*       find_hendoku_moji5(void);
kanlist*       find_fukusi( void );
kanlist*       find_root_okuri( void );
kanlist*       find_root_daisi( void );
kanlist*       find_root_kaisi( void );
kanlist*       find_root_okikana(void);
kanlist*       find_no_hendoku(void);
kanakan*       find_root_kanakan( void );
saidoku*       find_root_saidoku( void );
kan_chain_list* find_siyeki(void);
kan_chain_list* find_hitei_hukusi(void);
kanokuri*      make_kanokuri(unsigned char* bp);
kanokuri_index* trace_juku(int index, unsigned char* bp);
int            process_kan(unsigned char* kun );
int            bun_line_read(void);
int            read_read_num( void );
int            haku_kuten_error( void );
void           write_kuten_error(int i);
void           caution(unsigned char* kan, unsigned char* message);
void           out_put_kan_index(unsigned char* haku_bp, unsigned char* kun_bp,int line);
void           org_file_message_with_int(unsigned char* s1, int dix, unsigned char* s2);
void           org_file_message(unsigned char* s1);
int            read_kuten_error( void );
int            g_test_read(void);

#ifdef DEBUG
int            debug_stop( int mark);

#endif
//#ifdef WINDOWS
extern FILE*       log_f;
extern int         log_on;
#define     wc_print(s) if(log_on==1)fprintf(log_f,(#s))
#define     wv_print(s) if(log_on==1)fprintf(log_f,"%s",s)
#define     wd_print(s) if(log_on==1)fprintf(log_f,"%d",s)
//#endif

#endif /* kunten_h */

