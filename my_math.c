//
//  my_math.c
//  kunten
//
//  Created by 岡崎康浩 on 2023/06/02.
//

#include "my_math.h"
#include "oyakan.h"

#define     KAERI_MAX   100
#define     ONLY_KAERI  30

int     temp_index_list[LISTMAX][LINEMAX];
int     index_list[LISTMAX][PROPMAX];
int     is_between_pair( pair p1, pair p2);





jun_struct      list_jun[MAX_MOJI_LIST];
pair            dubious_kaeri;
pair            dubious_kaeri_2nd;
pair            virtual_kaeri_pair[7];
jun_pair        each_kaeri_pairs[KAERI_MAX];
pair            kaeri_pairs[ONLY_KAERI];
pair            swap_pair;
int             list_length, max_kunjun;
int             jun_strata;
int             dubious_kaeri_num;
int             num_of_kaeri_in_sentence;

static int     prime_n[30]    = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113};
/*
 swap_pair_return(void)は、swap_pairを構造体ごと返す。
 */
pair*       swap_pair_return( void )
{
    return &swap_pair;
}
int     swap_pair_pre(void)
{
    return swap_pair.pre;
}
int     swap_pair_post(void)
{
    return swap_pair.post;
}
/*
 typedef struct jun_struct{
     int         ori_num;
     int         kun_jun_num;
     pair        kun_jun_pair;
     int         kaeri_num;
     long int    check;
 } jun_struct;
 の内容を表示する。
 */
void    print_list_jun( void )
{
    int     i;
    unsigned char* temp_kan;
    unsigned char  error_str[4] = {"???"};
    oyakan*        temp;
    i = 0;
    while(i < list_length){
        temp = get_kan_index(i);
        if(temp != NULL){
            temp_kan = temp->oya;
        }
        else{
            temp_kan = (unsigned char*)error_str;
        }
        printf("No. %02d:%s: jun = %02d, pair = (%02d : %2d), kaeri_num= %d jufuku = %09ld \n",list_jun[i].ori_num, temp_kan, list_jun[i].kun_jun_num, list_jun[i].kun_jun_pair.pre, list_jun[i].kun_jun_pair.post, list_jun[i].kaeri_num, list_jun[i].check);
        i++;
    }
}
void    print_dubious_kaeri( void )
{
    printf("dubious_kaeri = (pre: %d; post: %d)\n ",dubious_kaeri.pre, dubious_kaeri.post);
    printf("swap_pair = (pre: %d; post: %d)\n ",swap_pair.pre, swap_pair.post);
}
/*
 prime_num()は、何番目の素数を取り出すかというord、と最大で何番目かというmaxで、素数を返す。
 なお、maxを超える素数に対しては1を返す。
 */
int prime_num( int ord, int max, int flag )
{
    int     temp;
    if(ord > 30) return 1;
    temp = prime_n[ord-1];
    if(temp >= max)
    {
        if(flag == 1) printf("予定の素数の領域を超えています。\n");
        return 1;
    }
    return temp;
}
/*
int  find_kunjun(int jun, int max)は、junで示されたkun_junに相当するkan_index上の漢字のindexを返す。
 maxは、最大のkun_junを指定する。最大のものを超えた場合は−１を返す。int find_max_kunjun( void )を用いて、最大値を指定する。
 */
int     find_max_kunjun( int list_max )
{
    int              max,i;
    max = 1;
    i = 0;
    while(i < list_max)
    {
        if(max < list_jun[i].kun_jun_num) max = list_jun[i].kun_jun_num;
        i++;
    }
    return max;
}
int     find_kunjun( int jun, int max, int list_max)
{
    int         i;
    if(jun > max || jun < 1) return -1;
    i = 0;
    while(i < list_max)
    {
        if(list_jun[i].kun_jun_num == jun) return i;
        i++;
    }
    return -1;
}
/*
 ある数mをlist_junのpreとしてもっているものを見つける。
 */
