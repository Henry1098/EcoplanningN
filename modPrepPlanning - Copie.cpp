//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#include <stdio.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>


#include "globals.h"
#include "modPrepPlanning.h"
#include "modEntete.h"
#include "modecadre.h"
#include "modDcal.h"
#include "modTaches.h"
#include "modCorps.h"
#include "modPied.h"
#include "modBandes.h"
#include "modMarges.h"
#include "modTri.h"
#include "modColonnes.h"
#include "modFilter.h"
#include "modTcol.h"
#include "modTligne.h"
#include "modDump.h"
#include "modGhost.h"
#include "modHelp.h"
#include "modImport.h" 
#include "globalvars.h"

#include "plstd.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGrid"
#pragma link "BaseGrid"
#pragma resource "*.dfm"
TPrepPlanning *PrepPlanning;
static int etat;
int etatsel;
int countertri;
int essai;
using namespace std;
//---------------------------------------------------------------------------
__fastcall TPrepPlanning::TPrepPlanning(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPrepPlanning::Button7Click(TObject *Sender)
{
etat = 0;
Close();
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btEnteteClick(TObject *Sender)
{
 TEntete *Entete;
 Entete = new TEntete(Application);
 Entete->ShowModal();
 delete Entete;
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btCadreClick(TObject *Sender)
{

 TCadre *Cadre;
 Cadre = new TCadre(Application);
 Cadre->ShowModal();
 delete Cadre;

}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btOKClick(TObject *Sender)
{
SaveData(0);
Label23->Caption = eMemoire->Text;
etat = 0;
Close();
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btCalClick(TObject *Sender)
{
 TFDcal *Cal;
 Cal = new TFDcal(Application);
 Cal->ShowModal();
 delete Cal;


}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btTachesClick(TObject *Sender)
{
TFDtaches *Taches;
 Taches = new TFDtaches(Application);
 Taches->ShowModal();
 delete Taches;
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btCorpsClick(TObject *Sender)
{
TFDcorps *Corps;
 Corps = new TFDcorps(Application);
 Corps->ShowModal();
 delete Corps;
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btPiedClick(TObject *Sender)
{
TFDpied *Pied;
 Pied = new TFDpied(Application);
 Pied->ShowModal();
 delete Pied;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::btMClick(TObject *Sender)
{

 TFDmarges *Marges;
 Marges = new TFDmarges(Application);
 Marges->ShowModal();
 delete Marges;

}
//---------------------------------------------------------------------------





void __fastcall TPrepPlanning::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 //if (plannings) plannings->CloseEngine();
 Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button3Click(TObject *Sender)
{
 TColonnes *Col;
 if (P_GENRE[pc] == 'C') return;
 strcpy(m_exchange,"<col>"); strcat(m_exchange,P_COLONNES[pc]); strcat(m_exchange,"</col>");
 strcat(m_exchange,"<util>utiliser</util>");
 Col = new TColonnes(Application);
 // Col->Height=705;
 // Col->Width=778;

 Col->ShowModal();
 delete Col;
 Label7->Caption= AnsiString(P_COLONNES[pc]);
 if (strlen(m_exchange)) Affect_Colonnes(P_COLONNES[pc]);

}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Init_Planning()
{
 int i;

 //rbPortrait->Checked=true;
 //cb10->Checked=true;
 //cb3->Checked=true;
 cb4->Checked=true;
 cb5->Checked=true;
 cb6->Checked=false;
 cb7->Checked=false;
 cb1->Checked=false;
 cb2->Checked=true;
 if (pc==0) cb2->Checked = false;

 rb12->Checked=true;
 rb1->Checked=true;

 eMemoire->Text="";
 mPlanning->Clear();
 Ghost->New_Planning();



}
void __fastcall TPrepPlanning::Button2Click(TObject *Sender)
{
 TTri *Tri;


 strcpy(m_exchange,"<tri>"); strcat(m_exchange,P_TRI[pc]); strcat(m_exchange,"</tri>");
 strcat(m_exchange,"<util>utiliser</util>");
 Tri = new TTri(Application);
 //Tri->Height=619;
 //Tri->Width=754;
Tri->ShowModal();  //Modal();
delete Tri;

 Label3->Caption = AnsiString(P_TRI[pc]);
 Affect_Tri(P_TRI[pc]);
 if(etatsel != 1)
 {
   etatsel = 1;

 }

   Button10->Click();
if(etatsel==1){
AnsiString direct = ExtractFileDir(ParamStr(0));

AnsiString nomplanning = eMemoire->Text;
AnsiString nomFile = direct+"\\Ref\\";


 AdvStringGrid2->SaveToFile(nomFile+nomplanning+"2");
               }


}
//---------------------------------------------------------------------------


void __fastcall TPrepPlanning::Button1Click(TObject *Sender)
{
 TxFilter *filt;
 char tmp[250];
 char strfilter[100];


 strcpy(m_exchange,"<util>utiliser</util><filt>");
 strcat(m_exchange,P_FILTRE[pc]); strcat(m_exchange,"</filt>");


 strcpy(m_project,DatabaseName);
 filt = new TxFilter(Application);
 filt->Height = 705;
 filt->Width = 774;

//  filt->Visible=true;
 m_filtre[0]=0;

filt->ShowModal(); //Modal();
delete filt;

 /*
 Label3->Caption = AnsiString(P_FILTRE[pc]);
 delete filt;
 Affect_Filtre(P_FILTRE[pc]);
 */

 if(etatsel != 1)
 {
   etatsel = 1;
 }
 Button5->Click();
 }
//---------------------------------------------------------------------------


void __fastcall TPrepPlanning::Save_Preseance()
{
FILE *fp;
int i,count;  char tmp[250];
strcpy(filename,m_directory); //DatabaseName);
strcat(filename,"\\eco_plannings.txt");
count=lbPlanning->Items->Count;
fp = fopen(filename,"wt");
for (i=0;i<count;i++)
  {
   sprintf(tmp,"<l>%s</l>\n",lbPlanning->Items->Strings[i].c_str());
   fputs(tmp,fp);
  }
fclose(fp);
}

void __fastcall TPrepPlanning::BufMarges()
{
 char tmp[50];

 buf_marges[0]=0;
 strcat(buf_marges,"<MRG>");
 strcat(buf_marges,"<mrg>");
 if (P_MARGES[pc]=='O') strcat(buf_marges,"O"); else strcat(buf_marges,"N");
 strcat(buf_marges,"</mrg>");

 strcat(buf_marges,"<mht>");
 strcat(buf_marges,P_MV1[pc].c_str());
 strcat(buf_marges,"</mht>");
 strcat(buf_marges,"<mba>");
 strcat(buf_marges,P_MV2[pc].c_str());
 strcat(buf_marges,"</mba>");
 strcat(buf_marges,"<mga>");
 strcat(buf_marges,P_MH1[pc].c_str());
 strcat(buf_marges,"</mga>");
 strcat(buf_marges,"<mdr>");
 strcat(buf_marges,P_MH2[pc].c_str());
 strcat(buf_marges,"</mdr>");
 strcat(buf_marges,"</MRG>");
}

void __fastcall TPrepPlanning::Bufferize(char *f,int indx)
{
 char tmp[50]; int c;

 strcat(f,"<txt>");
 strcat(f,PT[pc][indx].c_str());
 strcat(f,"</txt>");

 strcat(f,"<act>");
 strcat(f,PACTIF[pc][indx].c_str());
 strcat(f,"</act>");


 strcat(f,"<pol>");
 strcat(f,PF[pc][indx].c_str());
 strcat(f,"</pol>");

 strcat(f,"<fgd>");
 c=(int) PC[pc][indx]; sprintf(tmp,"%d",c); strcat(f,tmp);
 strcat(f,"</fgd>");

 strcat(f,"<ita>"); strcat(f,PN[pc][indx].c_str());  strcat(f,"</ita>");
// if (PN[pc][indx]=='N') strcat(f,"N"); else strcat(f,"I");
// strcat(f,"</ita>");

 strcat(f,"<bld>"); strcat(f,PX[pc][indx].c_str()); strcat(f,"</bld>");
// if (PX[pc][indx]=='N') strcat(f,"N"); else strcat(f,"B");
// strcat(f,"</bld>");

 strcat(f,"<siz>");
 sprintf(tmp,"%d",PH[pc][indx]);  strcat(f,tmp);
 strcat(f,"</siz>");

 strcat(f,"<bkg>");
 c=(int) PB[pc][indx]; sprintf(tmp,"%d",c); strcat(f,tmp);
 strcat(f,"</bkg>");

 strcat(f,"<jsh>"); strcat(f,PJH[pc][indx].c_str()); strcat(f,"</jsh>");


 strcat(f,"<jsv>");
 if (PJV[pc][indx]=='H') strcat(f,"H");
 else if (PJV[pc][indx]=='C') strcat(f,"C");
 else strcat(f,"B");
 strcat(f,"</jsv>");

 strcat(f,"<hgt>");
 sprintf(tmp,"%d",PHH[pc][indx]);  strcat(f,tmp);
 strcat(f,"</hgt>");

 strcat(f,"<lsp>");  // PTYPE  L,C, O/N
 tmp[0]=PTYPE[pc][indx];
 tmp[1]=0; strcat(f,tmp);
 strcat(f,"</lsp>");

}


void __fastcall TPrepPlanning::BufEntete()
{

  buf_entete[0]=0;
  strcat(buf_entete,"<ETT>");

  strcat(buf_entete,"<ett>");
  if (P_ENTETE[pc]=='O') strcat(buf_entete,"O</ett>");
    else strcat(buf_entete,"N</ett>");

  strcat(buf_entete,"<logn>");
  strcat(buf_entete,LOGONAME[pc].c_str());
  strcat(buf_entete,"</logn>");
  strcat(buf_entete,"<logp>");
  strcat(buf_entete,LOGOPOS[pc].c_str());
  strcat(buf_entete,"</logp>");

  strcat(buf_entete,"<eg1>");
  Bufferize(buf_entete,0);
  strcat(buf_entete,"</eg1>");

  strcat(buf_entete,"<eg2>");
  Bufferize(buf_entete,1);
  strcat(buf_entete,"</eg2>");

  strcat(buf_entete,"<eg3>");
  Bufferize(buf_entete,2);
  strcat(buf_entete,"</eg3>");

  strcat(buf_entete,"<ec1>");
  Bufferize(buf_entete,3);
  strcat(buf_entete,"</ec1>");

  strcat(buf_entete,"<ec2>");
  Bufferize(buf_entete,4);
  strcat(buf_entete,"</ec2>");

  strcat(buf_entete,"<ec3>");
  Bufferize(buf_entete,5);
  strcat(buf_entete,"</ec3>");

  strcat(buf_entete,"<ed1>");
  Bufferize(buf_entete,6);
  strcat(buf_entete,"</ed1>");

  strcat(buf_entete,"<ed2>");
  Bufferize(buf_entete,7);
  strcat(buf_entete,"</ed2>");

  strcat(buf_entete,"<ed3>");
  Bufferize(buf_entete,8);
  strcat(buf_entete,"</ed3>");

  strcat(buf_entete,"</ETT>");

}

void __fastcall TPrepPlanning::BufCal()
{
 char tmp[50];

 buf_cal[0]=0;
 strcat(buf_cal,"<CAL>");

 strcat(buf_cal,"<cah>");
 if (P_CALHAUT[pc]=='O') strcat(buf_cal,"O"); else strcat(buf_cal,"N");
 strcat(buf_cal,"</cah>");

 strcat(buf_cal,"<cab>");
 if (P_CALBAS[pc]=='O') strcat(buf_cal,"O"); else strcat(buf_cal,"N");
 strcat(buf_cal,"</cab>");

 strcat(buf_cal,"<cl1>");  // descriptif ligne 1    // unite, form ,freq, et INDEX = 9;
 strcat(buf_cal,"<unt>");  // unite
 strcat(buf_cal,P_UNIT1[pc].c_str());
 strcat(buf_cal,"</unt>");
 strcat(buf_cal,"<fmt>");  // format
 strcat(buf_cal,P_FORMATCAL1[pc].c_str());
 strcat(buf_cal,"</fmt>");
 strcat(buf_cal,"<frq>");  // frequence
 strcat(buf_cal,P_FREQ1[pc].c_str());
 strcat(buf_cal,"</frq>");
 // puis les attributs format d'edition classiques
 Bufferize(buf_cal,9);


 strcat(buf_cal,"</cl1>");

 strcat(buf_cal,"<cl2>");  // descriptif ligne 2    // unite, form ,freq, et INDEX = 9;
 strcat(buf_cal,"<unt>");  // unite
 strcat(buf_cal,P_UNIT2[pc].c_str());
 strcat(buf_cal,"</unt>");
 strcat(buf_cal,"<fmt>");  // format
 strcat(buf_cal,P_FORMATCAL2[pc].c_str());
 strcat(buf_cal,"</fmt>");
 strcat(buf_cal,"<frq>");  // frequence
 strcat(buf_cal,P_FREQ2[pc].c_str());
 strcat(buf_cal,"</frq>");
 // puis les attributs format d'edition classiques
 Bufferize(buf_cal,10);
 strcat(buf_cal,"</cl2>");

 strcat(buf_cal,"</CAL>");
}

void __fastcall TPrepPlanning::PrepTache(char *f,int indx)
{
 int i;   char tmp[200];

 strcat(buf_taches,"<tt>"); strcat(buf_taches,P_TRAIT[pc][indx].c_str()); strcat(buf_taches,"</tt>");
 strcat(buf_taches,"<tet>"); strcat(buf_taches,P_EPTRAIT[pc][indx].c_str()); strcat(buf_taches,"</tet>");
 strcat(buf_taches,"<tm>"); strcat(buf_taches,P_MARGE[pc][indx].c_str()); strcat(buf_taches,"</tm>");
 strcat(buf_taches,"<ts>"); strcat(buf_taches,P_STYLEM[pc][indx].c_str()); strcat(buf_taches,"</ts>");
 strcat(buf_taches,"<tcm>"); strcat(buf_taches,P_COLMARGE[pc][indx].c_str()); strcat(buf_taches,"</tcm>");
 strcat(buf_taches,"<tl>"); strcat(buf_taches,P_LIENS[pc][indx].c_str()); strcat(buf_taches,"</tl>");
 strcat(buf_taches,"<tsl>"); strcat(buf_taches,P_STYLELIEN[pc][indx].c_str()); strcat(buf_taches,"</tsl>");
 strcat(buf_taches,"<tcl>"); strcat(buf_taches,P_COL_LIEN[pc][indx].c_str()); strcat(buf_taches,"</tcl>");
 strcat(buf_taches,"<tel>"); strcat(buf_taches,P_EPAIS_LIEN[pc][indx].c_str()); strcat(buf_taches,"</tel>");

 for (i=0;i<4;i++)
    {
    sprintf(tmp,"<ti%d>",i);  strcat(buf_taches,tmp);
    strcat(buf_taches,"<inf>"); strcat(buf_taches,P_INF[pc][indx][i].c_str()); strcat(buf_taches,"</inf>");
    strcat(buf_taches,"<info>"); strcat(buf_taches,P_INFO[pc][indx][i].c_str()); strcat(buf_taches,"</info>");
    strcat(buf_taches,"<pol>"); strcat(buf_taches,P_POLT[pc][indx][i].c_str()); strcat(buf_taches,"</pol>");
    strcat(buf_taches,"<ctxt>"); strcat(buf_taches,P_COLTXTF[pc][indx][i].c_str()); strcat(buf_taches,"</ctxt>");
    strcat(buf_taches,"<ital>"); strcat(buf_taches,P_ITALT[pc][indx][i].c_str()); strcat(buf_taches,"</ital>");
    strcat(buf_taches,"<bold>"); strcat(buf_taches,P_BOLDT[pc][indx][i].c_str()); strcat(buf_taches,"</bold>");
    strcat(buf_taches,"<siz>"); strcat(buf_taches,P_SIZET[pc][indx][i].c_str()); strcat(buf_taches,"</siz>");
    strcat(buf_taches,"<cbkg>"); strcat(buf_taches,P_COLTXTB[pc][indx][i].c_str()); strcat(buf_taches,"</cbkg>");
    sprintf(tmp,"</ti%d>",i);  strcat(buf_taches,tmp);
   }

}

void __fastcall TPrepPlanning::BufTaches()
{

 buf_taches[0]=0;
 strcat(buf_taches,"<TCH>");
 strcat(buf_taches,"<tc0>");
 PrepTache(buf_taches,0);       // Prev Critique
 strcat(buf_taches,"</tc0>");
 strcat(buf_taches,"<tc1>");
 PrepTache(buf_taches,1);       // Prev non crit
 strcat(buf_taches,"</tc1>");

 strcat(buf_taches,"<tc2>");
 PrepTache(buf_taches,2);     // reel critique
 strcat(buf_taches,"</tc2>");

 strcat(buf_taches,"<tc3>");
 PrepTache(buf_taches,3);     // reel non critique
 strcat(buf_taches,"</tc3>");
 strcat(buf_taches,"</TCH>");
}

void __fastcall TPrepPlanning::BufCorps()
{
 char tmp[250];  int i;
 char str[250];

 strcpy(buf_corps,"");
 strcat(buf_corps,"<COR>");

 for (i=0;i<8;i++)
  {
    sprintf(str,"<c%d>",i);strcat(buf_corps,str);
    strcat(buf_corps,"<cyn>"); strcat(buf_corps,P_CYN[pc][i].c_str()); strcat(buf_corps,"</cyn>");
    strcat(buf_corps,"<ccfg>"); strcat(buf_corps,P_CCFG[pc][i].c_str()); strcat(buf_corps,"</ccfg>");
    //strcat(buf_corps,"<ccbk>"); strcat(buf_corps,P_CCBK[pc][i].c_str()); strcat(buf_corps,"</ccbk>");
    strcat(buf_corps,"<ccst>"); strcat(buf_corps,P_CCST[pc][i].c_str()); strcat(buf_corps,"</ccst>");
    strcat(buf_corps,"<ccep>"); strcat(buf_corps,P_CCEP[pc][i].c_str()); strcat(buf_corps,"</ccep>");
    sprintf(str,"</c%d>",i);strcat(buf_corps,str);
  }

 strcat(buf_corps,"<cynjt>"); strcat(buf_corps,P_CYNJT[pc].c_str()); strcat(buf_corps,"</cynjt>");
 strcat(buf_corps,"<cynjnt>"); strcat(buf_corps,P_CYNJNT[pc].c_str()); strcat(buf_corps,"</cynjnt>");
 strcat(buf_corps,"<cynwent>"); strcat(buf_corps,P_CYNWENT[pc].c_str()); strcat(buf_corps,"</cynwent>");

 strcat(buf_corps,"<cjt>"); strcat(buf_corps,P_CJT[pc].c_str()); strcat(buf_corps,"</cjt>");
 strcat(buf_corps,"<cjtfl>"); strcat(buf_corps,P_CJTFLAG[pc].c_str()); strcat(buf_corps,"</cjtfl>");
 strcat(buf_corps,"<cjnt>"); strcat(buf_corps,P_CJNT[pc].c_str()); strcat(buf_corps,"</cjnt>");
 strcat(buf_corps,"<cwent>"); strcat(buf_corps,P_CWENT[pc].c_str()); strcat(buf_corps,"</cwent>");
 strcat(buf_corps,"<cdh>"); strcat(buf_corps,P_CDH[pc].c_str()); strcat(buf_corps,"</cdh>");
 strcat(buf_corps,"<cdv>"); strcat(buf_corps,P_CDV[pc].c_str()); strcat(buf_corps,"</cdv>");

 strcat(buf_corps,"<ccjt>"); strcat(buf_corps,P_CCJT[pc].c_str()); strcat(buf_corps,"</ccjt>");
 strcat(buf_corps,"<ccjnt>"); strcat(buf_corps,P_CCJNT[pc].c_str()); strcat(buf_corps,"</ccjnt>");
 strcat(buf_corps,"<ccwent>"); strcat(buf_corps,P_CCWENT[pc].c_str()); strcat(buf_corps,"</ccwent>");
 strcat(buf_corps,"</COR>");

}

void __fastcall TPrepPlanning::BufPied()
{
 strcpy(buf_pied,"");
 strcat(buf_pied,"<PIE>");

  strcat(buf_pied,"<pg1>");
  Bufferize(buf_pied,25);
  strcat(buf_pied,"</pg1>");

  strcat(buf_pied,"<pg2>");
  Bufferize(buf_pied,26);
  strcat(buf_pied,"</pg2>");

  strcat(buf_pied,"<pg3>");
  Bufferize(buf_pied,27);
  strcat(buf_pied,"</pg3>");

  strcat(buf_pied,"<pc1>");
  Bufferize(buf_pied,28);
  strcat(buf_pied,"</pc1>");

  strcat(buf_pied,"<pc2>");
  Bufferize(buf_pied,29);
  strcat(buf_pied,"</pc2>");

  strcat(buf_pied,"<pc3>");
  Bufferize(buf_pied,30);
  strcat(buf_pied,"</pc3>");

  strcat(buf_pied,"<pd1>");
  Bufferize(buf_pied,31);
  strcat(buf_pied,"</pd1>");

  strcat(buf_pied,"<pd2>");
  Bufferize(buf_pied,32);
  strcat(buf_pied,"</pd2>");

  strcat(buf_pied,"<pd3>");
  Bufferize(buf_pied,33);
  strcat(buf_pied,"</pd3>");

 strcat(buf_pied,"</PIE>");

}

void __fastcall TPrepPlanning::BufTligne()
{
/*
 char tmp[3000];
 strcpy(buf_tligne,"");
 strcat(buf_tligne,"<TLIG>");
 tmp[0]=0;
 strcat(tmp,"<pol>"); strcat(tmp,PL_POL[pc].c_str()); strcat(tmp,"</pol>");
 strcat(tmp,"<ct>"); strcat(tmp,PL_CTEXT[pc].c_str()); strcat(tmp,"</ct>");
 strcat(tmp,"<it>"); strcat(tmp,PL_ITAL[pc].c_str()); strcat(tmp,"</it>");
 strcat(tmp,"<bd>"); strcat(tmp,PL_BOLD[pc].c_str()); strcat(tmp,"</bd>");
 strcat(tmp,"<ht>"); strcat(tmp,PL_HAUTEUR[pc].c_str()); strcat(tmp,"</ht>");
 strcat(tmp,"<jh>"); strcat(tmp,PL_JSH[pc].c_str()); strcat(tmp,"</jh>");
 strcat(tmp,"<jv>"); strcat(tmp,PL_JSV[pc].c_str()); strcat(tmp,"</jv>");
 strcat(tmp,"<cb>"); strcat(tmp,PL_CBKG[pc].c_str()); strcat(tmp,"</cb>");
 strcat(tmp,"<lg>"); strcat(tmp,PL_LARG[pc].c_str()); strcat(tmp,"</lg>");
 strcat(tmp,"<ep1>"); strcat(tmp,PL_LS1EP[pc].c_str()); strcat(tmp,"</ep1>");
 strcat(tmp,"<cl1>"); strcat(tmp,PL_LS1CL[pc].c_str()); strcat(tmp,"</cl1>");
 strcat(tmp,"<ls2>"); strcat(tmp,PL_LS2[pc].c_str()); strcat(tmp,"</ls2>");
 strcat(tmp,"<ep2>"); strcat(tmp,PL_LS2EP[pc].c_str()); strcat(tmp,"</ep2>");
 strcat(tmp,"<cl2>"); strcat(tmp,PL_LS2CL[pc].c_str()); strcat(tmp,"</cl2>");

 strcat(buf_tligne,tmp);
 strcat(buf_tligne,"</TLIG>");
*/

}

void __fastcall TPrepPlanning::BufTcol()
{
/*
 char tmp[3000];
 strcpy(buf_tcol,"");
 strcat(buf_tcol,"<TCOL>");

 tmp[0]=0;
 strcat(tmp,"<pol>"); strcat(tmp,PC_POL[pc].c_str()); strcat(tmp,"</pol>");
 strcat(tmp,"<ct>"); strcat(tmp,PC_CTEXT[pc].c_str()); strcat(tmp,"</ct>");
 strcat(tmp,"<it>"); strcat(tmp,PC_ITAL[pc].c_str()); strcat(tmp,"</it>");
 strcat(tmp,"<bd>"); strcat(tmp,PC_BOLD[pc].c_str()); strcat(tmp,"</bd>");
 strcat(tmp,"<ht>"); strcat(tmp,PC_HAUTEUR[pc].c_str()); strcat(tmp,"</ht>");
 strcat(tmp,"<jh>"); strcat(tmp,PC_JSH[pc].c_str()); strcat(tmp,"</jh>");
 strcat(tmp,"<jv>"); strcat(tmp,PC_JSV[pc].c_str()); strcat(tmp,"</jv>");
 strcat(tmp,"<cb>"); strcat(tmp,PC_CBKG[pc].c_str()); strcat(tmp,"</cb>");
 strcat(tmp,"<lg>"); strcat(tmp,PC_LARG[pc].c_str()); strcat(tmp,"</lg>");
 strcat(tmp,"<ep1>"); strcat(tmp,PC_LS1EP[pc].c_str()); strcat(tmp,"</ep1>");
 strcat(tmp,"<cl1>"); strcat(tmp,PC_LS1CL[pc].c_str()); strcat(tmp,"</cl1>");
 strcat(tmp,"<ls2>"); strcat(tmp,PC_LS2[pc].c_str()); strcat(tmp,"</ls2>");
 strcat(tmp,"<ep2>"); strcat(tmp,PC_LS2EP[pc].c_str()); strcat(tmp,"</ep2>");
 strcat(tmp,"<cl2>"); strcat(tmp,PC_LS2CL[pc].c_str()); strcat(tmp,"</cl2>");

 strcat(buf_tcol,tmp);
 strcat(buf_tcol,"</TCOL>");
*/
}



void __fastcall TPrepPlanning::BufCadre()
{
 char tmp[50]; int c;
 float ff;

 strcpy(buf_cadre,"");
 strcat(buf_cadre,"<CDR>");

 strcat(buf_cadre,"<cad>");
 if (P_CADRE[pc]=='O') strcat(buf_cadre,"O"); else strcat(buf_cadre,"N");
 strcat(buf_cadre,"</cad>");
 strcat(buf_cadre,"<cep>");

 ff = AnsiToFloat(P_ELC[pc]);
 sprintf(tmp,"%f",ff); strcat(buf_cadre,tmp);
 strcat(buf_cadre,"</cep>");

 // c= (int)P_CLC[pc];
 strcat(buf_cadre,"<ccl>");
 //sprintf(tmp,"%d",);
 strcat(buf_cadre,P_CLC[pc].c_str());
 strcat(buf_cadre,"</ccl>");
 strcat(buf_cadre,"</CDR>");

}


void __fastcall TPrepPlanning::BufGen()
{
 char tmp[250];


  strcpy(buf_gen,"<GEN>");
  strcpy(tmp,eMemoire->Text.c_str());

 strcat(buf_gen,"<nom>"); strcat(buf_gen,tmp); strcat(buf_gen,"</nom>");
 strcat(buf_gen,"<com>");
 strcat(buf_gen,mPlanning->Lines->GetText());
 strcat(buf_gen,"</com>");
 strcat(buf_gen,"<fut>");
 strcat(buf_gen,P_FILTRE[pc]);
 strcat(buf_gen,"</fut>");
 strcat(buf_gen,"<tut>");
 strcat(buf_gen,P_TRI[pc]);
 strcat(buf_gen,"</tut>");
 strcat(buf_gen,"<cut>");
 strcat(buf_gen,P_COLONNES[pc]);
 strcat(buf_gen,"</cut>");

 strcat(buf_gen,"<gen>");
 if (rb11->Checked==true)
    strcat(buf_gen,"C"); else strcat(buf_gen,"G");

 strcat(buf_gen,"</gen>");
 strcat(buf_gen,"<ggt>");
 if (pc==0) cb2->Checked = false;
 //if(cb2->Checked==true) strcat(buf_gen,"O"); else strcat(buf_gen,"N");
 if (P_GRILLE[pc]=='O') strcat(buf_gen,"O"); else strcat(buf_gen,"N");
 strcat(buf_gen,"</ggt>");
 strcat(buf_gen,"<tpl>");
 if (rb1->Checked==true) strcat(buf_gen,"P"); else strcat(buf_gen,"R");
// if (P_TYPE[pc]=='P') strcat(buf_gen,"P"); else strcat(buf_gen,"R");
 strcat(buf_gen,"</tpl>");
 strcat(buf_gen,"<lbr>");
 if(cb1->Checked==true) strcat(buf_gen,"O"); else strcat(buf_gen,"N");
// if (P_LBRISE[pc]=='O') strcat(buf_gen,"O"); else strcat(buf_gen,"N");
 strcat(buf_gen,"</lbr>");

 strcat(buf_gen,"<mod>");
 //if (rbPortrait->Checked==true)
 strcat(buf_gen,"P");
 //else strcat(buf_gen,"L");
 strcat(buf_gen,"</mod>");

 strcat(buf_gen,"<mrg>");
 //if (cb10->Checked==true)
 strcat(buf_gen,"O"); // else strcat(buf_gen,"N");
 strcat(buf_gen,"</mrg>");

 strcat(buf_gen,"<cad>");
 //if (cb3->Checked==true)
 strcat(buf_gen,"O"); //else strcat(buf_gen,"N");
 strcat(buf_gen,"</cad>");

 strcat(buf_gen,"<ett>");
 if (cb4->Checked==true) strcat(buf_gen,"O"); else strcat(buf_gen,"N");
 strcat(buf_gen,"</ett>");

 strcat(buf_gen,"<cah>");
 if (cb5->Checked==true) strcat(buf_gen,"O"); else strcat(buf_gen,"N");
 strcat(buf_gen,"</cah>");

 strcat(buf_gen,"<cab>");
 if (cb6->Checked==true) strcat(buf_gen,"O"); else strcat(buf_gen,"N");
 strcat(buf_gen,"</cab>");

 strcat(buf_gen,"<pie>");
 if (cb7->Checked==true) strcat(buf_gen,"O"); else strcat(buf_gen,"N");
 strcat(buf_gen,"</pie>");

 strcat(buf_gen,"<pdet>");
 sprintf(tmp,"%d",P_DETAIL[pc]);
 strcat(buf_gen,tmp);
 strcat(buf_gen,"</pdet>");
 strcat(buf_gen,"</GEN>");

}

void __fastcall TPrepPlanning::SaveData(int mode)
{
 char name[100];
 char TempKey[100],Key[100],RetKey[100];
 int rc,recdata,i,nbitems;
 char tmp[2000];
 char filename[250];
 char deb[100];

 FILE *fp;
 int count;


 strcpy(TempKey,eMemoire->Text.c_str());
 if (strlen(TempKey) == 0)
   {
    Application->MessageBoxA("Pas de Nom de Planning",m_ecoplan,MB_OK);
    return;
   }

   

strcpy(tmp,m_directory); strcat(tmp,"\\Reference\\Plannings\\"); strcat(tmp,TempKey); strcat(tmp,".pln");
fp = fopen(tmp,"wt");

 // preparer buffer
 BufGen();
 BufMarges();
 BufEntete();
 BufCal();
 BufCadre();
 BufTaches();
 BufCorps();
 BufPied();
 BufBrisees();
 //BufTligne();
 //BufTcol();


 if (strlen(P_TRI[pc])) Ghost->Save_GlobTri(P_TRI[pc]);
 if (strlen(P_COLONNES[pc])) Ghost->Save_GlobColonnes(P_COLONNES[pc]);

 strcpy(filename,m_directory);  //DatabaseName);
 strcat(filename,"\\eco_plannings");
 rc = plannings->OpenEngine(filename,1016,"ndx","dat");   //1024 - 8
 rc = plannings->SetIndexMode(0,0);

 strcpy(Key,TempKey); strcat(Key,"-GEN");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_gen, strlen(buf_gen)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_gen,strlen(buf_gen)+1);
       lbPlanning->Items->Add(AnsiString(TempKey));
     }
 strcat(buf_gen,"\n");
 fputs(buf_gen,fp);

 strcpy(Key,TempKey); strcat(Key,"-MRG");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_marges, strlen(buf_marges)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_marges,strlen(buf_marges)+1);
     }
  strcat(buf_marges,"\n");
  fputs(buf_marges,fp);

 strcpy(Key,TempKey); strcat(Key,"-ETT");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_entete, strlen(buf_entete)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_entete,strlen(buf_entete)+1);
     }
  strcat(buf_entete,"\n");
  fputs(buf_entete,fp);


 strcpy(Key,TempKey); strcat(Key,"-CAL");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_cal, strlen(buf_cal)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_cal,strlen(buf_cal)+1);
     }
 strcat(buf_cal,"\n");
 fputs(buf_cal,fp);

 strcpy(Key,TempKey); strcat(Key,"-CDR");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_cadre, strlen(buf_cadre)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_cadre,strlen(buf_cadre)+1);
     }
  strcat(buf_cadre,"\n");
  fputs(buf_cadre,fp);

 strcpy(Key,TempKey); strcat(Key,"-TCH");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_taches, strlen(buf_taches)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_taches,strlen(buf_taches)+1);
     }
 strcat(buf_taches,"\n");
 fputs(buf_taches,fp);

 strcpy(Key,TempKey); strcat(Key,"-COR");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);

 //sprintf(deb,"size of buf_cor %d",strlen(buf_corps));
 //Application->MessageBox(deb,"ecopl",MB_OK);
 if (rc) rc = plannings->RewriteRecord(buf_corps, strlen(buf_corps)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_corps,strlen(buf_corps)+1);
     }
  strcat(buf_corps,"\n");
  fputs(buf_corps,fp);

 strcpy(Key,TempKey); strcat(Key,"-PIE");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_pied, strlen(buf_pied)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_pied,strlen(buf_pied)+1);
     }
  strcat(buf_pied,"\n");
  fputs(buf_pied,fp);


 strcpy(Key,TempKey); strcat(Key,"-TLIG");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_tligne, strlen(buf_tligne)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_tligne,strlen(buf_tligne)+1);
     }
 // fputs(buf_gen,fp);

 strcpy(Key,TempKey); strcat(Key,"-TCOL");
 rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = plannings->RewriteRecord(buf_tcol, strlen(buf_tcol)+1,recdata);
     else
     { recdata=plannings->GetNewRecordNumber();
       rc = plannings->WriteKey(0,Key,recdata);
       rc = plannings->WriteRecord(buf_tcol,strlen(buf_tcol)+1);
     }
