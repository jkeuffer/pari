/* Copyright (C) 2004  The PARI group.

This file is part of the PARI/GP package.

PARI/GP is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation. It is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY WHATSOEVER.

Check the License for details. You should have received a copy of it, along
with the package; see the file 'COPYING'. If not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

BEGINEXTERN
/* hashtables */
hashtable *hashstr_import_static(hashentry *e, ulong size);
void hashstr_dbg(hashtable *h);

/* for qsort */
typedef int (*QSCOMP)(const void *, const void *);

#define ucoeff(a,i,j)  (((ulong**)(a))[j][i])
#define umael(a,i,j)   (((ulong**)(a))[i][j])
#define uel(a,i)       (((ulong*)(a))[i])

#define numberof(x) (sizeof(x) / sizeof((x)[0]))

/* to manipulate 'blocs' */
#define BL_HEAD 4
#define bl_base(x) (void*)((x) - BL_HEAD)
#define bl_refc(x) (((GEN)x)[-4])
#define bl_next(x) (((GEN*)x)[-3])
#define bl_prev(x) (((GEN*)x)[-2])
#define bl_num(x)  (((GEN)x)[-1])
INLINE void
clone_lock(GEN C) { if (isclone(C)) ++bl_refc(C); }
INLINE void
clone_unlock(GEN C) { if (isclone(C)) gunclone(C); }

/* swap */
#define lswap(x,y) {long _z=x; x=y; y=_z;}
#define pswap(x,y) {GEN *_z=x; x=y; y=_z;}
#define swap(x,y)  {GEN  _z=x; x=y; y=_z;}
#define dswap(x,y) { double _t=x; x=y; y=_t; }
#define pdswap(x,y) { double* _t=x; x=y; y=_t; }
#define swapspec(x,y, nx,ny) {swap(x,y); lswap(nx,ny);}

/* unused */
void ellprint(GEN e);

/* binary splitting */
struct abpq { GEN *a, *b, *p, *q; };
struct abpq_res { GEN P, Q, B, T; };
void abpq_init(struct abpq *A, long n);
void abpq_sum(struct abpq_res *r, long n1, long n2, struct abpq *A);

/* generic */
GEN trans_fix_arg(long *prec, GEN *s0, GEN *sig, GEN *tau, pari_sp *av, GEN *res);
GEN sort_factor_pol(GEN y, int (*cmp)(GEN,GEN));

/* loops */
GEN incloop(GEN a);
GEN resetloop(GEN a, GEN b);
GEN setloop(GEN a);

/* parser */
GEN  iferrpari(GEN a, GEN b, GEN c);
void forpari(GEN a, GEN b, GEN node);
void untilpari(GEN a, GEN b);
void whilepari(GEN a, GEN b);
GEN  ifpari(GEN g, GEN a, GEN b);
GEN  andpari(GEN a, GEN b);
GEN  orpari(GEN a, GEN b);
void ifpari_void(GEN g, GEN a, GEN b);
GEN  ifpari_multi(GEN g, GEN a);
GEN  geval_gp(GEN x, GEN t);

GEN  gadde(GEN *x, GEN y);
GEN  gadd1e(GEN *x);
GEN  gdive(GEN *x, GEN y);
GEN  gdivente(GEN *x, GEN y);
GEN  gdivrounde(GEN *x, GEN y);
GEN  gmode(GEN *x, GEN y);
GEN  gmule(GEN *x, GEN y);
GEN  gshiftle(GEN *x, long n);
GEN  gshiftre(GEN *x, long n);
GEN  gsube(GEN *x, GEN y);
GEN  gsub1e(GEN *x);
GEN  gshift_right(GEN x, long n);

