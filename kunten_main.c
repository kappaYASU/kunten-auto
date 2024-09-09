//
//  kunten_main.c
//  kunten
//
//  Created by 岡崎康浩 on 2023/06/07.
//
#include "kunten_main.h"

//#define DEBUG_2

int     bun_line, p_flag;
FILE*           log_f;
int             log_on;

int           kuten_error;
#ifdef DEBUG
int     stop_line = 4;
int     read_every( void );
#endif
/*
 kuten_errorの書き込みと、読み出し。
 */
void write_kuten_error(int i)
{
    kuten_error = i;
}
int read_kuten_error( void )
{
    return kuten_error;
}
/*
 verboseの値を読み取る。
 */
int   read_verbose( void )
{
    return verbose;
}
/*
 以下の関数は、それぞれ漢字に関するリストの元を取り出すための関数である。
 */
itai*   find_root_itai( void )
{
    return root_itai;
}
kanlist* find_hendoku_moji(void)
{
    return hendoku_moji;
}
kan_chain_list* find_siyeki(void)
{
    return siyeki;
}
kan_chain_list* find_hitei_hukusi(void)
{
    return hitei_hukusi;
}
kanlist* find_fukusi(void)
{
    return fukusi;
}
kanlist* find_root_okuri(void)
{
    return root_okuri;
}
kanlist* find_root_daisi(void)
{
    return daisi;
}
kanlist* find_root_kaisi(void)
{
    return kaisi;
}
kanlist* find_root_okikana(void)
{
    return okikana_root;
}
kanlist* find_no_hendoku(void)
{
    return no_hendoku;
}
kanlist* find_hendoku_moji2(void)
{
    return hendoku_moji2;
}
kanlist* find_hendoku_moji3(void)
{
    return hendoku_moji3;
}
kanlist* find_hendoku_moji4(void)
{
    return hendoku_moji4;
}
kanlist* find_hendoku_moji5(void)
{
    return hendoku_moji5;
}
kanakan*  find_root_kanakan(void)
{
    return kanakan_moji;
}
saidoku*  find_root_saidoku(void)
{
    return saidoku_moji;
}
/*
 変数bun_line（文番号）の読み出し
 */
int     bun_line_read(void)
{
    return bun_line;
}
/*
 次の関数は、g_testの値を取り出す。プログラムがtestモードで動いているかを判定する。動いていれば１、そうでなければ０を返す。
 */
int     g_test_read(void)
{
    return g_test;
}
/*
 これは、返読関係を表示するための関数。文法解析がうまくいっているかどうかの指標を提示する。
 */

//#ifdef DEBUG
void out_put_kan_index(unsigned char* haku_bp, unsigned char* kun_bp,int line)
{
    if(out_file != NULL){
        oyakan* temp_kan;
        int i,ind,len, tsume;
        ind = (int)strlen((char*)haku_bp);
        if(*(haku_bp + ind - 1) == '\n'){
            *(haku_bp + ind - 1) = '\0';
        }
        fprintf(out_file, "文番号:%d:白文:%s\n",line,haku_bp);
        fprintf(out_file, "文番号:%d:訓読文:%s\n",line,kun_bp);
        if(read_non_sai_okiji()){
            printf("文番号:%d:白文:%s\n",line,haku_bp);
            printf("文番号:%d:訓読文:%s\n",line,kun_bp);
        }
        i = 0;
        temp_kan = (oyakan*)get_kan_index( i );
        while(temp_kan != NULL){
            tsume = 24;
            len = temp_kan->okuri_n;
            len = (len / 3);
            tsume = tsume + len;
            fprintf(out_file,"%02d:%5s:送り仮名:%*s: 訓順:%02d 重複:%08ld 性質:%08ld 繋がり=%03d is_dosi= %03d ",i,temp_kan->oya,-tsume, temp_kan->okuri, temp_kan->kun_jun,temp_kan->jufuku,temp_kan->prop,temp_kan->gtest, temp_kan->is_dosi);
            if(temp_kan->label != NULL) fprintf(out_file,"ラベル: %s", temp_kan->label->comment);
            fprintf(out_file,"\n");
            i++;
            temp_kan = (oyakan*)get_kan_index( i );
        }
    }
}
//#endif
/*返読文字でありながら、補語に助詞を取らないあるいは特殊な助詞を取る漢字のリストhendoku.tblを読み込む
 戻り値は漢字リストkanlistへのポインタ*/