//  fputs(buf_gen,fp);

Save_Preseance();
plannings->CloseEngine();
fclose(fp);  // close the export file
change = false;
if (mode !=0) Application->MessageBoxA("Planning Sauvegard�",m_ecoplan,MB_OK);
}


void __fastcall TPrepPlanning::BufBrisees()
{

}

void __fastcall TPrepPlanning::Button6Click(TObject *Sender)
{

/* if(etatsel == 1)
{
 if(ListBox1->Items->IndexOf("<l>"+eMemoire->Text+"</l>") > -1){
 ShowMessage("Enregistrez le fichier sous un autre nom");
 return;}
} */

 int indx,count;
 // inserer dans la listbox;

 SaveData(1);
 Label23->Caption = eMemoire->Text;

 indx = lbPlanning->ItemIndex;
 if (indx == -1) return;
 count = lbPlanning->Items->Count;
 //if (indx == 0) {  }
 //else { lbPlanning->Items->Move(indx,0);  lbPlanning->ItemIndex=0; }
 AnsiString tri = Label3->Caption;
 AnsiString filtre = Label5->Caption;
 AnsiString plan = Label23->Caption;
  AnsiString direct = ExtractFileDir(ParamStr(0));
 AnsiString nomFile = direct+"\\Ref\\";
 AnsiString N1 =nomFile+plan+" "+tri;
 AnsiString N2 =nomFile+plan+" "+filtre;
AdvStringGrid1->SaveToFile(N2);
AdvStringGrid2->SaveToFile(N1);


}
//---------------------------------------------------------------------------





