//
//  tex_command.h
//  kunten
//
//  Created by 岡崎康浩 on 2023/05/29.
//

#ifndef tex_command_h
#define tex_command_h

typedef struct con_seq{
    char        cont_seq[30];
    char                mod;
    char*               com;
} con_seq;


int     read_kan_com( char* con_file );
int     read_mon_com( char* con_file );
void    opening_kan( FILE* o_file );
void    closing_kan( FILE* o_file );
void    env_nest_init(void);
char*   set_label_form( char* mark);
char*   get_con_seq( int index );
unsigned char* get_ori_line( void );
int     tex_header(FILE* o_file, char* full_def_file_name);
int     tex_ending( FILE* o_file);
int     init_write_haku_moji_mode(int kun, int size, double width);
int     read_haku_moji_mode( int n );
double  read_haku_moji_mode_width( void );
int     print_kan_index(FILE* o_file, int i );
int     print_kunten(FILE* o_file);
int     get_env_nest(void);
int     read_text(unsigned char* t_char, int end_flag);
int     interpret_tex(FILE* o_file, unsigned char* line);



#endif /* tex_command_h */