kanlist* readdata_long_okuri(const char* f_name, const char* sp_name, const char* marker)
{
    FILE* file_i;
    unsigned char lit;
    char full_okuri_file[MAX_DIR];
    char ex_char;
    int read_flag,pp,ex_flag, ex_char_flag, attrib_flag, temp_attrib;
    kanlist* tmp_kan;
    kanlist* next_tmp_kan;
    kanatuki* tmp_kana;
    kanatuki* next_tmp_kana;
    tmp_kana = NULL;
    strcpy(full_okuri_file, cdir_name);
    strcat(full_okuri_file, f_name);
    file_i = fopen(full_okuri_file,"r");
    if(file_i == NULL){
        printf("%sのデータファイルが開けません。\n",sp_name);
        return NULL;
    }
    tmp_kan = malloc(sizeof(kanlist));
    if(tmp_kan == NULL){
        printf("%sデータのメモリが足りません。\n",sp_name);
        return NULL;
    }
    tmp_kan->next = NULL;
    tmp_kan->attrib = 0;
    read_flag = 0;
    ex_char_flag = 0;
    attrib_flag = 0;
    temp_attrib = 1;
    ex_flag = 0;
    ex_char = '\0';
    //p_flag = 1;
    pp = 0;
    while ((signed char)(lit = getc(file_i)) != EOF){
        if(lit == '\n'){
            if(pp == 0 && read_flag == 0){
                read_flag = 2;
                ex_flag = 0;
                ex_char_flag = 0;
                attrib_flag = 0;
                temp_attrib = 1;
                ex_char = '\0';
                //p_flag = 1;
                continue;
            }
            else
            {
                if(tmp_kana!=NULL){
                    tmp_kana->hiramoji[pp] = '\0';
                    tmp_kana->mojisu = pp;
                    if(ex_char_flag > 0){
                        tmp_kana = expand_kana_abb(tmp_kana, ex_char, pp);
                    }
                }
                tmp_kan->okuri = tmp_kana;
                //tmp_kan->attrib = -1 * p_flag;
                next_tmp_kan = malloc(sizeof(kanlist));
                if(next_tmp_kan == NULL){
                    printf("%sデータのメモリが足りません。\n",sp_name);
                    return NULL;
                }
                next_tmp_kan->next = tmp_kan;
                tmp_kan = next_tmp_kan;
                pp = 0;
                read_flag = 2;
                ex_flag = 0;
                ex_char_flag = 0;
                attrib_flag = 0;
                temp_attrib = 1;
                ex_char = '\0';
                p_flag = 1;
                continue;
            }
        }
        if(lit == '?' && read_flag == 2){
            attrib_flag = 1;
            continue;
        }
        if(read_flag == 2 && attrib_flag == 1 && lit != '!'){
            if(strchr(marker,(int)lit) == NULL){
                printf("%sの属性設定に%cはありません\n",sp_name,lit);
                continue;
            }
            switch(lit){
                case 'E'://存在詞（有・無など、無表示はこれになる。）EXIST
                    temp_attrib = temp_attrib * EXIST;// 補語が、助詞なし、連体形、・・「こと」「もの」E (助詞なし２、連体形３、「こと」「モノ」５
                    break;
                case 'C'://同動詞（為など）COPULA
                    temp_attrib = temp_attrib * COPULA;// 名詞＋ノ、連体形＋ガ C　・・名詞＋ノ　７　連体形＋ガ　11
                    break;
                case 'M':// 名詞（助詞なし）
                    temp_attrib = temp_attrib * MEISI;
                    break;
                case 'R':// 連体形
                    temp_attrib = temp_attrib * RENTAI;
                    break;
                case 'G'://同動詞（為など）で否定形の場合「ニ」を取るもの
                    temp_attrib = temp_attrib * COPULA * POSTNI;// COPULA 77に否定形の場合「ニ」23を取ることが可能である。77*23=1771
                    break;
                case 'A'://形容詞のような副詞（難、易など）AJECT
                    temp_attrib = PROVERB;//　連用形（イ段、エ段、接尾辞なし）A　連用形 13
                    break;
                case 'S'://その他特殊な返読文字 SPECIAL
                    temp_attrib = temp_attrib * SPECIAL1;//　連体形、連体形＋コト S
                    break;
                case 'T'://その他特殊な返読文字 POSTTO「と」でのみ受けるもの
                    temp_attrib = temp_attrib * POSTTO;
                    break;
                case 'W'://その他特殊な返読文字 POSTWO「を」でのみ受けるもの
                    temp_attrib = temp_attrib * POSTWO;
                    break;
                case 'N'://その他特殊な返読文字 POSTWO「に」「を」でのみ受けるもの
                    temp_attrib = temp_attrib * POSTNI;
                    break;
                case 'H'://その他特殊な返読文字 POSTHE「へ」でのみ受けるもの
                    temp_attrib = temp_attrib * POSTHE;
                    break;
                case 'Y'://その他特殊な返読文字 YORINI「より、に」でのみ受けるもの
                    temp_attrib = temp_attrib * POSTYORI; 
                    break;
                case 'D'://代詞　PRONOM -1
                    temp_attrib = PRONOM;
                    break;
                default:
                    temp_attrib = EXIST;
                    break;
            }
            continue;
        }
        if(lit == '!' && read_flag == 2){
            attrib_flag = 0;
            tmp_kan->attrib = temp_attrib;
            tmp_kan->listed_kan[pp] = '\0';
            pp = 0;
            read_flag = 1;
            tmp_kana = malloc(sizeof(kanatuki));
            if(tmp_kana == NULL){
                printf("%s送り仮名データのメモリが足りません。\n",sp_name);
                return NULL;
            }
            tmp_kana->next = NULL;
            continue;
        }
        if(lit == '|' && read_flag == 1){
            tmp_kana->hiramoji[pp] = '\0';
            tmp_kana->mojisu = pp;
            if(ex_char_flag > 0){
                    tmp_kana = expand_kana_abb(tmp_kana, ex_char, pp);
            }
            next_tmp_kana = malloc(sizeof(kanatuki));
            if(next_tmp_kana == NULL){
                printf("%s送り仮名データのメモリが足りません。\n",sp_name);
                return NULL;
            }
            next_tmp_kana->next = tmp_kana;
            tmp_kana = next_tmp_kana;
            ex_flag = 0;
            ex_char_flag = 0;
            ex_char = '\0';
            p_flag = 1;
            pp = 0;
            continue;
        }
        if(lit == '%'){
            read_flag = 0;
            pp = 0;
            continue;
        }
        if(read_flag == 2){
            tmp_kan->listed_kan[pp] = lit;
            pp++;
            continue;
        }
        if(read_flag == 1){
            tmp_kana->hiramoji[pp] = lit;
            if(lit == '$')
            {   if(ex_flag == 1)
                {
                    ex_flag = 0;
                }
                else
                {
                    ex_flag = 1;
                }
                ex_char_flag = 2;
            }
            if(ex_flag == 1)
            {
                ex_char = lit;
            }
            pp++;
            continue;
        }
    }
    return tmp_kan->next;
}
/*
 
 */
kan_chain_list* read_kan_list(const char* f_name, const char* sp_name)
{
    FILE* file_i;
    unsigned char lit;
    int           pp, read_flag;
    char full_okuri_file[MAX_DIR];
    kan_chain_list* temp_list;
    kan_chain_list* next_temp_list;
    strcpy(full_okuri_file, cdir_name);
    strcat(full_okuri_file, f_name);
    file_i = fopen(full_okuri_file,"r");
    if(file_i == NULL){
        printf("%sのデータファイルが開けません。\n", sp_name);
        return NULL;
    }
    temp_list = malloc(sizeof(kan_chain_list));
    if(temp_list == NULL){
        printf("%sデータのメモリが足りません。\n", sp_name);
        return NULL;
    }
    pp = 0;
    read_flag = 0;
    while ((signed char)(lit = getc(file_i)) != EOF){
        if(lit == '\n'){
            if(pp == 0 && read_flag == 0){
                read_flag = 2;
                continue;
            }
            else
            {
                temp_list->listed_kan[pp] = '\0';
                next_temp_list = malloc(sizeof(kanlist));
                if(next_temp_list == NULL){
                    printf("%sデータのメモリが足りません。\n",sp_name);
                    return NULL;
                }
                next_temp_list->next = temp_list;
                temp_list = next_temp_list;
                pp = 0;
                read_flag = 2;
                continue;
            }
        }
        if(lit == '%'){
            read_flag = 0;
            continue;
        }
        if(read_flag > 1){
            temp_list->listed_kan[pp] = lit;
            pp++;
            continue;
        }
        
    }
    return temp_list->next;
}
/*
 副詞であって「ニ」「ト」などを送り仮名とする補語と紛らわしいもののリストを読み込む。
 */