GEN  asympnum0(GEN u, long muli, GEN alpha, long prec);
GEN  derivnum0(GEN a, GEN code, long prec);
GEN  derivfun0(GEN code, GEN args, long prec);
GEN  direuler0(GEN a, GEN b, GEN code, GEN c);
void forcomposite(GEN a, GEN b, GEN code);
void fordiv(GEN a, GEN code);
void forell0(long a, long b, GEN code);
void forprime(GEN a, GEN b, GEN code);
void forstep(GEN a, GEN b, GEN s, GEN code);
void forsubgroup0(GEN cyc, GEN bound, GEN code);
void forvec(GEN x, GEN code, long flag);
void forpart0(GEN k, GEN code , GEN nbound, GEN abound);
GEN  intcirc0(GEN a, GEN R, GEN code, GEN tab, long prec);
GEN  intfuncinit0(GEN a, GEN b, GEN code, long m, long prec);
GEN  intnum0(GEN a, GEN b, GEN code, GEN tab, long prec);
GEN  intnumgauss0(GEN a, GEN b, GEN code, GEN tab, long prec);
GEN  intnumromb0(GEN a, GEN b, GEN code, long flag, long prec);
GEN  limitnum0(GEN u, long muli, GEN alpha, long prec);
GEN  matrice(GEN nlig, GEN ncol, GEN code);
GEN  prodeuler0(GEN a, GEN b, GEN code, long prec);
GEN  prodinf0(GEN a, GEN code, long flag, long prec);
GEN  produit(GEN a, GEN b, GEN code, GEN x);
GEN  somme(GEN a, GEN b, GEN code, GEN x);
GEN  sumalt0(GEN a, GEN code,long flag, long prec);
GEN  sumdivexpr(GEN num, GEN code);
GEN  sumdivmultexpr(GEN num, GEN code);
GEN  suminf0(GEN a, GEN code, long prec);
GEN  sumnum0(GEN a, GEN code, GEN tab, long prec);
GEN  sumnummonien0(GEN a, GEN code, GEN tab, long prec);
GEN  sumpos0(GEN a, GEN code, long flag,long prec);
GEN  vecexpr0(GEN nmax, GEN code, GEN pred);
GEN  vecexpr1(GEN nmax, GEN code, GEN pred);
GEN  vecteursmall(GEN nmax, GEN code);
GEN  vecteur(GEN nmax, GEN n);
GEN  vvecteur(GEN nmax, GEN n);
GEN  zbrent0(GEN a, GEN b, GEN code, long prec);
GEN  solvestep0(GEN a, GEN b, GEN step, GEN code, long flag, long prec);

/* mt */
void mt_sigint(void);
void mt_err_recover(long er);
void mt_init_stack(size_t s);
int  mt_is_thread(void);
GEN  parapply_worker(GEN d, GEN code);
GEN  pareval_worker(GEN code);
void parfor0(GEN a, GEN b, GEN code, GEN code2);
GEN  parfor_worker(GEN i, GEN C);
void parforprime0(GEN a, GEN b, GEN code, GEN code2);
void parforvec0(GEN a, GEN code, GEN code2, long flag);
GEN  parvector_worker(GEN i, GEN C);
GEN  polmodular_worker(ulong p, ulong t, ulong L, GEN hilb, GEN factu,
       GEN vne, GEN vinfo, GEN J, long compute_derivs, GEN j_powers, GEN fdb);
GEN  nmV_polint_center_tree_worker(GEN Va, GEN T, GEN R, GEN xa, GEN m2);

/* multiprecision */
GEN   addrex01(GEN x);
GEN   adduispec_offset(ulong s, GEN x, long offset, long nx);
int   lgcdii(ulong* d, ulong* d1, ulong* u, ulong* u1, ulong* v, ulong* v1, ulong vmax);
ulong rgcduu(ulong d, ulong d1, ulong vmax, ulong* u, ulong* u1, ulong* v, ulong* v1, long *s);
ulong xgcduu(ulong d, ulong d1, int f, ulong* v, ulong* v1, long *s);
ulong xxgcduu(ulong d, ulong d1, int f, ulong* u, ulong* u1, ulong* v, ulong* v1, long *s);
GEN   divgunu(GEN x, ulong i);
GEN   divrunu(GEN x, ulong i);
GEN   muliispec(GEN x, GEN y, long nx, long ny);
GEN   red_montgomery(GEN T, GEN N, ulong inv);
GEN   sqrispec(GEN x, long nx);
GEN   subrex01(GEN x);
GEN   modr_safe(GEN x, GEN y);
ulong *convi(GEN x, long *l);

