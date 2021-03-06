bool   FCLOSE;
 int   PAGE_HEIGHT;
 int   PAGE_WIDTH;
 int   SCALE_X;
 int   SCALE_Y;
 int   BM_W;
 int   BM_H;

 bool  LICKEY;

 bool VERSION;
 int COL1,COL2,COL3,COL4,COL5,COL6,COL7,COL8,COL9,COL10,COL11,COL12,COL13,COL14,COL15;
 int COL16,COL17,COL18,COL19,COL20;
 char SEPCHAMPS;
 char SEPPRED;

 char *m_modcrit;
 char *m_coche;
 char *m_filtre;
 char *m_project;
 char *m_exchange;
 char *m_exchange2;
 char *m_ecoplan;
 char *m_menu;
 char *DatabaseName;
 char *m_pasapas;
 char *m_criteres;
 char *m_sender;
 char *m_params;
 char *m_sort;
 char *m_tridetail;
 char *m_reports;
 char *m_origin;
 char *m_directory;
 char *m_message;
 char *m_planning;
 char *m_rapport;
 char *m_assign;
 char *m_memoriser;
 char *m_datefinreelle;

 char *m_saveAV;
 char *m_help;
 char *m_dateprev;
 char *m_simul;
 char *m_savesimul;
 char *m_renum;
 char *m_status;
 char *m_excel;
 char *m_importtaches;
 char *m_importliaisons;
 char *m_importcriteres;
 char *m_save;
 char *m_mode;
 char *m_chcrit;
 char *m_impt;
 char *m_resize;
 char *m_affect_crit;
 char *m_zero;
 //char *m_renum;

 char SAVEAS[MAX_PATH];

 bool FIRST_AVANC;
 char FILE_AVANC_DELETED[1000];
 bool AVANC_DEL;
 bool SAISIE_AVANC;

 Graphics::TBitmap *im;
 Graphics::TBitmap *imP;
 Graphics::TBitmap *imR;
 Graphics::TBitmap *img;

 AnsiString DTFINPREV,DTFINREEL;

 char  dateavanc[5000];  // separes par des @
 char  lastfile[400];
 TStringList *TSL;
 TStringList *TSLCRIT;

 int C_BRISEES[10][20];
 float    E_BRISEES[10][20];
 bool   L_BRISEES[10][20];

 char TITRE[350];
 char OBSERV[2000];

 float TOTAL_POS;
 float TOTAL_NEG;

 int LASTTASK;

 char   LastAppli[250];
 char   LastPlanning[6][250];
 AnsiString m_ghost_prep[6];

 bool RELOADTRI[8];
 bool RELOADFILTRE[8];
 bool RELOADCOL[8];


int    PAGE_RAPPORT[8];    // Page en cours pour les rapports
int    MAX_PAGE_RAPPORT[8];
//int    MARQUEUR[2][200];    // marqueur pour les 2 rapports
//int    NEWY[2][200];        // nouveau Y de base
//int    ITEM_ENTETE[2];
//int    Y_ENTETE[2];

 char       SPAGE[5000];    // image des pages de rapport
 int        Y[5000];
 int        TCH[5000];
 AnsiString T[5000];
 int        INDXPAGE;


 char   AUTOMATE[6];   // memorise chargement plannings et rapport (origine)
 //char   LastRapport[250];
 char   LastTri[250];
 char   LastCol[250];
 char   LastFilter[250];
 char   HelpDirectory[MAX_PATH];
 char   ProjectDirectory[MAX_PATH];

 char   LocalBase[MAX_PATH];
 char   ProjectName[MAX_PATH];
 int Numt[3000];
 AnsiString Design[3000];
 int Duree[3000];
 AnsiString Deb[3000];
 AnsiString Fin[3000];

 AnsiString Criter[3000][8];
 int DistDeb [3000];
 int DistFin[3000];
 char P0[50],P1[50],P2[50],P3[50];
 char BRUPT[5]; 
 char m_automatic[20];

// CONSTANTES   : COEF = correspondance cm -> Pixels

#define COEF    100
#define COEFMM  10


// Liste des Criteres

AnsiString CRITERES[10];
AnsiString LIB_CRITERES[9];

