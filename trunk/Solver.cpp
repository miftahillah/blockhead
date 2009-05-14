/*  kociemba's sub-optimal cube algorithm  */

#pragma once

#define MAX_TIME 10

#include  <stdio.h>
#include  <stdlib.h>
#include  <setjmp.h>
#include  <signal.h>
#include <string.h>
#include "Solver.h"

bool tables_done = false;

   void  exit_w_error_message(char* msg)

{
printf("%s\n", msg);
exit(EXIT_FAILURE);

return;   /*  haha  */
}

int solve_start_time_secs = 0;
int solve_start_time_usecs = 0;

int shortest_sol = 100;

Solution best;

   void  user_interrupt(int unused_arg)

{
printf("\n-- user interrupt --\n");
longjmp(jump_env, 1);

return;  /*  haha  */
}



   void  perm_n_unpack(int nn, int indx, int array_out[])
{
int                     ii, jj;


for (ii = nn - 1; ii >= 0; ii--)
    {
    array_out[ii] = indx % (nn - ii);
    indx /= (nn - ii);

    for (jj = ii + 1; jj < nn; jj++)
        if (array_out[jj] >= array_out[ii])
           array_out[jj]++;
    }

return;
}



   int  perm_n_pack(int nn, int array_in[])
{
int                     indx, ii, jj;


indx = 0;

for (ii = 0; ii < nn; ii++)
    {
    indx *= (nn - ii);

    for (jj = ii + 1; jj < nn; jj++)
        if (array_in[jj] < array_in[ii])
           indx++;
    }

return indx;
}



int  check_perm_n(int nn, int array_in[])
{
int                     count[MAX_CHECK_PERM_N], ii;


for (ii = 0; ii < nn; ii++)
    count[ii] = 0;

for (ii = 0; ii < nn; ii++)
    {
    if (array_in[ii] < 0 || array_in[ii] >= nn)
       return 1;

    count[array_in[ii]]++;
    }

for (ii = 0; ii < nn; ii++)
    if (count[ii] != 1)
       return 1;

return 0;
}



int  parity_of_perm_n(int nn, int array_in[])
{
int                     ii, jj, par;


par = 0;

for (ii = 0; ii < nn - 1; ii++)
    for (jj = ii + 1; jj < nn; jj++)
        if (array_in[ii] > array_in[jj])
           par = 1 - par;

return par;
}



   void  two_cycle(int array[], int ind0, int ind1)
{
int                     temp;

temp = array[ind0];
array[ind0] = array[ind1];
array[ind1] = temp;
return;
}



   void  three_cycle(int array[], int ind0, int ind1, int ind2)
{
int                     temp;

temp = array[ind0];
array[ind0] = array[ind1];
array[ind1] = array[ind2];
array[ind2] = temp;
return;
}



   void  four_cycle(int array[], int ind0, int ind1, int ind2, int ind3)
{
int                     temp;

temp = array[ind0];
array[ind0] = array[ind1];
array[ind1] = array[ind2];
array[ind2] = array[ind3];
array[ind3] = temp;
return;
}



   void  conjugate_perm_n(int nn, int array_in[], int conjugator[], int array_out[])
{
int                     ii;

for (ii = 0; ii < nn; ii++)
    array_out[conjugator[ii]] = conjugator[array_in[ii]];
return;
}






   void  options_init(Options *p_options)
{
p_options->metric = metric = USE_METRIC;
p_options->limit_depth_stage2 = 0;

increment = (metric == QUARTER_TURN_METRIC) ? 2 : 1;
metric_char = (metric == QUARTER_TURN_METRIC) ? 'q' : 'f';

return;
}





   void  twist_lengths_init(int **p_a_lengths, int **p_b_lengths)
{
a_quarter_turn_length[A_TWIST_F] = a_quarter_turn_length[A_TWIST_F3] =
a_quarter_turn_length[A_TWIST_L] = a_quarter_turn_length[A_TWIST_L3] =
a_quarter_turn_length[A_TWIST_U] = a_quarter_turn_length[A_TWIST_U3] =
a_quarter_turn_length[A_TWIST_B] = a_quarter_turn_length[A_TWIST_B3] =
a_quarter_turn_length[A_TWIST_R] = a_quarter_turn_length[A_TWIST_R3] =
a_quarter_turn_length[A_TWIST_D] = a_quarter_turn_length[A_TWIST_D3] = 1;
a_quarter_turn_length[A_TWIST_F2] = a_quarter_turn_length[A_TWIST_L2] =
a_quarter_turn_length[A_TWIST_U2] = a_quarter_turn_length[A_TWIST_B2] =
a_quarter_turn_length[A_TWIST_R2] = a_quarter_turn_length[A_TWIST_D2] = 2;

b_quarter_turn_length[B_TWIST_U] = b_quarter_turn_length[B_TWIST_U3] =
b_quarter_turn_length[B_TWIST_D] = b_quarter_turn_length[B_TWIST_D3] = 1;
b_quarter_turn_length[B_TWIST_F2] = b_quarter_turn_length[B_TWIST_L2] =
b_quarter_turn_length[B_TWIST_U2] = b_quarter_turn_length[B_TWIST_B2] =
b_quarter_turn_length[B_TWIST_R2] = b_quarter_turn_length[B_TWIST_D2] = 2;


a_face_turn_length[A_TWIST_F] = a_face_turn_length[A_TWIST_F2] =
                                a_face_turn_length[A_TWIST_F3] = 
a_face_turn_length[A_TWIST_L] = a_face_turn_length[A_TWIST_L2] =
                                a_face_turn_length[A_TWIST_L3] = 
a_face_turn_length[A_TWIST_U] = a_face_turn_length[A_TWIST_U2] =
                                a_face_turn_length[A_TWIST_U3] = 
a_face_turn_length[A_TWIST_B] = a_face_turn_length[A_TWIST_B2] =
                                a_face_turn_length[A_TWIST_B3] = 
a_face_turn_length[A_TWIST_R] = a_face_turn_length[A_TWIST_R2] =
                                a_face_turn_length[A_TWIST_R3] = 
a_face_turn_length[A_TWIST_D] = a_face_turn_length[A_TWIST_D2] =
                                a_face_turn_length[A_TWIST_D3] = 1;

b_face_turn_length[B_TWIST_F2] = b_face_turn_length[B_TWIST_L2] =
b_face_turn_length[B_TWIST_U ] = b_face_turn_length[B_TWIST_U2] =
                                 b_face_turn_length[B_TWIST_U3] =
b_face_turn_length[B_TWIST_B2] = b_face_turn_length[B_TWIST_R2] =
b_face_turn_length[B_TWIST_D ] = b_face_turn_length[B_TWIST_D2] =
                                 b_face_turn_length[B_TWIST_D3] = 1;

if (user_options.metric == QUARTER_TURN_METRIC)
   {
   *p_a_lengths = a_quarter_turn_length;
   *p_b_lengths = b_quarter_turn_length;
   }
else if (user_options.metric == FACE_TURN_METRIC)
        {
        *p_a_lengths = a_face_turn_length;
        *p_b_lengths = b_face_turn_length;
        }
else
   exit_w_error_message("twist_lengths_init : unknown metric");

return;
}





   void  is_stage2_twist_init(int twist_arr[A_N_TWIST])
{
twist_arr[A_TWIST_F] = twist_arr[A_TWIST_F3] =
twist_arr[A_TWIST_L] = twist_arr[A_TWIST_L3] =
twist_arr[A_TWIST_B] = twist_arr[A_TWIST_B3] =
twist_arr[A_TWIST_R] = twist_arr[A_TWIST_R3] = 0;

twist_arr[A_TWIST_F2] = twist_arr[A_TWIST_L2] = 
twist_arr[A_TWIST_U ] = twist_arr[A_TWIST_U2] = twist_arr[A_TWIST_U2] =
twist_arr[A_TWIST_B2] = twist_arr[A_TWIST_R2] =
twist_arr[A_TWIST_D ] = twist_arr[A_TWIST_D2] = twist_arr[A_TWIST_D3] = 1;

return;
}





   void  twist_sequences_init(int tw_table[6][N_TWIST_LIST], int *a_tw_ptrs[A_N_TWIST], int *b_tw_ptrs[B_N_TWIST])
{
a_tw_ptrs[A_TWIST_F ] = a_tw_ptrs[A_TWIST_F2] = a_tw_ptrs[A_TWIST_F3] =
                                                                 tw_table[0];
a_tw_ptrs[A_TWIST_L ] = a_tw_ptrs[A_TWIST_L2] = a_tw_ptrs[A_TWIST_L3] =
                                                                 tw_table[1];
a_tw_ptrs[A_TWIST_U ] = a_tw_ptrs[A_TWIST_U2] = a_tw_ptrs[A_TWIST_U3] =
                                                                 tw_table[2];
a_tw_ptrs[A_TWIST_B ] = a_tw_ptrs[A_TWIST_B2] = a_tw_ptrs[A_TWIST_B3] =
                                                                 tw_table[3];
a_tw_ptrs[A_TWIST_R ] = a_tw_ptrs[A_TWIST_R2] = a_tw_ptrs[A_TWIST_R3] =
                                                                 tw_table[4];
a_tw_ptrs[A_TWIST_D ] = a_tw_ptrs[A_TWIST_D2] = a_tw_ptrs[A_TWIST_D3] =
                                                                 tw_table[5];

a_tw_ptrs[A_TWIST_F][TWIST_LIST_BEGIN] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_L] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_U] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_R] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_D] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_RL] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_UD] = TWIST_LIST_AFTER_F;
a_tw_ptrs[A_TWIST_F][TWIST_LIST_INVALID] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_F] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_B] =
a_tw_ptrs[A_TWIST_F][TWIST_LIST_AFTER_FB] = TWIST_LIST_INVALID;

a_tw_ptrs[A_TWIST_L][TWIST_LIST_BEGIN] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_F] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_U] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_B] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_D] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_FB] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_UD] = TWIST_LIST_AFTER_L;
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_R] = TWIST_LIST_AFTER_RL;
a_tw_ptrs[A_TWIST_L][TWIST_LIST_INVALID] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_L] =
a_tw_ptrs[A_TWIST_L][TWIST_LIST_AFTER_RL] = TWIST_LIST_INVALID;

a_tw_ptrs[A_TWIST_U][TWIST_LIST_BEGIN] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_F] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_L] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_B] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_R] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_FB] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_RL] = TWIST_LIST_AFTER_U;
a_tw_ptrs[A_TWIST_U][TWIST_LIST_INVALID] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_U] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_D] =
a_tw_ptrs[A_TWIST_U][TWIST_LIST_AFTER_UD] = TWIST_LIST_INVALID;

a_tw_ptrs[A_TWIST_B][TWIST_LIST_BEGIN] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_L] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_U] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_R] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_D] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_RL] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_UD] = TWIST_LIST_AFTER_B;
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_F] = TWIST_LIST_AFTER_FB;
a_tw_ptrs[A_TWIST_B][TWIST_LIST_INVALID] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_B] =
a_tw_ptrs[A_TWIST_B][TWIST_LIST_AFTER_FB] = TWIST_LIST_INVALID;

a_tw_ptrs[A_TWIST_R][TWIST_LIST_BEGIN] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_F] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_U] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_B] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_D] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_FB] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_UD] = TWIST_LIST_AFTER_R;
a_tw_ptrs[A_TWIST_R][TWIST_LIST_INVALID] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_L] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_R] =
a_tw_ptrs[A_TWIST_R][TWIST_LIST_AFTER_RL] = TWIST_LIST_INVALID;