int     find_pre( int m, int list_max)
{
    int         i;
    i = 0;
    while(i < list_max)
    {
        if(list_jun[i].kun_jun_pair.pre == m) return i;
        i++;
    }
    return -1;
}
/*
 list_junを初期化する。戻り値はlist_junの長さ。対応するkan_indexに対して、
 */
int    init_list_jun( void )
{
    int     index_kan_list, index_kun_jun, temp_index,temp_kun_jun;
    int     kaeri_num;
    oyakan* temp_kan;
    index_kan_list = 0;
    temp_kan = get_kan_index(index_kan_list);
    while(temp_kan != NULL)
    {
        list_jun[index_kan_list].ori_num = index_kan_list;
        list_jun[index_kan_list].kun_jun_num = temp_kan->kun_jun;
        list_jun[index_kan_list].kun_jun_pair.pre = -1;
        list_jun[index_kan_list].kun_jun_pair.post = -1;
        list_jun[index_kan_list].kaeri_num = 0;
        if(temp_kan->jufuku > 0){
            list_jun[index_kan_list].check = simple_factorization(temp_kan->jufuku);
        }
        else{
            list_jun[index_kan_list].check = 0;
        }
        index_kan_list++;
        temp_kan = get_kan_index(index_kan_list);
    }
    list_jun[index_kan_list].ori_num = 0;
    list_jun[index_kan_list].kun_jun_num = MAX_MOJI_LIST;
    list_length = index_kan_list;
    index_kun_jun = 1;
    index_kan_list = 0;
    kaeri_num = 0;
    temp_index = 0;//ori: temp_index = -1;
    max_kunjun = find_max_kunjun(list_length);
    while(temp_index < list_length)
    {
        temp_kun_jun = list_jun[temp_index].kun_jun_num;
        if(temp_kun_jun > 0){
            list_jun[temp_index].kun_jun_pair.pre = find_kunjun(temp_kun_jun - 1, max_kunjun,list_length);
            list_jun[temp_index].kun_jun_pair.post = temp_index;//find_kunjun(temp_kun_jun + 1, max_kunjun,list_length);
            if(temp_index < list_jun[temp_index].kun_jun_pair.pre){
                kaeri_num++;
                list_jun[temp_index].kaeri_num = kaeri_num;
            }
        }
        else{
            list_jun[temp_index].kun_jun_num = 0;
        }
        temp_index++;
    }
    return list_length;
}
/*
void init_vertual_kaeri(void)
 */
void    init_virtual_kaeri( void )
{
    int     i;
    for(i=1;i<7;i++){
        virtual_kaeri_pair[i].pre = -1;
        virtual_kaeri_pair[i].post = -1;
    }
}
/*
 int    jun_sequence( int pre_i, int post_i )はpre_i post_iの間が順序としても繋がっているかを確認する。
 */
int    jun_sequence( int pre_i, int post_i )
{
    int     temp_jun, temp_i,ret;
    if(pre_i == post_i - 1){
        temp_jun = list_jun[pre_i].kun_jun_num;
        if(temp_jun + 1 == list_jun[post_i].kun_jun_num){
            return 0;
        }
        else{
            return 1;
        }
    }
    ret = 1;
    temp_i = pre_i;
    temp_jun = list_jun[pre_i].kun_jun_num;
    while(temp_i < post_i)
    {
        temp_jun++;
        temp_i = find_kunjun(temp_jun, max_kunjun, list_length);
        if(temp_i == post_i) {
            ret = 0;
            break;
        }
        if(temp_i < pre_i || post_i < temp_i) return 1;
    }
    return ret;
}
/*
int     judge_kunjun(pair temp_kaeri,int max)は、temp_kaeriの配列に保存されている返読のペアを順に読み込んで、問題があるかを
 判断する。maxには、保存されているペアの数。
 */