void __fastcall TPrepPlanning::BitBtn1Click(TObject *Sender)
{
 // Up Move
  int indx,count;
 char tmp1[100],tmp2[100];

 indx = lbPlanning->ItemIndex;
 if (indx == -1) return;
 count = lbPlanning->Items->Count;
 if (indx == 0) { lbPlanning->Items->Move(indx,count-1); lbPlanning->ItemIndex=count-1; }
 else { lbPlanning->Items->Move(indx,indx-1);  lbPlanning->ItemIndex=indx-1; }
 Save_Preseance();
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::BitBtn2Click(TObject *Sender)
{
// Down Move
 int indx,count;
 char tmp1[100],tmp2[100];

 indx = lbPlanning->ItemIndex;
 if (indx == -1) return;
 count = lbPlanning->Items->Count;
 if (indx == count-1) { lbPlanning->Items->Move(indx,0); lbPlanning->ItemIndex=0; }

 else { lbPlanning->Items->Move(indx,indx+1); lbPlanning->ItemIndex=indx + 1; }
 Save_Preseance();
}
//---------------------------------------------------------------------------



int __fastcall TPrepPlanning::Explode(char sep,char *str)
{
 int i, count, ip;
 char c,cpred;
 if (strlen(str)==0) return 0;
 count = 0; params[count][0]=0; // indice params
 c=str[0]; cpred=0;
 i=0; ip=0;
 while (c)
  { cpred=c;
   if (c==sep)
     { params[count][ip]=0; count++; ip=0; if (count > 19) count=19; }
   else
     { params[count][ip]=c;  ip++; }
   i++; c=str[i];
  }
 if (cpred==sep) count--; return (count+1);
}


void __fastcall TPrepPlanning::Button9Click(TObject *Sender)
{
 //  Nettoyer la "forme"... et initialiser par defaut;
 eMemoire->Text="";
 mPlanning->Clear();
 Label3->Caption="";
 Label5->Caption="";
 Label7->Caption="";
 Label3->Caption="";

 /*
  TImport *Imp;
 strcpy(m_impt,"<type>"); strcat(m_impt,"pln"); strcat(m_impt,"</type>");

 Imp = new TImport(Application);
 Imp->Caption = "Import Donn�es Planning";
 Imp->ShowModal();
 delete Imp;
*/

 //Ghost->New_Planning();
 //Application->MessageBoxA("Planning Initialis� avec les Valeurs par D�faut",m_ecoplan,MB_OK);
 etat = 0;
 }
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::lbPlanningClick(TObject *Sender)
{
int indx; char tmp[5000];   AnsiString Content;
int rc,recdata; char Key[200],RetKey[200];


indx = lbPlanning->ItemIndex;
if (indx == -1) return;

if (change) { SaveData(1); change=0; }
 strcpy(tmp,lbPlanning->Items->Strings[indx].c_str());
 strcpy(current_planning,tmp);
 strcpy(LastPlanning[pc],current_planning);
 strcpy(P_NOM[pc],current_planning);
 Ghost->LoadPlanningFromFile(tmp);
 //lbPlanning->Items->Move(indx,0);
 //lbPlanning->ItemIndex=0;
 Content = lbPlanning->Items->Strings[indx];

// lbPlanning->Items->Delete(indx);
// lbPlanning->Items->Insert(0,Content);

 Maj_Decor();
 AnsiString tri = Label3->Caption;
 AnsiString filtre = Label5->Caption;
 AnsiString plan = Label23->Caption;
  AnsiString direct = ExtractFileDir(ParamStr(0));
 AnsiString nomFile = direct+"\\Ref\\";
 AnsiString N1 =nomFile+plan+" "+tri;
 AnsiString N2 =nomFile+plan+" "+filtre;

 if(filtre != "")
 {

  AdvStringGrid1->LoadFromFile(N2);

 }else{
 for(int i = 0; i < AdvStringGrid1->ColCount; i++){
  AdvStringGrid1->RowCount = 2;
  AdvStringGrid1->Cells[i][1] = "";

  }
 }
 if(tri != "")
 {

  AdvStringGrid2->LoadFromFile(N1);
  AdvStringGrid2->ColWidths[1]=79;


 }else{
   for(int i = 1; i < AdvStringGrid2->RowCount; i++){

  AdvStringGrid2->RowCount = 2;
  AdvStringGrid2->Cells[1][i] = "";
  AdvStringGrid2->Cells[2][i] = "";
    AdvStringGrid2->Cells[3][i] = "";
  AdvStringGrid2->Cells[4][i] = "";
  AdvStringGrid2->Cells[5][i] = "";
    AdvStringGrid2->Cells[6][i] = "";
  AdvStringGrid2->Cells[7][i] = "";
  AdvStringGrid2->Cells[8][i] = "";
    AdvStringGrid2->Cells[9][i] = "";
  AdvStringGrid2->Cells[10][i] = "";
  AdvStringGrid2->Cells[11][i] = "";
    AdvStringGrid2->Cells[12][i] = "";
  AdvStringGrid2->Cells[13][i] = "";
   }
    }
  if(etatsel ==1)
  {
   etatsel =0;
  }

AdvStringGrid1->HideColumn(0);

if(filtre !=""){
AdvStringGrid1->ColCount = 5;
AdvStringGrid1->ColWidths[0]=80;
AdvStringGrid1->ColWidths[1]=95;
AdvStringGrid1->ColWidths[2]=80;
AdvStringGrid1->ColWidths[3]=80;
AdvStringGrid1->ColWidths[4]=60;

}


  lbPlanning->Items->Move(indx,0);  lbPlanning->ItemIndex=0;
  countertri++;
  AdvStringGrid2->ColWidths[1]=79;

 AdvStringGrid1->Refresh();
  AdvStringGrid2->Refresh();
 }
//---------------------------------------------------------------------------



void __fastcall TPrepPlanning::Affect_Filtre(char *s)
{
 Label5->Caption = AnsiString(s);
}

void __fastcall TPrepPlanning::Affect_Colonnes(char *s)
{
 int recdata,rc; char Key[200],RetKey[200];
 char tmp[5000];

 Label7->Caption = AnsiString(s);
 if (strlen(s)==0) return;

 //Ghost->Global_Col(s);
}


void __fastcall TPrepPlanning::StripC(char *s)
{
 char tmp[500];
 int Row;   int count; int colcnt;
int i,k;



 Ghost->ExtractValue(tmp,s,"champs",0);
 count = Explode('|',s);
 P_COL[pc]=count+1;
 if (count)
   {
     for (i=8; i<=count+8; i++)
       {
        k=i;
        P_TRICOL[pc][i]=0;
        P_ATTR[pc][i]='C';

        // Colonnes Detail
       Ghost->ExtractValue(tmp,params[i-1],"ch",0);
       P_FIELD[pc][k] = AnsiString(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"ft",0);
       P_FONT[pc][k] = AnsiString(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"cl",0);
       P_CFCOLOR[pc][k] = atoi(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"it",0);
       P_ITAL[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"gr",0);
       P_BOLD[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"ta",0);
       P_HAUT[pc][k] = atoi(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"cf",0);
       P_CBCOLOR[pc][k] = atoi(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"jh",0);
       P_JH[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"jv",0);
       P_JV[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"ht",0);      // largeur
       P_LARG[pc][k] = atoi(tmp);

       Ghost->ExtractValue(tmp,params[i-1],"tch",0);
       T_TITRE_COL[pc][k] = AnsiString(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"tft",0);
       T_FONT[pc][k] = AnsiString(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"tcl",0);
       T_CFCOLOR[pc][k] = atoi(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"tit",0);
       T_ITAL[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"tgr",0);
       T_BOLD[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"tta",0);
       T_HAUT[pc][k] = atoi(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"tcf",0);
       T_CBCOLOR[pc][k] = atoi(tmp);
       Ghost->ExtractValue(tmp,params[i-1],"tjh",0);
       T_JH[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"tjv",0);
       T_JV[pc][k] = tmp[0];
       Ghost->ExtractValue(tmp,params[i-1],"tht",0);      // largeur
       T_LARG[pc][k] = atoi(tmp);
      }
   }


}

void __fastcall TPrepPlanning::Affect_Tri(char *s)
{
 Label3->Caption = AnsiString(s);
}


void __fastcall TPrepPlanning::LoadData(char *cp)
{
  int rc,recdata; char Key[200],RetKey[200];

  // Donnees generales
    strcpy(Key,cp); strcat(Key,"-GEN");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_gen,recdata);
        Ghost->ExtractValue(str,buf_gen,"GEN",0);
        LoadGen();
      }

    strcpy(Key,cp); strcat(Key,"-MRG");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_marges,recdata);
        Ghost->ExtractValue(str,buf_marges,"MRG",0);
        LoadMarges();
      }

    strcpy(Key,cp); strcat(Key,"-CDR");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_cadre,recdata);
        Ghost->ExtractValue(str,buf_cadre,"CDR",0);
        LoadCadre();
      }

   strcpy(Key,cp); strcat(Key,"-ETT");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_entete,recdata);
        Ghost->ExtractValue(str,buf_entete,"ETT",0);
        LoadEntete();
      }

 //  Calendrier
   strcpy(Key,cp); strcat(Key,"-CAL");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_cal,recdata);
        Ghost->ExtractValue(str,buf_cal,"CAL",0);
        LoadCalendrier();
      }
 // Taches
   strcpy(Key,cp); strcat(Key,"-TCH");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_taches,recdata);
        Ghost->ExtractValue(str,buf_taches,"TCH",0);
        LoadTaches();
      }
 // Corps Graphe
   strcpy(Key,cp); strcat(Key,"-COR");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_corps,recdata);
        Ghost->ExtractValue(str,buf_corps,"COR",0);
        LoadCorps();
      }


 // Pied de Page
    strcpy(Key,cp); strcat(Key,"-PIE");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_pied,recdata);
        Ghost->ExtractValue(str,buf_pied,"PIE",0);
        LoadPied();
      }

 // Bandes  Lignes

   strcpy(Key,cp); strcat(Key,"-TLIG");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_tligne,recdata);
        Ghost->ExtractValue(str,buf_tligne,"TLIG",0);
        LoadTLigne();
      }

 // Bandes  Colonnes

   strcpy(Key,cp); strcat(Key,"-TCOL");
    rc = plannings->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { rc =plannings->ReadRecord(buf_tcol,recdata);
        Ghost->ExtractValue(str,buf_tcol,"TCOL",0);
        LoadTCol();
      }
}