a_tw_ptrs[A_TWIST_D][TWIST_LIST_BEGIN] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_F] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_L] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_B] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_R] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_FB] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_RL] = TWIST_LIST_AFTER_D;
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_U] = TWIST_LIST_AFTER_UD;
a_tw_ptrs[A_TWIST_D][TWIST_LIST_INVALID] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_D] =
a_tw_ptrs[A_TWIST_D][TWIST_LIST_AFTER_UD] = TWIST_LIST_INVALID;

b_tw_ptrs[B_TWIST_F2] = tw_table[0];
b_tw_ptrs[B_TWIST_L2] = tw_table[1];
b_tw_ptrs[B_TWIST_U ] = b_tw_ptrs[B_TWIST_U2] = b_tw_ptrs[B_TWIST_U3] =
                                                                  tw_table[2];
b_tw_ptrs[B_TWIST_B2] = tw_table[3];
b_tw_ptrs[B_TWIST_R2] = tw_table[4];
b_tw_ptrs[B_TWIST_D ] = b_tw_ptrs[B_TWIST_D2] = b_tw_ptrs[B_TWIST_D3] =
                                                                  tw_table[5];
return;
}



   void  a_corner_unpack(int corner, int array_out[8])
{
int                     ii;

for (ii = 0; ii < 7; ii++)
    {
    array_out[ii] = corner % 3;
    corner = corner / 3;
    }

array_out[7] = (2 * (array_out[0] + array_out[1] + array_out[2] + array_out[3]
                         + array_out[4] + array_out[5] + array_out[6])) % 3;
return;
}



   int  a_corner_pack(int array_in[8])
{
int                     corner, ii;

corner = 0;
for (ii = 6; ii >= 0; ii--)
    corner = 3 * corner + array_in[ii];

return corner;
}



   void  adjust_corner(int array[8], int ind0, int ind1, int ind2, int ind3)
{
array[ind0] = (array[ind0] + 1) % 3;
array[ind1] = (array[ind1] + 2) % 3;
array[ind2] = (array[ind2] + 1) % 3;
array[ind3] = (array[ind3] + 2) % 3;
return;
}



   int  a_twist_f_on_corner(int corner)
{
int                     temp_arr[8];

a_corner_unpack(corner, temp_arr);
four_cycle(temp_arr, A_CORNER_DFL, A_CORNER_DRF, A_CORNER_UFR, A_CORNER_ULF);
adjust_corner(temp_arr, A_CORNER_DFL, A_CORNER_DRF, A_CORNER_UFR, A_CORNER_ULF);
return a_corner_pack(temp_arr);
}



   int  a_twist_l_on_corner(int corner)
{
int                     temp_arr[8];

a_corner_unpack(corner, temp_arr);
four_cycle(temp_arr, A_CORNER_DLB, A_CORNER_DFL, A_CORNER_ULF, A_CORNER_UBL);
adjust_corner(temp_arr, A_CORNER_DLB, A_CORNER_DFL, A_CORNER_ULF, A_CORNER_UBL);
return a_corner_pack(temp_arr);
}



   int  a_twist_u_on_corner(int corner)
{
int                     temp_arr[8];

a_corner_unpack(corner, temp_arr);
four_cycle(temp_arr, A_CORNER_UFR, A_CORNER_URB, A_CORNER_UBL, A_CORNER_ULF);
return a_corner_pack(temp_arr);
}



   int  a_twist_b_on_corner(int corner)
{
int                     temp_arr[8];

a_corner_unpack(corner, temp_arr);
four_cycle(temp_arr, A_CORNER_DBR, A_CORNER_DLB, A_CORNER_UBL, A_CORNER_URB);
adjust_corner(temp_arr, A_CORNER_DBR, A_CORNER_DLB, A_CORNER_UBL, A_CORNER_URB);
return a_corner_pack(temp_arr);
}



   int  a_twist_r_on_corner(int corner)
{
int                     temp_arr[8];

a_corner_unpack(corner, temp_arr);
four_cycle(temp_arr, A_CORNER_DRF, A_CORNER_DBR, A_CORNER_URB, A_CORNER_UFR);
adjust_corner(temp_arr, A_CORNER_DRF, A_CORNER_DBR, A_CORNER_URB, A_CORNER_UFR);
return a_corner_pack(temp_arr);
}



   int  a_twist_d_on_corner(int corner)
{
int                     temp_arr[8];

a_corner_unpack(corner, temp_arr);
four_cycle(temp_arr, A_CORNER_DFL, A_CORNER_DLB, A_CORNER_DBR, A_CORNER_DRF);
return a_corner_pack(temp_arr);
}



   void  a_twist_on_corner_init(int t_on_c_table[A_N_TWIST][A_N_CORNER], int *t_on_c_ptrs[A_N_TWIST])
{
int                     twist, corner;


for (twist = 0; twist < A_N_TWIST; twist++)
    t_on_c_ptrs[twist] = t_on_c_table[twist];

for (corner = 0; corner < A_N_CORNER; corner++)
    {
    t_on_c_ptrs[A_TWIST_F][corner] = a_twist_f_on_corner(corner);
    t_on_c_ptrs[A_TWIST_L][corner] = a_twist_l_on_corner(corner);
    t_on_c_ptrs[A_TWIST_U][corner] = a_twist_u_on_corner(corner);
    t_on_c_ptrs[A_TWIST_B][corner] = a_twist_b_on_corner(corner);
    t_on_c_ptrs[A_TWIST_R][corner] = a_twist_r_on_corner(corner);
    t_on_c_ptrs[A_TWIST_D][corner] = a_twist_d_on_corner(corner);
    }
for (corner = 0; corner < A_N_CORNER; corner++)
    {
    t_on_c_ptrs[A_TWIST_F2][corner] =
                      t_on_c_ptrs[A_TWIST_F][t_on_c_ptrs[A_TWIST_F][corner]];
    t_on_c_ptrs[A_TWIST_L2][corner] =
                      t_on_c_ptrs[A_TWIST_L][t_on_c_ptrs[A_TWIST_L][corner]];
    t_on_c_ptrs[A_TWIST_U2][corner] =
                      t_on_c_ptrs[A_TWIST_U][t_on_c_ptrs[A_TWIST_U][corner]];
    t_on_c_ptrs[A_TWIST_B2][corner] =
                      t_on_c_ptrs[A_TWIST_B][t_on_c_ptrs[A_TWIST_B][corner]];
    t_on_c_ptrs[A_TWIST_R2][corner] =
                      t_on_c_ptrs[A_TWIST_R][t_on_c_ptrs[A_TWIST_R][corner]];
    t_on_c_ptrs[A_TWIST_D2][corner] =
                      t_on_c_ptrs[A_TWIST_D][t_on_c_ptrs[A_TWIST_D][corner]];
    }
for (corner = 0; corner < A_N_CORNER; corner++)
    {
    t_on_c_ptrs[A_TWIST_F3][corner] =
                      t_on_c_ptrs[A_TWIST_F2][t_on_c_ptrs[A_TWIST_F][corner]];
    t_on_c_ptrs[A_TWIST_L3][corner] =
                      t_on_c_ptrs[A_TWIST_L2][t_on_c_ptrs[A_TWIST_L][corner]];
    t_on_c_ptrs[A_TWIST_U3][corner] =
                      t_on_c_ptrs[A_TWIST_U2][t_on_c_ptrs[A_TWIST_U][corner]];
    t_on_c_ptrs[A_TWIST_B3][corner] =
                      t_on_c_ptrs[A_TWIST_B2][t_on_c_ptrs[A_TWIST_B][corner]];
    t_on_c_ptrs[A_TWIST_R3][corner] =
                      t_on_c_ptrs[A_TWIST_R2][t_on_c_ptrs[A_TWIST_R][corner]];
    t_on_c_ptrs[A_TWIST_D3][corner] =
                      t_on_c_ptrs[A_TWIST_D2][t_on_c_ptrs[A_TWIST_D][corner]];
    }

return;
}




   void  a_eflip_unpack(int eflip, int array_out[12])
{
int                     ii;

for (ii = 0; ii < 11; ii++)
    {
    array_out[ii] = eflip % 2;
    eflip = eflip / 2;
    }
array_out[11] = (array_out[0] + array_out[1] + array_out[2] + array_out[3] +
                 array_out[4] + array_out[5] + array_out[6] + array_out[7] +
                 array_out[8] + array_out[9] + array_out[10]) % 2;
return;
}



   int  a_eflip_pack(int array_in[12])
{
int                     eflip, ii;

eflip = 0;
for (ii = 10; ii >= 0; ii--)
    eflip = 2 * eflip + array_in[ii];

return eflip;
}



   void  adjust_eflip(int array[12], int ind0, int ind1, int ind2, int ind3)
{
array[ind0] = 1 - array[ind0];
array[ind1] = 1 - array[ind1];
array[ind2] = 1 - array[ind2];
array[ind3] = 1 - array[ind3];
return;
}



   int  a_twist_f_on_eflip(int eflip)
{
int                     temp_arr[12];

a_eflip_unpack(eflip, temp_arr);
four_cycle(temp_arr, A_EDGE_FL, A_EDGE_DF, A_EDGE_FR, A_EDGE_UF);
adjust_eflip(temp_arr, A_EDGE_FL, A_EDGE_DF, A_EDGE_FR, A_EDGE_UF);
return a_eflip_pack(temp_arr);
}



   int  a_twist_l_on_eflip(int eflip)
{
int                     temp_arr[12];

a_eflip_unpack(eflip, temp_arr);
four_cycle(temp_arr, A_EDGE_BL, A_EDGE_DL, A_EDGE_FL, A_EDGE_UL);
return a_eflip_pack(temp_arr);
}



   int  a_twist_u_on_eflip(int eflip)
{
int                     temp_arr[12];

a_eflip_unpack(eflip, temp_arr);
four_cycle(temp_arr, A_EDGE_UR, A_EDGE_UB, A_EDGE_UL, A_EDGE_UF);
return a_eflip_pack(temp_arr);
}



   int  a_twist_b_on_eflip(int eflip)
{
int                     temp_arr[12];

a_eflip_unpack(eflip, temp_arr);
four_cycle(temp_arr, A_EDGE_BR, A_EDGE_DB, A_EDGE_BL, A_EDGE_UB);
adjust_eflip(temp_arr, A_EDGE_BR, A_EDGE_DB, A_EDGE_BL, A_EDGE_UB);
return a_eflip_pack(temp_arr);
}



   int  a_twist_r_on_eflip(int eflip)
{
int                     temp_arr[12];

a_eflip_unpack(eflip, temp_arr);
four_cycle(temp_arr, A_EDGE_FR, A_EDGE_DR, A_EDGE_BR, A_EDGE_UR);
return a_eflip_pack(temp_arr);
}



   int  a_twist_d_on_eflip(int eflip)
{
int                     temp_arr[12];

a_eflip_unpack(eflip, temp_arr);
four_cycle(temp_arr, A_EDGE_DL, A_EDGE_DB, A_EDGE_DR, A_EDGE_DF);
return a_eflip_pack(temp_arr);
}



   void  a_twist_on_eflip_init(int t_on_ef_table[A_N_TWIST][A_N_EFLIP],int *t_on_ef_ptrs[A_N_TWIST])