int     judge_kunjun(pair temp_kaeri)
{
    int     temp_post, ret;

    init_virtual_kaeri();
    if(jun_strata == 0){
        virtual_kaeri_pair[jun_strata].pre = temp_kaeri.pre;
        virtual_kaeri_pair[jun_strata].post = temp_kaeri.post;
        jun_strata = 1;
    }
    else{
        ret = is_between_pair(virtual_kaeri_pair[jun_strata-1], temp_kaeri);
        switch( ret ){
            case 0:
                virtual_kaeri_pair[jun_strata].pre = temp_kaeri.pre;
                virtual_kaeri_pair[jun_strata].post = temp_kaeri.post;
                jun_strata++;
                break;
            case 3:
                jun_strata--;
                virtual_kaeri_pair[jun_strata].pre = temp_kaeri.pre;
                virtual_kaeri_pair[jun_strata].post = temp_kaeri.post;
                break;
            case 5:
                temp_post = virtual_kaeri_pair[jun_strata-1].pre;
                ret = jun_sequence(temp_kaeri.post, temp_post);
                if(ret != 0){
                    dubious_kaeri.pre = temp_kaeri.pre;
                    dubious_kaeri.post = temp_kaeri.post;
                    return 1;
                }
                else{
                    virtual_kaeri_pair[jun_strata - 1].post = temp_kaeri.post;
                    break;
                }
            default:
                dubious_kaeri.pre = temp_kaeri.pre;
                dubious_kaeri.post = temp_kaeri.post;
                return 1;

        }
    }
    return 0;
}
/*
 別のやり方による訓順のチェック ただし、この場合は、jun
 */
int     judge_kunjun_2nd( int max)
{
    int     mark, mark2, index,temp_pre_1, temp_pre_2, temp_post_1, temp_post_2, temp_index, ret;
    index = 1;
    mark = 0;
    ret  = 0;
    
    temp_pre_1 = -1;
    temp_pre_2 = -1;
    temp_post_1 = -1;
    temp_post_2 = -1;
    temp_index = -1;
    while( index < max)
    {
        if(each_kaeri_pairs[index].jun == 0){
            index++;
            continue;
        }
        if(mark != 0){
            if(temp_index < index){
                temp_pre_2 = find_pre(index, max);
                temp_pre_1 = find_pre(temp_index, max);
                mark2 = each_kaeri_pairs[index].jun;
                if(each_kaeri_pairs[temp_pre_1].jun < 0 && each_kaeri_pairs[temp_pre_2].jun < 0 && each_kaeri_pairs[temp_pre_1].jun < 0 && index < temp_pre_1 && temp_pre_1 < temp_pre_2 ){
                    ret = 2;
                    dubious_kaeri.pre = each_kaeri_pairs[temp_index].pre;
                    dubious_kaeri.post = each_kaeri_pairs[temp_index].post;
                    break;
                }
                else{
                    mark = 0;
                    temp_index = -1;
                }
            }
            else{
                mark = 0;
                temp_index = -1;
            }
        }
        if(each_kaeri_pairs[index].jun != 0){
            mark = each_kaeri_pairs[index].jun;
            if(mark > 0){
                temp_index = each_kaeri_pairs[index].post;
            }
            else{
                temp_index = each_kaeri_pairs[index].pre;
            }
        }
        index++;
    }
    return ret;
}
/*
 mid_identifyは、問題となるpointがpreとpostの間にあるかどうかを判定する。postとpreはどちらが大きくても良い。
 間にあれば０、前にはみ出していれば１、後にはみ出していれば２を返す。
 */
int     mid_identify( int   point, int  pre, int post)
{
    int     swap;
    if(pre > post){
        swap = pre;
        pre = post;
        post = swap;
    }
    if(point < pre) return 1;
    if(post < point) return 2;
    return 0;
}
/*
 pairの数値に重なりがあるかどうかを判定する。
 */
