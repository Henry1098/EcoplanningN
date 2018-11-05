//---------------------------------------------------------------------------

#ifndef modTriH
#define modTriH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "AdvGrid.hpp"
#include "BaseGrid.hpp"
#include <Buttons.hpp>
#include <Grids.hpp>

#include "FinalIsam.h"
#include "modGhost.h"
#include <Menus.hpp>;
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TTri : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TListBox *lbTri;
        TMemo *mTri;
        TLabel *Label2;
        TButton *btLigne;
        TButton *btSupprimer;
        TButton *btAppliquer;
        TButton *btOk;
        TButton *btAnnul;
        TButton *btAide;
        TLabel *Label3;
        TEdit *eMemoire;
        TButton *btNew;
        TButton *btMod;
        TButton *btDelete;
        TAdvStringGrid *asFC;
        TBitBtn *btHaut;
        TBitBtn *btBas;
        TLabel *Label7;
        TButton *btMemor;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TLabel *Label9;
        TTimer *Timer1;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TButton *Button1;
        TButton *btAffPlann;
        TMemo *Memo1;
        TCheckBox *cb1;
        TLabel *Label11;
        TLabel *Label12;
        TLabel *Label10;
        TButton *Button2;
        TAdvStringGrid *AdvStringGrid1;
        TListBox *ListBox1;
        TAdvStringGrid *AdvStringGrid2;
        TButton *Button3;
        void __fastcall btLigneClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall btAnnulClick(TObject *Sender);
        void __fastcall asFCContextPopup(TObject *Sender, TPoint &MousePos,
          bool &Handled);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall btModClick(TObject *Sender);
        void __fastcall asFCClickCell(TObject *Sender, int ARow, int ACol);
        void __fastcall lbTriClick(TObject *Sender);
        void __fastcall btHautClick(TObject *Sender);
        void __fastcall btBasClick(TObject *Sender);
        void __fastcall btDeleteClick(TObject *Sender);
        void __fastcall btSupprimerClick(TObject *Sender);
        void __fastcall btOkClick(TObject *Sender);
        void __fastcall btMemorClick(TObject *Sender);
        void __fastcall btNewClick(TObject *Sender);
        void __fastcall btAppliquerClick(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall BitBtn2Click(TObject *Sender);
        void __fastcall btAffPlannClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Assign_Globals();
        void __fastcall cb1Click(TObject *Sender);
        void __fastcall btAideClick(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
private:	// User declarations

        int OldRow,OldCol;

        bool  change;
        realisam *tri;
        char  buffer[8000];
        char filename[250];
        char params[50][2000];
        char current_tri[250];
        char mode[250];
        // Memoire pour stocker les Caracteristiques titre en ligne ou en colonnes

        int __fastcall  Exec_Reports();
        int __fastcall  SaveData(int disp);
        //int __fastcall  ExtractValue(char *result, char *buff, char *tag, int posdeb);
        void __fastcall Strip(char *s);
        void __fastcall StripEnvelop(char *s);
        void __fastcall CStrip(char *s);
        void __fastcall Save_Preseance();
        int  __fastcall Explode(char sep,char *str);
        void __fastcall Clean(TAdvStringGrid *adv);
        void __fastcall CleanRow(int row);
        void __fastcall CleanMemory();
        void __fastcall Envoi_Params(int Row);
        void __fastcall Recupere_Params(int Row);
        void __fastcall Envoi_Params_TE(int Row);
        void __fastcall Recupere_Params_TE(int Row);
        void __fastcall Params_Env(int Row);
        void __fastcall Recup_Env(int Row);
        int __fastcall Display_Tri(char *s);
        void __fastcall Modify();
        void __fastcall Load_From_Globals();



public:		// User declarations
        __fastcall TTri(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTri *Tri;
//---------------------------------------------------------------------------
#endif