{
int                     twist, eflip;

for (twist = 0; twist < A_N_TWIST; twist++)
    t_on_ef_ptrs[twist] = t_on_ef_table[twist];

for (eflip = 0; eflip < A_N_EFLIP; eflip++)
    {
    t_on_ef_ptrs[A_TWIST_F][eflip] = a_twist_f_on_eflip(eflip);
    t_on_ef_ptrs[A_TWIST_L][eflip] = a_twist_l_on_eflip(eflip);
    t_on_ef_ptrs[A_TWIST_U][eflip] = a_twist_u_on_eflip(eflip);
    t_on_ef_ptrs[A_TWIST_B][eflip] = a_twist_b_on_eflip(eflip);
    t_on_ef_ptrs[A_TWIST_R][eflip] = a_twist_r_on_eflip(eflip);
    t_on_ef_ptrs[A_TWIST_D][eflip] = a_twist_d_on_eflip(eflip);
    }
for (eflip = 0; eflip < A_N_EFLIP; eflip++)
    {
    t_on_ef_ptrs[A_TWIST_F2][eflip] =
                    t_on_ef_ptrs[A_TWIST_F][t_on_ef_ptrs[A_TWIST_F][eflip]];
    t_on_ef_ptrs[A_TWIST_L2][eflip] =
                    t_on_ef_ptrs[A_TWIST_L][t_on_ef_ptrs[A_TWIST_L][eflip]];
    t_on_ef_ptrs[A_TWIST_U2][eflip] =
                    t_on_ef_ptrs[A_TWIST_U][t_on_ef_ptrs[A_TWIST_U][eflip]];
    t_on_ef_ptrs[A_TWIST_B2][eflip] =
                    t_on_ef_ptrs[A_TWIST_B][t_on_ef_ptrs[A_TWIST_B][eflip]];
    t_on_ef_ptrs[A_TWIST_R2][eflip] =
                    t_on_ef_ptrs[A_TWIST_R][t_on_ef_ptrs[A_TWIST_R][eflip]];
    t_on_ef_ptrs[A_TWIST_D2][eflip] =
                    t_on_ef_ptrs[A_TWIST_D][t_on_ef_ptrs[A_TWIST_D][eflip]];
    }
for (eflip = 0; eflip < A_N_EFLIP; eflip++)
    {
    t_on_ef_ptrs[A_TWIST_F3][eflip] =
                    t_on_ef_ptrs[A_TWIST_F2][t_on_ef_ptrs[A_TWIST_F][eflip]];
    t_on_ef_ptrs[A_TWIST_L3][eflip] =
                    t_on_ef_ptrs[A_TWIST_L2][t_on_ef_ptrs[A_TWIST_L][eflip]];
    t_on_ef_ptrs[A_TWIST_U3][eflip] =
                    t_on_ef_ptrs[A_TWIST_U2][t_on_ef_ptrs[A_TWIST_U][eflip]];
    t_on_ef_ptrs[A_TWIST_B3][eflip] =
                    t_on_ef_ptrs[A_TWIST_B2][t_on_ef_ptrs[A_TWIST_B][eflip]];
    t_on_ef_ptrs[A_TWIST_R3][eflip] =
                    t_on_ef_ptrs[A_TWIST_R2][t_on_ef_ptrs[A_TWIST_R][eflip]];
    t_on_ef_ptrs[A_TWIST_D3][eflip] =
                    t_on_ef_ptrs[A_TWIST_D2][t_on_ef_ptrs[A_TWIST_D][eflip]];
    }

return;
}



   void  a_eloc_conv_init(int conv_tab[A_N_ELOC], int unconv_tab[A_N_ELOC_CONV])
{
int                     ii, loc0, loc1, loc2, loc3, count;


for (ii = 0; ii < A_N_ELOC; ii++)
    conv_tab[ii] = 0;

for (ii = 0; ii < A_N_ELOC_CONV; ii++)
    unconv_tab[ii] = 0;

count = 0;
for (loc0 = 0; loc0 < 9; loc0++)
    for (loc1 = loc0 + 1; loc1 < 10; loc1++)
        for (loc2 = loc1 + 1; loc2 < 11; loc2++)
            for (loc3 = loc2 + 1; loc3 < 12; loc3++)
                {
                if (count >= A_N_ELOC)
                   exit_w_error_message("a_eloc_conv_init : too many eloc's");
                conv_tab[count] = (1 << loc0) | (1 << loc1) |
                                  (1 << loc2) | (1 << loc3);
                unconv_tab[conv_tab[count]] = count;
                count++;
                }
return;
}



   void  a_eloc_unpack(int eloc, int array_out[12])
{
int                     conv, ii;


conv = eloc_conv[eloc];

for (ii = 0; ii < 12; ii++)
    {
    array_out[ii] = conv % 2;
    conv = conv / 2;
    }

return;
}



   int  a_eloc_pack(int array_in[12])
{
int                     ii, conv;

conv = 0;
for (ii = 11; ii >= 0; ii--)
    conv = 2 * conv + array_in[ii];

return eloc_unconv[conv];
}



   int  a_twist_f_on_eloc(int eloc)
{
int                     temp_arr[12];

a_eloc_unpack(eloc, temp_arr);
four_cycle(temp_arr, A_EDGE_FL, A_EDGE_DF, A_EDGE_FR, A_EDGE_UF);
return a_eloc_pack(temp_arr);
}



   int  a_twist_l_on_eloc(int eloc)
{
int                     temp_arr[12];

a_eloc_unpack(eloc, temp_arr);
four_cycle(temp_arr, A_EDGE_BL, A_EDGE_DL, A_EDGE_FL, A_EDGE_UL);
return a_eloc_pack(temp_arr);
}



   int  a_twist_u_on_eloc(int eloc)
{
int                     temp_arr[12];

a_eloc_unpack(eloc, temp_arr);
four_cycle(temp_arr, A_EDGE_UF, A_EDGE_UR, A_EDGE_UB, A_EDGE_UL);
return a_eloc_pack(temp_arr);
}



   int  a_twist_b_on_eloc(int eloc)
{
int                     temp_arr[12];

a_eloc_unpack(eloc, temp_arr);
four_cycle(temp_arr, A_EDGE_BR, A_EDGE_DB, A_EDGE_BL, A_EDGE_UB);
return a_eloc_pack(temp_arr);
}



   int  a_twist_r_on_eloc(int eloc)
{
int                     temp_arr[12];

a_eloc_unpack(eloc, temp_arr);
four_cycle(temp_arr, A_EDGE_FR, A_EDGE_DR, A_EDGE_BR, A_EDGE_UR);
return a_eloc_pack(temp_arr);
}



   int  a_twist_d_on_eloc(int eloc)
{
int                     temp_arr[12];

a_eloc_unpack(eloc, temp_arr);
four_cycle(temp_arr, A_EDGE_DF, A_EDGE_DL, A_EDGE_DB, A_EDGE_DR);
return a_eloc_pack(temp_arr);
}



   void  a_twist_on_eloc_init(int t_on_el_table[A_N_TWIST][A_N_ELOC],  int *t_on_el_ptrs[A_N_TWIST])
{
int                     twist, eloc;


a_eloc_conv_init(eloc_conv, eloc_unconv);

for (twist = 0; twist < A_N_TWIST; twist++)
    t_on_el_ptrs[twist] = t_on_el_table[twist];

for (eloc = 0; eloc < A_N_ELOC; eloc++)
    {
    t_on_el_ptrs[A_TWIST_F][eloc] = a_twist_f_on_eloc(eloc);
    t_on_el_ptrs[A_TWIST_L][eloc] = a_twist_l_on_eloc(eloc);
    t_on_el_ptrs[A_TWIST_U][eloc] = a_twist_u_on_eloc(eloc);
    t_on_el_ptrs[A_TWIST_B][eloc] = a_twist_b_on_eloc(eloc);
    t_on_el_ptrs[A_TWIST_R][eloc] = a_twist_r_on_eloc(eloc);
    t_on_el_ptrs[A_TWIST_D][eloc] = a_twist_d_on_eloc(eloc);
    }
for (eloc = 0; eloc < A_N_ELOC; eloc++)
    {
    t_on_el_ptrs[A_TWIST_F2][eloc] =
                   t_on_el_ptrs[A_TWIST_F][t_on_el_ptrs[A_TWIST_F][eloc]];
    t_on_el_ptrs[A_TWIST_L2][eloc] =
                   t_on_el_ptrs[A_TWIST_L][t_on_el_ptrs[A_TWIST_L][eloc]];
    t_on_el_ptrs[A_TWIST_U2][eloc] =
                   t_on_el_ptrs[A_TWIST_U][t_on_el_ptrs[A_TWIST_U][eloc]];
    t_on_el_ptrs[A_TWIST_B2][eloc] =
                   t_on_el_ptrs[A_TWIST_B][t_on_el_ptrs[A_TWIST_B][eloc]];
    t_on_el_ptrs[A_TWIST_R2][eloc] =
                   t_on_el_ptrs[A_TWIST_R][t_on_el_ptrs[A_TWIST_R][eloc]];
    t_on_el_ptrs[A_TWIST_D2][eloc] =
                   t_on_el_ptrs[A_TWIST_D][t_on_el_ptrs[A_TWIST_D][eloc]];
    }
for (eloc = 0; eloc < A_N_ELOC; eloc++)
    {
    t_on_el_ptrs[A_TWIST_F3][eloc] =
                   t_on_el_ptrs[A_TWIST_F2][t_on_el_ptrs[A_TWIST_F][eloc]];
    t_on_el_ptrs[A_TWIST_L3][eloc] =
                   t_on_el_ptrs[A_TWIST_L2][t_on_el_ptrs[A_TWIST_L][eloc]];
    t_on_el_ptrs[A_TWIST_U3][eloc] =
                   t_on_el_ptrs[A_TWIST_U2][t_on_el_ptrs[A_TWIST_U][eloc]];
    t_on_el_ptrs[A_TWIST_B3][eloc] =
                   t_on_el_ptrs[A_TWIST_B2][t_on_el_ptrs[A_TWIST_B][eloc]];
    t_on_el_ptrs[A_TWIST_R3][eloc] =
                   t_on_el_ptrs[A_TWIST_R2][t_on_el_ptrs[A_TWIST_R][eloc]];
    t_on_el_ptrs[A_TWIST_D3][eloc] =
                   t_on_el_ptrs[A_TWIST_D2][t_on_el_ptrs[A_TWIST_D][eloc]];
    }

return;
}







   void  cornerperm_unpack(int cperm, int array_out[8])
{
perm_n_unpack(8, cperm, array_out);
return;
}



   int  cornerperm_pack(int array_in[8])
{
return perm_n_pack(8, array_in);
}



   int  twist_f_on_cornerperm(int cperm)
{
int                     temp_arr[8];

cornerperm_unpack(cperm, temp_arr);
four_cycle(temp_arr, B_CORNER_DFL, B_CORNER_DRF, B_CORNER_UFR, B_CORNER_ULF);
return cornerperm_pack(temp_arr);
}



   int  twist_l_on_cornerperm(int cperm)
{
int                     temp_arr[8];

cornerperm_unpack(cperm, temp_arr);
four_cycle(temp_arr, B_CORNER_DLB, B_CORNER_DFL, B_CORNER_ULF, B_CORNER_UBL);
return cornerperm_pack(temp_arr);
}



   int  twist_u_on_cornerperm( int cperm)
{
int                     temp_arr[8];

cornerperm_unpack(cperm, temp_arr);
four_cycle(temp_arr, B_CORNER_URB, B_CORNER_UBL, B_CORNER_ULF, B_CORNER_UFR);
return cornerperm_pack(temp_arr);
}



   int  twist_b_on_cornerperm(int cperm)
{
int                     temp_arr[8];

cornerperm_unpack(cperm, temp_arr);
four_cycle(temp_arr, B_CORNER_DBR, B_CORNER_DLB, B_CORNER_UBL, B_CORNER_URB);
return cornerperm_pack(temp_arr);
}



   int  twist_r_on_cornerperm(int cperm)
{
int                     temp_arr[8];

cornerperm_unpack(cperm, temp_arr);
four_cycle(temp_arr, B_CORNER_DRF, B_CORNER_DBR, B_CORNER_URB, B_CORNER_UFR);
return cornerperm_pack(temp_arr);
}



   int  twist_d_on_cornerperm(int cperm)
{
int                     temp_arr[8];

cornerperm_unpack(cperm, temp_arr);
four_cycle(temp_arr, B_CORNER_DFL, B_CORNER_DLB, B_CORNER_DBR, B_CORNER_DRF);
return cornerperm_pack(temp_arr);
}



   void  b_twist_on_cornerperm_init(unsigned short t_on_cp_table[A_N_TWIST][B_N_CORNERPERM],unsigned short *t_on_cp_ptrs[A_N_TWIST], unsigned short *b_t_on_cp_ptrs[B_N_TWIST])
{
int                     twist, cperm;


for (twist = 0; twist < A_N_TWIST; twist++)
    t_on_cp_ptrs[twist] = t_on_cp_table[twist];

for (cperm = 0; cperm < B_N_CORNERPERM; cperm++)
    {
    t_on_cp_ptrs[A_TWIST_F][cperm] =
                                (unsigned short)twist_f_on_cornerperm(cperm);
    t_on_cp_ptrs[A_TWIST_L][cperm] =
                                (unsigned short)twist_l_on_cornerperm(cperm);
    t_on_cp_ptrs[A_TWIST_U][cperm] =
                                (unsigned short)twist_u_on_cornerperm(cperm);
    t_on_cp_ptrs[A_TWIST_B][cperm] =
                                (unsigned short)twist_b_on_cornerperm(cperm);
    t_on_cp_ptrs[A_TWIST_R][cperm] =
                                (unsigned short)twist_r_on_cornerperm(cperm);
    t_on_cp_ptrs[A_TWIST_D][cperm] =
                                (unsigned short)twist_d_on_cornerperm(cperm);
    }
for (cperm = 0; cperm < B_N_CORNERPERM; cperm++)
    {
    t_on_cp_ptrs[A_TWIST_F2][cperm] =
                 t_on_cp_ptrs[A_TWIST_F][(int)t_on_cp_ptrs[A_TWIST_F][cperm]];
    t_on_cp_ptrs[A_TWIST_L2][cperm] =
                 t_on_cp_ptrs[A_TWIST_L][(int)t_on_cp_ptrs[A_TWIST_L][cperm]];
    t_on_cp_ptrs[A_TWIST_U2][cperm] =
                 t_on_cp_ptrs[A_TWIST_U][(int)t_on_cp_ptrs[A_TWIST_U][cperm]];
    t_on_cp_ptrs[A_TWIST_B2][cperm] =
                 t_on_cp_ptrs[A_TWIST_B][(int)t_on_cp_ptrs[A_TWIST_B][cperm]];
    t_on_cp_ptrs[A_TWIST_R2][cperm] =
                 t_on_cp_ptrs[A_TWIST_R][(int)t_on_cp_ptrs[A_TWIST_R][cperm]];
    t_on_cp_ptrs[A_TWIST_D2][cperm] =
                 t_on_cp_ptrs[A_TWIST_D][(int)t_on_cp_ptrs[A_TWIST_D][cperm]];
    }
for (cperm = 0; cperm < B_N_CORNERPERM; cperm++)
    {
    t_on_cp_ptrs[A_TWIST_F3][cperm] =
                 t_on_cp_ptrs[A_TWIST_F2][(int)t_on_cp_ptrs[A_TWIST_F][cperm]];
    t_on_cp_ptrs[A_TWIST_L3][cperm] =
                 t_on_cp_ptrs[A_TWIST_L2][(int)t_on_cp_ptrs[A_TWIST_L][cperm]];
    t_on_cp_ptrs[A_TWIST_U3][cperm] =
                 t_on_cp_ptrs[A_TWIST_U2][(int)t_on_cp_ptrs[A_TWIST_U][cperm]];
    t_on_cp_ptrs[A_TWIST_B3][cperm] =
                 t_on_cp_ptrs[A_TWIST_B2][(int)t_on_cp_ptrs[A_TWIST_B][cperm]];
    t_on_cp_ptrs[A_TWIST_R3][cperm] =
                 t_on_cp_ptrs[A_TWIST_R2][(int)t_on_cp_ptrs[A_TWIST_R][cperm]];
    t_on_cp_ptrs[A_TWIST_D3][cperm] =
                 t_on_cp_ptrs[A_TWIST_D2][(int)t_on_cp_ptrs[A_TWIST_D][cperm]];
    }

b_t_on_cp_ptrs[B_TWIST_F2] = t_on_cp_ptrs[A_TWIST_F2];
b_t_on_cp_ptrs[B_TWIST_L2] = t_on_cp_ptrs[A_TWIST_L2];
b_t_on_cp_ptrs[B_TWIST_U ] = t_on_cp_ptrs[A_TWIST_U ];
b_t_on_cp_ptrs[B_TWIST_U2] = t_on_cp_ptrs[A_TWIST_U2];
b_t_on_cp_ptrs[B_TWIST_U3] = t_on_cp_ptrs[A_TWIST_U3];
b_t_on_cp_ptrs[B_TWIST_B2] = t_on_cp_ptrs[A_TWIST_B2];
b_t_on_cp_ptrs[B_TWIST_R2] = t_on_cp_ptrs[A_TWIST_R2];
b_t_on_cp_ptrs[B_TWIST_D ] = t_on_cp_ptrs[A_TWIST_D ];
b_t_on_cp_ptrs[B_TWIST_D2] = t_on_cp_ptrs[A_TWIST_D2];
b_t_on_cp_ptrs[B_TWIST_D3] = t_on_cp_ptrs[A_TWIST_D3];

return;
}



   void  b_edgeperm_unpack(int eperm, int array_out[8])
{
perm_n_unpack(8, eperm, array_out);
return;
}



   int  b_edgeperm_pack(int array_in[8])
{
return perm_n_pack(8, array_in);
}




   int  b_twist_f2_on_edgeperm(int eperm)
{
int                     temp_arr[8];

b_edgeperm_unpack(eperm, temp_arr);
two_cycle(temp_arr, B_EDGE_UF, B_EDGE_DF);
return b_edgeperm_pack(temp_arr);
}



   int  b_twist_l2_on_edgeperm(int eperm)
{
int                     temp_arr[8];

b_edgeperm_unpack(eperm, temp_arr);
two_cycle(temp_arr, B_EDGE_UL, B_EDGE_DL);
return b_edgeperm_pack(temp_arr);
}



   int  b_twist_u_on_edgeperm(int eperm)
{
int                     temp_arr[8];

b_edgeperm_unpack(eperm, temp_arr);
four_cycle(temp_arr, B_EDGE_UR, B_EDGE_UB, B_EDGE_UL, B_EDGE_UF);
return b_edgeperm_pack(temp_arr);
}



   int  b_twist_b2_on_edgeperm(int eperm)
{
int                     temp_arr[8];

b_edgeperm_unpack(eperm, temp_arr);
two_cycle(temp_arr, B_EDGE_UB, B_EDGE_DB);
return b_edgeperm_pack(temp_arr);
}



   int  b_twist_r2_on_edgeperm(int eperm)
{
int                     temp_arr[8];

b_edgeperm_unpack(eperm, temp_arr);
two_cycle(temp_arr, B_EDGE_UR, B_EDGE_DR);
return b_edgeperm_pack(temp_arr);
}



   int  b_twist_d_on_edgeperm(int eperm)
{
int                     temp_arr[8];

b_edgeperm_unpack(eperm, temp_arr);
four_cycle(temp_arr, B_EDGE_DL, B_EDGE_DB, B_EDGE_DR, B_EDGE_DF);
return b_edgeperm_pack(temp_arr);
}



   void  b_twist_on_edgeperm_init(unsigned short t_on_ep_table[B_N_TWIST][B_N_EDGEPERM], unsigned short *t_on_ep_ptrs[B_N_TWIST])