int     is_between_pair( pair p1, pair p2)
{
    int pre1, pre2, post1, post2;
    int     swap, ret;
    pre1 = p1.pre;
    post1 = p1.post;
    pre2 = p2.pre;
    post2 = p2.post;
    if(pre1 > post1){
        swap = pre1;
        pre1 = post1;
        post1 = swap;
    }
    if(pre2 > post2){
        swap = pre2;
        pre2 = post2;
        post2 = swap;
    }
    if(post1 < pre2 || post1 == pre2) return 3;
    if(pre1 < pre2){        // not pre1 > pre2:  --pre1--pre2.......post2
        ret = mid_identify(post1, pre2, post2);
        switch(ret){
            case 1:     return 3;
                break;
            case 2:     return 0;
                break;
            case 0:     return 5;//問題
        }
    }
    if(pre2 < pre1){     // not post1 > post2 ...pre2....post1--post2
        ret = mid_identify(post2, pre1, post1);
        switch(ret){
            case 0: return 6; //問題
                break;
            case 2: return 1; //
                break;
            case 1: return 4;
                break;
        }
    }
    return 4;
}
/*
over_lapping_area( pair p1, pair p2)は、p1とp2がオーバーラップしている場合、
 */
int     over_lapping_area( pair p1, pair p2)
{
    int     ret;
    ret = is_between_pair(p1, p2);
    switch(ret){
        case 5: return p2.pre;
            break;
        case 6: return p2.post;
            break;
        default:
            return -1;
    }
}
/*
 check_kunjun()は、訓順が可能な形かどうかを判定する。問題がなければ、０、問題があれば１を返す。
 */
int     check_kunjun(void)
{
    int     index, max,kaeri_num,i,ret,ret2;
    index = 0;
    max = init_list_jun();
    kaeri_num = 0;
    dubious_kaeri.pre = -1;
    dubious_kaeri.post = -1;
    dubious_kaeri_num = -1;
    for(i = 0; i < KAERI_MAX; i++){
        each_kaeri_pairs[i].pre = -1;
        each_kaeri_pairs[i].post = -1;
        each_kaeri_pairs[i].jun = 0;
    }
    for(i = 0; i < ONLY_KAERI; i++)
    {
        kaeri_pairs[i].pre = -1;
        kaeri_pairs[i].post = -1;
    }
    while(index < max ){
        if(list_jun[index].kun_jun_num > 1){
            each_kaeri_pairs[index].pre = list_jun[index].kun_jun_pair.pre;
            each_kaeri_pairs[index].post = index;
            each_kaeri_pairs[index].jun = list_jun[index].kun_jun_pair.pre - index;
            if(list_jun[index].kaeri_num > 0){
                kaeri_pairs[kaeri_num].post = index;
                kaeri_pairs[kaeri_num].pre = each_kaeri_pairs[index].pre;
                kaeri_num++;
            }
        }
        index++;
    }
    num_of_kaeri_in_sentence = kaeri_num;
    if(read_verbose() > 1){
        print_list_jun();
        }
    i = 0;
    ret = 0;
    jun_strata = 0;
    while(i < kaeri_num)
    {
        ret = judge_kunjun(kaeri_pairs[i]);
        if(ret != 0) {
            dubious_kaeri_num = i;
            break;
        }
        i++;
    }
   ret2 = judge_kunjun_2nd(max);
    if(read_verbose() > 0){
        printf("judge_kunjun = %d; judge_kunjun_2nd = %d\n", ret, ret2);
    }
    return ret + ret2;
}
/*
 
 */


/*
 数値mを構成する素数を見つける。なお、least_limitとgreatest_limitは素数の下限と上限、ただし、
 下限は、何番目の素数かということで、２なら１、３なら２、５なら３、７なら４ということになる。上限までに
 当該の素数がなければ-1を返す。
 */
int prime_simple_factorization(long int m, int least_limit, int greatest_limit)
{
    int     p, i;
    i = least_limit;
    p = 1;
    while( 1 ){
        p = prime_num( i, greatest_limit,0);
        if(p == 1) break;
        if(m % p == 0 ) return i;
        i++;
    }
    return -1;
}
int   simple_factorization(long int m)
{
    int     p, i;
    i = 0;
    p = 2;
    while(p != 1)
    {
        p = prime_num(i,110,0);
        if(m % p == 0) return p;
        i++;
    }
    return 1;
}
/*
temp_index_list[n][m]に数値numを入れる。
 */
