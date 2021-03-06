//----------------------------------------------------------------------------
#ifndef MainH
#define MainH
//----------------------------------------------------------------------------
#include "FinalIsam.h"
#include "ChildWin.h"
#include "ProjectWin.h"
#include <syncobjs.hpp>
#include <vcl\ComCtrls.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Messages.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\Dialogs.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include <ToolWin.hpp>

#include "mdchoix.h"


//----------------------------------------------------------------------------


class TMainForm : public TForm
{
__published:
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *FileOpenItem;
	TMenuItem *FileCloseItem;
	TMenuItem *Window1;
	TMenuItem *Help1;
	TMenuItem *N1;
	TMenuItem *FileExitItem;
	TMenuItem *WindowCascadeItem;
	TMenuItem *WindowTileItem;
	TMenuItem *WindowArrangeItem;
	TMenuItem *HelpAboutItem;
	TOpenDialog *OpenDialog;
	TMenuItem *WindowMinimizeItem;
        TStatusBar *StatusBar;
        TActionList *ActionList1;
        TEditCut *EditCut1;
        TEditCopy *EditCopy1;
        TEditPaste *EditPaste1;
        TAction *FileNew1;
        TAction *FileSave1;
        TAction *FileExit1;
        TAction *FileOpen1;
        TAction *FileSaveAs1;
        TWindowCascade *WindowCascade1;
        TWindowTileHorizontal *WindowTileHorizontal1;
        TWindowArrange *WindowArrangeAll1;
        TWindowMinimizeAll *WindowMinimizeAll1;
        TAction *HelpAbout1;
        TWindowClose *FileClose1;
        TWindowTileVertical *WindowTileVertical1;
        TMenuItem *WindowTileItem2;
        TToolBar *ToolBar2;
        TToolButton *ToolButton4;
        TToolButton *ToolButton5;
        TToolButton *ToolButton6;
        TToolButton *ToolButton7;
        TImageList *ImageList1;
        TMenuItem *NouveauProjet1;
        TMenuItem *Outils1;
        TMenuItem *SauverBasedeconnaissances1;
        TMenuItem *RestorerBasedeConnaissances1;
        TMenuItem *Cocher1;
        TMenuItem *CocherTouteslestches1;
        TMenuItem *Dcochertouteslestches1;
        TMenuItem *Copierlestchescoches1;
        TMenuItem *Supprimerlestchescoches1;
        TMenuItem *Sauver1;
        TMenuItem *SauverComme1;
        TSaveDialog *SaveDialog;
        TMenuItem *Calendriers1;
        TTimer *Timer1;
        TMenuItem *Aide1;
        TMenuItem *N3;
        TMenuItem *PlanchesVolets1;
        TMenuItem *Filtres1;
        TMenuItem *FiltrerTches1;
        TMenuItem *FiltrerTouteslesTches1;
        TMenuItem *FiltrerAucuneTche1;
        TMenuItem *Critres1;
        TMenuItem *N6;
        TMenuItem *N7;
        TMenuItem *Cocher2;
        TMenuItem *CocherTouteslesTches2;
        TMenuItem *DcocherTouteslesTches2;
        TMenuItem *N8;
        TMenuItem *CocherDcocher1;
        TMenuItem *CocherDcocher2;
        TMenuItem *N9;
        TMenuItem *Configurationimpression1;
        TMenuItem *N10;
        TMenuItem *ImportExport1;
        TMenuItem *Projet1;
        TMenuItem *Rsumduprojet1;
        TMenuItem *Datededbutduprojet1;
        TMenuItem *Calendrier1;
        TMenuItem *Critres2;
        TMenuItem *Ressources1;
        TMenuItem *ConfigurationOnglets1;
        TMenuItem *Lier1;
        TMenuItem *Dlier1;
        TMenuItem *Affecteruncalendrier1;
        TMenuItem *Crertachesamont1;
        TMenuItem *Affecterplusieursprdcesseurs1;
        TMenuItem *Affecterplusieurssuccesseurs1;
        TMenuItem *Crertcheenveloppe1;
        TMenuItem *Complterdsignationdestches1;
        TMenuItem *Affecterdonnetachescoches1;
        TMenuItem *Renumrotertches1;
        TMenuItem *Prparationimpressionrapport1;
        TMenuItem *Prparationimpressionplanning1;
        TMenuItem *Simulation1;
        TPrinterSetupDialog *PrinterSetupDialog1;
        TMenuItem *Test1;
        TMenuItem *TestShow1;
        TMenuItem *ComplterCrerfiltresmmoriss1;
        TMenuItem *Variables1;
        TMenuItem *InterfaceExcel1;
        TImageList *ImageList2;
        TToolButton *ToolButton11;
        TToolButton *ToolButton13;
        TToolButton *ToolButton14;
        TToolButton *ToolButton15;
        TToolButton *ToolButton16;
        TToolButton *ToolButton17;
        TToolButton *ToolButton18;
        TToolButton *ToolButton19;
        TToolButton *ToolButton20;
        TToolButton *ToolButton21;
        TMenuItem *TrierGrouper1;
        TMenuItem *UtiliserTrierGrouper1;
        TMenuItem *CrerModifierSupprimerTrisGroupes1;
        TMenuItem *CrationRpartitionColonnes1;
        TMenuItem *ImportPlanning1;
        TMenuItem *Rsolution1024x7681;
        TMenuItem *RsolutionMaximum1;
        TMenuItem *ExportFichierExcel1;
        TMenuItem *ExportExcelCritres1;
        TMenuItem *ExportExcelCotsetRessources1;
        TMenuItem *RemiseZro1;
        TMenuItem *ImportFormatCSV1;
        TMenuItem *N2;
        TMenuItem *Enregistrer1;
        TMenuItem *Tachesenamont1;
        void __fastcall FileNew1Execute(TObject *Sender);
        void __fastcall FileOpen1Execute(TObject *Sender);
        void __fastcall HelpAbout1Execute(TObject *Sender);
        void __fastcall FileExit1Execute(TObject *Sender);
        void __fastcall NouveauProjet1Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall RestorerBasedeConnaissances1Click(TObject *Sender);
        void __fastcall SauverComme1Click(TObject *Sender);
        void __fastcall Calendriers1Click(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Aide1Click(TObject *Sender);
        void __fastcall PlanchesVolets1Click(TObject *Sender);
        void __fastcall FiltrerAucuneTche1Click(TObject *Sender);
        void __fastcall FiltrerTches1Click(TObject *Sender);
        void __fastcall Copierlestchescoches1Click(TObject *Sender);
        void __fastcall Supprimerlestchescoches1Click(TObject *Sender);
        void __fastcall Critres1Click(TObject *Sender);
        void __fastcall CocherTouteslestches1Click(TObject *Sender);
        void __fastcall Dcochertouteslestches1Click(TObject *Sender);
        void __fastcall CocherTouteslesTches2Click(TObject *Sender);
        void __fastcall CocherDcocher1Click(TObject *Sender);
        void __fastcall DcocherTouteslesTches2Click(TObject *Sender);
        void __fastcall CocherDcocher2Click(TObject *Sender);
        void __fastcall TrierTches1Click(TObject *Sender);
        void __fastcall Configurationimpression1Click(TObject *Sender);
        void __fastcall Prparationimpressionplanning1Click(
          TObject *Sender);
        void __fastcall Prparationimpressionrapport1Click(TObject *Sender);
        void __fastcall Test1Click(TObject *Sender);
        void __fastcall TestShow1Click(TObject *Sender);
        void __fastcall ComplterCrerfiltresmmoriss1Click(TObject *Sender);
        void __fastcall ImportExport1Click(TObject *Sender);
        void __fastcall Variables1Click(TObject *Sender);
        void __fastcall InterfaceExcel1Click(TObject *Sender);
        void __fastcall File1Click(TObject *Sender);
        void __fastcall Rsumduprojet1Click(TObject *Sender);
        void __fastcall Datededbutduprojet1Click(TObject *Sender);
        void __fastcall Affecterdonnetachescoches1Click(TObject *Sender);
        void __fastcall Simulation1Click(TObject *Sender);
        void __fastcall Complterdsignationdestches1Click(TObject *Sender);
        void __fastcall Lier1Click(TObject *Sender);
        void __fastcall Dlier1Click(TObject *Sender);
        void __fastcall FileCloseItemClick(TObject *Sender);
        void __fastcall Sauver1Click(TObject *Sender);
        void __fastcall Renumrotertches1Click(TObject *Sender);
        void __fastcall Calendrier1Click(TObject *Sender);
        void __fastcall Critres2Click(TObject *Sender);
        void __fastcall Affecteruncalendrier1Click(TObject *Sender);
        void __fastcall Crertachesamont1Click(TObject *Sender);
        void __fastcall Affecterplusieursprdcesseurs1Click(
          TObject *Sender);
        void __fastcall Affecterplusieurssuccesseurs1Click(
          TObject *Sender);
        void __fastcall UtiliserTrierGrouper1Click(TObject *Sender);
        void __fastcall CrerModifierSupprimerTrisGroupes1Click(
          TObject *Sender);
        void __fastcall CrationRpartitionColonnes1Click(TObject *Sender);
        void __fastcall ImportPlanning1Click(TObject *Sender);
        void __fastcall ImportTri1Click(TObject *Sender);
        void __fastcall ImportFiltre1Click(TObject *Sender);
        void __fastcall ImportColonnes1Click(TObject *Sender);
        void __fastcall Rsolution1024x7681Click(TObject *Sender);
        void __fastcall RsolutionMaximum1Click(TObject *Sender);
        void __fastcall ExportFichierExcel1Click(TObject *Sender);
        void __fastcall ExportExcelCritres1Click(TObject *Sender);
        void __fastcall ExportExcelCotsetRessources1Click(TObject *Sender);
        void __fastcall RemiseZro1Click(TObject *Sender);
        void __fastcall ImportFormatCSV1Click(TObject *Sender);
        void __fastcall Enregistrer1Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FiltrerTouteslesTches1Click(TObject *Sender);

        

        private:

#define MAX_PROJ 50
        char buffer[500];
        char tmp[500];
        char list_proj[MAX_PROJ][100];
        int  stat_proj[MAX_PROJ];
        int  indx_proj;
        int  pos;  // Menu
        char hist[11][100];
        char hpath[11][MAX_PATH];
        int  count; // menu items count
        int  flag_last;

        int  CLEF;
        char   nb_uses[250];
        int    nb_tries;
        int    timercnt;

        char params[50][150];



        realisam  *proj;
        realisam  *avanc;
        typedef struct // stch
          {
                int t_numtache;
                AnsiString t_design;
                int t_duree;
                //TStringList *t_pred;
                //TStringList *t_succ;
                //TStringList *t_amont;
                AnsiString t_crit[8];
                AnsiString t_libcrit[8];

          } T_tch;

        T_tch  *stt;
        char  sserial[256];

        TmodChoix *choixfilt;
        // TNotifyEvent FOnResize;

        // void __fastcall SetOnResize(TNotifyEvent Value);

        void __fastcall RAZ_Appli();
        void __fastcall Changed(char *tmp,int val);
        void __fastcall Insert_Project(char *nm);
        void __fastcall Delete_Project(char *nm);
	void __fastcall CreateMDIChild(const String Name);
        //int __fastcall ExtractValue(char *result, char *buff, char *tag, int posdeb);
        int __fastcall Recherche_Window(AnsiString str);
        void __fastcall History();
        void __fastcall Launch(char *s);
        void __fastcall Inserer_Menu(char *s);
        void __fastcall First (char *s);
        void __fastcall Build_History();
        void __fastcall OpenLast();
        void __fastcall WriteLast();
        int  __fastcall Extraire_CVS(char *fname);
        void __fastcall removequotes(char *str);
        int __fastcall Explode(char sep,char *str);
        int __fastcall InitKey();
        int __fastcall check_key(int val);
        int __fastcall DecCounter();


public:

        TplStandard *plST;


         virtual __fastcall TMainForm(TComponent *Owner);

};
//----------------------------------------------------------------------------
extern TMainForm *MainForm;
extern TMDIChild *__fastcall MDIChildCreate(void);

realisam *db;
int   Cancel;


//----------------------------------------------------------------------------
#endif