int approx_0(GEN x, GEN y);
GEN bernfrac_using_zeta(long n);

/* powers */
GEN    rpowuu(ulong a, ulong n, long prec);
ulong  u_pow10(int n);

/* floats */
double dabs(double s, double t);
void   dcxlog(double s, double t, double *a, double *b);
double dnorm(double s, double t);
double dbllog2(GEN z);

/* hnf */
GEN hnfadd(GEN m,GEN p,GEN* ptdep,GEN* ptA,GEN* ptC,GEN extramat,GEN extraC);
GEN hnfadd_i(GEN m,GEN p,GEN* ptdep,GEN* ptA,GEN* ptC,GEN extramat,GEN extraC);
GEN hnfspec_i(GEN m,GEN p,GEN* ptdep,GEN* ptA,GEN* ptC,long k0);
GEN hnfspec(GEN m,GEN p,GEN* ptdep,GEN* ptA,GEN* ptC,long k0);
GEN mathnfspec(GEN x, GEN *ptperm, GEN *ptdep, GEN *ptB, GEN *ptC);
GEN ZM_hnfmodall_i(GEN x, GEN dm, long flag);

GEN LLL_check_progress(GEN Bnorm, long n0, GEN m, int final, long *ti_LLL);
GEN extendedgcd(GEN A);

/* miscellaneous linear algebra */
GEN  imagecomplspec(GEN x, long *nlze);
GEN  ZM_imagecomplspec(GEN x, long *nlze);
GEN  dim1proj(GEN prh);
GEN  detcyc(GEN cyc, long *L);

GEN merge_factor_i(GEN f, GEN g);

/* integer factorization / discrete log */
GEN   coprime_part(GEN x, GEN f);
ulong ucoprime_part(ulong x, ulong f);
ulong is_kth_power(GEN x, ulong p, GEN *pt);
GEN   mpqs(GEN N);
ulong gcduodd(ulong x, ulong y);

/* Polynomials */
/* a) Arithmetic/conversions */
GEN  addmulXn(GEN x, GEN y, long d);
GEN  addshiftpol(GEN x, GEN y, long d);
GEN  lift_if_rational(GEN x);
GEN  monomial(GEN a, long degpol, long v);
GEN  monomialcopy(GEN a, long degpol, long v);
GEN  mulmat_pol(GEN A, GEN x);
GEN  ser2pol_i(GEN x, long lx);
GEN  ser2rfrac_i(GEN x);
GEN  shiftpol_i(GEN x, long v);
GEN  swap_vars(GEN b0, long v);
GEN  RgX_recipspec_shallow(GEN x, long l, long n);

/* b) Modular */
GEN  bezout_lift_fact(GEN T, GEN Tmod, GEN p, long e);
long F2x_split_Berlekamp(GEN *t);
long Flx_split_Berlekamp(GEN *t, ulong p);
long FpX_split_Berlekamp(GEN *t, GEN pp);
long FqX_split_Berlekamp(GEN *t, GEN T, GEN p);
GEN  FpX_quad_root(GEN x, GEN p, int unknown);
GEN  FqX_split_all(GEN z, GEN T, GEN p);
long FqX_split_by_degree(GEN *pz, GEN u, GEN T, GEN p);
long FqX_split_deg1(GEN *pz, GEN u, GEN T, GEN p);
GEN  FqX_split_roots(GEN z, GEN T, GEN p, GEN pol);
GEN  polsym_gen(GEN P, GEN y0, long n, GEN T, GEN N);
GEN  ZXQ_charpoly_sqf(GEN A, GEN B, long *lambda, long v);
GEN  ZX_disc_all(GEN,ulong);
GEN  ZX_resultant_all(GEN A, GEN B, GEN dB, ulong bound);
GEN  ZX_ZXY_resultant_all(GEN A, GEN B, long *lambda, GEN *LPRS);
GEN  RgXQ_minpoly_naive(GEN y, GEN P);
GEN lift_intern(GEN x);