kanlist* readdata_short_okuri(const char* f_name, const char* sp_name, const char* marker)
{
    FILE* file_i;
    unsigned char lit;
    char full_okuri_file[MAX_DIR];
    int read_flag,pp;
    kanlist* tmp_kan;
    kanlist* next_tmp_kan;
    kanatuki* tmp_kana;
    kanatuki* next_tmp_kana;
    tmp_kana = NULL;
    strcpy(full_okuri_file, cdir_name);
    strcat(full_okuri_file, f_name);
    file_i = fopen(full_okuri_file,"r");
    if(file_i == NULL){
        printf("%sのデータファイルが開けません。\n", sp_name);
        return NULL;
    }
    tmp_kan = malloc(sizeof(kanlist));
    if(tmp_kan == NULL){
        printf("%sデータのメモリが足りません。\n", sp_name);
        return NULL;
    }
    tmp_kan->next = NULL;
    tmp_kan->attrib = 1;
    read_flag = 0;
    //p_flag = 0;
    pp = 0;
    while ((signed char)(lit = getc(file_i)) != EOF){
        if(lit == '\n'){
            if(pp == 0 && read_flag == 0){
                read_flag = 2;
                // p_flag = 1;
                continue;
            }
            else
            {
                if(tmp_kana != NULL){
                    tmp_kana->hiramoji[pp] = '\0';
                    tmp_kana->mojisu = pp;
                    tmp_kan->okuri = tmp_kana;
                }
               // tmp_kan->attrib = (-1) * p_flag;
                next_tmp_kan = malloc(sizeof(kanlist));
                if(next_tmp_kan == NULL){
                    printf("%sデータのメモリが足りません。\n",sp_name);
                    return NULL;
                }
                next_tmp_kan->next = tmp_kan;
                tmp_kan = next_tmp_kan;
                pp = 0;
                read_flag = 2;
                //p_flag = 0;
                continue;
            }
        }
        if(lit == '?' && read_flag == 2){
            lit = getc(file_i);
            if(strchr(marker,(int)lit) == NULL){
           printf("%sの属性設定に%cはありません\n",sp_name, lit);
                continue;
            }
            switch(lit){
                case 'N'://「ニ」を送るもの
                    tmp_kan->attrib = POSTNI;
                    break;
                case 'T'://「ト」を送るもの
                    tmp_kan->attrib = POSTTO;
                    break;
                case 'Y'://「ヨリ」を送るもの
                    tmp_kan->attrib = POSTYORI;
                    break;
                case 'M'://「ヨリ」「ニ」を共に送るもの
                    tmp_kan->attrib = YORINI;
                    break;
                case 'O'://「ヲ」を送るもの
                    tmp_kan->attrib = POSTWO;
                    break;
                case 'Q'://　介詞「ヨリ」と「ト」に対応する
                    tmp_kan->attrib = YORITO;
                    break;
                case 'W':// 介詞「ト」に対応する
                    tmp_kan->attrib = WITH;
                    break;
                case 'F':// 介詞「ヨリ」に対応する
                    tmp_kan->attrib = FROM;
                    break;
                case 'G':// 介詞「ヨリ」に対応する ただし、動詞の後に置かれるときは置字となる。
                    tmp_kan->attrib = DISYORI;
                    break;
                default: break;
            }
            continue;
        }
        if(lit == '!' && read_flag == 2){
            tmp_kan->listed_kan[pp] = '\0';
            pp = 0;
            read_flag = 1;
            tmp_kana = malloc(sizeof(kanatuki));
            if(tmp_kana == NULL){
                printf("%sデータのメモリが足りません。\n",sp_name);
                return NULL;
            }
            tmp_kana->next = NULL;
            continue;
        }
        if(lit == '|' && read_flag == 1){
            tmp_kana->hiramoji[pp] = '\0';
            tmp_kana->mojisu = pp;
            next_tmp_kana = malloc(sizeof(kanatuki));
            if(next_tmp_kana == NULL){
                printf("%sデータのメモリが足りません。\n",sp_name);
                return NULL;
            }
            next_tmp_kana->next = tmp_kana;
            tmp_kana = next_tmp_kana;
            pp = 0;
            continue;
        }
        if(lit == '%'){
            read_flag = 0;
            continue;
        }
        if(read_flag == 2){
            tmp_kan->listed_kan[pp] = lit;
            pp++;
            continue;
        }
        if(read_flag == 1){
            tmp_kana->hiramoji[pp] = lit;
            pp++;
            continue;
        }
    }
    return tmp_kan->next;
}

/*
 異体字のデータベースであるitaiji.tblを読み込んで構造体itaiのリストを作る。itaiは、
 次の異体字のリストへのポインタnext_itaiと異体字のグループとなる異体字のリストへの
 へのポインタchain,そして、漢字そのものの文字コード3バイト、４バイト兼用の配列から
 なっている。すべての異体字のリストはnext_itaiで一本につながっているが、異体字のグル
 ープの中だけがchainでつながるようになっている。
 */
