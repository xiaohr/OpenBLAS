/*******************************************************************************
Copyright (c) 2016, The OpenBLAS Project
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
3. Neither the name of the OpenBLAS project nor the names of
its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "common.h"
#include "macros_msa.h"

#define ENABLE_PREFETCH

#ifdef ENABLE_PREFETCH
inline static void prefetch_load_lf(unsigned char *src) {
    __asm__ __volatile__("pref   0,  0(%[src])   \n\t" : : [src] "r"(src));
}
#endif

static void __attribute__ ((noinline))
dgemmkernel_8x4_core_msa(BLASLONG m, BLASLONG n, BLASLONG k, FLOAT alpha, FLOAT *A, FLOAT *B,
                         FLOAT *C, BLASLONG ldc)
{
    BLASLONG i, j, l;
    FLOAT *pc0, *pc1, *pc2, *pc3, *pa0, *pb0;
    v2f64 v_alpha = {alpha, alpha};
    v2f64 src_a0, src_a1, src_a2, src_a3, src_b, src_b0, src_b1;
    v2f64 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
    v2f64 res0, res1, res2, res3, res4, res5, res6, res7;
    v2f64 res8, res9, res10, res11, res12, res13, res14, res15;

    for (j = (n >> 2); j--;)
    {
        pc0 = C;
        pc1 = pc0 + ldc;
        pc2 = pc1 + ldc;
        pc3 = pc2 + ldc;

        pa0 = A;

        for (i = (m >> 3); i--;)
        {
            pb0 = B;

#ifdef ENABLE_PREFETCH
            prefetch_load_lf((unsigned char *)(pa0 + 8));
            prefetch_load_lf((unsigned char *)(pa0 + 12));
            prefetch_load_lf((unsigned char *)(pa0 + 16));
            prefetch_load_lf((unsigned char *)(pa0 + 20));

            prefetch_load_lf((unsigned char *)(pb0 + 4));
            prefetch_load_lf((unsigned char *)(pb0 + 8));
            prefetch_load_lf((unsigned char *)(pb0 + 12));
#endif

            LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
            LD_DP2_INC(pb0, 2, src_b0, src_b1);

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
            res0 = src_a0 * src_b;
            res1 = src_a1 * src_b;
            res2 = src_a2 * src_b;
            res3 = src_a3 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
            res4 = src_a0 * src_b;
            res5 = src_a1 * src_b;
            res6 = src_a2 * src_b;
            res7 = src_a3 * src_b;

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
            res8  = src_a0 * src_b;
            res9  = src_a1 * src_b;
            res10 = src_a2 * src_b;
            res11 = src_a3 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
            res12 = src_a0 * src_b;
            res13 = src_a1 * src_b;
            res14 = src_a2 * src_b;
            res15 = src_a3 * src_b;

            for (l = ((k - 1) >> 1); l--;)
            {
#ifdef ENABLE_PREFETCH
                prefetch_load_lf((unsigned char *)(pa0 + 16));
                prefetch_load_lf((unsigned char *)(pa0 + 20));
                prefetch_load_lf((unsigned char *)(pa0 + 24));
                prefetch_load_lf((unsigned char *)(pa0 + 28));
#endif
                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;
                res6 += src_a2 * src_b;
                res7 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res8  += src_a0 * src_b;
                res9  += src_a1 * src_b;
                res10 += src_a2 * src_b;
                res11 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res12 += src_a0 * src_b;
                res13 += src_a1 * src_b;
                res14 += src_a2 * src_b;
                res15 += src_a3 * src_b;

                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
#ifdef ENABLE_PREFETCH
                prefetch_load_lf((unsigned char *)(pb0 + 8));
                prefetch_load_lf((unsigned char *)(pb0 + 12));
#endif
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;
                res6 += src_a2 * src_b;
                res7 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res8  += src_a0 * src_b;
                res9  += src_a1 * src_b;
                res10 += src_a2 * src_b;
                res11 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res12 += src_a0 * src_b;
                res13 += src_a1 * src_b;
                res14 += src_a2 * src_b;
                res15 += src_a3 * src_b;
            }

            if ((k - 1) & 1)
            {
                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;
                res6 += src_a2 * src_b;
                res7 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res8  += src_a0 * src_b;
                res9  += src_a1 * src_b;
                res10 += src_a2 * src_b;
                res11 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res12 += src_a0 * src_b;
                res13 += src_a1 * src_b;
                res14 += src_a2 * src_b;
                res15 += src_a3 * src_b;
            }

#ifdef ENABLE_PREFETCH
            prefetch_load_lf((unsigned char *)(pc0 + 8));
            prefetch_load_lf((unsigned char *)(pc1 + 8));
            prefetch_load_lf((unsigned char *)(pc2 + 8));
            prefetch_load_lf((unsigned char *)(pc3 + 8));
#endif
            LD_DP4(pc0, 2, dst0, dst1, dst2, dst3);
            LD_DP4(pc1, 2, dst4, dst5, dst6, dst7);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;
            dst2 += res2 * v_alpha;
            dst3 += res3 * v_alpha;
            dst4 += res4 * v_alpha;
            dst5 += res5 * v_alpha;
            dst6 += res6 * v_alpha;
            dst7 += res7 * v_alpha;

            ST_DP4_INC(dst0, dst1, dst2, dst3, pc0, 2);
            ST_DP4_INC(dst4, dst5, dst6, dst7, pc1, 2);

            LD_DP4(pc2, 2, dst0, dst1, dst2, dst3);
            LD_DP4(pc3, 2, dst4, dst5, dst6, dst7);

            dst0 += res8 * v_alpha;
            dst1 += res9 * v_alpha;
            dst2 += res10 * v_alpha;
            dst3 += res11 * v_alpha;
            dst4 += res12 * v_alpha;
            dst5 += res13 * v_alpha;
            dst6 += res14 * v_alpha;
            dst7 += res15 * v_alpha;

            ST_DP4_INC(dst0, dst1, dst2, dst3, pc2, 2);
            ST_DP4_INC(dst4, dst5, dst6, dst7, pc3, 2);
        }

        l = (k << 2);
        B = B + l;
        i = (ldc << 2);
        C = C + i;
    }
}

static void __attribute__ ((noinline))
dgemmkernel_7x4_core_msa(BLASLONG m, BLASLONG n, BLASLONG k, FLOAT alpha, FLOAT *A, FLOAT *B,
                         FLOAT *C, BLASLONG ldc)
{
    BLASLONG i, j, l;
    FLOAT *pc0, *pc1, *pc2, *pc3, *pa0, *pb0;
    FLOAT tmp0, tmp1, tmp2, tmp3;
    FLOAT a0, b0, b1, b2, b3;
    v2f64 v_alpha = {alpha, alpha};
    v2f64 src_a0, src_a1, src_b, src_b0, src_b1;
    v2f64 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
    v2f64 res0, res1, res2, res3, res4, res5, res6, res7;

    for (j = (n >> 2); j--;)
    {

        pc0 = C + 8 * (m >> 3);
        pc1 = pc0 + ldc;
        pc2 = pc1 + ldc;
        pc3 = pc2 + ldc;

        pa0 = A + k * 8 * (m >> 3);

        if (m & 4)
        {
            pb0 = B;

            LD_DP2_INC(pa0, 2, src_a0, src_a1);
            LD_DP2_INC(pb0, 2, src_b0, src_b1);

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
            res0 = src_a0 * src_b;
            res1 = src_a1 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
            res2 = src_a0 * src_b;
            res3 = src_a1 * src_b;

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
            res4 = src_a0 * src_b;
            res5 = src_a1 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
            res6 = src_a0 * src_b;
            res7 = src_a1 * src_b;

            for (l = ((k - 1) >> 1); l--;)
            {
                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res2 += src_a0 * src_b;
                res3 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res6 += src_a0 * src_b;
                res7 += src_a1 * src_b;

                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res2 += src_a0 * src_b;
                res3 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res6 += src_a0 * src_b;
                res7 += src_a1 * src_b;
            }

            if ((k - 1) & 1)
            {
                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res2 += src_a0 * src_b;
                res3 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res6 += src_a0 * src_b;
                res7 += src_a1 * src_b;
            }

            LD_DP2(pc0, 2, dst0, dst1);
            LD_DP2(pc1, 2, dst2, dst3);
            LD_DP2(pc2, 2, dst4, dst5);
            LD_DP2(pc3, 2, dst6, dst7);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;
            dst2 += res2 * v_alpha;
            dst3 += res3 * v_alpha;
            dst4 += res4 * v_alpha;
            dst5 += res5 * v_alpha;
            dst6 += res6 * v_alpha;
            dst7 += res7 * v_alpha;

            ST_DP2_INC(dst0, dst1, pc0, 2);
            ST_DP2_INC(dst2, dst3, pc1, 2);
            ST_DP2_INC(dst4, dst5, pc2, 2);
            ST_DP2_INC(dst6, dst7, pc3, 2);
        }

        if (m & 2)
        {
            pb0 = B;

            src_a0 = LD_DP(pa0);
            pa0 += 2;
            LD_DP2_INC(pb0, 2, src_b0, src_b1);

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
            res0 = src_a0 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
            res1 = src_a0 * src_b;

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
            res2 = src_a0 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
            res3 = src_a0 * src_b;

            for (l = ((k - 1) >> 1); l--;)
            {
                src_a0 = LD_DP(pa0);
                pa0 += 2;
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res1 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res2 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res3 += src_a0 * src_b;

                src_a0 = LD_DP(pa0);
                pa0 += 2;
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res1 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res2 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res3 += src_a0 * src_b;
            }

            if ((k - 1) & 1)
            {
                src_a0 = LD_DP(pa0);
                pa0 += 2;
                LD_DP2_INC(pb0, 2, src_b0, src_b1);

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res1 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b1, (v2i64) src_b1);
                res2 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b1, (v2i64) src_b1);
                res3 += src_a0 * src_b;
            }

            dst0 = LD_DP(pc0);
            dst1 = LD_DP(pc1);
            dst2 = LD_DP(pc2);
            dst3 = LD_DP(pc3);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;
            dst2 += res2 * v_alpha;
            dst3 += res3 * v_alpha;

            ST_DP(dst0, pc0);
            ST_DP(dst1, pc1);
            ST_DP(dst2, pc2);
            ST_DP(dst3, pc3);

            pc0 += 2;
            pc1 += 2;
            pc2 += 2;
            pc3 += 2;
        }

        if (m & 1)
        {
            pb0 = B;

            a0 = pa0[0];
            b0 = pb0[0];
            tmp0 = a0 * b0;

            b1 = pb0[1];
            tmp1 = a0 * b1;

            b2 = pb0[2];
            tmp2 = a0 * b2;

            b3 = pb0[3];
            tmp3 = a0 * b3;

            pa0 += 1;
            pb0 += 4;

            for (l = ((k - 1) >> 1); l--;)
            {
                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                b1 = pb0[1];
                tmp1 += a0 * b1;

                b2 = pb0[2];
                tmp2 += a0 * b2;

                b3 = pb0[3];
                tmp3 += a0 * b3;

                pa0 += 1;
                pb0 += 4;

                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                b1 = pb0[1];
                tmp1 += a0 * b1;

                b2 = pb0[2];
                tmp2 += a0 * b2;

                b3 = pb0[3];
                tmp3 += a0 * b3;

                pa0 += 1;
                pb0 += 4;
            }

            if ((k - 1) & 1)
            {
                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                b1 = pb0[1];
                tmp1 += a0 * b1;

                b2 = pb0[2];
                tmp2 += a0 * b2;

                b3 = pb0[3];
                tmp3 += a0 * b3;

                pa0 += 1;
                pb0 += 4;
            }

            tmp0 = alpha * tmp0;
            tmp1 = alpha * tmp1;
            tmp2 = alpha * tmp2;
            tmp3 = alpha * tmp3;

            pc0[0] += tmp0;
            pc1[0] += tmp1;
            pc2[0] += tmp2;
            pc3[0] += tmp3;

            pc0 += 1;
            pc1 += 1;
            pc2 += 1;
            pc3 += 1;
        }

        l = (k << 2);
        B = B + l;
        i = (ldc << 2);
        C = C + i;
    }
}

static void __attribute__ ((noinline))
dgemmkernel_8x4_non_core_msa(BLASLONG m, BLASLONG n, BLASLONG k, FLOAT alpha, FLOAT *A, FLOAT *B,
                             FLOAT *C, BLASLONG ldc)
{
    BLASLONG i, l;
    FLOAT *pc0, *pc1, *pa0, *pb0;
    FLOAT tmp0, tmp1;
    FLOAT a0, b0, b1;
    v2f64 v_alpha = {alpha, alpha};
    v2f64 src_a0, src_a1, src_a2, src_a3, src_b, src_b0;
    v2f64 dst0, dst1, dst2, dst3, dst4, dst5, dst6, dst7;
    v2f64 res0, res1, res2, res3, res4, res5, res6, res7;

    if (n & 2)
    {
        pc0 = C;
        pc1 = pc0 + ldc;

        pa0 = A;

        for (i = (m >> 3); i--;)
        {
            pb0 = B;

            LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
            src_b0 = LD_DP(pb0);
            pb0 += 2;

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
            res0 = src_a0 * src_b;
            res1 = src_a1 * src_b;
            res2 = src_a2 * src_b;
            res3 = src_a3 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
            res4 = src_a0 * src_b;
            res5 = src_a1 * src_b;
            res6 = src_a2 * src_b;
            res7 = src_a3 * src_b;

            for (l = ((k - 1) >> 1); l--;)
            {
                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;
                res6 += src_a2 * src_b;
                res7 += src_a3 * src_b;

                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;
                res6 += src_a2 * src_b;
                res7 += src_a3 * src_b;
            }

            if ((k - 1) & 1)
            {
                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res4 += src_a0 * src_b;
                res5 += src_a1 * src_b;
                res6 += src_a2 * src_b;
                res7 += src_a3 * src_b;
            }

            LD_DP4(pc0, 2, dst0, dst1, dst2, dst3);
            LD_DP4(pc1, 2, dst4, dst5, dst6, dst7);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;
            dst2 += res2 * v_alpha;
            dst3 += res3 * v_alpha;
            dst4 += res4 * v_alpha;
            dst5 += res5 * v_alpha;
            dst6 += res6 * v_alpha;
            dst7 += res7 * v_alpha;

            ST_DP4_INC(dst0, dst1, dst2, dst3, pc0, 2);
            ST_DP4_INC(dst4, dst5, dst6, dst7, pc1, 2);
        }

        if (m & 4)
        {
            pb0 = B;

            LD_DP2_INC(pa0, 2, src_a0, src_a1);
            src_b0 = LD_DP(pb0);
            pb0 += 2;

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
            res0 = src_a0 * src_b;
            res1 = src_a1 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
            res2 = src_a0 * src_b;
            res3 = src_a1 * src_b;

            for (l = ((k - 1) >> 1); l--;)
            {
                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res2 += src_a0 * src_b;
                res3 += src_a1 * src_b;

                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res2 += src_a0 * src_b;
                res3 += src_a1 * src_b;
            }

            if ((k - 1) & 1)
            {
                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res2 += src_a0 * src_b;
                res3 += src_a1 * src_b;
            }

            LD_DP2(pc0, 2, dst0, dst1);
            LD_DP2(pc1, 2, dst2, dst3);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;
            dst2 += res2 * v_alpha;
            dst3 += res3 * v_alpha;

            ST_DP2_INC(dst0, dst1, pc0, 2);
            ST_DP2_INC(dst2, dst3, pc1, 2);
        }

        if (m & 2)
        {
            pb0 = B;

            src_a0 = LD_DP(pa0);
            pa0 += 2;
            src_b0 = LD_DP(pb0);
            pb0 += 2;

            src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
            res0 = src_a0 * src_b;

            src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
            res1 = src_a0 * src_b;

            for (l = ((k - 1) >> 1); l--;)
            {
                src_a0 = LD_DP(pa0);
                pa0 += 2;
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res1 += src_a0 * src_b;

                src_a0 = LD_DP(pa0);
                pa0 += 2;
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res1 += src_a0 * src_b;
            }

            if ((k - 1) & 1)
            {
                src_a0 = LD_DP(pa0);
                pa0 += 2;
                src_b0 = LD_DP(pb0);
                pb0 += 2;

                src_b = (v2f64) __msa_ilvr_d((v2i64) src_b0, (v2i64) src_b0);
                res0 += src_a0 * src_b;

                src_b = (v2f64) __msa_ilvl_d((v2i64) src_b0, (v2i64) src_b0);
                res1 += src_a0 * src_b;
            }

            dst0 = LD_DP(pc0);
            dst1 = LD_DP(pc1);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;

            ST_DP(dst0, pc0);
            ST_DP(dst1, pc1);

            pc0 += 2;
            pc1 += 2;
        }

        if (m & 1)
        {
            pb0 = B;

            a0 = pa0[0];
            b0 = pb0[0];
            tmp0 = a0 * b0;

            b1 = pb0[1];
            tmp1 = a0 * b1;

            pa0 += 1;
            pb0 += 2;

            for (l = ((k - 1) >> 1); l--;)
            {
                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                b1 = pb0[1];
                tmp1 += a0 * b1;

                pa0 += 1;
                pb0 += 2;

                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                b1 = pb0[1];
                tmp1 += a0 * b1;

                pa0 += 1;
                pb0 += 2;
            }

            if ((k - 1) & 1)
            {
                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                b1 = pb0[1];
                tmp1 += a0 * b1;

                pa0 += 1;
                pb0 += 2;
            }

            tmp0 = alpha * tmp0;
            tmp1 = alpha * tmp1;

            pc0[0] += tmp0;
            pc1[0] += tmp1;

            pc0 += 1;
            pc1 += 1;
        }

        l = (k << 1);
        B = B + l;
        i = (ldc << 1);
        C = C + i;
    }

    if (n & 1)
    {
        pc0 = C;
        pa0 = A;

        for (i = (m >> 3); i--;)
        {
            pb0 = B;

            LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
            src_b[0] = pb0[0];
            src_b[1] = pb0[0];

            res0 = src_a0 * src_b;
            res1 = src_a1 * src_b;
            res2 = src_a2 * src_b;
            res3 = src_a3 * src_b;

            pb0 += 1;

            for (l = ((k - 1) >> 1); l--;)
            {
                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                pb0 += 1;

                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                pb0 += 1;
            }

            if ((k - 1) & 1)
            {
                LD_DP4_INC(pa0, 2, src_a0, src_a1, src_a2, src_a3);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;
                res2 += src_a2 * src_b;
                res3 += src_a3 * src_b;

                pb0 += 1;
            }

            LD_DP4(pc0, 2, dst0, dst1, dst2, dst3);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;
            dst2 += res2 * v_alpha;
            dst3 += res3 * v_alpha;

            ST_DP4_INC(dst0, dst1, dst2, dst3, pc0, 2);
        }

        if (m & 4)
        {
            pb0 = B;

            LD_DP2_INC(pa0, 2, src_a0, src_a1);
            src_b[0] = pb0[0];
            src_b[1] = pb0[0];

            res0 = src_a0 * src_b;
            res1 = src_a1 * src_b;

            pb0 += 1;

            for (l = ((k - 1) >> 1); l--;)
            {
                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                pb0 += 1;

                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                pb0 += 1;
            }

            if ((k - 1) & 1)
            {
                LD_DP2_INC(pa0, 2, src_a0, src_a1);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;
                res1 += src_a1 * src_b;

                pb0 += 1;
            }

            LD_DP2(pc0, 2, dst0, dst1);

            dst0 += res0 * v_alpha;
            dst1 += res1 * v_alpha;

            ST_DP2_INC(dst0, dst1, pc0, 2);
        }

        if (m & 2)
        {
            pb0 = B;

            src_a0 = LD_DP(pa0);
            src_b[0] = pb0[0];
            src_b[1] = pb0[0];

            res0 = src_a0 * src_b;

            pa0 += 2;
            pb0 += 1;

            for (l = ((k - 1) >> 1); l--;)
            {
                src_a0 = LD_DP(pa0);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;

                pa0 += 2;
                pb0 += 1;

                src_a0 = LD_DP(pa0);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;

                pa0 += 2;
                pb0 += 1;
            }

            if ((k - 1) & 1)
            {
                src_a0 = LD_DP(pa0);
                src_b[0] = pb0[0];
                src_b[1] = pb0[0];

                res0 += src_a0 * src_b;

                pa0 += 2;
                pb0 += 1;
            }

            dst0 = LD_DP(pc0);

            dst0 += res0 * v_alpha;

            ST_DP(dst0, pc0);

            pc0 += 2;
        }

        if (m & 1)
        {
            pb0 = B;

            a0 = pa0[0];
            b0 = pb0[0];
            tmp0 = a0 * b0;

            pa0 += 1;
            pb0 += 1;

            for (l = ((k - 1) >> 1); l--;)
            {
                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                pa0 += 1;
                pb0 += 1;

                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                pa0 += 1;
                pb0 += 1;
            }

            if ((k - 1) & 1)
            {
                a0 = pa0[0];
                b0 = pb0[0];
                tmp0 += a0 * b0;

                pa0 += 1;
                pb0 += 1;
            }

            pc0[0] += alpha * tmp0;

            pc0 += 1;
        }

        l = (k << 0);
        B = B + l;
        i = (ldc << 0);
        C = C + i;
    }
}

int CNAME(BLASLONG m, BLASLONG n, BLASLONG k, FLOAT alpha, FLOAT *A, FLOAT *B,
          FLOAT *C, BLASLONG ldc
#ifdef TRMMKERNEL
          , BLASLONG offset
#endif
          )
{
    if (n >> 2)
    {
        if (m >> 3)
            dgemmkernel_8x4_core_msa(m, n, k, alpha, A, B, C, ldc);

        if (m & 7)
            dgemmkernel_7x4_core_msa(m, n, k, alpha, A, B, C, ldc);
    }

    if (n & 3)
    {
        B = B + (k << 2) * (n >> 2);
        C = C + (ldc << 2) * (n >> 2);

        dgemmkernel_8x4_non_core_msa(m, n, k, alpha, A, B, C, ldc);
    }

    return 0;
}