{
int                     twist, eperm;


for (twist = 0; twist < B_N_TWIST; twist++)
    t_on_ep_ptrs[twist] = t_on_ep_table[twist];

for (eperm = 0; eperm < B_N_EDGEPERM; eperm++)
    {
    t_on_ep_ptrs[B_TWIST_F2][eperm] =
                               (unsigned short)b_twist_f2_on_edgeperm(eperm);
    t_on_ep_ptrs[B_TWIST_L2][eperm] =
                               (unsigned short)b_twist_l2_on_edgeperm(eperm);
    t_on_ep_ptrs[B_TWIST_U ][eperm] =
                               (unsigned short)b_twist_u_on_edgeperm(eperm);
    t_on_ep_ptrs[B_TWIST_B2][eperm] =
                               (unsigned short)b_twist_b2_on_edgeperm(eperm);
    t_on_ep_ptrs[B_TWIST_R2][eperm] =
                               (unsigned short)b_twist_r2_on_edgeperm(eperm);
    t_on_ep_ptrs[B_TWIST_D ][eperm] =
                               (unsigned short)b_twist_d_on_edgeperm(eperm);
    }
for (eperm = 0; eperm < B_N_EDGEPERM; eperm++)
    {
    t_on_ep_ptrs[B_TWIST_U2][eperm] =
                 t_on_ep_ptrs[B_TWIST_U][(int)t_on_ep_ptrs[B_TWIST_U][eperm]];
    t_on_ep_ptrs[B_TWIST_D2][eperm] =
                 t_on_ep_ptrs[B_TWIST_D][(int)t_on_ep_ptrs[B_TWIST_D][eperm]];
    }
for (eperm = 0; eperm < B_N_EDGEPERM; eperm++)
    {
    t_on_ep_ptrs[B_TWIST_U3][eperm] =
                 t_on_ep_ptrs[B_TWIST_U2][(int)t_on_ep_ptrs[B_TWIST_U][eperm]];
    t_on_ep_ptrs[B_TWIST_D3][eperm] =
                 t_on_ep_ptrs[B_TWIST_D2][(int)t_on_ep_ptrs[B_TWIST_D][eperm]];
    }

return;
}



   void  b_midperm_unpack(int mperm, int array_out[4])
{
perm_n_unpack(4, mperm, array_out);
return;
}



   int  b_midperm_pack(int array_in[4])
{
return perm_n_pack(4, array_in);
}



   int  b_twist_f2_on_midperm(int mperm)
{
int                     temp_arr[4];

b_midperm_unpack(mperm, temp_arr);
two_cycle(temp_arr, B_MID_FR, B_MID_FL);
return b_midperm_pack(temp_arr);
}



   int  b_twist_l2_on_midperm(int mperm)
{
int                     temp_arr[4];

b_midperm_unpack(mperm, temp_arr);
two_cycle(temp_arr, B_MID_FL, B_MID_BL);
return b_midperm_pack(temp_arr);
}



   int  b_twist_u_on_midperm(int mperm)
{
return mperm;  /*  !  */
}



   int  b_twist_b2_on_midperm(int mperm)
{
int                     temp_arr[4];

b_midperm_unpack(mperm, temp_arr);
two_cycle(temp_arr, B_MID_BR, B_MID_BL);
return b_midperm_pack(temp_arr);
}



   int  b_twist_r2_on_midperm(int mperm)
{
int                     temp_arr[4];

b_midperm_unpack(mperm, temp_arr);
two_cycle(temp_arr, B_MID_FR, B_MID_BR);
return b_midperm_pack(temp_arr);
}



   int  b_twist_d_on_midperm(int mperm)
{
return mperm;  /*  !  */
}



   void  b_twist_on_midperm_init(int t_on_mp_table[B_N_TWIST][B_N_MIDPERM], int *t_on_mp_ptrs[B_N_TWIST])
{
int                     twist, mperm;


for (twist = 0; twist < B_N_TWIST; twist++)
    t_on_mp_ptrs[twist] = t_on_mp_table[twist];

for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
    {
    t_on_mp_ptrs[B_TWIST_F2][mperm] = b_twist_f2_on_midperm(mperm);
    t_on_mp_ptrs[B_TWIST_L2][mperm] = b_twist_l2_on_midperm(mperm);
    t_on_mp_ptrs[B_TWIST_U ][mperm] = b_twist_u_on_midperm(mperm);
    t_on_mp_ptrs[B_TWIST_B2][mperm] = b_twist_b2_on_midperm(mperm);
    t_on_mp_ptrs[B_TWIST_R2][mperm] = b_twist_r2_on_midperm(mperm);
    t_on_mp_ptrs[B_TWIST_D ][mperm] = b_twist_d_on_midperm(mperm);
    }
for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
    {
    t_on_mp_ptrs[B_TWIST_U2][mperm] =
                  t_on_mp_ptrs[B_TWIST_U][t_on_mp_ptrs[B_TWIST_U][mperm]];
    t_on_mp_ptrs[B_TWIST_D2][mperm] =
                  t_on_mp_ptrs[B_TWIST_D][t_on_mp_ptrs[B_TWIST_D][mperm]];
    }
for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
    {
    t_on_mp_ptrs[B_TWIST_U3][mperm] =
                  t_on_mp_ptrs[B_TWIST_U2][t_on_mp_ptrs[B_TWIST_U][mperm]];
    t_on_mp_ptrs[B_TWIST_D3][mperm] =
                  t_on_mp_ptrs[B_TWIST_D2][t_on_mp_ptrs[B_TWIST_D][mperm]];
    }

return;
}



   void  sliceedge_unpack(int sliceedge, int array_out[12])

