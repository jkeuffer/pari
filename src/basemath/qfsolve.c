/* Copyright (C) 2000-2004  The PARI group.

This file is part of the PARI/GP package.

PARI/GP is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation. It is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY WHATSOEVER.

Check the License for details. You should have received a copy of it, along
with the package; see the file 'COPYING'. If not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

/* Copyright (C) 2014 Denis Simon
 * Adapted from qfsolve.gp v. 09/01/2014
 *   http://www.math.unicaen.fr/~simon/qfsolve.gp
 *
 * Author: Denis SIMON <simon@math.unicaen.fr> */

#include "pari.h"
#include "paripriv.h"

/* LINEAR ALGEBRA */

/* Gives a unimodular matrix with the last column(s) equal to Mv.
 * Mv can be a column vector or a rectangular matrix.
 * redflag = 0 or 1. If redflag = 1, LLL-reduce the n-#v first columns. */
static GEN
completebasis(GEN Mv, long redflag)
{
  GEN U;
  long m, n;

  if (typ(Mv) == t_COL) Mv = mkmat(Mv);
  n = lg(Mv)-1;
  m = nbrows(Mv); /* m x n */
  if (m == n) return Mv;
  (void)ZM_hnfall(shallowtrans(Mv), &U, 0);
  U = ZM_inv(shallowtrans(U), gen_1);
  if (m==1 || !redflag) return U;
  /* LLL-reduce the m-n first columns */
  return shallowconcat(ZM_lll(vecslice(U,1,m-n), 0.99, LLL_INPLACE),
                       vecslice(U, m-n+1,m));
}

/* Compute the kernel of M mod p.
 * returns [d,U], where
 * d = dim (ker M mod p)
 * U in GLn(Z), and its first d columns span the kernel. */
static GEN
kermodp(GEN M, GEN p, long *d)
{
  long j, l;
  GEN K, B, U;

  K = FpM_center(FpM_ker(M, p), p, shifti(p,-1));
  B = completebasis(K,0);
  l = lg(M); U = cgetg(l, t_MAT);
  for (j =  1; j < l; j++) gel(U,j) = gel(B,l-j);
  *d = lg(K)-1; return U;
}

/* INVARIANTS COMPUTATIONS */

static GEN
principal_minor(GEN G, long  i)
{ return rowslice(vecslice(G,1,i), 1,i); }
static GEN
det_minors(GEN G)
{
  long i, l = lg(G);
  GEN v = cgetg(l+1, t_VEC);
  gel(v,1) = gen_1;
  for (i = 2; i <= l; i++) gel(v,i) = ZM_det(principal_minor(G,i-1));
  return v;
}

/* Given a symmetric matrix G over Z, compute the Witt invariant
 *  of G at the prime p (at real place if p = NULL)
 * Assume that none of the determinant G[1..i,1..i] is 0. */
static long
qflocalinvariant(GEN G, GEN p)
{
  long i, j, c, l = lg(G);
  GEN diag, v = det_minors(G);
  /* Diagonalize G first. */
  diag = cgetg(l, t_VEC);
  for (i = 1; i < l; i++) gel(diag,i) = mulii(gel(v,i+1), gel(v,i));

  /* Then compute the product of the Hilbert symbols */
  /* (diag[i],diag[j])_p for i < j */
  c = 1;
  for (i = 1; i < l-1; i++)
    for (j = i+1; j < l; j++)
      if (hilbertii(gel(diag,i), gel(diag,j), p) < 0) c = -c;
  return c;
}

static GEN
hilberts(GEN a, GEN b, GEN P, long lP)
{
  GEN v = cgetg(lP, t_VECSMALL);
  long i;
  for (i = 1; i < lP; i++) v[i] = hilbertii(a, b, gel(P,i)) < 0;
  return v;
}

/* G symmetrix matrix or qfb or list of quadratic forms with same discriminant.
 * P must be equal to factor(-abs(2*matdet(G)))[,1]. */
static GEN
qflocalinvariants(GEN G, GEN P)
{
  GEN sol;
  long i, j, l, lP = lg(P);

  /* convert G into a vector of symmetric matrices */
  G = (typ(G) == t_VEC)? shallowcopy(G): mkvec(G);
  l = lg(G);
  for (j = 1; j < l; j++)
  {
    GEN g = gel(G,j);
    if (typ(g) == t_QFI || typ(g) == t_QFR) gel(G,j) = gtomat(g);
  }
  sol = cgetg(l, t_MAT);
  if (lg(gel(G,1)) == 3)
  { /* in dimension 2, each invariant is a single Hilbert symbol. */
    GEN d = negi(ZM_det(gel(G,1)));
    for (j = 1; j < l; j++)
    {
      GEN a = gcoeff(gel(G,j),1,1);
      gel(sol,j) = hilberts(a, d, P, lP);
    }
  }
  else /* in dimension n > 2, we compute a product of n Hilbert symbols. */
    for (j = 1; j <l; j++)
    {
      GEN g = gel(G,j), v = det_minors(g), w = cgetg(lP, t_VECSMALL);
      long n = lg(v);
      gel(sol,j) = w;
      for (i = 1; i < lP; i++)
      {
        GEN p = gel(P,i);
        long k = n-2, h = hilbertii(gel(v,k), gel(v,k+1),p);
        for (k--; k >= 1; k--)
          if (hilbertii(negi(gel(v,k)), gel(v,k+1),p) < 0) h = -h;
        w[i] = h < 0;
      }
    }
  return sol;
}