itai*  readdata_itai( void )
{
    FILE* file_i;
    char    lit;
    char    full_itai_file[MAX_DIR];
    int     read_flag, root_flag;
    int     pp;
    itai*       temp;
    itai*       temp_next;
    itai*       temp_chain;
    temp_chain = NULL;
    strcpy(full_itai_file,cdir_name);
    strcat(full_itai_file,"itaiji.tbl");
    file_i = fopen(full_itai_file, "r");
    if(file_i == NULL)
    {
        printf("異体字のデータファイルが開けません！\n");
        return NULL;
    }
    temp_next = malloc(sizeof(itai));
    temp_next->next_jitai = NULL;
    read_flag = 0;
    root_flag = 0;
    pp = 0;
    if(temp_next == NULL)
    {
        printf("異体字用のメモリが足りません！\n");
        return NULL;
    }
    while((lit = getc(file_i))!= EOF)
    {
        if(lit == '%'){
            if(pp > 0){
                temp_chain->jitai[pp] = '\0';
                temp_next->chain =  temp_chain;
                temp = malloc(sizeof(itai));
                if(temp == NULL)
                {
                    printf("異体字用のメモリが足りません！\n");
                    return NULL;
                }
                temp->next_jitai = temp_chain;
                temp_next = temp;
                pp = 0;
            }
            read_flag = 0;
            root_flag = 0;
            continue;
        }
        if(lit == '\n' && read_flag == 0)
        {
            read_flag = 1;
            root_flag = 1;
            continue;
        }
        if(lit == '\n' && read_flag == 1)
        {
            temp_chain->jitai[pp] = '\0';
            temp_next->chain =  temp_chain;
            temp = malloc(sizeof(itai));
            if(temp == NULL)
            {
                printf("異体字用のメモリが足りません！\n");
                return NULL;
            }
            temp->next_jitai = temp_chain;
            temp_next = temp;
            root_flag = 1;
            pp = 0;
            continue;
        }
        if(lit == '?' && root_flag == 1)
        {
            root_flag = 0;
            temp_next->jitai[pp] = '\0';
            pp = 0;
            temp = malloc(sizeof(itai));
            if(temp == NULL)
            {
                printf("異体字用のメモリが足りません！\n");
                return NULL;
            }
            temp->next_jitai = temp_next;
            temp->chain = temp_next;
            temp_chain = temp;
            continue;
        }
        if(lit == '?' && root_flag == 0 && read_flag == 1)
        {
            temp_chain->jitai[pp] = '\0';
            pp = 0;
            temp = malloc(sizeof(itai));
            if(temp == NULL)
            {
                printf("異体字用のメモリが足りません！\n");
                return NULL;
            }
            temp->next_jitai = temp_chain;
            temp->chain = temp_chain;
            temp_chain = temp;
            continue;
        }
        if(root_flag == 1 && read_flag == 1)
        {
            temp_next->jitai[pp] = (unsigned char)lit;
            pp++;
        }
        if(root_flag == 0 && read_flag == 1)
        {
            temp_chain->jitai[pp] = (unsigned char)lit;
            pp++;
        }
        
    }
    temp = temp_next->next_jitai;
    free(temp_next);
    return temp;
}
/*
 再読文字のデータsaidoku.tblを読み込む。
 返り値は、仮名を対応させるリストであるkanakan構造体のポインターである。
 */
/*
 再読文字のデータsaidoku.tblを読み込む。
 返り値は、仮名を対応させるリストであるkanakan構造体のポインターである。
 */
saidoku* readdata_sai( void )
{
    FILE* file_d;
    char cp, ex_char[2];
    char full_sai_file[MAX_DIR];
    int pp;
    static int kan_flag,yomi_flag,index_flag,okuri_flag;
    static int ex_flag,ex_char_flag;
    saidoku* final_char_s;
    saidoku* next_char_s;
    kanaindex* kana_s;
    kanaindex* kana_s_next;
    strcpy(full_sai_file,cdir_name);
    strcat(full_sai_file,"saidoku.tbl");
    file_d = fopen(full_sai_file,"r");
    if(file_d == NULL){
        printf("再読文字のデータファイルが開けません!\n");
        return NULL;
    }
    final_char_s = malloc(sizeof(saidoku));
    final_char_s->next_s = NULL;
    next_char_s = NULL;
    kana_s = NULL;
    pp = 0;
    kan_flag = 1;
    okuri_flag = 0;
    yomi_flag = 0;
    index_flag = 0;
    ex_flag = 0;
    ex_char_flag = 0;
    ex_char[0]  = '\0';
    ex_char[1]  = '\0';
    while((cp = getc(file_d))!= EOF){
        if(cp == '%'){
            while((cp = getc(file_d))!= '\n')continue;
            }
        if(cp == '\n' && kan_flag == 1) continue;
        if(cp == '\n'){
            kana_s->kanaindex[pp] = '\0';
            kana_s->kanaindex_n = pp;
            if(yomi_flag == 1){
                kana_s->kanayomi[pp-(kana_s->mae_setu_n)] = '\0';
                kana_s->kanayomi_n = pp - (kana_s->mae_setu_n);
            }
            if(ex_char_flag > 0)
            {
                kana_s = expand_kana(kana_s,ex_char[0],ex_char[1], ex_char_flag);/*$X$を展開させる */
            }
        final_char_s->yomi_s = kana_s;
        next_char_s = final_char_s;
        final_char_s = malloc(sizeof(saidoku));
        final_char_s->next_s = next_char_s;
        kana_s_next = NULL;
        kan_flag = 1;
        yomi_flag = 0;
        index_flag = 0;
        ex_flag = 0;
        ex_char_flag = 0;
        ex_char[0] = '\0';
        ex_char[1] = '\0';
        pp = 0;
            continue;
        }
        if(cp == '?' && kan_flag == 1){
            final_char_s->saikan[pp] = '\0';
            okuri_flag = 1;
            kan_flag = 0;
            pp = 0;
            continue;
        }
        if(cp == '!'){
            final_char_s->okuri[pp] = '\0';
            final_char_s->okuri_n = pp;
            kan_flag = 0;
            okuri_flag = 0;
            yomi_flag = 1;
            index_flag = 1;
            pp = 0;
            kana_s = malloc(sizeof(kanaindex));
            kana_s->mae_setu_n = 0;
            kana_s->next = NULL;
            continue;
        }
        if(cp == '|'){
            kana_s->kanaindex[pp] = '\0';
            kana_s->kanaindex_n = pp;
            if(yomi_flag == 1){
                kana_s->kanayomi[pp-(kana_s->mae_setu_n)] = '\0';
                kana_s->kanayomi_n = pp-(kana_s->mae_setu_n);
            }
            if(ex_char_flag > 0)
            {
                kana_s = expand_kana(kana_s,ex_char[0],ex_char[1], ex_char_flag);/*$X$を展開させる */
            }
            pp = 0;
            kana_s_next = kana_s;
            kana_s = malloc(sizeof(kanaindex));
            kana_s->mae_setu_n = 0;
            kana_s->next = kana_s_next;
            yomi_flag = 1;
            ex_flag = 0;
            ex_char_flag = 0;
            ex_char[0] = '\0';
            ex_char[1] = '\0';
            continue;
        }
        if(cp == '=')
        {
            yomi_flag = 1;
            kana_s->kanayomi[pp] = '\0';
            strcpy((char*)kana_s->mae_setu, (char*)kana_s->kanayomi);
            kana_s->mae_setu_n = pp;
            continue;
        }
        if(cp == '-'){
            yomi_flag = 0;
            kana_s->kanayomi[pp-(kana_s->mae_setu_n)] = '\0';
            kana_s->kanayomi_n = pp-(kana_s->mae_setu_n);
            continue;
        }
        if((unsigned char)cp > 0x7F && kan_flag == 1){
            final_char_s->saikan[pp] = (unsigned char)cp;
            pp++;
            continue;
        }
        if(okuri_flag == 1){
            final_char_s->okuri[pp] = (unsigned char)cp;
            pp++;
            continue;
        }
        if(yomi_flag == 1){
            kana_s->kanayomi[pp-(kana_s->mae_setu_n)] = (unsigned char)cp;
            kana_s->kanaindex[pp] = (unsigned char)cp;
            if(cp == '$')
            {   if(ex_flag == 1)
                {
                    ex_flag = 0;
                }
                else
                {
                    ex_flag = 1;
                }
                ex_char_flag = 1;
            }
            if(ex_flag == 1)
            {
                ex_char[0] = cp;
            }
            pp++;
            continue;
        }
        if(index_flag == 1 && yomi_flag == 0){
            kana_s->kanaindex[pp] = (unsigned char)cp;
            if(cp == '$')
            {   if(ex_flag == 1)
                {
                    ex_flag = 0;
                }
                else
                {
                    ex_flag = 1;
                }
                ex_char_flag = 2;
            }
            if(ex_flag == 1)
            {
                ex_char[1] = cp;
            }
            pp++;
        }
    }
    return next_char_s;
}
/*
  訓読文でかならずカナにすべき文字（原則、訓読文で助詞、助動詞となるもの）の一覧である
  kankun.tblを読み込む。返り値は、仮名を漢字に対応させるリストであるkanakan構造体の
  ポインタである。
  */
