//
//  tex_command.c
//  kunten
//
//  Created by 岡崎康浩 on 2023/05/29.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tex_command.h"
#include "oyakan.h"
#include "utfstring.h"
#include "my_math.h"
#include "my_tex_command.h"
#define  MAX_LINE       4096
#define  MAX_TEX_LINE   6144

int             set;
int             unit_index;
con_seq*        nest[4];
char            temp_seq[20];
con_seq*        control_s[20];
char            pri[5][13];
char            line[MAX_LINE];
char            t_line[128];
unsigned char   ori_line[MAX_LINE];
unsigned char   tex_line[MAX_TEX_LINE];
void    choice_quest_tate(FILE* o_file, enum_com* temp);
void    choice_quest_yoko(FILE* o_file, enum_com* temp);

/*
 これは、control_sのコントロールシークエンスを引き渡すための関数です。引数はcontrol_sのインデックスナンバーです。
 */
char*   get_con_seq( int index )
{
    return zatu_con[index];
}
unsigned char* get_ori_line( void )
{
    return ori_line;
}
/*
 env_nest_num  を初期化,取得する。
 */
void     env_nest_init(void)
{
    env_nest_num = 0;
}
int     get_env_nest(void)
{
    return env_nest_num;
}
/*
 int    init_enum(int index, unsigned char* line)
 */
enum_com*    init_enum(int index, unsigned char* line)
{
    enum_com*       temp_enum;
    items*          temp_item;
    unsigned char*  bp_st;
    unsigned char*  bp_end;
    int             item_num,i;
    item_num  = 0;
    temp_enum = malloc(sizeof(enum_com));
    if(temp_enum == NULL){
        printf("選択肢用のメモリが足りませんでした。");
        return NULL;
    }
    temp_enum->title = index;
    for(i=0;i<10;i++){
        temp_enum->each_item[i] = NULL;
    }
    bp_st = (unsigned char*)strchr((char*)line,'$');
    bp_end = bp_st;
    if(bp_st != NULL){
        bp_st = bp_st + 1;
        bp_end = (unsigned char*)strchr((char*)bp_st,'&');
        cut_from_string((unsigned char*)temp_enum->env_mark, bp_st, bp_end);
        bp_st = bp_end + 1;
    }
    else{
        bp_end = (unsigned char*)strchr((char*)line,'&');
        strcpy(temp_enum->env_mark,"");
        bp_st = bp_end + 1;
    }
    while(bp_end != NULL){
        temp_item = malloc(sizeof(items));
        if(temp_item == NULL){
            printf("選択肢用のメモリが足りませんでした。");
            temp_enum->item_num = item_num;
            return temp_enum;
        }
        bp_end = (unsigned char*)strchr((char*)bp_st,'&');
        if(bp_end == NULL){
            strcpy((char*)temp_item->item_str,(char*)bp_st);
            temp_item->head_com = index;
            temp_enum->each_item[item_num] = temp_item;
            item_num++;
            break;
        }
        else{
            cut_from_string(temp_item->item_str, bp_st, bp_end);
            temp_item->head_com = index;
            temp_enum->each_item[item_num] = temp_item;
            item_num++;
        }
        bp_st = bp_end + 1;
    }
    temp_enum->item_num = item_num;
    return temp_enum;
}
void    choice_quest_tate(FILE* o_file,enum_com* temp)
{
    int i;
    
    fprintf(o_file,"\n\\begin{%s}\n",env_name[1]);
    fprintf(o_file,"\\item[\\rm %s]",temp->env_mark);
    for(i=0;i<temp->item_num;i++){
        if(i<temp->item_num - 1){
            fprintf(o_file,"%s %s\\hskip2\%s",enum_myitem_label[temp->title],temp->each_item[i]->item_str,moji_unit_list[unit_index]);
        }
        else{
            fprintf(o_file,"%s %s\n",enum_myitem_label[temp->title],temp->each_item[i]->item_str);
        }
        free(temp->each_item[i]);
    }
    free(temp);
    fprintf(o_file,"\n\\setcounter{myitem}{0}\\end{%s}",env_name[1]);
}
void    choice_quest_yoko(FILE* o_file, enum_com* temp)
{
    int i,ind;
    ind = temp->item_num;
    fprintf(o_file,"\n\\raisebox{%.2f%s}{%s}\n",raise_value[mondai_moji_size],moji_unit_list[unit_index],temp->env_mark);
    fprintf(o_file,"\\begin{%s}<y>{%.1f%s}\n",env_name[6],ind*1.5,moji_unit_list[unit_index]);
    fprintf(o_file,"$\\overbrace{\\begin{%s}<t>{l}\n",env_name[7]);
    for(i=0;i<ind;i++){
        fprintf(o_file,"%s %s\\\\\n",enum_myitem_label[temp->title],temp->each_item[i]->item_str);
        free(temp->each_item[i]);
    }
    free(temp);
    fprintf(o_file,"\\end{%s}}$\n\\setcounter{myitem}{0}\\end{%s}\n",env_name[7],env_name[6]);
}
/*
 TeXのソースファイル上に、訓読文（訓点付き漢文）の領域を始める場合に必要な環境を書き込む。
 */