/* QUADRATIC FORM REDUCTION */

/* M = [a,b;b;c] has integral coefficients
 * Gauss reduction of the binary quadratic form
 * qf = (a,b,c)=a*X^2+2*b*X*Y+c*Y^2
 * Returns the reduction matrix with det = +1. */
#if 0
static GEN
qfbreduce(GEN M)
{
  GEN U, a = gcoeff(M,1,1); b = gcoeff(M,1,2); c = gcoeff(M,2,2);
  (void)redimagsl2(mkqfi(a,b,c), &U); return U;
}
#else
static GEN
qfbreduce(GEN M)
{
  GEN H = matid(2), a = gcoeff(M,1,1), b = gcoeff(M,1,2), c = gcoeff(M,2,2);
  while (signe(a))
  {
    GEN t, r, q, nexta, nextb, nextc;
    q = dvmdii(b,a, &r); /* FIXME: export as dvmdiiround ? */
    if (signe(r) > 0 && absi_cmp(shifti(r,1), a) > 0) {
      r = subii(r, absi(a)); q = addis(q, signe(a));
    }

    gel(H,2) = ZC_lincomb(gen_1, negi(q), gel(H,2), gel(H,1));
    nextc = a; nextb = negi(r); nexta = addii(mulii(subii(nextb,b),q), c);

    if (absi_cmp(nexta, a) >= 0) return H;
    c = nextc; b = nextb; a = nexta;
    t = gel(H,1); gel(H,1) = ZC_neg(gel(H,2)); gel(H,2) = t;
  }
  return H;
}
#endif

/* LLL-reduce a positive definite qf QD bounding the indefinite G, dim G > 1.
 * Then finishes the reduction with qfsolvetriv() */
static GEN qfsolvetriv(GEN G, long base);
static GEN
qflllgram_indef(GEN G, long base)
{
  GEN M, QD, M1, S, red;
  long i, n = lg(G)-1;

  M = NULL;
  QD = G;
  /* gaussred with early abort when a principal minor is singular */
  for (i = 1; i < n; i++)
  {
    GEN d = gcoeff(QD,i,i);
    long j;
    if (isintzero(d)) return qfsolvetriv(G,base);
    M1 = matid(n); d = gneg(d);
    for(j = i+1; j <= n; j++) gcoeff(M1,i,j) = gdiv(gcoeff(QD,i,j), d);
    M = M? RgM_mul(M,M1): M1;
    QD = qf_apply_RgM(QD,M1);
  }
  /* M~*G*M = QD diagonal */
  M = RgM_inv_upper(M);
  for (i=1; i<=n; i++) gcoeff(QD,i,i) = Q_abs_shallow(gcoeff(QD,i,i));
  QD = qf_apply_RgM(QD, M);
  S = lllgramint(Q_primpart(QD));
  if (lg(S)-1 < n) S = completebasis(S, 0);
  red = qfsolvetriv(qf_apply_ZM(G,S), base);
  if (typ(red) == t_COL) return ZM_ZC_mul(S,red);
  gel(red,2) = gmul(S, gel(red,2));
  if (lg(red) == 4) gel(red,3) = gmul(S, gel(red,3));
  return red;
}

/* LLL reduction of the quadratic form G (Gram matrix)
k* where we go on, even if an isotropic vector is found. */
static GEN
qflllgram_indefgoon(GEN G)
{
  GEN red, U, U1, G2, U2, G3, U3, G4, V, B, U4, G5, U5, G6, g;
  long j, n;

  red = qflllgram_indef(G,1);
  if (lg(red) == 3) return red; /* no isotropic vector found: nothing to do */
  /* otherwise a solution is found: */
  U1 = gel(red,2);
  G2 = gel(red,1); /* G2[1,1] = 0 */
  U2 = gel(mathnf0(row(G2,1), 4), 2);
  G3 = qf_apply_ZM(G2,U2);
  /* The first line of G3 only contains 0, except some 'g' on the right,
   * where g^2| det G. */
  n = lg(G)-1; g = gcoeff(G3,1,n);
  U3 = matid(n);
  gcoeff(U3,1,n) = diviiround(negi(gcoeff(G3,n,n)), shifti(g,1));
  G4 = qf_apply_ZM(G3,U3); /* G4[n,n] is reduced modulo 2g */
  U = mkmat2(mkcol2(gcoeff(G4,1,1),gcoeff(G4,1,n)),
             mkcol2(gcoeff(G4,1,n),gcoeff(G4,2,2)));
  if (n == 2)
    V = cgetg(1,t_MAT); /* should be 2x0 */
  else
    V = rowpermute(vecslice(G4, 2,n-1), mkvecsmall2(1,n));
  B = ground(gneg(RgM_mul(RgM_inv(U), V)));
  U4 = matid(n);
  for (j = 2; j < n; j++)
  {
    gcoeff(U4,1,j) = gcoeff(B,1,j-1);
    gcoeff(U4,n,j) = gcoeff(B,2,j-1);
  }
  G5 = qf_apply_ZM(G4,U4); /* the last column of G5 is reduced */
  U = U1;
  U = ZM_mul(U,U2);
  U = ZM_mul(U,U3);
  U = ZM_mul(U,U4);
  if (n < 4) return mkvec2(G5,U);

  red = qflllgram_indefgoon(rowslice(vecslice(G5,2,n-1),2,n-1));
  U5 = shallowmatconcat(diagonal_shallow(mkvec3(gen_1, gel(red,2), gen_1)));
  G6 = qf_apply_ZM(G5,U5);
  return mkvec2(G6, ZM_mul(U,U5));
}