int     NB_TACHES;


#define MAX_T  3010
#define PS     300

#define NS     200
#define NN     300


int   niveaux[NN][MAX_T];
int   simul[MAX_T];
#define NB_CAL 5

int   duree[MAX_T];
int   duree_reelle[MAX_T];
char  flag_souhaites[MAX_T+10];

//char  libel[MAX_T][80];

int   pr[MAX_T][PS]; int su[MAX_T][PS];
int   pr_tl[MAX_T][PS]; int su_tl[MAX_T][PS];
int   pr_decal[MAX_T][PS]; int su_decal[MAX_T][PS];
int   deb_plus_tot[MAX_T];
int   fin_plus_tot[MAX_T];
int   deb_plus_tard[MAX_T];
int   fin_plus_tard[MAX_T];



int   vrai_deb_plus_tot[MAX_T];
int   vrai_fin_plus_tot[MAX_T];

int   retard[MAX_T];

char  coche[MAX_T];
char  coch[MAX_T];

char  filtint[MAX_T];  //  sert aux calculs intermediaires



char codetache[MAX_T][30];

char  da[MAX_T][11]; // date avancement format YYYY-MM-DD
char  dtr[MAX_T][11]; // date debut reel
char  ftr[MAX_T][11]; // date fin reelle
int   pca[MAX_T];     //  Pourcentage (restant) en avancement
int   nbj[MAX_T];      // nbjours restant
char  tmom[MAX_T][11];      // code tache amont
int   tcal[MAX_T+1];    //  calendriers affectes aux taches;
int   marg_tot[MAX_T];  // marge totale
int   marg_lib[MAX_T];  // marge libre
char  dds[MAX_T][11];    // date debut souhaite
char  dfs[MAX_T][11];    // date fin souhait�e

int   sommets[MAX_T+1];


int   selection[MAX_T];

int   t_select[MAX_T];
int   t_selectf[MAX_T];
int   t_pred[MAX_T][PS];

int   renum[MAX_T];


#define MAX_PERIOD 100
AnsiString DebPer[MAX_PERIOD];
AnsiString FinPer[MAX_PERIOD];
int d1[MAX_PERIOD],d2[MAX_PERIOD];
int  DurPer[MAX_PERIOD];
int  PerPer[MAX_PERIOD];
int  TDuree[MAX_PERIOD];    // pour chaque calcul de tache
int  NbjtPer[MAX_PERIOD];
int  NbPer;




#define MAX_JOURS 8000
char   cal[NB_CAL+1][MAX_JOURS+1]; // 10 ans
char   weeks[NB_CAL+1][30];




 // Criteres et champs
#define MAX_CHAMPS  100
#define MAX_PLAN 7

int NUM_CHAMP[MAX_PLAN][MAX_CHAMPS];
//char LIBEL_CHAMP[MAX_PLAN][MAX_CHAMPS][50];

AnsiString CommentPL[8];


int  pc;  // planning_en_cours  de 1 � 5)
int  rc;  // rapport en cours



//  Planning : 0-1-2
//  Rapport  : 3-4

bool P_PLANNING[MAX_PLAN];  // Plannings ou rapports initialis�s

char P_NOM[MAX_PLAN][200];   // Nom du Planning en cours
char P_GENRE[MAX_PLAN]; // compresse ou gantt
char P_TYPE[MAX_PLAN];  // prev ou prev+reel

char *P_TRI[MAX_PLAN];       // tri associe
char *P_FILTRE[MAX_PLAN];     // filtre associ�
char *P_COLONNES[MAX_PLAN];   // colonnes associees

int NB_ITEMS_TRI[MAX_PLAN];
int NB_ITEMS_FILT[MAX_PLAN];
int NB_ITEMs_COL[MAX_PLAN];

int  NB_DETAIL[MAX_PLAN];   // Nb lignes detail (nb taches)

char P_MODE[MAX_PLAN];   // Mode P ortrait ou L andscape
int  P_H[MAX_PLAN],P_W[MAX_PLAN];  // Hauteur et Largeur Impression