void    write_temp_list(int n, int m, int num)
{
    temp_index_list[n][m] = num;
}
/*factorial(int p)pの階乗をもとめる。戻り値は階乗 */
int     factorial( int p )
{
    int ind, f_num;
    if(p < 1){
        ind = 1;
    }
    else
    {
        ind = p;
    }
    f_num = ind;
    while(1){
        ind = ind - 1;
        if(ind == 0)break;
        f_num = f_num * ind;
    }
    return f_num;
}
/*init_temp_index,temp_index_list[LISTMAX=20][LINEMAX=3]の初期化、すべての要素に-1を代入する。 */
void    init_temp_index_list( void )
{
    int i,j;
    for(i=0;i<LISTMAX;i++)
    {
        for(j=0;j<LINEMAX;j++){
            temp_index_list[i][j] = -1;
        }
    }
}
/*init_index,index_list[LISTMAX=20][LINEMAX=3]の初期化、すべての要素に-1を代入する。 */
void    init_index_list( void )
{
    int i,j;
    for(i=0;i<LISTMAX;i++)
    {
        for(j=0;j<PROPMAX;j++){
            index_list[i][j] = -1;
        }
    }
}
/*temp_index_list[LISTMAX=20][LINEMAX=3]から、temp_index_list[n][m]の数値を取り出す。*/

int     read_temp_list(int n, int m)
{
    return temp_index_list[n][m];
}
/* index_list[LISTMAX=20][LINEMAX=3]から、index_list[n][m]の数値を取り出す。*/

int     read_list(int n, int m)
{
    return index_list[n][m];
}

/*
三つの数のうちから、最大値を取り出す。
 */
int     max_num( int n1, int n2, int n3)
{
    int temp_max;
    temp_max = n1;
    if(temp_max < n2) temp_max = n2;
    if(temp_max < n3) temp_max = n3;
    return temp_max;
}
/*
三つの数のうちから、最小値を取り出す。
 */
int     min_num( int n1, int n2, int n3)
{
    int temp_min;
    temp_min = n1;
    if(temp_min > n2) temp_min = n2;
    if(temp_min > n3) temp_min = n3;
    return temp_min;
}
/*
 temp_index_list[i][m]のm行から最大値を取り出す。
 */
int     max_temp_index_list( int line )
{
    int temp_max,i;
    i = 1;
    temp_max = temp_index_list[0][line];
    while(temp_index_list[i][line] > -1){
        if(temp_index_list[i][line] > temp_max) temp_max = temp_index_list[i][line];
        i++;
    }
    return temp_max;
}
/*
 temp_index_list[i][m]のm行から最小値を取り出す。
 */
int     min_temp_index_list( int line )
{
    int temp_min,i;
    i = 1;
    temp_min = temp_index_list[0][line];
    while(temp_index_list[i][line] > -1){
        if(temp_index_list[i][line] > temp_min) temp_min = temp_index_list[i][line];
        i++;
    }
    return temp_min;
}
/*
oyakan構造体からpropの値を取り出す。引数はindex番号
 */
long int  kan_index_property( int index)
{
    oyakan*     temp_kan;
    temp_kan = get_kan_index( index );
    return temp_kan->prop;
}
/*
oyakan構造体からkunjunの値を取り出す。引数はindex番号
 */
int  kan_index_kunjun( int index)
{
    oyakan*     temp_kan;
    temp_kan = get_kan_index( index );
    return temp_kan->kun_jun;
}
/*
 temp_index_indexのリストの中にある数字numが含まれているかどうかを判定する。あればその訓順を返す。なければ−1、また、訓順が
 書いてなければ、1を返す。
 */
int     exist_in_temp_index_list( int num )
{
    int     i;
    i = 0;
    while(temp_index_list[i][0] > -1){
        if(temp_index_list[i][0] == num){
            if(temp_index_list[i][1] < 1) return 1;
            return temp_index_list[i][1];
        }
        i++;
    }
    return -1;
}
/*
temp_index_listのリストにある数を参考にしながら、その最小値と最大値の間にあるkan_indexのpropを取り出して、各リストに順に
 ならべる。なお、約物に相当するものは飛ばす。
 */