/* c) factorization */
GEN chk_factors_get(GEN lt, GEN famod, GEN c, GEN T, GEN N);
long cmbf_maxK(long nb);
GEN ZX_DDF(GEN x);
GEN initgaloisborne(GEN T, GEN dn, long prec, GEN *pL, GEN *pprep, GEN *pdis);
GEN logmax_modulus_bound(GEN p);
GEN polint_i(GEN xa, GEN ya, GEN x, long n, GEN *ptdy);
GEN quicktrace(GEN x, GEN sym);
GEN special_pivot(GEN x);
GEN vandermondeinversemod(GEN L, GEN T, GEN den, GEN mod);
GEN ZX_monic_factorpadic(GEN f, GEN p, long prec);

/* Finite fields */

enum { t_FF_FpXQ = 0, t_FF_Flxq = 1, t_FF_F2xq = 2 };
GEN FF_ellinit(GEN E, GEN fg);
GEN FF_elldata(GEN E, GEN fg);

/* L functions */
enum { t_LFUN_GENERIC, t_LFUN_ZETA, t_LFUN_NF, t_LFUN_ELL, t_LFUN_KRONECKER,
       t_LFUN_CHIVEC, t_LFUN_CHIGEN, t_LFUN_ETA, t_LFUN_DIV, t_LFUN_MUL,
       t_LFUN_SYMSQ, t_LFUN_SYMSQ_ELL, t_LFUN_QF };
enum { t_LDESC_INIT, t_LDESC_THETA, t_LDESC_PRODUCT };

/* Elliptic curves */
/* common to Q and Rg */
enum { R_PERIODS = 1, R_ETA, R_ROOTS, R_AB };

enum { Qp_ROOT = 1, Qp_TATE };
enum { Q_GROUPGEN = 5, Q_GLOBALRED, Q_ROOTNO, Q_MINIMALMODEL };

/* common to Fp and Fq */
enum { FF_CARD = 1, FF_GROUP, FF_GROUPGEN, FF_O };

/* for Buchall_param */
enum { fupb_NONE, fupb_RELAT, fupb_LARGE, fupb_PRECI };

/*
 * Represents the data in the equation(s)
 *
 *   4p = t^2 - v^2 D = t^2 - v^2 u^2 D_K = w^2 D_K.
 *
 * t is the absolute trace, so always > 0.
 * T is a twisting parameter, which satisfies (T|p) == -1.
 */
typedef struct {
  long D;
  long t, u, v;
  ulong p, pi;
  ulong T;
} norm_eqn_struct;
typedef norm_eqn_struct norm_eqn_t[1];

#define zv_to_longptr(v) (&((v)[1]))
#define zv_to_ulongptr(v) ((ulong *)&((v)[1]))

/* Isogeny volcanos */
long j_level_in_volcano(
  GEN phi, ulong j, ulong p, ulong pi, long L, long depth);
ulong ascend_volcano(
  GEN phi, ulong j, ulong p, ulong pi, long level, long L, long depth,
  long steps);
ulong descend_volcano(
  GEN phi, ulong j, ulong p, ulong pi, long level, long L, long depth,
  long steps);
long walk_surface_path(
  ulong path[], GEN phi, ulong p, ulong pi, long L, long depth,
  long max_len);
GEN enum_j_with_endo_ring(
  ulong j0, int endo_cert, norm_eqn_t ne, GEN db, GEN pcp, long max_elts);

/* Modular invariants */
#define INV_J 0
#define INV_F 1
#define INV_F2 2
#define INV_F3 3
#define INV_F4 4
#define INV_G2 5

long inv_level(long inv);
long inv_degree(long *p1, long *p2, long inv);
double inv_height_factor(long inv);
int inv_good_discriminant(long D, long inv);
int inv_good_prime(long p, long inv);
ulong modfn_root(ulong j, norm_eqn_t ne, long inv);

/* Internal polclass and polmodular functions, to allow sharing
 * modular polynomial cache */
GEN polclass0(long D, long inv, long xvar, GEN *db);
GEN polmodular0_ZM(
  long L, long inv, GEN J, GEN Q, int compute_derivs, GEN *db);