{
int                     temp_arr[4], ii, count;


a_eloc_unpack(sliceedge % A_N_ELOC, array_out);
perm_n_unpack(4, sliceedge / A_N_ELOC, temp_arr);

count = 0;
for (ii = 0; ii < 12; ii++)
    if (array_out[ii] != 0)
       array_out[ii] = 1 + temp_arr[count++];

return;
}



   int  sliceedge_pack(int array_in[12])
{
int                     temp_arr[4], eloc_arr[12], ii, count;


count = 0;
for (ii = 0; ii < 12; ii++)
    {
    if (array_in[ii] != 0)
       temp_arr[count++] = array_in[ii] - 1;

    eloc_arr[ii] = (array_in[ii] != 0);
    }

return perm_n_pack(4, temp_arr) * A_N_ELOC + a_eloc_pack(eloc_arr);
}



   int  twist_f_on_sliceedge(int sliceedge)
{
int                     temp_arr[12];

sliceedge_unpack(sliceedge, temp_arr);
four_cycle(temp_arr, A_EDGE_FL, A_EDGE_DF, A_EDGE_FR, A_EDGE_UF);
return sliceedge_pack(temp_arr);
}



   int  twist_l_on_sliceedge(int sliceedge)
{
int                     temp_arr[12];

sliceedge_unpack(sliceedge, temp_arr);
four_cycle(temp_arr, A_EDGE_BL, A_EDGE_DL, A_EDGE_FL, A_EDGE_UL);
return sliceedge_pack(temp_arr);
}



   int  twist_u_on_sliceedge(int sliceedge)
{
int                     temp_arr[12];

sliceedge_unpack(sliceedge, temp_arr);
four_cycle(temp_arr, A_EDGE_UR, A_EDGE_UB, A_EDGE_UL, A_EDGE_UF);
return sliceedge_pack(temp_arr);
}



   int  twist_b_on_sliceedge(int sliceedge)
{
int                     temp_arr[12];

sliceedge_unpack(sliceedge, temp_arr);
four_cycle(temp_arr, A_EDGE_BR, A_EDGE_DB, A_EDGE_BL, A_EDGE_UB);
return sliceedge_pack(temp_arr);
}



   int  twist_r_on_sliceedge(int sliceedge)
{
int                     temp_arr[12];

sliceedge_unpack(sliceedge, temp_arr);
four_cycle(temp_arr, A_EDGE_FR, A_EDGE_DR, A_EDGE_BR, A_EDGE_UR);
return sliceedge_pack(temp_arr);
}



   int  twist_d_on_sliceedge(int sliceedge)
{
int                     temp_arr[12];

sliceedge_unpack(sliceedge, temp_arr);
four_cycle(temp_arr, A_EDGE_DL, A_EDGE_DB, A_EDGE_DR, A_EDGE_DF);
return sliceedge_pack(temp_arr);
}



   void  twist_on_sliceedge_init(unsigned short t_on_sl_table[A_N_TWIST][N_SLICEEDGE],
   unsigned short *t_on_sl_ptrs[A_N_TWIST])
{
int                     twist, sliceedge;


for (twist = 0; twist < A_N_TWIST; twist++)
    t_on_sl_ptrs[twist] = t_on_sl_table[twist];

for (sliceedge = 0; sliceedge < N_SLICEEDGE; sliceedge++)
    {
    t_on_sl_ptrs[A_TWIST_F][sliceedge] =
                             (unsigned short)twist_f_on_sliceedge(sliceedge);
    t_on_sl_ptrs[A_TWIST_L][sliceedge] =
                             (unsigned short)twist_l_on_sliceedge(sliceedge);
    t_on_sl_ptrs[A_TWIST_U][sliceedge] =
                             (unsigned short)twist_u_on_sliceedge(sliceedge);
    t_on_sl_ptrs[A_TWIST_B][sliceedge] =
                             (unsigned short)twist_b_on_sliceedge(sliceedge);
    t_on_sl_ptrs[A_TWIST_R][sliceedge] =
                             (unsigned short)twist_r_on_sliceedge(sliceedge);
    t_on_sl_ptrs[A_TWIST_D][sliceedge] =
                             (unsigned short)twist_d_on_sliceedge(sliceedge);
    }
for (sliceedge = 0; sliceedge < N_SLICEEDGE; sliceedge++)
    {
    t_on_sl_ptrs[A_TWIST_F2][sliceedge] =
            t_on_sl_ptrs[A_TWIST_F][(int)t_on_sl_ptrs[A_TWIST_F][sliceedge]];
    t_on_sl_ptrs[A_TWIST_L2][sliceedge] =
            t_on_sl_ptrs[A_TWIST_L][(int)t_on_sl_ptrs[A_TWIST_L][sliceedge]];
    t_on_sl_ptrs[A_TWIST_U2][sliceedge] =
            t_on_sl_ptrs[A_TWIST_U][(int)t_on_sl_ptrs[A_TWIST_U][sliceedge]];
    t_on_sl_ptrs[A_TWIST_B2][sliceedge] =
            t_on_sl_ptrs[A_TWIST_B][(int)t_on_sl_ptrs[A_TWIST_B][sliceedge]];
    t_on_sl_ptrs[A_TWIST_R2][sliceedge] =
            t_on_sl_ptrs[A_TWIST_R][(int)t_on_sl_ptrs[A_TWIST_R][sliceedge]];
    t_on_sl_ptrs[A_TWIST_D2][sliceedge] =
            t_on_sl_ptrs[A_TWIST_D][(int)t_on_sl_ptrs[A_TWIST_D][sliceedge]];
    }
for (sliceedge = 0; sliceedge < N_SLICEEDGE; sliceedge++)
    {
    t_on_sl_ptrs[A_TWIST_F3][sliceedge] =
            t_on_sl_ptrs[A_TWIST_F2][(int)t_on_sl_ptrs[A_TWIST_F][sliceedge]];
    t_on_sl_ptrs[A_TWIST_L3][sliceedge] =
            t_on_sl_ptrs[A_TWIST_L2][(int)t_on_sl_ptrs[A_TWIST_L][sliceedge]];
    t_on_sl_ptrs[A_TWIST_U3][sliceedge] =
            t_on_sl_ptrs[A_TWIST_U2][(int)t_on_sl_ptrs[A_TWIST_U][sliceedge]];
    t_on_sl_ptrs[A_TWIST_B3][sliceedge] =
            t_on_sl_ptrs[A_TWIST_B2][(int)t_on_sl_ptrs[A_TWIST_B][sliceedge]];
    t_on_sl_ptrs[A_TWIST_R3][sliceedge] =
            t_on_sl_ptrs[A_TWIST_R2][(int)t_on_sl_ptrs[A_TWIST_R][sliceedge]];
    t_on_sl_ptrs[A_TWIST_D3][sliceedge] =
            t_on_sl_ptrs[A_TWIST_D2][(int)t_on_sl_ptrs[A_TWIST_D][sliceedge]];
    }

return;
}



   void  halfedgeloc_conv_init(int conv_tab[N_HALFEDGELOC], int unconv_tab[N_HALFEDGELOC_CONV])
{
int                     ii, loc0, loc1, loc2, loc3, count;


for (ii = 0; ii < N_HALFEDGELOC; ii++)
    conv_tab[ii] = 0;

for (ii = 0; ii < N_HALFEDGELOC_CONV; ii++)
    unconv_tab[ii] = 0;

count = 0;
for (loc0 = 0; loc0 < 5; loc0++)
    for (loc1 = loc0 + 1; loc1 < 6; loc1++)
        for (loc2 = loc1 + 1; loc2 < 7; loc2++)
            for (loc3 = loc2 + 1; loc3 < 8; loc3++)
                {
                if (count >= N_HALFEDGELOC)
                   exit_w_error_message(
                           "halfedgeloc_conv_init : too many halfedgeloc's");
                conv_tab[count] = (1 << loc0) | (1 << loc1) |
                                  (1 << loc2) | (1 << loc3);
                unconv_tab[conv_tab[count]] = count;
                count++;
                }
return;
}



   void  halfedgeloc_unpack(int halfedgeloc, int array_out[8])
{
int                     conv, ii;


conv = halfedgeloc_conv[halfedgeloc];

for (ii = 0; ii < 8; ii++)
    {
    array_out[ii] = conv % 2;
    conv = conv / 2;
    }

return;
}



   int  halfedgeloc_pack(int array_in[8])
{
int                     ii, conv;


conv = 0;
for (ii = 7; ii >= 0; ii--)
    conv = 2 * conv + array_in[ii];

return halfedgeloc_unconv[conv];
}



   void  halfedgeperm_unpack(int halfedgeperm, int array_out[8])
{
int                     temp_arr[4], ii, count;


halfedgeloc_unpack(halfedgeperm % N_HALFEDGELOC, array_out);
perm_n_unpack(4, halfedgeperm / N_HALFEDGELOC, temp_arr);

count = 0;
for (ii = 0; ii < 8; ii++)
    if (array_out[ii] != 0)
       array_out[ii] = 1 + temp_arr[count++];

return;
}



   int  halfedgeperm_pack(int array_in[8])
{
int                     temp_arr[4], halfedgeloc_arr[8], ii, count;


count = 0;
for (ii = 0; ii < 8; ii++)
    {
    if (array_in[ii] != 0)
       temp_arr[count++] = array_in[ii] - 1;

    halfedgeloc_arr[ii] = (array_in[ii] != 0);
    }

return perm_n_pack(4, temp_arr) * N_HALFEDGELOC +
                                          halfedgeloc_pack(halfedgeloc_arr);
}



   int  unpacked_sliceedge_to_halfedge(int sl_array_in[12], int hep_array_out[8])
{
if (sl_array_in[A_EDGE_FR] || sl_array_in[A_EDGE_FL] ||
    sl_array_in[A_EDGE_BR] || sl_array_in[A_EDGE_BL])
   return 1;

hep_array_out[B_EDGE_UF] = sl_array_in[A_EDGE_UF];
hep_array_out[B_EDGE_UR] = sl_array_in[A_EDGE_UR];
hep_array_out[B_EDGE_UB] = sl_array_in[A_EDGE_UB];
hep_array_out[B_EDGE_UL] = sl_array_in[A_EDGE_UL];
hep_array_out[B_EDGE_DF] = sl_array_in[A_EDGE_DF];
hep_array_out[B_EDGE_DR] = sl_array_in[A_EDGE_DR];
hep_array_out[B_EDGE_DB] = sl_array_in[A_EDGE_DB];
hep_array_out[B_EDGE_DL] = sl_array_in[A_EDGE_DL];

return 0;
}



   void  sliceedge_to_halfedgeperm_init(int sl_to_h_array[N_SLICEEDGE])