void    listing_index_list( int min, int max )
{
    int     temp_index, i, ret;
    temp_index = min;
    max = max + 1;
    i = 0;
    init_index_list();
    while (temp_index < max){
        if(kan_index_kunjun(temp_index) == -1){
            temp_index++;
            continue;
        }
        index_list[i][0] = temp_index;
        ret = (int)kan_index_property(temp_index) & 15;
        index_list[i][2] = ret;
        ret = exist_in_temp_index_list(temp_index);
        index_list[i][1] = ret;
        temp_index++;
        i++;
    }
}
/*
temp_index_listが与えられたとき、index_listの中に読めた漢字、読めなくてもよい漢字（かなにすべき漢字、置き字）を
 除いたものがどれぐらいあるかを返す。
 */
int     unread_char_list( int max, int min )
{
    int     ret, num,i;
    ret = min;
    if(min == -1) ret = min_temp_index_list( 0 );
    min = ret;
    ret = max;
    if(max > 110) ret = max_temp_index_list( 0 );
    listing_index_list(min,ret);
    num = 0;
    i = 0;
    while(index_list[i][0] > -1)
    {
        if(index_list[i][1] < 1 && index_list[i][2] != 0) num++;
        i++;
    }
    return num;
}
/*
 kan_index[x]と kan_index[y]のjufukuの数を交換します。成功すれば0、うまくいかなければ-9を返します。
 */
int    swap_jufuku_kan_index( int x, int y)
{
    oyakan* kan1;
    oyakan* kan2;
    if( x == y) return 0;
    long int     temp;
    kan1 = get_kan_index( x );
    kan2 = get_kan_index( y );
    if(kan1 == NULL || kan2 == NULL) return -9;
    temp = kan2->jufuku;
    kan2->jufuku = kan1->jufuku;
    kan1->jufuku = temp;
    return 0;
}
/*
 これは、二つの数num1とnum2が有意かどうかを判定する、判定基準criteriaで、num1、num2のnum1のみ有意な場合は1、num2のみ
 有意な場合は2、共に有意な場合は3、その中でも有意なnum1とnum2が0、どちらも有意でない場合は、-1を返す。
 */
int     evaluate_two_number( int num1, int num2)
{
    int ret;
    ret = 0;
    if(num1) ret = 1;
    if(num2) ret = 2;
    return ret;
}
/*これは、num1とnum2を比較し、同じであれば0を返し、異なっていれば、num2を返す。
 */
int     equal_or_not(int num1, int num2)
{
    if(num1 == num2) return 0;
    return num2;
}
/*
 int    judge_juku_ret(int pre, int post)
 は、juku_after_identify,juku_before_identifyから出た戻り値をまとめて、一つの数値にする。
 pre = -2 V post = -2   -> 5
 pre = -1 ^ post = -1   -> 1
 pre = -1 ^ (post = 1 V post = 0) -> 2
 (pre = 0 V pre = 1) ^ post = -1 -> 3
 pre = 1 ^ post = 1 -> 4
 pre = 0 V post = 0 -> 0
 */
int    judge_juku_ret(int pre, int post)
{
    if(pre == -2 || post == -2)       return 5;
    if(pre == -1 && post == -1)     return 1;
    if(pre == -1)                   return 2;
    if(post == -1)                  return 3;
    if(pre == 1 && post == 1)       return 4;
    return 0;
}
/*
 あるインデックスにを挟むkan_indexの訓順とそのインデックスの関係を探る。
 後方はmodeに１、前方は-1を入れる。訓順がないまたは、前方、後方がない場合は０を返す。
 */
int     immediate_kun(int index, int mode)
{
    oyakan* temp;
    temp = get_kan_index(index+mode);
    while(temp!=NULL){
        if(temp->kun_jun < 0){
            mode = mode + mode;
            temp = get_kan_index(index+mode);
            continue;
        }
        return temp->kun_jun;
    }
    return 0;
}
/*
 これは、前後で最も近い、すでに読まれた漢字を探す。後方は1、前方は−1
 見つからない場合は-1を返す。
 */