/* Polmodular database maintenance */
GEN polmodular_db_init(long inv);
void polmodular_db_clear(GEN db);
void polmodular_db_add_level(GEN *db, long L, long inv);
void polmodular_db_add_levels(GEN *db, GEN levels, long inv);
GEN polmodular_db_for_inv(GEN db, long inv);
GEN polmodular_db_getp(GEN fdb, long L, ulong p);

void random_curves_with_m_torsion(
  ulong *a4, ulong *a6, ulong *tx, ulong *ty,
  long ncurves, long m, ulong p);

/* Allocation / gerepile */
long   getdebugvar(void);
void   setdebugvar(long n);
void   debug_stack(void);
void   fill_stack(void);
void   init_dalloc(void);
double *dalloc(size_t n);
void   minim_alloc(long n, double ***q, GEN *x, double **y,  double **z, double **v);
int    pop_entree_block(entree *ep, long loc);
int    pop_val_if_newer(entree *ep, long loc);

/* general printing */
void print_errcontext(PariOUT *out, const char *msg, const char *s, const char *entry);
void print_prefixed_text(PariOUT *out, const char *s, const char *prefix, const char *str);
INLINE void
print_text(const char *s) { print_prefixed_text(pariOut, s,NULL,NULL); }
INLINE void
out_print_text(PariOUT *out, const char *s) { print_prefixed_text(out, s,NULL,NULL); }
INLINE long
is_keyword_char(char c) { return (isalnum((int)c) || c=='_'); }

/* Interfaces (GP, etc.) */
hashtable *hash_from_link(GEN e, GEN names, int use_stack);
void gen_relink(GEN x, hashtable *table);
entree* do_alias(entree *ep);
char* get_sep(const char *t);
long get_int(const char *s, long dflt);
ulong get_uint(const char *s);
int  gp_init_functions(void);
void gp_initrc(pari_stack *p_A);

void pari_sigint(const char *s);
pariFILE *pari_last_tmp_file(void);
void* get_stack(double fraction, long min);
void  init_graph(void);
void  free_graph(void);
void  initout(int initerr);
void  resetout(int initerr);
void  init_linewrap(long w);
void  pari_kernel_init(void);
void  pari_kernel_close(void);
void  print_functions_hash(const char *s);
void  print_all_user_fun(int member);
GEN   readbin(const char *name, FILE *f, int *vector);
int   term_height(void);
int   term_width(void);
void  whatnow_new_syntax(const char *f, long n);
/* gp_colors */
void decode_color(long n, long *c);

/* defaults */
extern long precreal;

void lim_lines_output(char *s, long n, long max);
int tex2mail_output(GEN z, long n);
void gen_output(GEN x, pariout_t *T);
void fputGEN_pariout(GEN x, pariout_t *T, FILE *out);

void parsestate_reset(void);
void parsestate_save(struct pari_parsestate *state);
void parsestate_restore(struct pari_parsestate *state);

void compilestate_reset(void);
void compilestate_save(struct pari_compilestate *comp);
void compilestate_restore(struct pari_compilestate *comp);

void evalstate_clone(void);
void evalstate_reset(void);
void evalstate_restore(struct pari_evalstate *state);
GEN  evalstate_restore_err(struct pari_evalstate *state);
void evalstate_save(struct pari_evalstate *state);
void varstate_save(struct pari_varstate *s);
void varstate_restore(struct pari_varstate *s);

void mtstate_save(long *pending);
void mtstate_reset(void);
void mtstate_restore(long *pending);

void debug_context(void);

typedef struct {
  const char *s;
  size_t ls;
  char **dir;
} forpath_t;
void forpath_init(forpath_t *T, gp_path *path, const char *s);
char *forpath_next(forpath_t *T);

/* GP output && output format */
void gpwritebin(const char *s, GEN x);
extern char *current_logfile;

/* colors */
extern long    gp_colors[];
extern int     disable_color;

/* entrees */
#define EpVALENCE(ep) ((ep)->valence & 0xFF)
#define EpSTATIC(ep) ((ep)->valence & 0x100)
#define EpSETSTATIC(ep) ((ep)->valence |= 0x100)
enum { EpNEW = 100, EpALIAS, EpVAR, EpINSTALL };
#define initial_value(ep) ((ep)+1)