void __fastcall TPrepPlanning::LoadMem(char *s,int indx)
{
 char tmp[250];
 int c;
 Ghost->ExtractValue(tmp,s,"txt",0);
 PT[pc][indx]=AnsiString(tmp);

 Ghost->ExtractValue(tmp,s,"act",0);   // Actif O/N
 PACTIF[pc][indx]=AnsiString(tmp);

 Ghost->ExtractValue(tmp,s,"pol",0);
 PF[pc][indx]=AnsiString(tmp);
 Ghost->ExtractValue(tmp,s,"fgd",0); c=atoi(tmp);
 PC[pc][indx]=TColor(c);
 Ghost->ExtractValue(tmp,s,"bkg",0); c=atoi(tmp);
 PB[pc][indx]=TColor(c);
 Ghost->ExtractValue(tmp,s,"siz",0); c=atoi(tmp);
 PH[pc][indx]= c;;
 Ghost->ExtractValue(tmp,s,"hgt",0); c=atoi(tmp);
 PHH[pc][indx]=c;
 Ghost->ExtractValue(tmp,s,"ita",0);
 //if (strcmp(tmp,"N")==0) PN[pc][indx]='N'; else PN[pc][indx]='I';
 PN[pc][indx]= AnsiString(tmp);

 Ghost->ExtractValue(tmp,s,"bld",0);
 PX[pc][indx]= AnsiString(tmp);
 //if (strcmp(tmp,"N")==0) PX[pc][indx]='N'; else PX[pc][indx]='B';
 Ghost->ExtractValue(tmp,s,"jsh",0);
 PJH[pc][indx]= AnsiString(tmp);
 //if (strcmp(tmp,"G")==0) PJH[pc][indx]='G';
 //  else  if (strcmp(tmp,"C")==0) PJH[pc][indx]='C';
 //  else PJH[pc][indx]='D';
 Ghost->ExtractValue(tmp,s,"jsv",0);
 if (strcmp(tmp,"H")==0) PJV[pc][indx]='G';
   else  if (strcmp(tmp,"C")==0) PJV[pc][indx]='C';
   else PJV[pc][indx]='B';

 PTYPE[pc][indx]='N';
 // PACTIF[pc][indx]='O';
 P_ORIENT[pc][indx]='L';

}

