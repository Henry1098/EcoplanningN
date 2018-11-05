//---------------------------------------------------------------------------

#ifndef modFilterH
#define modFilterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "plstd.h"
#include <Buttons.hpp>

#include "FinalIsam.h"
#include <ExtCtrls.hpp>
#include "AdvGrid.hpp"
#include "BaseGrid.hpp"
#include <Grids.hpp>
//---------------------------------------------------------------------------


class TxFilter : public TForm
{
__published:	// IDE-managed Components
        TListBox *lbFiltre;
        TMemo *mFiltre;
        TLabel *Label3;
        TButton *btExecuter;
        TButton *btStep;
        TButton *btMemor;
        TButton *btAide;
        TButton *btOK;
        TLabel *Label10;
        TButton *btDelete;
        TLabel *Label5;
        TLabel *Label11;
        TEdit *eMemoire;
        TButton *btEffacer;
        TButton *btNew;
        TButton *btAnnul;
        TButton *btLigne;
        TButton *btMod;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TLabel *Label6;
        TLabel *Label7;
        TAdvStringGrid *asFC;
        TBitBtn *btHaut;
        TBitBtn *btBas;
        TMemo *Memo1;
        TRadioGroup *rg1;
        TRadioButton *rbtn1;
        TRadioButton *rbtn2;
        TButton *btAffP;
        TButton *btAffR;
        TLabel *Label1;
        TMemo *Memo2;
        TButton *Button1;
        TAdvStringGrid *AdvStringGrid1;
        TButton *Button2;
        TAdvStringGrid *AdvStringGrid2;
        TMemo *Memo3;
        void __fastcall btExecuterClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall btDeleteClick(TObject *Sender);
        void __fastcall btOKClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall btMemorClick(TObject *Sender);
        void __fastcall lbFiltreClick(TObject *Sender);
        void __fastcall btNewClick(TObject *Sender);
        void __fastcall btStepClick(TObject *Sender);
        void __fastcall btAnnulClick(TObject *Sender);
        void __fastcall btLigneClick(TObject *Sender);
        void __fastcall btModClick(TObject *Sender);
        void __fastcall btEffacerClick(TObject *Sender);
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall BitBtn2Click(TObject *Sender);
        void __fastcall btHautClick(TObject *Sender);
        void __fastcall asFCClickCell(TObject *Sender, int ARow, int ACol);
        void __fastcall btBasClick(TObject *Sender);
        void __fastcall btAffPClick(TObject *Sender);
        void __fastcall asFCCanEditCell(TObject *Sender, int ARow,
          int ACol, bool &CanEdit);
        void __fastcall btAffRClick(TObject *Sender);
        void __fastcall btAideClick(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
        char ecoplan[100];
        realisam *filtres;
        char buffer[10000];
        char params[20][250];
        char current_filter[120];
        char champ[150];
        char oper[50];
        char val1[150];
        char val2[150];
        char action[50];
        char filename[250];
        int OldRow,OldCol;
        int step;
        bool change;
        int  Current_Row;
        char  mode[250];

        int __fastcall explode(char sep,char *str);
        int __fastcall ExtractValue(char *result, char *buff, char *tag, int posdeb);
        int __fastcall Control();
        void __fastcall Save_Preseance();
        void __fastcall Strip(char *s);
        void __fastcall Clean(int row);
        void __fastcall SaveData(int mode);
        void __fastcall Display_Filtre(char *s);
        void __fastcall Modify();
        void __fastcall ComparerFiltres(TStringList *List,Integer Index1, Integer Index2);
public:		// User declarations
        __fastcall TxFilter(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TxFilter *xFilter;
extern char *strfilter;

//---------------------------------------------------------------------------
#endif