/* qf_apply_ZM(G,H),  where H = matrix of \tau_{i,j}, i != j */
static GEN
qf_apply_tau(GEN G, long i, long j)
{
  long l = lg(G), k;
  G = RgM_shallowcopy(G);
  swap(gel(G,i), gel(G,j));
  for (k = 1; k < l; k++) swap(gcoeff(G,i,k), gcoeff(G,j,k));
  return G;
}

/* LLL reduction of the quadratic form G (Gram matrix)
 * in dim 3 only, with detG = -1 and sign(G) = [2,1]; */
static GEN
qflllgram_indefgoon2(GEN G)
{
  GEN red, G2, a, b, c, d, e, f, u, v, r, r3, U2, G3;

  red = qflllgram_indef(G,1); /* always find an isotropic vector. */
  G2 = qf_apply_tau(gel(red,1),1,3); /* G2[3,3] = 0 */
  r = row(gel(red,2), 3);
  swap(gel(r,1), gel(r,3)); /* apply tau_{1,3} */
  a = gcoeff(G2,3,1);
  b = gcoeff(G2,3,2);
  d = bezout(a,b, &u,&v);
  if (!is_pm1(d))
  {
    a = diviiexact(a,d);
    b = diviiexact(b,d);
  }
  /* for U2 = [-u,-b,0;-v,a,0;0,0,1]
   * G3 = qf_apply_ZM(G2,U2) has known last row (-d, 0, 0),
   * so apply to principal_minor(G3,2), instead */
  U2 = mkmat2(mkcol2(negi(u),negi(v)), mkcol2(negi(b),a));
  G3 = qf_apply_ZM(principal_minor(G2,2),U2);
  r3 = gel(r,3);
  r = ZV_ZM_mul(mkvec2(gel(r,1),gel(r,2)),U2);

  a = gcoeff(G3,1,1);
  b = gcoeff(G3,1,2);
  c = negi(d); /* G3[1,3] */
  d = gcoeff(G3,2,2);
  if (mpodd(a))
  {
    e = addii(b,d);
    a = addii(a, addii(b,e));
    e = diviiround(negi(e),c);
    f = diviiround(negi(a), shifti(c,1));
    a = addmulii(addii(gel(r,1),gel(r,2)), f,r3);
  }
  else
  {
    e = diviiround(negi(b),c);
    f = diviiround(negi(shifti(a,-1)), c);
    a = addmulii(gel(r,1), f, r3);
  }
  b = addmulii(gel(r,2), e, r3);
  return mkvec3(a,b, r3);
}

/* QUADRATIC FORM MINIMIZATION */

/* Minimization of the quadratic form G, deg G != 0, dim n >= 2
 * G symmetric integral
 * Returns [G',U,factd] with U in GLn(Q) such that G'=U~*G*U*constant
 * is integral and has minimal determinant.
 * In dimension 3 or 4, may return a prime p if the reduction at p is
 * impossible because of local non-solvability.
 * P,E = factor(+/- det(G)), "prime" -1 is ignored. Destroy E. */