void opening_kan( FILE* temp_o_file )
{
    int     moji_index;
    double      width;
    moji_index = read_haku_moji_mode(2);
    fprintf(temp_o_file,"\n%s\n",(char*)moji_size_list[moji_index]);
    width = read_haku_moji_mode_width();
    fprintf(temp_o_file,"\n\\begin{%s}{%.1lf}\n",env_name[0],width);
}
void closing_kan( FILE* temp_o_file )
{
    fprintf(temp_o_file,"\n\\end{%s}\n",env_name[0]);
    //printf("%s\n",moji_size_list[mondai_moji_size]);
    fprintf(temp_o_file,"%s\n",moji_size_list[mondai_moji_size]);
}
char* set_label_form( char* mark)
{
    int     temp_index;
    temp_index = label_kind_index * 2;
    mark[0] = label_index[temp_index];
    mark[1] = label_index[temp_index + 1];
    mark[2] = '\0';
    return mark;
}

/*
 次の関数は配列arrayの中の要素がbufの中に出現するかどうかを確かめる。戻り値はarrayの何番目の要素という数字
 */
int     find_list(unsigned char* buf, unsigned char array[][16], int max,int mod)
{
    char*  lp;
    int     i;
    lp = (char*)buf;
    i = 0;
    while(i < max){
        lp = strstr((char*)buf,(char*)array[i]);
        if(lp != NULL ) {
            if(mod > 0){
                if (*(lp-1) == '=') return i;
            }
            else{
                return i;
            }
        }
        i++;
    }
    return -1;
}
int find_com_index(unsigned char* buf, unsigned char array[][16], int max)
{
    int     lp;
    int     i;
    lp = 1;
    i = 0;
    while(i < max){
        lp = b3_strlcmp(buf,array[i]);
        if(lp == 0 ) {
                return i;
            }
        i++;
    }
    return -1;
}
/*
 環境の導入と解除
 */
int     intro_env( char* env_name )
{
    strcpy(nest_env[env_nest_num].env_name,env_name);
    env_nest_num++;
    return env_nest_num;
}
int    end_env(FILE* o_file)
{
    if(env_nest_num < 1) return 0;
    env_nest_num--;
    fprintf(o_file,"\n\\end{%s}\n",nest_env[env_nest_num].env_name);
    
    return env_nest_num;
}