void __fastcall TPrepPlanning::LoadGen()
{
 char tmp[2500];

 Ghost->ExtractValue(tmp,str,"nom",0);
 strcpy(P_NOM[pc],tmp);
 eMemoire->Text=AnsiString(tmp);
 Ghost->ExtractValue(tmp,str,"com",0);
 mPlanning->SetTextBuf(tmp);
 Ghost->ExtractValue(tmp,str,"fut",0);
 strcpy(P_FILTRE[pc],tmp);
 Affect_Filtre(P_FILTRE[pc]);
 Ghost->ExtractValue(tmp,str,"tut",0);
 strcpy(P_TRI[pc],tmp);
 Affect_Tri(P_TRI[pc]);
 Ghost->ExtractValue(tmp,str,"cut",0);
 strcpy(P_COLONNES[pc],tmp);
 Affect_Colonnes(P_COLONNES[pc]);

 Ghost->ExtractValue(tmp,str,"pdet",0);
 P_DETAIL[pc]=atoi(tmp);



 Ghost->ExtractValue(tmp,str,"gen",0);
 if (strcmp(tmp,"C")==0) { P_GENRE[pc]='C'; rb11->Checked=true;rb12->Checked=false; cb2->Checked=false;}
     else  { P_GENRE[pc]='G'; rb12->Checked=true; rb11->Checked=false;}

 Ghost->ExtractValue(tmp,str,"ggt",0);
 if (pc==0) strcpy(tmp,"N"); 
 if (strcmp(tmp,"O")==0) { P_GRILLE[pc]='O'; cb2->Checked=true; }
     else { P_GRILLE[pc]='N'; cb2->Checked=false; }
 Ghost->ExtractValue(tmp,str,"tpl",0);
 if (strcmp(tmp,"P")==0){ P_TYPE[pc]='P'; rb1->Checked=true; rb2->Checked=false;}
     else { P_TYPE[pc]='R'; rb2->Checked = true; rb1->Checked=false; }
 Ghost->ExtractValue(tmp,str,"lbr",0);
 if (strcmp(tmp,"O")==0)  { P_LBRISE[pc]='O'; cb1->Checked = true; }
     else { P_LBRISE[pc]='N'; rb1->Checked = false; }
 Ghost->ExtractValue(tmp,str,"mod",0);
 //if (strcmp(tmp,"P")==0)  {
   P_MODE[pc]='P'; //rbPortrait->Checked=true; rbPaysage->Checked=false;}
 //    else { P_MODE[pc]='L'; rbPaysage->Checked=true; rbPortrait->Checked=false; }
 Ghost->ExtractValue(tmp,str,"mrg",0);
  P_MARGES[pc]='O';
 //if (strcmp(tmp,"O")==0) { P_MARGES[pc]='O'; }
 //    else { P_MARGES[pc]='N';  }

 Ghost->ExtractValue(tmp,str,"cad",0);
 P_CADRE[pc]='O';
 //if (strcmp(tmp,"O")==0) { P_CADRE[pc]='O'; cb3->Checked=true; }
 //    else { P_CADRE[pc]='N'; cb3->Checked=false; }

 Ghost->ExtractValue(tmp,str,"ett",0);
 if (strcmp(tmp,"O")==0) { P_ENTETE[pc]='O'; cb4->Checked=true; }
     else { P_ENTETE[pc]='N'; cb4->Checked=false; }

 Ghost->ExtractValue(tmp,str,"cah",0);
 if (strcmp(tmp,"O")==0) { P_CALHAUT[pc]='O'; cb5->Checked=true; }
     else { P_CALHAUT[pc]='N'; cb5->Checked=false; }

 Ghost->ExtractValue(tmp,str,"cab",0);
 if (strcmp(tmp,"O")==0) { P_CALBAS[pc]='O'; cb6->Checked=true; }
     else { P_CALBAS[pc]='N'; cb6->Checked=false; }

 Ghost->ExtractValue(tmp,str,"pie",0);
 if (strcmp(tmp,"O")==0) { P_PIED[pc]='O'; cb7->Checked=true; }
     else { P_PIED[pc]='N'; cb7->Checked=false; }

 
}

void __fastcall TPrepPlanning::LoadMarges()
{
 char tmp[200];

 Ghost->ExtractValue(tmp,str,"mrg",0);
 if (strcmp(tmp,"O")==0) P_MARGES[pc]='O';
     else P_MARGES[pc]='N';
 Ghost->ExtractValue(tmp,str,"mht",0); P_MV1[pc]= AnsiString(tmp);
 Ghost->ExtractValue(tmp,str,"mba",0); P_MV2[pc]= AnsiString(tmp);
 Ghost->ExtractValue(tmp,str,"mga",0); P_MH1[pc]= AnsiString(tmp);
 Ghost->ExtractValue(tmp,str,"mdr",0); P_MH2[pc]= AnsiString(tmp);
}

void __fastcall TPrepPlanning::LoadCadre()
{

 char tmp[200];float ff;
 int  val;
 Ghost->ExtractValue(tmp,str,"cad",0);
 if (strcmp(tmp,"O")==0) P_CADRE[pc]='O';
     else P_CADRE[pc]='N';

 Ghost->ExtractValue(tmp,str,"cep",0);
 P_ELC[pc]=AnsiString(tmp);
 ff =AnsiToFloat(P_ELC[pc]); ff=ff*COEFMM;
 P_ELCF[pc] = (int) ff;

 Ghost->ExtractValue(tmp,str,"ccl",0); val=atoi(tmp);
 P_CLC[pc]=(TColor)val;

}


float __fastcall TPrepPlanning::AnsiToFloat(AnsiString str)
{
 char tmp[200]; char *p;
 float f;
 strcpy(tmp,str.c_str());

 p=strchr(tmp,',');
 if (p) *p='.';
 f=atof(tmp);
 return f;

}

void __fastcall TPrepPlanning::LoadEntete()
{
 char tmp[2000];

 Ghost->ExtractValue(tmp,str,"ett",0);
 if (strcmp(tmp,"O")==0) P_ENTETE[pc]='O';
     else P_ENTETE[pc]='N';
 Ghost->ExtractValue(tmp,str,"eg1",0);
 LoadMem(tmp,0);
 Ghost->ExtractValue(tmp,str,"eg2",0);
 LoadMem(tmp,1);
 Ghost->ExtractValue(tmp,str,"eg3",0);
 LoadMem(tmp,2);
 Ghost->ExtractValue(tmp,str,"ec1",0);
 LoadMem(tmp,3);
 Ghost->ExtractValue(tmp,str,"ec2",0);
 LoadMem(tmp,4);
 Ghost->ExtractValue(tmp,str,"ec3",0);
 LoadMem(tmp,5);
 Ghost->ExtractValue(tmp,str,"ed1",0);
 LoadMem(tmp,6);
 Ghost->ExtractValue(tmp,str,"ed2",0);
 LoadMem(tmp,7);
 Ghost->ExtractValue(tmp,str,"ed3",0);
 LoadMem(tmp,8);
}