static GEN qfsolvemodp(GEN G, GEN p);
static GEN
qfminimize(GEN G, GEN P, GEN E)
{
  GEN d, U, Ker, sol, aux, faE, faP;
  long n = lg(G)-1, lP = lg(P), i, dimKer, dimKer2, m;

  faP = vectrunc_init(lP);
  faE = vecsmalltrunc_init(lP);
  U = matid(n);
  for (i = 1; i < lP; i++)
  {
    GEN p = gel(P,i);
    long vp = E[i];
    if (!vp || !p) continue;

    if (DEBUGLEVEL >= 4) err_printf("    p^v = %Ps^%ld\n", p,vp);
    /* The case vp = 1 can be minimized only if n is odd. */
    if (vp == 1 && n%2 == 0) {
      vectrunc_append(faP, p);
      vecsmalltrunc_append(faE, 1);
      continue;
    }
    Ker = kermodp(G,p, &dimKer); /* dimKer <= vp */
    if (DEBUGLEVEL >= 4) err_printf("    dimKer = %ld\n",dimKer);
    if (dimKer == n)
    { /* trivial case: dimKer = n */
      if (DEBUGLEVEL >= 4) err_printf("     case 0: dimKer = n\n");
      G = RgM_Rg_div(G, p);
      E[i] -= n;
      i--; continue; /* same p */
    }
    G = qf_apply_ZM(G, Ker);
    U = RgM_mul(U,Ker);

    /* 1st case: dimKer < vp */
    /* then the kernel mod p contains a kernel mod p^2 */
    if (dimKer < vp)
    {
      if (DEBUGLEVEL >= 4) err_printf("    case 1: dimker < vp\n");
      if (dimKer == 1)
      {
        long j;
        gel(G,1) = RgC_Rg_div(gel(G,1), p);
        for (j = 1; j<=n; j++) gcoeff(G,1,j) = gdiv(gcoeff(G,1,j), p);
        gel(U,1) = RgC_Rg_div(gel(U,1), p);
        E[i] -= 2;
      }
      else
      {
        GEN Ker2 = RgM_Rg_div(principal_minor(G,dimKer),p);
        long j;
        Ker2 = kermodp(Ker2, p, &dimKer2);
        for (j = 1; j <= dimKer2; j++)
          gel(Ker2,j) = RgC_Rg_div(gel(Ker2,j), p);
        Ker2 = shallowmatconcat(diagonal_shallow(mkvec2(Ker2,matid(n-dimKer))));
        G = qf_apply_RgM(G, Ker2);
        U = RgM_mul(U,Ker2);
        E[i] -= 2*dimKer2;
      }
      i--; continue; /* same p */
    }

   /* vp = dimKer
    * 2nd case: kernel has dim >= 2 and contains an element of norm 0 mod p^2
    * search for an element of norm p^2... in the kernel */
    sol = NULL;
    if (dimKer > 2) {
      if (DEBUGLEVEL >= 4) err_printf("    case 2.1\n");
      dimKer = 3;
      sol = qfsolvemodp(RgM_Rg_div(principal_minor(G,3),p),  p);
    }
    else if (dimKer == 2)
    {
      GEN a= gcoeff(G,1,1), b = gcoeff(G,1,2), c = gcoeff(G,2,2);
      GEN di = subii(sqri(b), mulii(a,c)), p2 = sqri(p);
      di = diviiexact(di, p2);
      if (kronecker(modii(di,p),p) >= 0)
      {
        if (DEBUGLEVEL >= 4) err_printf("    case 2.2\n");
        if (dvdii(a, p2))
          sol = vec_ei(2,1);
        else
        {
          a = diviiexact(a,p);
          b = diviiexact(b,p);
          sol = mkcol2(subii(Fp_sqrt(di,p), b), a);
          sol = FpC_red(sol, p);
        }
      }
    }
    if (sol)
    {
      long j;
      sol = FpC_center(sol, p, shifti(p,-1));
      sol = Q_primpart(sol);
      if (DEBUGLEVEL >= 4) err_printf("    sol = %Ps\n", sol);
      Ker = zerocol(n);
      for (j = 1; j <= dimKer; j++) gel(Ker,j) = gel(sol,j); /* fill with 0's */
      Ker = completebasis(Ker,1);
      gel(Ker,n) = RgC_Rg_div(gel(Ker,n), p);
      G = qf_apply_RgM(G, Ker);
      U = RgM_mul(U,Ker);
      E[i] -= 2;
      i--; continue; /* same p */
    }
    /* Now 1 <= vp = dimKer <= 2 and kernel contains no vector with norm p^2 */
    /* exchanging kernel and image makes minimization easier ? */
    m = (n-3)/2;
    d = ZM_det(G); if (odd(m)) d = negi(d);
    if ((vp==1 && kronecker(gmod(gdiv(negi(d), gcoeff(G,1,1)),p), p) >= 0)
     || (vp==2 && odd(n) && n >= 5)
     || (vp==2 && !odd(n) && kronecker(modii(diviiexact(d,sqri(p)), p),p) < 0))
    {
      long j;
      if (DEBUGLEVEL >= 4) err_printf("    case 3\n");
      Ker = matid(n);
      for (j = dimKer+1; j <= n; j++) gcoeff(Ker,j,j) = p;
      G = RgM_Rg_div(qf_apply_ZM(G, Ker), p);
      U = RgM_mul(U,Ker);
      E[i] -= 2*dimKer-n;
      i--; continue; /* same p */
    }

    /* Minimization was not possible so far. */
    /* If n == 3 or 4, this proves the local non-solubility at p. */
    if (n == 3 || n == 4)
    {
      if (DEBUGLEVEL >= 1) err_printf(" no local solution at %Ps\n",p);
      return(p);
    }
    vectrunc_append(faP, p);
    vecsmalltrunc_append(faE, vp);
  }
  /* apply LLL to avoid coefficient explosion */
  aux = lllint(Q_primpart(U));
  return mkvec4(qf_apply_ZM(G,aux), RgM_mul(U,aux), faP, faE);
}

/* CLASS GROUP COMPUTATIONS */
static GEN
qfb(GEN D, GEN a, GEN b, GEN c)
{
  if (signe(D) < 0) return mkqfi(a,b,c);
  retmkqfr(a,b,c,real_0(DEFAULTPREC));
}

/* Compute the square root of the quadratic form q of discriminant D. Not
 * fully implemented; it only works for detqfb squarefree except at 2, where
 * the valuation is 2 or 3.
 * mkmat2(P,zv_to_ZV(E)) = factor(2*abs(det q)) */
static GEN
qfbsqrt(GEN D, GEN q, GEN P, GEN E)
{
  GEN a = gel(q,1), b = shifti(gel(q,2),-1), c = gel(q,3), mb = negi(b);
  GEN m,n, aux,Q1,M, A,B,C;
  GEN d = subii(mulii(a,c), sqri(b));
  long i;

  /* 1st step: solve m^2 = a (d), m*n = -b (d), n^2 = c (d) */
  m = n = mkintmod(gen_0,gen_1);
  E[1] -= 3;
  for (i = 1; i < lg(P); i++)
  {
    GEN p = gel(P,i), N, M;
    if (!E[i]) continue;
    if (dvdii(a,p)) {
      aux = Fp_sqrt(c, p);
      N = aux;
      M = Fp_div(mb, aux, p);
    } else {
      aux = Fp_sqrt(a, p);
      M = aux;
      N = Fp_div(mb, aux, p);
    }
    n = chinese(n, mkintmod(N,p));
    m = chinese(m, mkintmod(M,p));
  }
  m = centerlift(m);
  n = centerlift(n);
  if (DEBUGLEVEL >=4) err_printf("    [m,n] = [%Ps, %Ps]\n",m,n);

  /* 2nd step: build Q1, with det=-1 such that Q1(x,y,0) = G(x,y) */
  A = diviiexact(subii(sqri(n),c), d);
  B = diviiexact(addii(b, mulii(m,n)), d);
  C = diviiexact(subii(sqri(m), a), d);
  Q1 = mkmat3(mkcol3(A,B,n), mkcol3(B,C,m), mkcol3(n,m,d));
  Q1 = gneg(adj(Q1));

  /* 3rd step: reduce Q1 to [0,0,-1;0,1,0;-1,0,0] */
  M = qflllgram_indefgoon2(Q1);
  if (signe(gel(M,1)) < 0) M = ZC_neg(M);
  a = gel(M,1);
  b = gel(M,2);
  c = gel(M,3);
  if (mpodd(a))
    return qfb(D, a, shifti(b,1), shifti(c,1));
  else
    return qfb(D, c, shifti(negi(b),1), shifti(a,1));
}