// MARGES
int  P_MARGH1[MAX_PLAN],P_MARGH2[MAX_PLAN],P_MARGV1[MAX_PLAN],P_MARGV2[MAX_PLAN]; // Marges en Pixels
AnsiString  P_MH1[MAX_PLAN],P_MH2[MAX_PLAN],P_MV1[MAX_PLAN],P_MV2[MAX_PLAN]; // Marges en mm

char P_MARGES[MAX_PLAN];  // O/N
char P_CADRE[MAX_PLAN];  // O / N
char P_ENTETE[MAX_PLAN];   // O/N
char P_CALHAUT[MAX_PLAN];   // O/N calendrier Haut
char P_CALBAS[MAX_PLAN];   // O/N Calendrier bas
char P_PIED[MAX_PLAN];   // O/N
char P_MPENTETE[MAX_PLAN];
char P_MPPIED[MAX_PLAN];

char P_GRILLE[MAX_PLAN];
char P_LBRISE[MAX_PLAN];

// ENVELOPPE
/*
AnsiString P_ENV_COUL[MAX_PLAN];
AnsiString P_ENV_EPAIS[MAX_PLAN];
AnsiString P_ENV_STYLE[MAX_PLAN];
*/

// CADRE
AnsiString  P_ELC[MAX_PLAN];    // epaisseur  du cadre
AnsiString  P_CLC[MAX_PLAN];  // Couleur du cadre
int P_ELCF[MAX_PLAN];
int P_CLCC[MAX_PLAN];


// LOGO

AnsiString LOGONAME[MAX_PLAN];
AnsiString LOGOPOS[MAX_PLAN];


// Ruptures sur criteres
int P_RUPT[MAX_PLAN][8];

int        NUM_CRIT[MAX_PLAN];  // dernier critere de rupture
#define MAX_RUPT 200
AnsiString RUPT[MAX_RUPT];
int        NB_TASK[9][MAX_RUPT];
int        CTR_NB_TASK[9][MAX_RUPT];

int        ENV_MIN[9][MAX_RUPT];
int        ENV_MAX[9][MAX_RUPT];
int        ENV_DUR[MAX_RUPT];
int        ENV_D1[9][MAX_RUPT];
int        ENV_D2[9][MAX_RUPT];


// description des colonnes
int P_COL[MAX_PLAN]; // nb de colonnes (champs)
int P_DETAIL[MAX_PLAN];     // Detail dans TRI
#define MAX_COL 50

int P_TRICOL[MAX_PLAN][MAX_COL];   // numero de colonne dans ASTRI
char P_ATTR[MAX_PLAN][MAX_COL];   // attribut 'L' ou 'C'


int P_NIV0[MAX_PLAN][MAX_COL];  // Sous Niveau pour le tri
int P_NIV1[MAX_PLAN][MAX_COL];
int P_NIV2[MAX_PLAN][MAX_COL];
int P_NIV3[MAX_PLAN][MAX_COL];

int  P_ENV[MAX_PLAN][MAX_COL];


AnsiString P_FONT[MAX_PLAN][MAX_COL];  // Nom de la fonte  dans la colonne
int  P_HAUT[MAX_PLAN][MAX_COL];   // Hauteur du texte dans la colonne
int  P_LARG[MAX_PLAN][MAX_COL];

AnsiString P_ITAL[MAX_PLAN][MAX_COL];    // Italic / Normal
AnsiString  P_BOLD[MAX_PLAN][MAX_COL];     // Bold / Normal
AnsiString P_JV[MAX_PLAN][MAX_COL];   //  justification V  'H,C,ou B'
AnsiString P_JH[MAX_PLAN][MAX_COL];   //  justification H  'G,C,D'


int  P_CFCOLOR[MAX_PLAN][MAX_COL];  // couleur Texte des colonnes
int  P_CBCOLOR[MAX_PLAN][MAX_COL];  // couleur Background

AnsiString T_TITRE_COL[MAX_PLAN][50];  // Titre des colonnes
AnsiString T_FONT[MAX_PLAN][MAX_COL];  // Nom de la fonte  dans la colonne
int  T_HAUT[MAX_PLAN][MAX_COL];   // Hauteur du texte dans la colonne
int  T_LARG[MAX_PLAN][MAX_COL];

