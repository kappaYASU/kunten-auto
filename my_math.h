//
//  my_math.h
//  kunten
//
//  Created by 岡崎康浩 on 2023/06/02.
//

#ifndef my_math_h
#define my_math_h

#include <stdio.h>

#define     LISTMAX    30
#define     LINEMAX    3
#define     PROPMAX    4
typedef struct pair{
    int     pre;
    int     post;
} pair;
typedef struct jun_pair{
    int     pre;
    int     post;
    int     jun; //正順は-1, 逆順は1
} jun_pair;
typedef struct jun_struct{
    int         ori_num;
    int         kun_jun_num;
    pair        kun_jun_pair;
    int         kaeri_num;
    long int    check;
} jun_struct;
typedef struct vertual{
    int         index_num;
    int         kun_jun;
    int         juku_on;
    long int    jufuku;
    int         check;
} vertual;
typedef struct jufuku_index{
    int     index;
    int     kind;
    int     check;
} jufuku_index;


int         prime_num( int ord, int max, int flag);
int         prime_simple_factorization(long int m, int least_limit, int greatest_limit);
int         simple_factorization(long int m);
void        write_temp_list(int n, int m, int num);
void        init_temp_index_list( void );
void        init_index_list( void );
int         read_temp_list(int n, int m);
int         read_list(int n, int m);
int         max_num( int n1, int n2, int n3);
int         min_num( int n1, int n2, int n3);
int         max_temp_index_list( int line );
long int    kan_index_property( int index);
int         exist_in_temp_kan_list( int num );
int         unread_char_list( int max, int min );
int         swap_jufuku_kan_index( int x, int y);
int         evaluate_two_number( int num1, int num2);
int         equal_or_not(int num1, int num2);
int         kunjun_around_index(int index, int temp_kun_jun, int mode);
int         immediate_kun(int index, int mode);
int         immediate_index(int index, int mode);
int         p_power(int num, int p);
int         factorial( int p);
int         init_list_jun( void );
int         check_kunjun(void);
void        init_vertual_kan_index(void);
void        write_vertual(int index, int jufuku_n, int kun_jun_n, int check_n, int juku);
//int         read_vertual_check(int  index);
//int         read_vertual_juku(int  index);
int         erronious_point_in_kunjun( void );
int         erronious_point_swapping( int omit );
int         read_vertual_kunjun( int index );
int         correct_kunjun( void );
int         judge_juku_ret(int pre, int post);
void        print_dubious_kaeri( void );
pair*       swap_pair_return( void );
int         swap_pair_pre(void);
int         swap_pair_post(void);
int         PID( long int num, int criterion);
int         penalty_count( void );
//long long int     quotient_remainder(long long int num, int modular);


#endif /* my_math_h */