/* \prod gen[i]^e[i] as a Qfb, e in {0,1}^n non-zero */
static GEN
qfb_factorback(GEN D, GEN gen, GEN e)
{
  GEN q = NULL, M, red;
  long j, l = lg(gen), n = 0;
  for (j = 1; j < l; j++)
    if (e[j]) { n++; q = q? qfbcompraw(q, gel(gen,j)): gel(gen,j); }
  if (n <= 1) return q;
  M = gtomat(q);
  red = qfbreduce(M);
  M = qf_apply_ZM(M, red);
  return qfb(D, gcoeff(M,1,1), shifti(gcoeff(M,1,2),1), gcoeff(M,2,2));
}

/* Shanks/Bosma-Stevenhagen algorithm to compute the 2-Sylow of the class
 * group of discriminant D. Only works for D = fundamental discriminant.
 * When D = 1(4), work with 4D.
 * P2D,E2D = factor(abs(2*D))
 * Pm2D = factor(-abs(2*D))[,1].
 * Return a form having Witt invariants W at Pm2D */
static GEN
quadclass2(GEN D, GEN P2D, GEN E2D, GEN Pm2D, GEN W, int n_is_4)
{
  GEN E, gen, Wgen, U2;
  long i, n, r, m, vD;

  if (equalii(D, utoineg(4))) return matid(2);
  if (mpodd(D))
  {
    D = shifti(D,2);
    E2D = shallowcopy(E2D);
    E2D[1] = 3;
  }

  n = lg(Pm2D)-1; r = n-3;
  m = (signe(D)>0)? r+1: r;
  if (m < 0) m = 0;

  if (n_is_4) /* look among forms of type q or 2*q: Q might be imprimitive */
    U2 = mkmat(hilberts(gen_2, D, Pm2D, lg(Pm2D)));
  else
    U2 = NULL;
  E = qfb(D, gen_1, gen_0, shifti(negi(D),-2));
  if (zv_equal0(W)) return gtomat(E);
  if (U2 && zv_equal(gel(U2,1),W)) return gmul2n(gtomat(E),1);

  gen = cgetg(m+1, t_VEC);
  vD = Z_lval(D,2);  /* = 2 or 3 */
  for (i = 1; i <= m; i++) /* no need to look at Pm2D[1]=-1, nor Pm2D[2]=2 */
  {
    GEN p = gel(Pm2D,i+2), d;
    long vp = Z_pvalrem(D,p, &d);
    GEN aux = powiu(p, vp);
    gel(gen,i) = qfb(D, aux, gen_0, negi(shifti(d,-2)));
  }
  if (vD == 2 && smodis(D,16) != 4)
  {
    GEN q2 = qfb(D, gen_2,gen_2, shifti(subsi(4,D),-3));
    m++; r++; gen = shallowconcat(gen, mkvec(q2));
  }
  if (vD == 3)
  {
    GEN q2 = qfb(D, int2n(vD-2),gen_0, negi(shifti(D,-vD)));
    m++; r++; gen = shallowconcat(gen, mkvec(q2));
  }
  if (!r) return gtomat(E);
  Wgen = qflocalinvariants(gen,Pm2D);
  for(;;)
  {
    GEN Wgen2, gen2, Ker, indexim = gel(Flm_indexrank(Wgen,2), 2);
    long dKer;
    if (lg(indexim)-1 >= r)
    {
      GEN W2 = Wgen, V;
      if (lg(indexim) < lg(Wgen)) W2 = vecpermute(Wgen,indexim);
      if (U2) W2 = shallowconcat(W2,U2);
      V = Flm_Flc_invimage(W2, W,2);
      if (V) {
        GEN Q = qfb_factorback(D, vecpermute(gen,indexim), V);
        Q = gtomat(Q);
        if (U2 && V[lg(V)-1]) Q = gmul2n(Q,1);
        return Q;
      }
    }
    Ker = Flm_ker(Wgen,2); dKer = lg(Ker)-1;
    gen2 = cgetg(m+1, t_VEC);
    Wgen2 = cgetg(m+1, t_MAT);
    for (i = 1; i <= dKer; i++)
    {
      GEN q = qfb_factorback(D, gen, gel(Ker,i));
      q = qfbsqrt(D,q,P2D,E2D);
      gel(gen2,i) = q;
      gel(Wgen2,i) = gel(qflocalinvariants(q,Pm2D), 1);
    }
    for (; i <=m; i++)
    {
      long j = indexim[i-dKer];
      gel(gen2,i) = gel(gen,j);
      gel(Wgen2,i) = gel(Wgen,j);
    }
    gen = gen2; Wgen = Wgen2;
  }
}