void __fastcall TPrepPlanning::LoadCalendrier()
{
 char s[2000],tmp[250];  int val;

 Ghost->ExtractValue(tmp,str,"cah",0);
 if (strcmp(tmp,"O")==0) P_CALHAUT[pc]='O'; else P_CALHAUT[pc]='N';
 Ghost->ExtractValue(tmp,str,"cab",0);
 if (strcmp(tmp,"O")==0) P_CALBAS[pc]='O'; else P_CALBAS[pc]='N';

 Ghost->ExtractValue(s,str,"cl1",0);
 Ghost->ExtractValue(tmp,s,"unt",0); P_UNIT1[pc]=AnsiString(tmp);
 Ghost->ExtractValue(tmp,s,"fmt",0); P_FORMATCAL1[pc]=AnsiString(tmp);
 Ghost->ExtractValue(tmp,s,"frq",0); P_FREQ1[pc]=AnsiString(tmp);
 LoadMem(s,9);


 Ghost->ExtractValue(s,str,"cl2",0);
 Ghost->ExtractValue(tmp,s,"unt",0); P_UNIT2[pc]=AnsiString(tmp);
 Ghost->ExtractValue(tmp,s,"fmt",0); P_FORMATCAL1[pc]=AnsiString(tmp);
 Ghost->ExtractValue(tmp,s,"frq",0); P_FREQ1[pc]=AnsiString(tmp);
 LoadMem(s,10);
}

void __fastcall TPrepPlanning::LoadTaches()
{
 char tmp[3000];
 int i;
 Ghost->ExtractValue(tmp,str,"tc0",0);
 Ftaches(tmp,0);
 Ghost->ExtractValue(tmp,str,"tc1",0);
 Ftaches(tmp,1);
 Ghost->ExtractValue(tmp,str,"tc2",0);
 Ftaches(tmp,2);
 Ghost->ExtractValue(tmp,str,"tc3",0);
 Ftaches(tmp,3);
}

void __fastcall TPrepPlanning::Ftaches(char *buf,int indice)
{
char tmp[5000]; int i; char tag[50];
char val[5000];


Ghost->ExtractValue(val,buf,"tt",0); P_TRAIT[pc][indice]=AnsiString(val);
Ghost->ExtractValue(val,buf,"tet",0);  P_EPTRAIT[pc][indice]=AnsiString(val);
Ghost->ExtractValue(val,buf,"tm",0);  P_MARGE[pc][indice]=AnsiString(val);
Ghost->ExtractValue(val,buf,"ts",0);  P_STYLEM[pc][indice]=AnsiString(val);
Ghost->ExtractValue(val,buf,"tcm",0);  P_COLMARGE[pc][indice]=AnsiString(val);
Ghost->ExtractValue(val,buf,"tl",0);  P_LIENS[pc][indice] = AnsiString(val);
Ghost->ExtractValue(val,buf,"tsl",0);  P_STYLELIEN[pc][indice]= AnsiString(val);
Ghost->ExtractValue(val,buf,"tcl",0);  P_COL_LIEN[pc][indice]=AnsiString(val);
Ghost->ExtractValue(val,buf,"tel",0);  P_EPAIS_LIEN[pc][indice]=AnsiString(val);


 for (i=0;i<4;i++)
   {
    sprintf(tag,"ti%d",i);
    Ghost->ExtractValue(tmp,buf,tag,0);
    Ghost->ExtractValue(val,tmp,"inf",0);P_INF[pc][indice][i]=AnsiString(val);
    Ghost->ExtractValue(val,tmp,"info",0);P_INFO[pc][indice][i]=AnsiString(val);
    Ghost->ExtractValue(val,tmp,"pol",0);P_POLT[pc][indice][i]=AnsiString(val);
    Ghost->ExtractValue(val,tmp,"ctxt",0);P_COLTXTF[pc][indice][i]=AnsiString(val);
    Ghost->ExtractValue(val,tmp,"ital",0);P_ITALT[pc][indice][i]=AnsiString(val);
    Ghost->ExtractValue(val,tmp,"bold",0);P_BOLDT[pc][indice][i]=AnsiString(val);
    Ghost->ExtractValue(val,tmp,"siz",0);P_SIZET[pc][indice][i]=AnsiString(val);
    Ghost->ExtractValue(val,tmp,"cbkg",0);P_COLTXTB[pc][indice][i]=AnsiString(val);
   }

}

void __fastcall TPrepPlanning::LoadCorps()
{
 char tmp[2000]; int i;
 char elm[2000];
 char tag[20];

 for (i=0;i<8;i++)
   {
    sprintf(tag,"c%d",i);
    Ghost->ExtractValue(tmp,str,tag,0);

    Ghost->ExtractValue(elm,tmp,"cyn",0);P_CYN[pc][i]=AnsiString(elm);
    Ghost->ExtractValue(elm,tmp,"ccfg",0);P_CCFG[pc][i]=AnsiString(elm);
    //Ghost->ExtractValue(elm,tmp,"ccbk",0);P_CCBK[pc][i]=AnsiString(elm);
    Ghost->ExtractValue(elm,tmp,"ccst",0);P_CCST[pc][i]=AnsiString(elm);
    Ghost->ExtractValue(elm,tmp,"ccep",0);P_CCEP[pc][i]=AnsiString(elm);
   }


  Ghost->ExtractValue(elm,str,"cjt",0);P_CJT[pc]=AnsiString(elm);
  Ghost->ExtractValue(elm,str,"cjtfl",0);P_CJTFLAG[pc]=AnsiString(elm);
  Ghost->ExtractValue(elm,str,"cjnt",0);P_CJNT[pc]=AnsiString(elm);
  Ghost->ExtractValue(elm,str,"cwent",0);P_CWENT[pc]=AnsiString(elm);
  Ghost->ExtractValue(elm,str,"cdh",0);P_CDH[pc]=AnsiString(elm);
  Ghost->ExtractValue(elm,str,"cdv",0);P_CDV[pc]=AnsiString(elm);  P_CDVV[pc]=atoi(elm);

  Ghost->ExtractValue(elm,str,"ccjt",0);P_CCJT[pc]=AnsiString(elm);
  Ghost->ExtractValue(elm,str,"ccjnt",0);P_CCJNT[pc]=AnsiString(elm);
  Ghost->ExtractValue(elm,str,"ccwent",0);P_CCWENT[pc]=AnsiString(elm);

}

void __fastcall TPrepPlanning::LoadPied()
{
char tmp[2000];

 Ghost->ExtractValue(tmp,str,"pg1",0);
 LoadMem(tmp,25);
 Ghost->ExtractValue(tmp,str,"pg2",0);
 LoadMem(tmp,26);
 Ghost->ExtractValue(tmp,str,"pg3",0);
 LoadMem(tmp,27);
 Ghost->ExtractValue(tmp,str,"pc1",0);
 LoadMem(tmp,28);
 Ghost->ExtractValue(tmp,str,"pc2",0);
 LoadMem(tmp,29);
 Ghost->ExtractValue(tmp,str,"pc3",0);
 LoadMem(tmp,30);
 Ghost->ExtractValue(tmp,str,"pd1",0);
 LoadMem(tmp,31);
 Ghost->ExtractValue(tmp,str,"pd2",0);
 LoadMem(tmp,32);
 Ghost->ExtractValue(tmp,str,"pd3",0);
 LoadMem(tmp,33);

}

void __fastcall TPrepPlanning::LoadTLigne()
{
  /*
   char tmp[250];
       Ghost->ExtractValue(tmp,str,"pol",0); PL_POL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ct",0);  PL_CTEXT[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"it",0);  PL_ITAL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"bd",0);  PL_BOLD[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ht",0);  PL_HAUTEUR[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"jh",0);  PL_JSH[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"jv",0);  PL_JSV[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"cb",0);  PL_CBKG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"lg",0);  PL_LARG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ep1",0); PL_LS1EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"cl1",0); PL_LS1CL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ls2",0); PL_LS2[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ep2",0); PL_LS2EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"cl2",0); PL_LS2CL[pc]=AnsiString(tmp);
*/
}

void __fastcall TPrepPlanning::LoadTCol()
{
/*
  char tmp[250];
       Ghost->ExtractValue(tmp,str,"pol",0); PC_POL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ct",0);  PC_CTEXT[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"it",0);  PC_ITAL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"bd",0);  PC_BOLD[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ht",0);  PC_HAUTEUR[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"jh",0);  PC_JSH[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"jv",0);  PC_JSV[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"cb",0);  PC_CBKG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"lg",0);  PC_LARG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ep1",0); PC_LS1EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"cl1",0); PC_LS1CL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ls2",0); PC_LS2[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"ep2",0); PC_LS2EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,str,"cl2",0); PC_LS2CL[pc]=AnsiString(tmp);
 */
}




