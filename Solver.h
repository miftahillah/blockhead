#pragma once

#include <setjmp.h>

#define  USE_METRIC     QUARTER_TURN_METRIC


#define MAX_SOLUTION 15

typedef struct cube
{
    int      edges[24];
    int      corners[24];
    int      a_corner;
    int      a_edgeflip;
    int      a_edgeloc;
    int      cornerperm;
    int      ud_sliceedge;
    int      rl_sliceedge;
    int      fb_sliceedge;
    int      parity;

    int      edges_wo_flip[12];   /*  going, ...  */

    char   **strings;
    int      dir;
} Cube;


typedef struct stage1_node
{
    int    remain_depth;
    int    twist;
    int    tw_list;
    int    corner;
    int    eflip;
    int    eloc;
} Stage1_node;


typedef struct stage2_node
{
    int    remain_depth;
    int    twist;
    int    tw_list;
    int    cperm;
    int    eperm;
    int    mperm;
} Stage2_node;



typedef struct options
{
    int    metric;
    int    limit_depth_stage2;
} Options;

struct Solution
{
    int steps;
    bool solved;
    int *solution;
};

/*  different metrics  */

#define  UNINITIALIZED_METRIC      0
#define  QUARTER_TURN_METRIC       1
#define  FACE_TURN_METRIC          2




#define  A_N_CORNER             2187
#define  A_N_EFLIP              2048
#define  A_N_ELOC                495
#define  A_N_ELOC_CONV          4096

#define  B_N_CORNERPERM        40320
#define  B_N_EDGEPERM          40320
#define  B_N_MIDPERM              24

#define  A_CORNER_START            0
#define  A_EFLIP_START             0
#define  A_ELOC_START            494

#define  B_CORNERPERM_START        0
#define  B_EDGEPERM_START          0
#define  B_MIDPERM_START           0




#define  N_SLICEEDGE           11880
#define  N_HALFEDGEPERM         1680
#define  N_HALFEDGELOC            70
#define  N_HALFEDGELOC_CONV      256

#define  EDGE_INVALID          54321




#define  MAX_PERM_N               12
#define  MAX_CHECK_PERM_N         24




#define  BIG                     181




/*  number the corner cubies  */

#define  CORNER_UFR                0
#define  CORNER_URB                1
#define  CORNER_UBL                2
#define  CORNER_ULF                3
#define  CORNER_DRF                4
#define  CORNER_DFL                5
#define  CORNER_DLB                6
#define  CORNER_DBR                7

#define  A_CORNER_UFR     CORNER_UFR
#define  A_CORNER_URB     CORNER_URB
#define  A_CORNER_UBL     CORNER_UBL
#define  A_CORNER_ULF     CORNER_ULF
#define  A_CORNER_DRF     CORNER_DRF
#define  A_CORNER_DFL     CORNER_DFL
#define  A_CORNER_DLB     CORNER_DLB
#define  A_CORNER_DBR     CORNER_DBR

#define  B_CORNER_UFR     CORNER_UFR
#define  B_CORNER_URB     CORNER_URB
#define  B_CORNER_UBL     CORNER_UBL
#define  B_CORNER_ULF     CORNER_ULF
#define  B_CORNER_DRF     CORNER_DRF
#define  B_CORNER_DFL     CORNER_DFL
#define  B_CORNER_DLB     CORNER_DLB
#define  B_CORNER_DBR     CORNER_DBR


/*  number the edge cubies  */

#define  A_EDGE_UF                 0
#define  A_EDGE_UR                 1
#define  A_EDGE_UB                 2
#define  A_EDGE_UL                 3
#define  A_EDGE_DF                 4
#define  A_EDGE_DR                 5
#define  A_EDGE_DB                 6
#define  A_EDGE_DL                 7
#define  A_EDGE_FR                 8
#define  A_EDGE_FL                 9
#define  A_EDGE_BR                10
#define  A_EDGE_BL                11

#define  B_EDGE_UF                 0
#define  B_EDGE_UR                 1
#define  B_EDGE_UB                 2
#define  B_EDGE_UL                 3
#define  B_EDGE_DF                 4
#define  B_EDGE_DR                 5
#define  B_EDGE_DB                 6
#define  B_EDGE_DL                 7


/*  number the middle edges  */

#define  B_MID_FR                  0
#define  B_MID_FL                  1
#define  B_MID_BR                  2
#define  B_MID_BL                  3


/*  number the twists  */

#define  A_N_TWIST                18

#define  A_TWIST_F                 0
#define  A_TWIST_F2                1
#define  A_TWIST_F3                2
#define  A_TWIST_L                 3
#define  A_TWIST_L2                4
#define  A_TWIST_L3                5
#define  A_TWIST_U                 6
#define  A_TWIST_U2                7
#define  A_TWIST_U3                8
#define  A_TWIST_B                 9
#define  A_TWIST_B2               10
#define  A_TWIST_B3               11
#define  A_TWIST_R                12
#define  A_TWIST_R2               13
#define  A_TWIST_R3               14
#define  A_TWIST_D                15
#define  A_TWIST_D2               16
#define  A_TWIST_D3               17

#define  B_N_TWIST                10

#define  B_TWIST_F2                0
#define  B_TWIST_L2                1
#define  B_TWIST_U                 2
#define  B_TWIST_U2                3
#define  B_TWIST_U3                4
#define  B_TWIST_B2                5
#define  B_TWIST_R2                6
#define  B_TWIST_D                 7
#define  B_TWIST_D2                8
#define  B_TWIST_D3                9