kanakan* readdata_kanakan( void )
{
    char cp, ex_char[2];
    char full_kk_file[MAX_DIR];
    int pp, number;
    static int kan_flag,yomi_flag,index_flag,ex_flag,ex_char_flag;
    kanakan* final_char;
    kanakan* next_char;
    kanaindex* kana;
    kanaindex* kana_next;
    FILE* file_d = NULL;
    strcpy(full_kk_file,cdir_name);
    strcat(full_kk_file,"kankun.tbl");
    file_d = fopen(full_kk_file,"r");
    number = errno;
    if(file_d == NULL){
        printf("ひらがなにすべき漢字のデータファイルが開けません!\n");
        printf("データファイル%sが開けません!\n", full_kk_file);
        printf("print error string by strerror: %s\n", strerror(number));
        printf("print error code: %d\n", number);
        return NULL;
    }
    final_char = malloc(sizeof(kanakan));
    final_char->next_kan = NULL;
    next_char = NULL;
    kana = NULL;
    pp = 0;
    kan_flag = 1;
    yomi_flag = 0;
    index_flag = 0;
    ex_flag = 0;
    ex_char_flag = 0;
    ex_char[0]  = '\0';
    ex_char[1]  = '\0';
    while((cp = getc(file_d))!= EOF){
        if(cp == '%'){
            while((cp = getc(file_d))!= '\n')continue;
        }
        if(cp == '\n' && kan_flag == 1) continue;
        if(cp == '\n'){
            kana->kanaindex[pp] = '\0';
            kana->kanaindex_n = pp;
            if(yomi_flag == 1){
                if(kana->mae_setu_n<0)
                {
                    kana->kanayomi[pp] = '\0';
                    kana->kanayomi_n = pp;
                }
                else
                {
                    kana->kanayomi[pp-(kana->mae_setu_n)] = '\0';
                    kana->kanayomi_n = pp-(kana->mae_setu_n);
                }
            }
            else
            {
                if(kana->mae_setu_n > -1 )
                {
                    if(kana->mae_setu_n + kana->kanayomi_n == pp || index_flag == 0)
                        kana->ato = -1;
                    index_flag = 1;
                }
                else
                {
                    if(kana->kanayomi_n == pp || index_flag == 0)
                        kana->ato = -1;
                    index_flag = 1;
                }
            }
            if(ex_char_flag > 0)
            {
                kana = expand_kana(kana,ex_char[0],ex_char[1], ex_char_flag);/*$X$を展開させる */
            }
            final_char->yomi = kana;
            next_char = final_char;
            final_char = malloc(sizeof(kanakan));
            final_char->next_kan = next_char;
            kana_next = NULL;
            kan_flag = 1;
            yomi_flag = 0;
            index_flag = 0;
            ex_flag = 0;
            ex_char_flag = 0;
            ex_char[0]  = '\0';
            ex_char[1]  = '\0';
            pp = 0;
        }
        if(cp == '?' && kan_flag == 1){
            final_char->oya_kan[pp] = '\0';
            cp = getc(file_d);
            switch(cp){
                case 'L': final_char->jun = -1;
                    continue;
                case 'R': final_char->jun = -2;
                    continue;
                case 'E': final_char->jun = 1;
                    continue;
                default:  final_char->jun = 0;
                    continue;
            }
        }
        if(cp == '!'){
            kan_flag = 0;
            yomi_flag = 1;
            index_flag = 1;
            pp = 0;
            kana = malloc(sizeof(kanaindex));
            kana->mae_setu_n = 0;
            kana->ato = 0;
            kana->next = NULL;
            continue;
        }
        if(cp == '|'){
            kana->kanaindex[pp] = '\0';
            kana->kanaindex_n = pp;
            if(yomi_flag == 1){
                if(kana->mae_setu_n<0)
                {
                    kana->kanayomi[pp] = '\0';
                    kana->kanayomi_n = pp;
                }
                else
                {
                    kana->kanayomi[pp-(kana->mae_setu_n)] = '\0';
                    kana->kanayomi_n = pp-(kana->mae_setu_n);
                }
            }
            else
            {
                if(kana->mae_setu_n > -1 )
                {
                    if(kana->mae_setu_n + kana->kanayomi_n == pp || index_flag == 0)
                        kana->ato = -1;
                    index_flag = 1;
                    //      if(kana->kanayomi_n == 0)
                    //          kana->ato = 1;
                }
                else
                {
                    if(kana->kanayomi_n == pp || index_flag == 0)
                        kana->ato = -1;
                    index_flag = 1;
                    //     if(kana->kanayomi_n == 0)
                    //         kana->ato = 1;
                }
            }
            if(ex_char_flag > 0)
            {
                kana = expand_kana(kana,ex_char[0],ex_char[1], ex_char_flag);/*$X$を展開させる */
            }
            pp = 0;
            kana_next = kana;
            kana = malloc(sizeof(kanaindex));
            kana->next = kana_next;
            kana->mae_setu_n = 0;
            kana->ato = 0;
            yomi_flag = 1;
            ex_flag = 0;
            ex_char_flag = 0;
            ex_char[0]  = '\0';
            ex_char[1]  = '\0';
            continue;
        }
        if(cp == '-'){
            if(yomi_flag == 1){
                yomi_flag = 0;
                if(kana->mae_setu_n < 0)
                {
                    kana->kanayomi[pp] = '\0';
                    kana->kanayomi_n = pp;
                }
                else
                {
                    kana->kanayomi[pp-(kana->mae_setu_n)] = '\0';
                    kana->kanayomi_n = pp-(kana->mae_setu_n);
                }
            }
            else{
                index_flag = 0;
                kana->kanayomi[0] = '\0';
                kana->kanayomi_n = 0;
                kana->kanaindex[pp] = '\0';
                kana->kanaindex_n = pp;
            }
            
            continue;
        }
        if(cp == '=')
        {
            yomi_flag = 1;
            kana->kanayomi[pp] = '\0';
            strcpy((char*)kana->mae_setu, (char*)kana->kanayomi);
            if(pp == 0){
                kana->mae_setu_n = -1;
            }
            else
            {
                kana->mae_setu_n = pp;
            }
            kana->kanayomi_n = 0;
            continue;
        }
        if((unsigned char)cp > 0x7F && kan_flag == 1){
            final_char->oya_kan[pp] = (unsigned char)cp;
            pp++;
            continue;
        }
        if(yomi_flag == 1){
            if(kana->mae_setu_n < 0){
                kana->kanayomi[pp] = (unsigned char)cp;
            }
            else
            {
                kana->kanayomi[pp-(kana->mae_setu_n)] = (unsigned char)cp;
            }
            kana->kanaindex[pp] = (unsigned char)cp;
            if(cp == '$')
            {   if(ex_flag == 1)
            {
                ex_flag = 0;
            }
            else
            {
                ex_flag = 1;
            }
                ex_char_flag = 1;
            }
            if(ex_flag == 1)
            {
                ex_char[0] = cp;
            }
            pp++;
            continue;
        }
        if(index_flag == 1 && yomi_flag == 0){
            kana->kanaindex[pp] = (unsigned char)cp;
            if(cp == '$')
            {   if(ex_flag == 1)
            {
                ex_flag = 0;
            }
            else
            {
                ex_flag = 1;
            }
                ex_char_flag = 2;
            }
            if(ex_flag == 1)
            {
                ex_char[1] = cp;
            }
            pp++;
        }
    }
    return next_char;
}
/*
 out_fileに数字などを書き込む。
 */
