//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

//#include "modHelp.h"
#include "modFilter.h"
#include <stdio.h>

#include <iostream>
#include <fstream>     
#include "globals.h"
#include "modChamps.h"
#include "modHelp.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGrid"
#pragma link "BaseGrid"
#pragma resource "*.dfm"
TxFilter *xFilter;
AnsiString nomFiltre;
AnsiString nomFile;
AnsiString direct;
int etatf;
int etatfiltre;
//---------------------------------------------------------------------------
__fastcall TxFilter::TxFilter(TComponent* Owner)
        : TForm(Owner)
{


}



//---------------------------------------------------------------------------
void __fastcall TxFilter::btExecuterClick(TObject *Sender)
{
 char tmp[250],tmpx[2500];
 int count,i;

 count = asFC->RowCount; ///lbDetail->Items->Count;
 if (asFC->Cells[0][1]=="")
   {
    Application->MessageBoxA("Filtre Vide",ecoplan,MB_OK);
    return;
   }
 strcpy(tmpx,"<E>");  // <E> pour exec
 strcat(tmpx,"<criteres>");

 for (i=1;i<count;i++)
   {
    tmp[0]=0;
    if (i==1)
     {strcat(tmp,"<I>");
     if (rbtn1->Checked==true) strcat(tmp,"T</I>"); else strcat(tmp,"N</I>");

     }

    strcat(tmp,"<A>"); strcat(tmp,asFC->Cells[1][i].c_str()); strcat(tmp,"</A>");
    strcat(tmp,"<C>"); strcat(tmp,asFC->Cells[2][i].c_str()); strcat(tmp,"</C>");
    strcat(tmp,"<R>"); strcat(tmp,asFC->Cells[3][i].c_str()); strcat(tmp,"</R>");
    strcat(tmp,"<V1>"); strcat(tmp,asFC->Cells[4][i].c_str()); strcat(tmp,"</V1>");
    strcat(tmp,"<V2>"); strcat(tmp,asFC->Cells[5][i].c_str()); strcat(tmp,"</V2>");

    //strcat(tmp,lbDetail->Items->Strings[i].c_str());
    strcat(tmpx,tmp);
    strcat(tmpx,"|");   // | as separator
   }
 strcat(tmpx,"</criteres>");

 //// strcpy(m_filtre,tmpx);

 SaveData(0);

 for (i=0;i<6;i++)
   {
    strcpy(P_FILTRE[i],eMemoire->Text.c_str());
    RELOADFILTRE[i]=true;
   }

    sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt><exec>O</exec>",pc,
    P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
    strcpy(m_planning,tmp);
    RELOADFILTRE[pc]=true;


   asFC->SaveToFile("Filtremodifie.ecfg");

 Close();

}
//---------------------------------------------------------------------------

void __fastcall TxFilter::FormCreate(TObject *Sender)
{
 FILE *fp;  char *p;
 char buff[250],tmp[250];
 int rc,recdata;
 char Key[100],RetKey[100];
 int ctr,lg;

 strcpy(ecoplan,m_ecoplan);
  filtres = new realisam;
  change = false;
 asFC->RowCount=2;
 OldRow=0; OldCol=0;
 step=0;
 strcpy(filename,m_directory);
 strcat(filename,"\\eco_filtres.txt");
 lbFiltre->Clear();

 Current_Row=0;
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
       lg = ExtractValue(tmp,buff,"l",0);
       if (lg==0) break;
       lbFiltre->Items->Add(AnsiString(tmp));
       fgets(buff,512,fp);
      }
   }
 ExtractValue(mode,m_exchange,"util",0);

 Modify();
 ExtractValue(tmp,m_exchange,"filt",0);
  if (strlen(tmp))Display_Filtre(tmp);
  AdvStringGrid1->Clear();
  AdvStringGrid1->Visible = false;
asFC->Visible = true;
etatf=0;
direct = ExtractFileDir(ParamStr(0));
etatfiltre = 0;
}