/* QUADRATIC EQUATIONS */
/* is x*y = -1 ? */
static int
both_pm1(GEN x, GEN y)
{ return is_pm1(x) && is_pm1(y) && signe(x) == -signe(y); }

/* Try to solve G = 0 with small coefficients. This is proved to work if
 * -  det(G) = 1, dim <= 6 and G is LLL reduced
 * Returns [G,matid] if no solution is found.
 * Exit with a norm 0 vector if one such is found.
 * If base == 1 and norm 0 is obtained, returns [H~*G*H,H,sol] where
 * sol is a norm 0 vector and is the 1st column of H. */
static GEN
qfsolvetriv(GEN G, long base)
{
  long n = lg(G)-1, i;
  GEN s, H = matid(n);

  /* case 1: A basis vector is isotropic */
  for (i = 1; i <= n; i++)
    if (!signe(gcoeff(G,i,i)))
    {
      s = gel(H,i);
      if (!base) return s;
      gel(H,i) = gel(H,1); gel(H,1) = s;
      return mkvec3(qf_apply_tau(G,1,i),H,s);
    }
  /* case 2: G has a block +- [1,0;0,-1] on the diagonal */
  for (i = 2; i <= n; i++)
    if (!signe(gcoeff(G,i-1,i)) && both_pm1(gcoeff(G,i-1,i-1),gcoeff(G,i,i)))
    {
      s = gel(H,i); gel(s,i-1) = gen_m1;
      if (!base) return s;
      gel(H,i) = gel(H,1); gel(H,1) = s;
      return mkvec3(qf_apply_ZM(G,H),H,s);
    }
  /* case 3: a principal minor is 0 */
  for (i = 2; i <= n; i++)
  {
    GEN GG = principal_minor(G,i);
    if (signe(ZM_det(GG))) continue;
    s = gel(keri(GG),1);
    s = shallowconcat(Q_primpart(s), zerocol(n-i));
    if (!base) return s;
    H = completebasis(s, 0);
    gel(H,n) = ZC_neg(gel(H,1)); gel(H,1) = s;
    return mkvec3(qf_apply_ZM(G,H),H,s);
  }
  return mkvec2(G,H);
}

/* p a prime number, G square such that det(G) !=0 mod p and dim G = n>=3
 * finds X such that X^t G X = 0 mod p */
static GEN
qfsolvemodp(GEN G, GEN p)
{
  GEN vdet, v1, v2, v3, mv3, G2,sol,x1,x2,x3,N1,N2,N3,s,r, S;
  GEN po2 = shifti(p,-1);
  long i, j, l = lg(G);

  vdet = cgetg(4, t_VEC);
  for (i = 1; i <= 3; i++)
  {
    GEN d;
    G2 = FpM_red(principal_minor(G,i), p);
    gel(vdet,i) = d = FpM_det(G2, p);
    if (!signe(d))
    {
      S = FpC_center(gel(FpM_ker(G2,p), 1), p, po2);
      goto END;
    }
  }
  v1 = gel(vdet,1);
  v2 = gel(vdet,2);
  v3 = gel(vdet,3);

  /* now, solve in dimension 3... reduction to the diagonal case: */
  x1 = mkcol3(gen_1, gen_0, gen_0);
  x2 = mkcol3(negi(gcoeff(G2,1,2)), gcoeff(G2,1,1), gen_0);

  N1 =  Fp_neg(v2,  p);
  if (kronecker(N1,p) == 1) {
    s = Fp_sqrt(N1,p);
    S = ZC_lincomb(s,gen_1,x1,x2);
    goto END;
  }

  x3 = mkcol3(subii(mulii(gcoeff(G2,2,2),gcoeff(G2,1,3)),
                    mulii(gcoeff(G2,2,3),gcoeff(G2,1,2))),
              subii(mulii(gcoeff(G2,1,1),gcoeff(G2,2,3)),
                    mulii(gcoeff(G2,1,3),gcoeff(G2,1,2))),
              subii(sqri(gcoeff(G2,1,2)),
                    mulii(gcoeff(G2,1,1),gcoeff(G2,2,2)))
       );
  x3 = FpC_red(x3, p);
  mv3 = Fp_neg(v3,p);
  N2 = Fp_div(mv3, v1, p);
  if (kronecker(N2,p) == 1) {
    s = Fp_sqrt(N2,p);
    S = ZC_lincomb(s,gen_1,x2,x3);
    goto END;
  }
  N3 = Fp_div(Fp_mul(v2,mv3,p), v1, p);
  if (kronecker(N3,p) == 1)
  {
    s = Fp_sqrt(N3,p);
    S = ZC_lincomb(s,gen_1,x1,x3);
    goto END;
  }
  r = gen_1;
  for(;;)
  {
    s = Fp_sub(gen_1, Fp_mul(N1,Fp_sqr(r,p),p), p);
    if (kronecker(s, p) <= 0) break;
    r = randomi(p);
  }
  s = Fp_sqrt(Fp_div(s,N3,p), p);
  S = ZC_add(x1, ZC_lincomb(r,s,x2,x3));
END:
  sol = zerocol(l-1);
  for  (j = 1; j <= i; j++) gel(sol,j) = gel(S,j);
  return sol;
}