void org_file_message_with_int(unsigned char* s1, int dix, unsigned char* s2)
{
    if(out_file != NULL)fprintf(out_file, "%s%d%s\n",s1,dix,s2);
}
void org_file_message(unsigned char* s1)
{
    if(out_file != NULL)fprintf(out_file, "%s\n",s1);
}
/*
 out_file(.orgを拡張子とするもの）のヘッダの説明書きをつける。
 */
void org_file_usage( void )
{
    if(out_file != NULL)fprintf(out_file,"このファイルは、訓読文（書き下し文）から漢文の語順が正しく判定されていることを判断するためのものです。\n");
    if(out_file != NULL)fprintf(out_file,"処理している各文について、白文、訓読文が出ます。次に漢字の位置が00〜100の数字、白文中の当該漢字、\n");
    if(out_file != NULL)fprintf(out_file,"送り仮名（平仮名が漢字にしたもの、再読文字の送り仮名は出ません）、訓読順序（これから返点が打たれます）、\n");
    if(out_file != NULL)fprintf(out_file,"文中の漢字の重複（０だと重複はありません）、性質（ここに、訓読文でひらがなにする、副詞、特殊な返読\n");
    if(out_file != NULL)fprintf(out_file,"などの属性がつけてあります。ただ、あくまで可能性です、-1であれば属性がありません）、\n");
    if(out_file != NULL)fprintf(out_file,"そして、最後に、繋がりとなっているのが、語順の判断です。内容は以下のとおりです。\n");
    if(out_file != NULL)fprintf(out_file,"000 は、訓読文中で、ひらがなとなっている漢字、もしくは置字。句読点もこうなります。\n");
    if(out_file != NULL)fprintf(out_file,"444 は漢字の繋がり（順読か、返読）がこの文法解析で妥当と判断されたもの\n");
    if(out_file != NULL)fprintf(out_file,"-93は、後の漢字との繋がりがこの文法解析で不当と判断されたもの\n");
    if(out_file != NULL)fprintf(out_file,"−92は、前の漢字との繋がりがこの文法解析で不当と判断されたもの\n");
    if(out_file != NULL)fprintf(out_file,"-95は、前後の漢字との繋がりがいずれもこの文法解析で不当と判断されたもの\n");
    if(out_file != NULL)fprintf(out_file,"-91は、繋がりの不当性に熟語等の問題が考えられるもの\n");
    if(out_file != NULL)fprintf(out_file,"999は、繋がりの妥当性が判定できなかったもの\n");
    if(out_file != NULL)fprintf(out_file,"但し、999でも訓点が打てなかったとは言えません。訓順を確認してみて下さい。\n\n");
    if(out_file != NULL)fprintf(out_file,"なお、以上の判定はあくまで訓点を打つ最終段階の状態を示します。漢字に訓読順を当てはめる段階では\n");
    if(out_file != NULL)fprintf(out_file,"何度か、同様な判定を行い、最も蓋然性の高い訓読順を当てはめる操作がされています。\n\n\n");
    if(out_file != NULL)fprintf(out_file,"ーーーー解析結果ーーーー\n");
}
void usage( void )
{
    printf("これは白文と訓読文からTeXのソースファイルを生成するプログラムです。\n");
    printf("kuntenに続いて、白文、訓読文、書式設定のファイルを記述してください。\n");
    printf("例：$ kunten haku.txt kundoku.txt shosiki.hd\n");
    printf("それぞれは、スペースで区切ってください。\n");
    printf("なお、白文、訓読文のファイルは、.txt拡張子がついているものと見做しますので、xx.txt\n");
    printf("ファイルは、xxだけでも構いません。また、shosikiファイルも、.hd拡張子がついたものと\n");
    printf("見做しますので、shosikiだけで構いません。\n");
    printf("このパッケージには、uplatex用のB4,B5横のb4y,b5y、およびA4縦のa4tと、lualatex用の\n");
    printf("B4,B5横のlub4y,lub5y、およびA4縦のlua4tを準備しています。\n");
    printf("このkuntenには、次のようなオプションスイッチをつけることができます\n\n");
    printf("-t: これはkuntenが使っている訓読文の文法解析がどの程度うまくいっているかを\n");
    printf("テストするためです。texファイルの生成と同時に、.orgという拡張子のファイル\n");
    printf("を生成します。解析内容については、.orgというファイルの冒頭を見てください。\n");
    printf("-v: これには、allとpartialがありますが、いずれも各プログラムのルーチンがどういった\n");
    printf("働きをしているかを吐き出してくれます。動きがおかしいようでしたら、できれば-v allオプション\n");
    printf("で吐き出されたログも含めて作者に連絡していただけると助かります。\n");
    printf("-h: この使用法を表示します。これをつけると後にファイル名等を指定しても終了します。\n\n");
    printf("kuntenは白文と訓読文を句点で一文を分け処理します。白文と訓読文は、\n");
    printf("かならず、一文を「。」で区切り、両者が一致していなければなりません。\n");
    printf("ただし、「・・。」と。のような場合は、「と。」までを一文と扱います。\n");
    printf("訓点をつけるべき白文は＜＞で囲んでください。\n");
    printf("白文、訓読文のファイルはテキストファイルの形式でであれば、拡張子はなんでもかまいません。\n");
    printf("ただ、txt以外の拡張子の場合には、必ず拡張子までつけて下さい。\n");
    printf("ただし、文字コードはUnicodeの符号であるutf-8専用です。\n");
    printf("Windowsなどでしばしば用いられるutf-16,utf-32などには対応していません。\n");
    printf("メモ帳などにコピーして保存したものを使ってください。\n");
    printf("なお、どうしても返点（送り仮名）の付け方がおかしい場合には、白文の当該漢字の下にxを「曰x」の\n");
    printf("ようにつけてみて下さい。ただし、これは「曰」に文法解析は行わないという意味であり、ただ出現順に\n");
    printf("返点を打つという意味なので、うまくいかない場合一文に複数回出現する他の箇所でも試してみて下さい。\n");
    printf("ただし、書き下し文でひらがなとなっている漢字には適用できません。\n");
    printf("\n");
    printf("問題がありましたら、kappakappa.yasu@gmail.com　または、\n");
    printf("kappa-y@nn.iij4u.or.jpまでご連絡ください。\n");
}