/* functions lists */
extern const long functions_tblsz;  /* hashcodes table size */
extern entree **functions_hash;   /* functions hashtable */
extern entree **defaults_hash;    /* defaults hashtable */

/* buffers */
typedef struct Buffer {
  char *buf;
  ulong len;
  jmp_buf env;
} Buffer;
Buffer *new_buffer(void);
void delete_buffer(Buffer *b);
void fix_buffer(Buffer *b, long newlbuf);

typedef struct {
  const char *s; /* source */
  char *t, *end; /* target, last char read */
  int in_string, in_comment, more_input, wait_for_brace;
  Buffer *buf;
} filtre_t;
void init_filtre(filtre_t *F, Buffer *buf);
Buffer *filtered_buffer(filtre_t *F);
const char *break_loop_prompt(long n);
void kill_buffers_upto_including(Buffer *B);
void pop_buffer(void);
void kill_buffers_upto(Buffer *B);
int gp_read_line(filtre_t *F, const char *PROMPT);
void parse_key_val(char *src, char **ps, char **pt);
void pari_init_buffers(void);
extern int (*cb_pari_get_line_interactive)(const char*, const char*, filtre_t *F);
extern char *(*cb_pari_fgets_interactive)(char *s, int n, FILE *f);
int get_line_from_file(const char *prompt, filtre_t *F, FILE *file);
void pari_skip_space(char **s);
void pari_skip_alpha(char **s);
char *pari_translate_string(const char *src, char *s, char *entry);

gp_data *default_gp_data(void);

void delete_dirs(gp_path *p);
void gp_expand_path(gp_path *p);
const char *pari_default_path(void);
int path_is_absolute(char *s);

typedef struct input_method {
/* mandatory */
  char * (*fgets)(char *,int,FILE*);
  char * (*getline)(char**, int f, struct input_method*, filtre_t *F);
  int free; /* boolean: must we free the output of getline() ? */
/* for interactive methods */
  const char *prompt, *prompt_cont;
/* for non-interactive methods */
  FILE *file;
} input_method;

int input_loop(filtre_t *F, input_method *IM);
char *file_input(char **s0, int junk, input_method *IM, filtre_t *F);
char *file_getline(Buffer *b, char **s0, input_method *IM);

/* By files */

/* Qfb.c */

GEN     redimagsl2(GEN q, GEN *U);
GEN     redrealsl2(GEN V, GEN d, GEN rd);
GEN     redrealsl2step(GEN A, GEN d, GEN rd);
GEN     redtausl2(GEN t, GEN *U);

/* alglin1.c */
typedef long (*pivot_fun)(GEN,GEN,long,GEN);
GEN ZM_pivots(GEN x0, long *rr);
GEN RgM_pivots(GEN x0, GEN data, long *rr, pivot_fun pivot);

/* arith1.c */

int     is_gener_Fp(GEN x, GEN p, GEN p_1, GEN L);
int     is_gener_Fl(ulong x, ulong p, ulong p_1, GEN L);

/* arith2.c */

int     divisors_init(GEN n, GEN *pP, GEN *pE);
long    set_optimize(long what, GEN g);

/* base2.c */

GEN     gen_if_principal(GEN bnf, GEN x);
GEN     polsymmodp(GEN g, GEN p);
GEN     nfbasis_gp(GEN T);

/* base3.c */

void    check_nfelt(GEN x, GEN *den);
GEN     zk_ei_mul(GEN nf, GEN x, long i);

/* base4.c */

void    check_listpr(GEN x);
GEN     extideal_HNF_mul(GEN nf, GEN x, GEN y);
GEN     factor_norm(GEN x);
GEN     factorbackprime(GEN nf, GEN L, GEN e);
long    val_norm(GEN x, GEN p, long *vz);

/* base5.c */

GEN     check_and_build_nfabs(GEN rnf, long prec);

/* buch1.c */

GEN     form_to_ideal(GEN x);
GEN     qfbforms(GEN D);

/* buch2.c */