/* Given a square matrix G of dimension n >= 1, */
/* solves over Z the quadratic equation X^tGX = 0. */
/* G is assumed to have integral coprime coefficients. */
/* The solution might be a vectorv or a matrix. */
/* If no solution exists, returns an integer, that can */
/* be a prime p such that there is no local solution at p, */
/* or -1 if there is no real solution, */
/* or 0 in some rare cases. */
static  GEN
qfsolve_i(GEN G)
{
  GEN M, signG, Min, U, G1, M1, G2, M2, solG2, P, E;
  GEN solG1, sol, Q, d, dQ, detG2, fam2detG;
  long n, np, codim, dim;

  if (typ(G) != t_MAT) pari_err_TYPE("qfsolve", G);
  n = lg(G)-1;
  if (n == 0) pari_err_DOMAIN("qfsolve", "dimension" , "=", gen_0, G);
  if (n != nbrows(G)) pari_err_DIM("qfsolve");
  G = Q_primpart(G); RgM_check_ZM(G, "qfsolve");

  /* Trivial case: det = 0 */
  d = ZM_det(G);
  if (!signe(d)) return ker(G);

  /* Small dimension: n <= 2 */
  if (n == 1) return gen_0;
  if (n == 2)
  {
    GEN di =  negi(d), a, b;
    if (!Z_issquare(di)) return gen_0;
    di = sqrti(di);
    a =  gcoeff(G,1,1);
    b =  gcoeff(G,1,2);
    return mkcol2(subii(di,b), a);
  }

  /* 1st reduction of the coefficients of G */
  M = qflllgram_indef(G,0);
  if (typ(M) == t_COL) return M;
  G = gel(M,1);
  M = gel(M,2);

  /* real solubility */
  signG = ZV_to_zv(qfsign(G));
  {
    long r =  signG[1], s = signG[2];
    if (!r || !s) return gen_m1;
    if (r < s) { G = ZM_neg(G); signG = mkvecsmall2(s,r);  }
  }

  /* factorization of the determinant */
  fam2detG = Z_factor( absi(d) );
  P = gel(fam2detG,1);
  E = ZV_to_zv(gel(fam2detG,2));
  /* P,E = factor(|det(G)|) */

  /* Minimization and local solubility */
  Min = qfminimize(G, P, E);
  if (typ(Min) == t_INT) return Min;

  M = gmul(M, gel(Min,2));
  G = gel(Min,1);
  P = gel(Min,3);
  E = gel(Min,4);
  /* P,E = factor(|det(G))| */

  /* Now, we know that local solutions exist (except maybe at 2 if n==4)
   * if n==3, det(G) = +-1
   * if n==4, or n is odd, det(G) is squarefree.
   * if n>=6, det(G) has all its valuations <=2. */

  /* Reduction of G and search for trivial solutions. */
  /* When |det G|=1, such trivial solutions always exist. */
  U = qflllgram_indef(G,0);
  if(typ(U) == t_COL) return gmul(M,U);
  G = gel(U,1);
  M = gmul(M, gel(U,2));
  /* P,E = factor(|det(G))| */

  /* If n >= 6 is even, need to increment the dimension by 1 to suppress all
   * squares from det(G) */
  np = lg(P)-1;
  if (n < 6 || odd(n) || !np)
  {
    codim = 0;
    G1 = G;
    M1 = NULL;
  }
  else
  {
    GEN aux;
    long i;
    codim = 1; n++;
    /* largest square divisor of d */
    aux = gen_1;
    for (i = 1; i <= np; i++)
      if (E[i] == 2) { aux = mulii(aux, gel(P,i)); E[i] = 3; }
    /* Choose sign(aux) so as to balance the signature of G1 */
    if (signG[1] > signG[2])
    {
      signG[2]++;
      aux = negi(aux);
    }
    else
      signG[1]++;
    G1 = shallowmatconcat(diagonal_shallow(mkvec2(G,aux)));
    /* P,E = factor(|det G1|) */
    Min = qfminimize(G1, P, E);
    G1 = gel(Min,1);
    M1 = gel(Min,2);
    P = gel(Min,3);
    E = gel(Min,4);
    np = lg(P)-1;
  }

  /* now, d is squarefree */
  if (!np)
  { /* |d| = 1 */
     G2 = G1;
     M2 = NULL;
  }
  else
  { /* |d| > 1: increment dimension by 2 */
    GEN factdP, factdE, W;
    long i, lfactdP;
    codim += 2;
    d = ZV_prod(P); /* d = abs(matdet(G1)); */
    if (odd(signG[2])) togglesign_safe(&d); /* d = matdet(G1); */
    /* solubility at 2 (this is the only remaining bad prime). */
    if (n == 4 && smodis(d,8) == 1 && qflocalinvariant(G,gen_2) == 1)
      return gen_2;

    P = shallowconcat(mpodd(d)? mkvec2(NULL,gen_2): mkvec(NULL), P);
    /* build a binary quadratic form with given Witt invariants */
    W = const_vecsmall(lg(P)-1, 0);
    /* choose signature of Q (real invariant and sign of the discriminant) */
    dQ = absi(d);
    if (signG[1] > signG[2]) togglesign_safe(&dQ); /* signQ = [2,0]; */
    if (n == 4 && smodis(dQ,4) != 1) dQ = shifti(dQ,2);
    if (n >= 5) dQ = shifti(dQ,3);

    /* p-adic invariants */
    if (n == 4)
    {
      GEN t = qflocalinvariants(ZM_neg(G1),P);
      for (i = 3; i < lg(P); i++) W[i] = ucoeff(t,i,1);
    }
    else
    {
      long s = signe(dQ) == signe(d)? 1: -1;
      GEN t;
      if (odd((n-3)/2)) s = -s;
      t = s > 0? utoipos(8): utoineg(8);
      for (i = 3; i < lg(P); i++)
        W[i] = hilbertii(t, gel(P,i), gel(P,i)) > 0;
    }
    /* for p = 2, the choice is fixed from the product formula */
    W[2] = Flv_sum(W, 2);

    /* Construction of the 2-class group of discriminant dQ until some product
     * of the generators gives the desired invariants. */
    factdP = vecsplice(P, 1); lfactdP =  lg(factdP);
    factdE = cgetg(lfactdP, t_VECSMALL);
    for (i = 1; i < lfactdP; i++) factdE[i] = Z_pval(dQ, gel(factdP,i));
    factdE[1]++;
    /* factdP,factdE = factor(2|dQ|), P = factor(-2|dQ|)[,1] */
    Q = quadclass2(dQ, factdP,factdE, P, W, n == 4);
    /* Build a form of dim=n+2 potentially unimodular */
    G2 = shallowmatconcat(diagonal_shallow(mkvec2(G1,ZM_neg(Q))));
    /* Minimization of G2 */
    detG2 = mulii(d, ZM_det(Q));
    for (i = 1; i < lfactdP; i++) factdE[i] = Z_pval(detG2, gel(factdP,i));
    /* factdP,factdE = factor(|det G2|) */
    Min = qfminimize(G2, factdP,factdE);
    M2 = gel(Min,2);
    G2 = gel(Min,1);
  }
  /* |det(G2)| = 1, find a totally isotropic subspace for G2 */
  solG2 = qflllgram_indefgoon(G2);
  /* G2 must have a subspace of solutions of dimension > codim */
  if (!gequal0(principal_minor(gel(solG2,1),codim+1)))
    pari_err_BUG("qfsolve (not enough solutions in G2)");

  dim = codim+2;
  while(gequal0(principal_minor(gel(solG2,1), dim))) dim ++;
  solG2 = vecslice(gel(solG2,2), 1, dim-1);

  if (!M2)
    solG1 = solG2;
  else
  { /* solution of G1 is simultaneously in solG2 and x[n+1] = x[n+2] = 0*/
    GEN K;
    solG1 = RgM_mul(M2,solG2);
    K = ker(rowslice(solG1,n+1,n+2));
    solG1 = RgM_mul(rowslice(solG1,1,n), K);
  }
  if (!M1)
    sol = solG1;
  else
  { /* solution of G1 is simultaneously in solG2 and x[n] = 0 */
    GEN K;
    sol = RgM_mul(M1,solG1);
    K = ker(rowslice(sol,n,n));
    sol = RgM_mul(rowslice(sol,1,n-1), K);
  }
  sol = Q_primpart(gmul(M, sol));
  if (lg(sol) == 2) sol = gel(sol,1);
  return sol;
}
GEN
qfsolve(GEN G)
{
  pari_sp av = avma;
  return gerepilecopy(av, qfsolve_i(G));
}