char* env_close(con_seq* con_t, int res)
{
    int q;
    char*   lp;
    char    temp[30];
    if(con_t->mod != 'R' && con_t->mod != 'r' && con_t->mod != 'p') return NULL;
    if(con_t->mod == 'R')
    {
        strcpy(temp, con_t->cont_seq);
        lp = strstr(temp,"\\begin{");
        if(lp != NULL){
            lp = lp + 7;
            lp = strtok(lp,"}");
            strcpy(temp_seq,"\\end{");
            strcat(temp_seq,lp);
            strcat(temp_seq,"}");
            return temp_seq;
        }
    }
    if(con_t->mod == 'r')
    {
        q = res - 1;
        while(q >= 0)
        {
            if(control_s[q]->mod == 'e'){
            strcpy(temp_seq,control_s[q]->cont_seq);
            return temp_seq;
            }
            q = q - 1;
        }
    }
    if(con_t->mod == 'p')
    {
        strcpy(temp_seq,"}");
        return temp_seq;
    }
    return NULL;
}

/*
 texファイルにするときのコントロールシークエンスで、漢文を読むのに必要なもののうち外部で操作できる一覧
 kancom/iniファイルを読み込む。戻り値は読み込んだ命令の数。
 */
int    read_kan_com( char* dir )
{
    FILE* i_file;
    int     result, ret_n, read_flag, pp;
    char    con_dir[128];
    unsigned char    line[MAX_LINE];
    unsigned char*   line_bp;
    unsigned char*   read_bp;
    unsigned char    lit;
    double           width;
    char*   temp_lit;
    char    con_file[15] = {"kancom.ini"};
    strcpy(con_dir,dir);
    strcat(con_dir,con_file);
    i_file = fopen( con_dir, "r");
    if(i_file == NULL)
    {
        printf("漢文用のコントロールシークエンスの表が読み込めません\n");
        return 1;
    }
    result = 0;
    read_flag = 1;
    pp = 0;
    init_buf(line,MAX_LINE);
    line_bp = line;
    while((signed char)(lit = getc(i_file)) != EOF){
        if(lit != '\n'){
            if(lit == '%') read_flag = 0;
            if(read_flag == 1){
                *line_bp = lit;
                line_bp++;}
                continue;
        }
        if(read_flag == 0){
            read_flag = 1;
            continue;
        }
        result =  find_list(line, init_com_list,9,0);
        switch(result){
            case 0: read_bp = (unsigned char*)strstr((char*)line,(char*)init_com_list[0]);
                ret_n = find_list(read_bp,kundoku_mode,6,1);
                init_write_haku_moji_mode(ret_n, -1, -1);
                break;
            case 1: read_bp = (unsigned char*)strstr((char*)line,(char*)init_com_list[1]);
                ret_n = find_list(read_bp,moji_size_kan,9,1);
                init_write_haku_moji_mode(-1, ret_n, -1);
                break;
            case 2: read_bp = (unsigned char*)strstr((char*)line,(char*)init_com_list[2]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                sscanf((char*)read_bp,"=%lf",&width);
                init_write_haku_moji_mode(-1, -1, width);
                break;
            case 3: read_bp = (unsigned char*)strstr((char*)line,(char*)init_com_list[3]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                temp_lit = line_adjust_com;
                sscanf((char*)read_bp,"=%lf",&st_adjust_dim);
                sprintf(temp_lit,"[%.1lf]",st_adjust_dim);
                break;
            case 4: read_bp = (unsigned char*)strstr((char*)line,(char*)init_com_list[4]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                ret_n = find_list(read_bp,label_form_index,8,1);
                label_kind_index = ret_n;
                break;
            case 5: read_bp = (unsigned char*)strstr((char*)line,(char*)init_com_list[5]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                break;
            }
        init_buf(line,MAX_LINE);
        line_bp = line;
    }
    fclose(i_file);
    return result;
}
/*
 texファイルにするときのコントロールシークエンスで、問題を作るのに必要なもののうち外部で操作できる一覧
 kancom/iniファイルを読み込む。戻り値は読み込んだ命令の数。
 */
int    read_mon_com( char* dir )
{
    FILE* i_file;
    int     result, ret_n, read_flag, pp,temp_int;
    char    con_dir[128];
    unsigned char    line[MAX_LINE];
    unsigned char*   line_bp;
    unsigned char*   read_bp;
    unsigned char    lit;
    char    con_file[15] = {"mondaicom.ini"};
    strcpy(con_dir,dir);
    strcat(con_dir,con_file);
    i_file = fopen( con_dir, "r");
    if(i_file == NULL)
    {
        printf("問題用のコントロールシークエンスの表が読み込めません\n");
        return 1;
    }
    result = 0;
    read_flag = 1;
    pp = 0;
    init_buf(line,MAX_LINE);
    line_bp = line;
    while((signed char)(lit = getc(i_file)) != EOF){
        if(lit != '\n'){
            if(lit == '%') read_flag = 0;
            if(read_flag == 1){
                *line_bp = lit;
                line_bp++;}
                continue;
        }
        if(read_flag == 0){
            read_flag = 1;
            continue;
        }
        result =  find_list(line, init_mondai_list,9,0);
        switch(result){
            case 0: read_bp = (unsigned char*)strstr((char*)line,(char*)init_mondai_list[0]);
                ret_n = find_list(read_bp,moji_size_kan,9,1);
                if(ret_n < 0){
                    printf("文字の大きさにそのようなものはありません。\n");
                }
                else{
                    my_title.moji_size = ret_n;
                }
                break;
            case 1: read_bp = (unsigned char*)strstr((char*)line,(char*)init_mondai_list[1]);
                ret_n = find_list(read_bp,moji_size_kan,9,1);
                if(ret_n < 0){
                    printf("文字の大きさにそのようなものはありません。\n");
                }
                else{
                    my_title.sub_moji_size = ret_n;
                }
                break;
            case 2: read_bp = (unsigned char*)strstr((char*)line,(char*)init_mondai_list[2]);
                ret_n = find_list(read_bp,moji_size_kan,9,1);
                if(ret_n < 0){
                    printf("文字の大きさにそのようなものはありません。\n");
                }
                else{
                    my_daimon.moji_size = ret_n;
                }
                break;
            case 3: read_bp = (unsigned char*)strstr((char*)line,(char*)init_mondai_list[3]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                sscanf((char*)read_bp,"=%d",&temp_int);
                if(temp_int < 0){
                    my_daimon.position = 0;
                }
                else{
                    my_daimon.position = temp_int;
                }
                break;
            case 4: read_bp = (unsigned char*)strstr((char*)line,(char*)init_mondai_list[4]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                ret_n = find_list(read_bp,moji_size_kan,9,1);
                if(ret_n < 0){
                    printf("文字の大きさにそのようなものはありません。\n");
                }
                else{
                     mondai_moji_size = ret_n;
                }
                break;
            case 5: read_bp = (unsigned char*)strstr((char*)line,(char*)init_mondai_list[5]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                ret_n = find_list(read_bp,enum_shomon_index,3,1);
                if(ret_n < 0){
                    printf("小問の形式にそのようなものはありません。\n");
                }
                else{
                     shomon_style = ret_n;
                }
                break;
            case 6: read_bp = (unsigned char*)strstr((char*)line,(char*)init_mondai_list[6]);
                read_bp = (unsigned char*)strchr((char*)read_bp, '=');
                ret_n = find_list(read_bp,enum_form_index,5,1);
                if(ret_n < 0){
                    printf("選択肢記号にそのようなものはありません。\n");
                }
                else{
                     choice_question_default = ret_n;
                }
                break;
            }
        init_buf(line,MAX_LINE);
        line_bp = line;
    }
    fclose(i_file);
    return result;
}
/*
 問題文を一文として読み込む。ここでは、単一の文字（多バイトであっても）を読んで、一行の文にする。
 引数は文字（１バイトでも３バイトであっても）、文の終わりであるかどうかを示すflag、これが０であれば
 引き続き文にする。１であれば、そこから新しい行を始める。戻り値は文の終わりであれば１そうでなければ０
 */
int read_text(unsigned char* t_char, int end_flag)
{
    static int      ref_flag;
    static char     lit[2];
    if(end_flag == 1) {
        init_buf(ori_line,MAX_LINE);
        ref_flag = 0;
    }
    end_flag = 0;
    if(( *t_char < 0x80) || is_utf_3b_ascii(t_char)){
        lit[0] = b3_to_ascii(t_char);
        lit[1] = '\0';
        if(lit[0] == 'R' || lit[0] == 'r'){
            ref_flag = 1;
            return 0;
        }
        if(lit[0] == '\r') return 0;
        strcpy((char*)t_char,(char*)lit);
    }
    if(*t_char == '(')
                strcpy((char*)t_char,"（");
    if(*t_char == ')')
                strcpy((char*)t_char,"）");
    if(!strcmp((const char*)t_char,"（") && ref_flag){
            strcpy((char*)t_char,"");
            strcat((char*)ori_line,"\\ref{");
        return 0;
    }
    if(!strcmp((const char*)t_char,"）") && ref_flag){
            strcpy((char*)t_char,"}");
            ref_flag = 0;
    }
    strcat((char*)ori_line,(char*)t_char);
    if(*t_char == '\n') end_flag = 1;
    return end_flag;
}

int interpret_tex(FILE* o_file, unsigned char* line)
{
    unsigned char   temp_line[64];
    unsigned char   temp_item[64];
    unsigned char*  bp_ori;
    unsigned char*  bp_temp;
    unsigned char*  bp_end;
    enum_com*       choice_quest;
    char*           dim_point;
    double          width;
    int             com_index,colunm,env_end;
    init_buf(tex_line, MAX_TEX_LINE);
    env_end = 0;
    bp_ori  = line;
    bp_temp = (unsigned char*)strchr((char*)line, (int)'#');
    if(bp_temp == NULL){
        bp_temp = (unsigned char*)strchr((char*)line,(int)'&');
        if(bp_temp == NULL){
            bp_end = (unsigned char*)strchr((char*)line,'/');
            if(bp_end != NULL){
                cut_from_string(temp_line, line, bp_end);
                fputs((const char*)temp_line,o_file);
                env_end = 1;
            }
            else{
                fputs((const char*)line, o_file);
            }
            if(env_end == 1)end_env(o_file);
            return 0;
        }
        cut_from_string(temp_line, bp_ori, bp_temp);
        strcpy((char*)tex_line,(char*)temp_line);
        strcat((char*)tex_line,"\\item ");
        bp_temp = bp_temp + 1;
        bp_end = (unsigned char*)strchr((char*)bp_temp,'/');
        if(bp_end != NULL){
            cut_from_string(temp_line, bp_temp, bp_end);
            bp_temp = temp_line;
            env_end = 1;
        }
        strcat((char*)tex_line,(char*)bp_temp);
        fputs((const char*)tex_line, o_file);
        if(env_end == 1) end_env(o_file);
        return 0;
    }
    cut_from_string(temp_line, bp_ori, bp_temp);
    strcpy((char*)tex_line,(char*)temp_line);
    bp_temp = bp_temp + 1;
    com_index = find_com_index(bp_temp, style_index, 11);
    if(com_index < 0){
        printf("命令は、表題,大問,小問,選択縦並び,選択横並び,選択括弧付,段組,改行幅全体,改行幅一行,文字空け,下詰め　だけです/n");
        return 9;
    }
    switch(com_index){
        case 0: bp_temp = bp_temp + style_index_num[0];//表題,
            bp_end = (unsigned char*)strchr((char*)bp_temp,'=');
            if(bp_end == NULL){
                bp_end = (unsigned char*)strchr((char*)bp_temp,'\n');
                cut_from_string(tex_line, bp_temp, bp_end);
                fprintf(o_file,"\\noindent{%s %s}\n\n",moji_size_list[my_title.moji_size],tex_line);
                break;
            }
            cut_from_string(temp_item, bp_temp, bp_end);
            fprintf(o_file,"\\noindent{%s %s}",moji_size_list[my_title.moji_size],temp_item);
            bp_temp = bp_end + 1;
            bp_end = (unsigned char*)strchr((char*)bp_temp,'\n');
            cut_from_string(tex_line, bp_temp, bp_end);
            fprintf(o_file,"\\hfill{%s %s}\n\n",moji_size_list[my_title.sub_moji_size],tex_line);
            break;
        case 1://大問
            bp_temp = bp_temp + style_index_num[1];
            bp_end = (unsigned char*)strchr((char*)bp_temp,'\n');
            cut_from_string(tex_line, bp_temp, bp_end);
            fprintf(o_file,"\\noindent\\hskip%d%s{%s %s}\n\n",my_daimon.position,moji_unit_list[unit_index],moji_size_list[my_daimon.moji_size],tex_line);
            break;
        case 2://小問
            bp_temp = (unsigned char*)strchr((char*)bp_temp,'=');
            if(bp_temp != NULL){
                com_index = find_list(bp_temp, enum_shomon_index, 3, 1);
                if(com_index > -1){
                    shomon_style = com_index;
                }
                else{
                    printf("小問形式の設定が違います。、mondaicom.iniで設定してください。\n");
                }
            }
            else{
                printf("小問形式の設定は、mondaicom.iniで設定してください。\n");
            }
            fprintf(o_file,"\\begin{%s}%s\n",env_name[2],enum_shomon_label[shomon_style]);
            intro_env(env_name[2]);
            break;
        case 3://選択縦並び
            bp_temp = (unsigned char*)strchr((char*)bp_temp,'=');
            if(bp_temp == NULL){
                printf("選択問題の形式は、mondaicom.iniで設定された規定値を用います。\n");
                com_index = choice_question_default;
            }
            else{
                com_index = find_list(bp_temp, enum_form_index, 5, 1);
            }
            choice_quest = init_enum(com_index, bp_temp);
            choice_quest_tate(o_file, choice_quest);
            break;
        case 4://選択横並び
            bp_temp = (unsigned char*)strchr((char*)bp_temp,'=');
            if(bp_temp == NULL){
                printf("選択問題の形式は、mondaicom.iniで設定された規定値を用います。\n");
                com_index = choice_question_default;
            }
            else{
                com_index = find_list(bp_temp, enum_form_index, 5, 1);
            }
            fprintf(o_file,"\\begin{%s}%s,itemsep=0pt]\n",env_name[2],enum_item_label[com_index]);
            intro_env(env_name[2]);
            break;
        case 5://選択括弧付
            bp_temp = (unsigned char*)strchr((char*)bp_temp,'=');
            if(bp_temp == NULL){
                printf("選択問題の形式は、mondaicom.iniで設定された規定値を用います。\n");
                com_index = choice_question_default;
            }
            else{
                com_index = find_list(bp_temp, enum_form_index, 5, 1);
            }
            bp_end = (unsigned char*)strchr((char*)bp_temp,'\n');
            cut_from_string(tex_line, bp_temp, bp_end);
            choice_quest = init_enum(com_index, tex_line);
            choice_quest_yoko(o_file, choice_quest);
            break;
        case 6://段組
            bp_temp = (unsigned char*)strchr((char*)bp_temp,'=');
            sscanf((char*)bp_temp,"=%d",&colunm);
            fprintf(o_file,"\\begin{%s}{%d}\n",env_name[4],colunm);
            intro_env(env_name[4]);
            break;
        case 7://改行幅全体
            bp_temp = (unsigned char*)strchr((char*)bp_temp,'=');
            sscanf((char*)bp_temp,"=%lf",&width);
            fprintf(o_file,"\\begin{%s}{%.1lf}\n",env_name[0],width);
            intro_env(env_name[0]);
            break;
        case 8://改行幅一行
            dim_point = strchr((char*)bp_temp,'=');
            sscanf(dim_point,"=%lf",&width);
            sprintf((char*)temp_line,"\\vskip%.1lf%s\n",width,moji_unit_list[unit_index]);
            strcat((char*)tex_line,(char*)temp_line);
            fputs((const char*)tex_line,o_file);
            break;
        case 9://文字空け
            dim_point = strchr((char*)bp_temp,'=');
            bp_temp = (unsigned char*)dim_point;
            bp_end = (unsigned char*)strstr((char*)bp_temp,"　");
            if(bp_end == NULL){
                bp_end = (unsigned char*)strchr((char*)bp_temp,' ');
            }
            if(bp_end == NULL){
                printf("この命令は数値の後に空白を入れてください。\n");
                fputs((const char*)tex_line,o_file);
                return 9;
            }
            sscanf(dim_point,"=%lf",&width);
            sprintf((char*)temp_line,"\\hskip%.1lf%s",width,moji_unit_list[unit_index]);
            strcat((char*)tex_line,(char*)temp_line);
            strcat((char*)tex_line,(char*)bp_end);
            fputs((const char*)tex_line,o_file);
            break;
        case 10://
            bp_temp = bp_temp + style_index_num[10];
            fprintf(o_file,"\n\\hfill  ");
            fprintf(o_file,"%s",bp_temp);
    }
    return 0;
}
/*
 tex_headerは、TeXのheader部分を読み込んで、出力ファイルに書き込む。引数は、出力ファイルへのポインタと
 ヘッダー部分を定義した書式設定ファイルの名。問題がなければ0を返す。
 */
int   tex_header(FILE* o_file, char* full_def_file_name)
{
    FILE* def_file1;
    static int  read_flag;
    char*   lp;
    char    c;
    read_flag = 0;
    if(o_file == NULL) return 1;
    def_file1 = fopen(full_def_file_name,"r");
    if(def_file1 == NULL){
        printf("書式設定ファイルが開けません。\n");
        return -1;
    }
    lp = line;
    init_buf((unsigned char*)lp, 128);
    set = 0;
    while((c = getc(def_file1))!= EOF)
    {
        if( c != '\n')
        {
            *lp = c;
            lp++;
            continue;
        }
        *lp = '\0';
        lp = strstr(line,"\\documentclass");
        if(read_flag == 0 && lp != NULL)
        {
            lp = strstr(lp, "lualatex");
            if(lp != NULL){
                unit_index = 1;
            }
            else
            {
                lp = strstr(line, "ltj");
                if(lp != NULL){
                    unit_index = 1;
                }
                else{
                    unit_index = 0;
                }
            }
            fprintf(o_file,"%s\n",line);
            init_buf((unsigned char*)line,128);
            read_flag = 1;
            lp = line;
            continue;
        }
        lp = strstr(line,"\\begin{");
        if(read_flag > 0 && lp != NULL){
            lp = lp + 7;
            strcpy(t_line,line);
            lp = strtok(lp,"}");
            strcpy(pri[set],lp);
            fprintf(o_file,"%s\n",t_line);
            set++;
            lp = line;
            init_buf((unsigned char*)line,128);
            read_flag = 2;
            continue;
        }
        fprintf(o_file,"%s\n",line);
        lp = line;
        init_buf((unsigned char*)lp, 128);
    }
    fclose(def_file1);
    return 0;
}
/*
 tex_endingは、tex_headerに対応して、tex文書の集結部を作る。引数は出力ファイルへのポインター
 問題がなければ0を返す。
 */
int     tex_ending( FILE* o_file)
{
    if(o_file == NULL) return 1;
    while(env_nest_num > 0)
    {
        env_nest_num = end_env(o_file);
    }
    set = set -1;
    while(set>-1)
    {
        fprintf(o_file,"\n\\end{%s}\n",pri[set]);
        set--;
    }
    return 0;
}

int    print_kan_index(FILE* o_file, int i )
{
    oyakan*     temp_kan;
    kaeriten*   temp_kaeri;
    comment*    temp_comment;
    int         kan_prop,kan_head,comment_flag,mark_ind,k;
    char        label_mark[3];
    char        kan_head_comm[20];
    char*        kan_prop_ind;
    comment_flag = 0;
    temp_kan = get_kan_index(i);
    if(temp_kan == NULL) return 1;
    kan_prop_ind = strchr((const char*)head_index, temp_kan->mod);
    if(kan_prop_ind == NULL) kan_prop_ind = head_index;
    kan_head = (int)(kan_prop_ind - head_index) * 2;
    if(temp_kan->label != NULL) {
        kan_head = kan_head + 1;
        comment_flag = 1;
    }
    strcpy(kan_head_comm,kan_head_list[kan_head]);
    if(temp_kan->b_end == 1) strcat(kan_head_comm,line_adjust_com);
    fprintf(o_file,"%s{%s}",kan_head_comm,temp_kan->oya);
    kan_prop = temp_kan->kunten_mode;
    if(kan_prop == 0 || (kan_prop == 1 && temp_kan->prop != 1)){//読み仮名
        fprintf(o_file,"{%s}",temp_kan->yomi);
    }
    else{
        fprintf(o_file,"{}");
    }
    if(kan_prop < 2 || kan_prop == 3){//送り仮名
        fprintf(o_file,"{%s}",temp_kan->okuri);
    }
    else{
        fprintf(o_file,"{}");
    }
    if(kan_prop == 0){
        fprintf(o_file,"{%s}",temp_kan->kaeri);//再読の読み
    }
    else{
        fprintf(o_file,"{}");
    }
    if(kan_prop < 2 || kan_prop == 3){
        fprintf(o_file,"{%s}",temp_kan->kaeri_okuri);
    }
    else{
        fprintf(o_file,"{}");
    }
    if(kan_prop < 3){
        fprintf(o_file,"{%s}",temp_kan->kaeri_ten);
    }
    else{
        fprintf(o_file,"{}");
    }
    if(comment_flag == 1){
        temp_comment = (comment*)temp_kan->label;
        strcpy(label_mark,temp_comment->mark);
        kan_prop_ind = strstr((const char*)label_index,label_mark);
        if(kan_prop_ind != NULL){
            mark_ind = (int)(kan_prop_ind - label_index)/2;
        }
        else{
            mark_ind = 0;
        }
        fprintf(o_file,"{%s{%s}}",label_list[mark_ind],temp_comment->comment);
    }
    temp_kaeri = get_kaeri_index(i);
    if(temp_kaeri != NULL && temp_kaeri->juku == 1)
        fprintf(o_file,"%s", zatu_con[0]);
    if(temp_kan->interval > 1){
        kan_prop = temp_kan->interval;
        mark_ind = p_power(kan_prop, 2);
        if(mark_ind > 0){
            fprintf(o_file,"%s0.%d%s",h_skip[0],mark_ind,moji_unit_list[unit_index]);
        }
        kan_prop = temp_kan->interval;
        mark_ind = p_power(kan_prop, 3);
        if(mark_ind > 0){
            fprintf(o_file,"\n\n");
        }
        for(k=1;k<mark_ind;k++){
            fprintf(o_file,"%s1%s",v_skip,moji_unit_list[unit_index]);
        }
        kan_prop = temp_kan->interval;
        mark_ind = p_power(kan_prop,5);
        for(k=0;k<mark_ind;k++){
            fprintf(o_file,"%s%.2lf%s",h_skip[1],st_moji_width,moji_unit_list[unit_index]);
        }
    }
    return 0;
}

int     print_kunten(FILE* o_file)
{
    int i,ret;
    if(o_file == NULL) return 9;
    i = 0;
    ret = 0;
    while( ret == 0){
        ret = print_kan_index(o_file, i);
        i++;
    }
    return 0;
}