int     immediate_index(int index, int mode)
{
    oyakan* temp;
    temp = get_kan_index(index+mode);
    while(temp!=NULL){
        if(temp->kun_jun < 1){
            mode = mode + mode;
            temp = get_kan_index(index+mode);
            continue;
        }
        return index+mode;
    }
    return -1;
}
/*
 これは、index周囲の訓順を調べるもので、indexより前にあるものtemp_kunを超えない最後のもの、
 後の訓順でtemp_kunの後に読まれる最初のものの、それぞれkan_index上の番号を返す。うまく見つからない場合-1
 */
int     kunjun_around_index(int index, int temp_kun_jun, int mode)
{
    oyakan* temp;
    int     tmp_i, temp_index,temp_jun;
    tmp_i = index + mode;
    if(tmp_i < 0) return -1;
    temp_index = index;
    temp_jun = immediate_kun(index, mode);
    temp = get_kan_index(tmp_i);
    while(temp != NULL)
    {
        if(temp->kun_jun == temp_jun && temp_jun * mode > temp_kun_jun * mode){
            temp_jun = temp->kun_jun - mode;
            temp_index = tmp_i;
            tmp_i = index + mode;
        }
        else
        {
            tmp_i = tmp_i + mode;
        }
        temp = get_kan_index(tmp_i);
    }
    return temp_index;
}

int     p_power(int num, int p)
{
    int     ret;
    int     count = 0;
    while(num != 1)
    {
        ret = num % p;
        if(ret != 0) break;
        num = num/p;
        count++;
    }
    return count;
}
/*
 vertual_kan_indexを用いて、訓読順序で問題となるような箇所を探す。見つかれば、その位置のvertual_kan_indexのcheckを３にする。戻り値は、そうした問題となる箇所の数。
 */