void message( void )
{
    printf("kunten ver. 0.99b1 date 2024. 7. 15\n");
    printf("This is a preprocessor of \"kunten\" TeX typesettig\n");
    printf("programmed by Yasuhiro Okazaki\n");
    printf("kappa-y@nn.iij4u.or.jp or kappakappa.yasu@gmail.com\n");
#ifdef WINDOWS
    printf("If you want to read my explanation of this program,\n");
    printf("you should type \"chcp 65001\" in your command prompt.\n");
    printf("The command \"chcp\" means change code page. This program uses\n");
    printf("utf-8, not Shift-JIS which is the defoult code in Windows.\n");
#endif
}
int main( int argc, char *argv[] )
{
    char    con_def_file[128];
    char    con_def_dir[128];
    char    input_file_name1[128];
    char    input_file_name2[128];
    char    org_file_name[128];
    char    output_file_name[128];
    int     count_kan;
    int     kan_read_count,temp_end;
    char    head_def_name[128];
    char    temp_file_name[4][128];
    char    full_log_file[128];
    char*   org_temp;
    char*   out_temp;
    char    opc;
    const char* opt_string = "htv:";
    char    log_file_name[12] = "kunten.log";
    int     ind,ret;
    int     kan_possible;
    unsigned char* kan_start = NULL;
    unsigned char* kun_start = NULL;
    oyakan*     read_haku;
    message();
    g_test = 0;
    verbose = 0;
#ifdef WINDOWS
    getcwd(exe_root,MAX_DIR);
    strcpy(con_def_dir, exe_root);
    strcpy(cdir_name,exe_root);
    strcat(cdir_name,"\\data\\list\\");
    strcat(con_def_dir,"\\data\\style\\");
    printf("Executable Dir = %s\n",exe_root);
    strcat(exe_root,"\\data\\");
#else
    strcpy(exe_root, argv[0]);
    strrtok(exe_root,'/');
    strcpy(con_def_dir, exe_root);
    strcpy(cdir_name,exe_root);
    strcat(cdir_name,"/data/list/");
    strcat(con_def_dir,"/data/style/");
    printf("Executable Dir = %s\n",exe_root);
    strcat(exe_root,"/data/");
#endif
    strcpy(full_log_file,exe_root);
    strcat(full_log_file,log_file_name);
    log_f = fopen(full_log_file,"w");
    if(log_f == NULL){
        printf("ログファイルが作れません。\n");
        log_on = 0;
    }
    else{
        printf("ログファイルは%sの中に%sという名で保存されます。\n",exe_root,log_file_name);
        log_on = 1;
    }
    count_kan = 0;
    ind = 0;
    while ((opc = getopt(argc, argv, opt_string)) != -1) {
        switch (opc) {
          case 'H':
          case 'h':
            usage();
                return 1;
          case 'T':
          case 't':
                g_test = 1;
                break;
          case 'V':
          case 'v':
                if(!strcmp(optarg,"all")) verbose = 2;
                if(!strcmp(optarg,"partial")) verbose = 1;
                if(verbose > 0){
                    break;
                }
                else{
                    printf("Unknown Argument for v %s\n",optarg);
                    break;
                }
          default:
            printf("error! \'%c\' \'%c\'\n", opc, optopt);
            return 1;
        }
    }
    while (optind < argc) {
        strcpy(temp_file_name[ind], argv[optind]);
        ind++;
        optind++;
    }
    kan_possible = 1;
    if(ind > 0){ strcpy(input_file_name1,temp_file_name[0]);}
    else{
        printf("処理対象のファイルがありません。処理対象は、テキスト形式で書かれた\n");
        printf("白文のファイルが必要です。拡張子が書かれていない場合は、txtが拡張\n");
        printf("子と見做します。Windowsのメモ帳などで保存するとtxtファイルになっ\n");
        printf("ているはずです。\n");
        printf("このプログラムは処理対象のファイルがないので用法説明をしてこのまま終了します。\n");
        usage();
        return 9;
    }
    if(ind > 1) {strcpy(input_file_name2,temp_file_name[1]);}
    else{printf("書き下し文のファイルの指定がありません。このままでは、漢文の処理ができません。\n");
         printf("漢文の処理をするのであれば、このプログラムは中断します。\n");
        kan_possible = 0;
    }
    if(ind > 2) {strcpy(con_def_file,temp_file_name[2]);}
    else{
        printf("TeX用のヘッダファイルが指定されていません。TeX処理をするためには\n");
        printf("自分で書き加えるかやり直してください。\n");
    }
    strcpy(output_file_name,input_file_name1);
    if(strchr(input_file_name1,'.') == NULL){
        strcat(input_file_name1,".txt");
    }
    strcpy(org_file_name,input_file_name1);
    if(strchr(input_file_name2,'.') == NULL)
        strcat(input_file_name2,".txt");
    if(g_test_read()){//#ifdef DEBUG
        if(strchr(org_file_name,'.') == NULL){
            strcat(org_file_name,".org");
        }
        else{
            org_temp = strtok(org_file_name,".");
            strcat(org_temp,".org");
        }
    }//#endif
    if(strchr(output_file_name,'.') == NULL){
        strcat(output_file_name,".tex");
    }
    else{
        out_temp = strtok(output_file_name, ".");
        strcat(out_temp,".tex");
    }
    if(strchr(con_def_file,'.') == NULL){
        strcat(con_def_file,".hd");
    }
    strcpy(head_def_name,con_def_dir);
    strcat(head_def_name,con_def_file);
    read_kan_com( exe_root );
    read_mon_com( exe_root );
    kanakan_moji = readdata_kanakan();
    saidoku_moji = readdata_sai();
    root_itai = readdata_itai();
    root_okuri = readdata_short_okuri("okuri.tbl","特殊な送り仮名","");
    okikana_root =readdata_long_okuri("okikana.tbl", "他に挟まれる助字", "");
    hendoku_moji = readdata_long_okuri("hendoku.tbl","特殊な返読文字","GECASTYWHNMR");
    hendoku_moji2 = readdata_long_okuri("hendoku2.tbl", "別の読み方をする返読文字用の補助リスト１", "GECASTYWHNMR");
    hendoku_moji3 = readdata_long_okuri("hendoku3.tbl", "別の読み方をする返読文字用の補助リスト２", "GECASTYWHNMR");
    hendoku_moji4 = readdata_long_okuri("hendoku4.tbl", "別の読み方をする返読文字用の補助リスト３", "GECASTYWHNMR");
    hendoku_moji5 = readdata_long_okuri("hendoku5.tbl", "別の読み方をする返読文字用の補助リスト４", "GECASTYWHNMR");
    daisi = readdata_long_okuri("daisi.tbl","代詞","D");
    fukusi = readdata_short_okuri("fukusi.tbl","副詞","NTYMO");
    kaisi = readdata_short_okuri("kaisi.tbl","介詞","WFGQ");
    siyeki = read_kan_list("siyeki.tbl", "使役の助動詞で読む漢字");
    hitei_hukusi = read_kan_list("hiteihukusi.tbl", "否定の副詞となる漢字");
    no_hendoku = readdata_short_okuri("nonhendoku.tbl", "返読文字と紛れやすい漢字", "");
    haku_file = fopen(input_file_name1,"r");
    if(haku_file == NULL){
        printf("白文ファイルが開けません。正しい白文ファイルを指定して下さい。\n");
        printf("このプログラムの用法を示して、終了します。\n");
        usage();
        return 9;
    }
    kun_file = fopen(input_file_name2,"r");
    if(kun_file == NULL){
        printf("書き下し文ファイルが開けません\n");
        return 9;
    }
    if(g_test_read()){//#ifdef DEBUG
        out_file = fopen(org_file_name,"w");
        if(out_file == NULL){
            printf("出力ファイルが開けません\n");
        }
        else{
            org_file_usage();
        }
    }
//#endif
    tex_file = fopen(output_file_name,"w");
    if(tex_file == NULL){
        printf("TEXファイルが開けません\n");
    }
    tex_header(tex_file, head_def_name);
    write_read_num( 0 );
    write_file_end_flag( 0 );
    write_kan_haku_flag( 0 );
    init_kakko_u();
    env_nest_init();
    bun_line = 0;
    write_kuten_error(0);
    kan_read_count = 0;
    temp_end = 0;
    while(get_file_end_flag() == 0)
    {
        
        kan_start = read_buf_kan(haku_file,tex_file);
        //if(kan_start == NULL) continue;
        if(kan_start != NULL ){
            if(kan_possible == 0) return 9;
            count_kan++;
            write_read_num(count_kan);
            read_haku = set_kanji(kan_start);
            kun_start = read_buf_kun(kun_file);
            ret = process_kan(kun_start);
            if(ret < 0) {
                closing_kan(tex_file);
                goto the_end;
            }
           if(!get_kan_haku_flag()&& get_file_end_flag()){
                kan_read_count++;
               temp_end = 1;
      //          goto the_end;
            }
            bun_line++;
//#ifdef DEBUG
            //if(read_every() != 0) {
            if(g_test_read()){

                out_put_kan_index(kan_start, kun_start, bun_line);

            }
            else{//#else
                if(read_non_sai_okiji()){
                    wc_print(文番号:);
                    wd_print(bun_line);
                    wc_print(:白   文:);
                    ind = (int)strlen((char*)kan_start);
                    if(*(kan_start + ind - 1) == '\n'){
                        *(kan_start + ind - 1) = '\0';
                    }
                    wv_print(kan_start);
                    wc_print(\n);
                    wc_print(文番号:);
                    wd_print(bun_line);
                    wc_print(:訓読文:);
                    wv_print(kun_start);
                    wc_print(\n);
                    printf("文番号:%d:白　文:%s\n",bun_line,kan_start);
                    printf("文番号:%d:訓読文:%s\n",bun_line,kun_start);
                }
            }
//#endif
            print_kunten(tex_file);
            if(temp_end)closing_kan(tex_file);
#ifdef DEBUG_2
            
            out_put_kan_index(kan_start, kun_start, bun_line);
            index_free();
#endif
        }
        if(!get_kan_haku_flag() && !get_file_end_flag()) {
            closing_kan(tex_file);
            kan_read_count++;
            wc_print(第);
            wd_print(kan_read_count);
            wc_print(節終了\n);
            printf("第%d節終了\n",kan_read_count);
        }
#ifdef DEBUG_4
        if(bun_line == stop_line){
            printf("please stop\n");
            debug_stop(1);
            out_put_kan_index(kan_start, kun_start, bun_line);
        }
#endif
    }
the_end:    
    wd_print(kan_read_count);
    wc_print(節を処理しました\n);
    printf("%d節を処理しました\n",kan_read_count);
    
    wd_print(bun_line);
    wc_print(文を処理しました\n);
    printf("%d 文を処理しました。\n",bun_line);
    fclose(haku_file);
    fclose(kun_file);
//#ifdef DEBUG
    if(g_test_read()){
        fclose(out_file);
    }
//#endif
    tex_ending(tex_file);
    fclose(tex_file);
}