/* G is a symmetric 3x3 matrix, and sol a solution of sol~*G*sol=0.
 * Returns a parametrization of the solutions with the good invariants,
 * as a matrix 3x3, where each line contains
 * the coefficients of each of the 3 quadratic forms.
 * If fl!=0, the fl-th form is reduced. */
GEN
qfparam(GEN G, GEN sol, long fl)
{
  pari_sp av = avma;
  GEN U, G1, G2, a, b, c, d, e;
  long n;

  if (typ(G) != t_MAT) pari_err_TYPE("qfsolve", G);
  n = lg(G)-1;
  if (n == 0) pari_err_DOMAIN("qfsolve", "dimension" , "=", gen_0, G);
  if (n != nbrows(G)) pari_err_DIM("qfsolve");
  sol = Q_primpart(sol);
  /* build U such that U[,3] = sol, and |det(U)| = 1 */
  U = completebasis(sol,1);
  G1 = qf_apply_ZM(G,U); /* G1 has a 0 at the bottom right corner */
  a = shifti(gcoeff(G1,1,2),1);
  b = shifti(negi(gcoeff(G1,1,3)),1);
  c = shifti(negi(gcoeff(G1,2,3)),1);
  d = gcoeff(G1,1,1);
  e = gcoeff(G1,2,2);
  G2 = mkmat3(mkcol3(b,gen_0,d), mkcol3(c,b,a), mkcol3(gen_0,c,e));
  sol = ZM_mul(U,G2);
  if (fl)
  {
    GEN v = row(sol,fl);
    a = gel(v,1);
    b = gmul2n(gel(v,2),-1);
    c = gel(v,3);
    U = qflllgram_indef(mkmat2(mkcol2(a,b),mkcol2(b,c)), 1);
    U = gel(U,2);
    a = gcoeff(U,1,1); b = gcoeff(U,1,2);
    c = gcoeff(U,2,1); d = gcoeff(U,2,2);
    U = mkmat3(mkcol3(sqri(a),mulii(a,c),sqri(c)),
               mkcol3(shifti(mulii(a,b),1), addii(mulii(a,d),mulii(b,c)),
                      shifti(mulii(c,d),1)),
               mkcol3(sqri(b),mulii(b,d),sqri(d)));
    sol = ZM_mul(sol,U);
  }
  return gerepileupto(av, sol);
}