void __fastcall TxFilter::Modify()
{
  if (strcmp(mode,"utiliser")==0)
   {
    Label1->Caption = "Filtres : Utiliser";
    Label1->Caption = "Filtres : Utiliser";
    xFilter->Caption = "Utiliser";
    btEffacer->Visible = false;
    btNew->Visible = false;
    btLigne->Visible=false;
    btDelete->Visible=false;
    rg1->Visible=true;
    rbtn1->Visible=true;
    rbtn2->Visible=true;
    btMod->Visible = false;
    btStep->Visible=true;
    btAffR->Visible=false;
    btAffP->Visible=true;
    Memo1->Visible=true;
    btExecuter->Visible=true;
    Memo1->SetTextBuf("Utiliser le filtre tel qu'affich�");
    Memo2->Visible = true;
    //lCond->Caption = "0";
    rg1->Visible = false;
    rbtn1->Visible = false;
    rbtn2->Visible = false;
    btStep->Visible=false;
   }
 else
   {
    Label1->Caption = "Filtres : Cr�er / Compl�ter / Supprimer";
    xFilter->Caption = "Cr�er Completer Filtres";
    //btRapports->Visible = false;
    //btPlannings->Visible= false;
    btLigne->Visible=true;
    btNew->Visible=true;
    btExecuter->Visible=false;
    btStep->Visible = false;
    btMod->Visible = true;
    Memo1->Visible=false;
    btAffR->Visible=false;
    btAffP->Visible=false;
    Memo1->SetTextBuf("");
    Memo2->Visible = false;
    //lCond->Caption = "";
    btStep->Visible = false;
   }


}

//---------------------------------------------------------------------------

void __fastcall TxFilter::Clean(int row)
{
 int i;
 for (i=0;i<asFC->ColCount;i++)  asFC->Cells[i][row]="";
}

void __fastcall TxFilter::btDeleteClick(TObject *Sender)
{
 int row,col,i,j;
 if (OldRow == 0)
   {
    Application->MessageBoxA("Aucune condition s�lectionn�e",m_ecoplan,MB_OK);
    return;
   }
 if (Application->MessageBoxA("Confirmer l'effacement",m_ecoplan,MB_YESNO)==IDNO) return;

 // tout d�caler ! merde
 if (asFC->RowCount==2)  { Clean(1); OldRow=0; return; }
 if (OldRow == (asFC->RowCount-1)){Clean(OldRow); asFC->RowCount--; OldRow--; return; }

 row = asFC->RowCount;
 for (i = OldRow; i<row;i++)
   {
    asFC->Cells[1][i] = asFC->Cells[1][i+1];
    asFC->Cells[2][i] = asFC->Cells[2][i+1];
    asFC->Cells[3][i] = asFC->Cells[3][i+1];
    asFC->Cells[4][i] = asFC->Cells[4][i+1];
    asFC->Cells[5][i] = asFC->Cells[5][i+1];
   }
//  renumeroter la colonne 0

asFC->RowCount--;  row= asFC->RowCount;

for (i=1;i<row;i++)  asFC->Cells[0][i]=IntToStr(i);

 // lbDetail->Items->Delete(indx_detail);
}
//---------------------------------------------------------------------------

/*
void __fastcall TxFilter::btUpClick(TObject *Sender)
{
 int indx,count;

 indx = lbDetail->ItemIndex;
 if (indx == -1) return;
 count = lbDetail->Items->Count;
 if (indx == 0) { lbDetail->Items->Move(indx,count-1); lbDetail->ItemIndex=count-1; }
 else { lbDetail->Items->Move(indx,indx-1);  lbDetail->ItemIndex=indx-1; }

}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btDownClick(TObject *Sender)
{
int indx,count;

 indx = lbDetail->ItemIndex;
 if (indx == -1) return;
 count = lbDetail->Items->Count;
 if (indx == count-1) { lbDetail->Items->Move(indx,0); lbDetail->ItemIndex=0; }

 else { lbDetail->Items->Move(indx,indx+1); lbDetail->ItemIndex=indx + 1; }
}

*/
//---------------------------------------------------------------------------