{
int                     sl_arr[12], hep_arr[8], sliceedge;


halfedgeloc_conv_init(halfedgeloc_conv, halfedgeloc_unconv);

for (sliceedge = 0; sliceedge < N_SLICEEDGE; sliceedge++)
    {
    sliceedge_unpack(sliceedge, sl_arr);
    if (unpacked_sliceedge_to_halfedge(sl_arr, hep_arr))
       sl_to_h_array[sliceedge] = EDGE_INVALID;
    else
       sl_to_h_array[sliceedge] = halfedgeperm_pack(hep_arr);
    }

return;
}



   int  combine_halfedgeperm(int hep_array0_in[8], int hep_array1_in[8], int eperm_array_out[8])
{
int                     trf_arr0[5], trf_arr1[5], ii;

for (ii = 0; ii < 8; ii++)
    if (hep_array0_in[ii] && hep_array1_in[ii])
       return 1;

trf_arr0[1] = B_EDGE_UF;
trf_arr0[2] = B_EDGE_UB;
trf_arr0[3] = B_EDGE_DF;
trf_arr0[4] = B_EDGE_DB;

trf_arr1[1] = B_EDGE_UR;
trf_arr1[2] = B_EDGE_UL;
trf_arr1[3] = B_EDGE_DR;
trf_arr1[4] = B_EDGE_DL;

for (ii = 0; ii < 8; ii++)
    {
    if (hep_array0_in[ii])
       eperm_array_out[ii] = trf_arr0[hep_array0_in[ii]];
    else if (hep_array1_in[ii])
            eperm_array_out[ii] = trf_arr1[hep_array1_in[ii]];
    else
       exit_w_error_message("combine_halfedgeperm : invalid input");
    }

return 0;
}



   void  half_to_edgeperm_init(unsigned short h_to_ep_table[N_HALFEDGEPERM][N_HALFEDGEPERM],
   unsigned short         *h_to_ep_ptrs[N_HALFEDGEPERM])
/* ------------------------------------------------------------------------- */



{
int                     unpacked_hep[N_HALFEDGEPERM][8], eperm_arr[8];
int                     halfedge0, halfedge1;


for (halfedge0 = 0; halfedge0 < N_HALFEDGEPERM; halfedge0++)
    h_to_ep_ptrs[halfedge0] = h_to_ep_table[halfedge0];

for (halfedge0 = 0; halfedge0 < N_HALFEDGEPERM; halfedge0++)
    halfedgeperm_unpack(halfedge0, unpacked_hep[halfedge0]);

for (halfedge0 = 0; halfedge0 < N_HALFEDGEPERM; halfedge0++)
    for (halfedge1 = 0; halfedge1 < N_HALFEDGEPERM; halfedge1++)
        {
        if (combine_halfedgeperm(unpacked_hep[halfedge0],
                                 unpacked_hep[halfedge1], eperm_arr))
           h_to_ep_ptrs[halfedge0][halfedge1] = (unsigned short)EDGE_INVALID;
        else
           h_to_ep_ptrs[halfedge0][halfedge1] =
                                    (unsigned short)b_edgeperm_pack(eperm_arr);
        }

return;
}



   int  unpacked_sliceedge_to_midperm(
   int                     sl_array_in[12], int mperm_array_out[4])
/* ------------------------------------------------------------------------- */


{
if (sl_array_in[A_EDGE_UF] || sl_array_in[A_EDGE_UR] ||
    sl_array_in[A_EDGE_UB] || sl_array_in[A_EDGE_UL] ||
    sl_array_in[A_EDGE_DF] || sl_array_in[A_EDGE_DR] ||
    sl_array_in[A_EDGE_DB] || sl_array_in[A_EDGE_DL])
   return 1;

mperm_array_out[B_MID_FR] = sl_array_in[A_EDGE_FR] - 1;
mperm_array_out[B_MID_FL] = sl_array_in[A_EDGE_FL] - 1;
mperm_array_out[B_MID_BR] = sl_array_in[A_EDGE_BR] - 1;
mperm_array_out[B_MID_BL] = sl_array_in[A_EDGE_BL] - 1;

return 0;
}


   void  sliceedge_to_midperm_init(int sl_to_mp_array[N_SLICEEDGE])

{
int                     sliceedge, sl_arr[12], mperm_arr[4];


for (sliceedge = 0; sliceedge < N_SLICEEDGE; sliceedge++)
    {
    sliceedge_unpack(sliceedge, sl_arr);
    if (unpacked_sliceedge_to_midperm(sl_arr, mperm_arr))
       sl_to_mp_array[sliceedge] = EDGE_INVALID;
    else
       sl_to_mp_array[sliceedge] = b_midperm_pack(mperm_arr);
    }

return;
}



   void  edge_transform_tables_init()
/* ------------------------------------------------------------------------- */

{
twist_on_sliceedge_init(twist_on_sliceedge_table, twist_on_sliceedge_ptrs);
sliceedge_to_halfedgeperm_init(sliceedge_to_halfedgeperm);
half_to_edgeperm_init(half_to_edgeperm_table, half_to_edgeperm_ptrs);
sliceedge_to_midperm_init(sliceedge_to_midperm);
return;
}




   void  transformation_tables_init()
/* ------------------------------------------------------------------------- */

{
a_twist_on_corner_init(twist_on_corner_table, twist_on_corner_ptrs);
a_twist_on_eflip_init(twist_on_eflip_table, twist_on_eflip_ptrs);
a_twist_on_eloc_init(twist_on_eloc_table, twist_on_eloc_ptrs);

b_twist_on_cornerperm_init(twist_on_cornerperm_table,
                      a_twist_on_cornerperm_ptrs, b_twist_on_cornerperm_ptrs);
b_twist_on_edgeperm_init(twist_on_edgeperm_table, twist_on_edgeperm_ptrs);
b_twist_on_midperm_init(twist_on_midperm_table, twist_on_midperm_ptrs);

edge_transform_tables_init();

return;
}



   void  a_corner_eflip_dist_init(unsigned char  cf_dist_table[A_N_CORNER][A_N_EFLIP],
   unsigned char          *cf_dist_ptrs[A_N_CORNER])