AnsiString  T_ITAL[MAX_PLAN][MAX_COL];    // Italic / Normal
AnsiString  T_BOLD[MAX_PLAN][MAX_COL];     // Bold / Normal
AnsiString T_JV[MAX_PLAN][MAX_COL];   //  justification V  'H,C,ou B'
AnsiString T_JH[MAX_PLAN][MAX_COL];   //  justification H  'G,C,D'

int  T_CFCOLOR[MAX_PLAN][MAX_COL];  // couleur Texte des colonnes
int  T_CBCOLOR[MAX_PLAN][MAX_COL];  // couleur Background

char    LSH[MAX_PLAN][MAX_COL];    // LSep Haute Oui/Non
TColor  LSH_COLOR[MAX_PLAN][MAX_COL];   // couleur ligne
int     LSH_STYLE[MAX_PLAN][MAX_COL];   // style ligne
int     LSH_EPAISSEUR[MAX_PLAN][MAX_COL];  // Epaisseur
char    LSB[MAX_PLAN][MAX_COL];    // LSep Basse Oui/Non
TColor  LSB_COLOR[MAX_PLAN][MAX_COL];   // couleur ligne
int     LSB_STYLE[MAX_PLAN][MAX_COL];   // style ligne
int     LSB_EPAISSEUR[MAX_PLAN][MAX_COL];  // Epaisseur

// Variable calcul�es
int P_LC[MAX_PLAN][MAX_COL];   // adresse x1;
int  P_MAXSIZE[MAX_PLAN][MAX_COL];  // Taille du texte le plus long dans une colonne
int  P_EPAISSV[MAX_PLAN][MAX_COL];  // Epaisseur du  trait de la colonne "a gauche"
int P_LENGTH[MAX_PLAN][MAX_COL];    // calcul�e d'apres la long maximum du champ
int T_LC[MAX_PLAN][MAX_COL];   // adresse x1;
int T_LENGTH[MAX_PLAN][MAX_COL];

TColor P_ET_BKG[MAX_PLAN];  // Background pour entete
TColor P_PD_BKG[MAX_PLAN]; // background pour pied

int  P_DEBUTGANTT[MAX_PLAN];   // debut du gantt et du calendrier
int  P_DEBUTCOL[MAX_PLAN];

// Caracteristiques des Taches Prev Critique,non critiques, reelles crit et non critiques

#define MT 5
//  1 = Prev Crit  2=Prev Non Crit  3=Reel Crit  4 Reel Non Crit
AnsiString P_TRAIT[MAX_PLAN][MT];      // couleur du trait
AnsiString P_EPTRAIT[MAX_PLAN][MT];    // epaisseur
AnsiString P_LIENS[MAX_PLAN][MT];      // liens O/N
AnsiString P_MARGE[MAX_PLAN][MT];      // Marge O/N
AnsiString P_STYLEM[MAX_PLAN][MT];     // Style marge
AnsiString P_COLMARGE[MAX_PLAN][MT];   // Couleur Trait marge

// Texte Haut Bas Droite Gauche
AnsiString P_INF[MAX_PLAN][MT][4];      // O/N
AnsiString P_INFO[MAX_PLAN][MT][4];     // Champs a afficher
AnsiString P_POLT[MAX_PLAN][MT][4];      // Police
AnsiString P_COLTXTF[MAX_PLAN][MT][4];   // couleur texte
AnsiString P_COLTXTB[MAX_PLAN][MT][4];   // couleur de fond
AnsiString P_ITALT[MAX_PLAN][MT][4]; //  Italique
AnsiString P_BOLDT[MAX_PLAN][MT][4]; //  Gras
AnsiString P_SIZET[MAX_PLAN][MT][4]; //  Taille texte

AnsiString P_STYLELIEN[MAX_PLAN][MT];    // Style Lien
AnsiString P_COL_LIEN[MAX_PLAN][MT];     // couleur Lien
AnsiString P_EPAIS_LIEN[MAX_PLAN][MT];   // Epaisseur lien