typedef struct GRHprime_t { ulong p; double logp; GEN dec; } GRHprime_t;
typedef struct GRHcheck_t { double cD, cN; GRHprime_t *primes; long clone, nprimes, maxprimes; ulong limp; forprime_t P; } GRHcheck_t;
void    free_GRHcheck(GRHcheck_t *S);
void    init_GRHcheck(GRHcheck_t *S, long N, long R1, double LOGD);
void    GRH_ensure(GRHcheck_t *S, long nb);
ulong   GRH_last_prime(GRHcheck_t *S);
int     GRHok(GRHcheck_t *S, double L, double SA, double SB);
GEN     check_and_build_matal(GEN bnf);
GEN     extract_full_lattice(GEN x);
GEN     init_red_mod_units(GEN bnf, long prec);
GEN     isprincipalarch(GEN bnf, GEN col, GEN kNx, GEN e, GEN dx, long *pe);
GEN     red_mod_units(GEN col, GEN z);

/* buch3.c */

GEN     minkowski_bound(GEN D, long N, long r2, long prec);
int     subgroup_conductor_ok(GEN H, GEN L);
GEN     subgrouplist_cond_sub(GEN bnr, GEN C, GEN bound);

/* ellsea.c */

void    pari_close_seadata(void);
void    pari_init_seadata(void);

/* es.c */

const char * eng_ord(long i);
char *  env_ok(const char *s);
void    filestate_restore(pariFILE *F);
void    killallfiles(void);
pariFILE* newfile(FILE *f, const char *name, int type);
void    pari_init_homedir(void);
void    pari_close_homedir(void);
void    pari_init_files(void);
void    pari_close_files(void);
int     popinfile(void);
pariFILE* try_pipe(const char *cmd, int flag);

/* Flx.c */

GEN FlxqM_mul_Kronecker(GEN A, GEN B, GEN T, ulong p);

/* Flxq_log.c */

GEN Flxq_log_index(GEN a0, GEN b0, GEN m, GEN T0, ulong p);
int Flxq_log_use_index(GEN m, GEN T0, ulong p);

/* FlxqE.c */

GEN     ZpXQ_norm_pcyc(GEN x, GEN T, GEN q, GEN p);
long    zx_is_pcyc(GEN T);

/* galconj.c */

GEN     galoiscosets(GEN O, GEN perm);
long    intheadlong(GEN x, GEN mod);
GEN     listznstarelts(long m, long p);
GEN     matheadlong(GEN W, GEN mod);
GEN     matrixnorm(GEN M, long prec);
long    polheadlong(GEN P, long n, GEN mod);
GEN     vandermondeinverseprep(GEN L);

/* galois.c */

GEN     polgaloisnamesbig(long n, long k);

/* gen1.c */

int     ff_poltype(GEN *x, GEN *p, GEN *pol);
GEN     gred_frac2(GEN x1, GEN x2);
GEN     gred_rfrac2(GEN x1, GEN x2);
GEN     gred_rfrac_simple(GEN n, GEN d);
GEN     sqr_ser_part(GEN x, long l1, long l2);

/* gen3.c */

GEN     gsubst_expr(GEN pol, GEN from, GEN to);
GEN     poltoser(GEN x, long v, long prec);
GEN     rfractoser(GEN x, long v, long prec);

/* hyperell.c */

GEN     ZlXQX_hyperellpadicfrobenius(GEN H, GEN T, ulong p, long n);

/* ifactor1.c */

GEN     ellfacteur(GEN n, int insist);
GEN     pollardbrent(GEN n);
ulong   snextpr(ulong p, byteptr *d, long *rcn, long *q, long k);
GEN     squfof(GEN n);

/* intnum.c */

GEN     contfraceval_inv(GEN CF, GEN tinv, long nlim);

/* prime.c */

long    BPSW_psp_nosmalldiv(GEN N);
int     Fl_MR_Jaeschke(ulong n, long k);
int     MR_Jaeschke(GEN n, long k);
long    isanypower_nosmalldiv(GEN N, GEN *px);
void    prime_table_next_p(ulong a, byteptr *pd, ulong *pp, ulong *pn);
int     uisprime_101(ulong n);
int     uisprime_661(ulong n);

/* init.c */

void    pari_init_defaults(void);

/* nffactor.c */

int     nfissplit(GEN nf, GEN x);