void __fastcall TPrepPlanning::rb2Click(TObject *Sender)
{
 if (rb2->Checked==true)
    P_TYPE[pc]='R';   // previsionnel ou r��l
   else P_TYPE[pc]='P';
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::rb1Click(TObject *Sender)
{
 if (rb1->Checked==true)
 P_TYPE[pc]='P';   // previsionnel ou r��l
 else
 P_TYPE[pc]='R';  
}
//---------------------------------------------------------------------------


void __fastcall TPrepPlanning::btLignesClick(TObject *Sender)
{
/*
 TFDTligne *TLigne;
 //Envoi_Params(0);
 TLigne = new TFDTligne(Application);
 TLigne->ShowModal();
 delete TLigne;
 //if (strlen(m_exchange)==0) return;
 // recuperer les donnees
 //Recupere_Params(0);
*/

 TTri *Tri;

 strcpy(m_exchange,"<tri>"); strcat(m_exchange,P_TRI[pc]); strcat(m_exchange,"</tri>");
 strcat(m_exchange,"<util>utiliser</util>");
 Tri = new TTri(Application);
 //Tri->Height=619;
 //Tri->Width=754;

 Tri->ShowModal();  //Modal();
 delete Tri;
}




void __fastcall TPrepPlanning::Envoi_Params(int ligcol)
{

strcpy(m_exchange,"");
/*
if (ligcol==0)  // ligne
{
strcat(m_exchange,"<pol>");strcat(m_exchange,PL_POL[pc].c_str()); strcat(m_exchange,"</pol>");
strcat(m_exchange,"<ct>");strcat(m_exchange,PL_CTEXT[pc].c_str()); strcat(m_exchange,"</ct>");
strcat(m_exchange,"<it>");strcat(m_exchange,PL_ITAL[pc].c_str()); strcat(m_exchange,"</it>");
strcat(m_exchange,"<bd>");strcat(m_exchange,PL_BOLD[pc].c_str()); strcat(m_exchange,"</bd>");
strcat(m_exchange,"<ht>");strcat(m_exchange,PL_HAUTEUR[pc].c_str()); strcat(m_exchange,"</ht>");
strcat(m_exchange,"<jh>");strcat(m_exchange,PL_JSH[pc].c_str()); strcat(m_exchange,"</jh>");
strcat(m_exchange,"<jv>");strcat(m_exchange,PL_JSV[pc].c_str()); strcat(m_exchange,"</jv>");
strcat(m_exchange,"<cb>");strcat(m_exchange,PL_CBKG[pc].c_str()); strcat(m_exchange,"</cb>");
strcat(m_exchange,"<lg>");strcat(m_exchange,PL_LARG[pc].c_str()); strcat(m_exchange,"</lg>");
strcat(m_exchange,"<ep1>");strcat(m_exchange,PL_LS1EP[pc].c_str()); strcat(m_exchange,"</ep1>");
strcat(m_exchange,"<cl1>");strcat(m_exchange,PL_LS1CL[pc].c_str()); strcat(m_exchange,"</cl1>");
strcat(m_exchange,"<ls2>");strcat(m_exchange,PL_LS2[pc].c_str()); strcat(m_exchange,"</ls2>");
strcat(m_exchange,"<ep2>");strcat(m_exchange,PL_LS2EP[pc].c_str()); strcat(m_exchange,"</ep2>");
strcat(m_exchange,"<cl2>");strcat(m_exchange,PL_LS2CL[pc].c_str()); strcat(m_exchange,"</cl2>");
}
else  // colonne
{
strcat(m_exchange,"<pol>");strcat(m_exchange,PC_POL[pc].c_str()); strcat(m_exchange,"</pol>");
strcat(m_exchange,"<ct>");strcat(m_exchange,PC_CTEXT[pc].c_str()); strcat(m_exchange,"</ct>");
strcat(m_exchange,"<it>");strcat(m_exchange,PC_ITAL[pc].c_str()); strcat(m_exchange,"</it>");
strcat(m_exchange,"<bd>");strcat(m_exchange,PC_BOLD[pc].c_str()); strcat(m_exchange,"</bd>");
strcat(m_exchange,"<ht>");strcat(m_exchange,PC_HAUTEUR[pc].c_str()); strcat(m_exchange,"</ht>");
strcat(m_exchange,"<jh>");strcat(m_exchange,PC_JSH[pc].c_str()); strcat(m_exchange,"</jh>");
strcat(m_exchange,"<jv>");strcat(m_exchange,PC_JSV[pc].c_str()); strcat(m_exchange,"</jv>");
strcat(m_exchange,"<cb>");strcat(m_exchange,PC_CBKG[pc].c_str()); strcat(m_exchange,"</cb>");
strcat(m_exchange,"<lg>");strcat(m_exchange,PC_LARG[pc].c_str()); strcat(m_exchange,"</lg>");
strcat(m_exchange,"<ep1>");strcat(m_exchange,PC_LS1EP[pc].c_str()); strcat(m_exchange,"</ep1>");
strcat(m_exchange,"<cl1>");strcat(m_exchange,PC_LS1CL[pc].c_str()); strcat(m_exchange,"</cl1>");
strcat(m_exchange,"<ls2>");strcat(m_exchange,PC_LS2[pc].c_str()); strcat(m_exchange,"</ls2>");
strcat(m_exchange,"<ep2>");strcat(m_exchange,PC_LS2EP[pc].c_str()); strcat(m_exchange,"</ep2>");
strcat(m_exchange,"<cl2>");strcat(m_exchange,PC_LS2CL[pc].c_str()); strcat(m_exchange,"</cl2>");

}
*/
}

void __fastcall TPrepPlanning::Recupere_Params(int ligcol)
{
 char tmp[250];
 /*
 if (ligcol==0)  // ligne
 {
       Ghost->ExtractValue(tmp,m_exchange,"pol",0); PL_POL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ct",0);  PL_CTEXT[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"it",0);  PL_ITAL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"bd",0);  PL_BOLD[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ht",0);  PL_HAUTEUR[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"jh",0);  PL_JSH[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"jv",0);  PL_JSV[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"cb",0);  PL_CBKG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"lg",0);  PL_LARG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ep1",0); PL_LS1EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"cl1",0); PL_LS1CL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ls2",0); PL_LS2[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ep2",0); PL_LS2EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"cl2",0); PL_LS2CL[pc]=AnsiString(tmp);
   }
 else // colonnes
   {
       Ghost->ExtractValue(tmp,m_exchange,"pol",0); PC_POL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ct",0);  PC_CTEXT[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"it",0);  PC_ITAL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"bd",0);  PC_BOLD[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ht",0);  PC_HAUTEUR[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"jh",0);  PC_JSH[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"jv",0);  PC_JSV[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"cb",0);  PC_CBKG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"lg",0);  PC_LARG[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ep1",0); PC_LS1EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"cl1",0); PC_LS1CL[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ls2",0); PC_LS2[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"ep2",0); PC_LS2EP[pc]=AnsiString(tmp);
       Ghost->ExtractValue(tmp,m_exchange,"cl2",0); PC_LS2CL[pc]=AnsiString(tmp);

   }
  */
}

void __fastcall TPrepPlanning::btAppliquerClick(TObject *Sender)
{
AnsiString planningsel, planningfinal;
if(etatsel == 1)
{
planningsel = eMemoire->Text;

for(int i = 0; i< lbPlanning->Items->Count;i++)
{
 if(lbPlanning->Items->Strings[i] == "Planning modifi�")
 {
  lbPlanning->ItemIndex = i;
  }else{
planningfinal = "Planning modifi�";
eMemoire->Text = planningfinal;

  }
} }
 char tmp[250];

 if (Label7->Caption == "" && P_GRILLE[pc] == 'O')
   {
    Application->MessageBoxA("S�lectionner une Grille",m_ecoplan,MB_OK);
    return;
   }

 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt><exec>O</exec>",pc,
    P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 strcpy(m_planning,tmp);
 RELOADFILTRE[pc]=true;
 strcpy(P_NOM[pc],eMemoire->Text.c_str());  // current_planning);
 Save_Preseance();

 SaveData(0);
 Label23->Caption = eMemoire->Text;
  m_automatic[pc] = ' ';
  etat = 0;
 Close();
}
//---------------------------------------------------------------------------

int __fastcall TPrepPlanning::DeleteK(char *k)
{
 char RetKey[100]; int recdata, rc;
 rc = plannings->ReadDirectKey(0,k,RetKey,&recdata);
 if (rc)
  {
   rc=plannings->DeleteKey(0,k,recdata);
   rc=plannings->DeleteRecord(recdata);
  }
 return rc;
}


void __fastcall TPrepPlanning::btSupprClick(TObject *Sender)
{
 int rc,recdata; char Key[200],RetKey[200];
 char tmp[350]; char TempKey[200];
 int indx;

 if (lbPlanning->Items->Count==0) return;

 if (eMemoire->Text == "")
   {
    Application->MessageBoxA("Pas de planning s�lectionn�",m_ecoplan,MB_OK);
    return;
  }
 if (Application->MessageBoxA("Confirmer l'effacement du planning",m_ecoplan,MB_YESNO)==IDNO)
    return;
 strcpy(TempKey,eMemoire->Text.c_str());

 strcpy(tmp,m_directory);  //DatabaseName);
 strcat(tmp,"\\eco_plannings");

 rc = plannings->OpenEngine(tmp,1016,"ndx","dat");   //1024 - 8
 rc = plannings->SetIndexMode(0,0);

 strcpy(Key,TempKey); strcat(Key,"-GEN");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-MRG");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-ETT");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-CAL");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-CDR");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-TCH");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-COR");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-PIE");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-TLIG");
 rc=DeleteK(Key);
 strcpy(Key,TempKey); strcat(Key,"-TCOL");
 rc=DeleteK(Key);

 plannings->CloseEngine();

 indx = lbPlanning->ItemIndex;
 indx=0;
 lbPlanning->Items->Delete(indx);

 Label3->Caption = "";
 Label5->Caption = "";
 Label7->Caption = "";
 Label23->Caption = "";
 mPlanning->Clear();

 eMemoire->Clear();
 Save_Preseance();
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Timer1Timer(TObject *Sender)
{
 Timer1->Enabled=false;
 char tmp[2000];
 char x_ghost_prep[200];


 if (strlen(m_assign))
  {
   Ghost->ExtractValue(tmp,m_assign,"col",0);
   if (strlen(tmp))     // resultat dans P_COLONNES
     {
      P_GENRE[pc] = 'G';
      rb11->Checked = false;
      rb12->Checked = true;
      cb2->Checked = true;
      P_GRILLE[pc] = 'O';
      if (pc==0) { cb2->Checked = false; P_GRILLE[pc] = 'N'; }


     }
   Ghost->ExtractValue(tmp,m_assign,"tri",0);
   if (strlen(tmp)) Label3->Caption = AnsiString(P_TRI[pc]);
   Ghost->ExtractValue(tmp,m_assign,"fil",0);
   if (strlen(tmp)) Label5->Caption = AnsiString(P_FILTRE[pc]);
   RELOADFILTRE[pc] = true;
   RELOADTRI[pc] = true;
   RELOADCOL[pc]=true;
  }
  m_assign[0]=0;
 if (m_ghost_prep[pc] != "")
  {
   strcpy(x_ghost_prep,m_ghost_prep[pc].c_str());
   //Ghost->ExtractValue(current_planning,x_ghost_prep,"pl",0);
   Ghost->ExtractValue(P_NOM[pc],x_ghost_prep,"pl",0);
   Ghost->ExtractValue(tmp,x_ghost_prep,"com",0);
   mPlanning->Clear(); mPlanning->SetTextBuf(tmp);
   Maj_Decor();
  }
 m_ghost_prep[pc]="";
 Timer1->Enabled=true;
}

void __fastcall TPrepPlanning::Maj_Decor()
{
 Label5->Caption = AnsiString(P_FILTRE[pc]);
 Label3->Caption = AnsiString(P_TRI[pc]);
 Label7->Caption = AnsiString(P_COLONNES[pc]);

 Label23->Caption = AnsiString(P_NOM[pc]); //current_planning);
 eMemoire->Text = AnsiString(P_NOM[pc]);   //current_planning);

 mPlanning->SetTextBuf(CommentPL[pc].c_str());
 //if ( P_MODE[pc]=='P')  { rbPortrait->Checked=true; rbPaysage->Checked=false;}
 //    else { rbPaysage->Checked=true; rbPortrait->Checked=false; }
 if (P_GENRE[pc]=='C') { rb11->Checked=true;rb12->Checked=false; cb2->Checked=false;}
 if (P_GENRE[pc]=='G') { rb12->Checked=true; rb11->Checked=false; }
 if (pc==0) P_GRILLE[pc] = 'N';
 if (P_GRILLE[pc]=='O') cb2->Checked=true; else cb2->Checked=false;

 if (P_TYPE[pc]=='P') { rb1->Checked=true;rb2->Checked=false; }
   else  { rb2->Checked=true; rb1->Checked=false; }

 if (P_LBRISE[pc]=='O') cb1->Checked=true; else cb1->Checked=false;
 //if (P_MARGES[pc]=='O') cb10->Checked=true; else cb10->Checked=false;

 //if (P_CADRE[pc]=='O') cb3->Checked=true; else cb3->Checked=false;
 if (P_ENTETE[pc]=='O') cb4->Checked=true; else cb4->Checked=false;
 if (P_CALHAUT[pc]=='O') cb5->Checked=true; else cb5->Checked=false;
 if (P_CALBAS[pc]=='O') cb6->Checked=true; else cb6->Checked=false;
 if (P_PIED[pc]=='O') cb7->Checked=true; else cb7->Checked=false;
}