// CALENDRIER
int H_CAL1[MAX_PLAN],H_CAL2[MAX_PLAN];
AnsiString P_FORMATCAL1[MAX_PLAN], P_FORMATCAL2[MAX_PLAN]; // Format
AnsiString P_FREQ1[MAX_PLAN], P_FREQ2[MAX_PLAN];          // frequence
AnsiString P_UNIT1[MAX_PLAN],P_UNIT2[MAX_PLAN];   // Unite
AnsiString P_ECHH[MAX_PLAN];       // Echelle haute
AnsiString P_ECHB[MAX_PLAN];       // Echelle Basse

// PIED DE DOCUMENTS   // Voir Plus Bas    Indice 25 a 33

int H_COPYRIGHT[MAX_PLAN];

 #define  MAX_TEXTE   150
 /*
 Descriptif des Entete, Calendrier, Style ligne detail
 Gauche 1  = 0  Gauche 2  = 1   Gauche 3  = 2;
 Centre 1  = 3;  Centre 2  = 4   Centre 3  = 5
 Droite 1  = 6   Droite 2  = 7    Droite 3  = 8
 Calendrier 1 = 9;   Calendrier 2 = 10;
 Crit 0  (en ligne) = 11;   + Affect LS
 Crit 1  (en colonne = 12;  Crit 2  = 13  Crit 3  = 14  Crit 4  = 15
 Crit 5  = 16  Crit 6  = 17  Crit 7  = 18
 Descriptif texte des colonnes = 20
 Libelle Taches = 21
 Pied de Documents
 25 Gauche 1 26 Gauche 2  27=Gauche 3
 28 29 30
 31 32 33
*/

 AnsiString PACTIF[MAX_PLAN][MAX_TEXTE];  //  actif ou non, surtout pour les criteres 3..4..5..6..
 char       P_ORIENT[MAX_PLAN][MAX_TEXTE];  // En Ligne = true, en colonne = false
 AnsiString PT[MAX_PLAN][MAX_TEXTE];    // Texte
 TColor     PC[MAX_PLAN][MAX_TEXTE];    // Couleur Foreground
 TColor     PB[MAX_PLAN][MAX_TEXTE];    // backgroung
 AnsiString PF[MAX_PLAN][MAX_TEXTE];    // Fonte Name
 int        PH[MAX_PLAN][MAX_TEXTE];    // size
 int        PHH[MAX_PLAN][MAX_TEXTE];   // hauteur totale
 AnsiString PN[MAX_PLAN][MAX_TEXTE];    // italique I normal =  N
 AnsiString PX[MAX_PLAN][MAX_TEXTE];    // bold  = B ou Regulier  N
 AnsiString PJH[MAX_PLAN][MAX_TEXTE];   // just H
 AnsiString PJV[MAX_PLAN][MAX_TEXTE];   // just V

 int        PR[MAX_PLAN][MAX_TEXTE];    // Retrait;
 char       PTYPE[MAX_PLAN][MAX_TEXTE];  // type impression pour crit : Ligne ou Colonne






 // Description CORPS

 #define CHOIX 9
 AnsiString P_CYN[MAX_PLAN][CHOIX];
 AnsiString P_CCFG[MAX_PLAN][CHOIX];
 //AnsiString P_CCBK[MAX_PLAN][CHOIX];
 AnsiString P_CCST[MAX_PLAN][CHOIX];
 AnsiString P_CCEP[MAX_PLAN][CHOIX];

 AnsiString P_CYNJT[MAX_PLAN];
 AnsiString P_CYNJNT[MAX_PLAN];
 AnsiString P_CYNWENT[MAX_PLAN];

 AnsiString P_CJTFLAG[MAX_PLAN];
 AnsiString P_CJT[MAX_PLAN];
 AnsiString P_CJNT[MAX_PLAN];
 AnsiString P_CWENT[MAX_PLAN];
 AnsiString P_CDH[MAX_PLAN];
 AnsiString P_CDV[MAX_PLAN];

 AnsiString P_CCJT[MAX_PLAN];
 AnsiString P_CCJNT[MAX_PLAN];
 AnsiString P_CCWENT[MAX_PLAN];




 // Variables intermediaires de calcul
int memy1[MAX_PLAN],memy2[MAX_PLAN];
int P_MAXDIST[MAX_PLAN];