/* perm.c */

long    cosets_perm_search(GEN C, GEN p);
GEN     group_export_GAP(GEN G);
GEN     group_export_MAGMA(GEN G);
GEN     perm_generate(GEN S, GEN H, long o);
long    perm_relorder(GEN p, GEN S);
GEN     perm_to_GAP(GEN p);

/* polarit1.c */

GEN     F2x_Berlekamp_ker(GEN u);
GEN     Flx_Berlekamp_ker(GEN u, ulong p);
GEN     FpX_Berlekamp_ker(GEN u, GEN p);
GEN     FlxqX_Berlekamp_ker(GEN u, GEN T, ulong p);
GEN     FpXQX_Berlekamp_ker(GEN u, GEN T, GEN p);
GEN     F2x_factcantor(GEN f, long flag);
GEN     Flx_factcantor(GEN f, ulong p, long flag);
GEN     FpX_factcantor(GEN f, GEN pp, long flag);
GEN     FqX_rand(long d1, long v, GEN T, GEN p);
int     cmp_padic(GEN x, GEN y);
GEN     factcantor0(GEN f, GEN pp, long flag);

/* polarit2.c */

GEN     sylvestermatrix_i(GEN x, GEN y);

/* QX_factor */

void    factor_quad(GEN x, GEN res, long *ptcnt);

/* FpX.c */

ulong   Flj_order_ufact(GEN P, ulong n, GEN F, ulong a4, ulong p, ulong pi);

/* FpX.c */

GEN     FpX_gcd_check(GEN x, GEN y, GEN p);

/* polarit3.c */

GEN     Flm_Frobenius_pow(GEN M, long d, GEN T, ulong p);
GEN     FpM_Frobenius_pow(GEN M, long d, GEN T, GEN p);
GEN     FpX_compositum(GEN A, GEN B, GEN p);
GEN     FpX_direct_compositum(GEN A, GEN B, GEN p);
ulong   ZX_ZXY_ResBound(GEN A, GEN B, GEN dB);
GEN     ffinit_Artin_Shreier(GEN ip, long l);
GEN     ffinit_rand(GEN p, long n);
void    init_modular(forprime_t *S);
GEN     polint_triv(GEN xa, GEN ya);

/* random.c */

void    pari_init_rand(void);

/* rootpol.c */

GEN     FFT(GEN x, GEN Omega);
GEN     FFTinit(long k, long prec);

/* subcyclo.c */

GEN     bnr_to_znstar(GEN bnr, long *complex);
GEN     galoiscyclo(long n, long v);
GEN     znstar_bits(long n, GEN H);
long    znstar_conductor(long n, GEN H);
GEN     znstar_cosets(long n, long phi_n, GEN H);
GEN     znstar_elts(long n, GEN H);
GEN     znstar_generate(long n, GEN V);
GEN     znstar_hnf(GEN Z, GEN M);
GEN     znstar_hnf_elts(GEN Z, GEN H);
GEN     znstar_hnf_generators(GEN Z, GEN M);
GEN     znstar_reduce_modulus(GEN H, long n);
GEN     znstar_small(GEN zn);

/* trans1.c */

GEN     logagmcx(GEN q, long prec);
void    pari_init_floats(void);
void    pari_close_floats(void);
GEN     rootsof1complex(GEN n, long prec);
GEN     rootsof1padic(GEN n, GEN y);
GEN     zellagmcx(GEN a0, GEN b0, GEN r, GEN t, long prec);

/* trans2.c */

GEN     cxpsi(GEN s0, long prec);
double  darg(double s, double t);

/* trans3.c */

GEN     bernreal_using_zeta(long n, GEN iz, long prec);
GEN     czeta(GEN s0, long prec);
GEN     double_eta_quotient(GEN a, GEN w, GEN D, long p, long q, GEN pq, GEN sqrtD);
GEN     inv_szeta_euler(long n, double lba, long prec);
GEN     polylogd0(long m, GEN x, long flag, long prec);
GEN     trueE2(GEN tau, long prec);
GEN     twistpartialzeta(GEN q, long f, long c, GEN va, GEN cff);

ENDEXTERN
