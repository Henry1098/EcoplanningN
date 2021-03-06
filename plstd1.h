//---------------------------------------------------------------------------

#ifndef plstd1H
#define plstd1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include "AdvGrid.hpp"
#include "BaseGrid.hpp"
#include <Menus.hpp>

#include <syncobjs.hpp>


#include <Menus.hpp>
#include <AxCtrls.hpp>
#include <OleCtrls.hpp>
#include <vcf1.hpp>
#include <AdvGrid.hpp>
//#include <BaseGrid.hpp>

#include "isamD.h"
//#include "AdvGridWorkbook.hpp"
//#include "AsgMemo.hpp"
#include "asgprint.hpp"
#include <Dialogs.hpp>
#include <Buttons.hpp>


//---------------------------------------------------------------------------
class TplStandard : public TForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl1;
        TTabSheet *pl1;
        TLabel *Label1;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label10;
        TLabel *Label11;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label6;
        TLabel *Label7;
        TAdvStringGrid *AdvStringGrid1;
        TAdvStringGrid *AdvStringGrid2;
        TAdvStringGrid *AdvStringGrid3;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TTabSheet *TabSheet3;
        TTabSheet *TabSheet5;
        TTabSheet *TabSheet4;
        TTabSheet *TabSheet6;
        TPopupMenu *PopupList;
        TMenuItem *CocherDcocherTacheencours1;
        TMenuItem *CocherDcocherTches1;
        TMenuItem *CochertouteslesTaches1;
        TMenuItem *DcochertouteslesTaches1;
        TMenuItem *N2;
        TMenuItem *CopierlesTaches1;
        TMenuItem *LierTachesCoches1;
        TMenuItem *DlierTchesCoches1;
        TMenuItem *SupprimerTchesCoches1;
        TMenuItem *TachesAmontssurtachesCoches1;
        TMenuItem *AffecterunCalendrier1;
        TTabSheet *TabSheet8;
        TTabSheet *TabSheet9;
        TImage *imAvanc;
        TAdvStringGrid *asAvanc;
        TAdvStringGrid *AdvStringGrid4;
        TAdvStringGrid *AdvStringGrid5;
        TButton *btDatAct;
        TLabel *Label5;
        TLabel *Label12;
        TButton *btSavAct;
        TLabel *fname;
        TLabel *Label21;
        TLabel *Label22;
        TLabel *Label9;
        TLabel *Label20;
        TImage *imGantt;
        TLabel *Label2;
        TImage *immGantt;
        TLabel *Label26;
        TScrollBar *ScrollBar5;
        TScrollBar *ScrollBar6;
        TLabel *lAlert;
        TTimer *Timer1;
        TLabel *lTA;
        TLabel *lAlertPred;
        TImage *immAct;
        TScrollBar *ScrollBar7;
        TScrollBar *ScrollBar8;
        TPopupMenu *PopupActiv;
        TMenuItem *A1;
        TMenuItem *ToutDcocher1;
        TMenuItem *TachesCochesTermines1;
        TMenuItem *DclarerTachesCochesNonCommences1;
        TButton *btSauver;
        TButton *btSauverComme;
        TSaveDialog *SaveDialog1;
        TPrintDialog *PrintDialog1;
        TTimer *Timer2;
        TButton *Button3;
        TButton *btPrAvanc;
        TTabSheet *TabSheet7;
        TAdvStringGrid *sg;
        TImage *imPort;
        TImage *imLand;
        TFontDialog *FontDialog1;
        TLabel *Label27;
        TLabel *Label28;
        TAdvStringGrid *asCrit;
        TLabel *Label8;
        TButton *btSaveAffect;
        TPopupMenu *PopupCrit;
        TMenuItem *AffecterCritre1;
        TTrackBar *Zoom;
        TLabel *Label24;
        TLabel *Label29;
        TLabel *Label30;
        TImage *imP1Screen;
        TButton *btAccCrit;
        TLabel *Label31;
        TLabel *Label32;
        TLabel *Label33;
        TLabel *Label34;
        TLabel *lC1;
        TLabel *lC2;
        TLabel *lC3;
        TLabel *lC4;
        TButton *btAideCrit;
        TAdvStringGrid *asT;
        TLabel *lD1;
        TLabel *lD2;
        TLabel *lD3;
        TLabel *lD4;
        TLabel *lT2;
        TLabel *lT3;
        TLabel *lT4;
        TImage *imR1Screen;
        TImage *imR1p;
        TImage *imP1p;
        TImage *imP1l;
        TLabel *Label23;
        TLabel *Label25;
        TLabel *Label36;
        TLabel *Label37;
        TButton *btPrR1;
        TButton *btPrP1;
        TLabel *Label38;
        TLabel *Label35;
        TTrackBar *ZoomR1;
        TTrackBar *ZoomP1;
        TLabel *Label40;
        TLabel *Label41;
        TLabel *Label42;
        TButton *btReset;
        TButton *btTout;
        TButton *btTri;
        TButton *btFiltre;
        TButton *btPrep;
        TButton *btDrawR;
        TButton *btDrawP;
        TImage *iTest;
        TTabSheet *TabSheet10;
        TLabel *Label43;
        TLabel *Label44;
        TImage *imP0p;
        TLabel *Label45;
        TLabel *Label46;
        TLabel *Label47;
        TLabel *Label48;
        TButton *btAideGantt;
        TLabel *Label49;
        TLabel *Label50;
        TLabel *Label51;
        TLabel *Label52;
        TLabel *Label53;
        TLabel *Label54;
        TLabel *Label55;
        TButton *btAideSuivi;
        TLabel *Label56;
        TLabel *Label57;
        TLabel *Label58;
        TLabel *Label59;
        TLabel *Label60;
        TLabel *Label61;
        TLabel *Label62;
        TButton *btAideRapport1;
        TLabel *Label63;
        TLabel *Label64;
        TLabel *Label65;
        TLabel *Label66;
        TLabel *Label67;
        TButton *btPrepR1;
        TButton *btRetPlanning1;
        TTabSheet *TabSheet12;
        TPopupMenu *PopPlann1;
        TMenuItem *Action11;
        TMenuItem *Action21;
        TMenuItem *Action31;
        TAdvStringGrid *av;
        TTabSheet *TabSheet11;
        TButton *btAideS;
        TComboBox *cbEchelle;
        TLabel *Label19;
        TComboBox *cbEchG;
        TTabSheet *TabSheet13;
        TLabel *Label69;
        TTrackBar *ZoomP0;
        TButton *Button1;
        TLabel *Label70;
        TLabel *Label71;
        TComboBox *ComboBox1;
        TLabel *Label72;
        TLabel *lT1;
        TBevel *Bevel1;
        TLabel *Label73;
        TBitBtn *b1Prev;
        TBitBtn *b1Next;
        TLabel *Label75;
        TLabel *Label76;
        TAdvStringGrid *asTri;
        TLabel *Label74;
        TBitBtn *BitBtn3;
        TBitBtn *BitBtn4;
        TButton *btRenum;
        TButton *Button2;
        TAdvStringGrid *avc;
        TAdvStringGrid *ave;
        TLabel *Label77;
        TLabel *Label78;
        TLabel *Label79;
        TLabel *Label80;
        TLabel *Label81;
        TLabel *Label82;
        TRadioGroup *RadioGroup1;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TRadioButton *RadioButton3;
        TButton *Button4;
        TButton *Button5;
        TLabel *lnbPred;
        TLabel *lnbSucc;
        TButton *Button6;
        TButton *Button7;
        TButton *Button8;
        TButton *Button9;
        TButton *btPDFR1;
        TButton *btP1PDF;
        TMenuItem *AffecterDonnesauxTachesCoches1;
        TMenuItem *ComplterDsignationTachesCoches1;
        TMenuItem *LierTacheavecPrdcesseurs1;
        TMenuItem *LiertacheavecSuccesseurs1;
        TLabel *lxy;
        TImage *imR2Screen;
        TImage *imR2p;
        TBitBtn *b2Prev;
        TBitBtn *b2Next;
        TLabel *Label18;
        TLabel *Label39;
        TLabel *Label83;
        TLabel *Label84;
        TLabel *Label85;
        TTrackBar *ZoomR2;
        TLabel *Label86;
        TLabel *Label87;
        TButton *btPrR2;
        TButton *btPDFR2;
        TButton *btPrepR2;
        TButton *Button10;
        TLabel *Label88;
        TLabel *Label89;
        TLabel *Label91;
        TLabel *Label92;
        TImage *imP2Screen;
        TImage *imP2p;
        TImage *imP2l;
        TLabel *Label93;
        TTrackBar *ZoomP2;
        TLabel *Label90;
        TButton *btPL2Help;
        TLabel *Label94;
        TImage *imP3p;
        TLabel *Label95;
        TLabel *Label96;
        TImage *imP3l;
        TButton *btPL3Help;
        TImage *imP3Screen;
        TLabel *Label97;
        TTrackBar *ZoomP3;
        TLabel *Label98;
        TLabel *Label99;
        TButton *btPrP2;
        TButton *btP2PDF;
        TButton *btRetPlanning2;
        TButton *btPrP3;
        TButton *btP3PDF;
        TButton *btRetPlanning3;
        TLabel *Label102;
        TLabel *lTEC;
        TLabel *Label103;
        TLabel *LP1TFC;
        TButton *btImpGantt;
        TButton *btEchelle;
        TRadioGroup *RadioGroup2;
        TRadioButton *cbPos;
        TRadioButton *cbNeg;
        TLabel *Label68;
        TLabel *Labelxxx;
        TLabel *LP2FTC;
        TLabel *LP3FTC;
        TLabel *Label104;
        TLabel *LR1FTC;
        TLabel *Label105;
        TLabel *LR2FTC;
        TLabel *Label106;
        TLabel *Label107;
        TLabel *Label108;
        TLabel *Label109;
        TLabel *Label110;
        TLabel *Label111;
        TLabel *Label112;
        TLabel *Label113;
        TLabel *Label114;
        TLabel *Label115;
        TLabel *Label116;
        TLabel *Label117;
        TButton *btMemor1;
        TButton *btmemor2;
        TButton *btMemor3;
        TButton *btMemor4;
        TButton *btMemor5;
        TButton *btMiseenPage1;
        TButton *btMiseEnPage2;
        TButton *Button11;
        TButton *Button12;
        TButton *Button13;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Effacer1Click(TObject *Sender);
        void __fastcall Inserer1Click(TObject *Sender);
        void __fastcall CochertouteslesTaches1Click(TObject *Sender);
        void __fastcall DcochertouteslesTaches1Click(TObject *Sender);
        void __fastcall CopierlesTaches1Click(TObject *Sender);
        void __fastcall CocherDcocherTacheencours1Click(TObject *Sender);
        void __fastcall CocherDcocherTches1Click(TObject *Sender);
        void __fastcall LierTachesCoches1Click(TObject *Sender);
        void __fastcall SupprimerTchesCoches1Click(TObject *Sender);
        void __fastcall DlierTchesCoches1Click(TObject *Sender);
        void __fastcall AdvStringGrid1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall AdvStringGrid1MouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall AdvStringGrid1ContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
        void __fastcall AdvStringGrid2KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall AdvStringGrid3KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall AdvStringGrid1CanEditCell(TObject *Sender,
          int ARow, int ACol, bool &CanEdit);
        void __fastcall AdvStringGrid1ClickCell(TObject *Sender, int ARow,
          int ACol);
        void __fastcall AdvStringGrid1CellValidate(TObject *Sender,
          int ACol, int ARow, AnsiString &Value, bool &Valid);
        void __fastcall AdvStringGrid1SelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
        void __fastcall TachesAmontssurtachesCoches1Click(TObject *Sender);
        void __fastcall AffecterunCalendrier1Click(TObject *Sender);
        void __fastcall AdvStringGrid2CanEditCell(TObject *Sender,
          int ARow, int ACol, bool &CanEdit);
        void __fastcall AdvStringGrid3CanEditCell(TObject *Sender,
          int ARow, int ACol, bool &CanEdit);
        void __fastcall AdvStringGrid2CellValidate(TObject *Sender,
          int ACol, int ARow, AnsiString &Value, bool &Valid);
        void __fastcall AdvStringGrid3CellValidate(TObject *Sender,
          int ACol, int ARow, AnsiString &Value, bool &Valid);
        void __fastcall AdvStringGrid2SelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
        void __fastcall AdvStringGrid3SelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
        void __fastcall AdvStringGrid1GetFormat(TObject *Sender, int ACol,
          TSortStyle &AStyle, AnsiString &aPrefix, AnsiString &aSuffix);
        void __fastcall AdvStringGrid4CanEditCell(TObject *Sender,
          int ARow, int ACol, bool &CanEdit);
        void __fastcall btDatActClick(TObject *Sender);
        void __fastcall btSavActClick(TObject *Sender);
        void __fastcall ScrollBar6Scroll(TObject *Sender,
          TScrollCode ScrollCode, int &ScrollPos);
        void __fastcall ScrollBar5Scroll(TObject *Sender,
          TScrollCode ScrollCode, int &ScrollPos);
        void __fastcall AdvStringGrid5CellValidate(TObject *Sender,
          int ACol, int ARow, AnsiString &Value, bool &Valid);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall asAvancCanEditCell(TObject *Sender, int ARow,
          int ACol, bool &CanEdit);
        void __fastcall AdvStringGrid5CanEditCell(TObject *Sender,
          int ARow, int ACol, bool &CanEdit);
        void __fastcall asAvancClickCell(TObject *Sender, int ARow,
          int ACol);
        void __fastcall AdvStringGrid4CellValidate(TObject *Sender,
          int ACol, int ARow, AnsiString &Value, bool &Valid);
        void __fastcall asAvancContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
        void __fastcall btSauverCommeClick(TObject *Sender);
        void __fastcall btSauverClick(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall imPortMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall btPrAvancClick(TObject *Sender);
        void __fastcall imLandMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall btFontClick(TObject *Sender);
        void __fastcall asAvancGetCellColor(TObject *Sender, int ARow,
          int ACol, TGridDrawState AState, TBrush *ABrush, TFont *AFont);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall asCritMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall asCritContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall asCritClickCell(TObject *Sender, int ARow,
          int ACol);
        void __fastcall btAccCritClick(TObject *Sender);
        void __fastcall btSaveAffectClick(TObject *Sender);
        void __fastcall btResetClick(TObject *Sender);
        void __fastcall btToutClick(TObject *Sender);
        void __fastcall btTriClick(TObject *Sender);
        void __fastcall btPrepClick(TObject *Sender);
        void __fastcall imP1pMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall imP1lMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall btPrP1Click(TObject *Sender);
        void __fastcall btRetPlanning1Click(TObject *Sender);
        void __fastcall imP1ScreenMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall ZoomP1Change(TObject *Sender);
        void __fastcall BitBtn3Click(TObject *Sender);
        void __fastcall BitBtn4Click(TObject *Sender);
        void __fastcall asTriClickCell(TObject *Sender, int ARow,
          int ACol);
        void __fastcall btRenumClick(TObject *Sender);
        void __fastcall Button6Click(TObject *Sender);
        void __fastcall btAideSuiviClick(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button7Click(TObject *Sender);
        void __fastcall avcCanEditCell(TObject *Sender, int ARow, int ACol,
          bool &CanEdit);
        void __fastcall RadioButton1Click(TObject *Sender);
        void __fastcall RadioButton2Click(TObject *Sender);
        void __fastcall RadioButton3Click(TObject *Sender);
        void __fastcall avcCellValidate(TObject *Sender, int ACol,
          int ARow, AnsiString &Value, bool &Valid);
        void __fastcall cbPosClick(TObject *Sender);
        void __fastcall cbNegClick(TObject *Sender);
        void __fastcall aveGetAlignment(TObject *Sender, int ARow,
          int ACol, TAlignment &HAlign, TVAlignment &VAlign);
        void __fastcall Button8Click(TObject *Sender);
        void __fastcall AffecterDonnesauxTachesCoches1Click(
          TObject *Sender);
        void __fastcall ComplterDsignationTachesCoches1Click(
          TObject *Sender);
        void __fastcall LierTacheavecPrdcesseurs1Click(TObject *Sender);
        void __fastcall LiertacheavecSuccesseurs1Click(TObject *Sender);
        void __fastcall btP1PDFClick(TObject *Sender);
        void __fastcall btPrepR1Click(TObject *Sender);
        void __fastcall TabSheet8Show(TObject *Sender);
        void __fastcall TabSheet10Show(TObject *Sender);
        void __fastcall TabSheet12Show(TObject *Sender);
        void __fastcall TabSheet11Show(TObject *Sender);
        void __fastcall TabSheet4Show(TObject *Sender);
        void __fastcall btRetPlanning2Click(TObject *Sender);
        void __fastcall btPrP2Click(TObject *Sender);
        void __fastcall btRetPlanning3Click(TObject *Sender);
        void __fastcall imP2pMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imP3pMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imP2lMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imP3lMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imR1pMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imR2pMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall btPrepR2Click(TObject *Sender);
        void __fastcall cbEchelleChange(TObject *Sender);
        void __fastcall ZoomR1Change(TObject *Sender);
        void __fastcall ZoomR2Change(TObject *Sender);
        void __fastcall ZoomP2Change(TObject *Sender);
        void __fastcall ZoomP3Change(TObject *Sender);
        //void __fastcall arCanEditCell(TObject *Sender, int ARow, int ACol,
        //  bool &CanEdit);
        //void __fastcall arMouseUp(TObject *Sender, TMouseButton Button,
        //  TShiftState Shift, int X, int Y);
        //void __fastcall arMouseDown(TObject *Sender, TMouseButton Button,
        //  TShiftState Shift, int X, int Y);
        //void __fastcall Button11Click(TObject *Sender);
        //void __fastcall Button12Click(TObject *Sender);
        void __fastcall b1PrevClick(TObject *Sender);
        void __fastcall b1NextClick(TObject *Sender);
        void __fastcall AdvStringGrid2ClickCell(TObject *Sender, int ARow,
          int ACol);
        void __fastcall AdvStringGrid3ClickCell(TObject *Sender, int ARow,
          int ACol);
        void __fastcall asCritCanEditCell(TObject *Sender, int ARow,
          int ACol, bool &CanEdit);
        void __fastcall TabSheet5Show(TObject *Sender);
        void __fastcall TabSheet5Hide(TObject *Sender);
        void __fastcall b2PrevClick(TObject *Sender);
        void __fastcall b2NextClick(TObject *Sender);
        void __fastcall btPrR1Click(TObject *Sender);
        void __fastcall btPrR2Click(TObject *Sender);
        void __fastcall imR1ScreenMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imR2ScreenMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imP2ScreenMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imP3ScreenMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TabSheet1Show(TObject *Sender);
        void __fastcall ZoomP0Change(TObject *Sender);
        void __fastcall imP0pMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall btP2PDFClick(TObject *Sender);
        void __fastcall btP3PDFClick(TObject *Sender);
        void __fastcall btPrP3Click(TObject *Sender);
        void __fastcall btImpGanttClick(TObject *Sender);
        void __fastcall btEchelleClick(TObject *Sender);
        void __fastcall asCritSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
        void __fastcall btMemor1Click(TObject *Sender);
        void __fastcall btmemor2Click(TObject *Sender);
        void __fastcall btMemor3Click(TObject *Sender);
        void __fastcall btMemor4Click(TObject *Sender);
        void __fastcall btMemor5Click(TObject *Sender);
        void __fastcall btMiseenPage1Click(TObject *Sender);
        void __fastcall btMiseEnPage2Click(TObject *Sender);
        void __fastcall AdvStringGrid1GetAlignment(TObject *Sender,
          int ARow, int ACol, TAlignment &HAlign, TVAlignment &VAlign);
        void __fastcall Button11Click(TObject *Sender);
        void __fastcall Button12Click(TObject *Sender);
        void __fastcall Button13Click(TObject *Sender);
        void __fastcall btAideSClick(TObject *Sender);
        void __fastcall imGanttMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall imGanttMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall Label71Click(TObject *Sender);

private:	// User declarations

        TStringList *slrupt;
        TStringList *tchcrit;

        char strcoche[250];
        char strdanger[250];
        TRect  Diagram;
        TColor  BgColor,FgColor,TxColor;

        Graphics::TBitmap *im;
        Graphics::TBitmap *imP;
        Graphics::TBitmap *imR;
        Graphics::TBitmap *img;

        TImage *Origin_Image;

        
        Word   Year1,Month1,Day1;
        Word   Year2,Month2,Day2;

        int  TimeCounter;

        genbase *proj;
        genbase *cals;
        genbase *avanc;
        genbase *oldavanc;
        genbase *temp;

        char isamname[255];
        char FileNameAvanc[255];   // avancement

        int    nb_taches;
        char   buffer[2000];
        int    current_task;
        int    CurCol,CurRow;
        int    change;

        char   dbtitre[250];
        char   dbcode[100];
        char   dateprevdeb[20];

        int    SG1Col,SG1Row,Cur1Col,Cur1Row;
        int    SG2Col,SG2Row,Cur2Col,Cur2Row;
        int    SG3Col,SG3Row,Cur3Col,Cur3Row;

        int    CrCol,CrRow;


        AnsiString PreviousPred, PreviousSucc;

        int    origin;


#define MAX_T  3010
#define PS     100

#define NS     200
#define NB_CAL 5

Word    Association[MAX_T];





int    NB1,NB2;   // nb de jours entre deux dates
int    DECAL,CALP,NUM_TACHE_PRED,IND_PRED;
int    DECALPRED;
int    CH,CHPRED,ROWPRED;

char  TYPL[5][5];

int   nbsucc,nbpred;
int   nb_tachfiltr;
int   iniv;

int   CLEF;


#define MAX_PAR 200

char  dateavanc[5000];  // separes par des @
int   nb_avanc;
char params[MAX_PAR][150];
bool flag_alert;
bool flip_flop;
bool flag_alertPred;
bool flip_flopPred;
char std_date[20];
char reverse_date[20];
char date10[15];
int row_contrainte;

int  maximum;
int   xmax;    // memorise derniere tache , duree au plus tot
int   xstart;

int   new_nb_taches;
int   ctask;

int    row_avancement;
int    row_av_pred;
int    av_tache_pred;
bool   FLAG_AVANC_LOADED;
bool   global_valid;
char   adv[255];
bool   flag_avanc;

#define MAX_CHAMP 100
char ch_cod[MAX_CHAMP][20];
char ch_des[MAX_CHAMP][50];
char ch_abb[MAX_CHAMP][15];
char ch_typ[MAX_CHAMP][10];
char ch_sai[MAX_CHAMP][15];
char ch_esp[MAX_CHAMP][10];


#define MAX_VOL 15
#define MAX_CHP 20
int vl_chp[MAX_VOL][MAX_CHP];   // 20 champs maximum par volets
bool flag_loadavanc;
bool flag_filter;
int  modfilter;
char strfilter[2000];



// Champs filtres et tri

#define MXC 100
char libchamp[MXC][100];
char cnt[MXC][10];
char abb[MXC][10];
char cln[MXC][10];

int CNT_CHAMPS;
char CONTROL[10];


int OldCol,OldRow;

// Criteres;
#define MAX_CRIT 10


#define MAX_ZONE  500
int Cal1Z[MAX_ZONE];
int Cal2Z[MAX_ZONE];


int Hauteur_Cal;
int Hauteur_Gen; // pour calcul des rapports

//  Graphique pour PLANNING
int rx1,ry1,rx2,ry2;
#define    ZOOM  100
int        X0,Y0,L,H;
bool       flag_real;
bool       flag_init;

int  increm_CAL1,increm_CAL2;

AnsiString Format_Cal;

int    YY0,YY00;
int    INDX;   // Index du nb de ruptures
int    HAUTEUR;
int    HET;
int    Nb_Rupt,Nb_Ligne,Nb_Envel;
int    Hauteur_CalBas,Hauteur_CalHaut, Hauteur_Entete,Hauteur_Pied;



TImage *iPort;
TImage *iLand;
TImage *iScreen;

char    sserial[255];
int     timercnt;

AnsiString MemoDecal2;
AnsiString MemoDecal3;

AnsiString LibRupt[500];
int  LibCtr;

int    AnPrev2;
int    An,Mois,Jour;
int    AnVerif,MoisVerif,JourVerif;
int    AnAvanc,MoisAvanc,JourAvanc;
int    AnPred,MoisPred,JourPred;





/////////////////////////////////////////////////////////////////////////////
        void __fastcall insert_simple(char *nstr,char *dst,int pos);
        void __fastcall Modify_Criteres(char *str);
        void __fastcall Copy_File(char *dest,char *src);
        int __fastcall Compute_Week(int c_an,int c_mois,int c_jour);
        int __fastcall GetStyle(AnsiString STYLE);
        int __fastcall Compute_Trav(int ix,int zcal);
        int __fastcall Compute_Base();
        AnsiString __fastcall ChercheCode(int k, AnsiString a);
        int __fastcall  CheckNiv(AnsiString ccrit,int rupt);
        void __fastcall Trace_Precalcul(int npage);
        void __fastcall Update_Hauteur(int hl,int pagecounter,int npage);
        void __fastcall Trace_Page_Rapport(int Page_Rapport, TImage *imRapport);
        void __fastcall ImprimeDirect(int dual,TImage *IM1, TImage *IM2);
        bool __fastcall Check_Automatic();
        int __fastcall  Compute_Nb_Enveloppes();
        void __fastcall DispCritere(int ARow,int ACol);
        void __fastcall SelectPrinter(int modepdf,int dual,TImage *IM1, TImage *IM2);
        int  __fastcall LookFirst();
        void __fastcall Format_Calendrier(AnsiString c_format, int c_an,int c_mois, int c_jour);
        int  __fastcall Ajouter_Tirets(char *val);
        void __fastcall Faire_LSH(int rupt);
        void __fastcall Faire_LSB(int rupt);
        void __fastcall Finir(int rupt,AnsiString txt);
        void __fastcall Commencer(int rupt,AnsiString txt);
        void __fastcall Finir_Hauteur(int rupt,AnsiString txt);
        void __fastcall Commencer_Hauteur(int rupt,AnsiString txt);
        void __fastcall Compute_LH(AnsiString FName,AnsiString txt,int h,AnsiString Ital,AnsiString Bold);

        void __fastcall InitTri();
        void __fastcall Read_Criteres();
        AnsiString __fastcall extract_rupt(AnsiString Crit,int numcrit);
        AnsiString __fastcall text_rupt(AnsiString Crit);
        void __fastcall Copy_Col(int c1,int c2,char *typ,int numcrit);
        int  __fastcall RechercheCol(char *str);
        int  __fastcall RechercheColCrit(char *str);
        void __fastcall Exec_Pas_Pas(char *str);
        int  __fastcall Exec_Alpha(char *ch,char *cond,char *v1,char *v2);
        int  __fastcall Exec_Num(char *ch,char *cond,char *v1,char *v2);
        int  __fastcall Exec_Date(char *ch,char *cond,char *v1,char *v2);
        void __fastcall Draw_Rupture(int rupt,AnsiString txt);

        void __fastcall Insert_G(char ctx, char *typ,int x1,int y1,int x2,int y2,
        char *ref, TColor bg, TColor fg, int epais,int style, AnsiString font, AnsiString ital,
        AnsiString bold, AnsiString text);

        /*
        void __fastcall Insert_R(char ctx,char *typ,int x1,int y1,int x2,int y2,
        char *ref, TColor bg, TColor fg, int epais, int style, AnsiString font, AnsiString ital,
        AnsiString bold, AnsiString text);
        */
        void __fastcall Call_Dialog(int x,int y);

        void __fastcall ModifDatePrev(char *str);



        void __fastcall Draw_Planning(Graphics::TBitmap *I);

        void __fastcall Remplir_asTri();
        void __fastcall Remplir_AV1();
        void __fastcall Clear_Diagram(TImage *im);
        int  __fastcall Add_Row(int row,int nt);
        void __fastcall Load_Grille_Criteres();
        void __fastcall Planning_Defaut(); 


/////////////////////////////////////////////////////////////////////////////
        void __fastcall Create_First_Real();
        void __fastcall Tout_Cocher();
        void __fastcall Tout_Decocher();
        void __fastcall Copier_Cocher();
        void __fastcall Coche_Decoche_Crit(int col,int row);
        void __fastcall Cocher_Decocher_Crit();
        void __fastcall Tout_Cocher_Crit();
        void __fastcall Tout_Decocher_Crit();
        int  __fastcall Exec_Coches(char *s);
        int  __fastcall Exec_Criteres(char *s);
        int  __fastcall Exec_Trier(int mod, char *s);
        void __fastcall Exec_Simul();
        int  __fastcall Init_Grids();
        int  __fastcall Exec_Filter(int mod,char *sfilter);
        int  __fastcall Verif_Decal(int mode, int typ,int t,int decal,int sp);
        int  __fastcall Verif_Succ(int t,int a);
        int  __fastcall Verif_Pred(int t,int a);
        void __fastcall Draw_Calendars(int nbt,int maxdist,int spx,int spy);
        void __fastcall Draw_Diagram(int maxdist,int spx,int spy);
        void __fastcall Graph_Avanc(int nbt,int maxdist);
        void __fastcall Image_Avanc(char *title);
        void __fastcall Print_Image(TImage *image);
        int __fastcall Calcul_MR();
        int  __fastcall Calcul_Margereelle(int retard);
        int __fastcall Calcul_Avancement(int niv, int tache,int jour);
        int __fastcall TrouveRow(int nt);
        int __fastcall Decale_Tache_Succ(int row,int retard);
        int __fastcall Change1();
        int __fastcall Change0();
        int  __fastcall Organise_Niveaux(void);
        int  __fastcall Extraire_Date(char *fn);
        int  __fastcall Load_Avanc(char *fn,bool concern);
        void __fastcall Load_Previsionnel();
        void __fastcall Load_Reference();

        int  __fastcall concat(char *tag);
        int  __fastcall Propage(int ch);
        int  __fastcall Faire_Riper();
        


        int  __fastcall Compute_Percent_Restant();
        int  __fastcall Compute_Jours_Restants();
        int  __fastcall Compute_Duree_Reelle();
        int  __fastcall Compute_Date_Fin();
        int  __fastcall Compute_Date_Deb();
        int  __fastcall Compute_Deb_From_Fin(char *dt);
        int  __fastcall Compute_Retard_Total();
        int  __fastcall Compute_Retard_Imputable();
        int  __fastcall Compute_Flag_Alert();
        int  __fastcall Compute_DateDebutPossible();
        int  __fastcall Compute_Fin_Avance();

        int  __fastcall Compute_Percent_RestantPred(int row);
        int  __fastcall Compute_Jours_RestantsPred(int row);
        int  __fastcall Compute_Duree_ReellePred(int row);
        int  __fastcall Compute_Date_FinPred(int row);
        int  __fastcall Compute_Date_DebPred(int row);
        int  __fastcall Compute_Deb_From_FinPred(int row);
        int  __fastcall Compute_Retard_TotalPred(int row);
        int  __fastcall Compute_Retard_ImputablePred(int row);
        int  __fastcall Compute_Flag_AlertPred(int row);
        int  __fastcall Compute_DateDebutPossiblePred(int row);
        int  __fastcall Compute_Fin_AvancePred(int row);

        int __fastcall  verifdate(char *dt10);
        int __fastcall  verifdate8(char *dt8);
        int  __fastcall add_jour(int year, int month, int day, int jours);
        int  __fastcall comp_dist_dates(char *dt1, char *dt2,int cal,int offset);
        int  __fastcall elargir_annee(char *dt8);

        int  __fastcall comp_dist_succ(int indx,int succ,int isucc);
        int  __fastcall comp_dist_succ_avanc(int indx,int succ,int isucc,int tot,int tard);
        int __fastcall  comp_dist(int indx,int xtsucc, int isucc);

        AnsiString __fastcall Debut_Possible(int indx,int xtsucc, int isucc);

        int  __fastcall convert_date(int year, int month, int day);
        int  __fastcall avance_conges(int base,int ncal); //,int dur);
        int  __fastcall avance_temps(int base,int ncal,int dur);
        int  __fastcall recule_conges(int base,int ncal);
        int  __fastcall recule_temps(int base,int ncal, int dur);
        int  __fastcall comp_decal_fin(int st,int fin, int ncal);

        int  __fastcall CopieAvancePrev(char *fn);
        int  __fastcall CopieAvance(char *suff,char *fn);

        int  __fastcall verif_conges(char *dt, int xcal);
        int  __fastcall Compute_Decalages(int base,char *typl,int caltache,int calpred,int decal,int dur);
        int  __fastcall getvalue(int vol,int row,int ch);
        int  __fastcall setvalue(int vol,int row,int ch, char *p);
        int  __fastcall fetch_champ(int vol,int col);
        void __fastcall Load_Fields();
        void __fastcall Load_Ecochamps();
        void __fastcall Init_Volet(int vol, TAdvStringGrid *avs);
        int  __fastcall Explode(char sep,char *str);
        void __fastcall Compute_PlusTotA();
        void __fastcall Compute_PlusTardA(char mode);
        
        void __fastcall DrawGantt(int task,int pos);
        void __fastcall DrawMiniGantt(int task,int pos);
        void __fastcall Refresh_Pred_Succ (int row);

        void __fastcall InsertPredX(int tach, int tpred, int tl, int decal);
        void __fastcall InsertSuccX(int tach, int tsucc, int tl, int decal);

        void __fastcall InsertPredSeul(int tach,int tpred, int tl, int decal,bool disp);
        void __fastcall InsertSuccSeul(int tach,int tsucc, int tl, int decal,bool disp);
        void __fastcall RemovePredSeul(int tach,int tpred);
        void __fastcall RemoveSuccSeul(int tach, int tsucc);



        //void __fastcall WriteExchange(int n);
        //void __fastcall ReadExchange(int n);

        void __fastcall effacer_tache(int t);
        void __fastcall delier_tache(int t);
        int  __fastcall exist_tache(int t,int range);
        int  __fastcall exist_prev(int t);
        int  __fastcall next_task();

        void __fastcall Coch_Decoch();
        void __fastcall Supp_Coch();
        void __fastcall Lier_Coch();
        void __fastcall Delier_Coch();
        void __fastcall Affect_Cal();
        void __fastcall Affect_Amont();
        void __fastcall Affect_Pred();
        void __fastcall Affect_Succ();
        void __fastcall Affect_Des();
        void __fastcall Affect_Data();
        

        void __fastcall refresh_coches();
        void __fastcall str_replace(char *result,char *src,char *dst);

        int  __fastcall add_select(int it);
        int  __fastcall remove_task(int it);
        int  __fastcall exist_select(int it);

        int  __fastcall init_calendrier();
        int  __fastcall is_succ(int base,int numt);

        int  __fastcall comp_up(int deb,int ncal,int dur);
        int  __fastcall comp_down(int deb,int ncal,int dur);

        int  __fastcall compute_marge(int tot, int tard, int ncal);
        int __fastcall  insere_tache(int a, int dur, int nllet, char *txt); //, int tmere, int tl,
                    //   int dec, char *design);
        int __fastcall  grefresh(int task,char mode);
        int __fastcall  refresh_marges();
        void __fastcall  validate_avancement();
        void __fastcall Clean_Grid(TAdvStringGrid *sgrid, int strow);

        void __fastcall Compute_Nlle_Duree(int t);
        void __fastcall Compute_Nlle_DureeA(int t);

        int  __fastcall Bcl_Pred(int t,int p);
        int  __fastcall Bcl_Succ(int t,int s);
        int  __fastcall Load_Data();
        int  __fastcall Ord_Pred(int ind,int xtpred,int ipred);
        int  __fastcall Compute_Possible(int row,int indice,int ipred);
        void __fastcall Exec_Message(char *mess);
        void __fastcall Exec_Planning(char *plan, bool reload);
        int  __fastcall Lect_Numt(int Row);
        void __fastcall SaveCriteres();
        void __fastcall LoadConfig();
        void __fastcall SaveConfig();
        float __fastcall AnsiToFloat(AnsiString str);
        int   __fastcall AnsiToInt(AnsiString str);
        void __fastcall ZoomCopy0(TImage *i,int X,int Y);
        void __fastcall ZoomCopy1(TImage *i,int X,int Y);
        void __fastcall ZoomCopy2(TImage *i,int X,int Y);
        void __fastcall ZoomCopy3(TImage *i,int X,int Y);

        void __fastcall ZoomCopy4(TImage *i,int X,int Y);
        void __fastcall ZoomCopy5(TImage *i,int X,int Y);

        int __fastcall Attrib_Tri();
        int __fastcall Lookup(int t);

        //  Trac� plannings et rapports
        void __fastcall Prep_Rapport(int npage);

        void __fastcall Exec_Rapport(char *s,bool reload);
        void __fastcall Prepare_Draw_Planning();  // main routine  lance tri et filtre
        void __fastcall Prepare_Draw_Rapport(int npage);
        void __fastcall Trace_Col_Rapport(int tache);
        void __fastcall RAZ();
        void __fastcall Prepare_Variables();
        void __fastcall Trace_Entete();
        void __fastcall Prepare_Colonnes();
        void __fastcall Prepare_Ruptures();
        void __fastcall Trace_Calendrier();  // cal et entete colonnes

        void __fastcall Trace_Vertical(int ix);
        void __fastcall Trace_Taches();
        void __fastcall Trace_Liens();
        void __fastcall Trace_Pied();

        void __fastcall Trace_CalBas();

        void __fastcall Trace_Cadre();
        void __fastcall Trace_Marges();

        void __fastcall Draw_Rapport(Graphics::TBitmap *B, int pagenum);

        void __fastcall Clean_Footer();

        void __fastcall Compute_BaseCal();

        void __fastcall Insert_Block(int i);
        void __fastcall Insert_Col(int i);
        bool __fastcall Insert_Compress(int k,int i,int deb, int xmax,int x2);
        void __fastcall Fleche(Graphics::TBitmap *B,char sens,int x,int y);
        void __fastcall Prepare_Hauteur();
        void __fastcall Compute_Enveloppe(int k, int i,int x1,int x2,int d1,int d2);
        void __fastcall Prepare_Enveloppes();
        void __fastcall Trace_Enveloppe(int k,int i,AnsiString txt);
        int  __fastcall Trouve(int i);

        void __fastcall ReloadAllData(char *name);
        int __fastcall exist_renum(int t);
        int __fastcall nbjt(int d1,int d2);

        void __fastcall SaveAv();
        void __fastcall Load_Couts();
        void __fastcall refresh_couts();
        void __fastcall Calcul_Echeancier();
        int  __fastcall convert_datex(char *dat);
        int  __fastcall InitKey();
        int  __fastcall check_key(int val);

        void __fastcall Exec_Import_Excel();
        void __fastcall Exec_Export_Excel();
        void __fastcall conc(char *dst,char *src,int typel);

        void __fastcall Compute_Marqueur();  // pc implicite
        void __fastcall DefaultPage();

        void __fastcall SaveAs();
        void __fastcall Save();

public:		// User declarations
        __fastcall TplStandard(TComponent* Owner);
        //int __fastcall SaveAs(char *name);
         void __fastcall updatedatabase(char *name,int prompt,int mode);
         void __fastcall Renum();


};
//---------------------------------------------------------------------------
extern PACKAGE TplStandard *plStandard;
extern char  *DatabaseName;
extern char *m_status;

//---------------------------------------------------------------------------
#endif