int P_CDVV[MAX_PLAN];

int P_DECAL[MAX_PLAN];
int P_LASTTEXT[MAX_PLAN];
int P_LST[MAX_PLAN];

int LAST_RX1[MAX_PLAN];
int LAST_RX2[MAX_PLAN];
int LAST_RY1[MAX_PLAN];
int LAST_RY2[MAX_PLAN];
int LAST_ZOOM[MAX_PLAN];
char LAST_TYPE[MAX_PLAN];

int LAST_X1;
int LAST_X2;
int LAST_Y1;
int LAST_Y2;
int LAST_TYP;
int LAST_ZOM;

int  D_LEFT[MAX_PLAN];
int  D_RIGHT[MAX_PLAN];
int  D_TOP[MAX_PLAN];
int  D_BOTTOM[MAX_PLAN];

//  Variables pour TRIS

#define MAX_TRI 8
AnsiString S_CRIT[MAX_TRI][4];

#define MAX_TCHP 8

AnsiString  T_LLCC[MAX_PLAN][MAX_TCHP];  // valeur L ou C  ou B ou T
AnsiString  T_POL[MAX_PLAN][MAX_TCHP]; // police
AnsiString  T_CTEXT[MAX_PLAN][MAX_TCHP]; // couleur Texte
AnsiString  T_TITAL[MAX_PLAN][MAX_TCHP];
AnsiString  T_TBOLD[MAX_PLAN][MAX_TCHP];
AnsiString  T_HAUTEUR[MAX_PLAN][MAX_TCHP];
AnsiString  T_JSH[MAX_PLAN][MAX_TCHP];
AnsiString  T_JSV[MAX_PLAN][MAX_TCHP];
AnsiString  T_CBKG[MAX_PLAN][MAX_TCHP];
AnsiString  T_TLARG[MAX_PLAN][MAX_TCHP];

AnsiString  T_LS1[MAX_PLAN][MAX_TCHP];
AnsiString  T_LS1EP[MAX_PLAN][MAX_TCHP];    // Epaisseur
AnsiString  T_LS1CL[MAX_PLAN][MAX_TCHP];    // Couleur
AnsiString  T_LS1ST[MAX_PLAN][MAX_TCHP];    // Style

AnsiString  T_LS2[MAX_PLAN][MAX_TCHP];      // Oui/Non (pour titre en ligne)
AnsiString  T_LS2EP[MAX_PLAN][MAX_TCHP];
AnsiString  T_LS2CL[MAX_PLAN][MAX_TCHP];
AnsiString  T_LS2ST[MAX_PLAN][MAX_TCHP];    // Style

// ENVELOPPES
AnsiString  E_ENV[MAX_PLAN][MAX_TCHP];   // Oui ou Non
AnsiString  E_POL[MAX_PLAN][MAX_TCHP]; // police
AnsiString  E_CTEXT[MAX_PLAN][MAX_TCHP]; // couleur Texte
AnsiString  E_TITAL[MAX_PLAN][MAX_TCHP];
AnsiString  E_TBOLD[MAX_PLAN][MAX_TCHP];
AnsiString  E_HAUTEUR[MAX_PLAN][MAX_TCHP];
AnsiString  E_JSH[MAX_PLAN][MAX_TCHP];
AnsiString  E_JSV[MAX_PLAN][MAX_TCHP];
AnsiString  E_CBKG[MAX_PLAN][MAX_TCHP];
AnsiString  E_TLARG[MAX_PLAN][MAX_TCHP];

AnsiString  E_COULTRAIT[MAX_PLAN][MAX_TCHP];
AnsiString  E_STYLE[MAX_PLAN][MAX_TCHP];
AnsiString  E_EPAISS[MAX_PLAN][MAX_TCHP];
AnsiString  E_DESIGN[MAX_PLAN][MAX_TCHP];
AnsiString  E_ENCADR[MAX_PLAN][MAX_TCHP];


Word    AnPrev,MoisPrev,JourPrev;
Word    AnFin,MoisFin,JourFin;
char  filtr[MAX_T];
AnsiString P_FIELD[MAX_PLAN][MAX_COL]; // Nom Variables  ou criteres pour tri