int     erronious_point_in_kunjun( void )
{
    int     pre_i, post_i,temp_i, i, j, ret, temp_ret;
    jufuku_index     jufuku[6];
    int     jufuku_num, temp_kaeri_num, kaeri_num;
    kaeri_num = dubious_kaeri_num;
    pre_i = dubious_kaeri.pre;
    post_i = dubious_kaeri.post;
    swap_pair.pre = -1;
    swap_pair.post = -1;
    if(post_i > pre_i){
        temp_i = post_i;
        post_i = pre_i;
        pre_i = temp_i;
    }
    for(i = 0; i < 6; i++){
        jufuku[i].kind = 0;
        jufuku[i].index = -1;
        jufuku[i].check = 0;
    }
    ret = 1;
    jufuku_num = 0;
    i = pre_i;
    while (i > post_i - 1)
    {
        if(list_jun[i].check > 0){
            if(jufuku_num == 0){
                jufuku[jufuku_num].kind = (int)list_jun[i].check;
                jufuku[jufuku_num].index = i;
                jufuku_num++;
            }
            else{
                jufuku[jufuku_num].kind = (int)list_jun[i].check;
                jufuku[jufuku_num].index = i;
                for(j = 0; j < jufuku_num; j++){
                    if(jufuku[j].kind == list_jun[i].check){
                        jufuku[j].check = jufuku[j].kind;
                        jufuku[jufuku_num].check = jufuku[j].kind;
                        swap_pair.pre = jufuku[j].index;
                        swap_pair.post = i;
                        ret = 0;
                    }
                }
            }
        }
        i--;
    }
    temp_kaeri_num = kaeri_num - 1;
    temp_ret = -1;
    while(temp_kaeri_num > -1 && temp_ret < 5 ){
        temp_ret = over_lapping_area(dubious_kaeri, kaeri_pairs[temp_kaeri_num]);
        if(temp_ret > -1) break;
        temp_kaeri_num--;
    }
    if(temp_ret < 0) return 1;
    post_i = temp_ret;
    for(i = 0; i < 6; i++){
        jufuku[i].kind = 0;
        jufuku[i].index = -1;
        jufuku[i].check = 0;
    }
    jufuku_num = 0;
    i = pre_i;
    while (i > post_i - 1)
    {
        if(list_jun[i].check > 0){
            if(jufuku_num == 0){
                jufuku[jufuku_num].kind = (int)list_jun[i].check;
                jufuku[jufuku_num].index = i;
                jufuku_num++;
            }
            else{
                jufuku[jufuku_num].kind = (int)list_jun[i].check;
                jufuku[jufuku_num].index = i;
                for(j = 0; j < jufuku_num; j++){
                    if(jufuku[j].kind == list_jun[i].check){
                        jufuku[j].check = jufuku[j].kind;
                        jufuku[jufuku_num].check = jufuku[j].kind;
                        swap_pair.pre = jufuku[j].index;
                        swap_pair.post = i;
                        ret = 0;
                    }
                }
            }
        }
        i--;
    }
    temp_kaeri_num = kaeri_num + 1;
    temp_ret = -1;
    while(temp_kaeri_num > -1 && temp_ret < 5 ){
        temp_ret = over_lapping_area(dubious_kaeri, kaeri_pairs[temp_kaeri_num]);
        if(temp_ret > -1) break;
        temp_kaeri_num++;
    }
    if(temp_ret < 0) return 1;
    pre_i = temp_ret;
    for(i = 0; i < 6; i++){
        jufuku[i].kind = 0;
        jufuku[i].index = -1;
        jufuku[i].check = 0;
    }
    jufuku_num = 0;
    i = pre_i;
    while (i > post_i - 1)
    {
        if(list_jun[i].check > 0){
            if(jufuku_num == 0){
                jufuku[jufuku_num].kind = (int)list_jun[i].check;
                jufuku[jufuku_num].index = i;
                jufuku_num++;
            }
            else{
                jufuku[jufuku_num].kind = (int)list_jun[i].check;
                jufuku[jufuku_num].index = i;
                for(j = 0; j < jufuku_num; j++){
                    if(jufuku[j].kind == list_jun[i].check){
                        jufuku[j].check = jufuku[j].kind;
                        jufuku[jufuku_num].check = jufuku[j].kind;
                        swap_pair.pre = jufuku[j].index;
                        swap_pair.post = i;
                        ret =  0;
                    }
                }
            }
        }
        i--;
    }
    return ret;
}
/*
 PIDは、kan_indexにあるpropの値（この関数ではnum）がcriterionの値を含むかどうかを判定する。含む場合は０、含まない場合は１を返す。
 */
int PID( long int num, int criterion)
{
    long int    temp;
    temp = -num;
    if(temp & criterion){
        return 0;
    }
    return 1;
}
/*
 penalty_countは、訓読文中のpenaltyがどうかを判定する。マイナスの値が返れば、問題が多い、プラスの値が返れば問題が少ないと考えられる。
 */
int     penalty_count( void )
{
    int     count, index;
    oyakan*     temp_kan;
    count = 0;
    index = 0;
    temp_kan = get_kan_index(index);
    while(temp_kan != NULL)
    {
        count = count + temp_kan->gtest;
        index++;
        temp_kan = get_kan_index(index);
    }
    return count;
}
/*
 vertual_kan_indexを用いて、問題がありそうな箇所にある同一の漢字の順序を入れ替える。戻り値は、入れ替えた場所のindexを返す。引数のomitは、その位置にある漢字の訓順の交換を除外する。−１であれば、除外がない。
 */
/*
 erronious_point_in_kunjunとerronious_point_swappingを組み合わせて、矛盾のない語順を作り出す。引数は、元のエラーの数、返り値はエラーの数。

int     correct_kunjun( void )
{
    int     temp_error, const_error, omit,count;
    omit = 1;
    count = 0;
    temp_error = erronious_point_in_kunjun(omit+1);
    const_error = factorial(temp_error);
    while(temp_error != 0 && count < const_error)
    {
        omit = erronious_point_swapping(omit);
        temp_error = erronious_point_in_kunjun(omit+1);
        if(temp_error == 0) return 0;
        count++;
    }
    return 1;
} */

/*
 vertual_to_realはvertual_kan_indexにある訓読順をkan_indexに移す。
 */

