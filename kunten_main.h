//
//  kunten_main.h
//  kunten
//
//  Created by 岡崎康浩 on 2023/06/07.
//

#ifndef kunten_main_h
#define kunten_main_h

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
//#include "my_math.h"

FILE*           haku_file;
FILE*           kun_file;
FILE*           out_file;
FILE*           def_file1;
FILE*           tex_file;

char            cdir_name[MAX_DIR];
char            exe_root[MAX_DIR];

itai*       root_itai;
kanlist*    hendoku_moji;
kanlist*    fukusi;
kanlist*    root_okuri;
kanlist*    daisi;
kanlist*    kaisi;
kanlist*    okikana_root;
kanlist*    no_hendoku;
kanlist*    hendoku_moji2;
kanlist*    hendoku_moji3;
kanlist*    hendoku_moji4;
kanlist*    hendoku_moji5;
kan_chain_list*    siyeki;
kan_chain_list*    hitei_hukusi;
kanakan*    kanakan_moji;
saidoku*    saidoku_moji;

int            get_kan_haku_flag( void );
void           write_kan_haku_flag( int num);
void           write_read_num( int num );
void           write_file_end_flag( int num );
int            get_file_end_flag ( void );
int            read_non_sai_okiji(void);
unsigned char* read_buf_kan( FILE* i_file, FILE* o_file);
unsigned char* read_buf_kun( FILE* i_file);
void           init_kakko_u( void );
itai*          readdata_itai( void );
kanlist*       readdata_long_okuri(const char* f_name, const char* sp_name, const char* marker);
kanlist*       readdata_short_okuri(const char* f_name, const char* sp_name, const char* marker);
kan_chain_list* read_kan_list(const char* f_name, const char* sp_name);
oyakan*        set_kanji(unsigned char* stream);
saidoku*       readdata_sai( void );
kanakan*       readdata_kanakan( void );
int            g_test;
int            verbose;


#endif /* kunten_main_h */