void __fastcall TxFilter::btOKClick(TObject *Sender)
{

 if (strcmp(mode,"utiliser") == 0) Close();
 else
   {
    strcpy(mode,"utiliser");
    Modify();
   }
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::FormClose(TObject *Sender, TCloseAction &Action)
{

// if (filtres) filtres->CloseEngine();
 Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::SaveData(int mode)
{
 char name[100];
 char Key[100],RetKey[100];
 int rc,recdata,i,nbitems;
 char tmp[2000];

 FILE *fp;
 int count;

 strcpy(Key,eMemoire->Text.c_str());
 if (strlen(Key) == 0)
   {
    Application->MessageBoxA("Pas de Nom de Filtre",ecoplan,MB_OK);
    return;
   }

 if ((rbtn1->Checked==true) && (rbtn2->Checked==true))
   {
    Application->MessageBoxA("Anomalie sur base de d�part",ecoplan,MB_OK);
    return;
   }

strcpy(tmp,m_directory); strcat(tmp,"\\Reference\\Filtres\\"); strcat(tmp,Key);
strcat(tmp,".fil");
fp = fopen(tmp,"wt");


 // preparer buffer
 strcpy(buffer,"<nom>"); strcat(buffer,Key); strcat(buffer,"</nom>");
 strcat(buffer,"<comment>");
 strcat(buffer,mFiltre->Lines->GetText());
 strcat(buffer,"</comment>");
 strcat(buffer,"<tout>");
 if (rbtn1->Checked == true) strcat(buffer,"Y");
 strcat(buffer,"</tout><aucun>");
 if (rbtn2->Checked==true) strcat(buffer,"Y");
 strcat(buffer,"</aucun><criteres>");

 nbitems=asFC->RowCount-1;  // lbDetail->Items->Count;

 for (i=1;i<=nbitems;i++)
   {
     strcpy(tmp,"<A>"); strcat(tmp,asFC->Cells[1][i].c_str()); strcat(tmp,"</A>");
     strcat(tmp,"<C>"); strcat(tmp,asFC->Cells[2][i].c_str()); strcat(tmp,"</C>");
     strcat(tmp,"<R>"); strcat(tmp,asFC->Cells[3][i].c_str()); strcat(tmp,"</R>");
     strcat(tmp,"<V1>"); strcat(tmp,asFC->Cells[4][i].c_str()); strcat(tmp,"</V1>");
     strcat(tmp,"<V2>"); strcat(tmp,asFC->Cells[5][i].c_str()); strcat(tmp,"</V2>");

    strcat(buffer,tmp); strcat(buffer,"|");   // | as separator
   }
 strcat(buffer,"</criteres>");

 strcpy(filename,m_directory);
 strcat(filename,"\\eco_filtres");
 rc = filtres->OpenEngine(filename,248,"ndx","dat");   //256 - 8
 rc = filtres->SetIndexMode(0,0);

 rc = filtres->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc) rc = filtres->RewriteRecord(buffer, strlen(buffer)+1,recdata);
     else
     { recdata=filtres->GetNewRecordNumber();
       rc = filtres->WriteKey(0,Key,recdata);
       rc = filtres->WriteRecord(buffer,strlen(buffer)+1);
       lbFiltre->Items->Add(AnsiString(Key));
     }
filtres->CloseEngine();

strcat(buffer,"\n");
fputs(buffer,fp);
fclose(fp);

Save_Preseance();
change = false;
if (mode) Application->MessageBoxA("Sauvegarde Effectu�e",m_ecoplan,MB_OK);
}

void __fastcall TxFilter::btMemorClick(TObject *Sender)
{
 AnsiString ae= eMemoire->Text;
 AnsiString cc=direct+"\\Ref2\\"+ae+".txt";

 if(ae != ""){
 char str_ = ae[1];
    if(isdigit(str_))
    {
     ShowMessage("Veuillez changer de nom!Pour enregistrer un Filtre, il faut que la d�signation commence obligatoirement par une lettre !!!");
     return;
    }
  }
  if(ae=="")
  {
   ShowMessage("Veuillez entrer un nom pour le filtre");
   return;
  }

 for(int i =0; i< AdvStringGrid1->RowCount; i++)
 {
  AdvStringGrid2->Cells[0][i] = AdvStringGrid1->Cells[1][i];
  AdvStringGrid2->Cells[1][i] = AdvStringGrid1->Cells[2][i];
  AdvStringGrid2->Cells[2][i] = AdvStringGrid1->Cells[3][i];
  AdvStringGrid2->Cells[3][i] = AdvStringGrid1->Cells[4][i];
  AdvStringGrid2->Cells[4][i] = AdvStringGrid1->Cells[5][i];
 }

 AdvStringGrid2->SaveToFile(cc);
 if(lbFiltre->Items->IndexOf(ae) > -1)
 {
     etatf =0;

 }else{
 etatf=1;

 }

  if(etatf==0){
 nomFiltre = eMemoire->Text;
 nomFile = direct+"\\Ref\\";
 AdvStringGrid1->SaveToFile(nomFile+nomFiltre);}else{
  nomFiltre = eMemoire->Text;
 nomFile = direct+"\\Ref\\";
  asFC->SaveToFile(nomFile+nomFiltre);
 }



 SaveData(1);
 RELOADFILTRE[pc]=true;
}

void __fastcall TxFilter::Save_Preseance()
{
FILE *fp;
int i,count;  char tmp[250];
strcpy(filename,m_directory);
strcat(filename,"\\eco_filtres.txt");
count=lbFiltre->Items->Count;
fp = fopen(filename,"wt");
for (i=0;i<count;i++)
  {
   sprintf(tmp,"<l>%s</l>\n",lbFiltre->Items->Strings[i].c_str());
   fputs(tmp,fp);
  }
fclose(fp);


}


//---------------------------------------------------------------------------

void __fastcall TxFilter::lbFiltreClick(TObject *Sender)
{
 int count; int rc,recdata;  AnsiString Content;
 char Key[100],RetKey[100];
 char tmp[5000];
 int indx,lg,i;
 char filename[250];
 AnsiString nomFile;
 AnsiString nomc;

 asFC->Clear();
 AdvStringGrid1->Clear();
asFC->Cells[0][1] = "";

 asFC->Cells[1][0] = "Action";
 asFC->Cells[2][0] = "Crit�res ou Champ";
 asFC->Cells[3][0] = "Condition";
 asFC->Cells[4][0] = "Valeur 1";
 asFC->Cells[5][0] = "Valeur 2";


 Current_Row = 0;

 asFC->RowCount=2;
 OldRow=0;
 step=0;

 indx = lbFiltre->ItemIndex;
 if (indx == -1) return;
 nomFiltre=lbFiltre->Items->Strings[indx];
 strcpy(tmp,lbFiltre->Items->Strings[indx].c_str());
 //lFiltre->Caption=AnsiString(tmp);

 Content = lbFiltre->Items->Strings[indx];
 lbFiltre->Items->Delete(indx);
 lbFiltre->Items->Insert(0,Content);

 Display_Filtre(tmp);
 nomFiltre = eMemoire->Text;
 nomFile = direct+"\\Ref\\";
 AdvStringGrid1->LoadFromFile(nomFile+nomFiltre);

 for(int i = 0; i<asFC->RowCount; i++)
 {

          asFC->Cells[4][i] = AdvStringGrid1->Cells[4][i];
          asFC->Cells[5][i] = AdvStringGrid1->Cells[5][i];
 }

 AdvStringGrid2->RowCount = asFC->RowCount;

}

void __fastcall TxFilter::Display_Filtre(char *s)
{
  int count; int rc,recdata;
 char Key[100],RetKey[100];
 char tmp[5000];
 int indx,lg,i;
 char filename[250];

 strcpy(current_filter,s);
 // Read Data and display it

 strcpy(filename,m_directory);
 strcat(filename,"\\eco_filtres");
 rc = filtres->OpenEngine(filename,248,"ndx","dat");   //256 - 8
 rc = filtres->SetIndexMode(0,0);

  strcpy(Key,current_filter);
  rc=filtres->ReadDirectKey(0,Key,RetKey,&recdata);
  if (rc==0)  //  not defined
   { Application->MessageBoxA("Filtre non trouv� dans la base de donn�es",ecoplan,MB_OK);
    return;
   }

 rc=filtres->ReadRecord(buffer,recdata);
 if (rc)
   { // extract
    ExtractValue(tmp,buffer,"nom",0);
    eMemoire->SetTextBuf(tmp);
    ExtractValue(tmp,buffer,"comment",0);
    mFiltre->Clear();
    mFiltre->SetTextBuf(tmp);

    ExtractValue(tmp,buffer,"tout",0);
    if (strcmp(tmp,"Y")==0) rbtn1->Checked=true; else rbtn1->Checked=false;
    ExtractValue(tmp,buffer,"aucun",0);
    if (strcmp(tmp,"Y")==0) rbtn2->Checked=true; else rbtn2->Checked=false;

    ExtractValue(tmp,buffer,"criteres",0);

    //lbDetail->Clear();
    count = explode('|',tmp);
    if (count > 0)
      {
        for (i=0;i<count;i++)
          { if (strlen(params[i])) Strip(params[i]); }
     }
   }
 filtres->CloseEngine();
}

int __fastcall TxFilter::ExtractValue(char *result, char *buff, char *tag, int posdeb)
{
 char tmp[250];
 char *p,*p1,*p2;
 int pos,l;

 result[0]=0;
 strcpy(tmp,"<"); strcat(tmp,tag); strcat(tmp,">");
 p = strstr(buff,tmp);
 l=0;
 if (p)
   {
    strcpy(tmp,"</"); strcat(tmp,tag); strcat(tmp,">");
    p1= strstr(buff,tmp);
    if (p1)
      {
       p2=p + strlen(tag)+2; l= p1-p2;
       strncpy(result,p2,l);
       result[l]=0;
      }
   }
 return l;
}


int __fastcall TxFilter::explode(char sep,char *str)
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


//---------------------------------------------------------------------------

void __fastcall TxFilter::btNewClick(TObject *Sender)
{
 int r,c,rc,cc;
 mFiltre->Clear();
 cc=asFC->ColCount; rc=asFC->RowCount;
 for (r=1;r<rc;r++)
  {
   for (c=1;c<cc;c++)  asFC->Cells[c][r]="";
  }
 asFC->RowCount=2;

 eMemoire->Clear();
 Clean(1);
asFC->Cells[1][0] = "Action";
asFC->Cells[2][0] = "Crit�re ou Champ";
asFC->Cells[3][0] = "Condition";
asFC->Cells[4][0] = "Valeur 1";
asFC->Cells[5][0] = "Valeur 2";
etatf=1;
 //lFiltre->Caption="Aucun";
}
//---------------------------------------------------------------------------


void __fastcall TxFilter::btStepClick(TObject *Sender)
{
 TChamps *Champs; int indx;
 char tmp[1000];  int i,cnt;

 cnt=asFC->RowCount;


 Current_Row++;
 if (Current_Row >= cnt)
    { Application->MessageBoxA("Fin du filtre atteinte","Ecoplanning 5.0",MB_OK);
      Current_Row=0;
      return;
    }  

 //lCond->Caption = AnsiString(Current_Row);
 // preparer la ligne "filtre" dans pasapas
 //indx = lbDetail->ItemIndex;
 if (OldRow==0) { OldRow=1; step=1;} //return;
 else step++;
 if (step>=cnt) return;

 tmp[0]=0;

 //i=OldRow;

 i=Current_Row;
 strcat(tmp,"<A>"); strcat(tmp,asFC->Cells[1][i].c_str()); strcat(tmp,"</A>");
 strcat(tmp,"<C>"); strcat(tmp,asFC->Cells[2][i].c_str()); strcat(tmp,"</C>");
 strcat(tmp,"<R>"); strcat(tmp,asFC->Cells[3][i].c_str()); strcat(tmp,"</R>");
 strcat(tmp,"<V1>"); strcat(tmp,asFC->Cells[4][i].c_str()); strcat(tmp,"</V1>");
 strcat(tmp,"<V2>"); strcat(tmp,asFC->Cells[5][i].c_str()); strcat(tmp,"</V2>");

 strcpy(m_pasapas,tmp);   //lbDetail->Items->Strings[indx].c_str());

 Champs =  new TChamps(Application);
 Champs->ShowModal();
 delete Champs;




 // resultat est dans m_exchange
 // on prepare pour filtre dans prog princ
 if (strlen(m_exchange)==0) return;

 // On doit remettre a jour la rang�e.
 ExtractValue(tmp,m_exchange,"A",0); asFC->Cells[1][i]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"C",0); asFC->Cells[2][i]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"R",0); asFC->Cells[3][i]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"V1",0); asFC->Cells[4][i]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"V2",0); asFC->Cells[5][i]=AnsiString(tmp);



 strcpy(m_filtre,"<P>");

  if (i==1)
  {strcat(m_filtre,"<I>");
    if (rbtn1->Checked==true) strcat(m_filtre,"T</I>"); else strcat(m_filtre,"N</I>");
  }

 strcat(m_filtre,m_exchange);

}