{
int                     corner, eflip, new_corner, new_eflip, n_found;
int                     depth, twist, max_len;


for (corner = 0; corner < A_N_CORNER; corner++)
    cf_dist_ptrs[corner] = cf_dist_table[corner];

for (corner = 0; corner < A_N_CORNER; corner++)
    for (eflip = 0; eflip < A_N_EFLIP; eflip++)
        cf_dist_ptrs[corner][eflip] = (unsigned char)BIG;

max_len = 0;
for (twist = 0; twist < A_N_TWIST; twist++)
    if (max_len < a_metric_length[twist])
       max_len = a_metric_length[twist];

//printf("corner-eflip space..\n");

n_found = 0;
depth = 0;
cf_dist_ptrs[A_CORNER_START][A_EFLIP_START] = depth;
n_found++;

while (n_found)
      {
      //printf("%2d %8d\n", depth, n_found);
      depth++;
      n_found = 0;

      for (corner = 0; corner < A_N_CORNER; corner++)
          for (eflip = 0; eflip < A_N_EFLIP; eflip++)
              {
              if (((int)cf_dist_ptrs[corner][eflip] >= depth) ||
                  ((int)cf_dist_ptrs[corner][eflip] < depth - max_len))
                 continue;

              for (twist = 0; twist < A_N_TWIST; twist++)
                  {
                  if ((int)cf_dist_ptrs[corner][eflip] + a_metric_length[twist]
                                                                      != depth)
                     continue;

                  new_corner = twist_on_corner_ptrs[twist][corner];
                  new_eflip = twist_on_eflip_ptrs[twist][eflip];

                  if ((int)cf_dist_ptrs[new_corner][new_eflip] > depth)
                     {
                     cf_dist_ptrs[new_corner][new_eflip] = (unsigned char)depth;
                     n_found++;
                     }
                  }
              }
      }

return;
}




   void  a_corner_eloc_dist_init(unsigned char cl_dist_table[A_N_CORNER][A_N_ELOC],
   unsigned char *cl_dist_ptrs[A_N_CORNER])
{
int                     corner, eloc, new_corner, new_eloc, n_found;
int                     depth, twist, max_len;


for (corner = 0; corner < A_N_CORNER; corner++)
    cl_dist_ptrs[corner] = cl_dist_table[corner];

for (corner = 0; corner < A_N_CORNER; corner++)
    for (eloc = 0; eloc < A_N_ELOC; eloc++)
        cl_dist_ptrs[corner][eloc] = (unsigned char)BIG;

max_len = 0;
for (twist = 0; twist < A_N_TWIST; twist++)
    if (max_len < a_metric_length[twist])
       max_len = a_metric_length[twist];

//printf("\ncorner-eloc space\n");

n_found = 0;
depth = 0;
cl_dist_ptrs[A_CORNER_START][A_ELOC_START] = depth;
n_found++;

while (n_found)
      {
      //printf("%2d %8d\n", depth, n_found);
      depth++;
      n_found = 0;

      for (corner = 0; corner < A_N_CORNER; corner++)
          for (eloc = 0; eloc < A_N_ELOC; eloc++)
              {
              if (((int)cl_dist_ptrs[corner][eloc] >= depth) ||
                  ((int)cl_dist_ptrs[corner][eloc] < depth - max_len))
                 continue;

              for (twist = 0; twist < A_N_TWIST; twist++)
                  {
                  if ((int)cl_dist_ptrs[corner][eloc] + a_metric_length[twist]
                                                                      != depth)
                     continue;

                  new_corner = twist_on_corner_ptrs[twist][corner];
                  new_eloc = twist_on_eloc_ptrs[twist][eloc];

                  if ((int)cl_dist_ptrs[new_corner][new_eloc] > depth)
                     {
                     cl_dist_ptrs[new_corner][new_eloc] = (unsigned char)depth;
                     n_found++;
                     }
                  }
              }
      }

return;
}



   void  a_eflip_eloc_dist_init(unsigned char fl_dist_table[A_N_EFLIP][A_N_ELOC],
   unsigned char *fl_dist_ptrs[A_N_EFLIP])
{
int                     eflip, eloc, new_eflip, new_eloc, n_found;
int                     depth, twist, max_len;


for (eflip = 0; eflip < A_N_EFLIP; eflip++)
    fl_dist_ptrs[eflip] = fl_dist_table[eflip];

for (eflip = 0; eflip < A_N_EFLIP; eflip++)
    for (eloc = 0; eloc < A_N_ELOC; eloc++)
        fl_dist_ptrs[eflip][eloc] = (unsigned char)BIG;

max_len = 0;
for (twist = 0; twist < A_N_TWIST; twist++)
    if (max_len < a_metric_length[twist])
       max_len = a_metric_length[twist];

//printf("\neflip-eloc space\n");

n_found = 0;
depth = 0;
fl_dist_ptrs[A_EFLIP_START][A_ELOC_START] = depth;
n_found++;

while (n_found)
      {
      //printf("%2d %8d\n", depth, n_found);
      depth++;
      n_found = 0;

      for (eflip = 0; eflip < A_N_EFLIP; eflip++)
          for (eloc = 0; eloc < A_N_ELOC; eloc++)
              {
              if (((int)fl_dist_ptrs[eflip][eloc] >= depth) ||
                  ((int)fl_dist_ptrs[eflip][eloc] < depth - max_len))
                 continue;

              for (twist = 0; twist < A_N_TWIST; twist++)
                  {
                  if ((int)fl_dist_ptrs[eflip][eloc] + a_metric_length[twist]
                                                                      != depth)
                     continue;

                  new_eflip = twist_on_eflip_ptrs[twist][eflip];
                  new_eloc = twist_on_eloc_ptrs[twist][eloc];

                  if ((int)fl_dist_ptrs[new_eflip][new_eloc] > depth)
                     {
                     fl_dist_ptrs[new_eflip][new_eloc] = (unsigned char)depth;
                     n_found++;
                     }
                  }
              }
      }

return;
}



   void  b_cornerperm_dist_init(int cp_dist[B_N_CORNERPERM])
{
int                     cperm, new_cperm, n_found, depth, twist, max_len;


for (cperm = 0; cperm < B_N_CORNERPERM; cperm++)
    cp_dist[cperm] = BIG;

max_len = 0;
for (twist = 0; twist < B_N_TWIST; twist++)
    if (max_len < b_metric_length[twist])
       max_len = b_metric_length[twist];

//printf("\ncornerperm space\n");

n_found = 0;
depth = 0;
cp_dist[B_CORNERPERM_START] = depth;
n_found++;

while (n_found)
      {
      //printf("%2d %8d\n", depth, n_found);
      depth++;
      n_found = 0;

      for (cperm = 0; cperm < B_N_CORNERPERM; cperm++)
          {
          if ((cp_dist[cperm] >= depth) || (cp_dist[cperm] < depth - max_len))
             continue;

          for (twist = 0; twist < B_N_TWIST; twist++)
              {
              if (cp_dist[cperm] + b_metric_length[twist] != depth)
                 continue;

              new_cperm = (int)b_twist_on_cornerperm_ptrs[twist][cperm];

              if (cp_dist[new_cperm] > depth)
                 {
                 cp_dist[new_cperm] = depth;
                 n_found++;
                 }
              }
          }
      }

return;
}



   void  b_mid_cornerperm_dist_init(unsigned char           mc_dist_table[B_N_MIDPERM][B_N_CORNERPERM],
   unsigned char          *mc_dist_ptrs[B_N_MIDPERM])
{
int                     mperm, cperm, new_mperm, new_cperm, n_found;
int                     depth, twist, max_len;


for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
    mc_dist_ptrs[mperm] = mc_dist_table[mperm];

for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
    for (cperm = 0; cperm < B_N_CORNERPERM; cperm++)
        mc_dist_ptrs[mperm][cperm] = (unsigned char)BIG;

max_len = 0;
for (twist = 0; twist < B_N_TWIST; twist++)
    if (max_len < b_metric_length[twist])
       max_len = b_metric_length[twist];

//printf("\nmidperm-cornerperm space\n");

n_found = 0;
depth = 0;
mc_dist_ptrs[B_MIDPERM_START][B_CORNERPERM_START] = depth;
n_found++;

while (n_found)
      {
      //printf("%2d %8d\n", depth, n_found);
      depth++;
      n_found = 0;

      for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
          for (cperm = 0; cperm < B_N_CORNERPERM; cperm++)
              {
              if (((int)mc_dist_ptrs[mperm][cperm] >= depth) ||
                  ((int)mc_dist_ptrs[mperm][cperm] < depth - max_len))
                 continue;

              for (twist = 0; twist < B_N_TWIST; twist++)
                  {
                  if ((int)mc_dist_ptrs[mperm][cperm] + b_metric_length[twist]
                                                                      != depth)
                     continue;

                  new_mperm = twist_on_midperm_ptrs[twist][mperm];
                  new_cperm = (int)b_twist_on_cornerperm_ptrs[twist][cperm];

                  if ((int)mc_dist_ptrs[new_mperm][new_cperm] > depth)
                     {
                     mc_dist_ptrs[new_mperm][new_cperm] = (unsigned char)depth;
                     n_found++;
                     }
                  }
              }
      }

return;
}



   void  b_mid_edgeperm_dist_init(unsigned char me_dist_table[B_N_MIDPERM][B_N_EDGEPERM],
   unsigned char *me_dist_ptrs[B_N_MIDPERM])
{
int                     mperm, eperm, new_mperm, new_eperm, n_found;
int                     depth, twist, max_len;


for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
    me_dist_ptrs[mperm] = me_dist_table[mperm];

for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
    for (eperm = 0; eperm < B_N_EDGEPERM; eperm++)
        me_dist_ptrs[mperm][eperm] = (unsigned char)BIG;

max_len = 0;
for (twist = 0; twist < B_N_TWIST; twist++)
    if (max_len < b_metric_length[twist])
       max_len = b_metric_length[twist];

//printf("\nmidperm-edgeperm space\n");

n_found = 0;
depth = 0;
me_dist_ptrs[B_MIDPERM_START][B_EDGEPERM_START] = depth;
n_found++;

while (n_found)
      {
      //printf("%2d %8d\n", depth, n_found);
      depth++;
      n_found = 0;

      for (mperm = 0; mperm < B_N_MIDPERM; mperm++)
          for (eperm = 0; eperm < B_N_EDGEPERM; eperm++)
              {
              if (((int)me_dist_ptrs[mperm][eperm] >= depth) ||
                  ((int)me_dist_ptrs[mperm][eperm] < depth - max_len))
                 continue;

              for (twist = 0; twist < B_N_TWIST; twist++)
                  {
                  if ((int)me_dist_ptrs[mperm][eperm] + b_metric_length[twist]
                                                                      != depth)
                     continue;

                  new_mperm = twist_on_midperm_ptrs[twist][mperm];
                  new_eperm = (int)twist_on_edgeperm_ptrs[twist][eperm];

                  if ((int)me_dist_ptrs[new_mperm][new_eperm] > depth)
                     {
                     me_dist_ptrs[new_mperm][new_eperm] = (unsigned char)depth;
                     n_found++;
                     }
                  }
              }
      }

return;
}



   void  prune_tables_init()
/* ------------------------------------------------------------------------- */

{
    if(tables_done)
        return;
    tables_done = true;
    printf("Generating solving tables ");
a_corner_eflip_dist_init(corner_eflip_dist_table, corner_eflip_dist_ptrs);
printf(".");
a_corner_eloc_dist_init(corner_eloc_dist_table, corner_eloc_dist_ptrs);
printf(".");
a_eflip_eloc_dist_init(eflip_eloc_dist_table, eflip_eloc_dist_ptrs);
printf(".");
b_cornerperm_dist_init(cornerperm_dist_table);
printf(".");
b_mid_cornerperm_dist_init(mid_cornerperm_dist_table, mid_cornerperm_dist_ptrs);
printf(".");
b_mid_edgeperm_dist_init(mid_edgeperm_dist_table, mid_edgeperm_dist_ptrs);
printf(" done!\n");
return;
}



   int  user_enters_cube(Cube *p_cube, char* state)
{
char                    e_str[12][4], c_str[8][5];
int                     num, ii, jj, stat;

num = sscanf(state, "%3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %4s %4s %4s %4s %4s %4s %4s %4s",
            e_str[0], e_str[1], e_str[2], e_str[3], e_str[4], e_str[5],
            e_str[6], e_str[7], e_str[8], e_str[9], e_str[10], e_str[11],
            c_str[0], c_str[1], c_str[2], c_str[3],
            c_str[4], c_str[5], c_str[6], c_str[7]);

if (num == EOF)
   return -1;

if (num < 20)
   {
   printf("not enough cubies entered\n");
   return 1;
   }

stat = 0;

for (ii = 0; ii < 12; ii++)
    {
    for (jj = 0; jj < 24; jj++)
        {
        if (strcmp(e_str[ii], edge_cubie_string[jj]) == 0)
           {
           p_cube->edges[ii] = jj;
           break;
           }
        }
    if (jj == 24)
       {
       p_cube->edges[ii] = -1;
       printf("improper edge cubie:  %s\n", e_str[ii]);
       stat = 1;
       }
    }

for (ii = 0; ii < 8; ii++)
    {
    for (jj = 0; jj < 24; jj++)
        {
        if (strcmp(c_str[ii], corner_cubie_string[jj]) == 0)
           {
           p_cube->corners[ii] = jj;
           break;
           }
        }
    if (jj == 24)
       {
       p_cube->corners[ii] = -1;
       printf("improper corner cubie:  %s\n", c_str[ii]);
       stat = 1;
       }
    }
return stat;
}



   int  check_cube(Cube *p_cube)
{
int                     l_edge[12], l_corner[8];
int                     ii, twist, flip, edge_par, corner_par, status;

/*  fill out the remaining oriented edges and corners  */

for (ii = 12; ii < 24; ii++)
    p_cube->edges[ii] = (12 + p_cube->edges[ii - 12]) % 24;

for (ii = 8; ii < 24; ii++)
    p_cube->corners[ii] = (8 + p_cube->corners[ii - 8]) % 24;

/*  now check to see that it's a valid cube  */

if (check_perm_n(24, p_cube->edges) || check_perm_n(24, p_cube->corners))
   {
   printf("bad cubies\n");
   return 1;
   }

status = 0;

flip = 0;
for (ii = 0; ii < 12; ii++)
    flip += (p_cube->edges[ii] / 12);

if ((flip % 2) != 0)
   {
   printf("flip any edge cubie!\n");
   status = 1;
   }

twist = 0;
for (ii = 0; ii < 8; ii++)
    twist += (p_cube->corners[ii] / 8);

if ((twist % 3) != 0)
   {
   printf("twist any corner cubie %sclockwise!\n",
           (twist == 1) ? "counter" : "");
   status = 1;
   }

for (ii = 0; ii < 12; ii++)
    l_edge[ii] = p_cube->edges[ii] % 12;

edge_par = parity_of_perm_n(12, l_edge);

for (ii = 0; ii < 8; ii++)
    l_corner[ii] = p_cube->corners[ii] % 8;

p_cube->parity = corner_par = parity_of_perm_n(8, l_corner);

if (edge_par != corner_par)
   {
   printf("swap any two edge cubies or any two corner cubies!\n");
   status = 1;
   }

return status;
}





   void  process_cube(Cube *p_cube)
{
int                     corner_arr[8], edge_arr[12];
int                     trf_ud[12], trf_rl[12], trf_fb[12], ii;


for (ii = 0; ii < 8; ii++)
    corner_arr[ii] = p_cube->corners[ii] / 8;

p_cube->a_corner = a_corner_pack(corner_arr);

for (ii = 0; ii < 12; ii++)
    edge_arr[ii] = p_cube->edges[ii] / 12;

p_cube->a_edgeflip = a_eflip_pack(edge_arr);

for (ii = 0; ii < 12; ii++)
    edge_arr[ii] = (p_cube->edges[ii] % 12) / 8;

p_cube->a_edgeloc = a_eloc_pack(edge_arr);

for (ii = 0; ii < 8; ii++)
    corner_arr[ii] = p_cube->corners[ii] % 8;

p_cube->cornerperm = cornerperm_pack(corner_arr);

for (ii = 0; ii < 12; ii++)
    trf_ud[ii] = trf_rl[ii] = trf_fb[ii] = 0;

trf_ud[A_EDGE_FR] = 1 + B_MID_FR;
trf_ud[A_EDGE_FL] = 1 + B_MID_FL;
trf_ud[A_EDGE_BR] = 1 + B_MID_BR;
trf_ud[A_EDGE_BL] = 1 + B_MID_BL;

trf_rl[A_EDGE_UF] = 1;
trf_rl[A_EDGE_UB] = 2;
trf_rl[A_EDGE_DF] = 3;
trf_rl[A_EDGE_DB] = 4;

trf_fb[A_EDGE_UR] = 1;
trf_fb[A_EDGE_UL] = 2;
trf_fb[A_EDGE_DR] = 3;
trf_fb[A_EDGE_DL] = 4;

for (ii = 0; ii < 12; ii++)
    edge_arr[ii] = trf_ud[p_cube->edges[ii] % 12];

p_cube->ud_sliceedge = sliceedge_pack(edge_arr);

for (ii = 0; ii < 12; ii++)
    edge_arr[ii] = trf_rl[p_cube->edges[ii] % 12];

p_cube->rl_sliceedge = sliceedge_pack(edge_arr);

for (ii = 0; ii < 12; ii++)
    edge_arr[ii] = trf_fb[p_cube->edges[ii] % 12];

p_cube->fb_sliceedge = sliceedge_pack(edge_arr);

return;
}