#define  N_TWIST_LIST             11

#define  TWIST_LIST_INVALID        0
#define  TWIST_LIST_BEGIN          1
#define  TWIST_LIST_AFTER_F        2
#define  TWIST_LIST_AFTER_L        3
#define  TWIST_LIST_AFTER_U        4
#define  TWIST_LIST_AFTER_B        5
#define  TWIST_LIST_AFTER_R        6
#define  TWIST_LIST_AFTER_D        7
#define  TWIST_LIST_AFTER_FB       8
#define  TWIST_LIST_AFTER_RL       9
#define  TWIST_LIST_AFTER_UD      10







/*  global variables  */


/*  transformation tables  */

static int              twist_on_corner_table[A_N_TWIST][A_N_CORNER];
static int             *twist_on_corner_ptrs[A_N_TWIST];

static int              twist_on_eflip_table[A_N_TWIST][A_N_EFLIP];
static int             *twist_on_eflip_ptrs[A_N_TWIST];

static int              twist_on_eloc_table[A_N_TWIST][A_N_ELOC];
static int             *twist_on_eloc_ptrs[A_N_TWIST];


static unsigned short   twist_on_sliceedge_table[A_N_TWIST][N_SLICEEDGE];
static unsigned short  *twist_on_sliceedge_ptrs[A_N_TWIST];
static int              sliceedge_to_halfedgeperm[N_SLICEEDGE];
static unsigned short   half_to_edgeperm_table[N_HALFEDGEPERM][N_HALFEDGEPERM];
static unsigned short  *half_to_edgeperm_ptrs[N_HALFEDGEPERM];
static int              sliceedge_to_midperm[N_SLICEEDGE];




static unsigned short   twist_on_cornerperm_table[A_N_TWIST][B_N_CORNERPERM];
static unsigned short  *a_twist_on_cornerperm_ptrs[A_N_TWIST];
static unsigned short  *b_twist_on_cornerperm_ptrs[B_N_TWIST];

static unsigned short   twist_on_edgeperm_table[B_N_TWIST][B_N_EDGEPERM];
static unsigned short  *twist_on_edgeperm_ptrs[B_N_TWIST];

static int              twist_on_midperm_table[B_N_TWIST][B_N_MIDPERM];
static int             *twist_on_midperm_ptrs[B_N_TWIST];


static int              eloc_conv[A_N_ELOC];
static int              eloc_unconv[A_N_ELOC_CONV];


static int              halfedgeloc_conv[N_HALFEDGELOC];
static int              halfedgeloc_unconv[N_HALFEDGELOC_CONV];


/*  pruning tables  */

static unsigned char    corner_eflip_dist_table[A_N_CORNER][A_N_EFLIP];
static unsigned char   *corner_eflip_dist_ptrs[A_N_CORNER];

static unsigned char    corner_eloc_dist_table[A_N_CORNER][A_N_ELOC];
static unsigned char   *corner_eloc_dist_ptrs[A_N_CORNER];

static unsigned char    eflip_eloc_dist_table[A_N_EFLIP][A_N_ELOC];
static unsigned char   *eflip_eloc_dist_ptrs[A_N_EFLIP];


static int              cornerperm_dist_table[B_N_CORNERPERM];

static unsigned char    mid_cornerperm_dist_table[B_N_MIDPERM][B_N_CORNERPERM];
static unsigned char   *mid_cornerperm_dist_ptrs[B_N_MIDPERM];

static unsigned char    mid_edgeperm_dist_table[B_N_MIDPERM][B_N_EDGEPERM];
static unsigned char   *mid_edgeperm_dist_ptrs[B_N_MIDPERM];





static int              twist_list_table[6][N_TWIST_LIST];
static int             *a_twist_list_ptrs[A_N_TWIST];
static int             *b_twist_list_ptrs[B_N_TWIST];



static char            *edge_cubie_string[] = {"UF", "UR", "UB", "UL",
                                               "DF", "DR", "DB", "DL",
                                               "FR", "FL", "BR", "BL",
                                               "FU", "RU", "BU", "LU",
                                               "FD", "RD", "BD", "LD",
                                               "RF", "LF", "RB", "LB"};

static char            *corner_cubie_string[] = {"UFR", "URB", "UBL", "ULF",
                                                 "DRF", "DFL", "DLB", "DBR",
                                                 "FRU", "RBU", "BLU", "LFU",
                                                 "RFD", "FLD", "LBD", "BRD",
                                                 "RUF", "BUR", "LUB", "FUL",
                                                 "FDR", "LDF", "BDL", "RDB"};

static char            *a_twist_string[] = {"F ", "F2", "F'", "L ", "L2", "L'",
                                            "U ", "U2", "U'", "B ", "B2", "B'",
                                            "R ", "R2", "R'", "D ", "D2", "D'"};

static char            *b_twist_string[] = {"F2", "L2", "U ", "U2", "U'",
                                            "B2", "R2", "D ", "D2", "D'"};

static int              is_stage2_twist[A_N_TWIST];


static int              a_quarter_turn_length[A_N_TWIST];
static int              b_quarter_turn_length[B_N_TWIST];
static int              a_face_turn_length[A_N_TWIST];
static int              b_face_turn_length[B_N_TWIST];



static Cube            *p_current_cube;
static Options          user_options;

static int              shortest_solution;
static int              stage2_allotment;

static int              metric;
static int              increment;
static int             *a_metric_length;
static int             *b_metric_length;

static char             metric_char;


static jmp_buf          jump_env;

Solution  solve_main(char* state);