//---------------------------------------------------------------------------

void __fastcall TxFilter::btAnnulClick(TObject *Sender)
{
  if (change)
   {
    if (Application->MessageBoxA("Sauvegarde de vos donn�es ?",m_ecoplan,MB_YESNO)==IDYES)
       SaveData(1);

   }
 Close();
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btLigneClick(TObject *Sender)
{
 //

 TChamps *Champs; int indx;

 strcpy(m_pasapas,"");

 Champs =  new TChamps(Application);
 Champs->ShowModal();
 delete Champs;

 // resultat est dans m_exchange
 // on prepare pour filtre dans prog princ
 if (strlen(m_exchange)==0) return;

 // lbDetail->Items->Add(AnsiString(m_exchange));

Strip(m_exchange);
}

void __fastcall TxFilter::Strip(char *s)
{
 char tmp[250];
 int Row;   int count;
 Row = asFC->RowCount-1;
 if ((Row>=1) && (asFC->Cells[0][Row] != "")) asFC->RowCount++;
 Row=asFC->RowCount-1;;

 asFC->Cells[0][Row]=Row;

 ExtractValue(tmp,s,"A",0); asFC->Cells[1][Row]=AnsiString(tmp);
 ExtractValue(tmp,s,"C",0); asFC->Cells[2][Row]=AnsiString(tmp);

  ExtractValue(tmp,s,"R",0); asFC->Cells[3][Row]=AnsiString(tmp);

 ExtractValue(tmp,s,"V1",0); asFC->Cells[4][Row]=AnsiString(tmp);
 ExtractValue(tmp,s,"V2",0); asFC->Cells[5][Row]=AnsiString(tmp);
}


//---------------------------------------------------------------------------

void __fastcall TxFilter::btModClick(TObject *Sender)
{
char tmp[500]; int indx,lg,count;
 char tmp1[500];

 //indx = lbDetail->ItemIndex;
 if (OldRow==0)
   {
   Application->MessageBoxA("S�lectionner une ligne D�tail",m_ecoplan,MB_OK);
   return;
   }


 /*
 strcpy(tmp,lbDetail->Items->Strings[indx].c_str());
 lg = ExtractValue(champ,tmp,"C",0);
 lg = ExtractValue(oper,tmp,"R",0);
 lg = ExtractValue(val1,tmp,"V1",0);
 lg = ExtractValue(val2,tmp,"V2",0);
 lg = ExtractValue(action,tmp,"A",0);
 */

 strcpy(tmp,"<A>"); strcat(tmp,asFC->Cells[1][OldRow].c_str()); strcat(tmp,"</A>");
 strcat(tmp,"<C>"); strcat(tmp,asFC->Cells[2][OldRow].c_str()); strcat(tmp,"</C>");
 strcat(tmp,"<R>"); strcat(tmp,asFC->Cells[3][OldRow].c_str()); strcat(tmp,"</R>");
 strcat(tmp,"<V1>"); strcat(tmp,asFC->Cells[4][OldRow].c_str()); strcat(tmp,"</V1>");
 strcat(tmp,"<V2>"); strcat(tmp,asFC->Cells[5][OldRow].c_str()); strcat(tmp,"</V2>");

 strcpy(m_pasapas,tmp);

 TChamps *Champs;

 Champs =  new TChamps(Application);
 Champs->ShowModal();
 delete Champs;

 // resultat est dans m_exchange
 // on prepare pour filtre dans prog princ
 if (strlen(m_exchange)==0) return;

 // lbDetail->Items->Strings[indx]= AnsiString(m_exchange);
 ExtractValue(tmp,m_exchange,"A",0); asFC->Cells[1][OldRow]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"C",0); asFC->Cells[2][OldRow]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"R",0); asFC->Cells[3][OldRow]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"V1",0); asFC->Cells[4][OldRow]=AnsiString(tmp);
 ExtractValue(tmp,m_exchange,"V2",0); asFC->Cells[5][OldRow]=AnsiString(tmp);
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btEffacerClick(TObject *Sender)
{
 int indx; int rc,recdata; char Key[100],RetKey[100];
 char filename[250];

indx = lbFiltre->ItemIndex;

if (lbFiltre->Items->Count==0) return;

indx=0;
if (lbFiltre->Items->Strings[indx]=="") return;

// if (indx == -1)
//    {
//     Application->MessageBoxA("Pas de Filtre S�lectionn�",m_ecoplan,MB_OK);
//     return;
//    }
 if (Application->MessageBoxA("Confirmer l'effacement du Filtre",m_ecoplan,MB_YESNO)==IDNO)
    return;

 strcpy(filename,m_directory);
 strcat(filename,"\\eco_filtres");
 rc = filtres->OpenEngine(filename,248,"ndx","dat");   //256 - 8
 rc = filtres->SetIndexMode(0,0);
 strcpy(Key,lbFiltre->Items->Strings[indx].c_str());
 rc = filtres->ReadDirectKey(0,Key,RetKey,&recdata);
 if (rc)
  {
   rc=filtres->DeleteKey(0,Key,recdata);
   rc=filtres->DeleteRecord(recdata);
  }
 else { Application->MessageBoxA("Record Not Found",m_ecoplan,MB_OK);
        return;
      }

  int r,c,xc,cc;
 cc=asFC->ColCount; xc=asFC->RowCount;
 for (r=0;r<xc;r++)
  {
   for (c=1;c<cc;c++)  asFC->Cells[c][r]="";
  }

 asFC->RowCount=2;

 indx=0;
 lbFiltre->Items->Delete(indx);
 Save_Preseance();

 mFiltre->Clear();
// lFiltre->Caption = "";
 eMemoire->Clear();
 change = true;
 filtres->CloseEngine();
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::BitBtn1Click(TObject *Sender)
{
 // Up Move
  int indx,count;
 char tmp1[100],tmp2[100];

 indx = lbFiltre->ItemIndex;
 if (indx == -1) return;
 count = lbFiltre->Items->Count;
 if (indx == 0) { lbFiltre->Items->Move(indx,count-1); lbFiltre->ItemIndex=count-1; }
 else { lbFiltre->Items->Move(indx,indx-1);  lbFiltre->ItemIndex=indx-1; }
 Save_Preseance();
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::BitBtn2Click(TObject *Sender)
{
 // Down Move
 int indx,count;
 char tmp1[100],tmp2[100];

 indx = lbFiltre->ItemIndex;
 if (indx == -1) return;
 count = lbFiltre->Items->Count;
 if (indx == count-1) { lbFiltre->Items->Move(indx,0); lbFiltre->ItemIndex=0; }

 else { lbFiltre->Items->Move(indx,indx+1); lbFiltre->ItemIndex=indx + 1; }
 Save_Preseance();
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btHautClick(TObject *Sender)
{
//
 int XRow,i;
 int indx,count;
 AnsiString l0,l1,l2,l3,l4,l5;

 if (OldRow==0)
   {Application->MessageBoxA("Pas de ligne s�lectionn�e",m_ecoplan,MB_OK);
    return;
   }

 indx=OldRow; count=asFC->RowCount-1;
 if(OldRow == 1) return;
 // l0= asFC->Cells[0][OldRow];
 l1= asFC->Cells[1][OldRow];
 l2= asFC->Cells[2][OldRow];
 l3= asFC->Cells[3][OldRow];
 l4= asFC->Cells[4][OldRow];
 l5= asFC->Cells[5][OldRow];
 XRow=OldRow-1;
 // asFC->Cells[0][OldRow]=asFC->Cells[0][XRow]; asFC[0][XRow]=l0;
 asFC->Cells[1][OldRow]=asFC->Cells[1][XRow]; asFC->Cells[1][XRow]=l1;
 asFC->Cells[2][OldRow]=asFC->Cells[2][XRow]; asFC->Cells[2][XRow]=l2;
 asFC->Cells[3][OldRow]=asFC->Cells[3][XRow]; asFC->Cells[3][XRow]=l3;
 asFC->Cells[4][OldRow]=asFC->Cells[4][XRow]; asFC->Cells[4][XRow]=l4;
 asFC->Cells[5][OldRow]=asFC->Cells[5][XRow]; asFC->Cells[5][XRow]=l5;
 OldRow=XRow;

}
//---------------------------------------------------------------------------

void __fastcall TxFilter::asFCClickCell(TObject *Sender, int ARow,
      int ACol)
{
 if (ARow==0) return;
 OldRow=ARow;
 OldCol=ACol;
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btBasClick(TObject *Sender)
{
int XRow,i;
 int indx,count;
 AnsiString l0,l1,l2,l3,l4,l5;
 count = asFC->RowCount-1;
 if (OldRow==0)
   {Application->MessageBoxA("Pas de ligne s�lectionn�e",m_ecoplan,MB_OK);
    return;
   }

 if(OldRow == count) return;
 if (OldRow ==0) return;
 // l0= asFC->Cells[0][OldRow];
 l1= asFC->Cells[1][OldRow];
 l2= asFC->Cells[2][OldRow];
 l3= asFC->Cells[3][OldRow];
 l4= asFC->Cells[4][OldRow];
 l5= asFC->Cells[5][OldRow];
 XRow=OldRow+1;
 //asFC->Cells[0][OldRow]=asFC->Cells[0][XRow]; asFC[0][XRow]=l0;
 asFC->Cells[1][OldRow]=asFC->Cells[1][XRow]; asFC->Cells[1][XRow]=l1;
 asFC->Cells[2][OldRow]=asFC->Cells[2][XRow]; asFC->Cells[2][XRow]=l2;
 asFC->Cells[3][OldRow]=asFC->Cells[3][XRow]; asFC->Cells[3][XRow]=l3;
 asFC->Cells[4][OldRow]=asFC->Cells[4][XRow]; asFC->Cells[4][XRow]=l4;
  asFC->Cells[5][OldRow]=asFC->Cells[5][XRow]; asFC->Cells[5][XRow]=l5;
 OldRow=XRow;

}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btAffPClick(TObject *Sender)
{
  char Key[100];  int i;
  char tmp[500];
// Assigner Tri
 strcpy(Key,eMemoire->Text.c_str());
 if (strlen(Key) == 0)
   {
    Application->MessageBoxA("Aucun Filtre S�lectionn�",m_ecoplan,MB_OK);
    return;
   }

strcpy(m_assign,"<fil>"); strcat(m_assign,eMemoire->Text.c_str());
strcat(m_assign,"</fil>");
strcat(m_assign,"<org>ModFilter:AffecterPL</org>");
strcpy(P_FILTRE[pc],eMemoire->Text.c_str());
RELOADFILTRE[pc] = true;
SaveData(0);
for (i=0;i<6;i++)
   {
    strcpy(P_FILTRE[i],eMemoire->Text.c_str());
    RELOADFILTRE[i]=true;
   }



 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt><exec>O</exec>",pc,
    P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 strcpy(m_planning,tmp);
Close();
}
//---------------------------------------------------------------------------


void __fastcall TxFilter::asFCCanEditCell(TObject *Sender, int ARow,
      int ACol, bool &CanEdit)
{
 if (ARow==0) return;
 CanEdit=false;
 if (ACol==4 || ACol==5) CanEdit = true;
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btAffRClick(TObject *Sender)
{
  char Key[100];

 strcpy(Key,eMemoire->Text.c_str());
 if (strlen(Key) == 0)
   {
    Application->MessageBoxA("Aucun Filtre S�lectionn�",m_ecoplan,MB_OK);
    return;
   }

strcpy(m_assign,"<fil>"); strcat(m_assign,eMemoire->Text.c_str());
strcat(m_assign,"</fil>");
strcat(m_assign,"<org>ModFilter:AffecterRapport</org>");
strcpy(P_FILTRE[pc],eMemoire->Text.c_str());
Close();
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::btAideClick(TObject *Sender)
{
/*Thelp *Aide;
 strcpy(m_help,"Filtrer.html#Filtre");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide; */
}
//---------------------------------------------------------------------------

void __fastcall TxFilter::Button1Click(TObject *Sender)
{
 lbFiltre->Sorted = true;
 Save_Preseance();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TxFilter::Button2Click(TObject *Sender)
{
etatfiltre =1;
}
//---------------------------------------------------------------------------