//---------------------------------------------------------------------------




//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::cb4Click(TObject *Sender)
{
 if (cb4->Checked==true)
 P_ENTETE[pc]='O';
 else P_ENTETE[pc]='N';
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::cb5Click(TObject *Sender)
{
 if (cb5->Checked==true)
 P_CALHAUT[pc]='O';
 else P_CALHAUT[pc]='N';
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::cb6Click(TObject *Sender)
{
 if (cb6->Checked==true)
 P_CALBAS[pc]='O';
 else P_CALBAS[pc]='N';
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::cb7Click(TObject *Sender)
{
 if (cb7->Checked==true)
 P_PIED[pc]='O';
 else P_PIED[pc]='N';
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::rb11Click(TObject *Sender)
{
  if (rb11->Checked==true)
   { P_GENRE[pc]='C';
     cb2->Checked=false;
     Label7->Caption = "";
     strcpy(P_COLONNES[pc],"");
     Button3->Enabled = false;
   }
 else { P_GENRE[pc]='G';
        Button3->Enabled = true;
      }
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::rb12Click(TObject *Sender)
{
  if (rb12->Checked==true)
     { P_GENRE[pc]='G'; Button3->Enabled = true; }
 else { P_GENRE[pc]='C'; Button3->Enabled = false; rb11->Checked=true; }
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::cb2Click(TObject *Sender)
{
  if (cb2->Checked==true)
    { P_GRILLE[pc]='O'; rb11->Checked=false; rb12->Checked=true; }
     else P_GRILLE[pc]='N';

}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::cb1Click(TObject *Sender)
{
 if (rb2->Checked==false) cb1->Checked = false;
 if (cb1->Checked==true)
  P_LBRISE[pc]='O'; else P_LBRISE[pc]='N';

}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::FormShow(TObject *Sender)
{
 //
 FILE *fp;  char *p;
 char buff[250],tmp[250];
 int rc,recdata;
 char Key[200],RetKey[200];
 int ctr,lg;

  plannings = new realisam;
  change = false;
  strcpy(filename,m_directory);  //DatabaseName);
  strcat(filename,"\\eco_plannings.txt");

  lbPlanning->Clear();

  fp=fopen(filename,"rt");
 if (fp == NULL)
   {

   }
 else   //read file pour ordre de pr�seance
   {
    buff[0]=0; ctr=0;
    fgets(buff,512,fp);
    while (!feof(fp))   // lire les noms dans l'ordre
      {
       ctr++;
       lg = Ghost->ExtractValue(tmp,buff,"l",0);
       if (lg==0) break;
       lbPlanning->Items->Add(AnsiString(tmp));
       fgets(buff,512,fp);
      }
    fclose(fp);
   }


 if (strlen(P_NOM[pc])==0)
   {
     Init_Planning();
     if (strlen(LastPlanning[pc]))
       {
        if (change) { SaveData(1); change=0; }

        eMemoire->Text=AnsiString(LastPlanning[pc]);
        Label23->Caption = eMemoire->Text;
        strcpy(P_NOM[pc],LastPlanning[pc]);  // current_planning,LastPlanning[pc]);
        Ghost->Global_Planning(LastPlanning[pc]);

        // Adapter les radio Button
        //if (P_MODE[pc]=='P') { rbPortrait->Checked=true; rbPaysage->Checked=false; }
        //else  { rbPortrait->Checked=false; rbPaysage->Checked=true; }
       }
   }
 else
    {
     Maj_Decor();
     //Ghost->LoadPlanningFromFile(P_NOM[pc]);
    }

 if (strcmp(m_memoriser,"memo")==0)
    {
     SaveData(1);
     strcpy(m_memoriser,"");
     Close();
    }

 if (pc != 3)
  {
   GroupBox1->Visible = false;
   rb1->Visible=false;
   rb2->Visible=false;
  }
 cb1->Visible = false;
 Timer1->Enabled = true;

}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::FormCreate(TObject *Sender)
{
etat = 1;
etatsel = 0;
countertri = 0;

AdvStringGrid2->ColWidths[1]=79;
ListBox1->Items->LoadFromFile("eco_plannings.txt");
 Timer1->Enabled = false;
 }
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::cbEbaucheClick(TObject *Sender)
{

 char tmp[250];
 if (cbEbauche->Checked == true)
   {
    if (Label7->Caption == "" && P_GRILLE[pc] == 'O')
     {
       Application->MessageBoxA("S�lectionner une Grille",m_ecoplan,MB_OK);
       return;
      }
    if (Application->MessageBoxA("Attention : Si Oui, Vous Revenez a l'�bauche initiale",m_ecoplan,MB_YESNO) == IDNO)
      {
       cbEbauche->Checked = false;
       m_automatic[pc]=' ';
      }
    else
     {

        m_automatic[pc] = 'X';
        sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt><exec>O</exec>",pc,
        P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
        strcpy(m_planning,tmp);
        RELOADFILTRE[pc]=true;
        strcpy(P_NOM[pc],eMemoire->Text.c_str());  // current_planning);
        Save_Preseance();
        cbEbauche->Checked = false;
        Close();
      }
   }
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button8Click(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"Plannings.html#Pl3");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;
}
//---------------------------------------------------------------------------



void __fastcall TPrepPlanning::mPlanningKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  CommentPL[pc] = AnsiString(mPlanning->Lines->GetText());
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button4Click(TObject *Sender)
{
lbPlanning->Sorted = false;
AnsiString element;
AnsiString elementunique;
TStringList *ListeChiffres = new TStringList;
TStringList *ListeLettres = new TStringList;
TStringList *ListeComplete = new TStringList;


for(int i = 0; i< lbPlanning->Items->Count; i++)
{
element= lbPlanning->Items->Strings[i];
if(element == "")
{
return;}else{
 elementunique=element[1];
 if(elementunique == '0' || elementunique == '1' || elementunique == '2' || elementunique == '3' || elementunique == '4' ||elementunique == '5' ||elementunique == '6' ||elementunique == '7' ||elementunique == '8' ||elementunique == '9')
 {
   ListeChiffres->Add(element);
 }else{
   ListeLettres->Add(element);

 }

}

}

lbPlanning->Clear();
ListeLettres->Sorted= true;
 ListeChiffres->Sorted = true;

 for(int j = 0; j < ListeLettres->Count; j++)
 {
 ListeComplete->Add(ListeLettres->Strings[j]);

 }

 for(int k = 0; k < ListeChiffres->Count; k++)
 {
 ListeComplete->Add(ListeChiffres->Strings[k]);

 }



 lbPlanning->Items->Assign(ListeComplete);


ListeChiffres->Clear();
ListeLettres->Clear();
ListeComplete->Clear();
 delete ListeLettres;
delete ListeChiffres;
delete ListeComplete;
Save_Preseance();
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button2ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 strcpy(P_TRI[pc],"");
 Label3->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button1ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 strcpy(P_FILTRE[pc],"");
 Label5->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button3ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 strcpy(P_COLONNES[pc],"");
 Label7->Caption = "";
 cb2->Checked=false;
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Label3ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
Label3->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Label5ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 Label5->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Label7ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 Label7->Caption = "";
}
//---------------------------------------------------------------------------





void __fastcall TPrepPlanning::Button5Click(TObject *Sender)
{
     for(int i=1; i< AdvStringGrid1->RowCount; i++)
     {
        AdvStringGrid1->Cells[1][i] = "";

     }

     AdvStringGrid1->SortIndexes->Clear();
     AdvStringGrid1->LoadFromFile("Filtremodifie.ecfg");

     AdvStringGrid1->HideColumn(0);
     AdvStringGrid1->ColCount = 5;
AdvStringGrid1->ColWidths[0]=80;
AdvStringGrid1->ColWidths[1]=95;
AdvStringGrid1->ColWidths[2]=80;
AdvStringGrid1->ColWidths[3]=80;
AdvStringGrid1->ColWidths[4]=60;
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button10Click(TObject *Sender)
{
     for(int i=1; i< AdvStringGrid2->RowCount; i++)
     {
        AdvStringGrid2->Cells[1][i] = "";
     }
     AdvStringGrid2->SortIndexes->Clear();
AdvStringGrid2->LoadFromFile("Trimodifie.ecfg");


}
//---------------------------------------------------------------------------


void __fastcall TPrepPlanning::Button11Click(TObject *Sender)
{
AnsiString direct = ExtractFileDir(ParamStr(0));

AnsiString nomplanning = eMemoire->Text;
AnsiString nomFile = direct+"\\Ref\\";


 AdvStringGrid1->SaveToFile(nomFile+nomplanning+"1");
 AdvStringGrid2->SaveToFile(nomFile+nomplanning+"2");
}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button13Click(TObject *Sender)
{
 TxFilter *filt;
 char tmp[250];
 char strfilter[100];


 strcpy(m_exchange,"<util>utiliser</util><filt>");
 strcat(m_exchange,P_FILTRE[pc]); strcat(m_exchange,"</filt>");


 strcpy(m_project,DatabaseName);
 filt = new TxFilter(Application);
 filt->Height = 705;
 filt->Width = 774;

//  filt->Visible=true;
 m_filtre[0]=0;

//filt->ShowModal(); //Modal();
//delete filt;

filt->Show();

 /*
 Label3->Caption = AnsiString(P_FILTRE[pc]);
 delete filt;
 Affect_Filtre(P_FILTRE[pc]);
 */

 if(etatsel != 1)
 {
   etatsel = 1;
 }
 Button5->Click();
 if(essai == 1)
 {
  filt->Close();
 }

}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::Button14Click(TObject *Sender)
{
 TTri *Tri;


 strcpy(m_exchange,"<tri>"); strcat(m_exchange,P_TRI[pc]); strcat(m_exchange,"</tri>");
 strcat(m_exchange,"<util>utiliser</util>");
 Tri = new TTri(Application);
 //Tri->Height=619;
 //Tri->Width=754;

Tri->Show();

 Label3->Caption = AnsiString(P_TRI[pc]);
 Affect_Tri(P_TRI[pc]);
 if(etatsel != 1)
 {
   etatsel = 1;

 }

   Button10->Click();
if(etatsel==1){
AnsiString direct = ExtractFileDir(ParamStr(0));

AnsiString nomplanning = eMemoire->Text;
AnsiString nomFile = direct+"\\Ref\\";


 AdvStringGrid2->SaveToFile(nomFile+nomplanning+"2");
               }


 if(essai == 1)
 {
  Tri->Close();
 }

}
//---------------------------------------------------------------------------

void __fastcall TPrepPlanning::AdvStringGrid2KeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
if (Key == VK_INSERT)
  {    //  insertion d une tache a la fin

    etatsel=0;
   }
}
//---------------------------------------------------------------------------