int  cube_init(Cube *p_cube, char* state)
{
    int stat;
    stat = user_enters_cube(p_cube, state);
    if (stat)
       return stat;

    stat = check_cube(p_cube);
    if (stat)
       return stat;

    process_cube(p_cube);
    return 0;
}



   void  output_solution(Solution* s, Stage1_node *node1_arr, Stage2_node *node2_arr)

{
int                     ii, q_length, f_length;

struct timeval m_tv;
gettimeofday(&m_tv, NULL);

double start_time = solve_start_time_secs + (solve_start_time_usecs/1000000.0);
double end_time = m_tv.tv_sec + (m_tv.tv_usec/1000000.0);

double time_elapsed = end_time - start_time;

shortest_solution = node1_arr->remain_depth + node2_arr->remain_depth;


s->solution = new int[shortest_solution];

for(int i = 0; i < shortest_solution; i++) {
    s->solution[i] = -1;
}

s->steps = shortest_solution;
s->solved = true;

stage2_allotment = node2_arr->remain_depth - increment;

q_length = f_length = 0;

int i = 0;
for (ii = 0; ii < BIG; ii++)
    {
    if (ii > 0)
       {
           s->solution[i] = node1_arr[ii].twist;
           i++;
       }
    if (node1_arr[ii].remain_depth == 0)
       break;
    }

//printf("  ");

for (ii = 0; ii < BIG; ii++)
    {
    if (ii > 0)
       {
            // stage2 moves have different move ids
            // so find the corresponding a_twist_move
           int move = node2_arr[ii].twist;
           for(int j = 0; j < 18; j++) {
               if(strcmp(b_twist_string[move], a_twist_string[j]) == 0) {
                   s->solution[i] = j;
                   break;
               }
           }
           i++;
       }
    if (node2_arr[ii].remain_depth == 0)
       break;
    }
    //printf("Solution found in %d moves\n", i);
    s->steps = i;
    
    if(i < shortest_sol) {
        printf("%d %fs\n", i, time_elapsed);

        best.steps = i;
        best.solution = s->solution;
        best.solved = true;
        shortest_sol = i;
    }

return;
}



Solution stage2_search(Stage1_node *node1_arr, Stage2_node *node2_arr)

{
register Stage2_node   *p_node;
register int            twist;


p_node = node2_arr;

Solution sol;

sol.solved = false;

while (p_node >= node2_arr)
      {
            struct timeval m_tv;
            gettimeofday(&m_tv, NULL);

            double start_time = solve_start_time_secs + (solve_start_time_usecs/1000000.0);
            double end_time = m_tv.tv_sec + (m_tv.tv_usec/1000000.0);

            double time_elapsed = end_time - start_time;

          if(time_elapsed >= MAX_TIME) {
              return sol;
          }
      if (p_node->remain_depth == 0)
         {
             Solution s;
             output_solution(&s, node1_arr, node2_arr);
             
             return s;
         }
      else
         {
         for (twist = p_node[1].twist + 1; twist < B_N_TWIST; twist++)
             {
             if (b_twist_list_ptrs[twist][p_node->tw_list] ==
                                                        TWIST_LIST_INVALID)
                continue;

             p_node[1].remain_depth = p_node->remain_depth
                                                    - b_metric_length[twist];
             p_node[1].mperm = twist_on_midperm_ptrs[twist][p_node->mperm];
             p_node[1].cperm = b_twist_on_cornerperm_ptrs[twist][p_node->cperm];

             if ((int)mid_cornerperm_dist_table[p_node[1].mperm]
                                               [p_node[1].cperm]
                                                    > p_node[1].remain_depth)
                continue;

             p_node[1].eperm = twist_on_edgeperm_ptrs[twist][p_node->eperm];

             if ((int)mid_edgeperm_dist_table[p_node[1].mperm][p_node[1].eperm]
                                                    > p_node[1].remain_depth)
                continue;

             p_node[1].twist = twist;
             p_node[1].tw_list = b_twist_list_ptrs[twist][p_node->tw_list];

             break;
             }

         if (twist == B_N_TWIST)
            p_node--;
         else
            {
            p_node++;
            p_node[1].twist = -1;
            }
         }
      }

return sol;
}



Solution stage2_solve(Stage1_node *node1_arr)

{
Stage2_node             node2_arr[25];
register Stage1_node   *p_node;
register int            cperm, sliceedge0, sliceedge1;
int                     dist, max_dist, ii;

Solution sol;
sol.solved = false;
cperm = p_current_cube->cornerperm;
for (p_node = node1_arr; p_node->remain_depth > 0; p_node++)
    cperm = a_twist_on_cornerperm_ptrs[p_node[1].twist][cperm];

if (cornerperm_dist_table[cperm] > stage2_allotment)
   return sol;

node2_arr->cperm = cperm;

sliceedge0 = p_current_cube->ud_sliceedge;
for (p_node = node1_arr; p_node->remain_depth > 0; p_node++)
    sliceedge0 = twist_on_sliceedge_ptrs[p_node[1].twist][sliceedge0];

node2_arr->mperm = sliceedge_to_midperm[sliceedge0];

max_dist = (int)mid_cornerperm_dist_ptrs[node2_arr->mperm][cperm];

if (max_dist > stage2_allotment)
   return sol;

sliceedge0 = p_current_cube->rl_sliceedge;
sliceedge1 = p_current_cube->fb_sliceedge;
for (p_node = node1_arr; p_node->remain_depth > 0; p_node++)
    {
    sliceedge0 = twist_on_sliceedge_ptrs[p_node[1].twist][sliceedge0];
    sliceedge1 = twist_on_sliceedge_ptrs[p_node[1].twist][sliceedge1];
    }

node2_arr->eperm = half_to_edgeperm_ptrs[sliceedge_to_halfedgeperm[sliceedge0]]
                                        [sliceedge_to_halfedgeperm[sliceedge1]];

dist = (int)mid_edgeperm_dist_ptrs[node2_arr->mperm][node2_arr->eperm];
if (dist > stage2_allotment)
   return sol;

if (max_dist < dist)
   max_dist = dist;

node2_arr->tw_list = TWIST_LIST_BEGIN;
for (ii = max_dist; ii <= stage2_allotment; ii += increment)
    {
    node2_arr->remain_depth = ii;
    node2_arr[1].twist = -1;
    Solution s = stage2_search(node1_arr, node2_arr);
    if(s.solved && s.steps < MAX_SOLUTION) {
        return s;
    }
    }

return sol;
}



Solution stage1_search(Stage1_node *node_arr)
{
register Stage1_node   *p_node;
register int            twist;


p_node = node_arr;

Solution sol;
sol.solved = false;
while (p_node >= node_arr)
      {
        // check time
          struct timeval m_tv;
          gettimeofday(&m_tv, NULL);

          double start_time = solve_start_time_secs + (solve_start_time_usecs/1000000.0);
          double end_time = m_tv.tv_sec + (m_tv.tv_usec/1000000.0);

          double time_elapsed = end_time - start_time;
    
        if(time_elapsed >= MAX_TIME) {
            sol.solved = true;
            return sol;
        }
        
      if (p_node->remain_depth == 0)
         {
         if ((node_arr->remain_depth == 0) ||
             (is_stage2_twist[p_node->twist] == 0)){
             Solution s = stage2_solve(node_arr);
             if(s.solved && s.steps < MAX_SOLUTION) {
                 return s;
             }
            }
         p_node--;
         }
      else
         {
         for (twist = p_node[1].twist + 1; twist < A_N_TWIST; twist++)
             {
             if (a_twist_list_ptrs[twist][p_node->tw_list] ==
                                                        TWIST_LIST_INVALID)
                continue;

             p_node[1].remain_depth = p_node->remain_depth
                                                    - a_metric_length[twist];
             p_node[1].corner = twist_on_corner_ptrs[twist][p_node->corner];
             p_node[1].eflip = twist_on_eflip_ptrs[twist][p_node->eflip];

             if ((int)corner_eflip_dist_ptrs[p_node[1].corner][p_node[1].eflip]
                                                    > p_node[1].remain_depth)
                continue;

             p_node[1].eloc = twist_on_eloc_ptrs[twist][p_node->eloc];

             if ((int)corner_eloc_dist_ptrs[p_node[1].corner][p_node[1].eloc]
                                                    > p_node[1].remain_depth)
                continue;

             if ((int)eflip_eloc_dist_ptrs[p_node[1].eflip][p_node[1].eloc]
                                                    > p_node[1].remain_depth)
                continue;

             p_node[1].twist = twist;
             p_node[1].tw_list = a_twist_list_ptrs[twist][p_node->tw_list];

             break;
             }

         if (twist == A_N_TWIST)
            p_node--;
         else
            {
            p_node++;
            p_node[1].twist = -1;
            }
         }
      }

return sol;
}



Solution  solve_cube(Cube *p_cube)

{
Stage1_node             node_arr[25];
int                     dist, max_dist, ii;


p_current_cube = p_cube;
shortest_solution = BIG;

if ((metric == QUARTER_TURN_METRIC) &&
    (p_cube->parity != (shortest_solution % 2)))
   shortest_solution++;

node_arr->corner = p_cube->a_corner;
node_arr->eflip  = p_cube->a_edgeflip;
node_arr->eloc   = p_cube->a_edgeloc;
node_arr->tw_list = TWIST_LIST_BEGIN;

max_dist = (int)corner_eflip_dist_ptrs[p_cube->a_corner][p_cube->a_edgeflip];
dist = (int)corner_eloc_dist_ptrs[p_cube->a_corner][p_cube->a_edgeloc];
if (max_dist < dist)
   max_dist = dist;

dist = (int)eflip_eloc_dist_ptrs[p_cube->a_edgeflip][p_cube->a_edgeloc];
if (max_dist < dist)
   max_dist = dist;


Solution sol;
sol.solved = false;
for (ii = max_dist; ii <= BIG; ii++)
    {
    stage2_allotment = shortest_solution - ii - increment;

    if (stage2_allotment < 0)
       break;

    node_arr->remain_depth = ii;
    node_arr[1].twist = -1;

    Solution s = stage1_search(node_arr);
    
    if(s.solved && s.steps < MAX_SOLUTION) {
        return s;
    }

    //printf("depth %2d completed\n", ii);
    }

return sol;
}



Solution  solve_main(char* state)
/* ------------------------------------------------------------------------- */

{
Cube                    cube_struc;
int                     stat, returned_via_longjump;

if(!tables_done) {
    options_init(&user_options);
    twist_lengths_init(&a_metric_length, &b_metric_length);
    is_stage2_twist_init(is_stage2_twist);
    twist_sequences_init(twist_list_table, a_twist_list_ptrs, b_twist_list_ptrs);
    transformation_tables_init();
    prune_tables_init();
}

struct timeval m_tv;
gettimeofday(&m_tv, NULL);
solve_start_time_secs = m_tv.tv_sec;
solve_start_time_usecs = m_tv.tv_usec;

Solution sol;
sol.solved = false;

shortest_sol = 100;

stat = cube_init(&cube_struc, state);
if (stat < 0)
    return sol;


Solution final = solve_cube(&cube_struc); 

return best;
}
