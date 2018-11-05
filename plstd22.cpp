//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

                                                            
#include <stdio.h>
#include <dstring.h>       

#include "struct.h"
#include <math.h>        
#include <dos.h>

#include "plstd.h"
#include "num_pred.h"
#include "num_succ.h"
#include "inserttask.h"
#include "copytask.h"
#include "liencc.h"
#include "tachesamont.h"
#include "affcal.h"
#include "actualisation.h"
#include "modfilter.h"
#include "modGhost.h"
#include "modCriteres.h"
#include "modPrepPlanning.h"
#include "modPrepRapport.h"
#include "modTri.h"
#include "modLignesBrisees.h"

#include "modAffTC.h"
#include "modRemplTC.h"
#include "modAffPred.h"
#include "modAffSucc.h"
#include "modhelp.h"

#include "modPreview.h"

#include "modEntete.h"
#include "modecadre.h"
#include "modDcal.h"
#include "modTaches.h"
#include "modCorps.h"
#include "modPied.h"
#include "modBandes.h"
#include "modMarges.h"
#include "modColonnes.h"
#include "modTcol.h"
#include "modTligne.h"
#include "modDescEnvelop.h"
#include "modMiseEnpage.h"
#include "modDistVert.h"
#include "modRenum.h"
#include "modperiode.h"
#include "modXLS.h"

#include <dos.h>
#include <io.h>

#include <fcntl.h>


// #include "ikey.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGrid"
#pragma link "BaseGrid"
#pragma resource "*.dfm"
TplStandard *plStandard;
int cpter = 0;
int cpter2 = 0;
int nrtaches[MAX_T];
int compteurt;
int tachec;
static int ARowT = 0;
static int modeal = 0;
bool choixperiode;
Graphics::TBitmap *imP2 = new Graphics::TBitmap();
//---------------------------------------------------------------------------
__fastcall TplStandard::TplStandard(TComponent* Owner)
        : TForm(Owner)
{

}


void __fastcall TplStandard::Synchro_LibCrit()
{
 int i,j,cntcrit,pos,posav, numt;
 char tmp[500]; //���

 AnsiString libcrit;
 cntcrit = asCrit->RowCount;
 for (i=1;i<cntcrit;i++)
   {
    strcpy(tmp,asCrit->Cells[1][i].c_str());
    numt = atoi(tmp);
    for (j=0; j<8; j++)
      {

       pos = j*2+3;
       //if (asCrit->Cells[pos][i]!="")
       //  {
          libcrit = text_criteres(j,asCrit->Cells[pos][i]);
          asCrit->Cells[pos+1][i] = libcrit;
          if (numt != 0)
            {
             posav = j + 51;
             av->Cells[posav][numt]=libcrit;

            }

        // }
      }
   }

}


 void __fastcall TplStandard::xstrcat(char *dst, char * src)
 {
  int ldst,lsrc,sof;
  ldst = strlen(dst); lsrc = strlen(src); sof=sizeof(dst);
  if (ldst+lsrc > sof)
   {
    sof = sof;
   }
  else strcat(dst,src);

 }


//---------------------------------------------------------------------------
void __fastcall TplStandard::FormClose(TObject *Sender,
      TCloseAction &Action)
{
 int recdata;
 char Key[200],RetKey[200];
  if (im) delete im;


  if (slrupt) delete slrupt;
  if (slrupt) delete tchcrit;
  if (VERSION == true)
   {
    if (!SAVE_AVANC)
    Save_Actualisation(false);
   }
  SaveConfig();
    // avancements
   if (proj)
    {

    }

 // if (change)
 updatedatabase(LocalBase,0,0); // *$$$$  0);
 if (proj) { proj->CloseEngine(); //delete proj;
           }

 // if(FLAG_AVANC_LOADED) { avanc->CloseEngine(); avanc->~realisam(); }

 // on signale a MainForm qu'on ferme
 strcpy(m_status,"<close>"); strcat(m_status,DatabaseName);
 strcat(m_status,"</close>");
 
// ikey_CloseDevice( handle );
// ikey_DeleteContext( handle );
 Sleep(1000);
 Action = caFree;
}
//---------------------------------------------------------------------------

int __fastcall TplStandard::Change1()
{
 change=1;
 return 0;
}

int __fastcall TplStandard::Change0()
{
 change=0;
 return 0;
}

void __fastcall TplStandard::updatedatabase(char *name, int prompt,int mode)
{
  int rc;
  int recdata;
  char buffer[15000];
  char RetKey[200];
  char Key[200];
  char tmp[15000];
  char format[30000];
  int ipred,isucc;
  int ii,cnt;
  char ss[20];
  char strpred[15000],strsucc[15000];
  char cdebsouh[20],cfinsouh[20];

  int result;
  char xtmp[300]; char *p;


 if (prompt==1)
  {
    result = Application->MessageBox("Voulez vous sauvegarder les modifications du projet",
        name,MB_YESNO);
    if (result == IDNO) return; // || result

   }
    if (proj) { rc = proj->CloseEngine(); //delete proj;


              }

    SaveCriteres(false);

    strcpy(buffer,name); strcat(buffer,".dat");
    rc=unlink(buffer);
    strcpy(buffer,name); strcat(buffer,".eco");
    rc=unlink(buffer);

    proj = new realisam;
    rc = proj->OpenEngine(name,120,"eco","dat");   //512 - 8
    rc = proj->SetIndexMode(0,0);


    if (rc==0) { ShowMessage("Erreur � l'ouverture des fichiers projets"); exit (1); }

    // titre de la base
    Write_Debut_DB(mode);
    // Taches par defaut
    cnt=av->RowCount;

    for (int i=1;i<cnt;i++)
      {
       if (av->Cells[1][i] != "")
       {
       strcpy(tmp,av->Cells[1][i].c_str());
       ii=atoi(tmp);
       //  predecesseurs,
       strcpy(strpred,"|");
       sprintf(Key,"<PR%04d>",ii);
       recdata=proj->GetNewRecordNumber();
       rc = proj->WriteKey(0,Key,recdata);
       ipred=1;
       buffer[0]=0; sprintf(buffer,"<PR%04d>",ii);
       while ((pr[ii][ipred]!=0) && (ipred<PS))       //
         {

            sprintf(tmp,"<P%02d><num>%d</num><typ>%d</typ><decal>%d</decal></P%02d>",
               ipred,pr[ii][ipred],pr_tl[ii][ipred],pr_decal[ii][ipred],ipred);
            strcat(buffer,tmp);
          sprintf(ss,"%d|",pr[ii][ipred]);
          strcat(strpred,ss);
          ipred++;
         }
       //len =strlen(strpred);
       nbpred=ipred-1;
       sprintf(tmp,"</PR%04d>",ii); strcat(buffer,tmp);
       rc = proj->WriteRecord(buffer,strlen(buffer)+1);

       //  successeurs
       sprintf(Key,"<SU%04d>",ii);
       recdata=proj->GetNewRecordNumber();
       rc = proj->WriteKey(0,Key,recdata);
       isucc=1; strcpy(strsucc,"|");
       buffer[0]=0; sprintf(buffer,"<SU%04d>",ii);
       while ((su[ii][isucc]!=0)  && (isucc<PS))
         {
             sprintf(tmp,"<S%02d><num>%d</num><typ>%d</typ><decal>%d</decal></S%02d>",
               isucc,su[ii][isucc],su_tl[ii][isucc],su_decal[ii][isucc],isucc);
            strcat(buffer,tmp);
            sprintf(ss,"%d|",su[ii][isucc]);
            strcat(strsucc,ss);
          isucc++;
         }
       //len=strlen(strsucc);
       nbsucc=isucc-1;
       sprintf(tmp,"</SU%04d>",ii); strcat(buffer,tmp);
       rc = proj->WriteRecord(buffer,strlen(buffer)+1);

       sprintf(Key,"<T%04d>",ii);
       recdata=proj->GetNewRecordNumber();
       rc = proj->WriteKey(0,Key,recdata);


       strcpy(cdebsouh,av->Cells[12][ii].c_str()); //dds[i],
       strcpy(cfinsouh,av->Cells[13][ii].c_str()); //dfs[i],

       strcpy(xtmp,av->Cells[2][i].c_str());
       p=strstr(xtmp,"\r");
       if (p) *p=0;
       p=strstr(xtmp,"\n");
       if (p) *p=0;
       av->Cells[2][i] = AnsiString(xtmp);

       strcpy(buffer,av->Cells[2][i].c_str());     // $$$$$ PAS II

       // verif data

       strcpy(tmp,av->Cells[3][i].c_str());    // duree[i],
       strcpy(tmp,av->Cells[4][i].c_str());    /// debut plus tot
       strcpy(tmp,av->Cells[5][i].c_str());



       strcpy(format,"<T>%d</T><nom>%s</nom><duree>%s</duree><cal>%s</cal><mom>%s</mom>");
       strcat(format,"<ct>%s</ct><da>%s</da><dtr>%s</dtr><ftr>%s</ftr><pc>%s</pc><nbj>%s</nbj>");
       strcat(format,"<dds>%s</dds><dfs>%s</dfs>");
       strcat(format,"<nbpred>%d</nbpred><nbsucc>%d</nbsucc>");
       strcat(format,"<pred>%s</pred><succ>%s</succ><coutp>%s</coutp><coutn>%s</coutn>");
       strcat(format,"<cdtch>%s</cdtch><chl1>%s</chl1><chl2>%s</chl2><chl3>%s</chl3>");
       sprintf(tmp,format,
           ii,
           buffer,       // OK
           av->Cells[3][i].c_str(),    // duree[i],
           av->Cells[10][i].c_str(),     // tcal[i],
           tmom[i],        ///
           av->Cells[11][i].c_str(),  //codetache[i],
           da[i],
           av->Cells[4][i].c_str(),    /// debut plus tot
           av->Cells[5][i].c_str(),    // fin plus tot
           av->Cells[31][i].c_str(),    // pourcentage avancementpca[i],
           av->Cells[32][i].c_str(),     // Nb Jour Restant nbj[i],

           av->Cells[12][i].c_str(),// cdebsouh,
           av->Cells[13][i].c_str(), //cfinsouh,
           nbpred,             // OK
           nbsucc,             // OK
           strpred,            // OK
           strsucc,            // OK
           av->Cells[18][i].c_str(),      /// couts pos et neg
           av->Cells[19][i].c_str(),      /// cout neg
           av->Cells[11][i].c_str(),      // code tache
           av->Cells[15][i].c_str(),      /// Champ1
           av->Cells[16][i].c_str(),      /// champ2
           av->Cells[17][i].c_str()       /// Champ3

           );

       }  // if (ii !=0)
       rc = proj->WriteRecord(tmp,strlen(tmp)+1);

     }      // boucle for

     proj->CloseReopen(name,120,"eco","dat");    //  CloseEngine();
     rc = proj->SetIndexMode(0,0);


   Change0();
   if (prompt==1) Application->MessageBoxA("Sauvegarde effectu�e",m_ecoplan,MB_OK);
}

void __fastcall TplStandard::Write_Debut_DB(int mode)
{
  int recdata,rc; char Key[200];   char tmp[15000];
  char fmt[250]; int i,j; char coul[30],e[20],t[20];

     strcpy(Key,"<dbtitre>"); recdata=proj->GetNewRecordNumber();
     rc = proj->WriteKey(0,Key,recdata);
     rc = proj->WriteRecord(TITRE,strlen(TITRE)+1);

     strcpy(Key,"<dbobserv>"); recdata=proj->GetNewRecordNumber();
     rc = proj->WriteKey(0,Key,recdata);
     rc = proj->WriteRecord(OBSERV,strlen(OBSERV)+1);

    // avancements
     if (mode) dateavanc[0]=0;
     strcpy(Key,"<avanc>"); recdata=proj->GetNewRecordNumber();
     rc = proj->WriteKey(0,Key,recdata);
     rc = proj->WriteRecord(dateavanc,strlen(dateavanc)+1);
     strcpy(Key,"<oldavanc>");recdata=proj->GetNewRecordNumber();
     rc = proj->WriteKey(0,Key,recdata);
     // strcpy(FileNameAvanc,"");
     rc = proj->WriteRecord(FileNameAvanc,strlen(FileNameAvanc)+1);
    // date previsionnelle
     strcpy(Key,"<dateprevdebut>"); recdata=proj->GetNewRecordNumber();
     rc = proj->WriteKey(0,Key,recdata);
     rc = proj->WriteRecord(dateprevdeb,strlen(dateprevdeb)+1);
     // nombre de taches
     strcpy(Key,"<taches>"); recdata=proj->GetNewRecordNumber();
     rc = proj->WriteKey(0,Key,recdata);
     sprintf(tmp,"%04d",nb_taches);
     rc = proj->WriteRecord(tmp,strlen(tmp)+1);
     strcpy(tmp,"");
     for (i=1;i<5;i++)
       {
        sprintf(fmt,"<PC%02d>",i);  strcat(tmp,fmt);
        for (j=1;j<20;j++)
          {
           sprintf(fmt,"<J%02d>",j);  strcat(tmp,fmt);
           sprintf(e,"%7.2f",E_BRISEES[i][j]);
           if (L_BRISEES[i][j]==true) strcpy(t,"Oui"); else strcpy(t,"Non");
           sprintf(fmt,"<c>%d</c><e>%s</e><t>%s</t>",C_BRISEES[i][j],e,t);
           strcat(tmp,fmt);
           sprintf(fmt,"</J%02d>",j);  strcat(tmp,fmt);

          }
        sprintf(fmt,"</PC%02d>",i);  strcat(tmp,fmt);
       }
      strcpy(Key,"<brisees>"); recdata=proj->GetNewRecordNumber();
      rc = proj->WriteKey(0,Key,recdata);
      rc = proj->WriteRecord(tmp,strlen(tmp)+1);

}





int __fastcall TplStandard::exist_tache (int t,int range)
{
 int i,ret;
 ret=0;


 if (av->Cells[1][t]=="") return 0;
 else return t;


/*
 for (i=1;i<=range+1;i++) { if (t==sommets[i]) { ret=i; break; } }
 return ret;
 */
}

//---------------------------------------------------------------------------

int __fastcall TplStandard::Explode(char sep,char *str)
{
 int i, l, count, ip;
 char c,cpred;
 l = strlen(str);
 if (l==0) return 0;
 count = 0; params[count][0]=0; // indice params
 c=str[0]; cpred=0;
 i=0; ip=0;
 while (c)
  { cpred=c;
   if (c==sep)
     { params[count][ip]=0; count++; ip=0; if (count > 49) count=49; }
   else
     { params[count][ip]=c;  ip++; }
   i++; c=str[i];
  }
 if (cpred==sep) count--; return (count+1);
}

void __fastcall TplStandard::Load_Fields()
{
 FILE *fp;
 char tmp[550]; //����
 int col,row;
 char *p;

 strcpy(tmp,m_directory); strcat(tmp,"\\champs.txt");
 fp=fopen(tmp,"r+b");
 //fp = fopen("champs.txt","r+b");
 if (!fp) return;
 while (!feof(fp))
   {
    tmp[0]=0;
    fgets(tmp,99,fp);
    strcat(tmp,",");
    Explode(',',tmp);
    col=atoi(params[0]);
    row=atoi(params[1]);
    if (row>0)
     {  p=strstr(params[2],"\r"); if (p) *p=0;
        if (col==2) strcpy(ch_cod[row],params[2]);   // Code Champ
        if (col==3) strcpy(ch_des[row],params[2]);   // D�signation
        if (col==4) strcpy(ch_abb[row],params[2]);   // Abbreviation
        if (col==5) strcpy(ch_typ[row],params[2]);   // Type
        if (col==6) strcpy(ch_sai[row],params[2]);   // Saisie OUI/NON
        if (col==13) strcpy(ch_esp[row],params[2]);  // Largeur dans la grille
       }
  }
 fclose(fp);
}

int  __fastcall TplStandard::getvalue(int vol, int row, int ch)
{
 AnsiString val1; int col,x;
 TAdvStringGrid *avs;
// obtient valeur en fonction du champ systeme adv (valeur en retour stockee
// dans char retvalue; la colonne est calcul�e en fonction du champ
if (vol==4) avs=AdvStringGrid4;
else if (vol==6) avs=AdvStringGrid5;
else if (vol==7) avs=AdvStringGrid6;
else if (vol==7) avs=AdvStringGrid7;
else if (vol==5) avs= asAvanc;
else if (vol==1) avs=AdvStringGrid1;
else if (vol==2) avs=AdvStringGrid2;
else if (vol==3) avs=AdvStringGrid3;
col=0;
while(col < MAX_CHP)
 {
   if (vl_chp[vol][col]==ch)break;
   col++;
 }
if (col>=MAX_CHP) {strcpy(adv,"");
Application->MessageBoxA("Get Value: erreur dans decodage des champs",m_ecoplan,MB_OK);
return 0; }
val1=avs->Cells[col][row];
strcpy(adv,val1.c_str());
x=atoi(adv);
return x;
}

int  __fastcall TplStandard::setvalue(int vol,int row, int ch, char *p)
{
 int col;
 TAdvStringGrid *avs;
 // place la valeur stockee dans char p   vers la  ligne du volet,
 // dans la colonne a calculer d'apres ch
 col=0;
if (vol==4) avs=AdvStringGrid4;
else if (vol==6) avs=AdvStringGrid5;
else if (vol==7) avs=AdvStringGrid6;
else if (vol==8) avs=AdvStringGrid7;
else if (vol==5) avs= asAvanc;
else if (vol==1) avs=AdvStringGrid1;
else if (vol==2) avs=AdvStringGrid2;
else if (vol==3) avs=AdvStringGrid3;

while(col < MAX_CHP)
 {
   if (vl_chp[vol][col]==ch)break;
   col++;
 }
if (col>=MAX_CHP) {strcpy(adv,"");
Application->MessageBoxA("SetValue : erreur dans decodage des champs",m_ecoplan,MB_OK);
return 0; }
avs->Cells[col][row]= AnsiString(p);
return 1;
}


void __fastcall TplStandard::Init_Volet(int vol,TAdvStringGrid *avs)
{
 int i; //for (i=1;i<10;i++)  SGChamps->ColWidths[i]=50;
 // lecture du fichiers des champs
 FILE *fp;
 char tmp[200]; //���
 char filename[255];
 int col,row,cnt,nbcol,numchamp,nbchamps;
 char *p;
 int esp;


 // clean zone volets
 for (i=0;i<20;i++)  vl_chp[vol][i]=0;
  // clean Grille (lib�lles, texte, etc..
 avs->RowCount=2;
 nbcol = avs->ColCount;
 for (i=0;i<nbcol;i++)
   {
    avs->Cells[i][0]="";
    avs->Cells[i][1]="";
   }

 nbchamps=0;
 sprintf(filename,"%s\\Volet%03d.txt",m_directory,vol);
 fp = fopen(filename,"r+b");
 if (!fp) return;
 while (!feof(fp))
   {
    tmp[0]=0;
    fgets(tmp,99,fp);
    strcat(tmp,",");
    cnt=Explode(',',tmp);
    col=atoi(params[0]);
    row=atoi(params[1]);
    if ((row>0)&& (col>1) && (cnt>2))
     {  p=strstr(params[2],"\r"); if (p) *p=0;
        if (params[2][0]=='C') p=params[2]+1;
        numchamp=atoi(p);
        vl_chp[vol][col-2]=numchamp;
        nbchamps++;
       }
  }
 fclose(fp);

 // boucle de chargement des labels et espacement
 if (nbchamps>avs->ColCount)
   { Application->MessageBoxA("Erreur definition Volet",m_ecoplan,MB_OK);
     nbchamps=avs->ColCount;
   }
 for (i=0;i<nbchamps;i++)
   {
    numchamp = vl_chp[vol][i];
    if (numchamp > MAX_CHAMP) numchamp = 51;  // champ neutre
    // espacement
    esp=atoi(ch_esp[numchamp]);
    avs->ColWidths[i]=esp;
    // ici on fait du HTML pour indiquer saisie possible
    if (strcmp(ch_sai[numchamp],"N") != 0)
      {
       strcpy(tmp,"<FONT color=\"clGreen\">");
       strcat(tmp,ch_abb[numchamp]);
       strcat(tmp,"</FONT>");
       avs->Cells[i][0]= AnsiString(tmp);
      }

    else avs->Cells[i][0]= AnsiString(ch_abb[numchamp]);
   }
}

int __fastcall TplStandard::Extraire_Date(char *fn)
{
 char *p;
 char tmp[100];
 char an[3],mois[3],jour[3];

 p = strstr(fn,"-");
 if (p)
   { strncpy(an,p+1,2); an[2]=0; AnAvanc=atoi(an);
     strncpy(mois,p+4,2); mois[2]=0; MoisAvanc=atoi(mois);
     strncpy(jour,p+7,2); jour[2]=0;  JourAvanc=atoi(jour);
     strcpy(tmp,jour); strcat(tmp,"/");
     strcat(tmp,mois); strcat(tmp,"/");
     strcat(tmp,an);
     // strcpy(DATEAVANC,tmp);
     Label12->Caption = AnsiString(tmp);
     Label118->Caption = AnsiString(tmp);
     Label68->Caption = AnsiString(tmp);
   }
 return 0;
}

void __fastcall TplStandard::Clear_Diagram(TImage *im)
{

 TRect Diagram;
 Diagram = Rect(0,0, im->Width,im->Height);
 im->Canvas->Brush->Color = clWhite; // BgColor;
 im->Canvas->FillRect(Diagram);
 im->Canvas->Pen->Color = clBlack;
 im->Canvas->MoveTo(0,0);
 im->Canvas->LineTo(0,im->Height-1);
 im->Canvas->LineTo(im->Width-1,im->Height-1);
 im->Canvas->LineTo(im->Width-1,0);
 im->Canvas->LineTo(0,0);
}

void __fastcall TplStandard::Border_Diagram(TImage *im)
{


 im->Canvas->Pen->Color = clBlack;
 im->Canvas->MoveTo(0,0);
 im->Canvas->LineTo(0,im->Height-1);
 im->Canvas->LineTo(im->Width-1,im->Height-1);
 im->Canvas->LineTo(im->Width-1,0);
 im->Canvas->LineTo(0,0);
}

int __fastcall TplStandard::Init_Grids()
{
 int i,j;

 Clean_Grid(AdvStringGrid1,2);
 Clean_Grid(AdvStringGrid2,2);
 Clean_Grid(AdvStringGrid3,2);
 Clean_Grid(AdvStringGrid4,2);
 Clean_Grid(AdvStringGrid5,2);
 Clean_Grid(AdvStringGrid6,2);
 Clean_Grid(AdvStringGrid7,2);
 Clean_Grid(asAvanc,2);
 Clean_Grid(avc,2);
 Clean_Grid(ave,2);
 // clearer les diagrammes;

 TSL->Clear();

 Clear_Diagram(immGantt);
 Clear_Diagram(imGantt);
 Clear_Diagram(immGnt);
 Clear_Diagram(imAvanc);

 Clear_Diagram(imPort);
 Clear_Diagram(imLand);

 Clear_Diagram(imR1p);
 //Clear_Diagram(imR1l);
 Clear_Diagram(imR1Screen);

 //Clear_Diagram(imR2p);
 //Clear_Diagram(imR1l);
 //Clear_Diagram(imR2Screen);

 Clear_Diagram(imP0p);
 Clear_Diagram(imGantt);

 Clear_Diagram(imP1p);
 Clear_Diagram(imP1l);
 Clear_Diagram(imP1Screen);

 Clear_Diagram(imP2p);
 Clear_Diagram(imP2l);
 Clear_Diagram(imP2Screen);

 Clear_Diagram(imP3p);
 // Clear_Diagram(imP3l);
 Clear_Diagram(imP3Screen);

 Clear_Diagram(imAvCal);
 Clear_Diagram(imAvGantt);

 Clear_Diagram(imAvanc);

return 0;
}


void __fastcall TplStandard::Load_Ecochamps()
{
 char tmp[250];  FILE *fp;
 char buff[500];

 strcpy(tmp,m_directory); strcat(tmp,"\\ecochamp.txt");
 fp=fopen(tmp,"rt");
 fgets(buff,250,fp); CNT_CHAMPS=-1;
 while (!feof(fp))
  {
   if (strstr(buff,"//"))
     {
     }
   else
     {
      if (strlen(buff)> 5)
      {CNT_CHAMPS++;
       if (CNT_CHAMPS >= MXC) break;
       Ghost->ExtractValue(tmp,buff,"L",0);
       strcpy(libchamp[CNT_CHAMPS],tmp);
       Ghost->ExtractValue(tmp,buff,"C",0);
       strcpy(cnt[CNT_CHAMPS],tmp);
       Ghost->ExtractValue(tmp,buff,"A",0);
       strcpy(abb[CNT_CHAMPS],tmp);
       Ghost->ExtractValue(tmp,buff,"N",0);
       strcpy(cln[CNT_CHAMPS],tmp);
      }
    }
   fgets(buff,250,fp);
  }
 fclose(fp);

}

void __fastcall TplStandard::Load_Previsionnel()
{
 int ix,cnt,j;  AnsiString S;
 // charge previsionnel d'apres le filtre en cours

 Clean_Grid(AdvStringGrid1,2);
 cnt=1;
 for (ix=1;ix<=LASTTASK;ix++)  //nb_taches;ix++)
 {
   if (av->Cells[2][ix] != "")
   {
   //if (filtr[ix]=='X')
   //{
     if(cnt>1) { AdvStringGrid1->RowCount++; }
     //  for (j=1;j<20;j++) AdvStringGrid1->Cells[j][cnt]= av->Cells[j][ix];
     for (j=1;j<15;j++) AdvStringGrid1->Cells[j][cnt]= av->Cells[j][ix];
     S =  av->Cells[42][ix];
  AdvStringGrid1->Cells[14][cnt]= av->Cells[19][ix];
     AdvStringGrid1->Cells[18][cnt]= asCrit->Cells[3][ix];
     AdvStringGrid1->Cells[19][cnt]= asCrit->Cells[5][ix];
    AdvStringGrid1->Cells[20][cnt]= asCrit->Cells[7][ix];
    AdvStringGrid1->Cells[21][cnt]= asCrit->Cells[9][ix];

     AdvStringGrid1->Colors[1][cnt] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[4][cnt] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[5][cnt] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[6][cnt] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[7][cnt] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[10][cnt] = TColor(RGB(255,255,153));
     AdvStringGrid1->Refresh();
     cnt++;
    //} // if filter
   }  // if not effac�
 } // boucle for
refresh_marges();
}




void __fastcall TplStandard::FormCreate(TObject *Sender)
{

   char tmp[MAX_PATH];    int i;
    int h,w;
  // c'est ici qu'on initialise  (pas dans create)
  AnsiString FNAME,DIRNAME;

  //  CLEF=0;   /////////////////////////////////////////
  LOADED = false;

  Timer1->Enabled=false;
  Timer2->Enabled=false;
  flag_real=false;
  flag_init=false;
  TPrinter *pt = Printer();

  /*
  PAGE_HEIGHT = pt->PageHeight;
  PAGE_WIDTH  = pt->PageWidth;
  */

  Display_Bcl=true;

  MEMORYSTATUS memory;
  int m;
  memory.dwLength =sizeof(memory);
  GlobalMemoryStatus(&memory);
  m= memory.dwTotalPhys;
  // m = memory.dwAvailPhys;
  // m= memory.dwTotalVirtual;

  MEMORY = AnsiString(m);


  if (m>1222333444)
    {
     PAGE_HEIGHT = 13742;
     PAGE_WIDTH  = 9600;
    }
  else
    {
     PAGE_HEIGHT = 6871;
     PAGE_WIDTH  = 4800;

    }


  CoefW = (float)PAGE_WIDTH/4800.0;
  CoefH = (float)PAGE_HEIGHT/6871.0;

  im = new Graphics::TBitmap();
  im->PixelFormat =pf8bit;
  im->Height= PAGE_HEIGHT;  //pt->PageHeight; //4125; //4000;
  im->Width=  PAGE_WIDTH;  // pt->PageWidth; //2892; //4000;


  Application->MessageBox("Version de Demonstration","Ecoplanning 5",MB_OK);

  //  if (CLEF) InitKey();

  timercnt=0;
  TimeCounter=0;
  m_assign[0]=0;

   /*
    i = DecCounter();
  if (i != 0x7fff) Close();
  */

  slrupt = new TStringList;
  tchcrit = new TStringList;

  IXDEB=0;
  IXFIN=100;


  w = Screen->Width;
  h=  Screen->Height;
  /*
  if (w>1024)
   {
    EcoResize(w,h);

   }
  */

  imP = new Graphics::TBitmap();
  imP->PixelFormat =pf8bit;
  imP->Height= PAGE_HEIGHT;
  imP->Width=  PAGE_WIDTH;

  Load_Fields();
  Init_Volet(4,AdvStringGrid4);
  Init_Volet(6,AdvStringGrid5);
  Init_Volet(5,asAvanc);
  Init_Volet(1,AdvStringGrid1);
  Init_Volet(2,AdvStringGrid2);
  Init_Volet(3,AdvStringGrid3);

  AdvStringGrid4->Cells[12][0]="Rrcs Prv";
  AdvStringGrid7->Cells[12][0]="Rrcs Prv";
  AdvStringGrid4->Cells[13][0]="Rrcs Prv";
  AdvStringGrid7->Cells[13][0]="Rrcs R�el";
  asAvanc->Cells[25][0]="CTache";

  AdvStringGrid1->ColWidths[9]=0;
  AdvStringGrid1->ColWidths[15]=0;
  AdvStringGrid1->ColWidths[16]=0;
  AdvStringGrid1->ColWidths[17]=0;
  AdvStringGrid1->ColWidths[23]=90;
  AdvStringGrid1->ColWidths[24]=90;
  AdvStringGrid1->ColWidths[25]=120;

  AdvStringGrid1->HideColumn(22);
  avc->HideColumn(12);
  avc->HideColumn(13);
  avc->HideColumn(6);
  avc->HideColumn(7);
  AdvStringGrid1->Cells[23][0]="Res Total";
  AdvStringGrid1->Cells[24][0]="Res/Jour";
  AdvStringGrid1->Cells[25][0]="Champs Libre";

  Load_Ecochamps();

  strcpy(TYPL[0],"FD");
  strcpy(TYPL[1],"FF");
  strcpy(TYPL[2],"AM");
  strcpy(TYPL[3],"DD");
 strcpy(tmp,m_directory);
 strcat(tmp,"\\coche.jpg"); // critical.bmp");
 strcpy(strcoche,"<IMG src=\"file://"); strcat(strcoche,tmp); strcat(strcoche,"\">");
 //GetCurrentDirectory(MAX_PATH,tmp);
 strcpy(tmp,m_directory);
 strcat(tmp,"\\danger.gif");
 strcpy(strdanger,"<IMG src=\"file://"); strcat(strdanger,tmp); strcat(strdanger,"\">");

 PageControl1->ActivePageIndex=0;
 TDateTime dtPresent = Now();
 DecodeDate(dtPresent, Year1, Month1, Day1);
 DecodeDate(dtPresent, Year2, Month2, Day2);
 //MonthCalendar1->Date = dtPresent;
 AnPrev=2009; MoisPrev=1; JourPrev=1;


 strcpy(LocalBase,DatabaseName);
 // Extract Project Directory
 DIRNAME = ExtractFileDir(AnsiString(LocalBase));
 strcpy(ProjectDirectory,DIRNAME.c_str());



 Init_Grids();
 //  Timer1->Enabled = true;
 // pas de Reload si Nouveau Projet avec Import !!!!
 //if (strlen(m_importtaches)==0)
 av->RowCount=2;
 ReloadAllData(LocalBase,true,false);   // false pour date modified

 asTri->ColWidths[65] = 40;
 asTri->ColWidths[66] = 40;




 if (VERSION == 0)
  {
   TabSheet2->TabVisible = true;
   TabSheet3->TabVisible = false;
   TabSheet11->TabVisible = true;
   TabSheet7->TabVisible = false;

   TabSheet9->TabVisible = true;
   TabSheet12->TabVisible = false;
   //TabSheet7->TabVisible = true;          // $$$$$$$$$$$$$$$$$$$$$
   TabSheet14->TabVisible = true;
   TabSheet6->TabVisible = false;

  }
 else if (VERSION == 1)
  {
   TabSheet2->TabVisible = true;
   TabSheet3->TabVisible = false;
   TabSheet11->TabVisible = true;
   TabSheet5->TabVisible = false;
   TabSheet9->TabVisible = true;
   TabSheet12->TabVisible = false;
   TabSheet14->TabVisible = true;
   TabSheet7->TabVisible = false; //true;
   // $$$$$$$$$$$ TabSheet7->TabVisible = true;
  }

 Timer1->Enabled = true;
 // model =1;


 AdvStringGrid2->Cells[4][0] = "Possible D�but Pr�visionnel";
AdvStringGrid3->HideColumn(8);



 }
//---------------------------------------------------------------------------


bool __fastcall TplStandard::CorrectAvanc(char *parm)
{
 int distp,distprev;  char JPrev[20],MPrev[20],APrev[20]; char *p;
 char tmp[200];  int jprev,mprev,aprev;

  sprintf(tmp,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
  p=tmp; strcpy(JPrev,p); JPrev[2]=0;  jprev=atoi(JPrev);
  p=p+3; strcpy(MPrev,p); MPrev[2]=0;  mprev=atoi(MPrev);
  p=p+3; strcpy(APrev,p); APrev[2]=0;  aprev=atoi(APrev);
  distprev = aprev*365 + mprev*31 + jprev;

  strcpy(tmp,parm);
  p=tmp; strcpy(APrev,p); APrev[2]=0;  aprev=atoi(APrev);
  p=p+3; strcpy(MPrev,p); MPrev[2]=0;  mprev=atoi(MPrev);
  p=p+3; strcpy(JPrev,p); JPrev[2]=0;  jprev=atoi(JPrev);
  distp = (aprev%100)*365 + mprev*31 + jprev;
  if (distp<distprev) return false;


  return true;
}

void __fastcall TplStandard::ReloadAllData(char *name,bool xopen,bool datemodif)
{
 char tmp[15000];   //����
   int rc,ix,aa;   int xx;
   char RetKey[200];
   char Key[200];     char dex[40];
   int  recdata;      char LesJ[50];
   int i,j,ok,l,xval,m;
   int ipred,isucc;
   char delim[50];
   char tmp1[50]; char fmt[200];
   int lg,ik;
   char *p;    int bcl;
   int x,a,zz,n,cnt;  int AA,MM,JJ;
   char A[10],M[10],J[10],N[10];
   char txt[250],fxname[255];
   int TACH;
   char buffer[10000];
   TCursor oldCursor;

   oldCursor = Screen->Cursor;
   Screen->Cursor = crHourGlass;
   for(i=0;i<MAX_T;i++) sommets[i]=0;

 SAISIE_AVANC = true;
 //init_calendrier();
 if (VERSION == true && xopen)
   {
     avanc=new realisam();
     oldavanc=new realisam();
     proj = new realisam();
     rc = proj->OpenEngine(LocalBase,120,"eco","dat");   //512 - 8
     if (rc==0) { ShowMessage("Erreur � l'ouverture des fichiers projets");
              Screen->Cursor = oldCursor;exit (1); }
     rc = proj->SetIndexMode(0,0);

    }
 if (!datemodif)
   {
   rc = proj->ReadDirectKey(0,"<dateprevdebut>",RetKey,&recdata);
   if (rc)  // ok : cl� trouv�e
    {

     rc = proj->ReadRecord(buffer,recdata);
     buffer[rc]=0;
     p=buffer;
     strncpy(J,p,2); J[2]=0;   JJ=atoi(J);
     strncpy(M,p+3,2); M[2]=0;  MM=atoi(M);
     strncpy(A,p+6,4); A[4]=0;  AA=atoi(A);

     if (strlen(buffer)<10 || JJ==0 || MM==0 || AA==0)
       {strcpy(buffer,"01/04/2009");
        Application->MessageBoxA("Date de d�but pr�visionnel absente: Veuillez pr�ciser la date correcte (Date forc�e au 01/04/2009)",
         m_ecoplan,MB_OK);

       }
     strcpy(dateprevdeb,buffer);
     // Label7->Caption = AnsiString(dateprevdeb);
     // Label109->Caption = AnsiString(dateprevdeb);

     buffer[2]=0; JourPrev=atoi(buffer);
     buffer[5]=0; MoisPrev=atoi(buffer+3);
     buffer[10]=0; AnPrev=atoi(buffer+6);
     if (AnPrev <100) AnPrev = AnPrev+2000;
     AnPrev2=AnPrev%100;
     sprintf(tmp,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev2);
     Label7->Caption=AnsiString(tmp);
     Label109->Caption = AnsiString(dateprevdeb);
     Label134->Caption = AnsiString(dateprevdeb);

     sprintf(dateprevdeb,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev);
     for (xx=0;xx<8;xx++)
       {
        strcpy(edDEB[xx],Label7->Caption.c_str());
       }
     init_calendrier();
     }
   }
  else
   {
    strcpy(buffer,dateprevdeb);
    buffer[2]=0; JourPrev=atoi(buffer);
    buffer[5]=0; MoisPrev=atoi(buffer+3);
    buffer[10]=0; AnPrev=atoi(buffer+6);
    if (AnPrev <100) AnPrev = AnPrev+2000;
    AnPrev2=AnPrev%100;
    sprintf(tmp,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev2);
    Label7->Caption=AnsiString(tmp);
    Label109->Caption = AnsiString(tmp);
    Label134->Caption = AnsiString(tmp);Label109->Caption = AnsiString(tmp);
    for (xx=0;xx<8;xx++)
    strcpy(edDEB[xx],Label7->Caption.c_str());
   }   

 strcpy(m_status,"<create>");
 strcat(m_status,DatabaseName);
 strcat(m_status,"</create>");

 rc = proj->ReadDirectKey(0,"<dbtitre>",RetKey,&recdata);
 if (rc)  // ok : cl� trouv�e
  {
   rc = proj->ReadRecord(buffer,recdata);
   buffer[rc]=0;
   strcpy(dbtitre,buffer);
   strcpy(TITRE,buffer);
   strcpy(tmp,"Project "); strcat(tmp,LocalBase);
   strcat(tmp," "); strcat(tmp,buffer);
  }

rc = proj->ReadDirectKey(0,"<dbobserv>",RetKey,&recdata);
 if (rc)  // ok : cl� trouv�e
  {
   rc = proj->ReadRecord(buffer,recdata);
   buffer[rc]=0;
   strcpy(OBSERV,buffer);
  }


  // nettoyage buffer avvancement
dateavanc[0]=0;

rc = proj->ReadDirectKey(0,"<brisees>",RetKey,&recdata);
if (rc)
 {
  rc = proj->ReadRecord(buffer,recdata);
  for (i=1;i<5;i++)
    {
     sprintf(fmt,"PC%02d",i);
     lg = Ghost->ExtractValue(tmp,buffer,fmt,0);
     for (j=1;j<20;j++)
       {
        sprintf(fmt,"J%02d",j);
        Ghost->ExtractValue(LesJ,tmp,fmt,0);
        Ghost->ExtractValue(fmt,LesJ,"c",0);
        C_BRISEES[i][j]=atoi(fmt);
        Ghost->ExtractValue(fmt,LesJ,"e",0);
        E_BRISEES[i][j] = atoxx(fmt);
        Ghost->ExtractValue(fmt,LesJ,"t",0);
        if (strcmp(fmt,"Oui")==0) L_BRISEES[i][j]=true;
        else L_BRISEES[i][j]=false;
       }
    }
 }

strcpy(dateavanc,"");
FillListBoxA();


//rc = proj->ReadDirectKey(0,"<avanc>",RetKey,&recdata);
// if (rc)  // ok : cl� trouv�e
//  {
//   rc = proj->ReadRecord(buffer,recdata);
//   buffer[rc]=0;
   strcpy(buffer,dateavanc);
   if( strlen(buffer) != 0)
    {
   // verifie si les fichiers existent
    cnt=Explode('@',buffer);
    CNTACTUAL =cnt;
    CntAV=cnt;
    CntAVBefore = cnt;
    strcpy(DATEAVANC,"");
    strcpy(dateavanc,"");
    for (aa=0;aa<CntAV;aa++)

     {  params[aa][2]='-'; params[aa][5]='-';
      strcpy(TableDatesAV[aa],params[aa]);
      strcat(dateavanc,params[aa]);
      strcat(dateavanc,"@");
     }
   //  Application->MessageBox(dateavanc,"dateavanc",MB_OK);
    for (i=0;i<cnt;i++)
      {
       strcpy(tmp,params[i]);
       // verifier si dates correctes
       if (!CorrectAvanc(params[i])) l=0;
        else l=strlen(tmp);
        if (l>=8)
         { strncpy(N,tmp+9,3); N[3]=0; n= atoi(N);
           strncpy(A+2,tmp,2);  A[4]=0;  A[0]= '2'; A[1]= '0'; a=atoi(A) % 100;
           strncpy(M,tmp+3,2);  M[2]=0;  m=atoi(M);
           strncpy(J,tmp+6,2);  J[2]=0;   j=atoi(J);
           sprintf(txt,"-%02d-%02d-%02d",a,m,j);
           strcpy(fxname,LocalBase); strcat(fxname,txt); strcat(fxname,".act");
           if (FileExists(fxname))
            {
              //strcat(dateavanc,params[i]);
              //strcat(dateavanc,"@");
              if (i==cnt-1)
               {
                p=strstr(fxname,".act");
                if (p)
                  {
                    *p = 0;
                       strcpy(FileNameAvanc,fxname);
                       Extraire_Date(fxname);
                       sprintf(DATEAVANC,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);
                       sprintf(dex,"%02d/%02d/%04d",JourAvanc,MoisAvanc,AnAvanc+2000);

                       concern=true;  // $$$$$

                       strcpy(m_special,"<da>"); strcat(m_special,dex);
                       strcat(m_special,"</da><tc>false</tc><new>false</new><file>");
                       strcat(m_special,txt); strcat(m_special,"</file>");
                       AVANC_DEL=false;
                       concern=true; //cbAvanc->Checked = false;
                       Exec_Avanc(true);
                  }
               }
            }
            // else dateavanc[0]=0;
         }
      }
     }
  // } // if rc
 // else
 // Application->MessageBox("dateavanc non trouv�e","Ecopl",MB_OK);



   LASTTASK=0;
   rc = proj->ReadFirstKey(0,RetKey,&recdata);
   while (rc)
    {  buffer[0]=0;
       rc = proj->ReadRecord(buffer,recdata);  // read record in buffer
       buffer[rc]=0;
       lg = Ghost->ExtractValue(tmp,buffer,"T",0);
       TACH = atoi(tmp);
       if (TACH>0)
         {
           if (TACH > LASTTASK) LASTTASK=TACH;
         }

       rc = proj->ReadNextKey(RetKey,&recdata);
    }

if (LASTTASK >1)
  {
   av->RowCount = LASTTASK+1;
   AdvStringGrid1->RowCount = LASTTASK+1;
  }
 else
  {
   //Application->MessageBoxA("Base de donn�es du projet corrompue\nou pr�sence d'une seule t�che",
   //                         m_ecoplan,MB_OK);
   // FormDestroy(NULL);
   av->RowCount = LASTTASK+1;
   StatusBar1->SimpleText = "Base de donn�es Vide";
   AdvStringGrid1->RowCount = LASTTASK+1;

  }

 //  lecture du nombre de taches
 nb_taches=0; NB_TACHES=0;
 rc = proj->ReadDirectKey(0,"<taches>",RetKey,&recdata);
 if (rc)  // ok : cl� trouv�e
  {
   rc = proj->ReadRecord(buffer,recdata);
   buffer[rc]=0;
   nb_taches = atoi (buffer);
  }


// NB_TACHES = nb_taches;
   for (i=0;i<=LASTTASK;i++)
  // for (i=0;i<=(nb_taches+10);i++)     //  &&&&&&&&&&&&
     {
      duree[i]=0; duree_reelle[i]=0; deb_plus_tot[i]=0; fin_plus_tot[i]=0;
      deb_plus_tard[i]=0; fin_plus_tard[i]=0; sommets[i]=0; coche[i]= ' '; coch[i]=' ';
      filtr[i]= ' '; tcal[i]=1; tmom[i][0]=0; da[i][0]=0; dtr[i][0]=0; ftr[i][0]=0;
      pca[i]=0; nbj[i]=0; codetache[i][0]=0; dds[i][0]=0; dfs[i][0]=0;
      //libel[i][0]=0;
      retard[i]=0;

      for (j=0;j<PS;j++)
        {
         pr[i][j]=0; su[i][j]=0;
         pr_tl[i][j]=0; su_tl[i][j]=0;
         pr_decal[i][j]=0; su_decal[i][j]=0;
        }
     }


 for (ix=0;ix<MAX_T;ix++) filtr[ix]='X';
 // Boucle de lecture des taches // chargement des tableaux
 // syntaxe pour les taxes : Cl� = <T%04d>
 // <T>%d</T><nom>Tache %d</nom><duree>1</duree>",i,i);

  // faire une lecture sequentielle des cles et charger sommets
  // on remplit sommets

  rc = proj->ReadFirstKey(0,RetKey,&recdata);
  ix=0;

  while (rc)
    {  buffer[0]=0;
       rc = proj->ReadRecord(buffer,recdata);  // read record in buffer
       buffer[rc]=0;
       lg = Ghost->ExtractValue(tmp,buffer,"T",0);
       TACH = atoi(tmp);
       //stt->t_numtache = AnsiString(tmp);
       if (lg)
        {
          Ghost->ExtractValue(tmp,buffer,"duree",0);
          xval=atoi(tmp);
          if (TACH!=0) NB_TACHES++;
          ix++;

          sommets[ix]= TACH;

          filtr[TACH]='X';

          av->Cells[1][TACH]= AnsiString(TACH);

          Ghost->ExtractValue(tmp,buffer,"nom",0);

          av->Cells[2][TACH]= AnsiString(tmp);
          Ghost->ExtractValue(tmp,buffer,"duree",0);
          if (atoi(tmp)==0) strcpy(tmp,"1");
          duree[TACH]=atoi(tmp);
          Ghost->ExtractValue(tmp,buffer,"cal",0);
          if (strlen(tmp)) tcal[TACH]=atoi(tmp);
            else tcal[TACH]=1;
          Ghost->ExtractValue(tmp,buffer,"mom",0);
          if (strlen(tmp)) strcpy(tmom[ix],tmp);
            else tmom[TACH][0]=0;
          Ghost->ExtractValue(tmp,buffer,"da",0);
          if (strlen(tmp)) strcpy(da[TACH],tmp); else da[TACH][0]=0;
          Ghost->ExtractValue(tmp,buffer,"dtr",0);
          if (strlen(tmp)) strcpy(dtr[TACH],tmp); else dtr[TACH][0]=0;
          Ghost->ExtractValue(tmp,buffer,"ftr",0);
          if (strlen(tmp)) strcpy(ftr[TACH],tmp); else ftr[TACH][0]=0;
          Ghost->ExtractValue(tmp,buffer,"pc",0);
          if (strlen(tmp))
             {pca[TACH]=atoi(tmp);
              av->Cells[31][TACH]=AnsiString(tmp);
             }
             else {pca[TACH]=-1; av->Cells[31][TACH]="-1"; } // pourcentage
          Ghost->ExtractValue(tmp,buffer,"nbj",0);
          if (strlen(tmp))
            { nbj[TACH]=atoi(tmp); av->Cells[32][TACH]=AnsiString(tmp); }
           else { nbj[TACH]=-1; av->Cells[32][TACH]="-1"; }

          Ghost->ExtractValue(tmp,buffer,"dds",0);
          if (strlen(tmp)) av->Cells[12][TACH]=AnsiString(tmp);
          Ghost->ExtractValue(tmp,buffer,"dfs",0);
          if (strlen(tmp)) av->Cells[13][TACH]=AnsiString(tmp);
          AdvStringGrid1->Colors[1][ix] = TColor(RGB(255,255,153));
          AdvStringGrid1->Colors[4][ix] = TColor(RGB(255,255,153));
          AdvStringGrid1->Colors[5][ix] = TColor(RGB(255,255,153));
          AdvStringGrid1->Colors[6][ix] = TColor(RGB(255,255,153));
          AdvStringGrid1->Colors[7][ix] = TColor(RGB(255,255,153));
          AdvStringGrid1->Colors[10][ix] = TColor(RGB(255,255,153));

        }
       rc = proj->ReadNextKey(RetKey,&recdata);
    }

  if (ix != nb_taches)
     {
      // nb_taches = ix;
      if (Application->MessageBox("Anomalie sur Nombre de T�ches: Base de donn�es invalide, Voulez-vous continuer?","",MB_YESNO)==IDNO) Close();
      nb_taches=ix;
      // Close();
     }

     //  Application->MessageBox("Anomalie sur NB_Taches","",MB_OK);

  for (ix=1;ix<=LASTTASK;ix++)  // nb_taches;ix++)       // &&&&&&&&&&&&
    {
     // traitement des predecesseurs,
     // structure du record
     // Cle = P000n    Record <PR%04d> puis ... <P%02d><num>.. </num>
                                       //       <typ>..</typ>
                                       //       <decal>..</decal></P%02d>
     //                  puis  </PR%04d>

        ik=ix; //  sommets[ix];
        sprintf(Key,"<PR%04d>",ik);   //  build  key for predecesseurs
        rc = proj->ReadDirectKey(0,Key,RetKey,&recdata);
        if (rc)  // ok : cl� trouv�e
         { buffer[0]=0;
          rc = proj->ReadRecord(buffer,recdata);
          buffer[rc]=0;
          if (rc)   //  ok, record predecesseur lu
           {
            ipred=1; ok=true;
            while (ok)
              {
               sprintf(delim,"P%02d",ipred);
               l = Ghost->ExtractValue(tmp,buffer,delim,0);
               if (l == 0)ok=false; //
                else
                  {
                   l= Ghost->ExtractValue(tmp1,tmp,"num",0); xval=atoi(tmp1);
                   pr[ik][ipred]= xval;
                   l= Ghost->ExtractValue(tmp1,tmp,"typ",0); xval=atoi(tmp1);
                   pr_tl[ik][ipred]= xval;
                   l= Ghost->ExtractValue(tmp1,tmp,"decal",0); xval=atoi(tmp1);
                   pr_decal[ik][ipred] = xval;
                   ipred++;
                  }

               if (ipred >= PS) ok=false;

              }

           }
         }
   // traitement des successeurs
      // structure du record
     // Cle = S000n    Record <SU%04d> puis ... <SP%02d><num>.. </num>
                                       //       <typ>..</typ>
                                       //       <decal>..</decal></S%02d>
     //                  puis  </SU%04d>

        sprintf(Key,"<SU%04d>",ik);   // &&&&&&&& ok build  key for successeurs
        rc = proj->ReadDirectKey(0,Key,RetKey,&recdata);
        if (rc)  // ok : cl� trouv�e
         { buffer[0]=0;
          rc = proj->ReadRecord(buffer,recdata);
          buffer[rc]=0;
          if (rc)   //  ok, record successeur lu
           {

            isucc=1; ok=true;

            for (bcl=1;bcl< PS;bcl++)
              {

               sprintf(delim,"S%02d",bcl);  // succ);
               l = Ghost->ExtractValue(tmp,buffer,delim,0);
               if (l == 0) break; //  ok=false;  // { su[ix][isucc]=9999; ok=false; }
                else
                  {

                   l= Ghost->ExtractValue(tmp1,tmp,"num",0); xval=atoi(tmp1);
                   su[ik][isucc]= xval;
                   if (xval !=0 && xval !=9999)
                    {
                     l= Ghost->ExtractValue(tmp1,tmp,"typ",0); xval=atoi(tmp1);
                     su_tl[ik][isucc]= xval;
                     l= Ghost->ExtractValue(tmp1,tmp,"decal",0); xval=atoi(tmp1);
                     su_decal[ik][isucc] = xval;
                     isucc++;
                    }
                  }
               if (isucc >= PS) break;  // ok=false;
             } // BCL// while OK
            // Rajouter 9999 en dernier
            su[ix][isucc] = 9999;
           } // if rc
         }
    }  //  boucle for nb of tasks

//delete stt;

Load_Grille_Criteres();
Load_Reference();
Load_Previsionnel();
LoadConfig();
Remplir_asTri();
// init_calendrier();

if (VERSION == true)
 {
  rc = proj->ReadDirectKey(0,"<oldavanc>",RetKey,&recdata);
 if (rc)  // ok : cl� trouv�e
  {

   rc = proj->ReadRecord(buffer,recdata);
   buffer[rc]=0;
   strcpy(FileNameAvanc,buffer);
   strcpy(fxname,buffer); strcat(fxname,".act");
   if (FileExists(fxname))
     {
       Extraire_Date(FileNameAvanc);
       flag_loadavanc=true;
       flag_real=false;
       Exec_Avanc(true);
     }
   else
     {
      flag_loadavanc = false;
      Clean_Grid(asAvanc,2);
      Clean_Grid(AdvStringGrid6,2);
      flag_real=true;
     // Create_First_Real();
     }
  }
   // extraite la date d'actualisation

  }

  pc=0;  //������
  //Read_Criteres();
  //Load_Grille_Criteres();  // read_criteres effectue dans load_grille
  CopyCriteres();
  // strcpy(filter,"<T>");
  for(i=1;i<=LASTTASK;i++)  filtr[i]='X';
  for (i=1;i<8;i++) { MAX_PAGE_RAPPORT[i]=1; PAGE_RAPPORT[i]=1; }
  flag_init=true;
  Timer1->Enabled = true;
  grefresh(current_task,'1');
  OldRow=0; OldCol=0;
  for(i=15;i<=40;i++)   asTri->ColWidths[i]=0;
  for(i=45;i<70;i++)   asTri->ColWidths[i]=0;
Load_Couts();
Screen->Cursor = oldCursor;
LOADED = true;
}

float __fastcall TplStandard::atoxx(char *tout)
{

 int l,i,lg; char *endptr;
 char *p; float f;  char extra[50];

 l=strlen(tout);
 lg=0;
 for (i=0;i<l;i++)
   {
    if (tout[i] == ',')  tout[i]='.';
    if (tout[i] != ' ') { extra[lg]=tout[i]; lg++; }
   }
 extra[lg]=0; // strcat(extra," ");
 f=(float) atof(extra);
 //f = strtod(extra,NULL);

 return (f);
}

void __fastcall TplStandard::Load_Reference()
{
int ix; char tmp[2500];
int rc,recdata; char Key[200],RetKey[200];
int delta,iz,iy, ixx,iint;

// ix=nb_taches;

//Clean_Grid(av,2);

//  for (ix=1;ix<=nb_taches;ix++)
for (ix=1;ix<=LASTTASK;ix++)

 {
    //  sprintf(Key,"<T%04d>",sommets[ix]);

    sprintf(Key,"<T%04d>",ix);     // &&&&&
    // ixx = sommets[ix];
    ixx = ix; // &&&&&
    rc=proj->ReadDirectKey(0,Key,RetKey,&recdata);
    if (rc)
      { buffer[0]=0;
         rc = proj->ReadRecord(buffer,recdata);
         buffer[rc]=0;
         Ghost->ExtractValue(tmp,buffer,"T",0);
         //if(ix>1) { av->RowCount++; }
         av->Cells[1][ixx]=atoi(tmp);
         Ghost->ExtractValue(tmp,buffer,"nom",0);
         if (strlen(tmp)==0) sprintf(tmp,"Tache N� %d",ixx);
         av->Cells[2][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"duree",0);
         iint = atoi(tmp); if (iint ==0) strcpy(tmp,"1");
         av->Cells[3][ixx]=tmp;
         rc=atoi(tmp);
         // if (rc==0) av->Cells[0][ixx]="D";   // deleted
         Ghost->ExtractValue(tmp,buffer,"ddr",0);
         av->Cells[4][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"dfr",0);
         av->Cells[5][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"cal",0);
         av->Cells[10][ixx]=tcal[ix];
         Ghost->ExtractValue(tmp,buffer,"dds",0);
         av->Cells[12][ixx]=AnsiString(tmp);
         Ghost->ExtractValue(tmp,buffer,"dfs",0);
         av->Cells[13][ixx]=AnsiString(tmp);

         Ghost->ExtractValue(tmp,buffer,"nbpred",0);
         av->Cells[61][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"pred",0);
         av->Cells[62][ixx]=tmp;                     // OK
         Ghost->ExtractValue(tmp,buffer,"nbsucc",0);
         av->Cells[63][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"succ",0);
         av->Cells[64][ixx]=tmp;                     //  $$$$OK


         Ghost->ExtractValue(tmp,buffer,"chl1",0);
         if (strcmp(tmp,"(null)")==0) strcpy(tmp,"");
         av->Cells[15][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"chl2",0);
         if (strcmp(tmp,"(null)")==0) strcpy(tmp,"");
         av->Cells[16][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"chl3",0);
         if (strcmp(tmp,"(null)")==0) strcpy(tmp,"");
         av->Cells[17][ixx]=tmp;

         Ghost->ExtractValue(tmp,buffer,"cdtch",0);
         if (strcmp(tmp,"(null)")==0) strcpy(tmp,"");
         av->Cells[11][ixx]=tmp;

         Ghost->ExtractValue(tmp,buffer,"coutp",0);
         if (strcmp(tmp,"(null)")==0) strcpy(tmp,"");
         av->Cells[18][ixx]=tmp;
         Ghost->ExtractValue(tmp,buffer,"coutn",0);
         if (strcmp(tmp,"(null)")==0) strcpy(tmp,"");
         av->Cells[19][ixx]=tmp;

        }
//    } // if filter
 } // boucle for

}

void __fastcall TplStandard::Create_First_Real()
{
  char isamname[250],tmp[250];
  int x,lg,ix;
  int rc,recdata; char Key[200],RetKey[200];

  x=1;
  AnAvanc=AnPrev; MoisAvanc=MoisPrev; JourAvanc=JourPrev;
  sprintf(tmp,"%s-%02d-%02d-%02d",LocalBase,AnPrev%100,MoisPrev,JourPrev);
  strcpy(FileNameAvanc,tmp);
  strcpy(isamname,tmp); strcat(isamname,".act"); unlink(isamname);
  strcpy(isamname,tmp); strcat(isamname,".don"); unlink(isamname);

   rc = proj->ReadFirstKey(0,RetKey,&recdata);
  ix=0;
  while (rc)
    {  buffer[0]=0;
       rc = proj->ReadRecord(buffer,recdata);  // read record in buffer
       buffer[rc]=0;
       lg = Ghost->ExtractValue(tmp,buffer,"T",0);
       if (lg)
        { ix++;
          Ghost->ExtractValue(tmp,buffer,"dtr",0);
          if (!strlen(tmp)) { strcpy(adv,av->Cells[4][ix].c_str());
           // getvalue(1,ix,4);
          strcpy(tmp,adv); }
          av->Cells[21][ix]=tmp;
          Ghost->ExtractValue(tmp,buffer,"ftr",0);
          if (!strlen(tmp)) { strcpy(adv,av->Cells[5][ix].c_str());
           // getvalue(1,ix,5);
             strcpy(tmp,adv); }
          av->Cells[22][ix]=tmp;
          Ghost->ExtractValue(tmp,buffer,"duree",0);
          if (!strlen(tmp)) { strcpy(adv,av->Cells[3][ix].c_str());
            // getvalue(1,ix,3);
           strcpy(tmp,adv); }
          av->Cells[23][ix]=tmp;
        }

     rc = proj->ReadNextKey(RetKey,&recdata);
    }
//$$$$  Change1();
}

void __fastcall TplStandard::InitTri()
{
 P_MODE[1]='P';
 P_MODE[4]='P';
 RAZ();

 LoadConfig();

 pc=0;   // �������
}

////////////////////////////////////////
void __fastcall TplStandard::LoadConfig()
{
 char tmp[MAX_PATH];
 FILE *fp;

 strcpy(tmp,LocalBase); strcat(tmp,"_config.txt");

 fp=fopen(tmp,"rt");
 if (fp)
   {
    buffer[0]=0;
    fgets(buffer,512,fp);
    Ghost->ExtractValue(P_NOM[0],buffer,"0",0);
    Ghost->ExtractValue(P_NOM[1],buffer,"1",0);
    Ghost->ExtractValue(P_NOM[2],buffer,"2",0);
    Ghost->ExtractValue(P_NOM[3],buffer,"3",0);
    Ghost->ExtractValue(P_NOM[4],buffer,"4",0);
    Ghost->ExtractValue(P_NOM[5],buffer,"5",0);
    Ghost->ExtractValue(P_NOM[6],buffer,"6",0);
    fclose (fp);
   }
  else
   {
   // wwww strcpy(P_NOM[0],"Defaut");
    strcpy(P_NOM[0],"Ordonnancement des t�ches en planning compress�");
    // strcpy(P_NOM[0],"");
    strcpy(P_NOM[1],""); strcpy(P_NOM[2],"");
    strcpy(P_NOM[3],"");strcpy(P_NOM[4],"");strcpy(P_NOM[5],"");
    strcpy(P_NOM[6],"");

   }


 if (strlen(P_NOM[0])) { pc=0; Ghost->LoadPlanningFromFile(P_NOM[0]); }
 if (strlen(P_NOM[1])) { pc=1; Ghost->LoadPlanningFromFile(P_NOM[1]); }
 if (strlen(P_NOM[2])) { pc=2; Ghost->LoadPlanningFromFile(P_NOM[2]); }
 if (strlen(P_NOM[3])) { pc=3; Ghost->LoadPlanningFromFile(P_NOM[3]); }
 if (strlen(P_NOM[4])) { pc=4; Ghost->LoadPlanningFromFile(P_NOM[4]); }
 if (strlen(P_NOM[5])) { pc=5; Ghost->LoadPlanningFromFile(P_NOM[5]); }


 pc=0;  // ��������

}

void __fastcall TplStandard::SaveConfig()
{
 char tmp[MAX_PATH];
 FILE *fp;
 int i,count;
 strcpy(tmp,LocalBase);
 strcat(tmp,"_config.txt");

 fp = fopen(tmp,"wt");
 if (fp)
   {
   tmp[0]=0;
   strcat(tmp,"<0>"); strcat(tmp,P_NOM[0]); strcat(tmp,"</0>");
   strcat(tmp,"<1>"); strcat(tmp,P_NOM[1]); strcat(tmp,"</1>");
   strcat(tmp,"<2>"); strcat(tmp,P_NOM[2]); strcat(tmp,"</2>");
   strcat(tmp,"<3>"); strcat(tmp,P_NOM[3]); strcat(tmp,"</3>");
   strcat(tmp,"<4>"); strcat(tmp,P_NOM[4]); strcat(tmp,"</4>");
   strcat(tmp,"<5>"); strcat(tmp,P_NOM[5]); strcat(tmp,"</5>");
   strcat(tmp,"<6>"); strcat(tmp,P_NOM[6]); strcat(tmp,"</6>");
   strcat(tmp,"\r\n");
   fputs(tmp,fp);
  }
 fclose(fp);
}




int  __fastcall TplStandard::exist_select(int it)
{
 int retval=0;
 int ix;
 for (ix=1;ix<=LASTTASK;ix++)      // &&&&&&&&&&&&
   {
    if (t_select[ix]==it) {retval=ix; break; }
   }
 return retval;
}


void __fastcall TplStandard::RemovePredSeul(int tach,int tpred)
{

  int it, nt,indx,ipred;

  ipred=1;
  indx=tach;  //exist_tache(tach,nb_taches);  // &&&&&&&&&&
  if (indx==0) return;
     while (pr[indx][ipred] !=0)
        {
         nt=pr[indx][ipred];
         if (nt==tpred)
          {// decaler  + break;
           for (it=ipred;it<PS;it++)
             {
              pr[indx][it]=pr[indx][it+1];
              pr_tl[indx][it]=pr_tl[indx][it+1];
              pr_decal[indx][it]=pr_decal[indx][it+1];
             }
           break;
          }
         ipred++;
       }
}

void __fastcall TplStandard::RemoveSuccSeul(int tach, int tsucc)
{
 int nt,it,indx,isucc;
 int tbl[PS];
 int m;
 for (m=0;m<PS;m++)  tbl[m]=0;
 for (m=0;m<PS;m++)  tbl[m]=su[tach][m];


 isucc=1;
 indx=tach;


  if (indx==0) return;
      while ((su[indx][isucc] !=0) && isucc < PS)  // && (su[indx][isucc] != 9999))
       {
        nt=su[indx][isucc];
        if (nt != 9999)
         {
           if (nt==tsucc)
             { // decaler + break;
             for (it=isucc;it<PS-1;it++)
                {
                  su[indx][it]=su[indx][it+1];
                  su_tl[indx][it]=su_tl[indx][it+1];
                  su_decal[indx][it]=su_decal[indx][it+1];
                  su[indx][it+1]=0;
                }
           // su[indx][it+1]=0;
           break;
          }
        }
        isucc++;
       }
   for (m=0;m<PS;m++)  tbl[m]=0;
  for (m=0;m<PS;m++)  tbl[m]=su[indx][m];
  m=m;
}

int  __fastcall TplStandard::remove_task(int it)
{
 // examine all predecesseurs, and remove task (decalage);
 int ix,ip, xt, xtrouve;

 ix = it;  //exist_tache(it,nb_taches);
     // ��������������������
 if (ix==0)
   return 0;
 t_pred[ix][1]=-1;
 // t_pred[it][1] = -1;
 for (ix=1;ix<=LASTTASK;ix++)  // nb_taches
    {  ip=1; xtrouve = 0;
       while (t_pred[ix][ip] !=0)
       {
        if (t_pred[ix][ip]==it) xtrouve=ip;   // xtrouve a indice

        ip++;
       }
     // si xtrouv�, on va decaler et eliminer it , ip contient n+1 predecceseurs
     if (xtrouve)
       {
        for (xt=xtrouve;xt<=ip;xt++) t_pred[ix][xt]= t_pred[ix][xt+1];

       }
    }
 return 0;
}


int  __fastcall TplStandard::init_calendrier()
{
 TDateTime dtFirst;
 int sum, i, n_cal;
 int rc, recdata;
 char ikey[50];
 char RetKey[50];
 char buffer[255];
 AnsiString Comment;
 char tmp[550];  //����
 char test;

 //lb1->Clear();

 char days[7][15] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi",
  "Samedi" };
 // char months[12][15] = {"Janvier","F�vrier","Mars","Avril","Mai","Juin","Juillet",
 //  "Aout","Septembre","Octobre","Novembre","D�cembre"};
 // int nbdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  if (AnPrev==0) AnPrev=2009;
  if (MoisPrev==0) MoisPrev=1;
  if (JourPrev==0) JourPrev=1;

  dtFirst = EncodeDate(AnPrev,MoisPrev,JourPrev);
  //int counter = nbdays[MoisPrev-1];
  //if (MoisPrev==2){ if (IsLeapYear(AnPrev)) counter = 29;}
  // on a besoin de savoir le 1er jour du mois
  int firstday = dtFirst.DayOfWeek();
  int dow;  // day of week
  dow = firstday-1;
  Comment = "1 er Jour Date Prev Debut " + AnsiString(dow) + " = " + AnsiString(days[dow]);
  //lb1->Items->Add(Comment);

  cals = new realisam();

  strcpy(tmp,m_directory); strcat(tmp,"\\calend");
  rc = cals->OpenEngine(tmp,56,"ndx","dat");   //64 - 8
  rc = cals->SetIndexMode(0,0);
  // calcule la longueur de la duree reelle probable
  //  Sommme(duree[i] + 2) * 2
  sum=0;
  for (i=1;i<=LASTTASK;i++) sum=sum+duree[i]+2;   // �������������
  sum = sum + sum;

  // for (i=0;i<=NB_CAL;i++) cal[i] = new char[sum];


  for (n_cal=1;n_cal<=NB_CAL;n_cal++)
   {
    // lecture WE
    // la date prev de debut correspond au 1 er jour
    sprintf(ikey,"%02d-0000-00-00",n_cal);   // clef pour determiner WE
    rc = cals->ReadDirectKey(0,ikey,RetKey,&recdata);
    rc = cals->ReadRecord(buffer,recdata);
    buffer[rc]=0;
    strncpy(weeks[n_cal],buffer+4,7); strncat(weeks[n_cal],buffer+4,7);
    // pour avoir au moins 13 jours
    //lb1->Items->Add(AnsiString(weeks[n_cal]));

    for (i=0;i<MAX_JOURS;i++)
      {

       cal[n_cal][i]=weeks[n_cal][dow-1 + (i % 7)];
       test = cal[n_cal][i];
       add_jour(AnPrev,MoisPrev,JourPrev,i);
       if ((i % 7) == 0)
         {
          sprintf (tmp,"%02d-%02d-%04d",Jour,Mois,An);
          //lb1->Items->Add(AnsiString(tmp));
         }
       //verifie si il existe un record N ou T
       sprintf(tmp,"%02d-%04d-%02d-%02d",n_cal,An,Mois,Jour);
       rc = cals->ReadDirectKey(0,tmp,RetKey,&recdata);
       if (rc)    // il existe des modifs apport�es
          {
            rc = cals->ReadRecord(buffer,recdata);
            buffer[rc]=0;
            if (buffer[13]==0) buffer[13]='N';
            cal[n_cal][i]=buffer[13];
          }
      }
    cal[n_cal][MAX_JOURS-1]= 0;
   }

 cals->CloseEngine(); cals->~realisam();
 //delete cals;
 return 1;
}

int  __fastcall TplStandard::add_jour(int year, int month, int day, int jours)
{
  // resultats dans An,Mois,Jour
  int i,counter,nbj_restant, j;
  int nbdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};


  An=year;Mois=month;Jour=day;
  if (An <100) An = An + 2000;
  int ok=1; j=jours;
  while (ok)
    {
     counter = nbdays[Mois-1];
     if (Mois==2){ if (IsLeapYear(An)) counter = 29;}
     nbj_restant = counter - Jour; // nb jours restants dans le mois en cours
     if (j <= nbj_restant)   // on va sortir
        {
         Jour=Jour+j;
         ok=0;
        }
     else
        {
         j=j-nbj_restant-1;
         Mois++;
         if (Mois==13) {Mois=1; An++; }
         Jour=1;  // 1er jour du mois suivant //Jour+nbj_restant+1;
        }

    }

 return 1;
 }

 int  __fastcall TplStandard::sub_jour(int year, int month, int day, int jours)
{

  int i,counter,ctrjj;
  int ok;
  int nbdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  An=year;Mois=month;Jour=day;
  if (An <100) An = An + 2000;
  ok=1;
    ctrjj=jours+1;  // !!!!!!!!!!!!!!!!!!!!
  while (ok)
    {
     ctrjj--;
     if (ctrjj<=0)
      { ok = 0;
      }
     else
      {
       Jour=Jour-1;
       if (Jour == 0) // on passe au mois Pr�c�dent
        {
         Mois = Mois-1;
         if (Mois <0)
           {
            Mois = 12;
            An=An-1;
           }
         counter = nbdays[Mois-1];
         if (Mois==2) { if (IsLeapYear(An)) counter = 29;}
         Jour=counter;  // 31,30,29,28
        }
       }
     /*
     if (j <= nbj_restant)   // on va sortir
        {
         Jour=Jour+j;
         ok=0;
        }
     else
        {
         j=j-nbj_restant-1;
         Mois++;
         if (Mois==13) {Mois=1; An++; }
         Jour=1;  // 1er jour du mois suivant //Jour+nbj_restant+1;
        }
     */
    }
 return 1;
 }



int  __fastcall TplStandard::convert_date(int year, int month, int day)
{
 int n1,n2,bcl,N;
 int initval;
 //int nbdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  // calcul approximatif de  la distance par rapport au previsionnel
  n1 = (AnPrev%100)*365 + MoisPrev*28 + JourPrev;
  n2 = (year%100)*365 + month*28 + day;
  if (n2<n1) return -1;
  
 if (n2<n1) n2=n1+30;

  initval = n2-n1-30; N=initval;

  for (bcl=0;bcl<100;bcl++)
    {
     add_jour(AnPrev,MoisPrev,JourPrev,N);
     if ((An==year) && (Mois==month) && (Jour==day)) break;
     else N++;
    }
 return N;
}

int __fastcall TplStandard::Convert_Date(AnsiString D)
{
char dt1[20]; int dist1,j1,m1,a1;
strcpy(dt1,D.c_str());
dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
dist1 = convert_date(a1+2000,m1,j1);
return dist1;
}



int  __fastcall TplStandard::avance_conges(int base,int ncal)
{
 // base doit etre place sur le Premier jour a examiner
 int xdeb,xlimit;
 xdeb=base; xlimit= MAX_JOURS-1000;
 // enlever les jours feries
 if (cal[ncal][xdeb] == 'N')
   { while ((cal[ncal][xdeb]=='N')&& (xlimit>0))
      { xdeb++; xlimit--; }

   }
return xdeb;
}

int  __fastcall TplStandard::avance_temps(int base,int ncal,int dur)
{
  // base doit etre positionn� sur le premier jour a examiner
 int xdeb,xdur,xlimit;
 xdeb=base; xdur=dur; xlimit=MAX_JOURS-1000;
 while ((xdur>0) && (xlimit > 0))
   { if (cal[ncal][xdeb]=='T') xdur--;
    xdeb++; xlimit--;
    }
return xdeb;
}

int  __fastcall TplStandard::recule_conges(int base,int ncal)
{

 int xdeb,xlimit;
 //  base pointe sur le jour devant celui a examiner
 // en retour c'est un pointeur sur le "premier jour de conges"
 // sinon, c'est sur base
 xdeb=base-1;xlimit=xdeb;
 // sauter les jours feries;
 //tmp = cal[ncal][xdeb];
 if (cal[ncal][xdeb] == 'N')
   { while((cal[ncal][xdeb] == 'N') && (xlimit >= 0)) {xlimit--; xdeb--; }
   }
 xdeb++;
 return xdeb;

 }

int  __fastcall TplStandard::recule_temps(int base,int ncal, int dur)
{
  int xdeb, xdur, xlimit;
  // base pointe sur le jour devant celui a examiner
  xdeb=base-1; xdur=dur; xlimit=xdeb;
  xlimit=base;
 while ((xdur > 0) && (xlimit >= 0))
   {
    if (cal[ncal][xdeb] == 'T') xdur--;
    xdeb--; xlimit--;
   }
 xdeb++;
 return xdeb;
}


int  __fastcall TplStandard::recule_jours(int base,int ncal, int dur)
{
  int xdeb, xdur, xlimit;
  // base pointe sur le jour devant celui a examiner
  xdeb=base-1; xdur=dur; xlimit=xdeb;
  xlimit=base;
 while ((xdur > 0) && (xlimit >= 0))
   {
    xdur--;
    xdeb--; xlimit--;
   }
 xdeb++;
 return xdeb;
}

////////////////////////////////////////////
int __fastcall TplStandard::Organise_Niveaux()
{
 int i,j,ipred,ok,flag,inext,idd,ddflag,ind,count;
 int memocount;
 int countinsert;


 for (i=0; i<LASTTASK+1;i++) t_select[i]=0;
 for (i=0;i<NN;i++) { for (j=0;j<MAX_T;j++)  niveaux[i][j]=0; }

 for (i=0;i<=LASTTASK;i++) {  for (j=0;j<PS;j++) {    t_pred[i][j]=0; }
    }

 for (i=1;i<=LASTTASK;i++)     // ����������������
   {
     if (av->Cells[1][i] != "")
      {
       ipred=1;
        while(pr[i][ipred] !=0)
        { t_pred[i][ipred]= pr[i][ipred];
          ipred++;
        }
      }
   }
 //    niveau 0


 iniv=0; ok=1; memocount=0;
 while (ok)  // on definit les niveaux
    {
     flag=0;
     inext=0;
     // t_select va recevoir les taches sans predecesseurs
     for (i=0;i<=LASTTASK;i++) t_select[i]=0;  // ���������������
     countinsert = 0;
     for (i=1;i<=LASTTASK;i++) //nb_taches;i++)
       {
        if (av->Cells[1][i] != "")
        {
        if (t_pred[i][1] == 0)
          {
           // elle entre dans t_select,
           add_select(i);   
           countinsert++;
           // examen des successeurs pour detecter les AMONT
           idd=1; ddflag=0;
           while (su[i][idd] !=0)
             {if (su_tl[i][idd] == 2)
               { ddflag=1; break; }
              idd++;
             }
           if (ddflag == 0)  // les AM ne sont pas integrees
             { niveaux[iniv][inext]=i; 
               inext++;
             }
           } // if pred == 0
         } // if not ""
       } // end boucle for


     if (countinsert==0)
       {
        countinsert = countinsert;
        // boucle sur Taches
        return 2;
       }


     ind = 1;
     while (t_select[ind] != 0)
     { remove_task(t_select[ind]);
       ind++;
     }
     // next
     iniv++;
     if(iniv>=NN)
       {
        Application->MessageBoxA("Trop de Niveaux dans Calcul au plus tot",m_ecoplan,MB_OK);
        return 0;
       }

     // fin de calcul ?  quand toutes les taches ont ete selectionnees
     count=0;
     for (i=1;i<=LASTTASK;i++)
      { if (av->Cells[2][i] != "")   // 2 au lieu de 1 ????
        {
          if (t_pred[i][1] == -1)  count++;    // -1
        }
      }
     if (count == LASTTASK) flag=1;       // NB_TACHE

     if (count==memocount)
       {
        return 0;
       }
     memocount=count;

     if (flag) ok=0;
    }  // end while

 return 0;
}

int  __fastcall TplStandard::add_select(int it)
{
 int ix; int flg;
 ix = 1;  flg=0;
 while (t_select[ix] != 0)
   {
    if (t_select[ix] == it)
      {flg=1;
      break;
      }

    ix++;
   }
 if (flg==0)  t_select[ix]=it;
 return flg;
}

int __fastcall TplStandard::Ord_Pred(int ind,int xtpred,int ipred)
{

int total,zcal,xtl;
char tmp[200];   // ����

 xtl = pr_tl[ind][ipred];

if (pr_tl[ind][ipred]==0)     //  c est un lien fd
   {
    if ( pr_decal[ind][ipred] > 0)
      { zcal=tcal[ind];
        total = avance_conges(fin_plus_tot[xtpred]+1,zcal);
        total = avance_temps(total,zcal,pr_decal[ind][ipred]);
        total = avance_conges(total,zcal);  // ������
      }
    else if ( pr_decal[ind][ipred] < 0)
      {  zcal=tcal[xtpred];
         total = avance_conges(fin_plus_tot[xtpred]+1,zcal);
         total = recule_temps(total,zcal,-pr_decal[ind][ipred]);
      }
   else { zcal=tcal[ind];
          total = avance_conges(fin_plus_tot[xtpred]+1,zcal);
        } // pas de calcul de decalage
   }
 if (pr_tl[ind][ipred]==1)     // c est un lien ff
   {
     if (pr_decal[ind][ipred] > 0)
      { zcal=tcal[xtpred];
        total = avance_conges(fin_plus_tot[xtpred]+1,zcal);
        total = avance_temps(total,zcal,pr_decal[ind][ipred]);
        total = avance_conges(total,zcal); //�����
        total = recule_temps(total,zcal,duree[ind]);
      }
     else if (pr_decal[ind][ipred] < 0)
      { zcal=tcal[xtpred];
        total=recule_conges(fin_plus_tot[xtpred]+1,zcal);
        total=recule_temps(total,zcal,-pr_decal[ind][ipred]);
        total = recule_temps(total,zcal,duree[ind]);
      }
     else
      {
        zcal= tcal[ind];
        total=recule_conges(fin_plus_tot[xtpred]+1,zcal);
        total = recule_temps(total,zcal,duree[ind]);
       }
  }

if (pr_tl[ind][ipred] == 2)  total = -1;   //flag_amont = ipred;
if (pr_tl[ind][ipred] == 3)    // Lien DD   Decal est >= 0
   { total=0;
     if ( pr_decal[ind][ipred] > 0)
       { zcal=tcal[xtpred];
         total=avance_conges(deb_plus_tot[xtpred],zcal);
         total = avance_temps(total,zcal,pr_decal[ind][ipred]);
         total = avance_conges(total,zcal); //�������
       }
     else if ( pr_decal[ind][ipred] == 0)
       { zcal=tcal[ind];
         total=avance_conges(deb_plus_tot[xtpred],zcal);
        } // pas de calcul de decalage
   }
if (pr_tl[ind][ipred] == 5)   //    c'est un lien souhait�
   {



   }

 // if (total < 0) total = 0;

 return total;

}

void __fastcall TplStandard::Compute_PlusTotA()
{
  int indx,ix,ipred,imax,istop, flag, count;
  int ok, xtpred, total;
  int nb_pass,fini;
  char tmp[550];  // �����
  int xind;     AnsiString TEMP;
  int avance,inter;
  int A,M,J;
  int inext,ind,tniv;
  int fduree,fdecal;
  int idd, ddflag,flag_amont,t_amont,t_start;
  int i,j,res;
  int mom; int zcal;


sommets[LASTTASK+1]=0;
if (nb_taches == 0) return;


for (i=0;i<MAX_T;i++)  flag_souhaites[i] = 'N';

 // calculer Nouveaux Liens pour  DEbut ou Fin Souhaitee
 for (i=1;i<=LASTTASK;i++)
    {
     if (av->Cells[1][i] != "")
       {
        strcpy(tmp,av->Cells[3][i].c_str());
        duree[i] = atoi(tmp);
        duree_reelle[i]=duree[i];
        TEMP= AdvStringGrid1->Cells[12][i];
        if (AdvStringGrid1->Cells[12][i] != "")
            Compute_DebutSouhaite(12,i);
        TEMP= AdvStringGrid1->Cells[13][i];
        if (AdvStringGrid1->Cells[13][i] != "")
           Compute_FinSouhaitee(13,i);
       }

    }

  res = Organise_Niveaux();

 xind = 1;

 for (tniv=0;tniv<iniv;tniv++)
   {
    inext=0; xmax=-1000;
    while (niveaux[tniv][inext] != 0)
     {
      ind=niveaux[tniv][inext];

      //ind = exist_tache(ind,nb_taches);
       if (tniv==0)  // niveau zero   : Taches sans predecesseurs
       {
         if (ind != 9999)
           {
            // on traite les taches normales
            imax=0; zcal=tcal[ind];
            total=avance_conges(0,zcal);
            deb_plus_tot[ind] = total;
            av->Cells[38][ind]= total;
            inter =  avance_temps(total,zcal,duree[ind])-1;
            fin_plus_tot[ind]=  inter;
            av->Cells[39][ind]= fin_plus_tot[ind];
            duree_reelle[ind] = fin_plus_tot[ind] - deb_plus_tot[ind]+ 1;
            if (fin_plus_tot[ind] > xmax)
            { xmax= fin_plus_tot[ind]; xind=ind; }
           }
        else
           {

           }

       }
      else
       {
        // cherchons les predecesseurs de ind
        imax = -1000;ipred = 1; total=0;  flag_amont = 0;
        while (pr[ind][ipred]!=0)
          {
           xtpred=pr[ind][ipred];
           total = Ord_Pred(ind,xtpred,ipred);
           if (total > imax) imax=total;
           ipred++;
         }
        zcal=tcal[ind];
        deb_plus_tot[ind] = imax;
        av->Cells[38][ind]= imax;
        fin_plus_tot[ind] = avance_temps(imax,zcal,duree[ind])-1;  // !!!! decalage de -1
        fin_plus_tot[ind] = avance_conges(fin_plus_tot[ind],zcal);
        av->Cells[39][ind]= fin_plus_tot[ind];
        if (fin_plus_tot[ind] > xmax) { xmax= fin_plus_tot[ind];xind = ind; }

        duree_reelle[ind]=fin_plus_tot[ind]-deb_plus_tot[ind]+1;

        if (fin_plus_tot[ind] > xmax)
            { xmax= fin_plus_tot[ind];xind = ind; }

        // si taches amont : calcul des dates au plus tot
        if (flag_amont)
          {
           t_start = ind;
           t_amont = pr[ind][flag_amont];
           while (t_amont >0)
             { int x_amont = t_amont;
              //t_amont = exist_tache(t_amont,nb_taches);


              // on connait la mere
              // mom=tmom[t_amont];
              //deb_plus_tot[t_amont]=deb_plus_tot[mom];
              //deb_plus_tard[t_amont]=deb_plus_tard[mom];
              zcal=tcal[t_start];
              fdecal = comp_down(deb_plus_tot[t_start],zcal,pr_decal[t_start][flag_amont]);
              zcal=tcal[t_amont];
              fduree = comp_down(fdecal,zcal,duree[t_amont]);
              if (fduree < 0)
                { sprintf(tmp,"Tache Amont %d : Debordement possible sur le d�but",x_amont);
                 Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
                }
              deb_plus_tot[t_amont] = fduree; // fin_plus_tot[t_amont]=xstart;
              fin_plus_tot[t_amont] = comp_up(fduree-1,zcal,duree[t_amont]); //fdecal-1;
              duree_reelle[t_amont]= fin_plus_tot[t_amont]-deb_plus_tot[t_amont];
              t_start= t_amont;
              t_amont = pr[t_amont][1]; flag_amont = 1;
             }

          }

       }
     inext++;
     }
   }
 xmax=xmax;
 deb_plus_tot[LASTTASK+1]= deb_plus_tot[xind]+ duree[xind]; //duree_reelle[xind];
 //if (xmax <0) xmax = 0;

 // on fait des successeurs pour la tache fictive  (nb_taches+1)


 for (ind=1;ind<=LASTTASK;ind++)   //
   {
    if (su[ind][1]==0)
    su[ind][1]=9999;
   }

}




void __fastcall TplStandard::Compute_PlusTardA(char mode,bool disp)
{
 int ix,tniv,inext,indx,isucc,xtsucc;
 int imin,xtotal;
 int mmin; int ctr_1;
 int i,imax,xmax,zx; int xx;
 int fdecal, fduree;
 char tmp[550]; // ����
 int idd, ddflag,flag_amont,t_amont,t_start,imom;
 int indmom;  char stach[20];
 int zcal;

 flag_amont=0;
 if (nb_taches==0) return;
 for (ix=0;ix<MAX_T;ix++)   { deb_plus_tard[ix]=-1; }
 sommets[LASTTASK+1]=9999;   // nb_task juste pour calcul au plus tard

 // calcul de la date au plus tard de 9999  (nb_taches + 1)  depend de la duree
 // max si certaines taches sont longues
 imax=-1;
 for (i=1;i<=LASTTASK;i++)  // nb_taches;i++)    //�������������
   {
    if (av->Cells[1][i] != "")
     {
      ix=deb_plus_tot[i] + duree_reelle[i];
      if (ix > imax) imax=ix;
     }
   }

  if (imax>deb_plus_tot[LASTTASK+1])
      deb_plus_tard[LASTTASK+1]=imax;
   else deb_plus_tard[LASTTASK+1] = deb_plus_tot[LASTTASK+1];

  for (tniv=iniv-1;tniv>=0;tniv--)
   {
    inext=0;
    while (niveaux[tniv][inext] != 0)
      {
       indx=niveaux[tniv][inext];
       indmom = niveaux[tniv][inext];
       //  indx = exist_tache(indx,nb_taches);  // ������������
       imin=+100000;
        // cherchons les successeurs de ind
        isucc = 1; xtotal=0;
        xtsucc=su[indx][isucc];
        if (xtsucc==0 &&isucc==1) su[indx][1]=9999;
        while (su[indx][isucc]!=0) //  && su[indx][isucc]!=9999)
          {
            xtsucc=su[indx][isucc];
            if (xtsucc==9999)
              {
               if (isucc == 1)  // pas de vrai successeurs
                   // imin = deb_plus_tard[nb_taches+1] - duree_reelle[indx]; //0;
                  {
                   zcal=tcal[indx];
                   xtotal = recule_conges(deb_plus_tard[LASTTASK+1],zcal); //,duree[indx]);
                   xtotal = recule_temps(xtotal,zcal,duree[indx]);
                   imin=xtotal;
                  }
                else  // il existe un successeur
                  {  imin=imin; }
               }
            else   //  xtsucc !=
              {
               //if (xtsucc!=9999)
               //  {
                   xtotal = comp_dist_succ(indx,xtsucc,isucc);
                   if (xtotal < imin) { imin=xtotal; }
               //   }

              }
           isucc++;
          }  // while su

        if (imin==100000)  imin=0;
        deb_plus_tard[indx]=imin;
        zcal=tcal[indx];
        fin_plus_tard[indx]=avance_temps(imin,zcal,duree[indx]) - 1;
        if (su_tl[indx][1]== 2)
          {
           deb_plus_tard[indx] = deb_plus_tot[indx];
           fin_plus_tard[indx] = fin_plus_tot[indx];
          }
        if (flag_amont) { }
      inext++;
      }
   }

  // marge totale  et date de fin de projet
  xmax = 0;
  for (indx=1;indx<=LASTTASK;indx++)
    {
     if (deb_plus_tard[indx]==deb_plus_tot[indx])
       {  marg_tot[indx]=0;
          if (fin_plus_tot[indx] > xmax) { xmax= fin_plus_tot[indx]; }
       }
     else
      { // calculer le nombre de jour travaill�s
         zcal=tcal[indx];
         marg_tot[indx] = compute_marge(deb_plus_tot[indx],deb_plus_tard[indx],zcal);
         if (fin_plus_tot[indx] > xmax) { xmax= fin_plus_tot[indx]; }
           
      }
    }
 AnsiString CR;
 CR="";
 StatusBar1->SimpleText = "";
 ctr_1=0;
 for (indx=1;indx<=LASTTASK;indx++)
   {
    if (av->Cells[1][indx]!="")
     {
      if (deb_plus_tard[indx]==-1)
         { ctr_1++;
           CR = CR + AnsiString(indx) + ",";
         }
     }
   }

 if (ctr_1 >0)
 {
  // if (CR!="")
 //  StatusBar1->SimpleText = " " + CR;
   //Beep();
  }


 add_jour(AnPrev,MoisPrev,JourPrev,xmax);
 AnFin=An;MoisFin=Mois;JourFin=Jour;
 sprintf(tmp,"%02d/%02d/%02d",JourFin,MoisFin,AnFin%100);
 Label44->Caption=AnsiString(tmp);
 Label117->Caption=AnsiString(tmp);
 Label135->Caption=AnsiString(tmp);

 for (xx=0;xx<8;xx++)
   strcpy(edFIN[xx],Label44->Caption.c_str());
 Label130 ->Caption = Label44->Caption;
 Label152->Caption = Label44->Caption;
 Label126 ->Caption = Label44->Caption;
 Label99 ->Caption = Label44->Caption;
 Label131 ->Caption = AnsiString(tmp);
 Label154 ->Caption = AnsiString(tmp);
 DTFINREEL = Label131 ->Caption;
 Label128 ->Caption = AnsiString(tmp);
 Label106 ->Caption = Label128->Caption;
 Label118 ->Caption = Label12->Caption;
 Label68 ->Caption = Label12->Caption;

 sommets[LASTTASK+1]=0;


 //  Remplissage des differentes colonnes de AdvStringGrid1;

 int cnt,bcl;
 //if (mode=='1')
 // {
   cnt=AdvStringGrid1->RowCount-1;
   for (bcl=1;bcl<=cnt;bcl++)
   //for (indx=1;indx<=nb_taches;indx++)     // ������������������
    {
      indx=Lect_Numt(bcl);

      AdvStringGrid1->Cells[3][bcl] = AnsiString(duree[indx]);
      av->Cells[3][indx]=AnsiString(duree[indx]);
      add_jour(AnPrev,MoisPrev,JourPrev,deb_plus_tot[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[4][bcl] = AnsiString(tmp); //deb_plus_tot[indx]+1);
      av->Cells[4][indx]=AnsiString(tmp);
      add_jour(AnPrev,MoisPrev,JourPrev,fin_plus_tot[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[5][bcl] = AnsiString(tmp);  // fin_plus_tot[indx]);
      av->Cells[5][indx]=AnsiString(tmp);
      add_jour(AnPrev,MoisPrev,JourPrev,deb_plus_tard[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[6][bcl] = AnsiString(tmp); //deb_plus_tard[indx]+1);
      av->Cells[6][indx]=AnsiString(tmp);
      add_jour(AnPrev,MoisPrev,JourPrev,fin_plus_tard[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[7][bcl] = AnsiString(tmp);   // fin_plus_tard[indx]);
      av->Cells[7][indx]=AnsiString(tmp);
      AdvStringGrid1->Cells[8][bcl] = AnsiString(marg_tot[indx]);
      av->Cells[8][indx] = AnsiString(marg_tot[indx]);
      AdvStringGrid1->Colors[1][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[4][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[5][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[6][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[7][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[10][bcl] = TColor(RGB(255,255,153));
      if (marg_tot[indx] == 0)
        {
          AdvStringGrid1->Colors[8][bcl]=TColor(RGB(255,255,51));
        }
      else
        {

         AdvStringGrid1->Cells[9][bcl] = "";
         AdvStringGrid1->Colors[8][bcl]=clWhite;
        }
   //  }
  }

}


void __fastcall TplStandard::Compute_PlusTotAReel()
{
  int indx,ix,ipred,imax,istop, flag, count;
  int ok, xtpred, total;
  int nb_pass,fini;
  char tmp[550];  // �����
  int xind;     AnsiString TEMP;
  int avance,inter;
  int A,M,J;
  int inext,ind,tniv;
  int fduree,fdecal;
  int idd, ddflag,flag_amont,t_amont,t_start;
  int i,j,res;
  int mom; int zcal;


sommets[LASTTASK+1]=0;
if (nb_taches == 0) return;


for (i=0;i<MAX_T;i++)  flag_souhaites[i] = 'N';

 // calculer Nouveaux Liens pour  DEbut ou Fin Souhaitee
 for (i=1;i<=LASTTASK;i++)
    {
     if (av->Cells[1][i] != "")
       {
        strcpy(tmp,av->Cells[3][i].c_str());
        duree[i] = atoi(tmp);
        duree_reelle[i]=duree[i];
        TEMP= AdvStringGrid1->Cells[12][i];
        if (AdvStringGrid1->Cells[12][i] != "")
            Compute_DebutSouhaite(12,i);
        TEMP= AdvStringGrid1->Cells[13][i];
        if (AdvStringGrid1->Cells[13][i] != "")
           Compute_FinSouhaitee(13,i);
       }

    }

  res = Organise_Niveaux();

 xind = 1;

 for (tniv=0;tniv<iniv;tniv++)
   {
    inext=0; xmax=-1000;
    while (niveaux[tniv][inext] != 0)
     {
      ind=niveaux[tniv][inext];

      //ind = exist_tache(ind,nb_taches);
       if (tniv==0)  // niveau zero   : Taches sans predecesseurs
       {
         if (ind != 9999)
           {
            // on traite les taches normales
            imax=0; zcal=tcal[ind];
            total=avance_conges(0,zcal);
            deb_plus_tot[ind] = total;
            av->Cells[38][ind]= total;
            inter =  avance_temps(total,zcal,duree[ind])-1;
            fin_plus_tot[ind]=  inter;
            av->Cells[39][ind]= fin_plus_tot[ind];
            duree_reelle[ind] = fin_plus_tot[ind] - deb_plus_tot[ind]+ 1;
            if (fin_plus_tot[ind] > xmax)
            { xmax= fin_plus_tot[ind]; xind=ind; }
           }
        else
           {

           }

       }
      else
       {
        // cherchons les predecesseurs de ind
        imax = -1000;ipred = 1; total=0;  flag_amont = 0;
        while (pr[ind][ipred]!=0)
          {
           xtpred=pr[ind][ipred];
           total = Ord_Pred(ind,xtpred,ipred);
           if (total > imax) imax=total;
           ipred++;
         }
        zcal=tcal[ind];
        deb_plus_tot[ind] = imax;
        av->Cells[38][ind]= imax;
        fin_plus_tot[ind] = avance_temps(imax,zcal,duree[ind])-1;  // !!!! decalage de -1
        fin_plus_tot[ind] = avance_conges(fin_plus_tot[ind],zcal);
        av->Cells[39][ind]= fin_plus_tot[ind];
        if (fin_plus_tot[ind] > xmax) { xmax= fin_plus_tot[ind];xind = ind; }

        duree_reelle[ind]=fin_plus_tot[ind]-deb_plus_tot[ind]+1;

        if (fin_plus_tot[ind] > xmax)
            { xmax= fin_plus_tot[ind];xind = ind; }

        // si taches amont : calcul des dates au plus tot
        if (flag_amont)
          {
           t_start = ind;
           t_amont = pr[ind][flag_amont];
           while (t_amont >0)
             { int x_amont = t_amont;
              //t_amont = exist_tache(t_amont,nb_taches);


              // on connait la mere
              // mom=tmom[t_amont];
              //deb_plus_tot[t_amont]=deb_plus_tot[mom];
              //deb_plus_tard[t_amont]=deb_plus_tard[mom];
              zcal=tcal[t_start];
              fdecal = comp_down(deb_plus_tot[t_start],zcal,pr_decal[t_start][flag_amont]);
              zcal=tcal[t_amont];
              fduree = comp_down(fdecal,zcal,duree[t_amont]);
              if (fduree < 0)
                { sprintf(tmp,"Tache Amont %d : Debordement possible sur le d�but",x_amont);
                 Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
                }
              deb_plus_tot[t_amont] = fduree; // fin_plus_tot[t_amont]=xstart;
              fin_plus_tot[t_amont] = comp_up(fduree-1,zcal,duree[t_amont]); //fdecal-1;
              duree_reelle[t_amont]= fin_plus_tot[t_amont]-deb_plus_tot[t_amont];
              t_start= t_amont;
              t_amont = pr[t_amont][1]; flag_amont = 1;
             }

          }

       }
     inext++;
     }
   }
 xmax=xmax;
 deb_plus_tot[LASTTASK+1]= deb_plus_tot[xind]+ duree[xind]; //duree_reelle[xind];
 //if (xmax <0) xmax = 0;

 // on fait des successeurs pour la tache fictive  (nb_taches+1)


 for (ind=1;ind<=LASTTASK;ind++)   //
   {
    if (su[ind][1]==0)
    su[ind][1]=9999;
   }

}




void __fastcall TplStandard::Compute_PlusTardAReel(char mode,bool disp)
{
 int ix,tniv,inext,indx,isucc,xtsucc;
 int imin,xtotal;
 int mmin; int ctr_1;
 int i,imax,xmax,zx; int xx;
 int fdecal, fduree;
 char tmp[550]; // ����
 int idd, ddflag,flag_amont,t_amont,t_start,imom;
 int indmom;  char stach[20];
 int zcal;

 flag_amont=0;
 if (nb_taches==0) return;
 for (ix=0;ix<MAX_T;ix++)   { deb_plus_tard[ix]=-1; }
 sommets[LASTTASK+1]=9999;   // nb_task juste pour calcul au plus tard

 // calcul de la date au plus tard de 9999  (nb_taches + 1)  depend de la duree
 // max si certaines taches sont longues
 imax=-1;
 for (i=1;i<=LASTTASK;i++)  // nb_taches;i++)    //�������������
   {
    if (av->Cells[1][i] != "")
     {
      ix=deb_plus_tot[i] + duree_reelle[i];
      if (ix > imax) imax=ix;
     }
   }

  if (imax>deb_plus_tot[LASTTASK+1])
      deb_plus_tard[LASTTASK+1]=imax;
   else deb_plus_tard[LASTTASK+1] = deb_plus_tot[LASTTASK+1];

  for (tniv=iniv-1;tniv>=0;tniv--)
   {
    inext=0;
    while (niveaux[tniv][inext] != 0)
      {
       indx=niveaux[tniv][inext];
       indmom = niveaux[tniv][inext];
       //  indx = exist_tache(indx,nb_taches);  // ������������
       imin=+100000;
        // cherchons les successeurs de ind
        isucc = 1; xtotal=0;
        xtsucc=su[indx][isucc];
        if (xtsucc==0 &&isucc==1) su[indx][1]=9999;
        while (su[indx][isucc]!=0) //  && su[indx][isucc]!=9999)
          {
            xtsucc=su[indx][isucc];
            if (xtsucc==9999)
              {
               if (isucc == 1)  // pas de vrai successeurs
                   // imin = deb_plus_tard[nb_taches+1] - duree_reelle[indx]; //0;
                  {
                   zcal=tcal[indx];
                   xtotal = recule_conges(deb_plus_tard[LASTTASK+1],zcal); //,duree[indx]);
                   xtotal = recule_temps(xtotal,zcal,duree[indx]);
                   imin=xtotal;
                  }
                else  // il existe un successeur
                  {  imin=imin; }
               }
            else   //  xtsucc !=
              {
               //if (xtsucc!=9999)
               //  {
                   xtotal = comp_dist_succ(indx,xtsucc,isucc);
                   if (xtotal < imin) { imin=xtotal; }
               //   }

              }
           isucc++;
          }  // while su

        if (imin==100000)  imin=0;
        deb_plus_tard[indx]=imin;
        zcal=tcal[indx];
        fin_plus_tard[indx]=avance_temps(imin,zcal,duree[indx]) - 1;
        if (su_tl[indx][1]== 2)
          {
           deb_plus_tard[indx] = deb_plus_tot[indx];
           fin_plus_tard[indx] = fin_plus_tot[indx];
          }
        if (flag_amont) { }
      inext++;
      }
   }

  // marge totale  et date de fin de projet
  xmax = 0;
  for (indx=1;indx<=LASTTASK;indx++)
    {
     if (deb_plus_tard[indx]==deb_plus_tot[indx])
       {  marg_tot[indx]=0;
          if (fin_plus_tot[indx] > xmax) { xmax= fin_plus_tot[indx]; }
       }
     else
      { // calculer le nombre de jour travaill�s
         zcal=tcal[indx];
         marg_tot[indx] = compute_marge(deb_plus_tot[indx],deb_plus_tard[indx],zcal);
         if (fin_plus_tot[indx] > xmax) { xmax= fin_plus_tot[indx]; }
           
      }
    }
 AnsiString CR;
 CR="";
 StatusBar1->SimpleText = "";
 ctr_1=0;
 for (indx=1;indx<=LASTTASK;indx++)
   {
    if (av->Cells[1][indx]!="")
     {
      if (deb_plus_tard[indx]==-1)
         { ctr_1++;
           CR = CR + AnsiString(indx) + ",";
         }
     }
   }

 if (ctr_1 >0)
 {
  // if (CR!="")
 //  StatusBar1->SimpleText = " " + CR;
   //Beep();
  }


 add_jour(AnPrev,MoisPrev,JourPrev,xmax);
 AnFin=An;MoisFin=Mois;JourFin=Jour;
 sprintf(tmp,"%02d/%02d/%02d",JourFin,MoisFin,AnFin%100);
 Label44->Caption=AnsiString(tmp);
 Label117->Caption=AnsiString(tmp);
 Label135->Caption=AnsiString(tmp);

 for (xx=0;xx<8;xx++)
   strcpy(edFIN[xx],Label44->Caption.c_str());
 Label130 ->Caption = Label44->Caption;
 Label126 ->Caption = Label44->Caption;
 Label99 ->Caption = Label44->Caption;
 Label131 ->Caption = AnsiString(tmp);
 DTFINREEL = Label131 ->Caption;
 Label128 ->Caption = AnsiString(tmp);
 Label106 ->Caption = Label128->Caption;
 Label118 ->Caption = Label12->Caption;
 Label68 ->Caption = Label12->Caption;

 sommets[LASTTASK+1]=0;


 //  Remplissage des differentes colonnes de AdvStringGrid1;

 int cnt,bcl;
 //if (mode=='1')
 // {
   cnt=AdvStringGrid1->RowCount-1;
   for (bcl=1;bcl<=cnt;bcl++)
   //for (indx=1;indx<=nb_taches;indx++)     // ������������������
    {
      indx=Lect_Numt(bcl);

      AdvStringGrid1->Cells[3][bcl] = AnsiString(duree[indx]);
      av->Cells[3][indx]=AnsiString(duree[indx]);
      add_jour(AnPrev,MoisPrev,JourPrev,deb_plus_tot[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[4][bcl] = AnsiString(tmp); //deb_plus_tot[indx]+1);
      av->Cells[4][indx]=AnsiString(tmp);
      add_jour(AnPrev,MoisPrev,JourPrev,fin_plus_tot[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[5][bcl] = AnsiString(tmp);  // fin_plus_tot[indx]);
      av->Cells[5][indx]=AnsiString(tmp);
      add_jour(AnPrev,MoisPrev,JourPrev,deb_plus_tard[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[6][bcl] = AnsiString(tmp); //deb_plus_tard[indx]+1);
      av->Cells[6][indx]=AnsiString(tmp);
      add_jour(AnPrev,MoisPrev,JourPrev,fin_plus_tard[indx]);
      sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
      AdvStringGrid1->Cells[7][bcl] = AnsiString(tmp);   // fin_plus_tard[indx]);
      av->Cells[7][indx]=AnsiString(tmp);
      AdvStringGrid1->Cells[8][bcl] = AnsiString(marg_tot[indx]);
      av->Cells[8][indx] = AnsiString(marg_tot[indx]);
      AdvStringGrid1->Colors[1][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[4][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[5][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[6][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[7][bcl] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[10][bcl] = TColor(RGB(255,255,153));
      if (marg_tot[indx] == 0)
        {
          AdvStringGrid1->Colors[8][bcl]=TColor(RGB(255,255,51));
        }
      else
        {

         AdvStringGrid1->Cells[9][bcl] = "";
         AdvStringGrid1->Colors[8][bcl]=clWhite;
        }
   //  }
  }

}


int  __fastcall TplStandard::comp_up(int deb,int ncal,int dur)
{
 int xdeb,xdur,xlimit;
 xdeb=deb+1; xdur=dur; xlimit=MAX_JOURS;
 // enlever les jours feries
 if (cal[ncal][xdeb] == 'N')
   { while (cal[ncal][xdeb]=='N') xdeb++;
     xstart= xdeb; xdeb--;
   }
 else xstart=xdeb;

 while ((xdur>0) && (xlimit > 0))
   { if (cal[ncal][xdeb]=='T')
         xdur--;
    xdeb++; xlimit--;
    }
 xdeb--;
return xdeb;
}

int  __fastcall TplStandard::comp_down(int deb,int ncal,int dur)
{
 int xdeb,xdur,xlimit;
 xdeb=deb-1; xdur=dur; xlimit=xdeb;
 // sauter les jours feries;
 if (cal[ncal][xdeb] == 'N')
   { while((cal[ncal][xdeb] == 'N') && (xlimit >= 0)) {xlimit--; xdeb--; }
     xstart=xdeb; xdeb++;
   }
 else { xstart=xdeb; }   // xdeb
 xlimit=deb;
 while ((xdur > 0) && (xlimit >0))
   {
    if (cal[ncal][xdeb] == 'T') xdur--;
    xdeb--; xlimit--;
   }
 xdeb++;
 return xdeb;
}

int __fastcall  TplStandard::comp_dist_succ(int indx,int xtsucc, int isucc)
{
 int zcal,xtotal,flag_amont,fduree;

 if (su_tl[indx][isucc]==0)     //  c est un lien fd
                  {
                   if (su_decal[indx][isucc] > 0)
                     { zcal=tcal[indx];  // xtsucc];
                       xtotal=recule_conges(deb_plus_tard[xtsucc],zcal);
                       xtotal=recule_temps(xtotal,zcal,su_decal[indx][isucc]);
                       xtotal=recule_temps(xtotal,zcal,duree[indx]);
                     }
                   else if (su_decal[indx][isucc] < 0)
                     {
                      zcal=tcal[indx]; //xtsucc];
                      xtotal=avance_conges(deb_plus_tard[xtsucc],zcal);
                      xtotal=avance_temps(xtotal,zcal,-su_decal[indx][isucc]);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);
                      //fduree = comp_up(deb_plus_tard[xtsucc],zcal,-su_decal[indx][isucc]);
                      //zcal=tcal[indx];
                      //xtotal = comp_down(fduree,zcal,duree[indx]);
                     }
                   else
                     {zcal=tcal[indx];
                      xtotal=recule_conges(deb_plus_tard[xtsucc],zcal);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);
                      //xtotal = comp_down(deb_plus_tard[xtsucc],zcal,duree[indx]);
                     }
                  }
                if (su_tl[indx][isucc]==1)     //  c est un lien ff
                  {
                   if (su_decal[indx][isucc] > 0)
                     {zcal=tcal[indx]; //  indx]; // xtsucc];
                      xtotal=recule_conges(fin_plus_tard[xtsucc]+1,zcal);
                      xtotal=recule_temps(xtotal,zcal,su_decal[indx][isucc]);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);

                      //fduree = comp_down(fin_plus_tard[xtsucc],zcal,su_decal[indx][isucc]-1);
                      //zcal=tcal[indx];
                      //xtotal = comp_down(fduree,zcal,duree[indx]);
                     }
                   else if (su_decal[indx][isucc] < 0)
                     {zcal=tcal[indx];  //  indx]; //xtsucc];
                      xtotal=avance_conges(fin_plus_tard[xtsucc]+1,zcal);
                      xtotal=avance_temps(xtotal,zcal,-su_decal[indx][isucc]);
                      xtotal = avance_conges(xtotal,zcal); //�������
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);

                      //fduree = comp_up(fin_plus_tard[xtsucc],zcal,-su_decal[indx][isucc]); // -1);
                      //zcal=tcal[indx];
                      //xtotal = comp_down(fduree,zcal,duree[indx]);
                     }
                   else
                     {zcal=tcal[indx];
                      xtotal=recule_conges(fin_plus_tard[xtsucc]+1,zcal);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);
                      // xtotal = comp_down(fduree,zcal,duree[indx]);
                     }
                  }
                 if (su_tl[indx][isucc]==2)     //  c est un lien amont
                  {
                   flag_amont = isucc;
                  }

                 if (su_tl[indx][isucc]==3)     //  c est un lien DD
                  {
                   if (su_decal[indx][isucc] > 0)
                     { zcal=tcal[indx];  // xtsucc];

                       xtotal = comp_down(deb_plus_tard[xtsucc],zcal,su_decal[indx][isucc]);
                       //xtotal = comp_down(fduree,zcal,duree[indx]);    // $$$$$$$$$$$$
                     }
                   else if (su_decal[indx][isucc] == 0)
                     {zcal=tcal[indx];
                      //xtotal = comp_down(deb_plus_tard[xtsucc],zcal,duree[indx]);
                      xtotal = deb_plus_tard[xtsucc];
                     }
                  }

 return xtotal;


}


int __fastcall TplStandard::refresh_marges()
{
 int indx,ntach;
 char tmp[50];   //����
 for (indx=1;indx<=AdvStringGrid1->RowCount;indx++)
   {
    strcpy(tmp,AdvStringGrid1->Cells[1][indx].c_str());
    ntach = atoi(tmp);
    if (marg_tot[ntach] == 0) AdvStringGrid1->Colors[8][indx]=TColor(RGB(255,255,51));
      else   AdvStringGrid1->Colors[8][indx]=clWhite;
   }
 return 1;
}

int  __fastcall TplStandard::compute_marge(int tot, int tard, int ncal)
{
 int i,marg;
 marg=0;
 for (i=tot; i<tard;i++)
  {
   if (cal[ncal][i]== 'T') marg++;
  }
 return marg;
}


void __fastcall TplStandard::DrawGantt(int task,int pos)
{
  int nbjours;
  int ix,ixx,tdeb,tfin;
  int xbase,x1base,ttot;
  int interval;
  char tmp[220];  // ����
  int  X1,X2,Y,xx1,xx2;
  int isucc;
  int firstday;
  AnsiString Texte;
  TDateTime dtFirst;
  int jx,jcal;
  int kx,kinterv,K1,K2;
  int basecal;
  int xtask;

  dtFirst = EncodeDate(AnPrev,MoisPrev,JourPrev);
  firstday = dtFirst.DayOfWeek();
  //int dow;  // day of week
  // dow = firstday-1;

  if (nb_taches==0) return;

   BgColor=clWhite;
   FgColor=clLime;
   TxColor=clYellow;


 Diagram = Rect(0,0, imGantt->Width,imGantt->Height);
 imGantt->Canvas->Brush->Color = clWhite; // BgColor;
 imGantt->Canvas->FillRect(Diagram);

 Diagram = Rect(0,0,imGantt->Width,imGantt->Height/10);
 imGantt->Canvas->Brush->Color = clAqua; //
 imGantt->Canvas->FillRect(Diagram);

 // trac� du rectangle du haut;
 imGantt->Canvas->Brush->Color = clBlack; //
 imGantt->Canvas->MoveTo(0,0);
 imGantt->Canvas->LineTo(imGantt->Width-1,0);
 imGantt->Canvas->LineTo(imGantt->Width-1,imGantt->Height/20);
 imGantt->Canvas->LineTo(0,imGantt->Height/20);
 imGantt->Canvas->LineTo(0,0);
 imGantt->Canvas->LineTo(0,imGantt->Height);
 //  Trac� du compte de jours

 if (nb_taches==0) return;

 nbjours = deb_plus_tot[LASTTASK+1];
 if (nbjours != 0) interval = (imGantt->Width / nbjours)*0.90;
   else interval = 100;
 imGantt->Canvas->Font->Color = clBlack;

 interval=25; //nbjours=30;

 xtask=task-5; if (xtask <1) xtask=1;

 ttot=deb_plus_tot[xtask];
 // adjust with position
 // position is relative to the middle (50 %)
 ttot = ttot + int (40*pos/200);     //  30/2
 if (ttot <0) ttot=0;
 //$$$$ScrollBar2->Position = 50 + pos;
 //$$$if (nb_taches>0) ScrollBar1->Position = 100 * xtask / nb_taches;

 imGantt->Canvas->Font->Color = clBlack;
 imGantt->Canvas->MoveTo(0,imGantt->Height/10);
 imGantt->Canvas->LineTo(imGantt->Width,imGantt->Height/10);

 // fix� a l'aide de calendrier 1;  //////////////////
 for (ix=ttot;ix<=(nbjours+ttot);ix++)    // tenir compte du calendrier
   {
    xbase=(ix-ttot)*interval +interval/3;
    sprintf (tmp,"%d",ix+1);       //  commence � 1, pas a z�ro
    add_jour(AnPrev,MoisPrev,JourPrev,ix);
    sprintf(tmp, "%02d",Jour);
    imGantt->Canvas->Font->Height = 8;
    imGantt->Canvas->Font->Color = clBlack;
    imGantt->Canvas->Brush->Color = clAqua;  // White;
    imGantt->Canvas->Brush->Style = bsClear;
    imGantt->Canvas->TextOut(xbase,3,(AnsiString) tmp);
    imGantt->Canvas->Font->Color = clBlack;
    imGantt->Canvas->MoveTo(ix*interval,0);
    imGantt->Canvas->LineTo(ix*interval,imGantt->Height/20);

    // trac� des conges
    xbase= (ix-ttot+1)*interval;
        if (cal[1][ix]=='N')
      {
       x1base  =  xbase - interval+1;
       Diagram = Rect(x1base,(imGantt->Height/10)+1,xbase,imGantt->Height);
       imGantt->Canvas->Brush->Color = clSilver; //
       imGantt->Canvas->FillRect(Diagram);
      }

     imGantt->Canvas->Brush->Color = clWhite;
     imGantt->Canvas->Pen->Style = psDot;  // Dash;
     imGantt->Canvas->MoveTo(xbase,imGantt->Height/10);
     imGantt->Canvas->LineTo(xbase,imGantt->Height);
     imGantt->Canvas->Pen->Style = psSolid;

     /// AN = 15100 ������������������������������

     dtFirst = EncodeDate(An,Mois,Jour);
     firstday = dtFirst.DayOfWeek();
     if (firstday == 2)  // Mondays
       {
       // trace d'un trait vertical noir
        x1base  =  xbase - interval;
        sprintf(tmp,"%02d/%02d/%04d",Jour,Mois,An);
        imGantt->Canvas->Font->Color = clBlack;
        imGantt->Canvas->Brush->Color = clAqua;  // White;
        imGantt->Canvas->Brush->Style = bsClear;
        imGantt->Canvas->TextOut(x1base+5,(imGantt->Height/20)+3,(AnsiString) tmp);
        imGantt->Canvas->MoveTo(x1base,imGantt->Height/20);
        imGantt->Canvas->LineTo(x1base,imGantt->Height);
       }
     if (Jour==1)  // 1er du mois, un trait rouge;
       {
        x1base  =  xbase - interval;
        imGantt->Canvas->Pen->Color = clRed;
        imGantt->Canvas->MoveTo(x1base,imGantt->Height/10);
        imGantt->Canvas->LineTo(x1base,imGantt->Height);
        imGantt->Canvas->Pen->Color = clBlack;
       }

   }


 //14 taches par ecran   // 30
 tdeb=xtask;  tfin=tdeb+30; if (tfin>LASTTASK) tfin=LASTTASK;

Y0 = 40;
 for (ixx=tdeb;ixx<=tfin;ixx++)
   {
    // calcul des coordonnees

    xx1 = duree[ixx];
    if (xx1==0) X2=-1; else X2= xx2*interval;

    if (X2>=0)
       {
         xx1 = deb_plus_tot[ixx]-ttot;
         xx2=fin_plus_tot[ixx]+1-ttot;
         X1= xx1*interval;
         Y0 = Y0 + 40;
         Y=Y0;
         imGantt->Canvas->Pen->Width = 4;
         if (su_tl[ixx][1] == 2) imGantt->Canvas->Pen->Color = clGreen;
         else if (marg_tot[ixx]==0) imGantt->Canvas->Pen->Color = clRed;
         else imGantt->Canvas->Pen->Color = clBlue;

         jcal= tcal[ixx];if ((jcal < 1) || (jcal>5)) jcal =1;
         //for (jx=ttot;jx<=(nbjours+ttot);jx++)
         basecal = deb_plus_tot[ixx]-1;
         for (jx=xx1;jx<xx2;jx++)
           {
            X1=jx*interval;
            X2 = X1+interval;

            basecal++;
            if (cal[jcal][basecal] == 'T')
             { imGantt->Canvas->MoveTo(X1,Y);
             imGantt->Canvas->LineTo(X2,Y);
             }
            else
             { kinterv = 5;  // interval / 5   *
               K1=X1+kinterv; K2=X1+2*kinterv;
               imGantt->Canvas->MoveTo(K1,Y);
               imGantt->Canvas->LineTo(K2,Y);
               K1=X1+3*kinterv; K2=X1+4*kinterv;
               imGantt->Canvas->MoveTo(K1,Y);
               imGantt->Canvas->LineTo(K2,Y);
               //K1=X1+kinterv; K2=X1+2*kinterv;
               //mGantt->Canvas->MoveTo(K1,Y);
               //imGantt->Canvas->LineTo(K2,Y);
             }

           }

         //imGantt->Canvas->MoveTo(X1,Y);
         //imGantt->Canvas->LineTo(X2,Y);

         Y= Y0 - 20;
         X1= xx1*interval;
         imGantt->Canvas->MoveTo(X1,Y);
         imGantt->Canvas->Pen->Color = clLime;
         imGantt->Canvas->Font->Color = clBlue;
         imGantt->Canvas->Brush->Color = clYellow;
         add_jour(AnPrev,MoisPrev,JourPrev,deb_plus_tot[ixx]);
         //sprintf(tmp,"%02d/%02d/%04d ",Jour,Mois,An);
         Texte = ""; AnsiString(tmp);
         Texte = Texte + av->Cells[2][ixx] + " (d=" + IntToStr(duree[ixx]) + ")";
         imGantt->Canvas->Brush->Style = bsClear;
         imGantt->Canvas->TextOut(X1,Y,Texte);

         // affichage des successeurs

         isucc=1;  Texte="  ";
         while ((su[ixx][isucc]!=0) && (isucc <PS))
            {
              if (su[ixx][isucc] != 9999)
              {
                Texte = Texte + IntToStr(su[ixx][isucc]);
                if (su_tl[ixx][isucc] == 0) Texte = Texte + " FD ";
                if (su_tl[ixx][isucc] == 1) Texte = Texte + " FF ";
                if (su_tl[ixx][isucc] == 2) Texte = Texte + " AM ";
                if (su_tl[ixx][isucc] == 3) Texte = Texte + " DD ";
               Texte = Texte + IntToStr(su_decal[ixx][isucc])+ "  ";
              }
            isucc++;
           }
         Y = Y0 - 3;
         //imGantt->Canvas->MoveTo(X2+8,Y);
         imGantt->Canvas->Brush->Color=clWhite;
         imGantt->Canvas->Pen->Color=clBlack;
         imGantt->Canvas->Brush->Style = bsClear;
         imGantt->Canvas->TextOut(X2,Y,Texte);
         imGantt->Canvas->Pen->Width = 1;
     }  // if X1 > 0
   }
 DrawMiniGantt(task,pos,immGantt,false);
}

void __fastcall TplStandard::DrawMiniGantt(int task,int pos,TImage *imgg,bool reel)
{
  int nbjours;
  int ix,ixx,tdeb,tfin,numsucc;
  int xbase,x1base,ttot;
  int interval;
  char tmp[220];  // ����
  int  X1,X2,Y,xx1,xx2;
  int isucc;
  int firstday;
  AnsiString Texte;
  TDateTime dtFirst;
  int jx,jcal;
  int kx,kinterv,K1,K2;
  int basecal; int YA;
  int xtask,hy,Lim1,Lim2;
  int row_succ_pred;
  int idateplustot,ipred,i,isel,indx,indice;
  int idateplustard;
  int modcal,delta;
  AnsiString mo[13];
  AnsiString MO[13];
  AnsiString mon[13];
  int disttot,disttard;
  AnsiString RetTotal,RetImput;  int xrow;
  int XDEB;

  mo[1]="Jan";  mo[2]="Fev"; mo[3]="Mar";  mo[4]="Avr"; mo[5]="Mai";  mo[6]="Jui";
  mo[7]="Jui";  mo[8]="Aou"; mo[9]="Sep";  mo[10]="Oct"; mo[11]="Nov";  mo[12]="Dec";

  MO[1]="Janvier";  MO[2]="F�vrier"; MO[3]="Mars";  MO[4]="Avril"; MO[5]="Mai";  MO[6]="Juin";
  MO[7]="Juillet";  MO[8]="Aout"; MO[9]="Septembre";  MO[10]="Octobre"; MO[11]="Novembre";  MO[12]="D�cembre";

  mon[1]="01";  mon[2]="02"; mon[3]="03";  mon[4]="04"; mon[5]="05";  mon[6]="06";
  mon[7]="07";  mon[8]="08"; mon[9]="09";  mon[10]="10"; mon[11]="11";  mon[12]="12";


  dtFirst = EncodeDate(AnPrev,MoisPrev,JourPrev);
  firstday = dtFirst.DayOfWeek();
 // int dow;  // day of week
 // dow = firstday-1;

  if (nb_taches==0) return;
   // rechercher les predecesseurs et successeurs de TASK !

 for (i=0;i<MAX_T;i++)
   selection[i]=0;

 isel=1;
 ipred=1;
 idateplustot= deb_plus_tot[task];
 if (!reel) idateplustard=fin_plus_tot[task];
   else
    {
     // $$$$$
     idateplustard=fin_plus_tot[task];

    }
 while ((pr[task][ipred]!=0) && (ipred<PS))
        { indx =  pr[task][ipred]; indice=indx; //exist_tache(indx,nb_taches);
          if (deb_plus_tot[indice]<idateplustot) idateplustot=deb_plus_tot[indice];
          if (fin_plus_tot[indice]>idateplustard) idateplustard=fin_plus_tot[indice];
          selection[isel]=indx; ipred++; isel++;
        }
 selection[isel]=task; isel++;
 isucc=1;
 while ((su[task][isucc]!=0) && (isucc<PS))
         {indx = su[task][isucc];
          if (su[task][isucc] != 9999)
            {
              indice=indx; //exist_tache(indx,nb_taches);
              if (deb_plus_tot[indice]<idateplustot) idateplustot=deb_plus_tot[indice];
              if (fin_plus_tot[indice]>idateplustard) idateplustard=fin_plus_tot[indice];
              selection[isel]=indx;
              isel++;
            }
          isucc++;
         }
   BgColor=clWhite;
   FgColor=clLime;
   TxColor=clYellow;


 Diagram = Rect(0,0, imgg->Width,imgg->Height);
 imgg->Canvas->Brush->Color = clWhite; // BgColor;
 imgg->Canvas->FillRect(Diagram);

 Diagram = Rect(0,0,imgg->Width,imgg->Height/8);
 imgg->Canvas->Brush->Color = clAqua; //
 imgg->Canvas->FillRect(Diagram);

 // trac� du rectangle du haut;
 imgg->Canvas->Brush->Color = clBlack; //
 imgg->Canvas->MoveTo(0,0);
 imgg->Canvas->LineTo(imgg->Width-1,0);
 imgg->Canvas->LineTo(imgg->Width-1,imgg->Height/16);
 imgg->Canvas->LineTo(0,imgg->Height/16);
 imgg->Canvas->LineTo(0,0);
 imgg->Canvas->LineTo(0,imgg->Height);
 //  Trac� du compte de jours

 if (nb_taches==0) return;

 nbjours = deb_plus_tot[LASTTASK+1]; //nb_taches+1];

 modcal=1;  // Jours Semaines
 //if (cbEchelle->Text=="Semaines/Mois") modcal=2;
 //if (cbEchelle->Text=="Mois/Ann�es") modcal=3;


  if (nbjours != 0) interval = (imgg->Width / nbjours)*0.90;
   else interval = 100;
 imgg->Canvas->Font->Color = clBlack;

 //interval=30;
 delta = idateplustard-idateplustot;
 if (delta>=0)
   {
    if (delta==0) delta=1;
    if (delta>=80 && delta<=400) modcal=2;
    if (delta<80) modcal=1;
    if (delta>400) modcal=3;

    interval = 80*imgg->Width/(100*delta);
    if (interval > 100) interval = 100;
    if (interval==0) interval=1;

    }

  ttot=idateplustot;   //deb_plus_tot[xtask];
 // adjust with position
 // position is relative to the middle (50 %)
 ttot = ttot + int (40*pos/200);     //  30/2
 if (ttot <0) ttot=0;
 //$$$ScrollBar2->Position = 50 + pos;
 //$$$if (nb_taches>0) ScrollBar1->Position = 100 * xtask / nb_taches;

 imgg->Canvas->Font->Color = clBlack;
 imgg->Canvas->MoveTo(0,imgg->Height/8);
 imgg->Canvas->LineTo(imgg->Width,imgg->Height/8);

 // fix� a l'aide de calendrier 1;  //////////////////
 for (ix=ttot;ix<=(nbjours+ttot)*150/100;ix++)    // tenir compte du calendrier
   {
    xbase=(ix-ttot)*interval +interval/3;
    sprintf (tmp,"%d",ix+1);       // commence � 1, pas a z�ro
    add_jour(AnPrev,MoisPrev,JourPrev,ix);
    if (modcal==1)
        {
        sprintf(tmp, "%02d",Jour);
        imgg->Canvas->Font->Height = 6;
        imgg->Canvas->Font->Color = clBlack;
        imgg->Canvas->Brush->Color = clAqua;  // White;
        imgg->Canvas->Brush->Style = bsClear;
        imgg->Canvas->TextOut(xbase,3,(AnsiString) tmp);
        imgg->Canvas->Font->Color = clBlack;
        imgg->Canvas->MoveTo(ix*interval,0);
        imgg->Canvas->LineTo(ix*interval,imgg->Height/16);  //20);
       }

     if (modcal==2)
       {
        dtFirst = EncodeDate(An,Mois,Jour);
        firstday = dtFirst.DayOfWeek();
        if (firstday==2)
         {
          sprintf(tmp, "%02d %s",Jour,mo[Mois]);
          imgg->Canvas->Font->Height = 6;
          imgg->Canvas->Font->Color = clBlack;
          imgg->Canvas->Brush->Color = clAqua;  // White;
          imgg->Canvas->Brush->Style = bsClear;
          imgg->Canvas->TextOut(xbase+10,3,(AnsiString) tmp);
          imgg->Canvas->Font->Color = clBlack;
          imgg->Canvas->MoveTo(ix*interval,0);
          imgg->Canvas->LineTo(ix*interval,imgg->Height/16);  //20);
         }

        }
      if (modcal==3)
       {
        if (Jour==1)
         {
          sprintf(tmp, "%02d",Mois);
          imgg->Canvas->Font->Height = 6;
          imgg->Canvas->Font->Color = clBlack;
          imgg->Canvas->Brush->Color = clAqua;  // White;
          imgg->Canvas->Brush->Style = bsClear;
          imgg->Canvas->TextOut(xbase+10,3,(AnsiString) tmp);
          imgg->Canvas->Font->Color = clBlack;
          imgg->Canvas->MoveTo(ix*interval,0);
          imgg->Canvas->LineTo(ix*interval,imgg->Height/16);  //20);
         }

       }

    // trac� des conges
    xbase= (ix-ttot+1)*interval;
        if (cal[1][ix]==0) cal[1][ix]='N';
        if (cal[1][ix]=='N')
      {
       x1base  =  xbase - interval+1;
       Diagram = Rect(x1base,(imgg->Height/8)+1,xbase,imgg->Height);
       imgg->Canvas->Brush->Color = clSilver; //
       imgg->Canvas->FillRect(Diagram);
      }

     if (modcal==1)
       {
        imgg->Canvas->Brush->Color = clWhite;
        imgg->Canvas->Pen->Style = psDot;  // Dash;
        imgg->Canvas->MoveTo(xbase,imgg->Height/8);
        imgg->Canvas->LineTo(xbase,imgg->Height);
        imgg->Canvas->Pen->Style = psSolid;
        dtFirst = EncodeDate(An,Mois,Jour);
        firstday = dtFirst.DayOfWeek();
        if (firstday == 2)  // Mondays
          {
            // trace d'un trait vertical noir
            x1base  =  xbase - interval;
            sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An % 100);
            imgg->Canvas->Font->Color = clBlack;
            imgg->Canvas->Brush->Color = clAqua;  // White;
            imgg->Canvas->Brush->Style = bsClear;
            imgg->Canvas->TextOut(x1base+5,(imgg->Height/16)+3,(AnsiString) tmp);
            imgg->Canvas->MoveTo(x1base,imgg->Height/16);
            imgg->Canvas->LineTo(x1base,imgg->Height);
           }
       }
     if (modcal==2)
       {
         if (Jour==1)  // Debut de Mois
          {
            // trace d'un trait vertical noir
            x1base  =  xbase - interval;
            sprintf(tmp,"%s/%04d",mo[Mois],An);
            imgg->Canvas->Font->Color = clBlack;
            imgg->Canvas->Brush->Color = clAqua;  // White;
            imgg->Canvas->Brush->Style = bsClear;
            imgg->Canvas->TextOut(x1base+5,(imgg->Height/16)+3,(AnsiString) tmp);
            imgg->Canvas->MoveTo(x1base,imgg->Height/16);
            imgg->Canvas->LineTo(x1base,imgg->Height);
           }
       }
     if (modcal==3)
       {
         if (Jour==1 && Mois==1)  // Debut de Mois
          {
            // trace d'un trait vertical noir
            x1base  =  xbase - interval;
            sprintf(tmp,"%04d",An);
            imgg->Canvas->Font->Color = clBlack;
            imgg->Canvas->Brush->Color = clAqua;  // White;
            imgg->Canvas->Brush->Style = bsClear;
            imgg->Canvas->TextOut(x1base+5,(imgg->Height/16)+3,(AnsiString) tmp);
            imgg->Canvas->MoveTo(x1base,imgg->Height/16);
            imgg->Canvas->LineTo(x1base,imgg->Height);
           }
        }

     if (Jour==1)  // 1er du mois, un trait rouge;
       {
        x1base  =  xbase - interval+1;
        imgg->Canvas->Pen->Color = clRed;
        imgg->Canvas->MoveTo(x1base,imgg->Height/8);
        imgg->Canvas->LineTo(x1base,imgg->Height);
        imgg->Canvas->Pen->Color = clBlack;
       }

   }

 //14 taches par ecran   // 30
 tdeb=selection[1]; tfin=tdeb+20; if (tfin>LASTTASK) tfin=LASTTASK;
 if (isel>1) hy = 90*imgg->Height/(isel*100);
  else hy = 25;
  if (hy>50) hy=50;

  if (!reel) hy = imgg->Height / 12;  // 10 + 2
  else hy = imgg->Height /6;

  Lim1 = SB1->Position * 10 + 1;
  if (Lim1 > isel) Lim1 = isel-10;
  if (Lim1<1) Lim1=1;
  Lim2 = Lim1 + 10;
  if (Lim2 > isel)  Lim2 = isel;


 for (ixx=Lim1;ixx<Lim2;ixx++)
   {
    indice = selection[ixx];
    // calcul des coordonnees
    xx1 = deb_plus_tot[indice]-ttot;
    xx2=fin_plus_tot[indice]+1-ttot;
    X1= xx1*interval;
    X2= xx2*interval;

    // Y = ixx*hy + 35;  //25+ 30;

    Y = (ixx-Lim1)*hy + 50;
    if (X2>=0)
       { imgg->Canvas->Pen->Width = 4;
         if (su_tl[indice][1] == 2) imgg->Canvas->Pen->Color = clGreen;
        // else if (marg_tot[indice]==0) imgg->Canvas->Pen->Color = clRed;
         else if (av->Cells[29][indice]==0) imgg->Canvas->Pen->Color = clRed;

         else if(AdvStringGrid1->Cells[8][indice]==0){
         imgg->Canvas->Pen->Color = (TColor)RGB(253,0,0);}else{
         imgg->Canvas->Pen->Color = clWhite;
         imgg->Canvas->Pen->Color = clBlue; }

         jcal= tcal[indice];if ((jcal < 1) || (jcal>5)) jcal =1;
         //for (jx=ttot;jx<=(nbjours+ttot);jx++)
         basecal = deb_plus_tot[indice]-1;
         for (jx=xx1;jx<xx2;jx++)
           {
            X1=jx*interval+1;
            X2 = X1+interval;

            basecal++;
            if (cal[jcal][basecal] == 'T')
             { imgg->Canvas->MoveTo(X1,Y);
             imgg->Canvas->LineTo(X2,Y);
             }
            else
             { kinterv = 5;  // interval / 5   *
               K1=X1+kinterv; K2=X1+2*kinterv;
               imgg->Canvas->MoveTo(K1,Y);
               imgg->Canvas->LineTo(K2,Y);
               K1=X1+3*kinterv; K2=X1+4*kinterv;
               imgg->Canvas->MoveTo(K1,Y);
               imgg->Canvas->LineTo(K2,Y);
             }

           }
         if (reel)
           {
             // rechercher row pour tache indice
             row_succ_pred = Row_AsAvanc(indice);

             disttot = Convert_Date(asAvanc->Cells[3][row_succ_pred]);
             disttard = Convert_Date(asAvanc->Cells[5][row_succ_pred]);



             xx1 = disttot-ttot;
             xx2 = disttard+1-ttot;
             X1= xx1*interval;
             X2= xx2*interval;

             if (su_tl[indice][1] == 2) imgg->Canvas->Pen->Color = clGreen;
             //else if (marg_tot[indice]==0) imgg->Canvas->Pen->Color = clGreen;
             else if (av->Cells[29][indice]==0) imgg->Canvas->Pen->Color = clGreen;
             else imgg->Canvas->Pen->Color = clLime;


             // calculer dist pour  debut au plus tot de la date
             //  basecal = deb_plus_tot[indice]-1;

             basecal = disttot-1;
             jx=xx1; XDEB=jx*interval+1;
             for (jx=xx1;jx<xx2;jx++)
               {

                 YA = Y+10;
                 X1=jx*interval+1;
                 X2 = X1+interval;
                 basecal++;
                 if (cal[1][basecal] == 'T')
                    { imgg->Canvas->MoveTo(X1,YA);
                      imgg->Canvas->LineTo(X2,YA);
                    }
                else
                    { kinterv = 5;  // interval / 5   *
                      K1=X1+kinterv; K2=X1+2*kinterv;
                      imgg->Canvas->MoveTo(K1,YA);
                      imgg->Canvas->LineTo(K2,YA);
                      K1=X1+3*kinterv; K2=X1+4*kinterv;
                      imgg->Canvas->MoveTo(K1,YA);
                      imgg->Canvas->LineTo(K2,YA);
                    }
                }
            // trac� de Retard Total et Retard Imputable

            xrow = TrouveRow(indice);
            if (xrow !=0)
             {
               RetTotal = asAvanc->Cells[7][xrow];    av->Cells[25][indice]=RetTotal;
               RetImput = asAvanc->Cells[6][xrow];    av->Cells[24][indice]=RetImput;
               Texte = "";
           //    if (RetTotal!="0") Texte = Texte + "RT"+RetTotal;
           //    if (RetImput!="0") Texte = Texte + "   RI" + RetImput;
               imgg->Canvas->Brush->Style = bsClear;
             //  imgg->Canvas->TextOutA(X2+5,YA-6,Texte);


               Texte = asAvanc->Cells[1][xrow];
               imgg->Canvas->Brush->Style = bsClear;
               imgg->Canvas->TextOutA(XDEB-20,YA-6,Texte);


              }

           }   // if reel


         xx1 = deb_plus_tot[indice]-ttot;
         xx2=fin_plus_tot[indice]+1-ttot;
         X1= xx1*interval;
         X2= xx2*interval;
         Y= (ixx-Lim1)*hy + 35;
         X1= xx1*interval;
         imgg->Canvas->MoveTo(X1,Y);
         imgg->Canvas->Pen->Color = clLime;
         imgg->Canvas->Font->Color = clBlue;
         imgg->Canvas->Brush->Color = clYellow;
         add_jour(AnPrev,MoisPrev,JourPrev,deb_plus_tot[ixx]);
         //sprintf(tmp,"%02d/%02d/%04d ",Jour,Mois,An);
         Texte = ""; // AnsiString(tmp);
         //Texte = Texte + av->Cells[2][indice] + " (d=" + IntToStr(duree[indice]) + ")";
         Texte = Texte +  av->Cells[1][indice] + " " + av->Cells[2][indice];  // + " (d=" + IntToStr(duree[indice]) + ")";
         strcpy(tmp,av->Cells[8][indice].c_str());
         int mt=atoi(tmp);
         if (mt > 0) Texte=Texte + "  Mt = " + av->Cells[8][indice];
         imgg->Canvas->Font->Size = 7;  // pas d'effet
         imgg->Canvas->Brush->Style = bsClear;
         imgg->Canvas->TextOut(X1,Y,Texte);

         // affichage des successeurs

         isucc=1;  Texte="  ";
         while ((su[indice][isucc]!=0) && (isucc <PS))
            {
              numsucc = su[indice][isucc];
              if (numsucc != 9999 && flag_souhaites[numsucc] != 'Y')
              {
 //               Texte = Texte + IntToStr(su[indice][isucc]);
         //       if (su_tl[indice][isucc] == 0) Texte = Texte + " FD ";
         //       if (su_tl[indice][isucc] == 1) Texte = Texte + " FF ";
         //       if (su_tl[indice][isucc] == 2) Texte = Texte + " AM ";
         //       if (su_tl[indice][isucc] == 3) Texte = Texte + " DD ";
   //            Texte = Texte + IntToStr(su_decal[indice][isucc])+ "  ";
              }
            isucc++;
           }

         Y = (ixx-Lim1)*hy+ 40;
         imgg->Canvas->Brush->Color=clWhite;
         imgg->Canvas->Brush->Style = bsClear;
         imgg->Canvas->Pen->Color=clBlack;
         imgg->Canvas->Font->Height = 6;
         if (Texte != "  ")
             imgg->Canvas->TextOut(X2,Y,Texte);
         imgg->Canvas->Pen->Width = 1;
     }  // if X1 > 0
   }
   imgg->Refresh();
}


int __fastcall  TplStandard::Row_AsAvanc(int indx)
{
 int i,cnt; char tmp[50];
 int result;int numt;
 cnt = asAvanc->RowCount;
 result = 0;
 for (i=1;i<cnt;i++)
  {
   strcpy(tmp,asAvanc->Cells[1][i].c_str());
   numt = atoi(tmp);
   if (numt==indx) { result=i; break; }
  }
 return result;

}

 void __fastcall TplStandard::refresh_coches()
 {
  int i, ctr_coche,ctr_filt;
  // calculer le nombre de taches cochees et filtrees
  Label15->Caption = IntToStr(nb_taches);   //   NB_TACHES = nb efficace
  Label50->Caption = IntToStr(nb_taches);
  Label148->Caption = IntToStr(nb_taches);
  Label65->Caption = IntToStr(nb_taches);
  Label61->Caption = IntToStr(nb_taches);
  Label111->Caption = IntToStr(nb_taches);

  Label53->Caption = IntToStr(nb_taches);
  Label87->Caption = IntToStr(nb_taches);
  ctr_coche=0; ctr_filt=0;
  for (i=1;i<=LASTTASK;i++)
    {
     if  (av->Cells[1][i] !="")
       {
        if (coche[i]!=' ') ctr_coche++;
        if (filtr[i]=='X') ctr_filt++;
       }
    }

 Label16->Caption = IntToStr(ctr_coche);
 Label17->Caption = IntToStr(ctr_filt);
 Label67->Caption = IntToStr(ctr_filt);
 Label62->Caption = IntToStr(ctr_filt);

 Label113->Caption = IntToStr(ctr_filt);
}

int __fastcall TplStandard::Compute_Possible(int row,int ipred)
{
  int zcal,fd;
  int xtpred;


  xtpred=pr[row][ipred];
  /// LASTTASK $$$$ xtpred=exist_tache(xtpred,nb_taches);


  if (pr_tl[row][ipred]==0)   // lien FD
    {
     if (pr_decal[row][ipred]>0)
           {  zcal=tcal[row];
              fd = avance_conges(fin_plus_tot[xtpred]+1,zcal);
              fd = avance_temps(fd,zcal,pr_decal[row][ipred]);
              fd = avance_conges(fd,zcal);
           }
          else if (pr_decal[row][ipred]<0)
           { zcal=tcal[xtpred];
             fd = avance_conges(fin_plus_tot[xtpred]+1,zcal);
             fd = recule_temps(fd,zcal,-pr_decal[row][ipred]);      // -
           }
          else // =0
           {
            zcal=tcal[row];
            fd = avance_conges(fin_plus_tot[xtpred]+1,zcal);
            }
    }

  if (pr_tl[row][ipred]==1)     // c est un lien ff
   {
     if (pr_decal[row][ipred] > 0)
      { zcal=tcal[xtpred];
        fd = avance_conges(fin_plus_tot[xtpred]+1,zcal);
        fd = avance_temps(fd,zcal,pr_decal[row][ipred]);
        fd = avance_conges(fd,zcal); //�����
        fd = recule_temps(fd,zcal,duree[row]);
      }
     else if (pr_decal[row][ipred] < 0)
      { zcal=tcal[xtpred];
        fd=recule_conges(fin_plus_tot[xtpred]+1,zcal);
        fd=recule_temps(fd,zcal,-pr_decal[row][ipred]);
        fd = recule_temps(fd,zcal,duree[row]);
      }
     else
      {
        zcal= tcal[row];
        fd=recule_conges(fin_plus_tot[xtpred]+1,zcal);
        fd = recule_temps(fd,zcal,duree[row]);
       }
  }

if (pr_tl[row][ipred] == 2)  fd = -1;   //flag_amont = ipred;
if (pr_tl[row][ipred] == 3)    // Lien DD   Decal est >= 0
   {
     if ( pr_decal[row][ipred] > 0)
       { zcal=tcal[xtpred];
         fd=avance_conges(deb_plus_tot[xtpred],zcal);
         fd = avance_temps(fd,zcal,pr_decal[row][ipred]);
         fd = avance_conges(fd,zcal); //�������
       }
     else if ( pr_decal[row][ipred] == 0)
       { zcal=tcal[row];
         fd=avance_conges(deb_plus_tot[xtpred],zcal);
        } // pas de calcul de decalage
   }
 return fd;
}


void __fastcall TplStandard::Refresh_Pred_Succ(int task)
 {
  int  ipred, xtpred,isucc,is,prsu;
  int irow,icol;
  char tmp[100];
  int itask;   int nbpred,nbsucc;
  int indice,dista;
  int indx;
  int fdfd,fd,pretask,prepre,XIPRED;

  if (nb_taches==0) return;
  if (task==0) return;
  // nettoyer les 2 grilles


  current_task = task;


  Label11->Caption = AnsiString(current_task);  //current_task);
  lTEC->Caption = AnsiString(current_task);
  Clean_Grid(AdvStringGrid2,2);
  Clean_Grid(AdvStringGrid3,2);
  itask=current_task;
  if (itask != 9999)
   {
   sprintf(tmp,"Pr�d�cesseurs de la t�che %d",itask);
   Label3->Caption = AnsiString(tmp);
   sprintf(tmp,"Successeurs de la t�che %d",itask);
   Label4->Caption = AnsiString(tmp);
   }
   else
    {
     Label3->Caption = "Pr�d�cesseurs";
     Label4->Caption = "Successeurs";
    }
  // afficher les predecesseurs;
  ipred=1;

  nbpred=0;
  while ((pr[itask][ipred]!=0) && (ipred<PS))
         {                                         // ���������������������
          indx =  pr[itask][ipred];

          if (ipred > 1) AdvStringGrid2->RowCount++;
          AdvStringGrid2->Colors[4][ipred] = TColor(RGB(255,255,153));
          AdvStringGrid2->Colors[7][ipred] = TColor(RGB(255,255,153));

          AdvStringGrid2->Cells[1][ipred]=pr[itask][ipred];           // taches
          AdvStringGrid2->Cells[2][ipred]=av->Cells[2][indx]; //[indice];    // designation
          AdvStringGrid2->Cells[3][ipred]=AnsiString(duree[indx]); //indice];    //duree

          fd = Compute_Possible (itask,ipred); //(itask,indx,ipred);

          add_jour(AnPrev,MoisPrev,JourPrev,fd);
          sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
          AdvStringGrid2->Cells[4][ipred]=AnsiString(tmp);

          if (pr_tl[itask][ipred]==0) AdvStringGrid2->Cells[5][ipred]="FD";
           else if (pr_tl[itask][ipred]==1) AdvStringGrid2->Cells[5][ipred]="FF";
           else if (pr_tl[itask][ipred]==2) AdvStringGrid2->Cells[5][ipred]="AM";
           else if (pr_tl[itask][ipred]==3) AdvStringGrid2->Cells[5][ipred]="DD";
           prsu = pr_decal[itask][ipred];
          AdvStringGrid2->Cells[6][ipred]=AnsiString(prsu);     // decal
          AdvStringGrid2->Cells[7][ipred]=av->Cells[8][indx];
          ipred++;
          nbpred++;
         }
  lnbPred->Caption = nbpred;

 /*
 AdvStringGrid2->SortIndexes->Clear();
 AdvStringGrid2->SortIndexes->Add(4);
 AdvStringGrid2->QSortIndexed();
 // AdvStringGrid2->QSort();
 */

  // afficher les successeurs;

  nbsucc=0;
  isucc=1; is=1;
  while ((su[itask][isucc]!=0) && (isucc<PS))
         {
          indx = su[itask][isucc];
          //indice = exist_tache(indx,nb_taches);
          indice=indx;
          // ne pas afficher si tache END
          if (su[itask][isucc] != 9999)
            {                          // ����������������������������
              if (isucc>1) AdvStringGrid3->RowCount++;

              AdvStringGrid3->Colors[4][is] = TColor(RGB(255,255,153));
              AdvStringGrid3->Colors[7][is] = TColor(RGB(255,255,153));
              AdvStringGrid3->Colors[8][is] = TColor(RGB(255,255,153));
              AdvStringGrid3->Cells[1][is]=su[itask][isucc];           // taches
              AdvStringGrid3->Cells[2][is]=av->Cells[2][indice];          // designation
              AdvStringGrid3->Cells[3][is]=duree[indice];    //duree


              add_jour(AnPrev,MoisPrev,JourPrev,deb_plus_tot[indice]);
              sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
              AdvStringGrid3->Cells[4][is]=AnsiString(tmp);

              // AdvStringGrid3->Cells[4][is]=deb_plus_tot[indice]+1;
              if (su_tl[itask][isucc]==0) AdvStringGrid3->Cells[5][is]="FD";
              else if (su_tl[itask][isucc]==1) AdvStringGrid3->Cells[5][is]="FF";
              else if (su_tl[itask][isucc]==2) AdvStringGrid3->Cells[5][is]="AM";
              else if (su_tl[itask][isucc]==3) AdvStringGrid3->Cells[5][is]="DD";
              AdvStringGrid3->Cells[6][is]=AnsiString(su_decal[itask][isucc]);
              AdvStringGrid3->Cells[7][isucc]=av->Cells[8][indx];

              //  chercher les successeurs possibles de indx

              prepre = 1;
              while ((pr[indx][prepre]!=0) && (prepre<PS))
                {
                  pretask = pr[indx][prepre];
                  if (pretask == itask)
                    {
                     XIPRED = Find_Index_Pred(indx,pretask);
                     if (XIPRED !=0)
                      {
                       fd = Compute_Possible(indx,XIPRED);
                       add_jour(AnPrev,MoisPrev,JourPrev,fd);
                       sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
                       AdvStringGrid3->Cells[8][isucc]=AnsiString(tmp);
                      }
                    }

                 /*
                 pretask = pr[indx][sucsuc];
                 XIPRED = Find_Index_Pred(suctask,indx);
                    sstask = pr[suctask][XIPRED];
                    if (XIPRED !=0 && sstask==indx)
                     { fd = Compute_Possible(suctask,XIPRED);
                       if (fd<fdfd)
                         {fdfd = fd;
                          add_jour(AnPrev,MoisPrev,JourPrev,fdfd);
                          sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
                          AdvStringGrid3->Cells[8][isucc]=AnsiString(tmp);
                         }
                     }
                   }
                 */

                 prepre++;
                }


            is++;
            nbsucc++;
            }

          isucc++;

         }
/*
AdvStringGrid3->SortIndexes->Clear();
AdvStringGrid3->SortIndexes->Add(8);
AdvStringGrid3->QSortIndexed();
//AdvStringGrid3->QSort();
*/

 lnbSucc->Caption = nbsucc;
 refresh_coches();
}
//---------------------------------------------------------------------------

int  __fastcall TplStandard::Find_Index_Pred(int indx,int itask)
{
 int i;
 int start;
 int tache;
 int result;
 bool found;

 result = 0;
 start=1;
  while ((pr[indx][start]!=0) && (start<PS))
         {
           tache =  pr[indx][start];
          if (tache == itask)
            {
             // found = true;
             result = start;
             break;
            }
          start++;
         }

return result;
}

int __fastcall  TplStandard::comp_dist(int indx,int xtsucc, int isucc)
{
 int zcal,xtotal,flag_amont,fduree;
 if (su_tl[indx][isucc]==0)     //  c est un lien fd
     {
       xtotal = fin_plus_tot[indx];
       if (su_decal[indx][isucc] > 0)
                     { zcal=tcal[indx];  // xtsucc];
                       xtotal=avance_conges(fin_plus_tot[indx]+1,zcal);
                       xtotal=avance_temps(xtotal,zcal,su_decal[indx][isucc]);
                     }
       else if (su_decal[indx][isucc] < 0)
                     {
                      zcal=tcal[indx]; //xtsucc];
                      xtotal=recule_conges(fin_plus_tot[indx],zcal);
                      xtotal=recule_temps(xtotal,zcal,-su_decal[indx][isucc]);
                     }
      else
                     {
                      zcal=tcal[indx];
                      xtotal=avance_conges(fin_plus_tot[indx]+1,zcal);
                     }
                  }
    if (su_tl[indx][isucc]==1)     //  c est un lien ff
                  {
                   if (su_decal[indx][isucc] > 0)
                     {zcal=tcal[indx]; //  indx]; // xtsucc];
                      xtotal=avance_conges(fin_plus_tot[indx]+1,zcal);
                      xtotal=avance_temps(xtotal,zcal,su_decal[indx][isucc]);
                      xtotal=recule_temps(xtotal,zcal,duree[xtsucc]);
                     }
                   else if (su_decal[indx][isucc] < 0)
                     {zcal=tcal[indx];  //  indx]; //xtsucc];
                      xtotal=recule_conges(fin_plus_tot[indx]+1,zcal);
                      xtotal=recule_temps(xtotal,zcal,su_decal[indx][isucc]);
                      // xtotal = avance_conges(xtotal,zcal); //�������
                      xtotal=recule_temps(xtotal,zcal,duree[xtsucc]);
                     }
                   else
                     {zcal=tcal[indx];
                      xtotal=recule_conges(fin_plus_tot[indx]+1,zcal);
                      xtotal=recule_temps(xtotal,zcal,duree[xtsucc]);
                     }
                  }

   if (su_tl[indx][isucc]==3)     //  c est un lien DD
                  {
                   if (su_decal[indx][isucc] > 0)
                     { zcal=tcal[indx];  // xtsucc];
                       xtotal=avance_conges(deb_plus_tot[indx],zcal);
                       xtotal=avance_temps(xtotal,zcal,su_decal[indx][isucc]);
                       xtotal = avance_conges(xtotal,zcal);
                     }
                   else if (su_decal[indx][isucc] == 0)
                     {zcal=tcal[indx];
                      xtotal = deb_plus_tot[indx];
                     }
                  }

 return xtotal;


}

AnsiString __fastcall TplStandard::Debut_Possible(int indx,int xtsucc, int isucc)
{ int dist1;
 AnsiString Possible;
 char tmp[250];  // �����
 Possible = "";
 dist1 = comp_dist(indx,xtsucc,isucc);
 add_jour(AnPrev,MoisPrev,JourPrev,dist1);
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);

 return AnsiString(tmp);
}


//---------------------------------------------------------------------------


void __fastcall TplStandard::InsertPredSeul(int tach,int tpred, int tl, int decal,bool disp)
{
 int ipred,found,indx,xpred,imax;

     if (tach==tpred)
      return;
    ipred=1;  found=0;
    if (new_nb_taches==0) new_nb_taches = nb_taches;  // PAS DE LASTTASK
    indx = tach; //exist_tache(tach,new_nb_taches);
    xpred=1;
    while ((pr[indx][ipred]!=0) && (ipred<PS)) // && (pr[tach][ipred]!=9999)) && (ipred<PS))
       {
        if (pr[indx][ipred] == tpred){found=1; xpred=ipred;}
        ipred++;
       }
    if (!found && (ipred > 1)) { xpred=ipred; pr[indx][ipred+1]=0; }
    if (ipred >= PS-1) Application->MessageBox("Insert Pred : Pred > PS ",m_ecoplan,MB_OK);
    pr[indx][xpred] = tpred;
    pr_tl[indx][xpred] = tl;
    pr_decal[indx][xpred] = decal;
    if (disp==true)
     {
       if (!found && (ipred>1))AdvStringGrid2->RowCount++;
     }
}

void __fastcall TplStandard::InsertSuccSeul(int tach,int tsucc,int tl, int decal,bool disp)
{
 int isucc;
 int dec,xsucc;
 int found,f9999;
 int indsucc,ind9999;
 int m; int tbl[PS];

 for (m=0;m<PS;m++) tbl[m]=0;
 for (m=0;m<PS;m++) tbl[m]=su[tach][m];
 if (tach==tsucc)
 return;

 if (new_nb_taches==0) new_nb_taches = nb_taches;
 dec= tach;  // exist_tache(tach,new_nb_taches);
 xsucc= tsucc; // =exist_tache(tsucc,new_nb_taches);
 if (dec==0) return;
 if (xsucc==0) return;

 if (su[dec][1]==0)
    su[dec][1]=9999;

    ind9999=0;
    indsucc=1;
     isucc=1; found=0; f9999=0;
    while ((su[dec][isucc]!=0)  && (isucc<PS))
       { if (su[dec][isucc] == tsucc) { found=1; indsucc= isucc; }
         if (su[dec][isucc]==9999) { f9999=1; ind9999=isucc; }
         isucc++;
       }

    if (f9999==0)
       { su[dec][isucc]=9999; ind9999=isucc; }

    if (isucc >= PS-2) { Application->MessageBox("Insert Successeur : Trop de successeur (>300)",m_ecoplan,MB_OK);

        return;  }
    // isucc pointe sur le premier libre 0;
      if (found==1)
       {
        su[dec][indsucc] = xsucc;
        su_tl[dec][indsucc] = tl;
        su_decal[dec][indsucc] = decal;
       //  su[dec][ind999insucc+1] =9999;
       }
      else
       {
        su[dec][ind9999] = xsucc;
        su_tl[dec][ind9999] = tl;
        su_decal[dec][ind9999] = decal;
        su[dec][ind9999+1] =9999;
       }
/*
       if (f9999==1)
        {
       su[dec][ind9999] = xsucc;
       su_tl[dec][ind9999] = tl;
       su_decal[dec][ind9999] = decal;
       su[dec][ind9999+1] =9999;
        }
      else
        {
         su[dec][indsucc] = xsucc;
         su_tl[dec][indsucc] = tl;
         su_decal[dec][indsucc] = decal;
         su[dec][insucc+1] =9999;

        }
*/
    if (disp==true)

     { if  (isucc>1) AdvStringGrid3->RowCount++; }

  for (m=0;m<PS;m++) tbl[m]=0;
 for (m=0;m<PS;m++) tbl[m]=su[tach][m];

}



//---------------------------------------------------------------------------


void __fastcall TplStandard::Effacer1Click(TObject *Sender)
{

 char tmp[100];
 // Effacer Tache   en cours
 if (current_task==0) return;
 sprintf(tmp,"Confirmer effacement de la t�che %d",current_task);
 if (Application->MessageBox(tmp,m_ecoplan,MB_YESNO) ==IDNO) return;
 effacer_tache(current_task,SG1Row);


 Tout_Decocher();

 Change1();


 current_task=Lect_Numt(1);   // &&&&&
 Remplir_asTri();
 Remplir_AV1();
 if (nb_taches==0) return;
 
 grefresh(current_task,'1');



}
//---------------------------------------------------------------------------




void __fastcall TplStandard::Inserer1Click(TObject *Sender)
{
 // click sur Inserer Tache
 int a,b,nt;
 int indx,k;
 char tmp[250];

 a=current_task;
 if (nb_taches==0)
 {
  Application->MessageBox("Pas de t�che existante - Utilisez la touche 'INSER'","Insertion T�che",MB_OK);
  return;
  }
 if (a==1)
 sprintf (tmp,"Insertion d'une t�che devant la t�che %d ? ", a);
  else
 sprintf (tmp,"Insertion d'une t�che entre les t�ches %d et %d ? ", a-1,a);

 if (Application->MessageBox(tmp,"Insertion T�che",MB_YESNO) == IDNO) return;

    // chercher premier numero de tache libre
    nt = next_task();
    sprintf(tmp,"Nlle Tache %d",nt);
    for (indx=nb_taches;indx>=a;indx--)
      {
       coche[indx+1]=coche[indx];
       coch[indx+1]=coch[indx];
       sommets[indx+1]=sommets[indx];
       filtr[indx+1]=filtr[indx];
       duree[indx+1]=duree[indx];
       duree_reelle[indx+1]=duree_reelle[indx];
       strcpy(codetache[indx+1],codetache[indx]);
       strcpy(da[indx+1],da[indx]);
       strcpy(dtr[indx+1],dtr[indx]);
       strcpy(ftr[indx+1],ftr[indx]);
       pca[indx+1]=pca[indx];
       nbj[indx+1]=nbj[indx];
       strcpy(tmom[indx+1],tmom[indx]);
       tcal[indx+1]=tcal[indx];


       // maj des predecesseurs et successeurs

       for (k=0;k<PS;k++)
         { pr[indx+1][k] = pr[indx][k];
           pr_tl[indx+1][k] = pr_tl[indx][k];
           pr_decal[indx+1][k] = pr_decal[indx][k];
           su[indx+1][k] = su[indx][k];
           su_tl[indx+1][k] = su_tl[indx][k];
           su_decal[indx+1][k] = su_decal[indx][k];
         }

       AdvStringGrid1->Cells[1][indx+1]=AdvStringGrid1->Cells[1][indx];
       av->Cells[1][indx+1]=av->Cells[1][indx];
       AdvStringGrid1->Cells[2][indx+1]=AdvStringGrid1->Cells[2][indx];
       av->Cells[2][indx+1]=av->Cells[2][indx];
       AdvStringGrid1->Cells[3][indx+1]=AdvStringGrid1->Cells[3][indx];
       av->Cells[3][indx+1]=av->Cells[3][indx];
      } // for indx

   // insertion de la tache en indice "a"
   sommets[a]=nt;
   coche[a]=' ';
   coch[a]=1; //' ';
   duree[a]=1;
   duree_reelle[a]=1;
   filtr[a]='X';
   tcal[a]=1;
   tmom[a][0]=0;
   codetache[a][0]=0;
   for (k=0;k<PS;k++)
     { pr[a][k]=0; pr_tl[a][k]=0; pr_decal[a][k]=0;
       su[a][k]=0; su_tl[a][k]=0; su_decal[a][k]=0;
     }
   su[a][1]=9999;
   sprintf(tmp,"Nlle Tache %d",nt);
   AdvStringGrid1->Cells[1][a]=nt;
   av->Cells[1][a]=nt;
   AdvStringGrid1->Cells[2][a]=AnsiString(tmp);
   av->Cells[2][a]=AnsiString(tmp);
   AdvStringGrid1->Cells[3][a]=1;
   av->Cells[3][a]=1;

   AdvStringGrid1->Colors[1][a] =   TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[4][a] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[5][a] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[6][a] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[7][a] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[10][a] = TColor(RGB(255,255,153));


   if (a>1) { AdvStringGrid1->RowCount++; av->RowCount++; }
   sprintf(tmp,"Creation d'une nouvelle t�che %d",nt);
   Application->MessageBox(tmp,"Insertion T�che",MB_OK);

   nb_taches++;
   
   grefresh(current_task,'1');
}
//---------------------------------------------------------------------------

int __fastcall TplStandard::next_task()
{
 int indx,it;
 it=0;
 for (indx=1;indx<=nb_taches;indx++)
   {
    if (sommets[indx] > it) it = sommets[indx];
   }
 it++;
 return it;
}

void __fastcall TplStandard::Tout_Cocher()
{
 int i,ix;
 char tmp[150];   // ����
 int cntrow;
 cntrow=AdvStringGrid1->RowCount -1;


 //for (i=1;i<=nb_taches;i++) // ���������������
   for (ix=1;ix<=cntrow;ix++)

   {
     strcpy(tmp,AdvStringGrid1->Cells[1][ix].c_str());
     i=atoi(tmp);
     coche[i] = 'X';
     AdvStringGrid1->Cells[0][ix]=AnsiString(strcoche);  //"X";
   }
 refresh_coches();
}


void __fastcall TplStandard::Tout_Decocher()
{
 int i,ix;
 char tmp[50];
 int cntrow;
 cntrow=AdvStringGrid1->RowCount -1;

for (i=0;i<MAX_T;i++) coche[i]=' ';

for (ix=1;ix<=cntrow;ix++)
   {
     //strcpy(tmp,AdvStringGrid1->Cells[1][ix].c_str());
     //i=atoi(tmp);
     //coche[i]=' ';
    AdvStringGrid1->Cells[0][ix]=" ";
   }
 refresh_coches();
}

void __fastcall TplStandard::Copier_Cocher()
{
 TCopy *CTask; int cascade,decalage,tl,tlien;
 int oldcoche;
 int newcoche;
 int nexttache;
 int ipred,tpred,decal,nt,isucc,tsucc;
 int numtachpred,decalpred,tlpred,indpred;
 int bcl,cntrow,ix;
 int count,i,index;
 //int first;
 int xnb_taches;
 int nb_coches; int xndex;
 int cnt;
 char tmp[500];   int rowcount;

 char ps[5000];



     CTask = new TCopy(Application);
     CTask->ShowModal();
     delete CTask;

     Ghost->ExtractValue(tmp,m_exchange,"null",0);
     if (strcmp(tmp,"TRUE")==0) return;

     nexttache=0; nb_coches=0;
     cntrow=AdvStringGrid1->RowCount-1;



     for (i=1;i<=nb_taches;i++)
        { if (sommets[i]>nexttache) nexttache=sommets[i];
          if (coche[sommets[i]]=='X') nb_coches++;
        }


     //  preparer sommets [i] defacon a prendre en compte les nlles taches
     // nb taches cochees dans nb_coches;
     for (i=1; i <=nb_coches; i++) sommets[i+nb_taches] = nexttache + i;

     new_nb_taches = nb_taches + nb_coches;

     // decodage de exchange
     cascade=0; decalage=0; tl=0;
     Ghost->ExtractValue(tmp,m_exchange,"cascade",0);
     if (strcmp(tmp,"T")==0)
       { cascade=1;
         Ghost->ExtractValue(tmp,m_exchange,"decalage",0);
         decalage=atoi(tmp);
         Ghost->ExtractValue(tmp,m_exchange,"tl",0);
         tlien=atoi(tmp);
       }
     oldcoche=0; newcoche=0;
     Ghost->ExtractValue(tmp,m_exchange,"old",0); if (strcmp(tmp,"T")==0)  oldcoche=1;
     Ghost->ExtractValue(tmp,m_exchange,"new",0); if (strcmp(tmp,"T")==0)  newcoche=1;

   // compter les coches : message si dup > MAX_T
   count=0;





   xnb_taches=nb_taches;
   for (i=1;i<=xnb_taches;i++)if (coche[i]=='X') count++;
   if ((nb_taches+count) >= MAX_T)
    {
     Application->MessageBox("Nombre de t�ches trop �lev� ( > 3000 )",m_ecoplan,MB_OK);
     return;
    }
   //  maintenant, on y va, on duplique




   count=0;
   // Indicer en fonction du numero dans advstringgrid1
   cntrow=AdvStringGrid1->RowCount-1;

   for (i=1;i<MAX_T;i++) Association[i]=0;

   for (ix=1;ix<=cntrow;ix++)
     {
       strcpy(tmp,AdvStringGrid1->Cells[1][ix].c_str());
       i=atoi(tmp);
       if ((coche[i]=='X') && (su_tl[i][1] != 2))   // ne pas copier taches amont
       {
        count++;   Association[i] = nexttache + count;
       }
     }


     count=0;
     // GRANDE BOUCLE
     // xndx = rang�e ou afficher les nlles taches
     for (ix=1;ix<=cntrow;ix++)
     {
       strcpy(tmp,AdvStringGrid1->Cells[1][ix].c_str());

       i=atoi(tmp);
       if ((coche[i]=='X') && (su_tl[i][1] != 2))   // ne pas copier taches amont
       {
        count++;
        index= Association[i];  //  xnb_taches + count;
        nexttache++;

        AdvStringGrid1->RowCount++; av->RowCount++; asCrit->RowCount++;
        xndex = cntrow+count;
        duree[index]=duree[i];
        tcal[index] = tcal[i];

        filtr[index]=filtr[i];
        duree_reelle[index]=duree_reelle[i];
        strcpy(codetache[index],codetache[i]);
        strcpy(da[index],da[i]);
        strcpy(dtr[index],dtr[i]);
        strcpy(ftr[index],ftr[i]);

        // copie des crit�res
        av->Cells[41][index]=av->Cells[41][i]; av->Cells[51][index]=av->Cells[51][i];
        av->Cells[42][index]=av->Cells[42][i]; av->Cells[52][index]=av->Cells[52][i];
        av->Cells[43][index]=av->Cells[43][i]; av->Cells[53][index]=av->Cells[53][i];
        av->Cells[44][index]=av->Cells[44][i]; av->Cells[54][index]=av->Cells[54][i];

        // sommets[index]= nexttache;
        //  if (oldcoche==0)  {coche[i]=' '; AdvStringGrid1->Cells[0][ix]= " "; }
        AdvStringGrid1->Cells[1][xndex]= nexttache;
        av->Cells[1][index]= nexttache;
        av->Cells[2][index]= av->Cells[2][i]; //nexttache;
        av->Cells[3][index]= av->Cells[3][i];
        av->Cells[10][index]= av->Cells[10][i];


        asTri->Cells[1][xndex] = nexttache;

        for(bcl=0;bcl<PS;bcl++)
          {
           pr[index][bcl]=0;
           su[index][bcl]=0;
          }

        // COPIE DES CHAMPS

        for (bcl=2;bcl<20;bcl++)
          { if (bcl < 4 || bcl>9)
            {
            AdvStringGrid1->Cells[bcl][xndex]= AdvStringGrid1->Cells[bcl][ix];
            // av->Cells[bcl][index]= AdvStringGrid1->Cells[bcl][xndex];
            // asTri->Cells[bcl][xndex]= AdvStringGrid1->Cells[bcl][xndex]; //"Tache " + IntToStr(nexttache) + " ";
            }
          }



     AdvStringGrid1->Colors[1][xndex] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[4][xndex] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[5][xndex] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[6][xndex] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[7][xndex] = TColor(RGB(255,255,153));
     AdvStringGrid1->Colors[10][xndex] = TColor(RGB(255,255,153));


        // copie des criteres pour la nlle tache
        int j,ind;


        asCrit->Cells[1][xndex]= AnsiString(nexttache);
        asCrit->Cells[2][xndex]= AdvStringGrid1->Cells[2][index];
        for (j=1;j<9;j++)   // copie des colonnes
         {
          ind = j*2 + 1;
          asCrit->Cells[ind][xndex] = av->Cells[j+40][i];  // asCrit->Cells[ind][i];
          asCrit->Colors[ind][xndex] = TColor(RGB(255,255,153));
          av->Cells[j+40][xndex]    = av->Cells[j+40][i];  // asCrit->Cells[ind][i];
          ind++;
          asCrit->Cells[ind][xndex] = av->Cells[j+50][i];  // asCrit->Cells[ind][i];
          av->Cells[j+50][index]    = av->Cells[j+50][i];  // asCrit->Cells[ind][i];
         }
        


        nb_taches=nb_taches+1;

        if (cascade)
          {

              InsertPredSeul(index,i,tlien,decalage,false);
              InsertSuccSeul(i,index,tlien,decalage,false);
             

          }
         ipred=1; strcpy(ps,"|");

         while (pr[i][ipred] != 0 && ipred<PS)
                {
                  // inserer un predecesseur que si tache cochee
                  nt= pr[i][ipred];  //
                  if (coche[nt]=='X')
                   {
                      tl = pr_tl[i][ipred]; decal = pr_decal[i][ipred];
                      //tpred = exist_tache(nt,2*cntrow)+ cntroww; //nexttache - i + nt;
                      tpred= Association[nt];
                      sprintf(tmp,"%d|",tpred,tl,decal);
                      strcat(ps,tmp);
                      InsertPredSeul(nexttache,tpred,tl,decal,false);
                      InsertSuccSeul(tpred,nexttache,tl,decal,false);
                   }
                  ipred++;
                 } // while
               ipred--;
               av->Cells[61][index]=AnsiString(ipred);
               av->Cells[62][index]=AnsiString(ps);        // OK $$$$$$$
              isucc=1;strcpy(ps,"|");
              while ((su[i][isucc] != 0) && (isucc<PS))
                {
                  // inserer un successeur que si tache cochee
                  nt= su[i][isucc];  //
                  if (nt==9999)
                  {nt=0; coche[0]=' '; }

                  if (coche[nt]=='X')
                   {
                      tl = su_tl[i][isucc];
                      decal = su_decal[i][isucc];
                      // tsucc = exist_tache(nt,2*cntrow)+cntrow; //sommets[nt+ cntrow]; //nexttache - i + nt;
                      tsucc = Association[nt];
                      sprintf(tmp,"%d|",tsucc,tl,decal);
                      strcat(ps,tmp);

                      InsertSuccSeul(nexttache,tsucc,tl,decal,false);
                      InsertPredSeul(tsucc,nexttache,tl,decal,false);
                   }
                  isucc++;
                 } // while
               isucc--;



               su[nexttache][isucc]=9999;
               strcat(ps,"9999|");
               av->Cells[63][index]=AnsiString(isucc);
               av->Cells[64][index]=AnsiString(ps);        // OK $$$$$$$$$



      }  // if coche=X
     } // for

           // cocher-decocher en fin de traitement





  // MAJ DES COCHES D'ABORD


  // Open_Dump();

  rowcount = cntrow;
  count=0;
  // for (bcl=1;bcl<=xnb_taches;bcl++)
  for (bcl=1;bcl<=rowcount;bcl++)

     {
      i=Lect_Numt(bcl);
      if (coche[i]=='X')
       {
        count++;
        index= rowcount+count;
        // nexttache++;

        //sprintf(tmp,"BCL COCHE XNB_TACHES= %d, BCL=%d I= %d INDEX= %d SOMMET(INDEX]= %d\n",xnb_taches,bcl,i,index,sommets[index]);
        //fputs(tmp,dump);

        if (oldcoche==0)  {coche[i]=' '; AdvStringGrid1->Cells[0][bcl]= " "; }

        if (newcoche==1)
          { // coche[sommets[index]]='X';
            // coche[index]='X';
            nt = Lect_Numt(index);
            coche[nt]='X';
            AdvStringGrid1->Cells[0][index]=AnsiString(strcoche);  //"X";
          }
        else
          {
            // coche[sommets[index]]=' ';
            // coche[index]=' ';
            nt = Lect_Numt(index);
            coche[nt]='X';
            AdvStringGrid1->Cells[0][index]= " ";
          }
      }
    }
  // Close_Dump();
  LASTTASK =  nexttache;
  refresh_coches();
  grefresh(current_task,'1');
  Change1();

}

void __fastcall TplStandard::CochertouteslesTaches1Click(TObject *Sender)
{
 Tout_Cocher();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::DcochertouteslesTaches1Click(TObject *Sender)
{
 Tout_Decocher();
}
//---------------------------------------------------------------------------




void __fastcall TplStandard::CopierlesTaches1Click(TObject *Sender)
{
Copier_Cocher();
}

void __fastcall TplStandard::effacer_tache(int t,int posit)
{
 int indice,i;
 int indx,k,nt,it,xx,numt;
 int ipred,isucc;
 char tmp[150]; bool found;
 int colcnt;

 //indice = exist_tache(t,nb_taches);
 //if ((indice==0) || (indice > MAX_T)) return;
 if (nb_taches < 1) return;
 // on neutralise predecesseur et successeur
  pr[t][0] = 0; su[t][0] = 9999;
  pr[t][1] = 0; su[t][1] = 9999;

 // Maintenant decaler AdvStringGrid1
 //for (indx=indice; indx<=nb_taches; indx++)
  indice = posit; // SG1Row;
   colcnt= AdvStringGrid1->ColCount;
   for (indx=indice; indx<AdvStringGrid1->RowCount; indx++)
   {
    for (xx=1;xx<colcnt;xx++)
      {

      AdvStringGrid1->Cells[xx][indx]=AdvStringGrid1->Cells[xx][indx+1];
     }
   }
 for (xx=1;xx<74;xx++)  av->Cells[xx][t]="";  // ?? On maintient numero   = NON

 av->Cells[3][t]="0";  duree[t]=0;

 for (k=0;k<AdvStringGrid1->ColCount;k++)
      AdvStringGrid1->Cells[k][nb_taches]="";
 if (AdvStringGrid1->RowCount>2) AdvStringGrid1->RowCount--;


// Effacer dans asCrit aussi
  // rechercher le numero de tache
  found = false;
  for (indice=1;indice<asCrit->RowCount;indice++)
    {
     strcpy(tmp,asCrit->Cells[1][indice].c_str());
     numt = atoi(tmp);
     if (numt == t){ found = true;  break; }
    }
  if (found)
    {
    colcnt=asCrit->ColCount;
    for (indx=indice; indx<asCrit->RowCount; indx++)
   { for (xx=1;xx<colcnt;xx++) asCrit->Cells[xx][indx]=asCrit->Cells[xx][indx+1];
   }
    if (asCrit->RowCount>1) asCrit->RowCount--;
    }

 //////////////////
   //nb_taches;
   NB_TACHES--;
   nb_taches--;
   if (t==LASTTASK)
     {
      LASTTASK--;
      av->RowCount--;
     }
///////////////////


  for (indx=1;indx<=LASTTASK;indx++)
    {
     ipred=1;


     while ((pr[indx][ipred] !=0) && (pr[indx][ipred] !=9999))
       {
        nt=pr[indx][ipred];
        if (nt==t)
          {// decaler  + break;
           for (it=ipred;it<PS;it++)
             {
              pr[indx][it]=pr[indx][it+1];
              pr_tl[indx][it]=pr_tl[indx][it+1];
              pr_decal[indx][it]=pr_decal[indx][it+1];
             }
           break;
          }

        ipred++;
       }
     isucc=1;

      //  su[task][isucc]!=0) && (isucc<PS)
      while ((su[indx][isucc] !=0) && (su[indx][isucc] != 9999))
       {
        nt=su[indx][isucc];
        if (nt==t)
          { // decaler + break;
           for (it=isucc;it<PS;it++)
             {
              su[indx][it]=su[indx][it+1];
              su_tl[indx][it]=su_tl[indx][it+1];
              su_decal[indx][it]=su_decal[indx][it+1];
             }
           break;
          }

        isucc++;
       }
    }

  indice = exist_tache(t,nb_taches);

   Rebuild_Sommets();

  //for (i=indice;i<=nb_taches;i++)
  //    sommets[i]=sommets[i+1];

  // sommets[nb_taches+1]=0;
}


//---------------------------------------------------------------------------




//---------------------------------------------------------------------------

void __fastcall TplStandard::CocherDcocherTacheencours1Click(
      TObject *Sender)
{
 int i;
 i = current_task;
 // tache en cours doit etre dans current_task
 if (coche[i]=='X')
   {
    coche[i]=' ';
    AdvStringGrid1->Cells[0][SG1Row]=" ";
   }
 else
   {
    coche[i]='X';
    AdvStringGrid1->Cells[0][SG1Row]=AnsiString(strcoche);
   }
 refresh_coches();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::CocherDcocherTches1Click(TObject *Sender)
{
 Coch_Decoch();

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Coch_Decoch()
{
  int i,n;  AnsiString COCHE;
  char NT[100];
  int cnt;
  cnt = AdvStringGrid1->RowCount;

  for (i=1;i<cnt;i++)
    {
     strcpy(NT,AdvStringGrid1->Cells[1][i].c_str());
     n=atoi(NT);
     COCHE = AdvStringGrid1->Cells[0][i];
     if (COCHE=="" || COCHE==" ")
       {
        AdvStringGrid1->Cells[0][i]=AnsiString(strcoche);
        coche[n]='X';
       }
     else
       {
        AdvStringGrid1->Cells[0][i]=" ";
        coche[n]=' ';
       }
    }

 /*
 for (i=1;i<=LASTTASK;i++)
  {
  if (coche[i]=='X')
   {
    coche[i]=' ';
    n=Trouve(i);
    if (n)AdvStringGrid1->Cells[0][n]=" ";
   }
 else
   {
    coche[i]='X';
    n=Trouve(i);
    if (n) AdvStringGrid1->Cells[0][n]=AnsiString(strcoche);
   }
 }
 */
 refresh_coches();


}



int __fastcall TplStandard::Trouve(int i)
{
 int cntrow; int n; int nx;
 char tmp[150];

 cntrow=AdvStringGrid1->RowCount-1;
 n=0;
 for (nx=1;nx<=cntrow;nx++)
   {
    strcpy(tmp,AdvStringGrid1->Cells[1][nx].c_str());
    n=atoi(tmp);
    if (n==i) break;

   }
return n;
}




void __fastcall TplStandard::LierTachesCoches1Click(TObject *Sender)
{
 Lier_Coch();
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::Lier_Coch()
{
  int decal,tl;
  char tmp[250];
  int ix; int first; int previous;
  int cnt; AnsiString XX;

   TLienCoche *CLien;
 // lier taches cochees
 // faire paraitre une petite fenetre de dialogue
     CLien = new TLienCoche(Application);
     CLien->ShowModal();
     delete CLien;
     //ReadExchange(1);
     Ghost->ExtractValue(tmp,m_exchange,"null",0);
     if (strcmp(tmp,"TRUE")==0) return;


 // on extrait type de lien et decalage
     Ghost->ExtractValue(tmp,m_exchange,"decalage",0);
     decal = atoi(tmp);
     Ghost->ExtractValue(tmp,m_exchange,"tl",0);
     tl= atoi(tmp);


 //  on fabrique les liens avec les predecesseurs
 // previous contient le numero de taches precedente
     first = 1;  // la premiere tache cochee n'aura pas de predecesseurs
     cnt = AdvStringGrid1->RowCount;
     for (ix=1;ix<cnt;ix++)

     // for (ix=1;ix<=LASTTASK;ix++)
       {
        //XX = AdvStringGrid1->Cells[0][ix];
        // if (AdvStringGrid1->Cells[0][ix] != "" || AdvStringGrid1->Cells[0][ix] != NULL)
         strcpy(tmp,AdvStringGrid1->Cells[1][ix].c_str());
         ctask=atoi(tmp);

         if (coche[ctask]=='X')
         {
          if (first)  first=0;
          else
           {

             new_nb_taches = nb_taches;
             //strcpy(tmp,AdvStringGrid1->Cells[1][ix].c_str());
             //ctask=atoi(tmp); // sommets[ix];

            if (ctask>0 && ctask<MAX_T)
             {
              InsertPredSeul(ctask,previous,tl,decal,false);
              InsertSuccSeul(previous,ctask,tl,decal,false);
             }
           }
          strcpy(tmp,AdvStringGrid1->Cells[1][ix].c_str());
          previous = atoi(tmp); // sommets[ix];
         }
       }

       Ghost->ExtractValue(tmp,m_exchange,"decoch",0);
       if (strcmp(tmp,"O")==0)
          Tout_Decocher();

     Change1();
     grefresh(current_task,'1');
}



void __fastcall TplStandard::SupprimerTchesCoches1Click(TObject *Sender)
{
 Supp_Coch();
}

void __fastcall TplStandard::Build_Coches()
{
 int i,cnt; char tmp[500];
 int numtache;
 for (i=0;i<=MAX_T;i++) coche[i]=' ';
 cnt=AdvStringGrid1->RowCount;
 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,AdvStringGrid1->Cells[0][i].c_str());
    if (AdvStringGrid1->Cells[0][i]!="" && AdvStringGrid1->Cells[0][i]!=" ")
     {
      strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
      numtache=atoi(tmp);
      coche[numtache]='X';
      }
    }

}

void __fastcall TplStandard::Supp_Coch()
{
 int i,numtache;
 int cntrow; char tmp[200];
 if (Application->MessageBox("Voulez vous supprimer les t�ches coch�es ?",
      m_ecoplan,MB_YESNO)== IDNO) return;

 // Build_Coches();


 cntrow=AdvStringGrid1->RowCount-1;
 for (i=cntrow;i>0;i--)
   {
    strcpy(tmp,AdvStringGrid1->Cells[0][i].c_str());
    if (AdvStringGrid1->Cells[0][i]!="" && AdvStringGrid1->Cells[0][i]!=" ")
     {
      strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
      numtache=atoi(tmp);
      coche[numtache]=' ';
      AdvStringGrid1->Cells[1][i] = "";
      effacer_tache(numtache,i);
     }
   }
 Tout_Decocher();
 Change1();
 current_task=Lect_Numt(1);
 Remplir_asTri();
 //Remplir_AV1();
 
 grefresh(current_task,'1');
 }

 //---------------------------------------------------------------------------

void __fastcall TplStandard::delier_tache(int t)
{
 int indice;
 int indx,it;
 int ipred,isucc;
 int tblpred[500], tblsucc[500];
 int ctrpred,ctrsucc,i;


 indice = t;   // exist_tache(t,nb_taches);
 if ((indice==0) || (indice > MAX_T)) return;
 if (nb_taches < 1) return;

 for (i=0;i<500;i++) {tblpred[i]=0; tblsucc[i]=0; }
 ctrpred=0;ctrsucc=0;

 // traitement pour retirer tous les predecesseurs ou successeurs
 // lies a la tache T   : delier uniquement entre taches cochees
  indx=1;
  while (indx<=LASTTASK) // nb_taches)
     {
      if (sommets[indx]==t)
        {
         ipred=1;
         while (pr[indx][ipred] !=0)
           {
             if (coche[pr[indx][1]]=='X')
               { tblpred[ctrpred]=pr[indx][ipred];
                 ctrpred++;
               }
             ipred++;
           }
         isucc=1;

         while  ((su[indx][isucc] !=0) && isucc<PS) // (su[indx][isucc]!=9999))
           {
             if (coche[su[indx][isucc]]=='X')
              { tblsucc[ctrsucc]= su[indx][isucc];
                ctrsucc++;
              }
             isucc++;
           }

         break;
        }
      indx++;
     }

// on va maintenant  retirer les pred et succ selectionnes

  for (i=0;i<ctrpred;i++)
     {
       ctask=sommets[indx];
       RemovePredSeul(ctask,tblpred[i]);
       RemoveSuccSeul(tblpred[i],ctask);
     }
  for (i=0;i<ctrsucc;i++)
     { ctask=sommets[indx];
       RemoveSuccSeul(indx,tblsucc[i]);
       RemovePredSeul(tblsucc[i],indx);
     }
 Change1();
}



void __fastcall TplStandard::DlierTchesCoches1Click(TObject *Sender)
{
 Delier_Coch();
 }

void __fastcall TplStandard::Delier_Coch()
{ 
// delier taches cochees
 // enlever tous les liens suppr et pred lies a une tache donnee
 int i;
 if (Application->MessageBox("Voulez vous d�lier les t�ches coch�es ?",
      m_ecoplan,MB_YESNO)== IDNO) return;

    for (i=1;i<=LASTTASK;i++) // nb_taches;i++)
       {
        if (av->Cells[1][i] != "")
         {
          if (coche[i] == 'X')  delier_tache(i);   //sommets[i]);
         }
        }


 Tout_Decocher(); 
 grefresh(current_task,'1');
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

/*
int __fastcall TplStandard::SaveAs(char *name)
{
 char newname[256];
 char tmp[250];
 char *p;

 strcpy(newname,name); strlwr(newname);
 p = strstr(newname,".indx");  // ???????????????
 if (p) *p=0;
 strcpy(LocalBase,name);
 updatedatabase(LocalBase,0,0);
 Application->MessageBoxA(newname,m_ecoplan,MB_OK);
 // envoyer un message a Main
 strcpy(tmp,"<caption>");
 strcat(tmp,name);
 strcat(tmp,"</caption>");
 strcpy(m_status,tmp);
 plStandard->Caption = AnsiString(newname);




 return 1;

}

*/

//---------------------------------------------------------------------------


void __fastcall TplStandard::ModifDatePrev(char *str)
{
 char Key[100],RetKey[100];
 int  rc, recdata;
 char tmp1[200],tmp[200];

  strcpy(Key,"<dateprevdebut>");


  strcpy(tmp1,str);
  // Open the


  /*
  rc = proj->ReadDirectKey(0,Key,RetKey,&recdata);
  if (rc)
      rc = proj->RewriteRecord(tmp1, strlen(tmp1)+1,recdata);
  else
   { recdata=proj->GetNewRecordNumber();
     rc = proj->WriteKey(0,Key,recdata);
     rc = proj->WriteRecord(tmp1,strlen(tmp1)+1);
   }
  */
  strcpy(dateprevdeb,tmp1);
  Label7->Caption = AnsiString(str);
  Label109->Caption = AnsiString(str);
  Label134->Caption = AnsiString(str);
   tmp1[2]=0; JourPrev=atoi(tmp1);
   tmp1[5]=0; MoisPrev=atoi(tmp1+3);
   tmp1[10]=0; AnPrev=atoi(tmp1+6);
  if (AnPrev <100) AnPrev = AnPrev+2000;
 Change1();
 PageControl1->ActivePageIndex=0;
 init_calendrier();
 // SaveAs();

 // ReloadAllData(LocalBase,true,true);
 grefresh(current_task,'1');
}

int __fastcall TplStandard::Bcl_Pred(int t,int p)
{
  int i,j,xniv,result; //memt,memp;
 Organise_Niveaux();
 // verifier si Predecesseur P n'est pas dans les successeurs

 if (iniv==1) return 0;

 result=0;
 xniv=-1;
 for (i=0; i<iniv;i++)
    {j=0;
     while (niveaux[i][j] !=0)
     { if (niveaux[i][j]== t) { xniv=i; break; }
       j++;
     }
    }
 for (i=xniv+1;i<iniv;i++)
   {
    j=0;
    while (niveaux[i][j] !=0)
     { if (niveaux[i][j]== p) { result=1; break; }
       j++;
     }
   }
 return result;

}

int __fastcall TplStandard::Bcl_Succ(int t,int s)
{
  int i,j,xniv,memt,mems; int result;  int ttmp;
 Organise_Niveaux();  // iniv est le niveau max
 // verifier si tache S n'est pas dans les niveaux avant T
 if (iniv==1) return 0;
 result=0;
 xniv=-1;
 for (i=0; i<iniv;i++)
    {j=0;
     while (niveaux[i][j] !=0)
     { if (niveaux[i][j]== t) { xniv=i; break; }
       j++;
     }
    }


 for (i=1;i<xniv;i++)
   {
    j=0;
    while (niveaux[i][j] !=0)
     {
       ttmp = niveaux[i][j]; if (ttmp== s) { result=1; break; }
       j++;
     }
   }
 return result;


 }



// =======================================================================
//  ALL ADVSTRINGGRID 1
// =======================================================================


int __fastcall TplStandard::Lect_Numt(int Row)
{
 char *p; char tmp[250]; int numt;
 strcpy(tmp,AdvStringGrid1->Cells[1][Row].c_str());
 p =strstr(tmp,"<"); if (p) *p=0;
 numt=atoi(tmp);
 return numt;
}


//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid1ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 int X,Y;
 X = MousePos.x;
 Y = MousePos.y;
  AdvStringGrid1->MouseToCell(X, Y, CurCol, CurRow);
 // conversion en ligne et colonne courantes
 PopupList->Popup(X, Y);
}

void __fastcall TplStandard::AdvStringGrid1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  int Column, Row, numt,cnt;
  // char *p,tmp[250];

  cnt=AdvStringGrid1->RowCount;
  AdvStringGrid1->MouseToCell(X, Y, Column, Row);
  if ((Row >= cnt) || (Row < 0)) return;

  numt=Lect_Numt(Row);
  if (Column==0)
     { SG1Row=Row; SG1Col=0;
      if (coche[numt]=='X')
      { coche[numt]=' '; AdvStringGrid1->Cells[0][Row] = " "; asTri->Cells[0][Row]= " ";
       }
       else
       { coche[numt]='X';    AdvStringGrid1->Cells[0][Row] = AnsiString(strcoche);
         asTri->Cells[0][Row]= AnsiString(strcoche);
        }

      refresh_coches();
     }

}



//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 int i, nbskip, isucc,indx;
 char tmp[250]; int m,count;
 int nt,dur;
 TNewTask *NewTask;
 int exist,ok;
 int j; int addr; bool Ctrl;
 AnsiString L[80]; bool Insert;


 Ctrl=false;
 if (Shift.Contains(ssCtrl)) { Ctrl=true; }



if (Key == VK_INSERT)
  {    //  insertion d une tache a la fin


     if (nb_taches == 0) LASTTASK=0;
     nt = Find_Hole();
     if (nt==-1) Insert=true; else Insert=false;
     if (nt==-1 || nt==1) // pas de trous
       { nt=LASTTASK+1;
         LASTTASK++;
       }

     /*
     ok=0;
     while (!ok)
      {if (nb_taches !=0)
           { exist = exist_tache(nt,nb_taches);
            if (exist !=0)  nt++;  else ok=1;
           }
        else ok=1;
       }
     */
     if (nt>1) { AdvStringGrid1->RowCount++;
                 // if (Insert)
                 //av->RowCount++;
                 av->RowCount=LASTTASK+1;
                 asCrit->RowCount++;
                }


     nb_taches++; new_nb_taches=nb_taches;
     NB_TACHES++;
     addr = AdvStringGrid1->RowCount-1;
     AdvStringGrid1->Cells[1][addr]=nt;
     // $$$$ av->Cells[1][LASTTASK]=AnsiString(nt);
     av->Cells[1][nt]=AnsiString(nt);
     sprintf(tmp,"Tache num�ro %d",nt);

     AdvStringGrid1->Cells[2][addr]= AnsiString (tmp);
     // $$$$$ av->Cells[2][nb_taches]= AnsiString (tmp);
     // $$$$$ asCrit->Cells[2][nb_taches] =  AnsiString (tmp);
     av->Cells[2][nt]= AnsiString (tmp);
     asCrit->Cells[2][addr] =  AnsiString (tmp);
     // effacer les criteres dans av et dans asCrit
     count = av->ColCount;
     for (m=4; m<count; m++)  av->Cells[m][nt]="";

     count = asCrit->ColCount;
     for (m=3; m<count; m++)  asCrit->Cells[addr][nt]="";

     // strcpy(libel[nb_taches],tmp);
     AdvStringGrid1->Cells[10][addr]= "1";  // cal
     // $$$$$ av->Cells[10][nb_taches]= "1";  // cal
      av->Cells[10][nt]= "1";  // cal

      i=nt;  ///nb_taches;
      duree[i]=1; duree_reelle[i]=0; deb_plus_tot[i]=0; fin_plus_tot[i]=0; deb_plus_tard[i]=0;
      fin_plus_tard[i]=0;
      // $$$$$ sommets[i]=0;
      coche[i]= ' ';  filtr[i]= 'X'; tcal[i]=1; tmom[i][0]=0;
      da[i][0]=0; dtr[i][0]=0; ftr[i][0]=0; pca[i]=0; nbj[i]=0;
      /// $$$$$   Remplir_asTri();
      for (j=0;j<PS;j++) {pr[i][j]=0; su[i][j]=0; pr_tl[i][j]=0; su_tl[i][j]=0;  pr_decal[i][j]=0; su_decal[i][j]=0;}
      dur=1; if (dur <=0) dur=1; duree[i]=dur;

      su[i][1]=9999;  /// IMPORTANT

     AdvStringGrid1->Cells[3][addr]= IntToStr(dur);
     // $$$$$
     av->Cells[3][nt]= AnsiString(dur);
     current_task=nt;  // $$$$ sommets[nb_taches];
     grefresh(current_task,'0');

     // calcule nb de positions a remonter
     if (Ctrl)
       {
        if (SG1Row!=0)
          {
            for (i=0;i<AdvStringGrid1->ColCount;i++)  L[i] = AdvStringGrid1->Cells[i][addr];
            for (indx = addr; indx>=SG1Row; indx--)
              {
               for (i=0;i<AdvStringGrid1->ColCount;i++)  AdvStringGrid1->Cells[i][indx] = AdvStringGrid1->Cells[i][indx-1];
              }

            for (i=0;i<AdvStringGrid1->ColCount;i++)  AdvStringGrid1->Cells[i][SG1Row]= L[i];
            Renum();
          }


       }
     Rebuild_Sommets();
     return;


 }  // VK_INSERT

 if (Key==VK_DELETE)
    {
     strcpy(tmp,AdvStringGrid1->Cells[1][SG1Row].c_str());
     current_task=atoi(tmp);
     if (current_task == 0) return;
     sprintf(tmp,"Confirmer effacement de la t�che %d",current_task);
     if (Application->MessageBox(tmp,m_ecoplan,MB_YESNO) ==IDNO) return;
     effacer_tache(current_task,SG1Row);
     Tout_Decocher();
     refresh_coches();
     Remplir_asTri();
     current_task=1;
        if (nb_taches==0) current_task=0;
     else  grefresh(current_task,'1');
     Rebuild_Sommets();
    }

  if (Key==51)  //'"')
    {
      if (coche[current_task]==' ')
        {
         AdvStringGrid1->Cells[0][current_task] = AnsiString(strcoche);
         coche[current_task]= 'X';
        }
        else
       {
        AdvStringGrid1->Cells[0][current_task] = " ";
         coche[current_task]= ' ';
       }
     refresh_coches();
    }

 Change1();

 }

void __fastcall TplStandard::Rebuild_Sommets()
{
 int i,cnt; int indsom; int lastt;
 char tmp[50];
 lastt=0;
 for (i=0; i<MAX_T;i++) sommets[i]=0;
 cnt = av->RowCount;
 indsom=0;
 for (i=1;i<cnt;i++)
   {
    if (av->Cells[1][i] != "")
      {
       indsom++;
       sommets[indsom]=i;
       strcpy(tmp,av->Cells[1][i].c_str());
       lastt= atoi(tmp);
      }
   }
 sommets[i]=0;
 if (lastt !=LASTTASK)
   { lastt = lastt;
   }
}

int  __fastcall TplStandard::Find_Hole()
{
 int retval; int i,cnt;
 retval = -1;
 cnt = av->RowCount;
 for (i=1;i<cnt;i++)
   {
    if (av->Cells[1][i]=="")
      {
       retval = i;
       break;
      }

   }
return retval;
}

void __fastcall TplStandard::AdvStringGrid1CanEditCell(TObject *Sender,
      int ARow, int ACol, bool &CanEdit)
{
 int ch;
 CanEdit = false;
 if (ARow ==0) return;
 ch=vl_chp[1][ACol];
 if (ch_sai[ch][0]=='N') return;

 if (ACol==3
     || ACol==10
     || ACol==12 ||
     ACol==13)
   {
    if (!Modifiable(ARow))
     {
      CanEdit = false;
      // Application->MessageBoxA("Champ non modifiable",m_ecoplan,MB_OK);
      Beep();
      StatusBar1->SimpleText="Champ non modifiable, Dates Ant�rieures � la date d'actualisation";
      return;
     }
   }

 if (ACol==18 || ACol==19 || ACol==20 || ACol==21 || ACol==22)  //  Champs Libres  et montant
    CanEdit==true;
 if(ACol==14)
 CanEdit==true;
 if(ACol==24)
 CanEdit==false;
 StatusBar1->SimpleText="";
 CanEdit = true;      
}
//---------------------------------------------------------------------------

bool __fastcall TplStandard::Modifiable(int row)
{
 char tmp[50]; int dist1,dist2;
 char datavanc[10];
 if (strlen(DATEAVANC)==0) return true;
 dist1=Convert_Date(AnsiString(DATEAVANC));
 dist2=Convert_Date(AdvStringGrid1->Cells[4][row]);

// if (dist2<=dist1) return false;


 return true;
}



void __fastcall TplStandard::AdvStringGrid1ClickCell(TObject *Sender,
      int ARow, int ACol)
{
  int cnt;   char tmp[80];  // �����
  ARowT = ARow;
  cnt = AdvStringGrid1->RowCount;
  if ((ARow <1) || (ARow >= cnt)) return;    // nb_taches
  SG1Col = Cur1Col = ACol;
  SG1Row = Cur1Row = ARow;

  if(ARowT != 0 || ARowT != ARow){
  AdvStringGrid1->Colors[1][ARow] = TColor(RGB(0,255,255));
  AdvStringGrid1->Colors[4][ARow] = TColor(RGB(0,255,255));
  AdvStringGrid1->Colors[5][ARow] = TColor(RGB(0,255,255));
    AdvStringGrid1->Colors[6][ARow] = TColor(RGB(0,255,255));
      AdvStringGrid1->Colors[7][ARow] = TColor(RGB(0,255,255));
      if(AdvStringGrid1->Cells[8][ARow] == "0"){
        AdvStringGrid1->Colors[8][ARow] = TColor(RGB(0,255,255));}
          AdvStringGrid1->Colors[10][ARow] = TColor(RGB(0,255,255));



  AdvStringGrid1->Colors[1][ARowT] = TColor(RGB(255,255,153));
  AdvStringGrid1->Colors[4][ARowT] = TColor(RGB(255,255,153));
  AdvStringGrid1->Colors[5][ARowT] = TColor(RGB(255,255,153));
    AdvStringGrid1->Colors[6][ARowT] = TColor(RGB(255,255,153));
      AdvStringGrid1->Colors[7][ARowT] = TColor(RGB(255,255,153));
      if(AdvStringGrid1->Cells[8][ARowT] == "0"){
        AdvStringGrid1->Colors[8][ARowT] = TColor(RGB(255,255,153));}
          AdvStringGrid1->Colors[10][ARowT] = TColor(RGB(255,255,153));

        ARowT = ARow;


                                                                        }


 if(ARowT != 0 || ARowT == ARow)
 {
  AdvStringGrid1->Colors[1][ARow] = TColor(RGB(0,255,255));
  AdvStringGrid1->Colors[4][ARow] = TColor(RGB(0,255,255));
  AdvStringGrid1->Colors[5][ARow] = TColor(RGB(0,255,255));
    AdvStringGrid1->Colors[6][ARow] = TColor(RGB(0,255,255));
      AdvStringGrid1->Colors[7][ARow] = TColor(RGB(0,255,255));
      if(AdvStringGrid1->Cells[8][ARow] == "0"){
        AdvStringGrid1->Colors[8][ARow] = TColor(RGB(0,255,255));}
          AdvStringGrid1->Colors[10][ARow] = TColor(RGB(0,255,255));



 }
  if (ACol != 12 && ACol!=13 && ACol!=2 && ACol!=3 && ACol!=10 && ACol!=11 )
   {
     strcpy(tmp,AdvStringGrid1->Cells[1][ARow].c_str());
     current_task=atoi(tmp); //ARow;
    // grefresh(current_task,'1');
   }
}
//---------------------------------------------------------------------------




void __fastcall TplStandard::AdvStringGrid1CellValidate(TObject *Sender,
      int ACol, int ARow, AnsiString &Value, bool &Valid)
{
  int a,nt;
  char tmp[250];
  int ctask; int debtache;
  int numt; int res,res1; int decal,dur;
  char dt1[12],dt2[12],dtPrev[12];
  int start,end; int ncal;
  int journ =0; int durr=0; float rest=0.0;
  int tott, durt; float restt = 0.0;
  ctask=Lect_Numt(ARow);

  switch( ACol)
   {
    case 3:  // duree
     // verifier si numerique (sinon ca plante)
     strcpy(tmp,Value.c_str());
     a = atoi(tmp);
     if (a <=0) { Valid=false; Beep(); return; }
     av->Cells[3][ctask]=AnsiString(a);
     duree[ctask] = a;
     Valid = true;
     break;

    case 2:  // Libelle du champ

     // strcpy(libel[ARow],Value.c_str());
     av->Cells[2][ctask]=Value;
     Valid=true;
     break;

 case 10 :  // calendrier
     strcpy(tmp,Value.c_str());
     ncal = atoi(tmp);
     if (ncal <1 || ncal > 5)
       { Application->MessageBoxA("Valeur Calendrier Incorrecte",m_ecoplan,MB_OK);
         break;
       }

     av->Cells[10][ctask] = Value;
     tcal[ctask] = ncal;
     Valid = true;
     break;

  case 11 :  // Code tache
     Valid = true;
     av->Cells[11][ctask] = Value;

     break;

  case 12:  // Debut Souhait�
     if (Compute_DebutSouhaite(ACol,ARow)) Valid = true;
     else Valid=false;
    break;


  case 13 : // Fin Souhait�e

     if (Compute_FinSouhaitee(ACol,ARow)) Valid = true;
     else Valid = false;


    break;
  ///// ANOMALIE
  case 14: av->Cells[19][ctask]=Value; Valid=true; break;
  case 19: av->Cells[15][ctask]=Value; Valid=true; break;   // champ libre 1
  case 23:
  tott = StrToInt(AdvStringGrid1->Cells[23][ctask]);
  durt = StrToInt(AdvStringGrid1->Cells[3][ctask]);
   rest = (tott/durt);
  AdvStringGrid1->Cells[24][ctask]  = FloatToStr(rest);
  AdvStringGrid1->Refresh();
  av->Cells[83][ctask]=Value; Valid=true;
  av->Cells[84][ctask]=AdvStringGrid1->Cells[24][ctask]; Valid=true;

  break;
  case 24:

  journ = StrToInt(AdvStringGrid1->Cells[24][ctask]);
  durr = StrToInt(AdvStringGrid1->Cells[3][ctask]);
  rest = (journ*durr);
  AdvStringGrid1->Cells[23][ctask]  = FloatToStr(rest);
  AdvStringGrid1->Refresh();
  av->Cells[84][ctask]=Value; Valid=true;
  av->Cells[83][ctask]=AdvStringGrid1->Cells[23][ctask]; Valid=true;
  break;

   } // end Switch

 Change1();
 current_task=sommets[ctask];
 grefresh(current_task,'0');
}

int __fastcall TplStandard::Compute_DebutSouhaite(int ACol,int ARow)
{
 int a,nt;
  char tmp[250];
  int ctask; int debtache;
  int numt; int res,res1; int decal,dur;
  char dt1[12],dt2[12],dtPrev[12];
  int start,end; int ncal;

  ctask=Lect_Numt(ARow);
   strcpy(dt2,AdvStringGrid1->Cells[ACol][ARow].c_str());
   if (dt2[0]==' ') { AdvStringGrid1->Cells[ACol][ARow]=""; return 0; }

     numt = LookFirst();
     if (numt==0)
       { Application->MessageBoxA("Premiere tache non trouv�e",m_ecoplan,MB_OK);
         return 0;
       }


    if (numt==ctask)
       {
        //Application->MessageBoxA("La t�che choisie est la premiere du Projet: On ne peut modifier le D�but Souhait�",m_ecoplan,MB_OK);
        AdvStringGrid1->Cells[ACol][ARow]="";
        av->Cells[12][ctask]="";
         return 0;
       }

      if (strlen(dt2)==0)
        {
         av->Cells[12][ctask]="";
         RemovePredSeul(ctask,numt);
         RemoveSuccSeul(numt,ctask);
         sprintf(tmp,"R�tablir le lien avec la tache %d si n�cessaire",numt);

         // Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
         return 1;
        }
     res = verifdate8(dt2);
     if (res) return 0;

    strcpy(dt1,av->Cells[5][numt].c_str());  //  date de fin premiere tache

    if (strlen(dt1)==0)   // Date de fin pas encore connue
      {
       add_jour(AnPrev,MoisPrev,JourPrev,duree[numt]);
       sprintf (dt1,"%02d-%02d-%02d",Jour,Mois,An%100);
      }
    res = comp_dist_dates(dt1,dt2,1,0);
    decal=NB2; if (decal > 0) decal--;


    sprintf(dtPrev,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev%100);
    res = comp_dist_dates(dtPrev,dt2,1,0);
    decal = NB2;
    if (numt != ctask)
      {
       flag_souhaites[ctask]='Y';
       InsertPredSeul(ctask,numt,3,decal,true);
       InsertSuccSeul(numt,ctask,3,decal,true);
       //InsertPredSeul(ctask,-1,3,decal,true);

      }
    AdvStringGrid1->Cells[ACol+1][ARow]="";    // Raz fin souhaitee
    av->Cells[ACol+1][ctask]="";
    av->Cells[ACol][ctask]=AdvStringGrid1->Cells[ACol][ARow];
  return 1;
 }

////////////////////////////////////::::
 int __fastcall TplStandard::Compute_FinSouhaitee(int ACol,int ARow)
{
 int a,nt;
  char tmp[250];
  int ctask; int debtache,itmp;
  int numt; int res,res1; int decal,dur;
  //int  durfirst;
  char dt1[12],dt2[12],dtPrev[12];
  int start,end; int ncal;

 ctask=Lect_Numt(ARow);
 strcpy(dt2,AdvStringGrid1->Cells[ACol][ARow].c_str());
 if (dt2[0]==' ') { AdvStringGrid1->Cells[ACol][ARow]=""; return 0; }

     numt = LookFirst();

     if (numt==0)
       { Application->MessageBoxA("FS- Premiere tache non trouv�e",m_ecoplan,MB_OK);
         return 0;
       }
     if (numt==ctask)
       {
        Application->MessageBoxA("FS- La t�che choisie est la premiere du Projet: On ne peut modifier la Fin Souhait�e",m_ecoplan,MB_OK);
         AdvStringGrid1->Cells[ACol][ARow]="";
         av->Cells[13][ctask]="";
         return 0;
       }

     if (strlen(dt2)==0)
        {
         av->Cells[13][ctask]="";
         RemovePredSeul(ctask,numt);
         RemoveSuccSeul(numt,ctask);
         sprintf(tmp,"FS- R�tablir le lien avec la tache %d si n�cessaire",numt);
         //Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
         return 1;
        }

     res = verifdate8(dt2);
     if (res) return 0;
     strcpy(tmp,AdvStringGrid1->Cells[3][ARow].c_str());
     dur = atoi(tmp);
     strcpy(tmp,av->Cells[3][numt].c_str());
     //durfirst = atoi(tmp);



     sprintf(dtPrev,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev%100);
     res = comp_dist_dates(dtPrev,dt2,1,0);    // distance au debut Prev
     res = NB1;
     res = recule_temps(res+1,1,dur);
     Convert_Res_To_Date(res);
     strcpy(dt1,Computed_Date);
     res = comp_dist_dates(dtPrev,dt1,1,0);
     decal = CountWorkDay(NB1,1);  // Ncal  forc� a 1
     new_nb_taches = nb_taches;
     flag_souhaites[ctask]='Y';
     InsertPredSeul(ctask,numt,3,decal,true);     // type DD
     InsertSuccSeul(numt,ctask,3,decal,true);
     // InsertPredSeul(ctask,-1,3,decal,true);
     AdvStringGrid1->Cells[ACol-1][ARow]="";    // Raz debut souhaite
     av->Cells[ACol-1][ctask]="";
     av->Cells[ACol][ctask]=AdvStringGrid1->Cells[ACol][ARow];

     // total=recule_conges(fin_plus_tot[xtpred]+1,zcal);
     // total = recule_temps(total,zcal,duree[ind]);

 return 1;

}

int __fastcall TplStandard::CountWorkDay(int dist,int ncal)
{
 int i; int res;
 res = 0;
 for (i=0;i<dist;i++)
  {
   if (cal[ncal][i] == 'T') res++;
  }
 return res;
}

int __fastcall TplStandard::Convert_Res_To_Date(int res)
{
add_jour(AnPrev,MoisPrev,JourPrev,res);
sprintf (Computed_Date,"%02d-%02d-%02d",Jour,Mois,An%100);
return 0;
}


int  __fastcall TplStandard::comp_decal_fin(int st,int fin, int ncal)
{
 int dec;  int i;

 dec=0;
 for (i=st;i<fin;i++)
   {
    if (cal[ncal][i]=='T') dec++;
   }
 return dec;
}

int  __fastcall TplStandard::LookFirst()
{
 char dtprev[50];
 int i,ret,distmin;

 sprintf(dtprev,"%02d%02d%02d",JourPrev,MoisPrev,AnPrev%100);
  //   La premiere tache sera celle qui n'a pas de predecesseur
 ret=1; distmin = 10000;

 for (i=1; i<=LASTTASK;i++)
   {
    if (av->Cells[1][i] != "")
      {
       if (pr[i][1]==0)
         {
          ret = i;
          break;
         }
      }
   }
return ret;



}

//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid1SelectCell(TObject *Sender,
      int ACol, int ARow, bool &CanSelect)
{
 int cnt;
 char tmp[80];
 cnt=AdvStringGrid1->RowCount;

 if ((ARow>0) && (ARow <cnt))  //nb_taches))
  {
   //current_task=Lect_Numt(ARow);
   strcpy(tmp,AdvStringGrid1->Cells[1][ARow].c_str());
   current_task=atoi(tmp);  //ARow;
   grefresh(current_task,'0');
  }
}

// =================================================================
//          ADVSTRINGGRID2
// =================================================================

void __fastcall TplStandard::AdvStringGrid2KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 int numt,nt;
 int tl;
 int decal;
 TNum_Tache_Pred *Num_Tache_Pred;
 //int ipred,isucc;

 char tmp[250];
 char xxx[150];
 compteurt = 0;

 // ������������������
   for(int mgtAdv1 = 1; mgtAdv1 < AdvStringGrid1->RowCount; mgtAdv1++)
   {
     if(StrToInt(AdvStringGrid1->Cells[8][mgtAdv1]) > 0)
     {
        nrtaches[mgtAdv1] = StrToInt(AdvStringGrid1->Cells[1][mgtAdv1]);
        compteurt++;
     }

    }


 if (Key==VK_INSERT)
   {
    m_exchange[0]=0;
    sprintf(m_exchange,"%d",current_task);
    //WriteExchange(1);
    Num_Tache_Pred = new TNum_Tache_Pred(Application);
    Num_Tache_Pred->ShowModal();
    delete Num_Tache_Pred;
    //ReadExchange(1);
    Ghost->ExtractValue(tmp,m_exchange,"tache",0);
    numt = atoi(tmp);
    if ((numt <1) || (numt>9999)) return;
    //nt = exist_tache(numt,nb_taches);
    nt = numt;
    if (av->Cells[2][nt]=="")
      {
       Application->MessageBox("Cette tache n'existe plus",m_ecoplan,MB_OK);
       return;
      }
    if (nt==0)
     {
     Application->MessageBox("Cette tache n'existe pas",m_ecoplan,MB_OK);
     return;
     }
    if (Verif_Succ(current_task,nt))
      { Application->MessageBoxA("Cette tache est d�ja un successeur",m_ecoplan,MB_OK);
        return;
      }


    /*
    if (Bcl_Pred(current_task,numt))
      {
        Beep();
        Application->MessageBoxA("Anomalie Ordonnancement - Bouclage",m_ecoplan,MB_OK);
        return;
      }
    */

    Ghost->ExtractValue(tmp,m_exchange,"tl",0);
    tl = atoi(tmp);
    Ghost->ExtractValue(tmp,m_exchange,"decal",0);
    decal = atoi(tmp);


     rc =Verif_DecalX(nt,current_task,tl,decal);
     if (!rc)
      {
       //Application->MessageBox("Pr�decesseur Commence apres la t�che: Revoir D�calage ou Type de Lien",m_ecoplan,MB_OK);
      return;

      }

    if (numt==current_task)
      {
      Application->MessageBox("M�me num�ro de t�che pr�decesseur",m_ecoplan,MB_OK);
      return;
     }


    new_nb_taches = nb_taches;
    ctask=current_task; //sommets[current_task];
    InsertPredSeul(ctask,numt,tl,decal,true);           // numt @@@@@@@@@@@@��� nt
    InsertSuccSeul(numt,ctask,tl,decal,true);           //  nt  @@@@@@@@@@@@@�
    sprintf(LastOper," Tache N� %d pr�decesseur de %d",ctask,numt);
    Display_Bcl=false;
    btBclClick(NULL);
    Display_Bcl=true; 

    grefresh(ctask,'0');  //current_task,'1');
    }
  else if (Key == VK_DELETE)
    {
     if (Application->MessageBox("Comfirmez la suppression du lien predecesseur",m_ecoplan,MB_YESNO)== IDNO)
     return;

     strcpy(xxx,AdvStringGrid2->Cells[1][SG2Row].c_str());

     numt = atoi(xxx); //StrToInt(AdvStringGrid2->Cells[1][SG2Row]);
     ctask=current_task; //sommets[current_task];
     RemovePredSeul(ctask,numt);
     RemoveSuccSeul(numt,ctask);
     grefresh(ctask,'0'); // urrent_task,'1');
    }
 Change1();
}


// =========================================================================
//      ADVSTRINGGRID3
// =========================================================================

void __fastcall TplStandard::AdvStringGrid3KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 int nt,numt;
 int tl;
 int decal; char mess[250];
 TNum_Tache_Succ *Num_Tache_Succ;
 //int isucc,ipred;

 char tmp[250];  char xxx[150];

 Change1();
 // ����������������������

 if (Key==VK_INSERT)
   {
    m_exchange[0]=0;
    sprintf(m_exchange,"%d",current_task);  //sommets[current_task]);
    //WriteExchange(1);
    Num_Tache_Succ = new TNum_Tache_Succ(Application);
    Num_Tache_Succ->ShowModal();
    delete Num_Tache_Succ;
    //ReadExchange(1);
    Ghost->ExtractValue(tmp,m_exchange,"tache",0);
    numt = atoi(tmp);
    if ((numt <1) || (numt>9999)) return; //nb_taches)) return;
    nt = numt;  // exist_tache(numt,nb_taches);
    if (av->Cells[2][nt]=="")
      {
       Application->MessageBox("Cette tache n'existe plus",m_ecoplan,MB_OK);
       return;
      }
    if (nt==0)
     {
     Application->MessageBox("Cette tache n'existe pas",m_ecoplan,MB_OK);
     return;
     }

     if (Verif_Pred(current_task,nt))
      { Application->MessageBoxA("Cette tache est d�ja un pr�decesseur",m_ecoplan,MB_OK);
        return;
      }

/*
    if (Bcl_Succ(current_task,numt))
      {
       sprintf(mess,"Bouclage entre tache %d et tache %d",current_task,numt);
       Application->MessageBoxA(mess,m_ecoplan,MB_OK);
        return;

       }
*/       


    Ghost->ExtractValue(tmp,m_exchange,"tl",0);
    tl = atoi(tmp);
    Ghost->ExtractValue(tmp,m_exchange,"decal",0);
    decal = atoi(tmp);

    // int rc=Verif_Decal(2,tl, current_task,decal,nt);
    rc = Verif_DecalX(current_task,nt,tl,decal);
    if (!rc)
      {
       // Application->MessageBox("Sucesseur commence avant la tache: Revoir D�calage ou Type de Lien",m_ecoplan,MB_OK);
      return;

      }

    if (numt==current_task) // sommets[current_task])
      {
      Application->MessageBox("M�me num�ro de t�che successeur",m_ecoplan,MB_OK);
      return;
      }

    // insere tache successeur  (la tache en cours est current_task
    new_nb_taches = nb_taches;
    ctask=current_task;  // sommets[current_task];
    InsertSuccSeul(ctask,numt,tl,decal,true);
    InsertPredSeul(numt,ctask,tl,decal,true);
    sprintf(LastOper," Tache N� %d successeur de la Tache %d",numt,ctask);
    Display_Bcl=false;
    btBclClick(NULL);
    Display_Bcl=true;


    grefresh(ctask,'0');  //urrent_task,'1');
    }
  else if (Key == VK_DELETE)
    {
     if (Application->MessageBox("Comfirmez la suppression du lien successeur",m_ecoplan,MB_YESNO)== IDNO)
     return;

     strcpy(xxx,AdvStringGrid3->Cells[1][SG3Row].c_str());
     numt = atoi(xxx) ; //StrToInt(AdvStringGrid3->Cells[1][SG3Row]);
     ctask=current_task; //sommets[current_task];
     RemoveSuccSeul(ctask,numt);
     RemovePredSeul(numt,ctask);
     grefresh(ctask,'0'); // current_task,'1');
    }
}


// =========================================================================
//---------------------------------------------------------------------------

int __fastcall TplStandard::insere_tache(int pos, int dur, int nt, char *txt)
{
int a,indx,k;
char tmp[250];
a=pos;

/*
for (indx=nb_taches;indx>=a;indx--)  // PROVISOIRE   ������������������
      {
       coche[indx+1]=coche[indx];
       sommets[indx+1]=sommets[indx];
       filtr[indx+1]=filtr[indx];
       tcal[indx+1]=tcal[indx];
       strcpy(tmom[indx+1],tmom[indx]);
       duree[indx+1]=duree[indx];
       duree_reelle[indx+1]=duree_reelle[indx];
       // maj des predecesseurs et successeurs

       for (k=0;k<PS;k++)
         { pr[indx+1][k] = pr[indx][k];
           pr_tl[indx+1][k] = pr_tl[indx][k];
           pr_decal[indx+1][k] = pr_decal[indx][k];
           su[indx+1][k] = su[indx][k];
           su_tl[indx+1][k] = su_tl[indx][k];
           su_decal[indx+1][k] = su_decal[indx][k];
         }

       AdvStringGrid1->Cells[1][indx+1]=AdvStringGrid1->Cells[1][indx];
       av->Cells[1][indx+1]=av->Cells[1][indx];
       AdvStringGrid1->Cells[2][indx+1]=AdvStringGrid1->Cells[2][indx];
       av->Cells[2][indx+1]=av->Cells[2][indx];
       AdvStringGrid1->Cells[3][indx+1]=AdvStringGrid1->Cells[3][indx];
       av->Cells[3][indx+1]=av->Cells[3][indx];
      }

   // insertion de la tache en indice "a"
   sommets[a]=nt;
   coche[a]=' ';
   duree[a]=1;
   duree_reelle[a]=1;
   tcal[a]=1;
   tmom[a][0]=0;
   filtr[a]='X';
   if (a>1) AdvStringGrid1->RowCount++;
    for (k=0;k<PS;k++)
    { pr[a][k]=0; pr_tl[a][k]=0; pr_decal[a][k]=0;
      su[a][k]=0; su_tl[a][k]=0; su_decal[a][k]=0;
    }
  su[a][1]=9999;
  su[a][2]=0;

   // sprintf(tmp,"Nlle Tache %d",nt);
   AdvStringGrid1->Cells[1][a]=nt;
   AdvStringGrid1->Cells[2][a]=AnsiString(txt);
   av->Cells[1][a]=nt;
   av->Cells[2][a]=AnsiString(txt);
   nb_taches++;
 return 0;
 */
}

void __fastcall TplStandard::TachesAmontssurtachesCoches1Click(
      TObject *Sender)
{
 Affect_Amont();
}
//---------------------------------------------------------------------------

 void __fastcall TplStandard::Affect_Amont()
{

 /*
 int ind,precedent,torig, nt,ix;
  char tmp[250]; int nbt,dur_amont,design,pos;
  char libelx[100],opdesign[10];
 //  examiner la premiere tache coch�es
  TfTachesAmont *tam;


for (ind=1;ind <= nb_taches; ind++)
  {
   if (coche[ind]=='X')
    {
     m_exchange[0]=0;
     sprintf(m_exchange,"%d",sommets[ind]);
     strcpy(m_exchange2,AdvStringGrid1->Cells[2][ind].c_str());
     //WriteExchange(2);
     tam = new TfTachesAmont(Application);
     tam->ShowModal();
     delete tam;
     //ReadExchange(1);
     Ghost->ExtractValue(tmp,m_exchange,"null",0);
     if (strcmp(tmp,"TRUE")!=0)
       {
         Ghost->ExtractValue(tmp,m_exchange,"nbt",0);
         nbt = atoi(tmp);
         Ghost->ExtractValue(tmp,m_exchange,"dur",0);
         dur_amont = atoi(tmp);
         Ghost->ExtractValue(opdesign,m_exchange,"design",0);
         strcpy(libelx,AdvStringGrid1->Cells[2][ind].c_str());

         // on demarre avec tache mere
         precedent = sommets[ind];
         torig = ind+1;
         for (ix=1;ix<=nbt;ix++)
          {
            nt = next_task();

            if (strcmp(opdesign,"same")==0) strcpy(tmp,libelx);
              else sprintf(tmp,"Tache Amont %d",nt);
            insere_tache(torig,dur_amont,nt,tmp);  // ,precedent,2,1, tmp);
            new_nb_taches = nb_taches;
            InsertPredSeul(precedent,nt,2,0,true);
            InsertSuccSeul(nt,precedent,2,0,true);
            pos = exist_tache(nt,new_nb_taches);
            sprintf(tmp,"%d",ind);
            strcpy(tmom[pos],tmp);   // memorise la mere;
            precedent=nt;
            torig++;
          }
         current_task=sommets[ind];
         grefresh(current_task,'0');
       } //if not null

   } // if coche
  } // end boucle for
*/
}


void __fastcall TplStandard::AffecterunCalendrier1Click(TObject *Sender)
{
Affect_Cal();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Affect_Cal()
{
 int ind,num_cal;
 TAffectCal *taf;
 char tmp[250];
 int idx;

 taf = new TAffectCal(Application);
 taf->ShowModal();
 delete taf;

 //ReadExchange(1);
 Ghost->ExtractValue(tmp,m_exchange,"null",0);
 if (strcmp(tmp,"TRUE")==0)  return;
 Ghost->ExtractValue(tmp,m_exchange,"ncal",0);
 num_cal = atoi(tmp);
 if ((num_cal<1) || (num_cal>5)) num_cal= 1;


 for (ind=1;ind <= LASTTASK; ind++)
  {
   if (coche[ind]=='X')
    {
         tcal[ind] = num_cal;
         idx = Trouve(ind);
         AdvStringGrid1->Cells[10][idx]= num_cal;
         av->Cells[10][ind]= num_cal;
    }
   } // end FOR

  Ghost->ExtractValue(tmp,m_exchange,"decoch",0);
       if (strcmp(tmp,"O")==0) Tout_Decocher();

   current_task=Lect_Numt(1);   //sommets[1];
   //  init_calendrier();
   grefresh(current_task,'1');
}



void __fastcall TplStandard::AdvStringGrid2CanEditCell(TObject *Sender,
      int ARow, int ACol, bool &CanEdit)
{
 CanEdit = true;
 if (ARow < 1) CanEdit = false;

 if ((ACol < 3) || (ACol > 6)) CanEdit = false;
 if  (ACol==4) CanEdit = false;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid3CanEditCell(TObject *Sender,
      int ARow, int ACol, bool &CanEdit)
{
  CanEdit = true;
 if (ARow < 1) CanEdit = false;

 if ((ACol < 3) || (ACol > 6)) CanEdit = false;
 if  (ACol==4) CanEdit = false;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid2CellValidate(TObject *Sender,
      int ACol, int ARow, AnsiString &Value, bool &Valid)
{
  int a; int vtask;
  char tmp[250];
  int typl,idecal;
  char stypl[10];

  strcpy(tmp,Value.c_str());
  if (ACol==3)  // duree
    { // verifier si numerique (sinon ca plante)
     vtask = atoi(AdvStringGrid2->Cells[1][ARow].c_str());
     vtask=exist_tache(vtask,nb_taches);
     a = atoi(tmp);
     if (a <=0) { Valid=false; Beep(); return; }

     duree[vtask] = a;
     av->Cells[3][vtask]=AnsiString(a);
     Valid = true;

     grefresh(current_task,'0');    // &&&&&
     return;
    }
  if (ACol==5)  // Type
    { Valid=false;
      vtask = atoi(AdvStringGrid2->Cells[1][ARow].c_str());
      // vtask=exist_tache(vtask,nb_taches);
      strupr(tmp);
      if (strcmp(tmp,"FD")==0) { Valid=true; AdvStringGrid2->Cells[5][ARow]="FD"; }
      if (strcmp(tmp,"FD")==0) { Valid = true; AdvStringGrid2->Cells[5][ARow]="FF"; }
      if (strcmp(tmp,"AM")==0) { Valid=true;  AdvStringGrid2->Cells[5][ARow]="AM"; }
      if (strcmp(tmp,"DD")==0) { Valid=true;  AdvStringGrid2->Cells[5][ARow]="DD"; }
      if ((strcmp(tmp,"FD")==0) || (strcmp(tmp,"FF")==0) || (strcmp(tmp,"DD")==0) ||
           (strcmp(tmp,"AM")==0))
        {
        typl=0;
        if (strcmp(tmp,"FD")==0) typl = 0;
        else if (strcmp(tmp,"FF")==0) typl= 1;
        else if (strcmp(tmp,"AM")==0) typl= 2;
        else if (strcmp(tmp,"DD")==0) typl= 3;
        idecal = atoi(AdvStringGrid2->Cells[6][ARow].c_str());


         rc = Verif_DecalX(vtask,current_task,typl,idecal);
         if (rc ==0)
               {
                Valid = false;
                AdvStringGrid2->Cells[5][ARow]=MemoTypl2;
                return;
               }

        Valid=true;
        new_nb_taches=nb_taches;
        InsertPredSeul(current_task,vtask,typl,idecal,true);
        InsertSuccSeul(vtask,current_task,typl,idecal,true);
        grefresh(current_task,'0');
        return;
       }
       else { Valid=false; Beep(); return; }
    }

  if (ACol==6) // Decal
     {
      vtask = atoi(AdvStringGrid2->Cells[1][ARow].c_str());
      idecal = atoi(tmp);  // decalage
      strcpy(stypl,AdvStringGrid2->Cells[5][ARow].c_str());
      strupr(stypl);
        if (strcmp(stypl,"FD")==0) typl = 0;
        if (strcmp(stypl,"FF")==0) typl= 1;
        if (strcmp(stypl,"AM")==0) typl= 2;
        if (strcmp(stypl,"DD")==0) typl= 3;
        rc = Verif_DecalX(vtask,current_task,typl,idecal);
         if (rc ==0)
               {
                Valid = false;
                AdvStringGrid2->Cells[6][ARow]=MemoDecal2;
                return;
               }


        Valid=true;
        new_nb_taches=nb_taches;
        InsertPredSeul(current_task,vtask,typl,idecal,true);
        InsertSuccSeul(vtask,current_task,typl,idecal,true);
        grefresh(current_task,'0');
        return;
     }
 Change1();
}
//---------------------------------------------------------------------------


int __fastcall  TplStandard::grefresh(int task,char mode)
{

 char tmp[150];  int first; int a;

 // task = numero de tache obtenu dans AdvStr1
 if (nb_taches==0)
   {
    Clear_Diagram(immGantt);
    return 0;
  }

  if (mode=='1')
  {
   strcpy(tmp,AdvStringGrid1->Cells[1][1].c_str());
   if (strlen(tmp))
   current_task=atoi(tmp);
  }
 else current_task=task;

 su[1][1]=su[1][1];

 Compute_PlusTotA();
 Compute_PlusTardA('1',false);

 Label11->Caption = AnsiString(first);
 DrawMiniGantt(current_task,0,immGantt,false);

 Refresh_Pred_Succ(current_task);

 refresh_marges();
 refresh_couts();
 return 0;
}


/////////////////////////////////////////////////////////////////

int __fastcall TplStandard::Verif_Succ(int t,int a)
{
 int exist,xtsucc,isucc;
 exist=0;
 isucc=1;
 while (su[t][isucc]!=0)
    { xtsucc=su[t][isucc];
      xtsucc=exist_tache(xtsucc,nb_taches);
      if (xtsucc==a) exist = 1;
      isucc++;
    }
 return exist;
}


int __fastcall TplStandard::Verif_Pred(int t,int a)
{
 int exist,xtpred,ipred;
 exist=0;
 ipred=1;
 while (pr[t][ipred]!=0)
    { xtpred=pr[t][ipred];
      xtpred=exist_tache(xtpred,nb_taches);
      if (xtpred==a) exist = 1;
      ipred++;
    }
 return exist;
}

// mode = 1 : predecesseur
// mode = 2 : successeur
// typ 0=FD, 1=FF, 2=AM, 3=DD
int __fastcall TplStandard::Verif_Decal(int mode,int typ, int t, int decal,int sp)
{
 //int
 //dist1,
 //dist2,
 //task,
 //durt,
 //durcur;

 //task = exist_tache(t,nb_taches);
 //dist1 = deb_plus_tot[task];
 //dist2 = fin_plus_tot[task];
 //durt = duree[sp];
 //durcur = duree[task];
 if (typ==2)
   {
    if (decal >=0)
     {
      Application->MessageBox("Decalage Positif pour taches amont",m_ecoplan,MB_OK);
      return 0;
     }
    return 1;
   }
 /*
 if (mode==2)
   {
    if (dist2+decal <= dist1) return 0;
   }

 if (mode == 1)
   {
    if (typ==0)  // tl FD
      {
       if (durt< (-decal)) return 0;
      }
    if (typ==1)  // TL = FF
      {
       if ((-decal + durcur) > durt) return 0;
      }

    if (typ==3)  // TL = DD
      {
       if (decal < 0) return 0;
      }
   }
 //if ((dist2+decal+1) < dist1) return 0;
 //if ((dist1+decal) < 0) return 0;
 */
 return 1;
}


int  __fastcall TplStandard::Verif_DecalX(int tpred, int tsucc, int typ, int decal)
{
 int dur1,dur2;

 dur1  = duree[tpred];
 dur2  = duree[tsucc];

 // typ 0=FD  1= FF 2 = AMont 3 = DD
 switch (typ)
   { case 0:
       if (dur1+decal < 0)
       {
           Application->MessageBoxA("Le Successeur Commence avant le Pr�d�cesseur (FD)",m_ecoplan,MB_OK);
           return 0;
       }
       break;
     case 1:
       if (decal < 0)
         { // if ((decal - dur2)  < dur1 )
           if ((dur2 - decal) > dur1)
           {
            Application->MessageBoxA("Le Successeur Commence avant le Pr�decesseur (FF)",m_ecoplan,MB_OK);
            return 0;
           }
         }
       break;
     case 2:
       break;
case 3:
       if (decal < 0)
          {
           Application->MessageBoxA("Le D�calage ne peut �tre n�gatif en Lien DD",m_ecoplan,MB_OK);
           return 0;
          }
       break;
   }
 return 1;
}

void __fastcall TplStandard::AdvStringGrid3CellValidate(TObject *Sender,
      int ACol, int ARow, AnsiString &Value, bool &Valid)
{
    int a; int vtask;
  char tmp[250];
  int typl,idecal;
  char stypl[10];

  strcpy(tmp,Value.c_str());
  if (ACol==3)  // duree
    { // verifier si numerique (sinon ca plante)
     vtask = atoi(AdvStringGrid3->Cells[1][ARow].c_str());
     vtask=exist_tache(vtask,nb_taches); // OK
     a = atoi(tmp);
     if (a <=0) { Valid=false; Beep(); return; }

     duree[vtask] = a;
     Valid = true;
     grefresh(current_task,'0');
     return;
    }
  if (ACol==5)  // Type
    { Valid=false;
      vtask = atoi(AdvStringGrid3->Cells[1][ARow].c_str());
      // vtask=exist_tache(vtask,nb_taches);
      strupr(tmp);
      if (strcmp(tmp,"FD")==0) { Valid=true;  AdvStringGrid3->Cells[5][ARow]="FD"; }
      if (strcmp(tmp,"FF")==0) { Valid = true; AdvStringGrid3->Cells[5][ARow]="FF"; }
      if (strcmp(tmp,"AM")==0) { Valid=true; AdvStringGrid3->Cells[5][ARow]="AM"; }
      if (strcmp(tmp,"DD")==0) { Valid=true;  AdvStringGrid3->Cells[5][ARow]="DD"; }

      if ((strcmp(tmp,"FD")==0) ||
           (strcmp(tmp,"FF")==0) ||
           (strcmp(tmp,"AM")==0) ||
           (strcmp(tmp,"DD")==0))
       {
        idecal = atoi(AdvStringGrid3->Cells[6][ARow].c_str()); typl=0;
        typl=0;
        if (strcmp(tmp,"FD")==0) typl = 0;
        else if (strcmp(tmp,"FF")==0) typl= 1;
        else if (strcmp(tmp,"AM")==0) typl= 2;
        else if (strcmp(tmp,"DD")==0) typl= 3;

        rc = Verif_DecalX(current_task,vtask,typl,idecal);
        if (rc ==0)
            {
             Valid = false;
             AdvStringGrid3->Cells[5][ARow]=MemoTypl3;
              return;
           }

        Valid=true;
        new_nb_taches=nb_taches;
        InsertSuccSeul(current_task,vtask,typl,idecal,true);
        InsertPredSeul(vtask,current_task,typl,idecal,true);
        grefresh(current_task,'0');
        return;
       }
       else { Valid=false; Beep(); return; }
    }

  if (ACol==6) // Decal
     {
      vtask = atoi(AdvStringGrid3->Cells[1][ARow].c_str());

      strcpy(stypl,AdvStringGrid3->Cells[5][ARow].c_str());
        strupr(stypl);
        if (strcmp(stypl,"FD")==0) typl = 0;
        if (strcmp(stypl,"FF")==0) typl= 1;
        if (strcmp(stypl,"AM")==0) typl= 2;
        if (strcmp(stypl,"DD")==0) typl= 3;
        idecal = atoi(AdvStringGrid3->Cells[6][ARow].c_str());
        // rc = Verif_Decal(2,typl, current_task,idecal,vtask);
        rc = Verif_DecalX(current_task,vtask,typl,idecal);
             if (rc ==0)
               {
                Valid = false;
                AdvStringGrid3->Cells[6][ARow]=MemoDecal3;
                return;
               }

        Valid=true;
        new_nb_taches=nb_taches;
        InsertSuccSeul(current_task,vtask,typl,idecal,true);
        InsertPredSeul(vtask,current_task,typl,idecal,true);
        grefresh(current_task,'0');
     return;
     }
 Change1();
}
//---------------------------------------------------------------------------



void __fastcall TplStandard::AdvStringGrid2SelectCell(TObject *Sender,
      int ACol, int ARow, bool &CanSelect)
{
 SG2Col=ACol;
 SG2Row=ARow; // pour pouvoir effacer
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::AdvStringGrid3SelectCell(TObject *Sender,
      int ACol, int ARow, bool &CanSelect)
{
 SG3Col=ACol;
 SG3Row=ARow; // pour pouvoir effacer
}
//---------------------------------------------------------------------------



int __fastcall TplStandard::verifdate(char *dt10)
{
 char d[50];     // format DD/MM/AAAA    et stockage dans std_date et reverse_date
 char tmp[30];
 AnsiString Today;
  Today = DateTimeToStr(Now());


 strcpy(d,dt10);
 d[2]=0; JourVerif=atoi(d);
 d[5]=0; MoisVerif=atoi(d+3);
 d[10]=0; AnVerif=atoi(d+6);


 std_date[0]=0; reverse_date[0]=0;

 if ((JourVerif ==0) || (JourVerif > 31))
   {
    Application->MessageBoxA("Jour Incorrect",m_ecoplan,MB_OK);
    return 1;
   }
 if ((MoisVerif == 0) || (MoisVerif > 12))
   {
    Application->MessageBoxA("Mois Incorrect",m_ecoplan,MB_OK);
    return 1;
   }
 if ((AnVerif < 2000) || (AnVerif > 2050))
   {
   // Application->MessageBoxA("Ann�e Incorrecte",m_ecoplan,MB_OK);
    return 1;
   }

sprintf(std_date,"%02d/%02d/%02d",JourVerif,MoisVerif,AnVerif%100);
sprintf(reverse_date,"%02d/%02d/%02d",AnVerif,MoisVerif,JourVerif%100);
return 0;
}

int __fastcall TplStandard::verifdate8(char *dt)
{
 char d[50];     // format DD/MM/AA    et stockage dans std_date et reverse_date
 char tmp[30];

 AnsiString Today;
 Today = DateTimeToStr(Now());


 strcpy(d,dt);
 d[2]=0; JourVerif=atoi(d);
 d[5]=0; MoisVerif=atoi(d+3);
 d[8]=0; AnVerif=atoi(d+6);


 std_date[0]=0; reverse_date[0]=0;

 if ((JourVerif == 0) || (JourVerif > 31))
   {
    Application->MessageBoxA("Jour Incorrect",m_ecoplan,MB_OK);
    return 1;
   }
 if ((MoisVerif == 0) || (MoisVerif > 12))
   {
    Application->MessageBoxA("Mois Incorrect",m_ecoplan,MB_OK);
    return 1;
   }
 if ((AnVerif == 0) || (AnVerif > 50))
   {
   Application->MessageBoxA("Ann�e Incorrecte",m_ecoplan,MB_OK);
    return 1;
   }

sprintf(std_date,"%02d/%02d/%02d",JourVerif,MoisVerif,AnVerif%100);
sprintf(reverse_date,"%02d/%02d/%02d",AnVerif,MoisVerif,JourVerif%100);
return 0;
}


void __fastcall TplStandard::AdvStringGrid1GetFormat(TObject *Sender,
      int ACol, TSortStyle &AStyle, AnsiString &aPrefix,
      AnsiString &aSuffix)
{

//TSortStyle = (ssAutomatic, ssAlphabetic, ssNumeric, ssDate, ssAlphaNoCase, ssAlphaCase,
//ssShortDateEU, ssShortDateUS, ssCustom, ssFinancial);

switch(ACol) {
case 1: AStyle=ssNumeric; //AStyle=ssAlphaNoCase;
break;
case 2: AStyle=ssAlphaNoCase;
break;
case 3: AStyle=ssNumeric;
break;
case 4: ssShortDateEU; //  AStyle=ssDate;
case 5: ssShortDateEU;
case 6: ssShortDateEU;
case 7: ssShortDateEU;
break;

}

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::validate_avancement()
{
 if (!global_valid) return;
 // row_avancement est a l'origine
 //asAvanc->Cells[6][row_avancement] = AdvStringGrid4->Cells[7][1];
 //asAvanc->Cells[7][row_avancement] = AdvStringGrid4->Cells[8][1];
 //asAvanc->Cells[8][row_avancement] = AdvStringGrid4->Cells[6][1];
 //asAvanc->Cells[9][row_avancement] = AdvStringGrid4->Cells[5][1];
// asAvanc->Cells[6][row_avancement] = AdvStringGrid4->Cells[7][1];
// asAvanc->Cells[6][row_avancement] = AdvStringGrid4->Cells[7][1];
// asAvanc->Cells[6][row_avancement] = AdvStringGrid4->Cells[7][1];
// asAvanc->Cells[6][row_avancement] = AdvStringGrid4->Cells[7][1];

}

//---------------------------------------------------------------------------

int  __fastcall TplStandard::CopieAvancePrev(char *fn)
{

 int recdata; char Key[100],RetKey[100];
 int count; int lim;
 char tmp[250]; int nt;
 char ntach[10];

 avanc->OpenEngine(fn,120,"act","don");
 avanc->SetIndexMode(0,0);
 // NON :fabriquer d'apres la grille du previsionnel
 // Fabriquer d'apres AV

// %%%%% lim = AdvStringGrid1->RowCount;
 lim = av->RowCount;

 for (count=1;count<lim;count++)
   {
    //sprintf(ntach,"%d",sommets[count]);
    //getvalue(1,count,1); strcpy(ntach,adv);
    //if (sommets[count] !=0) //strlen(ntach))
    if (av->Cells[2][count] != "")
      {

       //nt=atoi(ntach);
       nt = count;
       sprintf(ntach,"%d",nt);
       buffer[0]=0;
       strcat(buffer,"<nt>"); strcat(buffer,ntach); strcat(buffer,"</nt>");
       sprintf(tmp,"<da>%02d-%02d-%02d</da>",JourAvanc,MoisAvanc,AnAvanc%100);
       strcat(buffer,tmp);

       strcpy(adv,av->Cells[2][count].c_str()); concat("lb");
       strcpy(adv,av->Cells[4][count].c_str()); concat("ddp");
       strcpy(adv,av->Cells[3][count].c_str()); concat("dp");
       strcpy(adv,av->Cells[5][count].c_str()); concat("dfp");
       strcpy(adv,av->Cells[9][count].c_str()); concat("cal");
       strcpy(adv,av->Cells[4][count].c_str()); concat("ddr");
       strcpy(adv,av->Cells[3][count].c_str()); concat("dr");
       strcpy(adv,av->Cells[5][count].c_str()); concat("dfr");
       strcpy(adv,av->Cells[8][count].c_str()); concat("mt");
       strcpy(adv,av->Cells[4][count].c_str()); concat("dtposs");
       strcpy(adv,"0"); concat("rimp"); concat("rtot");
       strcpy(adv,"100"); concat("prest");
       strcpy(adv,av->Cells[3][count].c_str()); concat("jrest");
       strcpy(adv,av->Cells[8][count].c_str()); concat("margrl");



       recdata = avanc->GetNewRecordNumber();
       sprintf(Key,"T%04d",nt);
       avanc->WriteKey(0,Key,recdata);
       avanc->WriteRecord(buffer, strlen(buffer)+1);
      }
    }
avanc->CloseEngine(); // avanc->~realisam();
// delete avanc;
return 0;
}

int  __fastcall TplStandard::CopieAvance(char *suf,char *fn)
{
 char tmp[250]; char tmp1[250]; int lg;
 int rc,recdata,recold; char Key[100],RetKey[100];

 char suffancien[250];  lg = strlen(lastfile);
 char *p;

 strcpy(tmp1,LocalBase); //   strcat(tmp1,"\\"); // strcat(tmp1,ProjectName);
 strcat(tmp1,lastfile+lg-13);   // FAUX 
 p = strstr(tmp1,".act");
 if (p) *p=0;

 strcpy(tmp,LocalBase); strcat(tmp,suf);
 rc = avanc->OpenEngine(fn,120,"act","don");
 rc = avanc->SetIndexMode(0,0);
 rc = oldavanc->OpenEngine(tmp1,120,"act","don");
 rc = oldavanc->SetIndexMode(0,0);


 rc = oldavanc->ReadFirstKey(0,RetKey,&recold);
 while (rc)
   {
    rc=oldavanc->ReadRecord(buffer,recold);
    recdata = avanc->GetNewRecordNumber();
    rc = avanc->WriteKey(0,RetKey,recdata);
    rc = avanc->WriteRecord(buffer, strlen(buffer)+1);
    rc = oldavanc->ReadNextKey(RetKey,&recold);
    strcpy(Key,RetKey);
   }

oldavanc->CloseEngine(); //oldavanc->~realisam();
// delete oldavanc;
avanc->CloseEngine(); // avanc->~realisam();
// delete avanc;
 return 0;
}


int __fastcall TplStandard::Load_Avanc(char *fn,bool cn)
{
 char Key[100], RetKey[100];
 int rc, recdata;
 realisam *avanc;
 char dt1[15];char dt2[15]; bool OKK;
 bool ok; int count,avccount,zcal;
 char tmp[250];  int numtache,numt;
 int dist1,dist2,dist3,dur,i,j,xxcal;
 int a1,m1,j1,a2,m2,j2;
 float fdata,fdata2;
 int result2,x;   int per;

 flag_alert=false;
 flag_alertPred=false;

 for (j=1;j<AdvStringGrid4->ColCount;j++)
   { AdvStringGrid4->Cells[j][1]="";  AdvStringGrid4->Colors[j][1]=clWhite; }
for (j=1;j<AdvStringGrid7->ColCount;j++)
   { AdvStringGrid7->Cells[j][1]="";  AdvStringGrid7->Colors[j][1]=clWhite; }

   for (i=1;i<AdvStringGrid5->RowCount;i++)
    { for (j=1;j<AdvStringGrid5->ColCount;j++)  AdvStringGrid5->Cells[j][1]=""; }
  AdvStringGrid5->RowCount = 2;

  avact->RowCount = 2;
  for (i=1;i<avact->RowCount;i++)
    { for (j=1;j<avact->ColCount;j++)  avact->Cells[j][1]="";
    }
  avact->RowCount = 2;


   taches_concernees = 0;
    // On ouvre maintenant le fichier et on charge les donn�es
  avanc = new  realisam();    // ne pas enlever, sinon erreur
  rc = avanc->OpenEngine(FileNameAvanc,120,"act","don");   //512 - 8
  rc = avanc->SetIndexMode(0,0);
  int nbk = avanc->NumberOfKeys(0);

  //  RETKEY MAUVAIS !!
  Clean_Grid(asAvanc,2);
Clean_Grid(AdvStringGrid6,2);

  rc = avanc->ReadFirstKey(0,RetKey,&recdata);
  count=0; avccount=0;
  while (rc) //  &strcmp(RetKey,"T0000")!=0)
   {
    if (strcmp(RetKey,"T0000") !=0)

    {

    rc=avanc->ReadRecord(buffer,recdata);
    buffer[rc]=0;
    Ghost->ExtractValue(tmp,buffer,"ddp",0);  // debut previsionnel
    // mettre sous la forme AA-MM-JJ
    dt2[0]=tmp[6]; dt2[1]=tmp[7]; dt2[2]='-';dt2[3]=tmp[3];
    dt2[4]=tmp[4];dt2[5]='-'; dt2[6]=tmp[0];dt2[7]=tmp[1];

    ok=false;
    if (!concern) ok=true;
    //else if  (strcmp(dt1,dt2) >=0) ok=true;  // tache concernee
    if (strcmp(RetKey,"T0000")==0) ok=false;
     else ok =true;
    if (ok)
      { count++;
        if (count>1) { asAvanc->RowCount++;AdvStringGrid6->RowCount++; }

        Ghost->ExtractValue(tmp,buffer,"nt",0); asAvanc->Cells[1][count] = AnsiString(tmp); //setvalue(5,count,39,tmp);
		AdvStringGrid6->Cells[1][count] = AnsiString(tmp);
        numtache = atoi(tmp);
        Ghost->ExtractValue(tmp,buffer,"lb",0); asAvanc->Cells[2][count] = AnsiString(tmp); //setvalue(5,count,40,tmp);
		AdvStringGrid6->Cells[2][count] = AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"da",0); asAvanc->Cells[13][count] = AnsiString(tmp); //setvalue(5,count,70,tmp);
        Ghost->ExtractValue(tmp,buffer,"ddr",0);
        if (strncmp(tmp,"00",2)==0) strncpy(tmp,"01",2);
        asAvanc->Cells[3][count] = AnsiString(tmp); //setvalue(5,count,41,tmp);
		AdvStringGrid6->Cells[3][count] = AnsiString(tmp);
        av->Cells[21][numtache]=AnsiString(tmp);

        

        Ghost->ExtractValue(tmp,buffer,"dr",0); asAvanc->Cells[4][count] = AnsiString(tmp); AdvStringGrid6->Cells[4][count] = AnsiString(tmp);av->Cells[23][numtache]=AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"dfr",0); asAvanc->Cells[5][count] = AnsiString(tmp);AdvStringGrid6->Cells[5][count] = AnsiString(tmp); //setvalue(5,count,43,tmp);
        av->Cells[22][numtache]=AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"rimp",0); asAvanc->Cells[6][count] = AnsiString(tmp);AdvStringGrid6->Cells[6][count] = AnsiString(tmp); av->Cells[24][numtache]=AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"rtot",0); asAvanc->Cells[7][count] = AnsiString(tmp);AdvStringGrid6->Cells[7][count] = AnsiString(tmp); av->Cells[25][numtache]=AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"ddp",0); asAvanc->Cells[8][count] = AnsiString(tmp); AdvStringGrid6->Cells[8][count] = AnsiString(tmp);//setvalue(5,count,46,tmp);
        Ghost->ExtractValue(tmp,buffer,"dp",0); asAvanc->Cells[9][count] = AnsiString(tmp);AdvStringGrid6->Cells[9][count] = AnsiString(tmp); //setvalue(5,count,47,tmp);
        Ghost->ExtractValue(tmp,buffer,"dfp",0); asAvanc->Cells[10][count] = AnsiString(tmp);AdvStringGrid6->Cells[10][count] = AnsiString(tmp);//setvalue(5,count,48,tmp);
        //  Col11 Cout recette (49)
        //  Col 12 Cout reel (50)
        // col 13 ci dessus data avancement
        Ghost->ExtractValue(tmp,buffer,"cal",0);
        xxcal = atoi(tmp);  if (xxcal <1 || xxcal > 5) strcpy(tmp,"1");
        asAvanc->Cells[14][count] = AnsiString(tmp); //setvalue(5,count,62,tmp);
        Ghost->ExtractValue(tmp,buffer,"mt",0); asAvanc->Cells[16][count] = AnsiString(tmp); av->Cells[29][numtache]=AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"dtposs",0); asAvanc->Cells[17][count] = AnsiString(tmp);AdvStringGrid6->Cells[17][count] = asAvanc->Cells[17][count];//setvalue(5,count,69,tmp);
        Ghost->ExtractValue(tmp,buffer,"prest",0); asAvanc->Cells[18][count] = AnsiString(tmp);AdvStringGrid6->Cells[18][count] = AnsiString(tmp); //setvalue(5,count,71,tmp);
        Ghost->ExtractValue(tmp,buffer,"jrest",0); asAvanc->Cells[19][count] = AnsiString(tmp); AdvStringGrid6->Cells[19][count] = AnsiString(tmp);//setvalue(5,count,72,tmp);
        Ghost->ExtractValue(tmp,buffer,"margrl",0); asAvanc->Cells[15][count] = AnsiString(tmp); av->Cells[28][numtache]=AnsiString(tmp);
        //Ghost->ExtractValue(tmp,buffer,"pourcrest",0); setvalue(5,count,69,tmp);
        // on attribue un %

         Ghost->ExtractValue(tmp,buffer,"ctach",0); asAvanc->Cells[25][count] = AnsiString(tmp);

        // MISE A JOUR DES JOUR RESTANTS ET PERCENT
        strcpy(adv,asAvanc->Cells[5][count].c_str());  // Date fin reelle
        dist1 = Convert_Date(AnsiString(adv));
        dist2 = Convert_Date(AnsiString(DATEAVANC));
        strcpy(adv,asAvanc->Cells[3][count].c_str());
        dist3 = Convert_Date(AnsiString(adv));
        if (dist1<dist2)
        { asAvanc->Cells[18][count]="0";
          asAvanc->Cells[19][count]="0";
		  
		  AdvStringGrid6->Cells[18][count]="0";
          AdvStringGrid6->Cells[19][count]="0";
                  
		 av->Cells[31][numtache]="0";
          av->Cells[32][numtache]="0";

        }
        if (dist3>dist2)
        { asAvanc->Cells[18][count]="100";
           asAvanc->Cells[19][count]= asAvanc->Cells[4][count];
          AdvStringGrid6->Cells[18][count]="100";
		  AdvStringGrid6->Cells[19][count]= asAvanc->Cells[4][count];
		  av->Cells[31][numtache]="100";
          av->Cells[32][numtache]=asAvanc->Cells[4][count];
         }

        Ghost->ExtractValue(tmp,buffer,"zero",0); asAvanc->Cells[20][count] = AnsiString(tmp); av->Cells[65][numtache]=AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"cent",0); asAvanc->Cells[21][count] = AnsiString(tmp); av->Cells[66][numtache]=AnsiString(tmp);
        Ghost->ExtractValue(tmp,buffer,"fix",0); asAvanc->Cells[24][count] = AnsiString(tmp); av->Cells[59][numtache]=AnsiString(tmp);
        // if (asAvanc->Cells[18][count]=="0") asAvanc->Cells[20][count] = "X";
        // if (asAvanc->Cells[18][count]=="100") asAvanc->Cells[21][count] = "X";

         // avccount++; if (avccount > 1) avact->RowCount++;
         Ghost->ExtractValue(tmp,buffer,"nt",0);
         numt=atoi(tmp);
         OKK = false;
         if (!concern) OKK=true;
         if (concern && IsConcern(numt)) OKK=true;
         if (OKK)
          {
           avccount++; if (avccount> 1) {avact->RowCount++; }
           Ghost->ExtractValue(tmp,buffer,"nt",0);
           avact->Cells[1][avccount] = AnsiString(tmp);
           Ghost->ExtractValue(tmp,buffer,"lb",0);
           avact->Cells[2][avccount] = AnsiString(tmp);

           avact->Cells[3][avccount] = asAvanc->Cells[20][count];
           avact->Cells[4][avccount] = asAvanc->Cells[21][count];

           avact->Cells[5][avccount]=asAvanc->Cells[18][count];
           avact->Cells[6][avccount]= AnsiString(10000-dist1); // AnsiString(10000-dist1);
           taches_concernees++;

           if (avact->Cells[3][avccount]=="" && avact->Cells[4][avccount]=="")
           avact->Colors[1][avccount]=clYellow; else avact->Colors[1][avccount]=clWhite;

           }


      } // if OK
    } // strcmp (T0000)
    rc = avanc->ReadNextKey(RetKey,&recdata);
   } // end while
 avanc->CloseEngine(); // avanc->~realisam();

 asAvanc->SortIndexes->Clear();
 
 AdvStringGrid6->SortIndexes->Clear();
 asAvanc->SortIndexes->Add(3);  // tri sur date debut reel
 AdvStringGrid6->SortIndexes->Add(3);
 asAvanc->QSortIndexed();
AdvStringGrid6->QSortIndexed();
 // Remplissage de avact en fonction  du contenu de  asAvanc

 //  taches_concernees = Fill_Avact(concern);



 // concern=true;
 // Refresh_Avanc_All();
 STRICT=false;
 Refresh_Avanc(concern,STRICT);
 // asAvanc->RowCount = count+1;

 fname->Caption = AnsiString(FileNameAvanc);
 Compute_Fin_Reelle();
 sprintf(tmp,"%d",taches_concernees);

 Update_Aff_Concernees(taches_concernees);



 btSavAct->Enabled=true;
 btSaveActu->Enabled = true;

 SAVE_AVANC=true;

 if (SAISIE_AVANC == true)
  {
   btSavAct->Enabled = true;
   asAvanc->Enabled = true;
   AdvStringGrid6->Enabled = true;
   AdvStringGrid4->Enabled = true;
   AdvStringGrid7->Enabled = true;
   AdvStringGrid5->Enabled = true;
   btSaveActu->Enabled = true;
   avas->Enabled = true;
   avact->Enabled = true;

  }
 else
  {
   btSavAct->Enabled = false;
   asAvanc->Enabled = false;
   AdvStringGrid6->Enabled = true;
   AdvStringGrid7->Enabled = true;
   AdvStringGrid4->Enabled = false;
   AdvStringGrid5->Enabled = false;
   btSaveActu->Enabled = false;
   avas->Enabled = false;
   avact->Enabled = false;

  }

 Compute_Retards();

 row_avancement = 1;
 Clean_Grid(AdvStringGrid4,2);
 Clean_Grid(AdvStringGrid7,2);


 return 0;

}

int __fastcall TplStandard::Fill_Avact(int concern)
{
 int i,cnt;  int nt; char tmp[50];   bool ok;
 Clean_Grid(avact,2);
 cnt = asAvanc->RowCount;
 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,asAvanc->Cells[1][i].c_str());
    nt = atoi(tmp); ok=false;
    if (nt!=0)
     {
       if (concern)
        {
         if (IsConcern(nt)) ok=true; else ok=false;

        }
      else
        {
         ok=true;
        }
     if (ok)
       {


       }

     } // if NT  

   }
 return 1;
}

bool __fastcall TplStandard::IsConcern(int nt)
{
 int i,cnt,row; char numtach[50];
 int dist1,dist2,dist3; int tache;

 row=0;
 cnt = asAvanc->RowCount;
 for (i=1;i<cnt;i++)
    {
     strcpy(numtach,asAvanc->Cells[1][i].c_str());
     tache = atoi(numtach);
     if (tache == nt) { row = i;  break; }
    }

if (row==0) return false;
// recherche dans asAvanc les caracteristiques

dist1 = Convert_Date(asAvanc->Cells[8][row]);     // 8 = debut prev 3= debut reel
dist2 = Convert_Date(AnsiString(DATEAVANC));
dist3 = Convert_Date(asAvanc->Cells[10][row]);   // 10= fin prev  5 = fin reelle

// dist1 =  Convert_Date(asAvanc->Cells[3][row]);
// dist3 = Convert_Date(asAvanc->Cells[5][row]);

asAvanc->Cells[23][row]="X";

if (dist1 >= dist2)
  {

    asAvanc->Cells[23][row]=""; return false;
  }
 // else return true;  
if (dist3 <= dist2)
 {

  asAvanc->Cells[23][row]="X"; return true;
  
 }

return true;
}

void __fastcall TplStandard::Update_Aff_Concernees(int tcon)
{
 int i,cnt; int tccconn;
  char tmp[50];

  tccconn = 0;
  cnt = asAvanc->RowCount;
  for (i=1;i<cnt;i++)
   {
    if (asAvanc->Cells[23][i]=="X") tccconn++;
   }
 sprintf(tmp,"%d",tccconn);
 Label22->Caption = AnsiString(tmp);
  Label150->Caption = AnsiString(tmp);
 Label124->Caption = AnsiString(tmp);
 Label55->Caption = AnsiString(tmp);
 Label97->Caption = AnsiString(tmp);
}

void __fastcall TplStandard::MAJ_ADV_AV()
{
 int i,cnt; char tmp[50]; int nt;
 cnt = asAvanc->RowCount;
 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,asAvanc->Cells[1][i].c_str());
    nt=atoi(tmp);
    if (nt != 0)
      {
       av->Cells[21][nt] = asAvanc->Cells[3][i];  // debut r�el
       av->Cells[22][nt] = asAvanc->Cells[5][i];  // fin r�elle
      }
   }
}

void __fastcall TplStandard::btDatActClick(TObject *Sender)
{
BTACTUAL = 1;
Exec_Avanc(false);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Exec_Avanc(bool skip)
{

 TActual *tac;

 char tmp[550];
 char dt1[20];char dt2[20];
 int i,j,ok,indx;
 int count,rc,nt,l,cnt,x;

 char tachconcern[200];
 realisam *avanc;
 char nameact[255];
 char *p;
 char Key[100],RetKey[100];
 int  recdata;
 char buffer[1000];
 char str[200];

 bool flag_new;
 char suffixe[250];



  lRetard->Caption = "";
  Label157->Caption = "";
  // $$$$$ dateavanc[0]=0;
 // avanc->CloseEngine(); avanc->~realisam();


 Label118->Caption =  "../../..";
 Label68->Caption =  "../../..";
 Label12->Caption =   "../../..";
 Label146->Caption = "../../..";
 Label106->Caption = "../../..";
 Label22->Caption =  "0";
 Label55->Caption =  "0";
 //Label118->Caption =  "0";
 Label124->Caption = "0";

 AVANC_DEL=false;
 FLAG_AVANC_LOADED = false;
 // Change1();

  if (!skip)
    {
     // sprintf(m_dateprev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
     strcpy(m_exchange,dateavanc);
     strcpy(m_exchange2,LocalBase);
     tac = new TActual(Application);
     tac->ShowModal();
     //if (BTACTUAL==1)  PageControl1->ActivePageIndex=7;
     //else PageControl1->ActivePageIndex=13;
    delete tac;
   }

 if (AVANC_DEL) //  && FLAG_AVANC_LOADED)
   {

    Clean_Actu();
    return;
   }


  if (skip) strcpy(m_exchange,m_special);

 //ReadExchange(1);
 l=Ghost->ExtractValue(tmp,m_exchange,"null",0);
 if (strcmp(tmp,"TRUE")==0) return;

  l=Ghost->ExtractValue(dt1,m_exchange,"da",0);
  if (strlen(dt1)==0) return;

  if (l!=10)
    {
     Application->MessageBoxA("Date Actualisation Incorrecte",m_ecoplan,MB_OK);
     return;
    }


  rc = verifdate(dt1);
  if (rc>0) return;
  Label12->Caption = AnsiString(std_date);
  Label146->Caption = AnsiString(std_date);
  Label118->Caption = AnsiString(std_date);
  Label68->Caption = AnsiString(std_date);
  AdvStringGrid7->Cells[1][1] = AnsiString(std_date);
  Clear_Diagram(imAvCal);
  Clear_Diagram(imAvGantt);



  l=Ghost->ExtractValue(tachconcern,m_exchange,"tc",0);
  concern=false;
  if (strcmp(tachconcern,"true")==0) concern=true;


  l=Ghost->ExtractValue(tmp,m_exchange,"new",0);

  flag_new=false; if(strcmp(tmp,"true")==0) flag_new=true;
  l=Ghost->ExtractValue(suffixe,m_exchange,"file",0);


  count=0;

 AnAvanc=AnVerif;MoisAvanc=MoisVerif;JourAvanc=JourVerif;
 // pour verifier les dates dt1 et dt2
 sprintf(dt1,"%02d-%02d-%02d",AnAvanc%100,MoisAvanc,JourAvanc);
 sprintf(DATEAVANC,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);
 // ou bien il existe deja un avancement, sinon, on convertit
 // en date, a partir de date au plus tot prev;

 // on nettoie la stringgrid !

 for (i=1;i<asAvanc->ColCount;i++){for (j=1;j<asAvanc->RowCount;j++) asAvanc->Cells[i][j]="";}
 asAvanc->RowCount=2;

 for (i=1;i<AdvStringGrid6->ColCount;i++){for (j=1;j<AdvStringGrid6->RowCount;j++) AdvStringGrid6->Cells[i][j]="";}
 AdvStringGrid6->RowCount=2;


 for (i=1;i<avact->ColCount;i++){for (j=1;j<=avact->RowCount;j++) avact->Cells[i][j]="";}
 avact->RowCount=2;
 // // lecture du fichier pour trouver les avancements possibles
 // examen des dates d'avancement de toutes les taches

  for (i=1;i<=AdvStringGrid4->ColCount;i++){for (j=1;j<=1;j++) AdvStringGrid4->Cells[i][j]="";}
  AdvStringGrid4->RowCount=2;

  for (i=1;i<=AdvStringGrid7->ColCount;i++){for (j=1;j<=1;j++) AdvStringGrid7->Cells[i][j]="";}
  AdvStringGrid7->RowCount=2;

  for (i=1;i<=AdvStringGrid6->ColCount;i++){for (j=1;j<=1;j++) AdvStringGrid6->Cells[i][j]="";}
  AdvStringGrid6->RowCount=2;

   for (i=1;i<AdvStringGrid5->ColCount;i++){for (j=1;j<AdvStringGrid5->RowCount;j++) AdvStringGrid5->Cells[i][j]="";}
  AdvStringGrid5->RowCount=2;

  for (i=1;i<avas->ColCount;i++){for (j=1;j<=1;j++) avas->Cells[i][j]="";}
  avas->RowCount=2;


  cnt= Explode('@',dateavanc);
  // copie des differentes dates dans un tableau;
  CntAV=cnt;    CntAVBefore = 0;
  for (i=0;i<CntAV;i++)
    {
      if (strcmp(suffixe+1,params[i])>=0)
        {
         CntAVBefore++;
         strcpy(TableDatesAV[i],params[i]);
        }
       else break;
    }

  if (cnt ==0)
     {flag_avanc=false;
      // on construit isam d'apres le previsionnel

      strcpy(tmp,Label12->Caption.c_str());
      p=strstr(tmp,"/"); if (p) *p='-';
      p=strstr(tmp,"/"); if (p) *p='-';

      Label12->Caption = AnsiString(tmp);
	  Label146->Caption = AnsiString(tmp);
      Label118->Caption = AnsiString(tmp);
      Label68->Caption = AnsiString(tmp);
	  AdvStringGrid7->Cells[1][1] = AnsiString(std_date);
      strcat(dateavanc,tmp);
      strcpy(TableDatesAV[0],dateavanc);

      //sprintf(tmp,"-%03d",1);
      //strcat(dateavanc,tmp);

      strcat(dateavanc,"@");

      x=1;
      sprintf(tmp,"%s-%02d-%02d-%02d",LocalBase,AnAvanc%100,MoisAvanc,JourAvanc);
      CntAV=1; CntAVBefore = 1;
      strcpy(FileNameAvanc,tmp);
      strcpy(isamname,tmp); strcat(isamname,".act"); unlink(isamname);
      strcpy(isamname,tmp); strcat(isamname,".don"); unlink(isamname);
      CopieAvancePrev(FileNameAvanc); //isamname);

     }
   else
     {
      if (flag_new)
       {
        count= Explode('@',dateavanc); strcpy(tmp,Label12->Caption.c_str());
        // dernier fichier dans params[count-1];
        CntAV++;
        CntAVBefore = CntAV;
        tmp[2]='-'; tmp[5]='-';
        /// echanger jour et ann�e
        Reverse_Date(tmp);
        strcpy(TableDatesAV[CntAV-1],tmp);


        strcat(dateavanc,tmp);

        //   sprintf(tmp,"-%03d@",count+1);  // NON
        // NON strcat(dateavanc,tmp);
        // strcat(TableDatesAV[CntAV-1],tmp);
        l= strlen(TableDatesAV[CntAV-1]);
        if (l>0) TableDatesAV[CntAV-1][l-1]=0;
        sprintf(isamname,"%s-%02d-%02d-%02d",LocalBase,AnAvanc%100,MoisAvanc,JourAvanc);
        strcpy(FileNameAvanc,isamname);
        sprintf(suffixe,"-%02d-%02d-%02d",AnAvanc%100,MoisAvanc,JourAvanc);  // $$$ +1

        strcpy(tmp,isamname); strcat(tmp,".act"); unlink(tmp); // delete just in case
        strcpy(tmp,isamname); strcat(tmp,".don"); unlink(tmp); // delete former
        CopieAvance(suffixe,FileNameAvanc);
       }

      else // on a le fichier ancien
       {

        strcpy(FileNameAvanc,LocalBase);
        strcat(FileNameAvanc,suffixe);
       }
     }


 //  Change1();

 //  if (CntAV>1) concern = true; else concern = false;


 avanc = new  realisam();    // ne pas enlever, sinon erreur


 /// $$$$$$$$$$$$$$$$$$$$$ rc = avanc->OpenEngine(FileNameAvanc,120,"act","don");   //512 - 8
 /// $$$$$$$$$$$$$$$$$$$$$ rc = avanc->SetIndexMode(0,0);
 //int  nbrec = avanc->NumberOfKeys(0);

//    Compute_DateDebutPossible();

  btSavAct->Enabled = true;
  fname->Caption = AnsiString(FileNameAvanc);

  FLAG_AVANC_LOADED = true;
  sprintf(tmp,"%d",count);
  //  Label22->Caption = AnsiString(tmp);

  Load_Avanc(FileNameAvanc,false);
  btActRapidClick(NULL);
  // $$$$$$$$$$$$$$

  //  Remplir_asTri();  // �������������

  Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
  Graph_Avanc(AdvStringGrid1->RowCount-1,MAXDIST);


  // $$$$ Save_Actualisation(false);

// $$$$   Planning_Defaut();

}



void __fastcall TplStandard::btSavActClick(TObject *Sender)
{
 FillListBoxA();
 updatedatabase(LocalBase,0,0);
 Save_Actualisation(true);
 ReloadAllData(LocalBase,true,false);
 // Load_Avanc(FileNameAvanc,false);
}
                                        
void __fastcall TplStandard::Save_Actualisation(bool msg)
{
 // preparer nouvelle date pour string dateavanc.
 int count; int ix,ij; char tmp[250]; char cnt[100];
 int rc; int nt;  char bufx[200];
 char Key[100]; char RetKey[100];
 int recdata;  FILE *fp;
 char fname[MAX_PATH];
 int distbr;

// int nbrec;

 // boucle pour sauver les valeurs de asAvanc

 asAvanc->SortIndexes->Clear();
 asAvanc->SortIndexes->Add(1);
 asAvanc->QSortIndexed();

 AdvStringGrid6->SortIndexes->Clear();
 AdvStringGrid6->SortIndexes->Add(1);
 AdvStringGrid6->QSortIndexed();

 count = asAvanc->RowCount;

 // avanc->CloseEngine(); avanc->~realisam();
 avanc = new  realisam();    // ne pas enlever, sinon erreur
 rc = avanc->OpenEngine(FileNameAvanc,120,"act","don");   //512 - 8
 rc = avanc->SetIndexMode(0,0);

 strcpy(fname,FileNameAvanc); strcat(fname,".dbr");
 fp = fopen(fname,"wb");

 for (ix=1;ix<count;ix++)
  {
   getvalue(5,ix,39); nt=atoi(adv);
   sprintf(Key,"T%04d",nt);
   buffer[0]=0;

      getvalue(5,ix,39); concat("nt");
      getvalue(5,ix,70); concat("da");
      getvalue(5,ix,40); concat("lb");
      getvalue(5,ix,41); concat("ddr");
      getvalue(5,ix,42); concat("dr");
      getvalue(5,ix,43); concat("dfr");
      getvalue(5,ix,44); concat("rimp");
      getvalue(5,ix,45); concat("rtot");
      getvalue(5,ix,46); concat("ddp");
      getvalue(5,ix,47); concat("dp");
      getvalue(5,ix,48); concat("dfp");
      getvalue(5,ix,62); concat("cal");
      getvalue(5,ix,66); concat("mt");
      getvalue(5,ix,69); concat("dtposs");
      getvalue(5,ix,71); concat("prest");
      getvalue(5,ix,72); concat("jrest");
      getvalue(5,ix,73); concat("margrl");

      strcpy(adv,asAvanc->Cells[20][ix].c_str()); concat("zero");
      strcpy(adv,asAvanc->Cells[21][ix].c_str()); concat("cent");
      strcpy(adv,asAvanc->Cells[24][ix].c_str()); concat("fix");
      strcpy(adv,asAvanc->Cells[25][ix].c_str()); concat("ctach");

      strcpy(adv,asAvanc->Cells[22][ix].c_str()); concat("brisee");
      distbr = atoi(adv);
      sprintf(bufx,"<T>%04d</T><BR>%d</BR>\n",nt,distbr);
      fputs(bufx,fp);

    rc=avanc->ReadDirectKey(0,Key,RetKey,&recdata);
   if (rc)
     { rc =avanc->RewriteRecord(buffer,strlen(buffer)+1,recdata);
     }
   else
     {
      recdata = avanc->GetNewRecordNumber();
      rc = avanc->WriteKey(0,Key,recdata);
      rc= avanc->WriteRecord(buffer,strlen(buffer)+1);
     }

  //  else Application->MessageBoxA("Tache non trouv�e dans Fichier",m_ecoplan,MB_OK);
  }

 avanc->CloseEngine(); // avanc->~realisam();
 fclose(fp);
 // delete avanc;

/*
 avanc = new  realisam();    // ne pas enlever, sinon erreur
 rc = avanc->OpenEngine(FileNameAvanc,120,"act","don");   //512 - 8
 rc = avanc->SetIndexMode(0,0);
*/

// re-trier par date debut r�el 
asAvanc->SortIndexes->Clear();
 asAvanc->SortIndexes->Add(3);
 asAvanc->QSortIndexed();
AdvStringGrid6->SortIndexes->Clear();
 AdvStringGrid6->SortIndexes->Add(3);
 AdvStringGrid6->QSortIndexed();

 
 FLAG_AVANC_LOADED = true;
 SAVE_AVANC=true;
 if (msg)
     Application->MessageBoxA("Actualisation Sauvegard�e",m_ecoplan,MB_OK);
}
//---------------------------------------------------------------------------

int __fastcall TplStandard::concat(char *tag)
{
 strcat(buffer,"<"); strcat(buffer,tag); strcat(buffer,">");
 strcat(buffer,adv);
 strcat(buffer,"</"); strcat(buffer,tag); strcat(buffer,">");
 return 0;
 }


void __fastcall TplStandard::SB1Scroll(TObject *Sender,
      TScrollCode ScrollCode, int &ScrollPos)
{
 int x1,x2;
 //x2=SB1->Position;
 //$$$$x1=int (ScrollBar1->Position*nb_taches/100)+1;

 DrawMiniGantt(current_task,0,immGantt,false);

}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::ScrollBar5Scroll(TObject *Sender,
      TScrollCode ScrollCode, int &ScrollPos)
{
 int x;
 x=ScrollBar5->Position - 50;
 DrawMiniGantt(current_task,0);
}
*/


void __fastcall TplStandard::Remplir_asTri()
{
 char Test;
 int i,j, count,cnt,ntach;
 AnsiString Debug; char tmp[50];
 char dt1[10],dt2[10];

 Debug = P_FIELD[pc][0];
 Clean_Grid(asTri,2);
 cnt=0;
 // for (i=1;i<=nb_taches;i++)
   for (i=1;i<=LASTTASK;i++)
   {
    //  &&&&& ntach = sommets[i];
    ntach=i;
    Debug = av->Cells[1][ntach];
    if(av->Cells[1][ntach] !="")
    {
     Test = filtr[ntach];
     if (Test == 'X')

     {
      cnt++;
      if (cnt>1) asTri->RowCount++;
      asTri->Cells[0][cnt]=" ";  // normal

       if (av->Cells[21][ntach]=="")
         {
          av->Cells[21][ntach]=av->Cells[4][ntach];
          av->Cells[22][ntach]=av->Cells[5][ntach];
         }

      count = 75; // av->ColCount;
      for (j=1;j<count;j++) asTri->Cells[j][cnt]=av->Cells[j][ntach];
      // remplir les zones de tri :

      if (coche[ntach]==' ')  asTri->Cells[0][cnt]=" "; else
         asTri->Cells[0][cnt] = AnsiString(strcoche);
      Debug = asTri->Cells[41][cnt]; //av->Cells[1][ntach];
      asTri->Cells[70][cnt] = "AAA";
      asTri->Cells[71][cnt] = asTri->Cells[41][cnt];
      Debug = asTri->Cells[71][cnt];
      asTri->Cells[72][cnt] = asTri->Cells[42][cnt];
      Debug = asTri->Cells[72][cnt];
      asTri->Cells[73][cnt] = asTri->Cells[43][cnt];
      asTri->Cells[74][cnt] = asTri->Cells[44][cnt];
      // convert Debut Plus Tot
      strcpy(dt1,asTri->Cells[4][cnt].c_str());
      dt2[0]=dt1[6]; dt2[1]=dt1[7]; dt2[2]=dt1[3];
      dt2[3]=dt1[4]; dt2[4]=dt1[0]; dt2[5]=dt1[1];

      dt2[6]=0;

      //// ���� $$$$ Tri par date
      //// asTri->Cells[75][cnt] = AnsiString(dt2);
      sprintf(tmp,"%04d",ntach);
      asTri->Cells[75][cnt] =  AnsiString(tmp);

     }
    }  // if not ""
   }

asTri->QSort();

NB_TACHES = cnt;
Debug = P_FIELD[pc][0];
}


void __fastcall TplStandard::Remplir_asTri_Avanc()
{
 char Test;
 int i,j, count,cnt,ntach;
 AnsiString Debug;
 char dt1[10],dt2[10];
 int nt; char ntaches[50];
 int xcount;
 Debug = P_FIELD[pc][0];
 Clean_Grid(asTri,2);
 cnt = 0;
 xcount=asAvanc->RowCount;

 for (i=1;i<xcount;i++)
   {
    strcpy(ntaches,asAvanc->Cells[1][i].c_str());
    ntach = atoi(ntaches);

    if (asAvanc->Cells[23][i] == "") ntach=0;

    Debug = av->Cells[1][ntach];
    if(ntach != 0)
    {
      cnt++;
      if (cnt>1) asTri->RowCount++;
      asTri->Cells[0][cnt]=" ";  // normal

       if (av->Cells[21][ntach]=="")
         {
          av->Cells[21][ntach]=asAvanc->Cells[4][ntach];
          av->Cells[22][ntach]=asAvanc->Cells[5][ntach];
         }

      count = 75; // av->ColCount;
      for (j=1;j<count;j++) asTri->Cells[j][cnt]=av->Cells[j][ntach];
      // remplir les zones de tri :

      asTri->Cells[0][cnt]=" ";
      Debug = asTri->Cells[41][cnt]; //av->Cells[1][ntach];
      asTri->Cells[70][cnt] = "AAA";
      asTri->Cells[71][cnt] = asTri->Cells[41][cnt];

     }
    
   }  // for

asTri->QSort();

NB_TACHES = cnt;
Debug = P_FIELD[pc][0];
}



void __fastcall TplStandard::Remplir_asCrit()
{
 char Test; char tmp[150];
 int i,j, count,cnt,ntach,tch;
 AnsiString Debug;

 Clean_Grid(asCrit,2);
 cnt=0;
 // for (i=1;i<=nb_taches;i++)
   for (i=1;i<=LASTTASK;i++)
   {
    ntach = i;   // &&&&& sommets[i];
    Debug = av->Cells[1][ntach];
    if(av->Cells[1][ntach] !="")
    {
     Test = filtr[ntach];
     if (Test == 'X')

     {
      cnt++;
      if (cnt>1) asCrit->RowCount++;
      strcpy(tmp,av->Cells[1][ntach].c_str());
       tch=atoi(tmp);
      asCrit->Cells[1][cnt] = AnsiString(tch);  // av->Cells[1][ntach];
      asCrit->Cells[2][cnt] = av->Cells[2][ntach];
      asCrit->Cells[3][cnt] = av->Cells[41][ntach];
      asCrit->Cells[4][cnt] = av->Cells[51][ntach];
      asCrit->Cells[5][cnt] = av->Cells[42][ntach];
      asCrit->Cells[6][cnt] = av->Cells[52][ntach];
      asCrit->Cells[7][cnt] = av->Cells[43][ntach];
      asCrit->Cells[8][cnt] = av->Cells[53][ntach];
      asCrit->Cells[9][cnt] = av->Cells[44][ntach];
      asCrit->Cells[10][cnt] = av->Cells[54][ntach];
      asCrit->Cells[11][cnt] = av->Cells[45][ntach];
      asCrit->Cells[12][cnt] = av->Cells[55][ntach];
      asCrit->Cells[13][cnt] = av->Cells[46][ntach];
      asCrit->Cells[14][cnt] = av->Cells[56][ntach];
      asCrit->Cells[15][cnt] = av->Cells[47][ntach];
      asCrit->Cells[16][cnt] = av->Cells[57][ntach];
      asCrit->Cells[17][cnt] = av->Cells[48][ntach];
      asCrit->Cells[18][cnt] = av->Cells[58][ntach];


     }
    }  // if not ""
   }

}

                                         /*
void __fastcall TplStandard::Remplir_AV1()
{
 int i,j, count,cnt; char tmp[150]; int tx;
 AnsiString Debug;
 Clean_Grid(AdvStringGrid1,2);
 Clean_Grid(avc,2);

 // cnt=0;
 count = asTri->RowCount;
for (i=1;i<count;i++)
  {
   if (i>1) { AdvStringGrid1->RowCount++;  avc->RowCount++; }
   for (j=1;j<20;j++) AdvStringGrid1->Cells[j][i]=asTri->Cells[j][i];  // 0 pour les coches

   refresh_marges();

   strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
   tx=atoi(tmp);
   avc->Cells[1][i]=av->Cells[1][tx];
   avc->Cells[2][i]=av->Cells[2][tx];
   avc->Cells[3][i]=av->Cells[3][tx];
   avc->Cells[4][i]=av->Cells[4][tx];
   avc->Cells[5][i]=av->Cells[5][tx];
   avc->Cells[6][i]=av->Cells[84][tx];
   avc->Cells[7][i]=av->Cells[83][tx];
   avc->Cells[8][i]=av->Cells[41][tx];
   avc->Cells[9][i]=av->Cells[42][tx];
   avc->Cells[10][i]=av->Cells[43][tx];
  }

AdvStringGrid1->SortIndexes->Clear();
AdvStringGrid1->SortIndexes->Add(1);
AdvStringGrid1->QSortIndexed();

Debug = P_FIELD[pc][0];
Calcul_Echeancier();
}
                                        */

void __fastcall TplStandard::Remplir_AV1()
{
 int i,j, ndx, count,cnt; char tmp[150]; int tx;
 AnsiString Debug;
 Clean_Grid(AdvStringGrid1,2);
 Clean_Grid(avc,2);

 ndx=0;
 count = av->RowCount;
for (i=1;i<count;i++)
  {
   if (av->Cells[1][i] != "" && filtr[i]=='X')
     {
         ndx++; if (ndx>1){ AdvStringGrid1->RowCount++;  avc->RowCount++; }
       //  for (j=1;j<20;j++) AdvStringGrid1->Cells[j][ndx]=av->Cells[j][i];  // 0 pour les coches

       // for (j=1;j<15;j++) AdvStringGrid1->Cells[j][ndx]=av->Cells[j][i];

       for (j=1;j<15;j++) AdvStringGrid1->Cells[j][ndx]= av->Cells[j][i];
     // S =  av->Cells[42][i];

AdvStringGrid1->Cells[14][ndx]= av->Cells[19][i];
AdvStringGrid1->Cells[18][ndx]= asCrit->Cells[3][i];
AdvStringGrid1->Cells[19][ndx]= asCrit->Cells[5][i];
AdvStringGrid1->Cells[20][ndx]= asCrit->Cells[7][i];
AdvStringGrid1->Cells[21][ndx]= asCrit->Cells[9][i];
AdvStringGrid1->Refresh();

       avc->Cells[1][ndx]=av->Cells[1][i];
       avc->Cells[2][ndx]=av->Cells[2][i];
       avc->Cells[3][ndx]=av->Cells[3][i];
       avc->Cells[4][ndx]=av->Cells[4][i];
       avc->Cells[5][ndx]=av->Cells[5][i];
       avc->Cells[6][ndx]=av->Cells[84][i];
       avc->Cells[7][ndx]=av->Cells[83][i];
       avc->Cells[8][ndx]=asCrit->Cells[9][i];
       avc->Cells[9][ndx]=asCrit->Cells[3][i];
       avc->Cells[10][ndx]=asCrit->Cells[5][i];
       avc->Cells[11][ndx]=asCrit->Cells[7][i];
     }
  }

refresh_marges();

/*
AdvStringGrid1->SortIndexes->Clear();
AdvStringGrid1->SortIndexes->Add(1);
AdvStringGrid1->QSortIndexed();
*/
Debug = P_FIELD[pc][0];
Calcul_Echeancier();
}


int  __fastcall TplStandard::Exec_Alpha(char *ch,char *cond,char *v1,char *v2,int col)
{
 int result;
 result=0;
 int len;

 if (strcmp(cond,"egal �")==0)
   {
    len = strlen(ch);
    len = strlen(v1);
    if(strcmp(ch,v1)==0)
    result=1;
   }
 else if (strcmp(cond,"diff�rent de")==0)
   {
    if (strcmp(ch,v1)!=0)
    result=1;
   }
 else if (strcmp(cond,"inf�rieur �")==0)
   {
    if (strcmp(ch,v1) <0)
    result=1;
   }
 else if (strcmp(cond,"inf�rieur ou �gal �")==0)
   {
    if (strcmp(ch,v1)<=0)
    result=1;
   }
 else if (strcmp(cond,"sup�rieur �")==0)
   {
    if (strcmp(ch,v1) > 0)
    result=1;
   }
 else if (strcmp(cond,"sup�rieur ou �gal �")==0)
   {
    if (strcmp(ch,v1) >= 0)
    result=1;
   }
 else if (strcmp(cond,"compris entre")==0)
   {
    if ((strcmp(ch,v1)>=0)&&(strcmp(ch,v2)<=0))
    result=1;

   }
 else if (strcmp(cond,"non compris entre")==0)
   {
    if ((strcmp(ch,v1)<= 0) ||(strcmp(ch,v2)>=0))
    result=1;
   }
 else if (strcmp(cond,"contient")==0)
   {
    if (strstr(ch,v1)) result=1;
   }
 else if (strcmp(cond,"ne contient pas")==0)
   {
    if (!strstr(ch,v1)) result = 1;
   }
 else if (strcmp(cond,"contient masque")==0)
   {
    if (strstr(ch,v1)) result=1;
   }
 else if (strcmp(cond,"ne contient pas masque")==0)
   {
    if (!strstr(ch,v1)) result = 1;
   }


 return result;
}

int  __fastcall TplStandard::Exec_Num(char *ch,char *cond,char *v1,char *v2)
{
 int result,x1,x2,c;
 result=0;
 c=atoi(ch); x1=atoi(v1); x2=atoi(v2);

 if (strcmp(cond,"egal �")==0)
   { if (c==x1) result=1;
   }
 else if (strcmp(cond,"diff�rent de")==0)
   {
    if (c!=x2) result=1;
   }
 else if (strcmp(cond,"inf�rieur �")==0)
   {
    if (c<x1) result=1;
   }
 else if (strcmp(cond,"inf�rieur ou �gal �")==0)
   {
    if (c<=x1) result=1;
   }
 else if (strcmp(cond,"sup�rieur �")==0)
   {
    if (c>x1) result=1;
   }
 else if (strcmp(cond,"sup�rieur ou �gal �")==0)
   {
    if (c>=x1) result=1;
   }
 else if (strcmp(cond,"compris entre")==0)
   {
    if ((c>=x1) && (c<=x2)) result=1;
   }
 else if (strcmp(cond,"non compris entre")==0)
   {
    if ((c<=x1) || (c>=x2)) result = 1;
   }
 else if (strcmp(cond,"contient")==0)
   {

   }
 else if (strcmp(cond,"ne contient pas")==0)
   {

   }
 else if (strcmp(cond,"contient masque")==0)
   {

   }
 else if (strcmp(cond,"ne contient pas masque")==0)
   {

   }


 return result;
}

/*
int  __fastcall TplStandard::Exec_Date(char *ch,char *cond,char *v1,char *v2)
{
 int result;  char c[15],d[15],f[15];
  result=0;
 // convertir date au format correct
 c[0]=ch[6]; c[1]=ch[7]; c[2]=ch[3]; c[3]=ch[4]; c[4]=ch[0]; c[5]=ch[1]; c[6]=0;
 d[0]=v1[6]; d[1]=v1[7]; d[2]=v1[3]; d[3]=v1[4]; d[4]=v1[0]; d[5]=v1[1]; d[6]=0;
 f[0]=v2[6]; f[1]=v2[7]; f[2]=v2[3]; f[3]=v2[4]; f[4]=v2[0]; f[5]=v2[1]; f[6]=0;


 if (strcmp(ch,"16/11/09")==0)
   result=result;


 if (strcmp(cond,"egal �")==0)
   {
       if(strcmp(c,d)==0) result=1;
   }
 else if (strcmp(cond,"diff�rent de")==0)
   {
    if (strcmp(c,d)!=0) result=1;
   }
 else if (strcmp(cond,"inf�rieur �")==0)
   {
    if (strcmp(c,d) <0) result=1;
   }
 else if (strcmp(cond,"inf�rieur ou �gal �")==0)
   {
    if (strcmp(c,d)<=0) result=1;
   }
 else if (strcmp(cond,"sup�rieur �")==0)
   {
    if (strcmp(c,d) > 0) result=1;
   }
 else if (strcmp(cond,"sup�rieur ou �gal �")==0)
   {
    if (strcmp(c,d) >= 0) result=1;
   }
 else if (strcmp(cond,"compris entre")==0)
   {
    if ((strcmp(c,d)>=0)&&(strcmp(c,f)<=0)) result=1;
   }
 else if (strcmp(cond,"non compris entre")==0)
   {
    if ((strcmp(c,d)<= 0) ||(strcmp(c,f)>=0)) result=1;
   }
 else if (strcmp(cond,"contient")==0)
   {
    if (strcmp(c,d)==0) result = 1;
   }
 else if (strcmp(cond,"ne contient pas")==0)
   {
    if (strstr(c,d)== NULL) result=1;
   }
 else if (strcmp(cond,"contient masque")==0)
   {
    // if (strstr(
   }
 else if (strcmp(cond,"ne contient pas masque")==0)
   {

   }

   return result;
}
*/
int  __fastcall TplStandard::Exec_Date(char *ch,char *cond,char *v1,char *v2)
{
 int result;  char c[15],d[15],f[15];
 int dist,dist1,dist2;
 int yy,mm,dd;

  result=0;
 // convertir date au format correct
 //c[0]=ch[6]; c[1]=ch[7]; c[2]=ch[3]; c[3]=ch[4]; c[4]=ch[0]; c[5]=ch[1]; c[6]=0;
 //d[0]=v1[6]; d[1]=v1[7]; d[2]=v1[3]; d[3]=v1[4]; d[4]=v1[0]; d[5]=v1[1]; d[6]=0;
 //f[0]=v2[6]; f[1]=v2[7]; f[2]=v2[3]; f[3]=v2[4]; f[4]=v2[0]; f[5]=v2[1]; f[6]=0;

 if (strcmp(ch,"20/04/09")==0)
  result=result;

  if (strcmp(ch,"16/11/09")==0)
  result=result;


 c[0]=ch[6]; c[1]=ch[7]; c[2]=0;  yy=atoi(c);
 c[0]=ch[3]; c[1]=ch[4]; c[2]=0;  mm=atoi(c);
 c[0]=ch[0]; c[1]=ch[1]; c[2]=0;  dd=atoi(c);
 dist = yy*366 + mm*31 +dd;

 c[0]=v1[6]; c[1]=v1[7]; c[2]=0;  yy=atoi(c);
 c[0]=v1[3]; c[1]=v1[4]; c[2]=0;  mm=atoi(c);
 c[0]=v1[0]; c[1]=v1[1]; c[2]=0;  dd=atoi(c);
 dist1 = yy*366 + mm*31 +dd;

 c[0]=v2[6]; c[1]=v2[7]; c[2]=0;  yy=atoi(c);
 c[0]=v2[3]; c[1]=v2[4]; c[2]=0;  mm=atoi(c);
 c[0]=v2[0]; c[1]=v2[1]; c[2]=0;  dd=atoi(c);
 dist2 = yy*366 + mm*31 +dd;


 if (strcmp(ch,"16/11/09")==0)
   result=result;


 if (strcmp(cond,"egal �")==0)
   {
      if (dist==dist1)result=1;
   }
 else if (strcmp(cond,"diff�rent de")==0)
   {
    if (dist != dist1) result=1;
   }
 else if (strcmp(cond,"inf�rieur �")==0)
   {
    if (dist < dist1) result=1;
   }
 else if (strcmp(cond,"inf�rieur ou �gal �")==0)
   {
    if (dist <= dist1) result=1;
   }
 else if (strcmp(cond,"sup�rieur �")==0)
   {
    if (dist > dist1) result=1;
   }
 else if (strcmp(cond,"sup�rieur ou �gal �")==0)
   {
    if (dist >= dist1) result=1;
   }
 else if (strcmp(cond,"compris entre")==0)
   {
    if (dist >= dist1 && dist <=dist2) result=1;
   }
 else if (strcmp(cond,"non compris entre")==0)
   {
    if (dist < dist1 ||dist >=dist2) result=1;
   }
 else if (strcmp(cond,"contient")==0)
   {
    if (dist==dist1) result = 1;
   }

 //else if (strcmp(cond,"ne contient pas")==0)
 //  {
 //   if (strstr(c,d)== NULL) result=1;
 //  }
 //else if (strcmp(cond,"contient masque")==0)
 //  {
    // if (strstr(
 //  }
 //else if (strcmp(cond,"ne contient pas masque")==0)
 //  {

 //  }

   return result;
}

int __fastcall TplStandard::Ajouter_Tirets(char *val)
{
 int i,nbt,lg; char *p;
 lg=strlen(val); nbt=0;
 for (i=0;i<lg;i++)
  {
   if (val[i]=='-') nbt++;
  }
 if (nbt==0) strcat(val,"----");
 if (nbt==1) strcat(val,"---");
 if (nbt==2) strcat(val,"--");
 if (nbt==3) strcat(val,"-");
 return nbt;
}


void __fastcall TplStandard::Exec_Pas_Pas(char *str)
{
 int col; char lib[250];  int cnt,i,val,result;
 char condition[50]; char champ[100]; char init[10];
 char val1[50],val2[50];
 char action[50]; char tmp[250];
 int cntav;

 int cfi,cfg;

 Ghost->ExtractValue(lib,str,"C",0);
 Ghost->ExtractValue(condition,str,"R",0);
 strlwr(condition);
 Ghost->ExtractValue(val1,str,"V1",0);
 Ghost->ExtractValue(val2,str,"V2",0);
 Ghost->ExtractValue(action,str,"A",0);
 Ghost->ExtractValue(init,str,"I",0);
 strlwr(action);

 cntav=av->RowCount;

 for (i=0;i<cntav;i++) filtint[i]=' ';
 if (strcmp(init,"T")==0)
   {     for (i=1;i<cntav;i++) filtr[i]='X'; }
 if (strcmp(init,"N")==0)
   {     for (i=1;i<=cntav;i++) filtr[i]=' '; }

 //Application->MessageBoxA(lib,"Pas a Pas",MB_OK);
 if (lib[0]=='*')
   {
    col= RechercheColCrit(lib+1);
    if (col==0) return;
    if (strlen(val1)) Ajouter_Tirets(val1);
    if (strlen(val2)) Ajouter_Tirets(val2);
    col=col+40;
    strcpy(CONTROL,"A");
   }
 else
    col = RechercheCol(lib);
 if (col==0) return;
 // CONTROL contient le type de donn�es  A,N,D

 cnt=av->RowCount;
 cfi=0; cfg=0;
 for (i=1;i<cnt;i++)
   {
    strcpy(champ,av->Cells[col][i].c_str());
    if (strlen(champ)==0) { continue; }
    if (strcmp(CONTROL,"A")==0)
     {  result = Exec_Alpha(champ,condition,val1,val2,col);
        if (result) filtint[i]='X';
        //sprintf(tmp,"Result=%d  Champ=%s Val1=%s",result,champ,val1);
        //Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
     }
    if (strcmp(CONTROL,"N")==0)
     {  result = Exec_Num(champ,condition,val1,val2);
        if (result) filtint[i]='X';
     }
    if (strcmp(CONTROL,"D")==0)
     {  result = Exec_Date(champ,condition,val1,val2);
        if (result) filtint[i]='X';
     }

     if (result) cfi++;

     if (!result) filtint[i]=' ';

     if ((strcmp(action,"ajouter")==0)&&(result))
         { filtr[i]='X';   cfg++; }
     if ((strcmp(action,"supprimer")==0) &&(result))
         { filtr[i]=' '; cfg--; }
     if ((strcmp(action,"ajouter sauf")==0) &&  (!result) && (filtr[i]=='X'))
         { filtr[i]='X';  cfg++; }
     if ((strcmp(action,"supprimer sauf")==0) && (!result) &&(filtr[i]=='X'))
         { filtr[i]=' '; cfg--; }
   }
 cnt=av->RowCount;

}


int __fastcall TplStandard::Exec_Filter(int mod, char *filter)
{
 // on nettoye toutes les "grids"
 char fxname[250];
 int i; int nbt,t,count;
 int rc,recdata;
 char RetKey[100],tmp[1000];
 char *p;
// Init_Grids();


 if (strncmp(filter,"<N>",3) == 0)
   { // effacer toutes les taches
    for (i=0;i<MAX_T;i++) filtr[i]=' ';
    NB_TACHES = 0;
    nb_tachfiltr=0;
    Clean_Grid(asTri,2); // ne pas remplir
    Clean_Grid(AdvStringGrid1,2);
    Clean_Grid(asCrit,2);
   }
 if (strncmp(filter,"<T>",3)==0)
   { // prendre toutes les taches
    for (i=1;i<=nb_taches;i++) filtr[i]='X';
    nb_tachfiltr=nb_taches;
    Remplir_asTri();
    Remplir_AV1();
    Remplir_asCrit();
   }

 if (strncmp(filter,"<P>",3)==0)
   {
    Exec_Pas_Pas(filter);
    Remplir_asTri();
    Remplir_AV1();
    Remplir_asCrit();
    if (mod !=0)
    {
    PAGE_RAPPORT[pc]=1;
    switch (pc)
      {
       case 0: Prepare_Draw_Planning(); ZoomCopy0(imP0p,0,0); break;
       case 1: Prepare_Draw_Planning(); ZoomCopy1(imP1p,0,0); break;
       case 2: Prepare_Draw_Planning(); ZoomCopy2(imP2p,0,0); break;
       case 3: Prepare_Draw_Planning(); ZoomCopy3(imP3p,0,0); break;
       //case 3: Prepare_Draw_Planning(); ZoomCopyAv(imPort,0,0); break;
       case 4: Prepare_Draw_Rapport(1); ZoomCopy4(imR1p,0,0); break;
       // case 5: Prepare_Draw_Rapport(1); ZoomCopy5(imR2p,0,0); break;
      }
    }  // mod <> 0
   }

 if (strncmp(filter,"<E>",3)==0)
   {
    Ghost->ExtractValue(tmp,filter,"criteres",0);
    count = Explode('|',tmp); // result dans params
    for (i=0;i<count;i++)
      {
       Exec_Pas_Pas(params[i]);
      }

    sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt><exec>O</exec>",pc,
    P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
    strcpy(m_planning,tmp);


    Remplir_asTri();
    Remplir_AV1();
    Remplir_asCrit();
    if (mod != 0)
     {
      PAGE_RAPPORT[pc]=1;
      switch (pc)
      {
       case 0: Prepare_Draw_Planning(); ZoomCopy0(imP0p,0,0); break;
       case 1: Prepare_Draw_Planning(); ZoomCopy1(imP1p,0,0); break;
       case 2: Prepare_Draw_Planning(); ZoomCopy2(imP2p,0,0); break;
       case 3: Prepare_Draw_Planning(); ZoomCopy3(imP3p,0,0); break;
       // case 3: Prepare_Draw_Planning(); ZoomCopyAv(imPort,0,0); break;
       case 4: Prepare_Draw_Rapport(1); ZoomCopy4(imR1p,0,0); break;
       //case 5: Prepare_Draw_Rapport(1); ZoomCopy5(imR2p,0,0); break;
      }
     } // mod <> 0
   }

 nb_tachfiltr=0;
 for (i=1;i<=LASTTASK;i++)
  {
   if (av->Cells[1][i] != "")
     {
      if (filtr[i]=='X') nb_tachfiltr++;
     }
  }

 Label17->Caption = IntToStr(nb_tachfiltr);
 Label67->Caption = IntToStr(nb_tachfiltr);
 Label62->Caption = IntToStr(nb_tachfiltr);
 Label48->Caption = IntToStr(nb_tachfiltr);
  Label113->Caption = IntToStr(nb_tachfiltr);
 return 0;
}

void __fastcall TplStandard::Copy_Col(int c1,int c2,char *typ,int numcrit)
{
 int count,i,n;  char tmp[200],str[100];
 char d[15],x[20]; char c;
 char *p0,*p1,*p2,*p3;
 char tmp0[100],tmp1[100],tmp2[100],tmp3[100];

 //int nivo;


 count=asTri->RowCount;
 for (i=1;i<count;i++)
   {
    if (strcmp(typ,"N")==0)  // donn�es normales
      {
       strcpy(str,asTri->Cells[c1][i].c_str());
       n=atoi(str);
       sprintf(tmp,"%08d",n);
       asTri->Cells[c2][i] = AnsiString(tmp);
      }
    else if (strcmp(typ,"D")==0)  // donn�es de type Date
      {
       strcpy(x,asTri->Cells[c1][i].c_str());
       d[0]=x[6];
       d[1]=x[7];
       d[2]=x[3];
       d[3]=x[4];
       d[4]=x[0];
       d[5]=x[1];
       d[6]=0;
       asTri->Cells[c2][i] = AnsiString(d);
      }
    else
      {
       if (numcrit)
        {
         // nivo = 0;
         strcpy(tmp,asTri->Cells[c1][i].c_str());
         // extraire le niveau  d'apres les criteres

         if (numcrit > 1)
           {
            numcrit = numcrit;
           }

         p0 = strstr(tmp,"-");
         if (p0)
           { strcpy(tmp0,tmp);
            tmp0[p0-tmp]= 0;
             p1 = strstr(p0+1,"-");
             if (p1)
               {
                strcpy(tmp1,tmp);
                tmp1[p1-tmp]=0;
                p2=strstr(p1+1,"-");
                if (p2)
                  {
                   strcpy(tmp2,tmp);
                   tmp2[p2-tmp]=0;
                   p3=strstr(p2+1,"-");
                   if (p3)
                     {
                      strcpy(tmp3,tmp);
                      tmp3[p3-tmp] = 0;
                     }

                  }

               }

           }

         /*
         p1 = strstr(tmp,"----");
         if (p1) { nivo = 0; *p1 = 0; }
           else
              {
               p1 = strstr(tmp,"---");
               if (p1) { nivo = 1; *p1 = 0; }
                  else
                    {
                     p1 = strstr(tmp,"--");
                     if (p1) { nivo = 2; *p1 = 0; }
                        else
                          {
                           nivo = 3;  tmp[strlen(tmp)-1] = 0;  // on enleve le dernier "-"
                          }
                    }
              }


         if (P_NIV0[pc][numcrit-1]) strcpy(tmp,tmp0);
         if (P_NIV1[pc][numcrit-1]) strcpy(tmp,tmp1);
         if (P_NIV2[pc][numcrit-1]) strcpy(tmp,tmp2);
         if (P_NIV3[pc][numcrit-1]) strcpy(tmp,tmp3);
         */

         if (P_NIV0[pc][c2-71]) strcpy(tmp,tmp0);
         if (P_NIV1[pc][c2-71]) strcpy(tmp,tmp1);
         if (P_NIV2[pc][c2-71]) strcpy(tmp,tmp2);
         if (P_NIV3[pc][c2-71]) strcpy(tmp,tmp3);

         asTri->Cells[c2][i] = AnsiString(tmp);
        }
      else asTri->Cells[c2][i] = asTri->Cells[c1][i];
      }
   }
}


int __fastcall TplStandard::Attrib_Tri()
{
 char tmp[1500]; int i;
 AnsiString Debug;
 // fabriquer une chaine Champs et exec_tri d'apres GLOBAL TRI //
 strcpy(tmp,"<champs>");
 for (i=0;i<8;i++)
 {
   try
   {
   Debug = P_FIELD[pc][i];
   }
   catch (EAccessViolation &AccessViolation)
   {
    P_FIELD[pc][i]="";
   }
   //if (strlen(P_FIELD[pc][i].c_str())==0) break;
   //else
   //if (strlen(P_FIELD[pc][i].c_str())) //!="")

    { strcat(tmp,P_FIELD[pc][i].c_str()); strcat(tmp,"|"); }
 }
 strcat(tmp,"</champs>");
 Exec_Trier(0,tmp);
 return 0;
}

int __fastcall TplStandard::Exec_Trier(int mod,char *s)
{
 char tmp[1500];  int i,j,count,crit;
 int indextri,col;
 // cleaner colonnes de tri
 count = asTri->RowCount;
 int org = 70;
 for (i=1;i<count;i++)
   {
    for (j=1;j<=4;j++) asTri->Cells[j+org][i] = "";
   }



 Ghost->ExtractValue(tmp,s,"champs",0);
 count = Explode('|',tmp);     // result dans params
 if (count==0) return 0;

 indextri=71;
 for (i=0;i<count;i++)
   {
    if (params[i][1]=='*')   // c'est un critere
      {
       crit = RechercheColCrit(params[i]+2);      //
       if (crit !=0)
         {
           strcpy(CONTROL,"A");
           Copy_Col(crit+40,indextri,CONTROL,crit);
           indextri++;         // verifier si < 80;
         }
      }
    else // c'est un champ normal;
      {
       col = RechercheCol(params[i]+1);
       // CONTROL sait si c'est A,N ou D
       if (col !=0)
         {
          Copy_Col(col,indextri,CONTROL,0);
          indextri++;
         }
      }

   }

asTri->QSort();

// Ne pas faire RAZ()
Remplir_AV1();

if (mod !=0)
  {
   PAGE_RAPPORT[pc]=1;
   switch (pc)
      {
       case 0: Prepare_Draw_Planning(); ZoomCopy0(imP0p,0,0); break;
       case 1: Prepare_Draw_Planning(); ZoomCopy1(imP1p,0,0); break;
       case 2: Prepare_Draw_Planning(); ZoomCopy2(imP2p,0,0); break;
       case 3: Prepare_Draw_Planning(); ZoomCopy3(imP3p,0,0); break;
       //case 3: Prepare_Draw_Planning(); ZoomCopyAv(imPort,0,0); break;
       case 4: Prepare_Draw_Rapport(1); ZoomCopy4(imR1p,0,0); break;
       //case 5: Prepare_Draw_Rapport(1); ZoomCopy5(imR2p,0,0); break;
      }
  }    

 strcpy(m_status,"<minim>"); strcat(m_status,"Trier Grouper T�ches");
 strcat(m_status,"</minim>");
 return 0;
}

int __fastcall TplStandard::Exec_Criteres(char *s)
{
 int i,col;
 char c1[250],c2[250],c3[250],c4[250],c5[250],c6[250],c7[250],c8[250],lib[250],tmp[250];
 int avCol;
 AnsiString S1,S2,S3;

//  Load_Grille_Criteres();
 // affecter un criteres aux taches cochees
 Ghost->ExtractValue(c1,m_criteres,"c1",0);
 Ghost->ExtractValue(c2,m_criteres,"c2",0);
 Ghost->ExtractValue(c3,m_criteres,"c3",0);
 Ghost->ExtractValue(c4,m_criteres,"c4",0);
 Ghost->ExtractValue(c5,m_criteres,"c5",0);
 Ghost->ExtractValue(c6,m_criteres,"c6",0);
 Ghost->ExtractValue(c7,m_criteres,"c7",0);
 Ghost->ExtractValue(c8,m_criteres,"c8",0);
 Ghost->ExtractValue(lib,m_criteres,"lib",0);

  //recherche de la colonne correcte
 col=3;
 for (i=3;i<asCrit->ColCount;i++)
   {
    if ((asCrit->Cells[i][0]+"-") == AnsiString(c1))
      col = i;
   }
 strcpy(tmp,"");
 strcat(tmp,c1); strcat(tmp,c2); strcat(tmp,c3); strcat(tmp,c4);


 for (i=1;i<=LASTTASK;i++)     //nb_taches;i++)
   {
    if (coch[i]=='X')       ////
     {
      //asCrit->Cells[col][i]=AnsiString(tmp);
      avCol = col/2;
      av->Cells[avCol+40][i]= AnsiString(tmp);
      //asCrit->Cells[col+1][i]=lib;
      av->Cells[avCol+50][i]=lib;

     }
   }

 //  SaveCriteres(true);

 Regen_Criteres();
 CopyCriteres();
 // SaveCriteres(true);
 // Tout D�cocher
 Tout_Decocher_Crit();

 return 0;
}


int __fastcall TplStandard::Exec_Coches(char *s)
{

 if (strcmp(m_coche,"<21>")==0) Tout_Cocher_Crit();
 if (strcmp(m_coche,"<22>")==0) Tout_Decocher_Crit();
 if (strcmp(m_coche,"<23>")==0) Cocher_Decocher_Crit();

 if (strcmp(m_coche,"<01>")==0) Tout_Cocher();
 if (strcmp(m_coche,"<02>")==0) Tout_Decocher();
 if (strcmp(m_coche,"<03>")==0) Coch_Decoch();

 if (strcmp(m_coche,"<C>")==0) Copier_Cocher();
 if (strcmp(m_coche,"<S>")==0) Supp_Coch();
 if (strcmp(m_coche,"<L>")==0) Lier_Coch();

 if (strcmp(m_coche,"<D>")==0) Delier_Coch();
 if (strcmp(m_coche,"<K>")==0) Affect_Cal();
 if (strcmp(m_coche,"<A>")==0) Affect_Amont();

 if (strcmp(m_coche,"<X>")==0) Affect_Pred();
 if (strcmp(m_coche,"<Y>")==0) Affect_Succ();
 if (strcmp(m_coche,"<T>")==0) Affect_Des();

 if (strcmp(m_coche,"<V>")==0) Affect_Data();

 return 0;
}

void __fastcall TplStandard::Exec_Message(char *mess)
{
 char dst[50],action[50];
 Ghost->ExtractValue(dst,mess,"dst",0);
 Ghost->ExtractValue(action,mess,"act",0);

 if (strcmp(dst,"plstandard")==0)
   {
    //if (!ActiveControl) ActiveControl=true;
    //Application->PrepPlanning->Hide();

     SetFocus();
 // raz si c'etait le destinataire
    m_message[0]=0;
   }
}

void __fastcall TplStandard::Exec_Planning(char *plan,bool reload)
{
 char tmp[250];
 char tri[200];
 char col[200];
 char fil[200];
 int i;
 AnsiString PTFC;

 if (nb_taches==0) return;

 //pc=atoi(tmp);
 if (pc<0 || pc>6) pc=1;

   Ghost->ExtractValue(tmp,plan,"pl",0);
   Ghost->ExtractValue(tri,plan,"tri",0);
   Ghost->ExtractValue(col,plan,"col",0);
   Ghost->ExtractValue(fil,plan,"filt",0);

 //  strcpy(P_NOM[pc],LastPlanning[pc]);


   //if (reload)
   //{
   if (RELOADTRI[pc] && strlen(tri)) { Ghost->Global_Tri(tri); RELOADTRI[pc]=false; }


   if (RELOADFILTRE[pc])
     {
       if (strlen(fil))
        { Ghost->Global_Filtre(fil);  // fabrique une chaine de caracteres
          Exec_Filter(0,m_exchange);

        }
       else { for (i=0; i<nb_taches; i++) filtr[i]='X'; }
      RELOADFILTRE[pc]=false;
     }

   if (RELOADCOL[pc]) {   if (strlen(col))  Ghost->Global_Col(col); RELOADCOL[pc]=0; }
  //}

 PTFC = AnsiString(P_NOM[pc]) + " / " + AnsiString(tri) + " / " + AnsiString(fil) + " / " + AnsiString(col);
 if (pc==0) Label142->Caption = AnsiString(P_NOM[0]);
 //if (pc==1) LP1TFC->Caption = PTFC;
 //if (pc==2) LP2FTC->Caption = PTFC;
 //if (pc==3) Label96->Caption = PTFC;

 RAZ();

 PAGE_RAPPORT[pc]=1;
     switch (pc)
      {
       case 0: Prepare_Draw_Planning(); ZoomCopy0(imP0p,0,0); break;
       case 1: Prepare_Draw_Planning(); ZoomCopy1(imP1p,0,0); break;
       case 2: Prepare_Draw_Planning(); ZoomCopy2(imP2p,0,0); break;
       case 3: Prepare_Draw_Planning(); ZoomCopy3(imP3p,0,0); break;
       //case 3: Prepare_Draw_Planning(); ZoomCopyAv(imPort,0,0); break;
       case 4: Prepare_Draw_Rapport(1); ZoomCopy4(imR1p,0,0); break;
       // case 5: Prepare_Draw_Rapport(1); ZoomCopy5(imR2p,0,0); break;
      }
}


void __fastcall TplStandard::Timer1Timer(TObject *Sender)
{
 char tmp[255];
 int lg; int res,w,h;

 Timer1->Enabled = false;
 /*
 timercnt++;
 if (timercnt>=20)
    { lg = (rand() % 17)+2;
      if (CLEF) { res = check_key(lg); if (res) { Close();}
     }
     timercnt=0;
    }
 */

 if (change)
   {

    TimeCounter++;
    if (TimeCounter >=5000)
      { Timer1->Enabled=false;
       if (change==1)
       {
         if (Application->MessageBoxA("Voulez Vous Sauvegarder les Modifications",m_ecoplan,MB_YESNO)==IDYES)
           {
            updatedatabase(LocalBase,0,0);  // $$$$$ 0
            change=0;
           }
         Timer1->Enabled=true;
       }
       Exec_Export_Excel(false);

       TimeCounter=0;
      }
   }

 if (flag_real) { Create_First_Real(); flag_real=false; }
 if (flag_init) { InitTri(); flag_init=false;}
 if (flag_alert) { if (flip_flop) { lAlert->Font->Color = clBlack; lAlert->Caption = ""; flip_flop = false;}
 else {lAlert->Font->Color = clRed;lAlert->Caption = "Retard Imputable";flip_flop = true; }}

 if (flag_alertPred){if (flip_flopPred){ lAlertPred->Font->Color = clBlack;lAlertPred->Caption = "";flip_flopPred = false;}
 else {lAlertPred->Font->Color = clRed;sprintf(tmp,"Tache %d : Retard Imputable",av_tache_pred);lAlertPred->Caption = AnsiString(tmp); flip_flopPred = true; }}

 lg = strlen(m_filtre);
 if (lg > 0) { modfilter=1;
     Exec_Filter(modfilter,m_filtre);m_filtre[0]=0;

   }
 lg = strlen(m_coche);
  if (lg > 0){ Exec_Coches(m_coche); m_coche[0]=0; }
 //lg = strlen(m_criteres);
 //if (lg>0) { Exec_Criteres(m_criteres); m_criteres[0]=0; Change1(); }
 lg = strlen(m_sort);
 if (lg>0) {
    Exec_Trier(1,m_sort);
    m_sort[0]=0; Change1(); }
 lg = strlen(m_planning);
 if (lg>0)
   {
    Ghost->ExtractValue(tmp,m_planning,"exec",0);
    if (strcmp(tmp,"N")==0)
   Exec_Planning(m_planning,false);
   else Exec_Planning(m_planning,true);
   m_planning[0]=0;
   }
  lg=strlen(m_affect_crit);
  if (lg)
   {
    Exec_Affect_Crit();
    m_affect_crit[0]=0;
   }

  lg=strlen(m_rapport);
  if (lg>0)
   {Exec_Rapport(m_rapport,true); m_rapport[0]=0; }

 lg = strlen(m_message);
 if (lg>0)
  { Exec_Message(m_message); }   // ne pas faire RAZ du message
 lg=strlen(m_saveAV);
 if (lg>0) {SaveAv(); m_saveAV[0]=0; }


/* lg=strlen(m_suppcoch);
 if (lg>0)
   {
    supp_coch();
    m_suppcoch[0]=0;

   }
*/

 lg=strlen(m_dateprev);
 if (lg>0)
  { ModifDatePrev(m_dateprev); m_dateprev[0]=0; }
 lg = strlen(m_simul);
 if (lg>0)
   {
     Save();
     /*
     if (Application->MessageBox("Etes-vous s�r de vouloir effecteur la simulation?",m_ecoplan,MB_YESNO)==IDNO)
        {
         m_simul[0]=0;
         return;
        }
      */
     //strcpy(SAVEAS,ProjectName);  strcat(SAVEAS,"_Recal");
     //SaveAs(true);
     Exec_Simul();
     strcpy(SAVEAS,ProjectName);  strcat(SAVEAS,"_Recal");
     SaveAs(true);
     m_simul[0]=0;
    }

 lg = strlen(m_zero);
 if (lg>0)
    {
     /*Save();
     Exec_RAZ();
     strcpy(SAVEAS,ProjectName); strcat(SAVEAS,"_ACT");
     SaveAs(false);
     */
     btRAZClick(NULL);

     m_zero[0]=0;
    }
 lg = strlen(m_excel);
 if (lg>0) {
             if (strcmp(m_excel,"import")==0) Exec_Import_Excel();
             else if (strcmp(m_excel,"import_CSV")==0) Exec_Import_CSV();
             else if (strcmp(m_excel,"export")==0) Exec_Export_Excel(true);
             else if (strcmp(m_excel,"expCSV")==0) Exec_Excel_CSV();
             else if (strcmp(m_excel,"expCRIT")==0) Exec_Excel_CRIT();
             else if (strcmp(m_excel,"expCOUT")==0) Exec_Excel_COUT();

             m_excel[0]=0;
           }
 lg = strlen(m_save);
 if (lg>0)
   {
    if (strcmp(m_save,"saveas")==0) { strcpy(SAVEAS,"");  SaveAs(false);  }
    if (strcmp(m_save,"save")==0) Save();

    m_save[0]=0;
   }
 lg = strlen(m_chcrit); // changement code criteres
 if (lg>0)
   {
    Modify_Criteres(m_chcrit);
    m_chcrit[0]=0;
    // Re-actualiser
   }
 lg = strlen(m_resize);
 if (lg>0)
   { Ghost->ExtractValue(tmp,m_resize,"w",0);
     w = atoi(tmp);
     Ghost->ExtractValue(tmp,m_resize,"h",0);
     h = atoi(tmp);
     EcoResize(w,h); m_resize[0]=0;
   }

 lg = strlen(m_modcrit);
 if (lg > 0)
   {
    Remplace_Crit(m_modcrit);
    m_modcrit[0]=0;

   }
 lg = strlen(m_renum);
 if (lg>0)
   {
    if (strcmp(m_renum,"YES")==0) Renum();
    m_renum[0]=0;
   }

 Timer1->Enabled = true;
}
//---------------------------------------------------------------------------



void __fastcall TplStandard::Exec_RAZ()
{
 char tmp[1000]; char msg[500]; int i,cnt;
 int task;  int dur; AnsiString S;  int rowAS1;
 int isucc,ipred; int nt;  char dt1[20],dt2[20];
 int dist1,dist2; int N; int numt,numavanc,row;
 char snumt[30]; char DateDEBR[20];

 strcpy(tmp,FileNameAvanc);
 if (strlen(tmp)==0)
   {
    Application->MessageBox("Pas de fichier avancement charg�",m_ecoplan,MB_OK);
    return;
   }
 strcpy(msg,"Votre fichier "); strcat(msg,LocalBase);
 strcat(msg," va �tre nomm�: "); strcat(msg,FileNameAvanc); strcat(msg,"_ACT.ECO\n");
 strcat(msg," et va devenir le projet en cours.\n");
 strcat(msg," Les donn�es pr�visionnelles seront remplac�es par les donn�es r�elles\n");
 strcat(msg,"\nVoulez-vous continuer ?");
 if (Application->MessageBox(msg,m_ecoplan,MB_YESNO)==IDNO) return;


 numt = LookFirst();
 if (numt==0) return;

 // Date de d�but r�el de numt dans DateDEB


 /*
 cnt = asAvanc->RowCount;
 row = 0;
 sprintf(DateDEBR,"%02/%02d/%02d",JourPrev,MoisPrev,AnPrev%100);
 for (i=1;i<cnt;i++)
  {
   strcpy(snumt,asAvanc->Cells[1][i].c_str());
   numavanc = atoi(snumt);
   if (numavanc == numt)
     { row = i;
       strcpy(DateDEBR,asAvanc->Cells[3][row].c_str());
     }
 }

 ModifDatePrev(DateDEBR);
*/

 // Copy Reel --> pr�visionnel
 cnt = av->RowCount;
 for (i=1;i<cnt;i++)
  {

   av->Cells[3][i] = av->Cells[23][i];  // Duree prev
   strcpy(tmp,av->Cells[3][i].c_str());
   dur = atoi(tmp);
   S = av->Cells[21][i];  // Deb Plus Tot  reel
   av->Cells[4][i] = av->Cells[21][i];  // Deb Plus Tot  prev
   S = av->Cells[22][i];  // Fin Plus Tot  reel
   av->Cells[5][i] = av->Cells[22][i];  // Fin Plus Tot

   rowAS1 = ChercheRow(AnsiString(i));
   AdvStringGrid1->Cells[3][rowAS1]=av->Cells[3][i];
   // AdvStringGrid1->Cells[4][rowAS1]=av->Cells[4][i];   pas utile
   // AdvStringGrid1->Cells[5][rowAS1]=av->Cells[5][i];   pas utile

   // Boucle Predecesseur : changer type de lien et decalage


   ipred=1;
   while ((pr[i][ipred]!=0) && (ipred<PS))       //
      {
       nt = pr[i][ipred];     // pr_tl et pr_decal
       // calculer dist fin du predecesseur au d�but de la tache en cours
       strcpy(dt1,av->Cells[22][nt].c_str());
       strcpy(dt2,av->Cells[21][i].c_str());

       comp_dist_dates(dt1,dt2,1,0);
       pr_tl[i][ipred] = 0; // FD
       pr_decal[i][ipred]=NB1-1; // NB2-1;
       N=NB1;
       N=NB2;
       N=NB3;
       ipred++;
      }


   isucc=1;
   // Boucle Successeur  : changer type de lien et decalage
    while ((su[i][isucc] !=0) && isucc < PS)
       {
        nt=su[i][isucc];  // su_tl et su_decal
        if (nt != 9999)
         {
          strcpy(dt1,av->Cells[22][i].c_str());
          strcpy(dt2,av->Cells[21][nt].c_str());
          comp_dist_dates(dt1,dt2,1,0);

          su_tl[i][isucc]=0;
          su_decal[i][isucc]=NB1-1;

         }
        isucc++;
       }


  }

 /*
 cnt=asAvanc->RowCount;
 for (i=1;i<cnt;i++)
   {
    row_avancement = i;
    strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
    task = atoi(tmp);
    asAvanc->Cells[3][row_avancement] = av->Cells[21][task];  // date debut reel
    asAvanc->Cells[4][row_avancement] = av->Cells[23][task];  // duree reelle
    asAvanc->Cells[5][row_avancement] = av->Cells[22][task];  // Date fin reelle


    AdvStringGrid6->Cells[3][row_avancement] = av->Cells[21][task];  // date debut reel
    AdvStringGrid6->Cells[4][row_avancement] = av->Cells[23][task];  // duree reelle
    AdvStringGrid6->Cells[5][row_avancement] = av->Cells[22][task];  // Date fin reelle	
	
    asAvanc->Cells[8][row_avancement] = av->Cells[21][task];  // date debut prev=reel
    asAvanc->Cells[9][row_avancement] = av->Cells[23][task];  // duree prev=reelle
    asAvanc->Cells[10][row_avancement] = av->Cells[22][task];  // Date fin prev =reelle
	
	

    AdvStringGrid6->Cells[8][row_avancement] = av->Cells[21][task];  // date debut prev=reel
    AdvStringGrid6->Cells[9][row_avancement] = av->Cells[23][task];  // duree prev=reelle
    AdvStringGrid6->Cells[10][row_avancement] = av->Cells[22][task];  // Date fin prev =reelle
    //grefresh(task,'0');
   }
 */

 Compute_PlusTotA();
 Compute_PlusTardA('1',false);

  // updatedatabase(LocalBase,0,0);

}


void __fastcall TplStandard::Exec_Affect_Crit()
{
 int i,ctr;  AnsiString str;  char cr[50];
 char tmp[500];   char oc[50],nc[50],od[200],nd[200];
 int ct,x; int col;  char val[200];  char *p;
 int nt; char numt[20];

 ctr = TSLCRIT->Count;

 Open_Dump();
 for (i=0;i<ctr;i++)
  {
   str = TSLCRIT->Strings[i];
   strcpy(tmp,str.c_str());
   strcat(tmp,"\n");
   fputs(tmp,dump);
  }

 ct = asCrit->RowCount;
 for (i=0;i<ctr;i++)
   {
    str = TSLCRIT->Strings[i];
    strcpy(tmp,str.c_str());
    Ghost->ExtractValue(cr,tmp,"cr",0);
    col=0;
    if (strcmp(cr,"P")==0) col= 3;
    if (strcmp(cr,"I")==0) col= 5;
    if (strcmp(cr,"T")==0) col= 7;
    if (strcmp(cr,"R")==0) col= 9;
    if (col !=0)
    {
     Ghost->ExtractValue(oc,tmp,"oc",0);
     Ghost->ExtractValue(nc,tmp,"nc",0);
     Ghost->ExtractValue(od,tmp,"od",0);
     Ghost->ExtractValue(nd,tmp,"nd",0);

     if (strlen(oc))
       {
        for (x=1;x<ct;x++)
          {
           strcpy(val,asCrit->Cells[col][x].c_str());
           if (strcmp(val,oc)==0)
             { asCrit->Cells[col][x]=AnsiString(nc)+ "@";
               strcpy(numt,asCrit->Cells[1][x].c_str());
               nt=atoi(numt);
               if (nt != 0)
                 {
                  if (col==3) av->Cells[41][nt]=AnsiString(nc);
                  if (col==5) av->Cells[42][nt]=AnsiString(nc);
                  if (col==7) av->Cells[43][nt]=AnsiString(nc);
                  if (col==9) av->Cells[45][nt]=AnsiString(nc);
                 }
              sprintf(tmp,"Ascrit VAL = %s, TASK= %d, COL= %d, NEWCRIT = %s \n",val, nt,col,nc);
              fputs(tmp,dump);
             }
          }
       }
     /*
     if (strlen(od))
       {
        for (x=1;x<ct;x++)
          {
           strcpy(val,asCrit->Cells[col+1][x].c_str());
           if (strcmp(val,od)==0)
            { asCrit->Cells[col+1][x]=AnsiString(nd)+ "@";
              strcpy(numt,asCrit->Cells[1][x].c_str());
               nt=atoi(tmp);
               if (nt != 0)
                 {
                  if (col==3) av->Cells[51][nt]=AnsiString(nd);
                  if (col==5) av->Cells[52][nt]=AnsiString(nd);
                  if (col==7) av->Cells[53][nt]=AnsiString(nd);
                  if (col==9) av->Cells[54][nt]=AnsiString(nd);
                 }


            }
          }
       }
      */ 
    } // col != 0;
   } // for i

// remove all the @

 for (i=1;i<ct;i++)
  {
   for (x=3; x<11; x++)
     {
      strcpy(val,asCrit->Cells[x][i].c_str());
      p = strstr(val,"@");
      if (p)

       { *p=0;asCrit->Cells[x][i]= AnsiString(val);
       }

     }

  }
 SaveCriteres(false);
 Close_Dump();
 

}


void __fastcall TplStandard::Exec_Excel_CSV()
{

  char fname[250];  char name[250];
  char small[15000]; char *p;
  int i,ii,nt; char tmp[15000];
  FILE *fp;

  strcpy(name,LocalBase); strcat(name,"_projet.csv");


  Application->MessageBoxA("Vous allez indiquer o� sauvegarder le fichier Texte (CSV)",m_ecoplan,MB_OK);
  SaveDialog1->FileName = AnsiString(name);
  if (SaveDialog1->Execute()) strcpy(name,SaveDialog1->FileName.c_str());
   else return;


  fp=fopen(name,"wb");
  if (fp==NULL)
    {
     Application->MessageBoxA("Fichier d�j� utilis� par une autre application",m_ecoplan,MB_OK);
     return;
    }

   if (Application->MessageBox("Sauvegarde des t�ches pr�sentes dans le volet Saisie Pr�visonnelle.\nVoulez-Vous Continuer?",m_ecoplan,MB_YESNO)==IDNO) return;


  // Libell�s
  strcpy(tmp,"");
  strcat(tmp,"\""); strcat(tmp,"N� "); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�signation"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Dur�e"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�but Souhait�"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Fin Souhait�e"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Pr�d�cesseurs"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Successeurs"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Crit1"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit1"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Crit2"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit2"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Crit3"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit3"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Crit4"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit4"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Code t�che"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Co�t +"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Co�t -"); strcat(tmp,"\";");

  strcat(tmp,"\n");
  fputs(tmp,fp);

  for (i=1; i<=AdvStringGrid1->RowCount; i++)
  {
    strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
    ii = atoi(tmp);
    if (av->Cells[1][ii] != "")
          {
            strcpy(tmp,"");
            strcat(tmp,"\""); strcat(tmp,av->Cells[1][ii].c_str()); strcat(tmp,"\";");  // N�
            strcat(tmp,"\""); strcat(tmp,av->Cells[2][ii].c_str()); strcat(tmp,"\";");  // Des
            strcat(tmp,"\""); strcat(tmp,av->Cells[3][ii].c_str()); strcat(tmp,"\";");  // Dur�e
            strcat(tmp,"\""); strcat(tmp,av->Cells[12][ii].c_str()); strcat(tmp,"\";");  // D�but  Souhait�
            strcat(tmp,"\""); strcat(tmp,av->Cells[13][ii].c_str()); strcat(tmp,"\";");  // Fin Souhait�e

            strcpy(small,av->Cells[1][ii].c_str());
            nt = atoi(small);
            if (nt != 0)
              {
               // bcl Pred
               strcat(tmp,"\"");
               BCL_PRED(nt,small);
               strcat(tmp,small);
               strcat(tmp,"\";");
               strcat(tmp,"\"");
               BCL_SUCC(nt,small);
               strcat(tmp,small);
               strcat(tmp,"\";");
              }
            // Crit�re 1
            strcat(tmp,"\""); strcat(tmp,av->Cells[41][ii].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[51][ii].c_str()); strcat(tmp,"\";");
            // Crit�re 2
            strcat(tmp,"\""); strcat(tmp,av->Cells[42][ii].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[52][ii].c_str()); strcat(tmp,"\";");
            // Crit�re 3
            strcat(tmp,"\""); strcat(tmp,av->Cells[43][ii].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[53][ii].c_str()); strcat(tmp,"\";");
            // Crit�re 4
            strcat(tmp,"\""); strcat(tmp,av->Cells[44][ii].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[54][ii].c_str()); strcat(tmp,"\";");
            // Code tache
            strcat(tmp,"\""); strcat(tmp,av->Cells[11][ii].c_str()); strcat(tmp,"\";");
            // Cout +
            strcat(tmp,"\""); strcat(tmp,av->Cells[18][ii].c_str()); strcat(tmp,"\";");
            // Cout -
            strcat(tmp,"\""); strcat(tmp,av->Cells[19][ii].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\n");
            fputs(tmp,fp);
          }
       }   // For
 fclose(fp);
 Application->MessageBoxA("Vous pouvez ouvrir ce fichier _Projet.csv avec EXCEL et l'imprimer\n \
         Le s�parateur de champs est le ';' et le s�parateur de texte est le \" \n \
         Les champs pr�d�cesseurs et sucesseurs ont la virgule (,) pour s�parateurs\n",m_ecoplan,MB_OK);

//  Exec_Excel_CRIT();   // NE CONVIENT PAS
Exec_SAVE_CRIT(name);

}

void __fastcall TplStandard::Exec_SAVE_CRIT(char *fname)
{
 FILE *fp,*fcrit;  char mess[1000]; char buffer[512];
 char tmp[512];  int ctr;  char dname[1000]; char *p;
 char bufout[1000];

 strcpy(tmp,DatabaseName);
 strcat(tmp,"_crit.txt");
 fp=fopen(tmp,"rt");

 p = strstr(fname,"_projet");
 if (p==NULL) { strcpy(dname,DatabaseName); strcat(dname,"_criteres.csv"); }
 else { *p=0; strcpy(dname,fname); strcat(dname,"_criteres.csv"); }

 fcrit = fopen(dname,"wb");
 if (fcrit==NULL)
   {
    strcpy(mess,"Impossible d'ouvrir le fichier CSV: "); strcat(mess,dname);
    Application->MessageBox(mess,m_ecoplan,MB_OK);
    return;
   }




 if (fp == NULL)
   {
    strcpy(mess,"Fichier crit�re "); strcat(mess,tmp); strcat(mess," non trouv�: export CSV non effectu� pour les crit�res");
    Application->MessageBox(mess,m_ecoplan,MB_OK);
    return;
   }
 else  // lire et remplir le tableau
   {

     buffer[0]=0; ctr=0;
    fgets(buffer,512,fp);
    while (!feof(fp))   // lire les 8 premiers records;
      {
       ctr++;
       strcpy(bufout,"");
       Ghost->ExtractValue(tmp,buffer,"l",0);
       if (strcmp(tmp,"C1")==0) strcpy(tmp,"P");
       if (strcmp(tmp,"C4")==0) strcpy(tmp,"R");

       // asCrit->Cells[1][ctr]=tmp;
       strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");
       Ghost->ExtractValue(tmp,buffer,"d",0);
       if (strcmp(tmp,"D�signation 1")==0) strcpy(tmp,"PHASES");
       if (strcmp(tmp,"Crit�re 4")==0) strcpy(tmp,"RESSOURCES");
       if (strcmp(tmp,"D�signation 4")==0) strcpy(tmp,"RESSOURCES");
       strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");
       ///asCrit->Cells[2][ctr]=tmp;
       //Ghost->ExtractValue(tmp,buffer,"t",0);
       strcpy(tmp,""); // pour eviter le null
       strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");
       ///asCrit->Cells[3][ctr]=tmp;

       if (ctr==NB_CRIT) { strcat(bufout,"\n"); fputs(bufout,fcrit); break; }
       fgets(buffer,512,fp);
       strcat(bufout,"\n");
       fputs(bufout,fcrit);
      }


     fgets(buffer,512,fp);
     ctr = 0;
     // asDetail->RowCount=2;
     // for (i=1;i<asDetail->ColCount;i++) asDetail->Cells[i][1]="";
     while (!feof(fp))
      {
       ctr++;
       if (strlen(buffer) > 5)
       {
       strcpy(bufout,"");
       Ghost->ExtractValue(tmp,buffer,"c1",0);
       if (strcmp(tmp,"C1")==0) strcpy(tmp,"P");
       if (strcmp(tmp,"C4")==0) strcpy(tmp,"R");
       //if (ctr>1) asDetail->RowCount++;
        //asDetail->Cells[1][ctr]=tmp;
        strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");
        /*
       //if (strcmp(tmp,"C4")==0 || strcmp(tmp,"C5")==0 || strcmp(tmp,"C6")==0
       if (strcmp(tmp,"C5")==0 || strcmp(tmp,"C6")==0
           || strcmp(tmp,"C7")==0 || strcmp(tmp,"C8")==0)
         asDetail->RowHeights[ctr]=0;
       */

       Ghost->ExtractValue(tmp,buffer,"c2",0);
       //asDetail->Cells[2][ctr]=tmp;
       strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");

       Ghost->ExtractValue(tmp,buffer,"c3",0);
       //asDetail->Cells[3][ctr]=tmp;
       strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");

       Ghost->ExtractValue(tmp,buffer,"c4",0);
       //asDetail->Cells[4][ctr]=tmp;
       strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");

       // asDetail->Cells[5][ctr]="";

       Ghost->ExtractValue(tmp,buffer,"d",0);
       if (strcmp(tmp,"D�signation 1")==0) strcpy(tmp,"PHASES");
       if (strcmp(tmp,"Crit�re 4")==0) strcpy(tmp,"RESSOURCES");
       if (strcmp(tmp,"D�signation 4")==0) strcpy(tmp,"RESSOURCES");
       //asDetail->Cells[6][ctr]=tmp;
       strcat(bufout,"\""); strcat(bufout,tmp); strcat(bufout,"\";");

       /*
       Ghost->ExtractValue(tmp,buffer,"r",0);
       // asDetail->Cells[7][ctr]=tmp;
       Ghost->ExtractValue(tmp,buffer,"t",0);
       asDetail->Cells[8][ctr]=tmp;

       // Special Valeur d'origine

       S="";

       S = S + asDetail->Cells[1][ctr] + "-" + asDetail->Cells[2][ctr] + "-" ;
       S = S + asDetail->Cells[3][ctr] + "-" + asDetail->Cells[4][ctr] + "-";
       asDetail->Cells[10][ctr] =  S;
       asDetail->Cells[11][ctr] =  asDetail->Cells[6][ctr]; // designation
       */

       strcat(bufout,"\n");
       fputs(bufout,fcrit);
       }


       fgets(buffer,512,fp);
      }

    fclose (fp);
    fclose(fcrit);
   }


}





void __fastcall TplStandard::Exec_Excel_CRIT()
{

  char fname[250];  char name[250];
  char small[5000]; char *p;
  int i,nt; char tmp[5000]; char xtmp[5000];
  FILE *fp;

  strcpy(fname,LocalBase); strcat(fname,"_crit.csv");


  Application->MessageBoxA("Vous allez indiquer o� sauvegarder le fichier Texte (CSV)",m_ecoplan,MB_OK);
  SaveDialog1->FileName = AnsiString(fname);
  if (SaveDialog1->Execute()) strcpy(name,SaveDialog1->FileName.c_str());
   else return;


  //strcpy(name,m_directory); strcat(name,"\\TACHE.TXT");
  fp=fopen(name,"wt");
  if (fp==NULL)
    {
     Application->MessageBoxA("Fichier d�j� utilis� par une autre application",m_ecoplan,MB_OK);
     return;
    }


  // Libell�s
  strcpy(tmp,"");

  strcat(tmp,"\""); strcat(tmp,"N� "); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�signation"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Dur�e"); strcat(tmp,"\";");

  strcat(tmp,"\""); strcat(tmp,"Crit 1"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit 1"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Crit 2"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit 2"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Crit 3"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit 3"); strcat(tmp,"\";");

  strcat(tmp,"\""); strcat(tmp,"Crit 4"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�sign Crit 4"); strcat(tmp,"\";");

  strcat(tmp,"\n");
  fputs(tmp,fp);

  for (i=1; i<=av->RowCount; i++)
  //for (i=1;i<=nb_taches;i++)
        {
         if (av->Cells[1][i] != "")
          {
            strcpy(tmp,"");
            strcat(tmp,"\""); strcat(tmp,av->Cells[1][i].c_str()); strcat(tmp,"\";");  // N�
            strcat(tmp,"\""); strcat(tmp,av->Cells[2][i].c_str()); strcat(tmp,"\";");  // Des
            strcat(tmp,"\""); strcat(tmp,av->Cells[3][i].c_str()); strcat(tmp,"\";");  // Dur�e
            // criteres
            strcat(tmp,"\""); strcat(tmp,av->Cells[41][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[51][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[42][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[52][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[43][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[53][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[44][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[54][i].c_str()); strcat(tmp,"\";");
            /*
            strcat(tmp,"\"");
            strcpy(xtmp,av->Cells[43][i].c_str());
            if (strstr(xtmp,"</c2><d2>"))
               av->Cells[43][i]="";
            strcat(tmp,av->Cells[43][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[53][i].c_str()); strcat(tmp,"\";");
            */
            strcat(tmp,"\n");
            fputs(tmp,fp);
          }
       }   // For
  fclose(fp);
 Application->MessageBoxA("Vous pouvez ouvrir ce fichier _crit.csv avec EXCEL et l'imprimer\nLe s�parateur de champ est le ';' et le s�parateur de texte est le '\"'",m_ecoplan,MB_OK);

}


void __fastcall TplStandard::Exec_Excel_COUT()
{

  char fname[250];  char name[250];
  char small[5000]; char *p;
  int i,nt; char tmp[5000];
  FILE *fp;

  strcpy(fname,LocalBase); strcat(fname,"_couts.csv");


  Application->MessageBoxA("Vous allez indiquer o� sauvegarder le fichier Texte (CSV)",m_ecoplan,MB_OK);
  SaveDialog1->FileName = AnsiString(fname);
  if (SaveDialog1->Execute()) strcpy(name,SaveDialog1->FileName.c_str());
   else return;

  fp=fopen(name,"wt");
  if (fp==NULL)
    {
     Application->MessageBoxA("Fichier d�j� utilis� par une autre application",m_ecoplan,MB_OK);
     return;
    }

  // Libell�s
  strcpy(tmp,"");
  strcat(tmp,"\""); strcat(tmp,"N� "); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"D�signation"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Dur�e"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Co�t Pr�v. >0 "); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Co�t Pr�v. <0"); strcat(tmp,"\";");
  strcat(tmp,"\""); strcat(tmp,"Co�t R�el"); strcat(tmp,"\";");


  strcat(tmp,"\n");
  fputs(tmp,fp);

  for (i=1; i<=av->RowCount; i++)
  //for (i=1;i<=nb_taches;i++)
        {
         if (av->Cells[1][i] != "")
          {
            strcpy(tmp,"");
            strcat(tmp,"\""); strcat(tmp,av->Cells[1][i].c_str()); strcat(tmp,"\";");  // N�
            strcat(tmp,"\""); strcat(tmp,av->Cells[2][i].c_str()); strcat(tmp,"\";");  // Des
            strcat(tmp,"\""); strcat(tmp,av->Cells[3][i].c_str()); strcat(tmp,"\";");  // Dur�e




           // Couts
            strcat(tmp,"\""); strcat(tmp,av->Cells[18][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[19][i].c_str()); strcat(tmp,"\";");
            strcat(tmp,"\""); strcat(tmp,av->Cells[26][i].c_str()); strcat(tmp,"\";");

            strcat(tmp,"\n");
            fputs(tmp,fp);
          }
       }   // For
  fclose(fp);
 Application->MessageBoxA("Vous pouvez ouvrir ce fichier _couts.csv avec EXCEL et l'imprimer\nLe s�parateur de champ est le ';' et le s�parateur de texte est le '\"'",m_ecoplan,MB_OK);

}



void __fastcall TplStandard::BCL_PRED(int nt, char *result)
{
  char xx[10]; char tmp[5000];
  int pred,ipred,dec,tl;    int i;


  ipred = 1;
  result[0]=0;
  while (pr[nt][ipred]!=0 && ipred < 50)
          {
            pred=pr[nt][ipred];
            dec= pr_decal[nt][ipred];
            tl=pr_tl[nt][ipred];
            if (tl==0) strcpy(xx,"FD");
            else if (tl==1) strcpy(xx,"FF");
            else if (tl==2) strcpy(xx,"AM");
            else if (tl==3) strcpy(xx,"DD");
            sprintf(tmp,"%d%s%d,",pred,xx,dec);
            if (ipred<50)
             {
               if (strlen(result)< 4800)
                 {
                  //strcat(result,"\""); strcat(result,tmp); strcat(result,"\";");
                  strcat(result,tmp);
                 }

             }

            ipred++;
           }

}


void __fastcall TplStandard::BCL_SUCC(int nt, char *result)
{
  char xx[10]; char tmp[5000];
  int succ,isucc,dec,tl;   int i;

  isucc = 1;
  result[0]=0;
  while ((su[nt][isucc]!=0) && (su[nt][isucc]!=9999) && isucc <50)

          {
            succ=su[nt][isucc];
            dec= su_decal[nt][isucc];
            tl=su_tl[nt][isucc];
            if (tl==0) strcpy(xx,"FD");
            else if (tl==1) strcpy(xx,"FF");
            else if (tl==2) strcpy(xx,"AM");
            else if (tl==3) strcpy(xx,"DD");
            sprintf(tmp,"%d%s%d,",succ,xx,dec);
            if (isucc<50)
             {
               if (strlen(result) < 4800)
                 {
                  //strcat(result,"\""); strcat(result,tmp); strcat(result,"\";");
                  strcat(result,tmp);
                  }

             }
            isucc++;

           }
        
}


void __fastcall TplStandard::Remplace_Crit(char *mc)
{
 char oldc[200];
 char newc[200];
 char acol[20];
 char acrit[30];
 int col;
 int  crit,nocol,colavs, r,c,cnt,nt;
 char tmp[500];


 Ghost->ExtractValue(acrit,mc,"crit",0);
 crit = atoi(acrit);  if (crit==0)
   crit=1;  // crit de 1 a 8;
 Ghost->ExtractValue(acol,mc,"col",0);   //  CR ou DES
 Ghost->ExtractValue(oldc,mc,"old",0);
 Ghost->ExtractValue(newc,mc,"new",0);
 ///strcat(oldc,"-");
 ///strcat(newc,"-");

 cnt = asCrit->RowCount;
 if (strcmp(acol,"CR")==0)
    { nocol = crit*2 + 1;colavs = 40+crit; }
    else { nocol = crit*2+2; colavs = 50+crit; }
 for (r=1;r<cnt;r++)
  {
   strcpy(tmp,asCrit->Cells[1][r].c_str());
   nt = atoi(tmp);
   if (nt==0) nt=1;

   if (asCrit->Cells[nocol][r]==AnsiString(oldc))

     { asCrit->Cells[nocol][r]= AnsiString(newc);
       av->Cells[colavs][nt] = AnsiString(newc);

     }
  }
 SaveCriteres(true);
 Remplir_asTri();

}

void __fastcall TplStandard::Modify_Criteres(char *str)
{
 int r,c,cnt,lg,nt,nocol;
 char tmp[200];
 char oldc[100];
 char newc[100];
 char *p1,*p2;

 Ghost->ExtractValue(oldc,str,"old",0);
 Ghost->ExtractValue(newc,str,"new",0);
 strcat(oldc,"-");
 strcat(newc,"-");

 cnt = asCrit->RowCount;
 for (r=1;r<cnt;r++)
  {
   strcpy(tmp,asCrit->Cells[1][r].c_str());
   nt = atoi(tmp);
   if (nt==0) nt=1;
   for (c=0; c<8; c++)
     {
      nocol = c*2 + 3;
      strcpy(tmp,asCrit->Cells[nocol][r].c_str());
      p1 = strstr(tmp,oldc);
      if (p1)
        {
         p2=strstr(p1+1,"-");
         if (p2)
           {
            lg = strlen(p2)-1;
            memmove(p1,p2+1,lg+1); // +1 pour garder le -
            insert_simple(newc,tmp,tmp-p1);
            asCrit->Cells[nocol][r] = AnsiString(tmp);
            av->Cells[c+41][nt] = asCrit->Cells[nocol][r];
           }
        }
     }
  }
 SaveCriteres(true);
 Remplir_asTri();
}

void __fastcall TplStandard::insert_simple(char *nstr,char *dst,int pos)
{
 int lt,lc;
 lt=strlen(nstr); if (lt==0) return;
 lc=strlen(dst); if (lc==0) return;
 if (lc<=pos) { strcat(dst,nstr); return; }
 // Move memory
 memmove(dst+pos+lt,dst+pos,lc-pos+1);  // PLus one to keep \0
 memmove(dst+pos,nstr,lt);
}

void __fastcall TplStandard::Exec_Import_Excel()
{

 // a ce point : la base est cr�e, vide
 // Lire les fichiers LIAS.TXT et TACHE.TXT  et Criteres

 FILE *fp;   FILE *fcrit;  char needle[255]; char *pcx;
 char fname[MAX_PATH];
 char filename[MAX_PATH];
 char buf[512]; char *p,*p1,*p2,*p3;
 char champ1[100]; char champ2[200];
 char XXX[500];
 char field[30][250];  // maximum 30 champs
 int i,j,cnt,xrow,nt;
 float mt; char tmp[250];
 int succ,idecal,typl;
 int av1count,tmax;
 char CR1[10000];   // stockage des criteres
 char CR2[10000];
 char CR3[10000];
 char CR4[10000];
 char mess[200];


 strcpy(filename,m_importtaches); // strcat(filename,"\\TACHE.TXT");
 fp = fopen(filename,"rb");
 if (fp==NULL)
   {
    Application->MessageBoxA("Fichier Transfert TACHES.TXT non trouv�",m_ecoplan,MB_OK);
    return;
   }
 strcpy(buf,"");
 fgets(buf,512,fp);

 ScanBuf(buf);

 Clean_Grid(AdvStringGrid1,2);
 Clean_Grid(av,2);
 Clean_Grid(asCrit,2);

 xrow=1;
 nb_taches=0; LASTTASK=0;
 NB_TACHES = 0;

 strcpy(fname,LocalBase); strcat(fname,"_crit.txt");
 fcrit = fopen(fname,"wb");
 strcpy(CR1,"|");  strcpy(CR2,"|"); strcpy(CR3,"|");  strcpy(CR4,"|");

 fputs("<c>1</c><l>P</l><d>PHASES</d><t></t>\n",fcrit);
 fputs("<c>2</c><l>I</l><d>INTERVENANTS</d><t></t>\n",fcrit);
 fputs("<c>3</c><l>T</l><d>TRIS</d><t></t>\n",fcrit);
 fputs("<c>4</c><l>R</l><d>RESSOURCES</d><t></t>\n",fcrit);
 fputs("<c>5</c><l>C5</l><d>Crit�re 5</d><t></t>\n",fcrit);
 fputs("<c>6</c><l>C6</l><d>Crit�re 6</d><t></t>\n",fcrit);
 fputs("<c>7</c><l>C7</l><d>Crit�re 7</d><t></t>\n",fcrit);
 fputs("<c>8</c><l>C8</l><d>Crit�re 8</d><t></t>\n",fcrit);

 tmax = 0;
 while (!feof(fp))
   {
    p=strchr(buf,'\r'); if (p) *p=0;
    p=strchr(buf,'\n'); if (p) *p=0;
    strcat(buf,",");
    cnt=Explode(',',buf);
    if (cnt>29) cnt=29;
    for (i=0;i<cnt;i++)
      {
       strcpy(field[i],"");
       p1=strchr(params[i]+1,'"');  // +1 pour eviter le " d'en tete
       if (p1) { *p1=0; strcpy(field[i],params[i]+1); }
      }
    // preparation de la ligne
    nt=atoi(field[0]);

    if (nt>0)
      {
        //if (xrow>1) { AdvStringGrid1->RowCount++; av->RowCount++; asCrit->RowCount++; }
        if (nt>tmax) tmax=nt;

        nb_taches++; new_nb_taches=nb_taches;
        sommets[nb_taches]=nt;     //

        if (nb_taches> 1) AdvStringGrid1->RowCount++; // = nt+1;
        if (nt>= av->RowCount) av->RowCount = nt+1;
        if (nb_taches > 1) asCrit->RowCount++;  // nt+1;

        AdvStringGrid1->Cells[1][nb_taches]=nt;     // numero tache
        // av->Cells[1][nb_taches]=nt;
        av->Cells[1][nt] = AnsiString(nt);
        asCrit->Cells[1][nb_taches]=AnsiString(nt);

        if (strcmp(field[1],"") == 0) strcpy(field[1],"_");
        AdvStringGrid1->Cells[2][nb_taches]=AnsiString(field[1]);  // designation
        // av->Cells[2][nb_taches]=AnsiString(field[1]);
        av->Cells[2][nt]=AnsiString(field[1]);
        asCrit->Cells[2][nb_taches]=AnsiString(field[1]);

        AdvStringGrid1->Cells[3][nb_taches]=AnsiString(atoi(field[2]));  // dur�e
        av->Cells[3][nt]=AnsiString(atoi(field[2]));
        duree[nb_taches]=atoi(field[2]);

        AdvStringGrid1->Cells[10][nb_taches]=1;  // calendrier
        av->Cells[10][nt]=1;
        tcal[nt]=1;

        AdvStringGrid1->Cells[12][nb_taches]=AnsiString(field[11]);  // debut souhait�
        av->Cells[12][nt]=AnsiString(field[11]);
        AdvStringGrid1->Cells[13][nb_taches]=AnsiString(field[12]);  // fin souhait�e
        av->Cells[13][nt]=AnsiString(field[12]);

        // Criteres
         av->Cells[41][nt]="P-" + AnsiString(field[3]);
         /*
         if (strlen(field[3])<3) av->Cells[41][nt] = av->Cells[41][nt] + "---";
         if (strlen(field[3])==5) av->Cells[41][nt] = av->Cells[41][nt] + "--";
         if (strlen(field[3])==7) av->Cells[41][nt] = av->Cells[41][nt] + "-";
         if (strlen(field[3])==9) av->Cells[41][nt] = av->Cells[41][nt];
         */
         av->Cells[51][nt]=AnsiString(field[4]);

         asCrit->Cells[3][nb_taches]= av->Cells[41][nt];  // "P-" + AnsiString(field[3])+ "---";
         asCrit->Cells[4][nb_taches]=AnsiString(field[4]);

        strcpy(needle,"|"); strcat(needle,field[3]); strcat(needle,"@");
        strcat(needle,field[4]); strcat(needle,"|");
        pcx = strstr(CR1,needle);
        if (pcx==NULL) strcat(CR1,needle+1) ;  // +1 pour eviter le double ||


        av->Cells[42][nt]="I-" + AnsiString(field[5])+ "---";
        av->Cells[52][nt]=AnsiString(field[6]);
        asCrit->Cells[5][nb_taches]="I-" + AnsiString(field[5])+ "---";
        asCrit->Cells[6][nb_taches]=AnsiString(field[6]);

        strcpy(needle,"|"); strcat(needle,field[5]); strcat(needle,"@");
        strcat(needle,field[6]); strcat(needle,"|");
        pcx = strstr(CR2,needle);
        if (pcx==NULL) strcat(CR2,needle+1) ;  // pour eviter le double ||


        av->Cells[43][nt]="T-" + AnsiString(field[7])+ "---";
        av->Cells[53][nt]=AnsiString(field[8]);
        asCrit->Cells[7][nb_taches]="T-" + AnsiString(field[7])+ "---";
        asCrit->Cells[8][nb_taches]=AnsiString(field[8]);

        strcpy(needle,"|"); strcat(needle,field[7]); strcat(needle,"@");
        strcat(needle,field[8]); strcat(needle,"|");
        pcx = strstr(CR3,needle);
        if (pcx==NULL) strcat(CR3,needle+1) ;  // pour eviter le double ||


        AdvStringGrid1->Cells[10][nb_taches]=1;  // calendrier
        av->Cells[10][nt]=1;

        AdvStringGrid1->Cells[11][nb_taches]=AnsiString(field[9]);  // code tache
        av->Cells[11][nt]=AnsiString(field[9]);

        mt = atof(field[10]);

        if (mt>=0.0)
          { sprintf(tmp,"%.2f",mt);
            AdvStringGrid1->Cells[18][nb_taches]=AnsiString(tmp);
            av->Cells[18][nt]= AnsiString(tmp);
            AdvStringGrid1->Cells[19][nb_taches]="0.00";
            av->Cells[19][nt]= "0.00";

          }
        else
          {
           sprintf(tmp,"%.2f",-mt);
           AdvStringGrid1->Cells[19][nb_taches]=AnsiString(tmp);
           av->Cells[19][nt]= AnsiString(tmp);
           AdvStringGrid1->Cells[18][nb_taches]="0.00";
           av->Cells[18][nt]= "0.00";

          }
        xrow++;
      }
    else // NT ==0
      {
       strcpy(needle,"|"); strcat(needle,field[3]); strcat(needle,"@");
       strcat(needle,field[4]); strcat(needle,"|");
       pcx = strstr(CR1,needle);
       if (pcx==NULL) strcat(CR1,needle+1) ;  // +1 pour eviter le double ||

      }

    strcpy(buf,"");
    fgets(buf,512,fp);
    ScanBuf(buf);
   }
 fclose(fp);
 LASTTASK=tmax; // nb_taches;
 NB_TACHES = nb_taches;
 new_nb_taches = nb_taches;

 SaveCriteres(true);

 // on examine les CR pour etablir les criteres
 p1=CR1+1;
 fputs("<c1>P</c1><c2></c2><c3></c3><c4></c4><d>PHASES</d><r></r><t></t>\n",fcrit);
 while (p1)
   {
    p2=strchr(p1,'|');
    if (p2==NULL) p1=NULL;
      else
       {
        strcpy(buf,"<c1>P</c1>");
        p3= strchr(p1,'@'); if (p3==NULL) { strcpy(champ1,"Erreur"); strcpy(champ2,"Erreur"); }
        else
          {
            strncpy(champ1,p1,p3-p1); champ1[p3-p1]=0;
            Compose_Cnn(XXX,champ1);
            strcat(buf,XXX);
            // strcat(buf,"</c2><c3></c3><c4></c4><d>");

            strcat(buf,"<d>");
            strncpy(champ2,p3+1,p2-p3-1); champ2[p2-p3-1]=0;
            strcat(buf,champ2);
            strcat(buf,"</d><r></r><t></t>\n");
            if (strlen(XXX)) fputs(buf,fcrit);
            p1=p2+1;
            if (*p1 == 0) p1=NULL;
          }
       }


   }

 p1=CR2+1;
 fputs("<c1>I</c1><c2></c2><c3></c3><c4></c4><d>INTERVENANTS</d><r></r><t></t>\n",fcrit);
 while (p1)
   {
    p2=strchr(p1,'|');
    if (p2==NULL) p1=NULL;
      else
       {
        strcpy(buf,"<c1>I</c1><c2>");
        p3= strchr(p1,'@'); if (p3==NULL) { strcpy(champ1,"Erreur"); strcpy(champ2,"Erreur"); }
        else
          {
            strncpy(champ1,p1,p3-p1); champ1[p3-p1]=0; strcat(buf,champ1);
            strcat(buf,"</c2><c3></c3><c4></c4><d>");
            strncpy(champ2,p3+1,p2-p3-1); champ2[p2-p3-1]=0;
            strcat(buf,champ2);
            strcat(buf,"</d><r></r><t></t>\n");
            if (strlen(champ1)) fputs(buf,fcrit);
            p1=p2+1;
            if (*p1 == 0) p1=NULL;
          }
       }
   }

p1=CR3+1;
 fputs("<c1>T</c1><c2></c2><c3></c3><c4></c4><d>TRIS</d><r></r><t></t>\n",fcrit);
 while (p1)
   {
    p2=strchr(p1,'|');
    if (p2==NULL) p1=NULL;
      else
       {
        strcpy(buf,"<c1>T</c1><c2>");
        p3= strchr(p1,'@'); if (p3==NULL) { strcpy(champ1,"Erreur"); strcpy(champ2,"Erreur"); }
        else
          {
            strncpy(champ1,p1,p3-p1); champ1[p3-p1]=0; strcat(buf,champ1);
            strcat(buf,"</c2><c3></c3><c4></c4><d>");
            strncpy(champ2,p3+1,p2-p3-1); champ2[p2-p3-1]=0;
            strcat(buf,champ2);
            strcat(buf,"</d><r></r><t></t>\n");
            if (strlen(champ1)) fputs(buf,fcrit);
            p1=p2+1;
            if (*p1 == 0) p1=NULL;
          }
       }
   }

fputs("<c1>C4</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 4</d><r></r><t></t>\n",fcrit);
fputs("<c1>C5</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 5</d><r></r><t></t>\n",fcrit);
fputs("<c1>C6</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 6</d><r></r><t></t>\n",fcrit);
fputs("<c1>C7</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 7</d><r></r><t></t>\n",fcrit);
fputs("<c1>C8</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 8</d><r></r><t></t>\n",fcrit);

fclose(fcrit);

if (strlen(m_importcriteres))
   Build_Criteres();


 //   Successeurs et Predecesseurs   : Fichier LIAISONS.TXT

 for (i=0;i<=LASTTASK;i++)
 // for (i=0;i<=(nb_taches+10);i++)     //  &&&&&&&&&&&&
     {
      deb_plus_tot[i]=0; fin_plus_tot[i]=0;
      deb_plus_tard[i]=0; fin_plus_tard[i]=0;
      coche[i]= ' '; coch[i]=' ';
      filtr[i]= 'X';
      tmom[i][0]=0;
      da[i][0]=0; dtr[i][0]=0; ftr[i][0]=0;
      pca[i]=0; nbj[i]=0;
      dds[i][0]=0; dfs[i][0]=0;
      retard[i]=0;

      for (j=0;j<PS;j++)
        {
         pr[i][j]=0; su[i][j]=0;
         pr_tl[i][j]=0; su_tl[i][j]=0;
         pr_decal[i][j]=0; su_decal[i][j]=0;

        }
      su[i][1]=9999;
      su[i][2]=0;
     }

  strcpy(filename,m_importliaisons); // strcat(filename,"\\LIAS.TXT");
  fp=fopen(filename,"rb");
  if (fp==NULL)
   {
    Application->MessageBoxA("Fichier Transfert LIAISONS.TXT non trouv�",m_ecoplan,MB_OK);
    return;
   }

  fgets(buf,512,fp);
  while (!feof(fp))
    {
     p=strchr(buf,'\r'); if (p) *p=0;
     p=strchr(buf,'\n'); if (p) *p=0;
     strcat(buf,",");
     cnt=Explode(',',buf);
     if (cnt>29) cnt=29;
     for (i=0;i<cnt;i++)
      {
       strcpy(field[i],"");
       p1=strchr(params[i]+1,'"');  // +1 pour eviter le " d'en tete
       if (p1) { *p1=0; strcpy(field[i],params[i]+1); }
      }
    // preparation de la ligne
      nt=atoi(field[0]);
      if (nt>0)
      {

       strcpy(tmp,field[3]);   strupr(tmp);
       typl=0;
        if (strcmp(tmp,"FD")==0 || strcmp(tmp,"fd")==0 ) typl = 0;
        else if (strcmp(tmp,"FF")==0 || strcmp(tmp,"ff")==0) typl= 1;
        else if (strcmp(tmp,"AM")==0 || strcmp(tmp,"am")==0) typl= 2;
        else if (strcmp(tmp,"DD")==0 || strcmp(tmp,"dd")==0) typl= 3;
        succ=atoi(field[1]);
        idecal=atoi(field[2]);

        if (typl==3 && idecal <0)
          {
           sprintf(mess,"Attention : Lien DD avec d�calage n�gatif Tache %d D�calage %d",
                   nt,idecal);
           Application->MessageBoxA(mess,m_ecoplan,MB_OK);
           idecal = 0;
          }

        InsertSuccSeul(nt,succ,typl,idecal,true);
        InsertPredSeul(succ,nt,typl,idecal,true);



      }
     fgets(buf,512,fp);
    }



   // Boucle pour effectuer debut et fin souhait�e
  av1count = AdvStringGrid1->RowCount;
  for (i=1;i<av1count;i++)
    {
     //if (AdvStringGrid1->Cells[12][i] != "")
       //Compute_DebutSouhaite(12,i);
     //if (AdvStringGrid1->Cells[13][i] != "")
       //Compute_FinSouhaitee(13,i);

    }
  fclose(fp);

  //  Charger Pred et Succ dans AV
  Fill_PredSucc_Av();

  // pour charger les LibRupt
  Read_Criteres();


  // Possibilit� de "fermer le projet"
 /*
 MainForm->FileOpenItem->Enabled=false;
 MainForm->FileCloseItem->Enabled =true;
 */
  // update database
/*
  updatedatabase(LocalBase,0,0);
  ReloadAllData(LocalBase);
*/

  grefresh(current_task,'0');
  Change1();
}


void __fastcall TplStandard::Compose_Cnn(char *dst,char *ch)
{
 char t[100];

 dst[0]=0;
 strcpy(t,ch);

 if (strlen(ch)==4) {strcpy(dst,"<c2></c2><c3></c3><c4></c4>");return; }

 if (strlen(ch)==5)
   {
    strcpy(dst,"<c2>");
    dst[4]=ch[0]; dst[5]=ch[1]; dst[6]=0;
    strcat (dst,"</c2><c3></c3><c4></c4>");
    return;
   }
 if (strlen(ch)==7)
   {
    strcpy(dst,"<c2>");
    dst[4]=ch[0]; dst[5]=ch[1]; dst[6]=0;
    strcat(dst,"</c2><c3>");
    dst[15]=ch[3]; dst[16]=ch[4]; dst[17]=0;
    strcat(dst,"</c3><c4></c4>");
    return;

   }
 if (strlen(ch)==9)
   {
    strcpy(dst,"<c2>");
    dst[4]=ch[0]; dst[5]=ch[1]; dst[6]=0;
    strcat(dst,"</c2><c3>");
    dst[15]=ch[3]; dst[16]=ch[4]; dst[17]=0;
    strcat(dst,"</c3><c4>");
    dst[26]=ch[6]; dst[27]=ch[7]; dst[28]=0;
    strcat(dst,"</c4>");
    return;

   }

}


void __fastcall  TplStandard::RemoveQuotes(char *pr)
{
 int lg,i,m;
 char res[500];

 lg = strlen(pr);
 if (lg==0) return;
 if (strcmp(pr,"\"\"")==0) {strcpy(pr,""); return; }

 m=0;
 for (i=0;i<lg;i++)
   {
    if (pr[i] != '"')
      { res[m]=pr[i];
        m++; res[m]=0;
      }
   }
 strcpy(pr,res);

}

void __fastcall  TplStandard::Extract_Liens(char *fld,int nt,char *liens)
{
 char seppred[10]; char *p,*p1;
 int typl;int pred;int idecal; int i;
 char tmp[200];  int cnt;  char fldx[20];
 char fmt[100];
 if (strlen(fld)==0) return;
 //seppred[0]=SEPPRED; seppred[1]=0;
 //strcat(fld,seppred);
  strupr(fld);
  cnt = Explode('~',fld);
  for (i=0;i<cnt;i++)
    {
     strcpy(fldx,params[i]);
     typl = -1;
     p=strstr(fldx,"FD"); if (p) { typl=0; p1=p; }
     p=strstr(fldx,"FF"); if (p) { typl=1;  p1=p; }
     p=strstr(fldx,"AM"); if (p) { typl=2;  p1=p; }
     p=strstr(fldx,"DD"); if (p) { typl=3;  p1=p; }
     if (typl!=-1)
        {
          strncpy(tmp,fldx,p1-fldx);
          tmp[p1-fldx]=0;
          pred=atoi(tmp);
          p1=p1+2; // pointe sur decalage
          strcpy(tmp,p1);
          idecal=atoi(tmp);
          if (strcmp(liens,"PRED")==0)
           {
             InsertPredSeul(nt,pred,typl,idecal,true);
             InsertSuccSeul(pred,nt,typl,idecal,true);
           }
          else
           {
             InsertPredSeul(pred,nt,typl,idecal,true);
             InsertSuccSeul(nt,pred,typl,idecal,true);

           }
        }
    }
}

void __fastcall TplStandard::Exec_Import_CSV()
{

 // a ce point : la base est cr�e, vide
 // Lire les fichiers LIAS.TXT et TACHE.TXT  et Criteres
 //
 TTableCSV *ColExcel;

 FILE *fp;   FILE *fcrit;  char needle[255]; char *pcx;
 char fname[MAX_PATH];
 char filename[MAX_PATH];
 char buf[512]; char *p,*p1,*p2,*p3;
 char champ1[100]; char champ2[200];
 char mess[1000];
 char field[30][1000];  // maximum 30 champs
 int i,j,cnt,xrow,nt;
 float mt; char tmp[250];
 int succ,idecal,typl;
 int av1count,tmax; int cntp;   int ctrcrit;
 char sepchamps[10];
 char CR1[5000];   // stockage des criteres
 char CR2[5000];
 char CR3[5000];
 char CR4[5000];


  // Application->MessageBoxA("Attention : Bug, corrections en cours",m_ecoplan,MB_OK);
  // return;


 strcpy(filename,m_importtaches); // strcat(filename,"\\TACHE.TXT");
 fp = fopen(filename,"rb");
 if (fp==NULL)
   {
    Application->MessageBoxA("Fichier CSV non trouv�",m_ecoplan,MB_OK);
    return;
   }

 ColExcel=new TTableCSV(Application);
 ColExcel->ShowModal();
 delete ColExcel;


 if (strlen(m_exchange)==0) { fclose(fp); return; }

 sepchamps[0]=SEPCHAMPS; sepchamps[1]=0;

 strcpy(buf,"");   // skip First record
 fgets(buf,512,fp);

 strcpy(buf,"");
 fgets(buf,512,fp);

 ScanBuf(buf);

 Clean_Grid(AdvStringGrid1,2);
 Clean_Grid(av,2);
 Clean_Grid(asCrit,2);
  xrow=1;
 nb_taches=0; LASTTASK=0;
 NB_TACHES = 0;

 /*

 */

 tmax = 0;
 while (!feof(fp))
   {
    p=strchr(buf,'\r'); if (p) *p=0;
    p=strchr(buf,'\n'); if (p) *p=0;
    strcat(buf,sepchamps); // ",");
    cnt=Explode(SEPCHAMPS,buf);
    if (cnt>29) cnt=29;
    for (i=0;i<cnt;i++)
      {
       RemoveQuotes(params[i]);
       strcpy(field[i],params[i]);

       /*
       p1=strchr(params[i]+1,'"');  // +1 pour eviter le " d'en tete
       if (p1)
        { *p1=0;
           strcpy(field[i],params[i]+1);
          strcpy(tmp,field[i]);
          *p1=0;
        }
       */
      }
    // preparation de la ligne

    if (COL1==0) strcpy(tmp,"");
    else strcpy(tmp,field[COL1-1]);
    nt=atoi(tmp); //[0]

    if (nt>0)
      {
        //if (xrow>1) { AdvStringGrid1->RowCount++; av->RowCount++; asCrit->RowCount++; }
        if (nt>tmax) tmax=nt;

        nb_taches++; new_nb_taches=nb_taches;
        sommets[nb_taches]=nt;     //

        if (nb_taches> 1) AdvStringGrid1->RowCount++; // = nt+1;
        if (nt>= av->RowCount) av->RowCount = nt+1;
        if (nb_taches > 1) asCrit->RowCount++;  // nt+1;


        AdvStringGrid1->Cells[1][nb_taches]=AnsiString(nt);     // numero tache

        av->Cells[1][nt] = AnsiString(nt);
        asCrit->Cells[1][nb_taches]=AnsiString(nt);

        if (COL2==0) strcpy(tmp,"_");
        else strcpy(tmp,field[COL2-1]);  // 1]);
        if (strcmp(tmp,"") == 0) strcpy(tmp,"_");
        AdvStringGrid1->Cells[2][nb_taches]=AnsiString(tmp);  // designation
        av->Cells[2][nt]=AnsiString(tmp);
        asCrit->Cells[2][nb_taches]=AnsiString(tmp);

        if (COL3==0) strcpy(tmp,"1"); else strcpy(tmp,field[COL3-1]);
        AdvStringGrid1->Cells[3][nb_taches]=AnsiString(atoi(tmp));  // dur�e
        av->Cells[3][nt]=AnsiString(atoi(tmp));
        duree[nb_taches]=atoi(tmp);

        AdvStringGrid1->Cells[10][nb_taches]=1;  // calendrier
        av->Cells[10][nt]=1;
        tcal[nt]=1;

        if (COL4==0) strcpy(tmp,""); else strcpy(tmp,field[COL4-1]);
        AdvStringGrid1->Cells[12][nb_taches]=AnsiString(tmp);  // debut souhait�
        av->Cells[12][nt]=AnsiString(tmp);

        if (COL5==0) strcpy(tmp,""); else strcpy(tmp,field[COL5-1]);
        AdvStringGrid1->Cells[13][nb_taches]=AnsiString(tmp);  // fin souhait�e
        av->Cells[13][nt]=AnsiString(tmp);

        // EXTRACT PREDECESSEURS ET SUCCESSEURS
        //RAZ
        deb_plus_tot[nt]=0; fin_plus_tot[nt]=0; deb_plus_tard[nt]=0; fin_plus_tard[nt]=0;
        coche[nt]= ' '; coch[nt]=' '; filtr[nt]= 'X'; tmom[nt][0]=0;
      da[nt][0]=0; dtr[nt][0]=0; ftr[nt][0]=0; pca[nt]=0; nbj[nt]=0; dds[nt][0]=0; dfs[nt][0]=0;
      retard[nt]=0;
      for (j=0;j<PS;j++)
        {
         pr[nt][j]=0; su[nt][j]=0; pr_tl[nt][j]=0; su_tl[nt][j]=0; pr_decal[nt][j]=0; su_decal[nt][j]=0;
        }
      su[nt][1]=9999; su[nt][2]=0;
      if (cnt>29) cnt=29;
      // PREDECESSEURS DANS COL6-1
       if (COL6>0) Extract_Liens(field[COL6-1],nt,"PRED");
       if (COL7>0) Extract_Liens(field[COL7-1],nt,"SUCC");

       // CRITERES DANS AV et ADVSTR1

       if (COL8==0) strcpy(tmp,""); else   strcpy(tmp,field[COL8-1]);
       AdvStringGrid1->Cells[15][nb_taches]=AnsiString(tmp);   // P et I invers�
       av->Cells[41][nt]= AnsiString(tmp);

       if (COL9==0) strcpy(tmp,""); else strcpy(tmp,field[COL9-1]);
       av->Cells[51][nt]= AnsiString(tmp);


       if (COL10==0) strcpy(tmp,""); else strcpy(tmp,field[COL10-1]);
       AdvStringGrid1->Cells[14][nb_taches]=AnsiString(tmp);
       av->Cells[42][nt]= AnsiString(tmp);


       if (COL11==0) strcpy(tmp,""); else strcpy(tmp,field[COL11-1]);
       av->Cells[52][nt]= AnsiString(tmp);

       if (COL12==0) strcpy(tmp,""); else strcpy(tmp,field[COL12-1]);
       AdvStringGrid1->Cells[16][nb_taches]=AnsiString(tmp);
       av->Cells[43][nt]= AnsiString(tmp);

       if (COL13==0) strcpy(tmp,""); else strcpy(tmp,field[COL13-1]);
       av->Cells[53][nt]= AnsiString(tmp);

       if (COL14==0) strcpy(tmp,""); else strcpy(tmp,field[COL14-1]);
       //  AdvStringGrid1->Cells[14][nb_taches]=AnsiString(tmp); // RESSOURCE PAS RENSEIGN2
       av->Cells[44][nt]= AnsiString(tmp);

       if (COL15==0) strcpy(tmp,""); else  strcpy(tmp,field[COL15-1]);
       av->Cells[54][nt]= AnsiString(tmp);

       if (COL8==0) strcpy(tmp,""); else strcpy(tmp,field[COL16-1]);
        AdvStringGrid1->Cells[11][nb_taches]=AnsiString(tmp);  // code tache
        av->Cells[11][nt]=AnsiString(tmp);

        if (COL17==0) strcpy(tmp,""); else strcpy(tmp,field[COL17-1]);
        mt = atof(tmp);

        //if (mt>=0.0)
         //  {
            sprintf(tmp,"%.2f",mt);
            AdvStringGrid1->Cells[18][nb_taches]=AnsiString(tmp);
            av->Cells[18][nt]= AnsiString(tmp);
            AdvStringGrid1->Cells[19][nb_taches]="0.00";
           // av->Cells[19][nt]= "0.00";
        //  }
        //else
        //  {
         if (COL18==0) strcpy(tmp,""); else strcpy(tmp,field[COL18-1]);
         mt = atof(tmp);
           sprintf(tmp,"%.2f",-mt);
           AdvStringGrid1->Cells[19][nb_taches]=AnsiString(tmp);
           av->Cells[19][nt]= AnsiString(tmp);
           AdvStringGrid1->Cells[18][nb_taches]="0.00";
          ///  av->Cells[18][nt]= "0.00";

        //  }

        xrow++;
      }
    strcpy(buf,"");
    fgets(buf,512,fp);
    ScanBuf(buf);
   }
 fclose(fp);
 LASTTASK=tmax; // nb_taches;
 NB_TACHES = nb_taches;
 new_nb_taches = nb_taches;


 strcpy(filename,m_importcriteres); // strcat(filename,"\\TACHE.TXT");
 fp = fopen(filename,"rb");
 if (fp==NULL)
   {
    Application->MessageBoxA("Fichier Crit�res CSV non trouv�",m_ecoplan,MB_OK);
    return;
   }

 strcpy(fname,LocalBase); strcat(fname,"_crit.txt");
 fcrit = fopen(fname,"wb");
 strcpy(CR1,"|");  strcpy(CR2,"|"); strcpy(CR3,"|");

 fputs("<c>1</c><l>P</l><d>PHASES</d><t></t>\n",fcrit);
 fputs("<c>2</c><l>I</l><d>INTERVENANTS</d><t></t>\n",fcrit);
 fputs("<c>3</c><l>T</l><d>TRIS</d><t></t>\n",fcrit);
 fputs("<c>4</c><l>R</l><d>RESSOURCES</d><t></t>\n",fcrit);
 fputs("<c>5</c><l>C5</l><d>Crit�re 5</d><t></t>\n",fcrit);
 fputs("<c>6</c><l>C6</l><d>Crit�re 6</d><t></t>\n",fcrit);
 fputs("<c>7</c><l>C7</l><d>Crit�re 7</d><t></t>\n",fcrit);
 fputs("<c>8</c><l>C8</l><d>Crit�re 8</d><t></t>\n",fcrit);

 ctrcrit=0;

 // Skip First record
 strcpy(buf,"");
 fgets(buf,512,fp);

 strcpy(buf,"");
 fgets(buf,512,fp);

 while (!feof(fp))
   {

    p=strchr(buf,'\r'); if (p) *p=0;
    p=strchr(buf,'\n'); if (p) *p=0;
    strcat(buf,sepchamps); // ",");
    cnt=Explode(SEPCHAMPS,buf);
    if (cnt>29) cnt=29;
    for (i=0;i<cnt;i++)
      {
       RemoveQuotes(params[i]);
       strcpy(field[i],params[i]);
      }
     ctrcrit++;
     if (ctrcrit <=8) { strcpy(buf,""); fgets(buf,512,fp);} // SKIP
     else
       {
        strcpy(mess,"<c1>"); strcat(mess,field[0]); strcat(mess,"</c1>");
        strcat(mess,"<c2>"); strcat(mess,field[1]); strcat(mess,"</c2>");
        strcat(mess,"<c3>"); strcat(mess,field[2]); strcat(mess,"</c3>");
        strcat(mess,"<c4>"); strcat(mess,field[3]); strcat(mess,"</c4>");
        strcat(mess,"<d>"); strcat(mess,field[4]); strcat(mess,"</d>");
        strcat(mess,"<r>"); strcat(mess,"</r>");
        strcat(mess,"<t>"); strcat(mess,"</t>\n");
        fputs(mess,fcrit);
        strcpy(buf,"");
        fgets(buf,512,fp);
       }
    }


  fclose(fp);
  fclose (fcrit);


  // Examen existence des pred ou successeurs

  // boucle sur les taches pr�sentes dans AdvStringGrid1
  cnt = AdvStringGrid1->RowCount;
  strcpy(CR1,"");  // zone d'affichage
  strcpy(CR2,"");
  for (i=1;i<cnt;i++)
    {
     strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
     nt = atoi(tmp);
     if (nt>0)
      {
       //sprintf(mess,"T�che N� %d ",nt);
       //strcpy(CR2,mess);

       for (j=1;j<NS;j++)
         {
          succ=su[nt][j];
          if (succ==9999) break;
          if (succ != 0)
            {
             if (succ > LASTTASK)
              {
              sprintf(mess,"T�che %d -- Succ. Absent %d \n",nt,succ);
              strcat(CR1,mess);
              RemoveSuccSeul(nt,succ);
               }
             else if (av->Cells[1][succ]=="")
               {
               sprintf(mess,"T�che %d -- Succ. Absent %d \n",nt,succ);
               strcat(CR1,mess);
               RemoveSuccSeul(nt,succ);
               }
            }   

         }

      }


    }


 if (strlen(CR1))
   { //strcpy(CR3,CR2); strcat(CR3,CR1);
    Application->MessageBox(CR1,m_ecoplan,MB_OK);

   }





  // pour charger les LibRupt
 Read_Criteres();

  // Possibilit� de "fermer le projet"

 //MainForm->FileOpenItem->Enabled=false;

 // MainForm->FileCloseItem->Enabled =true;

  // update database

  updatedatabase(LocalBase,0,0);
  ReloadAllData(LocalBase,true,false);


grefresh(current_task,'0');
Change1();
}



void __fastcall TplStandard::Fill_PredSucc_Av()
{
 int isucc,ipred,i,indx,nt;
 char ps[10000],tmp[50];

 for (i=1;i<=LASTTASK;i++)
   {
     ipred=1; strcpy(ps,"|");
     while ((pr[i][ipred] != 0) && (ipred<PS))
        {
          nt= pr[i][ipred];  //
          sprintf(tmp,"%d",nt);
          strcat(ps,tmp); strcat(ps,"|");
          ipred++;

        }
      ipred--;
      av->Cells[61][i]=AnsiString(ipred);
      av->Cells[62][i]=AnsiString(ps);

     isucc=1; strcpy(ps,"|");
      while ((su[i][isucc] != 0) && (isucc<PS))
         {
           nt= su[i][isucc];  //
           if (nt!=9999)
              {
                sprintf(tmp,"%d",nt);
                strcat(ps,tmp);  strcat(ps,"|");

              }
            isucc++;   
          }

         isucc--;
         strcat(ps,"9999|");
         av->Cells[63][i]=AnsiString(isucc);
         av->Cells[64][i]=AnsiString(ps);    // OK

    } // end For

}

void __fastcall TplStandard::Build_Criteres()
{
 FILE *fimport,*fcrit;
 char fname[MAX_PATH],impcrit[MAX_PATH];
 char buf[512],tmp[200],des[200];
 char val[200];

 strcpy(fname,LocalBase); strcat(fname,"_crit.txt");
 fcrit = fopen(fname,"wb");
 if (fcrit==NULL)
    { Application->MessageBoxA("Erreur a l'ecriture du fichier Criteres",m_ecoplan, MB_OK);
      return;
    }
  fputs("<c>1</c><l>P</l><d>PHASES</d><r></r><t></t>\n",fcrit);
  fputs("<c>2</c><l>I</l><d>INTERVENANTS</d><r></r><t></t>\n",fcrit);
  fputs("<c>3</c><l>T</l><d>TRIS</d><r></r><t></t>\n",fcrit);
  fputs("<c>4</c><l>R</l><d>RESSOURCES</d><r></r><t></t>\n",fcrit);
  fputs("<c>5</c><l>C5</l><d>Crit�re 5</d><r></r><t></t>\n",fcrit);
  fputs("<c>6</c><l>C6</l><d>Crit�re 6</d><r></r><t></t>\n",fcrit);
  fputs("<c>7</c><l>C7</l><d>Crit�re 7</d><r></r><t></t>\n",fcrit);
  fputs("<c>8</c><l>C8</l><d>Crit�re 8</d><r></r><t></t>\n",fcrit);

  fputs("<c1>P</c1><c2></c2><c3></c3><c4></c4><d>PHASES</d><r></r><t></t>\n",fcrit);

 strcpy(impcrit,m_importcriteres);
 fimport = fopen(impcrit,"rb");
 if (fimport == NULL)
    { Application->MessageBoxA("Erreur a l'ouverture du fichier import Criteres",m_ecoplan, MB_OK);
      return;
    }
 strcpy(buf,"");
 fgets(buf,500,fimport);
 while (!feof(fimport))
   {
    Ghost->ExtractValue(des,buf,"d",0);
    Ghost->ExtractValue(tmp,buf,"c",0);
    Explode('-',tmp);  strcpy(val,"<c1>P</c1>");
    sprintf(val,"<c1>P</c1><c2>%s</c2><c3>%s</c3><c4>%s</c4><d>%s</d>\n",
    params[0],params[1],params[2],des);
    fputs(val,fcrit);
    strcpy(buf,"");
    fgets(buf,500,fimport);
   }
  fclose(fimport);
  fputs("<c1>I</c1><c2></c2><c3></c3><c4></c4><d>INTERVENANTS</d><r></r><t></t>\n",fcrit);
  fputs("<c1>T</c1><c2></c2><c3></c3><c4></c4><d>TRIS</d><r></r><t></t>\n",fcrit);
  fputs("<c1>R</c1><c2></c2><c3></c3><c4></c4><d>RESSOURCES</d><r></r><t></t>\n",fcrit);
  fputs("<c1>C5</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 5</d><r></r><t></t>\n",fcrit);
  fputs("<c1>C6</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 6</d><r></r><t></t>\n",fcrit);
  fputs("<c1>C7</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 7</d><r></r><t></t>\n",fcrit);
  fputs("<c1>C8</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 8</d><r></r><t></t>\n",fcrit);
 fclose (fcrit);

 /*

 p1=CR1+1;
 fputs("<c1>P</c1><c2></c2><c3></c3><c4></c4><d>PHASES</d><r></r><t></t>\n",fcrit);
 while (p1)
   {
    p2=strchr(p1,'|');
    if (p2==NULL) p1=NULL;
      else
       {
        strcpy(buf,"<c1>P</c1><c2>");
        p3= strchr(p1,'@'); if (p3==NULL) { strcpy(champ1,"Erreur"); strcpy(champ2,"Erreur"); }
        else
          {
            strncpy(champ1,p1,p3-p1); champ1[p3-p1]=0; strcat(buf,champ1);
            strcat(buf,"</c2><c3></c3><c4></c4><d>");
            strncpy(champ2,p3+1,p2-p3-1); champ2[p2-p3-1]=0;
            strcat(buf,champ2);
            strcat(buf,"</d><r></r><t></t>\n");
            if (strlen(champ1)) fputs(buf,fcrit);
            p1=p2+1;
            if (*p1 == 0) p1=NULL;
          }
       }


   }

 p1=CR2+1;
 fputs("<c1>I</c1><c2></c2><c3></c3><c4></c4><d>INTERVENANTS</d><r></r><t></t>\n",fcrit);
 while (p1)
   {
    p2=strchr(p1,'|');
    if (p2==NULL) p1=NULL;
      else
       {
        strcpy(buf,"<c1>I</c1><c2>");
        p3= strchr(p1,'@'); if (p3==NULL) { strcpy(champ1,"Erreur"); strcpy(champ2,"Erreur"); }
        else
          {
            strncpy(champ1,p1,p3-p1); champ1[p3-p1]=0; strcat(buf,champ1);
            strcat(buf,"</c2><c3></c3><c4></c4><d>");
            strncpy(champ2,p3+1,p2-p3-1); champ2[p2-p3-1]=0;
            strcat(buf,champ2);
            strcat(buf,"</d><r></r><t></t>\n");
            if (strlen(champ1)) fputs(buf,fcrit);
            p1=p2+1;
            if (*p1 == 0) p1=NULL;
          }
       }
   }

p1=CR3+1;
 fputs("<c1>T</c1><c2></c2><c3></c3><c4></c4><d>TRIS</d><r></r><t></t>\n",fcrit);
 while (p1)
   {
    p2=strchr(p1,'|');
    if (p2==NULL) p1=NULL;
      else
       {
        strcpy(buf,"<c1>T</c1><c2>");
        p3= strchr(p1,'@'); if (p3==NULL) { strcpy(champ1,"Erreur"); strcpy(champ2,"Erreur"); }
        else
          {
            strncpy(champ1,p1,p3-p1); champ1[p3-p1]=0; strcat(buf,champ1);
            strcat(buf,"</c2><c3></c3><c4></c4><d>");
            strncpy(champ2,p3+1,p2-p3-1); champ2[p2-p3-1]=0;
            strcat(buf,champ2);
            strcat(buf,"</d><r></r><t></t>\n");
            if (strlen(champ1)) fputs(buf,fcrit);
            p1=p2+1;
            if (*p1 == 0) p1=NULL;
          }
       }
   }

fputs("<c1>C4</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 4</d><r></r><t></t>\n",fcrit);
fputs("<c1>C5</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 5</d><r></r><t></t>\n",fcrit);
fputs("<c1>C6</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 6</d><r></r><t></t>\n",fcrit);
fputs("<c1>C7</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 7</d><r></r><t></t>\n",fcrit);
fputs("<c1>C8</c1><c2></c2><c3></c3><c4></c4><d>Crit�re 8</d><r></r><t></t>\n",fcrit);

fclose(fcrit);
*/


}


void __fastcall TplStandard::ScanBuf(char *buf)
{
 /*
 bool status;
 int  i,l;

 l = strlen(buf);
 status = false;
 for (i=0;i<l;i++)
  {
   if (buf[i]=='"')
     {
      if (status==false) status = true;
      else if (status==true) status = false;
     }
   if (status== true && buf[i]==',')
     buf[i] = '-';
  }
}

*/
 bool status;
 int  i,l;

 l = strlen(buf);
 status = true;
 for (i=0;i<l;i++)
  {
   if (buf[i]=='"')
     {
      if (status==false) status = true;
      else if (status==true) status = false;
     }
   //  if (status== true && buf[i]==SEPCHAMPS)  // ',')
   //  buf[i] = '-';
   if (status==false && buf[i]==SEPPRED)
      buf[i]='~';
  }
}


void __fastcall TplStandard::Exec_Export_Excel(bool question)
{
 char name[1000];
 char xmlname[1000];
 char tmp[1250];
 FILE *fp,*fp1; int i,kt;    char xtr[5000];
 char xx[50]; float mtp,mtn;
 int nbsucc,succ,isucc,tl,dec;
 char small[100];  int cd;   char *p;

  // Sauvegarder Fichier LIAISONS   et fichier TACHES

  if (question)
   {
    Application->MessageBoxA("Vous allez indiquer o� sauvegarder le fichier Taches",m_ecoplan,MB_OK);
    SaveDialog1->FileName = "Taches.txt";
    if (SaveDialog1->Execute()) strcpy(name,SaveDialog1->FileName.c_str());
     else return;
   }
   else
   {
    strcpy(name,m_directory); strcat(name,"\\TACHES.TXT");

   }

  strcpy(xmlname,name);
  p = strstr(xmlname,"Taches");
  if (p) strncpy(p,"XXXXXX",6);

  fp=fopen(name,"wt");
  fp1=fopen(xmlname,"wt");

  randomize();

  for (i=1; i<=av->RowCount; i++)

        {
         if (av->Cells[1][i] != "")
          {
            strcpy(tmp,"");
            strcat(tmp,"\""); strcat(tmp,av->Cells[1][i].c_str()); strcat(tmp,"\",");  // N�
            strcat(tmp,"\""); strcat(tmp,av->Cells[2][i].c_str()); strcat(tmp,"\",");  // Des
            strcat(tmp,"\""); strcat(tmp,av->Cells[3][i].c_str()); strcat(tmp,"\",");  // Dur�e

            // criteres

            strcpy(small,av->Cells[41][i].c_str()); //if (strlen(small) > 5) small[5]=0;
            strcat(tmp,"\""); strcat(tmp,small); strcat(tmp,"\",");
            strcat(tmp,"\""); strcat(tmp,av->Cells[51][i].c_str()); strcat(tmp,"\",");
            strcpy(small,av->Cells[42][i].c_str()); // if (strlen(small) > 5) small[5]=0;
            strcat(tmp,"\""); strcat(tmp,small); strcat(tmp,"\",");
            strcat(tmp,"\""); strcat(tmp,av->Cells[52][i].c_str()); strcat(tmp,"\",");
            strcpy(small,av->Cells[43][i].c_str()); // if (strlen(small) > 5) small[5]=0;
            strcat(tmp,"\""); strcat(tmp,small); strcat(tmp,"\",");
            strcat(tmp,"\""); strcat(tmp,av->Cells[53][i].c_str()); strcat(tmp,"\",");

            strcat(tmp,"\""); strcat(tmp,av->Cells[11][i].c_str()); strcat(tmp,"\",");  // Code tache
            strcpy(xx,av->Cells[18][i].c_str()); mtp=atof(xx);     // MT positif
            strcpy(xx,av->Cells[19][i].c_str()); mtn=atof(xx);     // MT Negatif

            mtp=mtp-mtn; sprintf(xx,"%2f",mtp);
            strcat(tmp,"\""); strcat(tmp,xx); strcat(tmp,"\",");

            strcat(tmp,"\""); strcat(tmp,av->Cells[12][i].c_str()); strcat(tmp,"\",");  // Debut Souhait�
            strcat(tmp,"\""); strcat(tmp,av->Cells[13][i].c_str()); strcat(tmp,"\"");  // Fin Souhait�e
            strcat(tmp,"\n");
            fputs(tmp,fp);
            // Idem pour XMLTaches

            strcpy(tmp,"");
            strcat(tmp,"<nt>"); strcat(tmp,av->Cells[1][i].c_str()); strcat(tmp,"</nt>");  // N�
            strcat(tmp,"<des>"); strcat(tmp,av->Cells[2][i].c_str()); strcat(tmp,"</des");  // Des
            strcat(tmp,"<dur>"); strcat(tmp,av->Cells[3][i].c_str()); strcat(tmp,"</dur>");  // Dur�e

            // criteres

            strcpy(small,av->Cells[41][i].c_str()); //if (strlen(small) > 5) small[5]=0;
            strcat(tmp,"<cr1>"); strcat(tmp,small); strcat(tmp,"</cr1>");
            strcat(tmp,"<des1>"); strcat(tmp,av->Cells[51][i].c_str()); strcat(tmp,"</des1>");
            strcpy(small,av->Cells[42][i].c_str()); // if (strlen(small) > 5) small[5]=0;
            strcat(tmp,"<cr2>"); strcat(tmp,small); strcat(tmp,"</cr2>");
            strcat(tmp,"<des2>"); strcat(tmp,av->Cells[52][i].c_str()); strcat(tmp,"</des2>");
            strcpy(small,av->Cells[43][i].c_str()); // if (strlen(small) > 5) small[5]=0;
            strcat(tmp,"<cr3>"); strcat(tmp,small); strcat(tmp,"</cr3>");
            strcat(tmp,"<des3>"); strcat(tmp,av->Cells[53][i].c_str()); strcat(tmp,"</des3>");

            strcat(tmp,"<dtot>"); strcat(tmp,av->Cells[4][i].c_str()); strcat(tmp,"</dtot>");
            strcat(tmp,"<ftot>"); strcat(tmp,av->Cells[5][i].c_str()); strcat(tmp,"</ftot>");

            //ici on genere un code au hasard

            cd = random(9999);
            sprintf(xx,"%d",cd);


            strcat(tmp,"<code>"); strcat(tmp,xx); strcat(tmp,"</code>");  // Code tache
            strcpy(xx,av->Cells[18][i].c_str()); mtp=atof(xx);     // MT positif
            strcpy(xx,av->Cells[19][i].c_str()); mtn=atof(xx);     // MT Negatif

            mtp=mtp-mtn; sprintf(xx,"%2f",mtp);
            strcat(tmp,"<cout>"); strcat(tmp,xx); strcat(tmp,"</cout>");
            strcat(tmp,"\n");
            fputs(tmp,fp1);

          }
       }   // For
  fclose(fp);
  fclose (fp1);

  if (question)
   {
    Application->MessageBoxA("Vous allez indiquer o� sauvegarder le fichier Liaisons",m_ecoplan,MB_OK);
    SaveDialog1->FileName = "Liaisons.txt";
    if (SaveDialog1->Execute())
     {
      strcpy(name,SaveDialog1->FileName.c_str());
     }
   else return;
  }
 else
    {
      strcpy(name,m_directory); strcat(name,"\\LIAISONS.TXT");

    }
  fp=fopen(name,"wt");

  int ixx;
  for (i=1;i<=av->RowCount;i++)  // nb_taches
    {
     if (av->Cells[1][i] != "")
     {
      strcpy(tmp,av->Cells[1][i].c_str());
      ixx = atoi(tmp);
      strcpy(xx,av->Cells[63][i].c_str());  nbsucc=atoi(xx);  // nb successeurs



        isucc = 1;
        while ((su[ixx][isucc]!=0) && (isucc<PS))
        // while ((su[ixx][isucc]!=0) && (su[i][isucc]!=9999))
          {
            if (su[ixx][isucc] != 9999)
             {
               succ=su[ixx][isucc]; dec= su_decal[ixx][isucc];
               tl=su_tl[ixx][isucc];
               if (tl==0) strcpy(xx,"fd");
               else if (tl==1) strcpy(xx,"ff");
               else if (tl==2) strcpy(xx,"am");
               else if (tl==3) strcpy(xx,"dd");

               sprintf(tmp,"\"%d\",\"%d\",\"%d\",\"%s\"\n",i,succ,dec,xx);
               fputs(tmp,fp);
             }
            isucc++;

           }
       //}
      }
    }
 fclose (fp);
 if (question)
  {
   Application->MessageBoxA("Fichiers Taches et Liaisons Sauvegard�s",m_ecoplan,MB_OK);
  }
}


void __fastcall TplStandard::Exec_Rapport(char *plan,bool reload)
{
 char tmp[250]; char tri[200];
 char col[200]; char fil[200];
 AnsiString PLTC;

 Ghost->ExtractValue(tmp,plan,"pl",0);
 Ghost->ExtractValue(tri,plan,"tri",0);
 Ghost->ExtractValue(col,plan,"col",0);
 Ghost->ExtractValue(fil,plan,"filt",0);
 pc=atoi(tmp);
 if (pc<=0 || pc>5) pc=4;
 if (strlen(fil))
   { Ghost->Global_Filtre(fil);  // fabrique une chaine de caracteres
     Exec_Filter(0,m_exchange);
    }
 if (strlen(tri))
     Ghost->Global_Tri(tri);
 if (strlen(col))
     Ghost->Global_Col(col);

 PLTC = AnsiString(P_NOM[pc]) + " / " + AnsiString(tri) + " / " + AnsiString(fil) + " / " + AnsiString(col);
 if (pc==4) LR1FTC->Caption = PLTC;
 //if (pc==5) LR2FTC->Caption = PLTC;
 RAZ();
 Prepare_Draw_Rapport(1);
 if (pc==4) ZoomCopy4(imR1p,0,0);
 // if (pc==5) ZoomCopy5(imR2p,0,0);

}

void __fastcall TplStandard::conc(char *dst,char *src,int typel)
{
 int nt,nbpar,i;
 int ipred, isucc;
 char tmp[200];

 nbpar=Explode('|',src);
 strcpy(dst,"");
 for (i=1;i<nbpar;i++)
   {
    nt=atoi(params[i]);
    if (nt==0) break;
    if (nt==9999) break; // strcat(dst,"9999,1,0,|");
    else
      {
       if (typel==1) // pred
        {
         ipred=1;
         while(pr[nt][ipred] !=0)
         {
          sprintf(tmp,"%d,",nt); strcat(dst,tmp);
          sprintf(tmp,"%d,%d,|",pr_tl[nt][ipred],pr_decal[nt][ipred]);
          strcat(dst,tmp);
          ipred++;
         }
        }
       else // succ
        {
         isucc=1;
         while ((su[nt][isucc] !=0) && (su[nt][isucc] != 9999))
           {
            sprintf(tmp,"%d,",nt); strcat(dst,tmp);
            sprintf(tmp,"%d,%d,|",su_tl[nt][isucc],su_decal[nt][isucc]);
            strcat(dst,tmp);
            isucc++;
           }
        }
      }
   }
}


void __fastcall TplStandard::Exec_Simul()
{
 int i; char per[100]; char tmp[2500]; char dt1[20];
 int lg; char d[20],f[20],t[20],p[20];
 int a1,m1,j1; int tx;   int NTT,DUR,ECART;
 int tniv,inext,nbt;  char temp[50];  int xrow;

 if (nb_taches==0) return;
 for (i=0;i<MAX_PERIOD;i++) { DebPer[i]=""; FinPer[i]=""; DurPer[i]=0; PerPer[i]=0; }

 Ghost->ExtractValue(tmp,m_simul,"mode",0);


 if (strcmp(tmp,"PERIODES")==0 || strcmp(tmp,"TACHES")==0)
   {
    NbPer=0;
    for (i=1;i<MAX_PERIOD;i++)
      {
       sprintf(per,"p%03d",i);
       lg=Ghost->ExtractValue(tmp,m_simul,per,0);
       if (lg==0) break;
       NbPer++;
       lg=Ghost->ExtractValue(d,tmp,"d",0);
       DebPer[NbPer]=AnsiString(d);
       lg=Ghost->ExtractValue(f,tmp,"f",0);
       FinPer[NbPer]=AnsiString(f);
       lg=Ghost->ExtractValue(t,tmp,"t",0);
       DurPer[NbPer]=atoi(t);
       lg=Ghost->ExtractValue(p,tmp,"p",0);
       PerPer[NbPer]=atoi(p);
       // calcul d1 et d2

       //strcpy(dt1,DebPer[NbPer].c_str());
       //dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
       //d1[NbPer] = convert_date(a1+2000,m1,j1);
       d1[NbPer] = Convert_Date(DebPer[NbPer]);

       strcpy(dt1,FinPer[NbPer].c_str());
       dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
       d2[NbPer] = convert_date(a1+2000,m1,j1);
       NbjtPer[NbPer]= nbjt(d1[NbPer],d2[NbPer]);

      }
      if (NbPer==0) return;
      NbjtPer[NbPer+1]=1000; // en cas de depassement par rapport a la fin Prev

      // on classe les taches par
      if (nb_taches == 0) return;

      for (i=0;i<MAX_T;i++) simul[i]=0;
       Organise_Niveaux();
      nbt=0;
      for (tniv=0;tniv<iniv;tniv++)
        {
          inext=0;
          while (niveaux[tniv][inext] != 0)
        {
       tx=niveaux[tniv][inext];
       nbt++;
       simul[nbt]=tx;
        inext++;
       }
      }


     for (inext=1;inext<=nbt;inext++)
      {
       tx=simul[inext];
        Compute_Nlle_DureeA(tx);
       Compute_PlusTotA();
      }



    }
 /*
  else //MODE TACHES
    {

     nbt=0;
     for (i=1;i<MAX_T;i++)
       {
        sprintf(per,"p%03d",i);
        Ghost->ExtractValue(tmp,m_simul,per,0);
        if (strlen(tmp)==0) break;
        nbt++;
        }

     for (i=1;i<=nbt;i++)  // NAX_T
       {
        sprintf(per,"p%03d",i);

        Ghost->ExtractValue(tmp,m_simul,per,0);
        // if (strlen(tmp)==0) break;

        //extraire num de tache et nlle dur�e

       Ghost->ExtractValue(temp,tmp,"nt",0);
       NTT = atoi(temp);
       lg=Ghost->ExtractValue(temp,tmp,"t",0);

       DUR = atoi(temp);
       av->Cells[3][NTT]=AnsiString(DUR);
       // Maj dans AdvString 1
       xrow = ChercheRow(AnsiString(NTT));

       //sprintf(temp,"NT = %d   DUR = %d XROW=%d",NTT,DUR,xrow);
       //Application->MessageBox(temp,"ECOPL",MB_OK);

       if (xrow != 0)  AdvStringGrid1->Cells[3][xrow] = AnsiString(DUR);

       for (inext=1;inext<=nbt;inext++)
         {
        //tx=simul[inext];
        tx=NTT;
        //Compute_Nlle_DureeA(tx);
        // Compute_PlusTotA();
          }

      }
    }
  */


 duree[0]=0;
 Change1();
 refresh_couts();
 grefresh(1,'1');
}

void __fastcall TplStandard::Compute_Nlle_DureeA(int tx)
{

 int  dx1,dx2;  // debut et fin de la tache
 int  per; int base; int ptrdeb;
 float fdur,fjt;  int reste,cum,JT,durper,durjt;
 int ipred,isucc;
 int PerPred,PerSucc;
 bool bPred;

 int num_succ,num_pred;

 for (per=1;per<=NbPer;per++) TDuree[per]=0;
 reste = duree[tx];
 dx1=deb_plus_tot[tx]; dx2=fin_plus_tot[tx];
 ptrdeb=dx1;
 cum=0;
 bPred=false;

 for (per=1;per<=NbPer;per++)
  {
   if (ptrdeb>=d1[per] && ptrdeb<=d2[per])
    {
     JT=nbjt(ptrdeb,d2[per]);

      if (dx1>=d1[per] && dx1<=d2[per]) PerPred=per;

     fjt = JT * 100.0 /((float) PerPer[per]) + 0.5;
     if (fjt < 1.0) fjt=1.0;
     durjt = int (fjt);

     fdur = reste * ((float) PerPer[per])/100.0  + 0.5;
     if (fdur < 1.0) fdur=1.0;
     durper = int (fdur);

     if (durper <= JT) // durjt)
         {
          if (durper<1) durper=1;
          TDuree[per]=durper;
          PerSucc=per;
          if (bPred==false) { PerPred=per; bPred=true; }
          break;
         }
      else
         {

          TDuree[per]= JT;
          reste = reste-durjt;
          ptrdeb = d1[per+1];
         }
    }
  }

 cum=0;

 for (per=1;per<=NbPer;per++) cum+= TDuree[per];

 duree[tx]=cum;
 av->Cells[3][tx]=AnsiString(cum);
 av->Cells[23][tx]=AnsiString(cum);


 isucc=1;
 while (su[tx][isucc] !=0 && su[tx][isucc] != 9999)
  {
   num_succ=su[tx][isucc];
   if (su_decal[tx][isucc] >=0)
     fdur = ((float) su_decal[tx][isucc] * (float) PerPer[PerSucc] / 100.0) + 0.5;
   else
     fdur = ((float) su_decal[tx][isucc] * (float) PerPer[PerSucc] / 100.0) - 0.5;
   su_decal[tx][isucc]=int(fdur);
   // chercher le predecesseur associ� a num_succ
   ipred=1;
   while (pr[num_succ][ipred] !=0)
     {
      num_pred=pr[num_succ][ipred];
      if (num_pred==tx)
        {
         pr_decal[num_succ][ipred]=int (fdur);
         break;
        }
      ipred++;
     }
   isucc++;
  }
}




void __fastcall TplStandard::Compute_Nlle_Duree(int tx)
{

 int  dx1,dx2;  // debut et fin de la tache
 int  per; int base; int ptrdeb;
 float fdur,fjt;  int reste,cum,JT,durper,durjt;
 int ipred,isucc; int PerPred,PerSucc;
 bool bPred;

 for (per=1;per<=NbPer;per++) TDuree[per]=0;
 reste = duree[tx];
 dx1=deb_plus_tot[tx]; dx2=fin_plus_tot[tx];
 ptrdeb=dx1;
 cum=0;  bPred=false;

 for (per=1;per<=NbPer;per++)
  {
   if (ptrdeb>=d1[per] && ptrdeb<=d2[per])
    {
     JT=nbjt(ptrdeb,d2[per]);

     if (dx1>=d1[per] && dx1<=d2[per]) PerPred=per;

     fjt = JT * 100.0 /((float) PerPer[per]) + 0.5;
     if (fjt < 1.0) fjt=1.0;
     durjt = int (fjt);

     fdur = reste * ((float) PerPer[per])/100.0 + 0.5;
     if (fdur < 1.0) fdur=1.0;
     durper = int (fdur);

     if (durper <= JT) // durjt)
         {
          if (durper<1) durper=1;
          TDuree[per]=durper;
          PerSucc=per;
          // if (bPred==false) { PerPred=per; bPred=true; }
          break;
         }
      else
         {

          TDuree[per]= JT;
          reste = reste-durjt;
          ptrdeb = d1[per+1];
         }
    }
  }

 cum=0;

 for (per=1;per<=NbPer;per++) cum+= TDuree[per];

 duree[tx]=cum;

 // bcl sur predecesseurs et successeurs
 ipred=1;
 while (pr[tx][ipred] != 0)
  {
   fdur = ((float) pr_decal[tx][ipred] * (float) PerPer[PerPred] / 100.0)+0.5;
   pr_decal[tx][ipred]=int(fdur);
   ipred++;
  }

 isucc=1;
 while (su[tx][isucc] !=0 && su[tx][isucc] != 9999)
  {
   fdur = ((float) su_decal[tx][isucc] * (float) PerPer[PerSucc] / 100.0)+0.5;
   su_decal[tx][isucc]=int(fdur);
   isucc++;
  }
}


int __fastcall TplStandard::nbjt(int d1,int d2)
{
 int jt,j;
 if (d1>=d2) return 1;

 j=d1; jt=0;
 while (j<=d2)
  {
   if (cal[1][j] == 'T') jt++;
   j++;
  }
 return jt;
}

void __fastcall TplStandard::AdvStringGrid5CanEditCell(TObject *Sender,
      int ARow, int ACol, bool &CanEdit)
{
 int ch;
 CanEdit = false;
 if (ARow==0) return;

 if (ACol==8) { CanEdit=true; return; }

 ch=vl_chp[6][ACol];
 if (ch_sai[ch][0]=='N') return;
 CanEdit = true;
}


///////////////////////////////////////:
void _fastcall  TplStandard::ClickasAvanc()
{
 // Row_Avancement est connu
 int task,ptask; //pptask;
 char tmp[250];  AnsiString Temp;
 char str[250];
 int dur,nb,i,t,nt,count;
 int ipred,isucc;
 int dist,maxdist;
 int a;
 char d1[15]; int a1,j1,m1;

 strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
 task = atoi(tmp);
 sprintf(tmp,"%d",task);
 Label129->Caption = AnsiString(tmp);
 Label156->Caption = AnsiString(tmp);
 // Label133->Caption = AnsiString(tmp);
 Label83->Caption = AnsiString(tmp);
 lTA->Caption = AnsiString(tmp);
  strcpy(adv,asAvanc->Cells[6][row_avancement].c_str()); //getvalue(5,row_avancement,44);           // retard imputable
  AdvStringGrid4->Cells[3][1] = AnsiString(adv); //setvalue(4,1,53,adv);
AdvStringGrid7->Cells[3][1] = AnsiString(adv); //setvalue(4,1,53,adv);
  a = atoi(adv);
    if (a>0) { AdvStringGrid4->Colors[3][1] = TColor(RGB(255,0,0));
               lRetard->Caption = "Retard Imputable " + AnsiString(adv);
			   AdvStringGrid7->Colors[3][1] = TColor(RGB(255,0,0));
			   Label157->Caption = "Retard Imputable " + AnsiString(adv);
             }
     else  { AdvStringGrid4->Colors[3][1] = TColor(RGB(255,255,255));
             lRetard->Caption = "";
			 AdvStringGrid7->Colors[3][1] = TColor(RGB(255,255,255));
             Label157->Caption = "";
  
           }
   // copie dans Avas ainsi que ADV4
   Temp =  asAvanc->Cells[1][row_avancement];
  avas->Cells[1][1] = Temp; //asAvanc->Cells[1][row_avancement];  // N�
  avas->Cells[2][1] = asAvanc->Cells[2][row_avancement];  // design
  avas->Cells[3][1] = asAvanc->Cells[18][row_avancement];  // % restant
  avas->Cells[4][1] = asAvanc->Cells[19][row_avancement];  // Jour Restants
  avas->Cells[5][1] = asAvanc->Cells[4][row_avancement];  // Dur�e r�elle
  avas->Cells[6][1] = asAvanc->Cells[3][row_avancement];  // Deb+Tot Reel
  avas->Cells[7][1] = asAvanc->Cells[5][row_avancement];  // fin + Tot R�el
  avas->Cells[8][1] = asAvanc->Cells[8][row_avancement];  // Deb + Tot Prev
  avas->Cells[9][1] = asAvanc->Cells[10][row_avancement];   // fin + tot prev
  avas->Cells[10][1] = asAvanc->Cells[7][row_avancement];  //  retard Total
  avas->Cells[11][1] = asAvanc->Cells[6][row_avancement];   // Retard Reel

  strcpy(adv,asAvanc->Cells[7][row_avancement].c_str()); //getvalue(5,row_avancement,45);           // retard total
  AdvStringGrid4->Cells[4][1] = AnsiString(adv); //setvalue(4,1,54,adv);
AdvStringGrid7->Cells[4][1] = AnsiString(adv);
  strcpy(adv,asAvanc->Cells[3][row_avancement].c_str()); //getvalue(5,row_avancement,41);           // date debut reelle
  AdvStringGrid4->Cells[8][1] = AnsiString(adv); //setvalue(4,1,56,adv);
AdvStringGrid7->Cells[8][1] = AnsiString(adv);
  if (asAvanc->Cells[4][row_avancement]=="") asAvanc->Cells[4][row_avancement]=asAvanc->Cells[9][row_avancement];
  AdvStringGrid6->Cells[4][row_avancement]=asAvanc->Cells[9][row_avancement];
  strcpy(adv,asAvanc->Cells[4][row_avancement].c_str()); //getvalue(5,row_avancement,42);           // duree reelle
  AdvStringGrid4->Cells[7][1] = AnsiString(adv); //setvalue(4,1,55,adv);
  AdvStringGrid7->Cells[7][1] = AnsiString(adv);

  strcpy(adv,asAvanc->Cells[5][row_avancement].c_str()); //getvalue(5,row_avancement,43);           // date fin reelle
  AdvStringGrid4->Cells[9][1] = AnsiString(adv); //setvalue(4,1,57,adv);
AdvStringGrid7->Cells[9][1] = AnsiString(adv);
  strcpy(adv,asAvanc->Cells[17][row_avancement].c_str()); //getvalue(5,row_avancement,69);           // date deb possible
  AdvStringGrid4->Cells[2][1] = AnsiString(adv); //setvalue(4,1,26,adv);
AdvStringGrid7->Cells[2][1] = AnsiString(adv);
  strcpy(adv,asAvanc->Cells[18][row_avancement].c_str()); //getvalue(5,row_avancement,71);
  AdvStringGrid4->Cells[5][1] = AnsiString(adv); //setvalue(4,1,29,adv);
AdvStringGrid7->Cells[5][1] = AnsiString(adv);
  strcpy(adv,asAvanc->Cells[19][row_avancement].c_str()); //getvalue(5,row_avancement,72);
  AdvStringGrid4->Cells[6][1] = AnsiString(adv); //setvalue(4,1,28,adv);
AdvStringGrid7->Cells[6][1] = AnsiString(adv);
  strcpy(adv,asAvanc->Cells[25][row_avancement].c_str()); //getvalue(5,row_avancement,43);           // date fin reelle
  AdvStringGrid4->Cells[12][1] = AnsiString(adv);
AdvStringGrid7->Cells[12][1] = AnsiString(adv);
 // Date dans label 12
 sprintf(tmp,"%02d/%02d/%02d",JourAvanc,MoisAvanc,AnAvanc%100);
 setvalue(4,1,52,tmp);

 //setvalue(4,1,29,"0");  // pourcentage restant
 //setvalue(4,1,28,"0");  // jours restant
 //setvalue(4,1,30,"0");  // cout
 // cleaner les predecesseurs

 AdvStringGrid5->RowCount=2;
 for(i=0;i<AdvStringGrid5->ColCount;i++)
     { AdvStringGrid5->Cells[i][1]="";
     }

//  AAdvSucc->RowCount=2;
// for(i=0;i<AAdvSucc->ColCount;i++)
//     { AAdvSucc->Cells[i][1]="";
//     }


 ipred=1; count=0;
 MAXDIST=0;

 while(pr[task][ipred] != 0)
   {
    nt=pr[task][ipred];

    for (i=1; i<asAvanc->RowCount; i++)
      {
       strcpy(adv,asAvanc->Cells[1][i].c_str());  //getvalue(5,i,39);
       t=atoi(adv);
       if (t==nt)
         {
          count++; if (count>1) AdvStringGrid5->RowCount++;
          strcpy(adv,asAvanc->Cells[1][i].c_str());  //getvalue(5,i,39);
          AdvStringGrid5->Cells[1][count] = AnsiString(adv); //setvalue(6,count,31,adv);

          strcpy(adv,asAvanc->Cells[2][i].c_str());  //getvalue(5,i,40);
          AdvStringGrid5->Cells[2][count] = AnsiString(adv); //setvalue(6,count,59,adv);
          strcpy(adv,asAvanc->Cells[3][i].c_str());  //getvalue(5,i,41);
          AdvStringGrid5->Cells[6][count] = AnsiString(adv); //setvalue(6,count,35,adv);

          if (asAvanc->Cells[4][i]=="") asAvanc->Cells[4][i]=asAvanc->Cells[9][i];
		  AdvStringGrid6->Cells[4][i]=AdvStringGrid6->Cells[9][i];

          strcpy(adv,asAvanc->Cells[4][i].c_str());  //getvalue(5,i,42);
          AdvStringGrid5->Cells[8][count] = AnsiString(adv); //setvalue(6,count,64,adv);
          strcpy(adv,asAvanc->Cells[5][i].c_str());  //getvalue(5,i,43);
          AdvStringGrid5->Cells[7][count] = AnsiString(adv); //setvalue(6,count,36,adv);
          strcpy(adv,asAvanc->Cells[6][i].c_str());  //getvalue(5,i,44);
          AdvStringGrid5->Cells[4][count] = AnsiString(adv); //setvalue(6,count,33,adv);
          strcpy(adv,asAvanc->Cells[7][i].c_str());  //getvalue(5,i,45);
          AdvStringGrid5->Cells[5][count] = AnsiString(adv); //setvalue(6,count,34,adv);
          strcpy(adv,asAvanc->Cells[17][i].c_str());  //getvalue(5,i,69);
          AdvStringGrid5->Cells[3][count] = AnsiString(adv); //setvalue(6,count,32,adv);
          strcpy(adv,asAvanc->Cells[10][i].c_str());  //getvalue(5,i,48);
          AdvStringGrid5->Cells[14][count] = AnsiString(adv); //setvalue(6,count,65,adv);
          strcpy(adv,asAvanc->Cells[14][i].c_str());  //getvalue(5,i,62);
          AdvStringGrid5->Cells[13][count] = AnsiString(adv); //setvalue(6,count,63,adv);
          strcpy(adv,asAvanc->Cells[18][i].c_str());  //getvalue(5,i,71);
          AdvStringGrid5->Cells[9][count] = AnsiString(adv); //setvalue(6,count,37,adv);
          strcpy(adv,asAvanc->Cells[19][i].c_str());  //getvalue(5,i,72);
          AdvStringGrid5->Cells[10][count] = AnsiString(adv); //setvalue(6,count,38,adv);

          strcpy(adv,asAvanc->Cells[5][i].c_str());  //getvalue(5,i,43);
          strcpy(d1,adv);   // fin reelle
          d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
          dist = convert_date(a1+2000,m1,j1);
          if (dist>MAXDIST) MAXDIST=dist;

          //pptask=exist_tache(nt,nb_taches);
          strcpy(tmp,TYPL[pr_tl[task][ipred]]);   //nt
          AdvStringGrid5->Cells[11][count] = AnsiString(tmp); //setvalue(6,ipred,60,tmp);
          sprintf(tmp,"%d",pr_decal[task][ipred]);    //nt
          AdvStringGrid5->Cells[12][count] = AnsiString(tmp); //setvalue(6,ipred,61,tmp);
         }
      }
    ipred++;
   }
 Compute_DateDebutPossible();
}


void __fastcall TplStandard::asAvancClickCell(TObject *Sender, int ARow,
      int ACol)
{
 int task; char tmp[50];

 if (ARow==0) return;
 row_avancement = ARow;
 ClickasAvanc(); // row_avancement);

 strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
 task = atoi(tmp);
 MEMO_TASK=task;
 DrawMiniGantt(task,0,immGnt,true);
 DrawMiniGantt(task,0,Image1,true);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid5CellValidate(TObject *Sender,
      int ACol, int ARow, AnsiString &Value, bool &Valid)
{
  // retrouver le champ
 int ch; int val,rc;
 char tmp[255]; float cout; int x; int tachepred;
 float fdata; float fdata2; int result,result2;
 char str[50];
 char dt1[15],dt2[15],dt3[15];
 int xcal;


 strcpy(adv,AdvStringGrid5->Cells[13][ARow].c_str()); // Cal getvalue(6,ARow,63);
 xcal=atoi(adv);
 strcpy(adv,AdvStringGrid5->Cells[1][ARow].c_str()); // Num Tache getvalue(6,ARow,31);
 tachepred=atoi(adv);
 av_tache_pred=tachepred;
 strcpy(adv,asAvanc->Cells[1][row_avancement].c_str()); // getvalue(5,row_avancement,39); //tache=atoi(adv);
 ch=vl_chp[6][ACol];
 strcpy(tmp,AdvStringGrid5->Cells[ACol][ARow].c_str());
 row_av_pred =ARow;
 Valid=true;
 switch (ch)
   {
    case 37 : // % restant
       val=atoi(tmp);
       if ((val <0) || (val > 100))
         { Application->MessageBoxA("Pourcentage incorrect",m_ecoplan,MB_OK);
           Valid = false; break;
         }

       // lire dur�e r�elle et calculer nb jours restants
       strcpy(adv,AdvStringGrid5->Cells[8][ARow].c_str()); // 64 ??? getvalue(6,ARow,55);
       result = atoi(adv);
       x =  ((float) val) * (((float) result) / 100.0) +0.5;

       sprintf(str,"%d",x);
       AdvStringGrid5->Cells[10][ARow] = AnsiString(str); //setvalue(6,ARow,38,str);
       break;
    case 38 : // Jours restants
       val=atoi(tmp);
       if ((val <0) || (val > 999))
         { Application->MessageBoxA("Nombre Jours restants Incorrect",m_ecoplan,MB_OK);
           Valid = false; break;
         }
       // compute percentage

       //getvalue(6,ARow,38);
       result = val; //atoi(adv);
       fdata = (float) result;
       strcpy(adv,AdvStringGrid5->Cells[8][ARow].c_str()); // getvalue(6,ARow,64); // duree reelle predecesseur
       result2=atoi(adv);
       fdata2 = (float) result2;
       x= ( fdata *100.0) / (fdata2);
       sprintf(str,"%d",x);
       AdvStringGrid5->Cells[9][ARow] = AnsiString(str); // setvalue(6,ARow,37,str);

       break;

    case 64 :  // duree reelle predecesseur
       val=atoi(tmp);
       if ((val <0) || (val > 999))
         { Application->MessageBoxA("Duree R�elle Incorecte",m_ecoplan,MB_OK);
           Valid = false; break;
         }

       break;

    case 35 :  // date d�but r�el
       rc=verifdate8(tmp);
       if (rc>0) { Valid = false; break;}
       rc=verif_conges(tmp,xcal);
       if (rc>0) { Valid=false;
       Application->MessageBoxA("Date invalide : jour de cong�",m_ecoplan,MB_OK);
                   break;
            }

       break;

    case 36 : // date fin r�lle
       rc=verifdate8(tmp);
       if (rc>0) { Valid = false; break;}
       rc=verif_conges(tmp,xcal);
       if (rc>0) { Valid=false;
       Application->MessageBoxA("Date invalide : jour de cong�",m_ecoplan,MB_OK);
                   break;
            }
       break;

   }
 if (Valid==true)
   {
    Compute_DateDebutPossiblePred(ARow);
    setvalue(6,ARow,ch,tmp);

    CHPRED=ch;
    CH=ch;
       switch (CH)
      {

        case 35:   // Compute fin
          // Compute_Date_Fin();
          Compute_Fin_AvancePred(ARow);
          break;

        case 36 :

         Compute_Date_DebPred(ARow);
         break;

       case 37 :
         //Compute_Jours_RestantsPred(ARow);
         Compute_Fin_AvancePred(ARow);
         Compute_Date_DebPred(ARow);
         break;

       case 38 :
         //Compute_Percent_RestantPred(ARow);
         Compute_Fin_AvancePred(ARow);
         Compute_Date_DebPred(ARow);
         break;
       case 64 :   // duree reelle pred
         Compute_Date_DebPred(ARow);
         break;


      }

    // getvalue(4,1,52); strcpy(dt1,adv);   // data avancement
    strcpy(dt1,DATEAVANC);
    getvalue(6,ARow,35); strcpy(dt2,adv);   // date debut reel
    getvalue(6,ARow,36); strcpy(dt3,adv);   // date fin reelle


    rc=comp_dist_dates(dt1,dt2,0,0);
    if (rc>0)  //  date debut reel apres avancement
      {
       setvalue(6,ARow,37,"100");
       getvalue(6,ARow,64); setvalue(6,ARow,38,adv);
       setvalue(6,ARow,33,"0"); setvalue(6,ARow,34,"0");
      }
    else // date debut avant avancement
      { rc=comp_dist_dates(dt1,dt3,0,0);
        if (rc < 0)  // date de fin avant date avancement : % = 0;
          {
           setvalue(6,ARow,37,"0");
           setvalue(6,ARow,38,"0");

          }
        else
          {
           // getvalue(5,row_avancement,62); xcal=atoi(adv);
           getvalue(6,ARow,63); xcal= atoi(adv); if (xcal < 1) xcal=1;
           rc=comp_dist_dates(dt1,dt3,xcal,0);
           sprintf(adv,"%d",NB2);
           setvalue(6,ARow,38,adv);
           Compute_Percent_RestantPred(ARow);
           Compute_Date_DebPred(ARow);
          }
       // ������������  inverser ou non
       Compute_Retard_TotalPred(ARow);
       Compute_Retard_ImputablePred(ARow);
      }


   Compute_DateDebutPossible();  // de la tache en cours

    CH=57;
    Timer2->Enabled=true;
    // Propage(57);  // propager sur la tache en cours
    Compute_Flag_Alert();
    Compute_Flag_AlertPred(ARow);

   }
 Change1();
}

/////////////////////////////

bool __fastcall TplStandard::Call_Process(int ch, int xcal,char *xtmp)
{
 int val,rc;  bool valid;
 char tmp[255]; float cout; int x;
 float fdata; float fdata2; int result,result2;
 char str[50]; char tmp1[250];
 strcpy(tmp,xtmp);

 char dt1[15],dt2[15],dt3[15];

 

 valid=true;
 switch (ch)
   {
    case 23 : // dur�e r�elle


       break;
    case 29 : // % restant
       val=atoi(tmp);
       if ((val <0) || (val > 100))
         { Application->MessageBoxA("Pourcentage incorrect",m_ecoplan,MB_OK);
           valid = false; break;
         }
       setvalue(4,1,29,tmp);
       getvalue(5,row_avancement,62);  // 14 Calendrier
       xcal=atoi(adv);   if (xcal < 1) xcal=1;
       // comparer date de fin et date avancement

       /*
       getvalue(5,row_avancement,71);    // % restant
       if (strcmp(adv,xtmp)==0)
         { if (strcmp(xtmp,"0")==0) {valid = false;   break; }
         }
       */
       getvalue(4,1,57);   // Col 9 Date Fin Reelle
       if (strlen(adv)==0)
         {
          getvalue(5,row_avancement,43);  // fin reelle
          setvalue(4,1,57,adv);
         }


       strcpy(dt1,adv);   // date fin reelle
       strcpy(dt2,DATEAVANC);
       rc=comp_dist_dates(dt1,dt2,0,0);
       getvalue(4,1,55);  // Col 7 Dur�e r�elle
       if (strlen(adv)==0)
         {

          getvalue(5,row_avancement,42);  // duree reelle
          setvalue(4,1,55,adv);

         }
       result = atoi(adv);
       if (result<0) result=0;
       //result = atoi(valperc);
       x =  ((float) val) * (((float) result) / 100.0) +0.5;
       sprintf(str,"%d",x);
       setvalue(4,1,28,str); // Col 6 Jours restants

       avas->Cells[4][1]= AnsiString(str);
       break;

    case 28 : // Jours restants
       val=atoi(tmp);
       if ((val <0) || (val > 999))
         { Application->MessageBoxA("Nombre Jours restants Incorrect",m_ecoplan,MB_OK);
           valid = false; break;
         }
       result = val; // atoi(valjr);
       fdata = (float) result;
       getvalue(4,1,55);  // Col 7 Duree reelle
       if (strlen(adv)==0)
         {

          getvalue(5,row_avancement,42);  // duree reelle
          setvalue(4,1,55,adv);

         }
       result2=atoi(adv);
       fdata2 = (float) result2;
       if (fdata2==0.0) fdata2=1.0;
       x= ( fdata *100.0) / (fdata2);
       sprintf(str,"%d",x);
       setvalue(4,1,29,str);     // Col 5  % restant
       avas->Cells[3][1]= AnsiString(str);
       break;

    case 55 :  // duree reelle
       val=atoi(tmp);
       if ((val <0) || (val > 999))
         { Application->MessageBoxA("Duree R�elle Incorecte",m_ecoplan,MB_OK);
           valid = false; break;
         }

       break;

    case 56 :  // date d�but r�el
       rc=verifdate8(tmp);
       if (rc>0) { valid = false; break;}
       rc=verif_conges(tmp,xcal);
       if (rc>0) { valid=false;
       Application->MessageBoxA("Date invalide : jour de cong�",m_ecoplan,MB_OK);
                   break;
            }

       break;

    case 57 : // date fin r�lle
       rc=verifdate8(tmp);
       if (rc>0) { valid = false; break;}
       rc=verif_conges(tmp,xcal);
       if (rc>0) { valid=false;
       Application->MessageBoxA("Date invalide : jour de cong�",m_ecoplan,MB_OK);
                   break;
                 }

       break;

    case 30 : // % cout reel
       val=atoi(tmp);
       if ((val <0) || (val > 100))
         { Application->MessageBoxA("Pourcentage Cout R�el Incorrect",m_ecoplan,MB_OK);
           valid = false; break;
         }
       break;

    case 58 : // Cout reel consom�
       cout = atof(tmp);
       if (cout < 0.0)
       { Application->MessageBoxA("Cout R�el Incorrect",m_ecoplan,MB_OK);
           valid = false; break;
         }
       break;
   }

 return valid;
}

void __fastcall TplStandard::AdvStringGrid4CellValidate(TObject *Sender,
      int ACol, int ARow, AnsiString &Value, bool &Valid)
{
 // retrouver le champ
 int ch;
 char tmp[255]; int xcal;
 bool valid;



 ch=vl_chp[4][ACol];

 strcpy(tmp,AdvStringGrid4->Cells[ACol][1].c_str());
 getvalue(5,row_avancement,62); // Calendrier
 xcal=atoi(adv); if (xcal < 1) xcal = 1;
 valid = Call_Process(ch,xcal,tmp);    // tmp est la valeur du champ cliqu�

 if (ACol==12)
   {
    //AdvStringGrid4->Cells[12][ARow]  = "";
      Valid=true;
      asAvanc->Cells[25][row_avancement]=AdvStringGrid4->Cells[12][ARow];

   }


 if (valid==true)
   { Valid=true;
    setvalue(4,1,ch,tmp);
    if (ch==28) setvalue(5,row_avancement,72,tmp);
    if (ch==29) setvalue(5,row_avancement,71,tmp);
    if (ch==57) setvalue(5,row_avancement,43,tmp);
    if (ACol==7) { ch= 23; asAvanc->Cells[4][row_avancement]=AnsiString(tmp);
	AdvStringGrid6->Cells[4][row_avancement]=AnsiString(tmp); }

    CH=ch;
    Timer2->Enabled=true;
    // Propage(ch);
   }
 else Valid=false;

}
//---------------------------------------------------------------------------

int  __fastcall TplStandard::Propage(int ch)
{
  char dt1[15],dt2[15],dt3[15]; int cnt;
  int rc; int xcal; AnsiString Temp;
  int row,maj,nt,i,t; char tmp[50];int task;
  int numt;

  strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
  numt = atoi(tmp);
    //CH=ch;
    //getvalue(4,1,52); strcpy(dt1,adv);   // data avancement
    strcpy(dt1,DATEAVANC);

    getvalue(4,1,56); strcpy(dt2,adv);   // date debut reel
    getvalue(4,1,57); strcpy(dt3,adv);   // date fin reelle



   Sleep(100);
    switch (CH)
      {
       case 23 :  // Dur�e r�elle
         asAvanc->Cells[24][row_avancement]="X";
         av->Cells[59][numt]="X";
         Compute_Date_Deb();
         // Compute_Fin_Avance();
         
         break;

       case 29 :    // pourcen restant
         asAvanc->Cells[24][row_avancement]="X";
         av->Cells[59][numt]="X";
         Compute_Jours_Restants();
         Compute_Fin_Avance();
         Compute_Date_Deb();
         break;

       case 28 :   // jours restants
         asAvanc->Cells[24][row_avancement]="X";
         av->Cells[59][numt]="X";
         Compute_Percent_Restant();
         Compute_Fin_Avance();
         Compute_Date_Deb();
         break;

       case 55: break;

       case 56: break;

       case 57 :   // Fin reelle
         asAvanc->Cells[24][row_avancement]="X";
         av->Cells[59][numt]="X";
         asAvanc->Cells[20][row_avancement]="";
         asAvanc->Cells[21][row_avancement]="";
         av->Cells[65][numt]="";
         av->Cells[66][numt]="";
         Compute_Date_Deb();
         //Compute_Jours_Restants();

         Deduct_JourRestant();
         Compute_Percent_Restant();
         break;

       case 100 :
         Compute_Date_0();
         break;

       case 101 :
         // date de d�but r�el = date Avanc;
         Compute_Date_100();

         break;
      }

    getvalue(4,1,52); strcpy(dt1,adv);   // data avancement
    strcpy(dt1,DATEAVANC);
    getvalue(4,1,56); strcpy(dt2,adv);   // date debut reel
    getvalue(4,1,57); strcpy(dt3,adv);   // date fin reelle
    rc=comp_dist_dates(dt1,dt2,0,0);

    if (rc>0)  //  date debut reel apres avancement
      {
       setvalue(4,1,29,"100");
       getvalue(4,1,55);
       setvalue(4,1,28,adv);
       setvalue(4,1,53,"0");
       setvalue(4,1,54,"0");
      }
    else // date debut avant avancement
      { rc=comp_dist_dates(dt1,dt3,0,0);
        if (rc < 0)  // date de fin avant date avancement : % = 0;
          {
           setvalue(4,1,29,"0");
           setvalue(4,1,28,"0");

          }
        else  /// date de fin apres date avancement
          {
           getvalue(5,row_avancement,62); xcal=atoi(adv);
           if (xcal < 1) xcal=1;
           rc=comp_dist_dates(dt1,dt3,xcal,0);

           sprintf(adv,"%d",NB2+1);
           setvalue(4,1,28,adv);  // remet NBJours a ZERO !!!!!!!!!!!!

           Compute_Percent_Restant();
           Compute_Date_Deb();
          }

      // Compute_Retard_Total();
       //Compute_Retard_Imputable();

      }
    // */
    Compute_Flag_Alert();
    //Compute_Flag_AlertPred();

  Compute_Retard_Total();
  Compute_Retard_Imputable();

   // On recalcule tout l'avancement

   // envoi des saisies dans la grille principale
 getvalue(4,1,53);  setvalue(5,row_avancement,44,adv);  // ret imp
 getvalue(4,1,54);  setvalue(5,row_avancement,45,adv);  // ret total
 getvalue(4,1,55);  setvalue(5,row_avancement,42,adv);  // duree reelle
 getvalue(4,1,56);  setvalue(5,row_avancement,41,adv);  // deb plus tot reel
 getvalue(4,1,57);  setvalue(5,row_avancement,43,adv);  // date fin reelle
 getvalue(4,1,26);  setvalue(5,row_avancement,69,adv);  // date debut reel possible
 strcpy(adv,DATEAVANC);
 //getvalue(4,1,52);
 setvalue(5,row_avancement,70,adv);  //
 getvalue(4,1,29);  setvalue(5,row_avancement,71,adv);  //
 getvalue(4,1,28);  setvalue(5,row_avancement,72,adv);  //



 // maj des pr�decesseurs aussi
 for (row=1;row<AdvStringGrid5->RowCount;row++)
  {
    getvalue(6,row,31); nt=atoi(adv);
    maj=0;
    for (i=1;i<asAvanc->RowCount; i++)
      { getvalue(5,i,39); t=atoi(adv);
        if (t==nt)
          {
           getvalue(6,row,35); setvalue(5,i,41,adv);  // date debut reel
           getvalue(6,row,36); setvalue(5,i,43,adv);
           getvalue(6,row,64); setvalue(5,i,42,adv);
           getvalue(6,row,33); setvalue(5,i,44,adv);
           getvalue(6,row,34); setvalue(5,i,45,adv);
           getvalue(6,row,32); setvalue(5,i,69,adv);
           getvalue(6,row,37); setvalue(5,i,71,adv);
           getvalue(6,row,38); setvalue(5,i,72,adv);
           strcpy(adv,DATEAVANC);
           //getvalue(4,1,52);
           setvalue(5,i,70,adv);  // date avancement
           maj=1;
           break;
          }
      }
   if (maj==0)
     {
       // predecesseur pas present dans la grille

     }
  }

 Faire_Riper();
 Compute_Fin_Reelle();  // Fin r�elle du projet

 // Maj dans onglet Avancement Rapide

 strcpy(adv,avas->Cells[1][1].c_str());  // N� de tache
 nt=atoi(adv);
 if (nt !=0)
  {
   for (i=1;i<asAvanc->RowCount;i++)
   {
    strcpy(adv,asAvanc->Cells[1][i].c_str());
    t= atoi(adv);
        if (t==nt)
          {
           // Transfert des donn�es
           avas->Cells[2][1] = asAvanc->Cells[2][i]; // d�signation
           avas->Cells[3][1] = asAvanc->Cells[18][i]; // %restant
           avas->Cells[4][1] = asAvanc->Cells[19][i]; // Jours restants
           Temp = asAvanc->Cells[4][t]; // Dur�e R�elle
           if (asAvanc->Cells[4][t]=="") asAvanc->Cells[4][t]=asAvanc->Cells[9][t];AdvStringGrid6->Cells[4][t]=asAvanc->Cells[9][t];


           avas->Cells[5][1] = asAvanc->Cells[4][i]; // Dur�e R�elle
           avas->Cells[6][1] = asAvanc->Cells[3][i]; // Deb+Tot R�el
           avas->Cells[7][1] = asAvanc->Cells[5][i]; // Fin + Tot R�elle
           avas->Cells[8][1] = asAvanc->Cells[8][i]; // Deb + Tot Pr�v
           avas->Cells[9][1] = asAvanc->Cells[10][i]; // Fin + tot Pr�v

           break;
          }


   }
  }

  Compute_Retards();

  ClickasAvanc();
  //STRICT=false;
  Refresh_Avanc(concern,false);
  if (ORIGINE_CLICK==2)Refresh_Concern(true);
 
  MAJ_ADV_AV();



   /*
  strcpy(tmp,AdvStringGrid4->Cells[1][1].c_str());
  task = atoi(tmp);
  if (task !=0)
 DrawMiniGantt(task,0,immGnt,true);
  */

  // btDPClick(NULL);
//  Clean_Grid(sg,2);
 for (i=0;i<CntAVBefore;i++)
   Image_Avanc("Avancement",TableDatesAV[i],i);

 Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
 
 Graph_Avanc(AdvStringGrid6->RowCount-1,MAXDIST);
 SAVE_AVANC=false;

 pc=3; Prepare_Draw_Planning();

 Change1();
 asAvanc->SortIndexes->Clear();
 asAvanc->SortIndexes->Add(3);  // tri sur date debut reel
 asAvanc->QSortIndexed();

 
 AdvStringGrid6->SortIndexes->Clear();
 AdvStringGrid6->SortIndexes->Add(3);  // tri sur date debut reel
 AdvStringGrid6->QSortIndexed();
 
 DrawMiniGantt(MEMO_TASK,0,immGnt,true);
 DrawMiniGantt(MEMO_TASK,0,Image1,true);
 cnt=asAvanc->RowCount;
  for (i=1;i<cnt;i++)
    {
     strcpy(tmp,asAvanc->Cells[1][i].c_str());
     task = atoi(tmp);
     if (task==MEMO_TASK) asAvanc->Row=i;

    }

 return 0;

 }


void __fastcall TplStandard::Deduct_JourRestant()
{

 char str[50],tmp[200];
int result,val,x;//,xcal;
 char dt1[15],dt2[15];   int res;

 getvalue(5,row_avancement,62); //xcal=atoi(adv);
 // comparer date de fin et date avancement
 getvalue(4,1,57); strcpy(dt1,adv);   // date fin reelle

 // getvalue(4,1,52); strcpy(dt2,adv);   // date avancement
 strcpy(dt2,DATEAVANC);
 comp_dist_dates(dt2,dt1,1,0);
 // comp_dist_dates(dt1,dt2,1,0);
  if (NB3 > 0) res = NB2;
   else res = - NB2;
  res++;
 sprintf(str,"%d",res);

 setvalue(4,1,28,str);
 asAvanc->Cells[19][row_avancement]=AnsiString(str);

 /*
 getvalue(4,1,55);  // duree reelle



 getvalue(4,1,29); val=atoi(adv);
 getvalue(4,1,55); result = atoi(adv);
 //result = atoi(valperc);
 x =  ((float) val) * (((float) result) / 100.0) +0.5;
 sprintf(str,"%d",x);
 setvalue(4,1,28,str);
       // set value in asAvanc;
 asAvanc->Cells[19][row_avancement]=AnsiString(str);
 */

}

int __fastcall  TplStandard::Compute_Fin_Reelle()
{
 int i,cnt,distmax,dist1; char dt1[20];
 cnt = asAvanc->RowCount;  int j1,m1,a1;

 distmax=0;
 for (i=1;i<cnt;i++)
   {
    strcpy(dt1,asAvanc->Cells[5][i].c_str());
    dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    dist1 = convert_date(a1+2000,m1,j1);
    if (dist1 > distmax)
      {
       Tache_Fin = i;
       strcpy(Date_Fin_Reelle,asAvanc->Cells[5][i].c_str());
       distmax = dist1;
      }
   }
  Label131->Caption = AnsiString(Date_Fin_Reelle);
  DTFINREEL = Label131 ->Caption;
  Label128->Caption = AnsiString(Date_Fin_Reelle);
}


int  __fastcall TplStandard::is_succ(int base,int numt)
{
 int ind,isucc,xtsucc,ok;

 ind = exist_tache(base,nb_taches);
 if (ind==0) return 0;
 isucc = 1;
 ok=0;
 while ((su[ind][isucc]!=0))
   {
    xtsucc=su[ind][isucc];
    if (xtsucc==9999) { ok= 0; break; }
     else
     { if (xtsucc==numt) { ok=1; break;  }}
   isucc++;
   }
 return ok;
}


int __fastcall TplStandard::Faire_Riper()
{
 int tache_en_cours,nt; //nbsel;
 int tniv,inext,ind,i,nbrow,ok;
 int mem_niv; //mem_ind;
 int retard, ret;
 int tache, tsucc,isel,iself;
 char tmp[200];
 // Pour chacune des taches

 getvalue(5,row_avancement,39); tache_en_cours = atoi(adv);
 //getvalue(4,1,54); ret= atoi(adv);
 //getvalue(4,1,53); retard = atoi(adv);
 //if (ret>retard) retard= ret;

 getvalue(5,row_avancement,72); //nb jours restants
 retard=atoi(adv);

 // rechercher la tache dans les niveaux
ok=0;
// for (tniv=0;tniv<iniv;tniv++)
//   {

      tniv=0;
      while(!ok)
       { inext=0;
         while (niveaux[tniv][inext] != 0)
          { ind=niveaux[tniv][inext];
            ind = exist_tache(ind,nb_taches);
            if (ind==tache_en_cours)
              {
               ok = 1;
               mem_niv=tniv; // mem_ind=inext;
               break;
              }
          inext++;
          } //
        tniv++;
        if (tniv>=iniv) { break;} //  return 0;}
       }
 //  }

for (i=0; i<nb_taches;i++) { t_select[i]=0; t_selectf[i]=0; }
//nbsel=1;
t_select[0]=tache_en_cours;

Calcul_Avancement(mem_niv,tache_en_cours,retard);
//Calcul_MR();

Calcul_Margereelle(retard);
return 0;

}

int __fastcall TplStandard::Decale_Tache_Succ(int row,int retard)
{
int dist,x,y,dur;
 int a1,m1,j1,a2,m2,j2;
 char d1[15]; char d2[15];
 char tmp[250];
 int xcal;

 /// === date de debut previ + Retard !!!!!
 getvalue(5,row,62); xcal=atoi(adv);  // calendrier
 if (xcal < 1) xcal=1;

 // calcule la date de debut reel a partir de la date debut previsionel
 getvalue(5,row,41); strcpy(d1,adv);   // debut reelle
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 // avance conges , avance temps
 dist = convert_date(a1+2000,m1,j1);   //
 x=avance_conges(dist,xcal);
 y=avance_temps(x,xcal,retard);
 x=avance_conges(y,xcal);
 add_jour(AnPrev,MoisPrev,JourPrev,x);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(5,row,41,tmp);   //

 getvalue(5,row,41); strcpy(d1,adv);   // debut reelle
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 // on calcule maintenant la date de fin
 getvalue(5,row,42); dur= atoi(adv);
 getvalue(5,row,41); strcpy(d2,adv);   // debut reelle
 d2[2]=0; j2=atoi(d2);  d2[5]=0; m2=atoi(d2+3);  d2[8]=0; a2=atoi(d2+6);
 // avance conges , avance temps
 dist = convert_date(a2+2000,m2,j2);
 x=avance_conges(dist,xcal);
 y=avance_temps(x,xcal,dur);
 x = avance_conges(y,xcal); //�������
 add_jour(AnPrev,MoisPrev,JourPrev,x-1);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(5,row,43,tmp);
 return 0;
}

int __fastcall TplStandard::TrouveRow(int t)
{
 int row,i,nt;
 row=0;
 for (i=1;i<asAvanc->RowCount;i++)
   {getvalue(5,i,39); nt = atoi(adv);
    if (nt==t) row=i;
   }
 return row;
}

int _fastcall  TplStandard::Calcul_Avancement(int niv, int tache,int jours)
{
int i,zcal,tniv,inext,xmax;
int total,flag_amont,ind,imax,xind,ipred,xtpred;
int rowtache,rowpred,dur,x,y;
int frpred,frtache,drpred,drtache;
int a1,j1,m1; char d1[15],dt1[15],dt2[15],tmp[100];
int percent;
int distfpred,distdpred;
int margetot; int res; //
int imin; char retimp[50];

 // if (asAvanc->Cells[24][row_avancement]=="X") return 0;


 for (i=1;i<=nb_taches;i++)
  {
    // duree_reelle[i]=duree[i];
    retard[i]=0;
   }
   // date debut = date previsionnelle
  //A=AnPrev; M=MoisPrev;// J=JourPrev;
  for (tniv=niv+1;tniv<iniv;tniv++)
  {
   inext=0; xmax=-1000;
   while (niveaux[tniv][inext] != 0)
   {
    ind=niveaux[tniv][inext];
    rowtache=TrouveRow(ind);
    // getvalue(5,rowtache,43); strcpy(d1,adv);   //  fin reelle  de la tache en cours

    // Calcul a faire si % restant   $$$$$$$$$$$$$$
    getvalue(5,rowtache,71); strcpy(tmp,adv);
    percent = atoi(tmp);
    //if (percent != 0)
    //   {
    if (asAvanc->Cells[24][rowtache] != "X")
     {

       d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
       // distftache = convert_date(a1+2000,m1,j1);
       ind = exist_tache(ind,nb_taches);
          if (tniv==0)  // niveau zero
            {
              imax=0; imin=0;

            }
          else // if tniv !=0
           {
             // cherchons les predecesseurs de ind
            imax = -1000;ipred = 1; total=0;  flag_amont = 0;
            imin = 10000;
            while (pr[ind][ipred]!=0)
             {xtpred=pr[ind][ipred];
              rowpred=TrouveRow(xtpred);
              xtpred=exist_tache(xtpred,nb_taches);
              strcpy(retimp,asAvanc->Cells[6][rowpred].c_str());
              strcpy(retimp,asAvanc->Cells[7][rowpred].c_str());

              getvalue(5,rowpred,43); strcpy(d1,adv);
              d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
              distfpred = convert_date(a1+2000,m1,j1);
              getvalue(5,rowpred,41); strcpy(d1,adv);
              d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
              distdpred = convert_date(a1+2000,m1,j1);

              if (pr_tl[ind][ipred]==0)     //  c est un lien fd
                 { if ( pr_decal[ind][ipred] > 0)
                      { zcal=tcal[ind];
                        total = avance_conges(distfpred+1,zcal);
                        total = avance_temps(total,zcal,pr_decal[ind][ipred]);
                        total = avance_conges(total,zcal); // �����
                      }
                   else if ( pr_decal[ind][ipred] < 0)
                      {  zcal=tcal[xtpred];
                         total = avance_conges(distfpred+1,zcal);
                         total = recule_temps(total,zcal,-pr_decal[ind][ipred]);
                      }
                   else { zcal=tcal[ind];
                          total = avance_conges(distfpred+1,zcal);
                        } // pas de calcul de decalage
                  }  // if lien fd
               if (pr_tl[ind][ipred]==1)     // c est un lien ff
                  {
                   if (pr_decal[ind][ipred] > 0)
                       { zcal=tcal[xtpred];
                         total = avance_conges(distfpred+1,zcal);
                         total = avance_temps(total,zcal,pr_decal[ind][ipred]);
                         total = avance_conges(total,zcal); //�������
                         total = recule_temps(total,zcal,duree[ind]);
                        }
                    else if (pr_decal[ind][ipred] < 0)
                       { zcal=tcal[xtpred];
                         total=recule_conges(distfpred+1,zcal);
                         total=recule_temps(total,zcal,-pr_decal[ind][ipred]);
                         total = recule_temps(total,zcal,duree[ind]);
                        }
                    else
                       {
                        zcal= tcal[ind];
                        total=recule_conges(distfpred+1,zcal);
                        total = recule_temps(total,zcal,duree[ind]);
                       }
                  }  // if lien FF

               if (pr_tl[ind][ipred] == 2)  flag_amont = ipred;
               if (pr_tl[ind][ipred] == 3)    // Lien DD   Decal est >= 0
                  {
                   if ( pr_decal[ind][ipred] > 0)
                      { zcal=tcal[xtpred];
                        total=avance_conges(distdpred,zcal);
                        total = avance_temps(total,zcal,pr_decal[ind][ipred]);
                        total = avance_conges(total,zcal); //�������
                       }
                   else if ( pr_decal[ind][ipred] == 0)
                        { zcal=tcal[ind];
                          total=avance_conges(distdpred,zcal);
                        } // pas de calcul de decalage
                  }  // if lien DD

               if (total > imax) imax=total;
               if (total < imin) imin=total;
               ipred++;
               }   // while ipred
           zcal=tcal[ind];

           add_jour(AnPrev,MoisPrev,JourPrev,imax);
           sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
           setvalue(5,rowtache,41,tmp);  // debut reel
           setvalue(5,rowtache,69,tmp);  // debut reel possible

           add_jour(AnPrev,MoisPrev,JourPrev,imin);
           sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);


           getvalue(5,rowtache,42); dur= atoi(adv);

           x=avance_temps(imax,zcal,dur-1);
           y= avance_conges(x,zcal);
           add_jour(AnPrev,MoisPrev,JourPrev,y);
           // result dans An,Mois,Jour
           sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
           setvalue(5,rowtache,43,tmp);

           // calcul du retard total = difference fin prev - fin reelle
           getvalue(5,rowtache,43); strcpy(dt2,adv);   // date fin r�el
           getvalue(5,rowtache,73); //margetot=atoi(adv);  // Marge Reelle
           getvalue(5,rowtache,48); strcpy(dt1,adv);   // date fin previsionnel

           comp_dist_dates(dt1,dt2,zcal,0);
           if (NB3>0)
           res = NB2;
           else
           res =-NB2;

           sprintf(tmp,"%d",res);
           setvalue(5,rowtache,45,tmp);  // Retard Total

           // calculer jours restants si date debut reel ant�rieur a date d'avancement

           getvalue(5,rowtache,41); strcpy(dt2,adv);  // debut r�el
           comp_dist_dates(DATEAVANC,dt2,zcal,0);
           if (NB3>0) res = NB2; else res =-NB2;
           // $$$$$$$$$$$$$$$$$
           if (res <=0)
             {
               Compute_Jours_RestantsX(rowtache);
               Compute_Percent_RestantX(rowtache);
             }
           // */
         } // end else !=0

      } //  if (!= "X")
      inext++;
      } // end while
   } // end for
 return 0;
}

int __fastcall  TplStandard::comp_dist_succ_avanc(int indx,int xtsucc, int isucc,int deb,int fin)
{
 int zcal,xtotal,flag_amont,fduree;
 if (su_tl[indx][isucc]==0)     //  c est un lien fd
                  {
                   if (su_decal[indx][isucc] > 0)
                     { zcal=tcal[indx];  // xtsucc];
                       xtotal=recule_conges(deb,zcal);
                       xtotal=recule_temps(xtotal,zcal,su_decal[indx][isucc]);
                       xtotal=recule_temps(xtotal,zcal,duree[indx]);
                     }
                   else if (su_decal[indx][isucc] < 0)
                     {
                      zcal=tcal[indx]; //xtsucc];
                      xtotal=avance_conges(deb,zcal);
                      xtotal=avance_temps(xtotal,zcal,-su_decal[indx][isucc]);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);
                     }
                   else
                     {zcal=tcal[indx];
                      xtotal=recule_conges(deb,zcal);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);
                     }
                  }
                if (su_tl[indx][isucc]==1)     //  c est un lien ff
                  {
                   if (su_decal[indx][isucc] > 0)
                     {zcal=tcal[indx]; //
                      xtotal=recule_conges(fin+1,zcal);
                      xtotal=recule_temps(xtotal,zcal,su_decal[indx][isucc]);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);

                     }
                   else if (su_decal[indx][isucc] < 0)
                     {zcal=tcal[indx];  //
                      xtotal=avance_conges(fin+1,zcal);     //��� + 1
                      xtotal=avance_temps(xtotal,zcal,-su_decal[indx][isucc]);
                      xtotal = avance_conges(xtotal,zcal); //�������
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);

                     }
                   else
                     {zcal=tcal[indx];
                      xtotal=recule_conges(fin+1,zcal);
                      xtotal=recule_temps(xtotal,zcal,duree[indx]);
                     }
                  }
                 if (su_tl[indx][isucc]==2)     //  c est un lien amont
                  {
                   flag_amont = isucc;
                  }

                 if (su_tl[indx][isucc]==3)     //  c est un lien DD
                  {
                   if (su_decal[indx][isucc] > 0)
                     { zcal=tcal[indx];  // xtsucc];

                       xtotal = comp_down(deb,zcal,su_decal[indx][isucc]);
                       //zcal=tcal[indx];
                       //xtotal = comp_down(fduree,zcal,duree[indx]);    // $$$$$$$$$$$$
                     }
                   else if (su_decal[indx][isucc] == 0)
                     {zcal=tcal[indx];
                      //xtotal = comp_down(deb,zcal,duree[indx]);
                      xtotal=deb;
                     }
                  }

 return xtotal;

}



int __fastcall TplStandard::Calcul_MR()
{
 int ix,t,indx,zcal,xtsucc;
 int isucc,imin,rowsucc,rowmin;
 char dat1[15],dat2[15],tmp[255];

 for (ix=1;ix<asAvanc->RowCount;ix++)
   {
    getvalue(5,ix,39); t=atoi(adv); // numero de taches
    indx = exist_tache(t,nb_taches);
    getvalue(5,ix,43); strcpy(dat1,adv);  // date fin reelle
    zcal = tcal[indx];
    isucc = 1; imin=10000;
    while ((su[indx][isucc]!=0)) //
      { xtsucc=su[indx][isucc];
        if (xtsucc == 9999)
         imin=0;
        else
        {
         rowsucc=TrouveRow(xtsucc);
         getvalue(5,rowsucc,41); strcpy(dat2,adv); // date debut
         comp_dist_dates(dat1, dat2,zcal,0);
         if (NB2 < imin) {imin = NB2; rowmin=rowsucc; }
        }
       isucc++;
      }

    getvalue(5,rowmin,41); strcpy(dat2,adv); // date debut
    comp_dist_dates(dat1, dat2,zcal,0);
    sprintf(tmp,"indx %d, succmin %d, NB2= %d",indx,rowmin,NB2);
    Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
   }
return 0;
}




int __fastcall TplStandard::Calcul_Margereelle(int retard)
{
 int ix,tniv,inext,indx,isucc,xtsucc;
 int imin; //,xtotal;
 int mmin; int i,zx; int fdecal, fduree;
 char tmp[250];
 int idd, ddflag,flag_amont,t_amont,t_start,imom;
 int indmom; int zcal; int a1,m1,j1;
 int DT; int dtsucc,ftsucc,dtx;   // ftx;
 int dist,t,rowx,rowsucc,indsucc;
 char d1[15]; int marge,percent;
 int diff,indexpred;
 char dt1[15],dt2[15];
 char dat1[15],dat2[15];
 char DAT1[15],DAT2[15],DATT[15];
char dtfictif[15],dtPrev[15];
 int a1fic,m1fic,j1fic;
 int margesucc; //,margeindx;
 int indfict,distmin;
 int minisucc,minxtsucc;
 int flag_ok,dtdecal;
 int distavanc;

 flag_amont=0;
 if (nb_taches==0) return 0;
 sommets[nb_taches+1]=9999;   // juste pour calcul au plus tard
 imin=10000;


 distavanc = convert_date(AnAvanc,MoisAvanc,JourAvanc);
 DistAvanc = distavanc;

  DT=0;    tniv=iniv-1; inext=0;
  while(niveaux[tniv][inext] !=0)
 {
  t=niveaux[tniv][inext]; rowx=TrouveRow(t);
  getvalue(5,rowx,43); strcpy(d1,adv);  // date fin reelle
  d1[2]=0; j1fic=atoi(d1);  d1[5]=0; m1fic=atoi(d1+3);  d1[8]=0; a1fic=atoi(d1+6);
  dist = convert_date(a1fic+2000,m1fic,j1fic);
  if (dist > DT) { DT=dist; indfict=rowx; }
  inext++;
 }


 //  DT=DT+1; // distance de la tache fictive 9999

 getvalue(5,indfict,43); strcpy(dtfictif,adv);


  margesucc=DT;
  for (tniv=iniv-1;tniv>=0;tniv--)
   {
    inext=0;
    while (niveaux[tniv][inext] != 0)
      {
       indx=niveaux[tniv][inext]; t=indx;
       // on extrait les valeurs liees a INDX
       rowx=TrouveRow(t);
       getvalue(5,rowx,43); strcpy(dt1,adv);  // date fin reelle
       strcpy(dat1,dt1);
       dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
       //ftx =
       convert_date(a1+2000,m1,j1);
       getvalue(5,rowx,73);
       //margeindx= atoi(adv);

       getvalue(5,rowx,41); strcpy(d1,adv);  // date deb reelle
       strcpy(DAT1,adv);
       d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
       dtx = convert_date(a1+2000,m1,j1);

       indmom = niveaux[tniv][inext];
       indx = exist_tache(indx,nb_taches);  // ������������
       imin=10000;
       zcal=tcal[indx];
        // cherchons les successeurs de ind
       isucc = 1; //xtotal=0;
       while ((su[indx][isucc]!=0)) // && (su[indx][isucc]!=9999))
          {
            xtsucc=su[indx][isucc];
            flag_ok=false;
            if (xtsucc==9999)
             { if (isucc == 1)  // pas de vrai successeurs
                  { comp_dist_dates(dat1,dtfictif,zcal,0);
                    strcpy(DAT2,dtfictif);
                    marge = NB2;
                    //marge=margesucc-ftx;
                    imin=marge;
                    flag_ok=true;
                  }
                else  // il existe un successeur
                  {  imin=imin; }
              isucc++;
              }
            else
              {
                flag_ok=false;
                xtsucc = exist_tache(xtsucc,nb_taches);
                rowsucc = TrouveRow(xtsucc);
                getvalue(5,rowsucc,41); strcpy(d1,adv);  // date deb reelle
                strcpy(dat2,adv);
                d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
                dtsucc = convert_date(a1+2000,m1,j1);

                getvalue(5,rowsucc,43); strcpy(d1,adv);  // fin reelle
                d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
                ftsucc = convert_date(a1+2000,m1,j1);



                getvalue(5,rowsucc,73); margesucc=atoi(adv);
                // dtdecal = avance_temps(dtsucc,zcal,margesucc);

                dtdecal = comp_dist_succ_avanc(indx,xtsucc,isucc,dtsucc,ftsucc);
                dtdecal = avance_temps(dtdecal,zcal,margesucc);

                if (dtdecal < imin)
                  {imin = dtdecal; minxtsucc = xtsucc;
                   minisucc = isucc;
                   strcpy(DAT2,dat2);
                  }

                isucc++;
              }
          }  // while su

        if (imin==10000)  imin=0;
        getvalue(5,rowx,71); percent=atoi(adv);
        if (percent >= 0)
         {
          if (flag_ok== false)
           {
             xtsucc = exist_tache(minxtsucc,nb_taches);
             rowsucc = TrouveRow(xtsucc);

             getvalue(5,rowsucc,43); strcpy(d1,adv);  // date deb reelle
             strcpy(dat2,adv);
             d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
             ftsucc = convert_date(a1+2000,m1,j1);

             strcpy(d1,DAT2); //date debut reel succ);
             d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
             dtsucc = convert_date(a1+2000,m1,j1);
             getvalue(5,rowsucc,73); margesucc=atoi(adv);

             dtdecal = comp_dist_succ_avanc(indx,xtsucc,minisucc,dtsucc,ftsucc);
             dtdecal = avance_temps(dtdecal,zcal,margesucc);
             add_jour(AnPrev,MoisPrev,JourPrev,dtdecal);
             sprintf(DATT,"%02d-%02d-%02d",Jour,Mois,An%100);



             // dtdecal = avance_temps(dtsucc,zcal,margesucc);
             comp_dist_dates(DAT1,DATT,zcal,0);


             marge = NB2;    // - margeindx;
             marge=marge;
           }
 //         if (ftx <  distavanc)
 //           {
 //
 //            setvalue(5,rowx,71,"0");
 //            setvalue(5,rowx,72,"0");
 //           }
 //         else
 //          {
             sprintf(tmp,"%d",marge);
             setvalue(5,rowx,73,tmp);

            if (dtx > distavanc)
            {
             setvalue(5,rowx,71,"100");
             getvalue(5,rowx,42); int dur=atoi(adv);
             sprintf(tmp,"%d",dur);
             setvalue(5,rowx,72,tmp);
            }
   //        }
         }
        else setvalue(5,rowx,73,"0");

        inext++;
       } // while

    } // end for


  sommets[nb_taches+1]=0;
  return 0;
 }


int __fastcall TplStandard::verif_conges(char *dt, int xcal)
{
 char dt1[15]; int dist1;
 int a1,m1,j1;
 strcpy(dt1,dt);

 dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);

 dist1=convert_date(a1+2000,m1,j1);

 if (cal[xcal][dist1]=='N') return 1;
 else return 0;

}

int __fastcall TplStandard::Compute_Percent_Restant()
{
 char str[50],tmp[200];
 int result,val,x,result2;
 float fdata,fdata2;



       getvalue(4,1,28);   // nbjrestants
       result = atoi(adv);
       JOURS_RESTANTS = result;
       //result = atoi(valjr);
       fdata = (float) result;
       getvalue(4,1,55);
       result2=atoi(adv);
       fdata2 = (float) result2;
       if (fdata2==0.0) fdata2=1.0;
       x= ( fdata *100.0) / (fdata2);
       sprintf(str,"%d",x);
       setvalue(4,1,29,str);
       PERC_RESTANT=x;
       asAvanc->Cells[18][row_avancement]=AnsiString(str);
       AdvStringGrid6->Cells[18][row_avancement]=AnsiString(str);
 return 0;
}


int __fastcall TplStandard::Compute_Jours_Restants()
{
char str[50],tmp[200];
int result,val,x;//,xcal;
 char dt1[15],dt2[15];

       getvalue(5,row_avancement,62); //xcal=atoi(adv);
       // comparer date de fin et date avancement
       getvalue(4,1,57); strcpy(dt1,adv);   // date fin reelle

       // getvalue(4,1,52); strcpy(dt2,adv);   // date avancement
       strcpy(dt2,DATEAVANC);
       comp_dist_dates(dt1,dt2,1,0);
       getvalue(4,1,29); val=atoi(adv);    // % RESTANT
       PERC_RESTANT=float(val);
       getvalue(4,1,55); result = atoi(adv);  // DUREE REELLE
       //result = atoi(valperc);
       if (result<0) result=0;
       x =  ((float) val) * (((float) result) / 100.0) +0.5;
       sprintf(str,"%d",x);
       setvalue(4,1,28,str);
       // set value in asAvanc;
       JOURS_RESTANTS = atoi(str);
       asAvanc->Cells[19][row_avancement]=AnsiString(str);
       AdvStringGrid6->Cells[19][row_avancement]=AnsiString(str);
	   return 0;
}


int __fastcall TplStandard::Compute_Percent_RestantX(int row)
{
 char str[50],tmp[200];
 int result,val,x,result2;
 float fdata,fdata2;



       getvalue(5,row,72);   // nbjrestants
       result = atoi(adv);
       if (result<0) result=0;
       //result = atoi(valjr);
       fdata = (float) result;
       getvalue(5,row,42);  // duree reelle
       result2=atoi(adv);
       fdata2 = (float) result2;
       if (fdata2==0.0) fdata2=1.0;
       x= ( fdata *100.0) / (fdata2);
       sprintf(str,"%d",x);
       //  setvalue(4,1,29,str);
       asAvanc->Cells[18][row]=AnsiString(str);
	   AdvStringGrid6->Cells[19][row_avancement]=AnsiString(str);
       
 return 0;
}


int __fastcall TplStandard::Compute_Jours_RestantsX(int row)
{
char str[50],tmp[200];
int result,val,x; int xcal;
 char dt1[15],dt2[15];   int res;

       getvalue(5,row,62); xcal=atoi(adv);   if (xcal <=0) xcal=1;
       // comparer date de fin et date avancement
       getvalue(5,row,43); strcpy(dt1,adv);   // date fin reelle


       strcpy(dt2,DATEAVANC);
       comp_dist_dates(dt2,dt1,1,0);
       // res=abs(NB2);

       if (NB3 > 0) res = NB2;
            else res = -NB2;
       res++;
       if (res <=0)
       res=0;


       /*
       getvalue(4,1,29); val=atoi(adv);    //  % jour restants
       getvalue(4,1,55); result = atoi(adv);  Dur�e reelle
       */
       // x =  ((float) val) * (((float) result) / 100.0) +0.5;
        sprintf(str,"%d",res);
       // setvalue(4,1,28,str);
       // set value in asAvanc;
       asAvanc->Cells[19][row]=AnsiString(str);
	  AdvStringGrid6->Cells[19][row_avancement]=AnsiString(str);
      
       return 0;
}




int __fastcall TplStandard::Compute_Duree_Reelle()
{
 int xcal; //int nbrestants,rc;
 char tmp[150],dt1[20], dt2[20];

 getvalue(5,row_avancement,62); xcal=atoi(adv); if (xcal <1) xcal=1; // calendrier
 getvalue(4,1,28); // nbrestants = atoi(adv);     // nbjourrestant
 // lire date de debut reelle
 getvalue(4,1,57); strcpy(dt2,adv);   // fin reelle
 getvalue(4,1,56); strcpy(dt1,adv);   // debut reelle
 comp_dist_dates(dt1,dt2,xcal,0);
 // NB1= distance y compris conges NB2= nombre de jours travailles
 sprintf(tmp,"%d",NB2+1);
 setvalue(4,1,55,tmp);
return 0;
}

int __fastcall TplStandard::Compute_Fin_Avance()
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int distavanc,x,y,xcal,nbrest,nb ,distfinreelle;
 char datfinprev[20]; char datfinreelle[20];


 getvalue(5,row_avancement,62); xcal=atoi(adv);  // calendrier
  if (xcal < 1) xcal=1;
  /*
 //getvalue(5,row_avancement,43);  // date fin r�elle
 //strcpy(d1,adv);
 //strcpy(d1,DATEAVANC);
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 distfinreelle = convert_date(a1+2000,m1,j1);
 getvalue(4,1,28); nbrest= atoi(adv);
 x=avance_conges(distfinreelle+1,xcal);
 y=avance_temps(x,xcal,nbrest);
 add_jour(AnPrev,MoisPrev,JourPrev,y-1);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);
 return 0;
 */


 if (PERC_RESTANT==0) return 0;  // pas de modifs
 // if (PERC_RESTANT == 100) { return 0; }


 getvalue(5,row_avancement,48);  // date fin Pr�visonnelle
 strcpy(datfinprev,adv);

 getvalue(4,1,28); nbrest= atoi(adv);

 strcpy(adv,DATEAVANC);
 strcpy(d1,adv);   // avancement
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 // avance conges , avance temps
 distavanc = convert_date(a1+2000,m1,j1);   // calcule N par rapport au deb previsionnel

 // calculer dist reel


 /*
 getvalue(4,1,57); // date fin reelle
 strcpy(d1,adv);
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 distfinreelle = convert_date(a1+2000,m1,j1);
 if (distfinreelle < distavanc)
   {
    if (nbrest==0) return 0;
   }

 //x=avance_conges(distavanc,xcal);  /// +1,xcal);
 x=avance_conges(distfinreelle,xcal);
 y=avance_temps(x,xcal,nbrest);
 add_jour(AnPrev,MoisPrev,JourPrev,y-1);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);
 return 0;
*/
 x=avance_conges(distavanc,xcal);
 y=avance_temps(x,xcal,nbrest);
 add_jour(AnPrev,MoisPrev,JourPrev,y-1);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);
 return 0;

}

int __fastcall TplStandard::Compute_Date_Fin()
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int dist_tot,x,y,xcal,dur;


 getvalue(5,row_avancement,62); xcal=atoi(adv);  // calendrier
 if (xcal < 1) xcal = 1;
 getvalue(4,1,55); dur= atoi(adv);
 getvalue(4,1,56); strcpy(d1,adv);   // debut reel
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 // avance conges , avance temps
 dist_tot = convert_date(a1+2000,m1,j1);   // calcule N par rapport au deb previsionnel
 x=avance_conges(dist_tot,xcal);
 y=avance_temps(x,xcal,dur);
 add_jour(AnPrev,MoisPrev,JourPrev,y-1);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);
 return 0;
}


int __fastcall TplStandard::Compute_Fin_AvancePred(int row)
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int dist_tot,x,y,xcal,nbrest;


 strcpy(adv,AdvStringGrid5->Cells[13][row].c_str());  // getvalue(6,row,63);
 xcal=atoi(adv);  // calendrier
 strcpy(adv,AdvStringGrid5->Cells[10][row].c_str());  //getvalue(6,row,38);
 nbrest= atoi(adv);
 strcpy(adv,DATEAVANC);

 strcpy(d1,adv);   // avancement
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 // avance conges , avance temps
 dist_tot = convert_date(a1+2000,m1,j1);   // calcule N par rapport au deb previsionnel
 x=avance_conges(dist_tot+1,xcal);
 y=avance_temps(x,xcal,nbrest);
 add_jour(AnPrev,MoisPrev,JourPrev,y-1);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 AdvStringGrid5->Cells[7][row] = AnsiString(tmp); // setvalue(6,row,36,tmp);
 return 0;
}



int  __fastcall TplStandard::Compute_Deb_From_Fin(char *dt)
{
 int xcal,x,y,dur; char d1[15];
 int a1,m1,j1,dist;  char tmp[100];

strcpy(adv,asAvanc->Cells[14][row_avancement].c_str()); //getvalue(5,row_avancement,62);
xcal=atoi(adv);  // calendrier
if (xcal < 1) xcal = 1;
strcpy(adv,AdvStringGrid4->Cells[7][1].c_str());//  !!!getvalue(4,1,55);
dur= atoi(adv);      // duree reelle
//getvalue(4,1,57);
strcpy(d1,dt);      // date fin reelle
d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
dist = convert_date(a1+2000,m1,j1);   //
x=recule_conges(dist+1,xcal);
y=recule_temps(x,xcal,dur);
add_jour(AnPrev,MoisPrev,JourPrev,y);
 // result dans An,Mois,Jour
sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
AdvStringGrid4->Cells[8][1]= AnsiString(tmp); //(//setvalue(4,1,56,tmp);  // deb tot reel
AdvStringGrid7->Cells[8][1]= AnsiString(tmp);
return 0;
}

int __fastcall TplStandard::Compute_Date_Deb()
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int dist_avanc,x,y,xcal,dur; //nbjrest; //,nbjfait;
 getvalue(5,row_avancement,62);
 xcal=atoi(adv); if (xcal < 1) xcal = 1; // calendrier
 strcpy(adv,AdvStringGrid4->Cells[7][1].c_str()); //getvalue(4,1,55);
 dur= atoi(adv);      // duree reelle

 strcpy(adv,AdvStringGrid4->Cells[9][1].c_str()); //getvalue(4,1,57);
 strcpy(d1,adv);    // date fin reel
 getvalue(4,1,28); //nbjrest=atoi(adv);
 // nbjfait=dur-nbjrest;

 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 dist_avanc = convert_date(a1+2000,m1,j1);   // calcule N par
 x=recule_conges(dist_avanc,xcal);
 y=recule_temps(x,xcal,dur-1); // nbjfait);
 if (y<0) y=0;
 add_jour(AnPrev,MoisPrev,JourPrev,y);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,56,tmp);  // deb tot reel
 // mais calculer date de fin reelle


 /* $$$$$$$$$
 add_jour(AnPrev,MoisPrev,JourPrev,y+dur+1);
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);
*/ 

 return 0;
 }

 int __fastcall TplStandard::Compute_Date_0()
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int dist_avanc,x,y,xcal,dur;



 getvalue(5,row_avancement,62);
 xcal=atoi(adv); if (xcal < 1) xcal = 1; // calendrier
 strcpy(adv,AdvStringGrid4->Cells[7][1].c_str()); //getvalue(4,1,55);
 dur= atoi(adv);      // duree reelle

 // enlever 1 jour a date Avanc
 strcpy(tmp,DATEAVANC); tmp[2]='/'; tmp[5]='/';
  strcpy(d1,tmp);
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 sub_jour(a1,m1,j1,1);
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);
 strcpy(d1,tmp);
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 dist_avanc = convert_date(a1+2000,m1,j1);
 dist_avanc++;
 x=recule_conges(dist_avanc,xcal);
 y=recule_temps(x,xcal,dur);
 add_jour(AnPrev,MoisPrev,JourPrev,y);
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,56,tmp);  // deb tot reel
 /*
 // mais calculer date de fin reelle
 add_jour(AnPrev,MoisPrev,JourPrev,y+dur+1);
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);
 */
  return 0;
 }

 int __fastcall TplStandard::Compute_Date_100()
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int dist_avanc,x,y,xcal,dur;



 getvalue(5,row_avancement,62);
 xcal=atoi(adv); if (xcal < 1) xcal = 1; // calendrier
 strcpy(adv,AdvStringGrid4->Cells[7][1].c_str()); //getvalue(4,1,55);
 dur= atoi(adv);      // duree reelle

 strcpy(tmp,DATEAVANC); tmp[2]='/'; tmp[5]='/';
 setvalue(4,1,56,tmp);
 strcpy(d1,DATEAVANC);
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 dist_avanc = convert_date(a1+2000,m1,j1);
 x=avance_conges(dist_avanc,xcal);
 y=avance_temps(x,xcal,dur);
 add_jour(AnPrev,MoisPrev,JourPrev,y);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(4,1,57,tmp);  // fin tot reel
  return 0;
 }




int __fastcall TplStandard::Compute_Retard_Total()
{
 char dt1[15]; char dt2[15];
 int xcal; char tmp[150];
 int margetot;  int res;
 int numtach;


 strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
 numtach=atoi(tmp);

 strcpy(adv,asAvanc->Cells[14][row_avancement].c_str()); //getvalue(5,row_avancement,62);
 xcal=atoi(adv); if (xcal < 1) xcal = 1;

 strcpy(adv,AdvStringGrid4->Cells[9][1].c_str()); //getvalue(4,1,57);
 strcpy(dt2,adv);   // date fin r�el

 strcpy(adv,asAvanc->Cells[16][row_avancement].c_str()); //getvalue(5,row_avancement,66);
 margetot=atoi(adv);
 strcpy(adv,asAvanc->Cells[10][row_avancement].c_str()); //getvalue(5,row_avancement,48);
 strcpy(dt1,adv);   // date fin previsionnel
 comp_dist_dates(dt1,dt2,xcal,0);

  //if (NB1<0)
 //  NB2=-NB2;
 if (NB3 >0)  res = NB2;
 else
    res = -NB2;

 sprintf(tmp,"%d",res);
 setvalue(4,1,54,tmp);

 av->Cells[25][numtach]=AnsiString(tmp);

 return 0;
}

/*
int __fastcall TplStandard::Compute_Retard_TotalX()
{
 char dt1[15]; char dt2[15];
 int xcal; char tmp[150];
 int margetot;  int res;

 strcpy(adv,asAvanc->Cells[14][row_avancement].c_str()); //getvalue(5,row_avancement,62);
 xcal=atoi(adv); if (xcal < 1) xcal = 1;
 strcpy(adv,AdvStringGrid5->Cells[5][row_avancement].c_str()); //getvalue(4,1,57);
 strcpy(dt2,adv);   // date fin r�elle
 strcpy(adv,asAvanc->Cells[16][row_avancement].c_str());
 margetot=atoi(adv);
 strcpy(adv,asAvanc->Cells[10][row_avancement].c_str());
 strcpy(dt1,adv);   // date fin previsionnel
 comp_dist_dates(dt1,dt2,xcal,0);


 if (NB3 >0)  res = NB2;
 else
    res = -NB2;

 sprintf(tmp,"%d",res);
 //setvalue(4,1,54,tmp);
 asAvanc->Cells[7][row_avancement]=AnsiString(tmp);
 AdvStringGrid6->Cells[7][row_avancement]=AnsiString(tmp);
 return 0;
}
*/

void __fastcall TplStandard::Compute_Retards()
{

int i; int cnt;
cnt = asAvanc->RowCount;

for (i=1;i<cnt;i++)
  {
   row_avancement = i;
   ClickasAvanc(); //
   Compute_Retard_Total();
   Compute_Retard_Imputable();
  }
 if (cnt>1) row_avancement = 1;
return;
}

/*
// row should be row_avancement
strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
numtach=atoi(tmp);

distfinR = Convert_Date(asAvanc->Cells[5][row]);   // date fin Reeelle
distdebRPossible = Convert_Date(asAvanc->Cells[17][row_avancement]);    // Debut Reel possible
strcpy(tmp,asAvanc->Cells[9][row].c_str());    durp=atoi(tmp);

res = distfinR - durp - distdebRPossible;
// Convert_Res_To_Date(res);
sprintf(tmp,"%d",res);
asAvanc->Cells[6][row]=AnsiString(tmp);    //Ret imputable
AdvStringGrid6->Cells[6][row]=AnsiString(tmp);
av->Cells[24][numtach]=AnsiString(tmp);
//

// retard total = fin reelle - fin previsionnelle
 strcpy(adv,asAvanc->Cells[14][row_avancement].c_str()); //getvalue(5,row_avancement,62);
 xcal=atoi(adv); if (xcal < 1) xcal = 1;

 strcpy(tmp,asAvanc->Cells[5][row].c_str());
 strcpy(dt2,tmp);   // date fin r�el

 strcpy(tmp,asAvanc->Cells[10][row].c_str()); //fin prev;
 strcpy(dt1,tmp);   // date fin previsionnel
 comp_dist_dates(dt1,dt2,xcal,0);

 // if (NB3 >0)  res = NB2;  else res = -NB2;
 res = NB2;

 sprintf(tmp,"%d",res);
 asAvanc->Cells[7][row]=AnsiString(tmp);    //Ret TOTAL
 av->Cells[25][numtach]=AnsiString(tmp);
 AdvStringGrid6->Cells[7][row]=AnsiString(tmp);    //Ret TOTAL

}

*/


int __fastcall TplStandard::Compute_Retard_Imputable()
{
 char dt1[15]; char dt2[15];
 int xcal; char tmp[150];
 int j1,m1,a1,j2,m2,a2;
 int dist,durp,x,y;
 int decal;
 int distfinR,distdebRPossible,distdebR;
 int dur_tache;
 int rettotalpred,rettotal,i;
 int totalsucc,totalimp;  int res;
 int numtach;

 //

 Compute_DateDebutPossible();

 strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
 numtach=atoi(tmp);

 getvalue(5,row_avancement,62); xcal=atoi(adv);  if (xcal <=0) xcal=1;
 getvalue(5,row_avancement,47); durp=atoi(adv);


 getvalue(5,row_avancement,48); strcpy(dt1,adv);   // date fin previsionnel
 getvalue(4,1,57); strcpy(dt2,adv);   // date fin reel
 getvalue(4,1,56);
 strcpy(dt1,adv);   // debut reel
 getvalue(4,1,26);
 strcpy(dt2,adv);   // debut reel possible


 // FORMULE : DATE DEB REEL POSSIBLE - DATE DEBUT REEL DE LA TACHE

 /*
 distdebRPossible = Convert_Date(asAvanc->Cells[17][row_avancement]);
 distdebR         = Convert_Date(asAvanc->Cells[3][row_avancement]);
 res = distdebRPossible - distdebR +1;
 Convert_Res_To_Date(res);
 comp_dist_dates(dt2,dt1,xcal,0);
 res = NB2 + 1;
 sprintf(tmp,"%d",res);
 setvalue(4,1,53,tmp);
 setvalue(5,row_avancement,44,tmp);
 */

 /*
 //  Formule  (FIN REELLE - DUREE PREV) - DEBUT REEL POSSIBLE
 // strcpy(adv,AdvStringGrid4->Cells[2][1].c_str());
 strcpy(adv,asAvanc->Cells[17][row_avancement].c_str());
 strcpy(dt2,adv);   // debut reel possible

 strcpy(adv,AdvStringGrid4->Cells[9][1].c_str());
 strcpy(dt1,adv);  // fin reelle
 comp_dist_dates(dt2,dt1,xcal,0);
 //if (NB3 >0)  res = NB2;
 //else
 res = NB2 + 1;
 res=res - durp;
 */


 /*
 distfinR=Convert_Date(asAvanc->Cells[5][row_avancement]);     // Fin Re��lle
 distdebRPossible = Convert_Date(asAvanc->Cells[17][row_avancement]);    // Debut Reel possible



 res = distfinR - durp - distdebRPossible+1;
 Convert_Res_To_Date(res);
 */

 /* /////////////
 sprintf(tmp,"%d",res);
 setvalue(4,1,53,tmp);
 setvalue(5,row_avancement,44,tmp);
 */ ///////////////

 // av->Cells[24][numtach]=AnsiString(tmp);


 // retard IMPUTABLE
 // DATE DE DEBUT REEL - DATE DEBUT REEL POSSIBLE + (DUREE REELLE - DUREE PREVISIONNELLE)
 /*
 comp_dist_dates(dt2,dt1,xcal,0);
 if (NB3 >0)  res = NB2;
 else
    res = -NB2;
 Convert_Res_To_Date(res);
 sprintf(tmp,"%d",res);
 setvalue(4,1,53,tmp);
 setvalue(5,row_avancement,44,tmp);
 */

  // DATE DEBUT REEL POSSIBLE - (FIN REELLE  - DUREE PREVISIONNELLE)
  // DATE DEBUT REEL POSSIBLE - FIN REELLE  + DUREE PREVISIONNELLE
  /*
  getvalue(4,1,26); strcpy(dt1,adv);  // debut reel possible
  getvalue(4,1,57); strcpy(dt2,adv); // dt2=fin reelle
  getvalue(5,row_avancement,47); durp=atoi(adv);  // duree previsionnelle
  comp_dist_dates(dt1,dt2,xcal,0);
  if (NB3 >0)  res = NB2;
 else  res = -NB2;
 Convert_Res_To_Date(res);
 sprintf(tmp,"%d",res);
 setvalue(4,1,53,tmp);
 setvalue(5,row_avancement,44,tmp);
  */

/*

 // retard IMPUTABLE = date fin reelle - Date fin previsionnelle
 // =============================================================


 getvalue(5,row_avancement,48); strcpy(dt1,adv);   // date fin previsionnel
 //dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt2+6);

 getvalue(4,1,57); strcpy(dt2,adv);   // date fin reel
 // dt2[2]=0; j2=atoi(dt2);  dt2[5]=0; m2=atoi(dt2+3);  dt2[8]=0; a2=atoi(dt2+6);

*/
 /*
 comp_dist_dates(dt1,dt2,xcal,0);
 if (NB3 >0)  res = NB2;
 else
    res = -NB2;
 Convert_Res_To_Date(res);
 sprintf(tmp,"%d",res);
 setvalue(4,1,53,tmp);
 setvalue(5,row_avancement,44,tmp);
*/

/*

 // ********************
 getvalue(4,1,26);
 //getvalue(5,row_avancement,69);
 strcpy(dt1,adv);   // debut reel possible
 getvalue(4,1,56);
 //getvalue(5,row_avancement,41);
 strcpy(dt2,adv);   // debut reel

 comp_dist_dates(dt1,dt2,xcal,0);
 if (NB3 >0)  res = NB2;
 else
    res = -NB2;
 Convert_Res_To_Date(res);
 sprintf(tmp,"%d",res);
 setvalue(4,1,53,tmp);
 setvalue(5,row_avancement,44,tmp);
 // *************
*/

 //  ret imputable = Retard total du succ - retard total pred (le plus contraignant)

 /*
 getvalue(4,1,57); strcpy(dt2,adv);   // date fin reel
 dt2[2]=0; j2=atoi(dt2);  dt2[5]=0; m2=atoi(dt2+3);  dt2[8]=0; a2=atoi(dt2+6);

 dist = convert_date(a2+2000,m2,j2);   //
 x=recule_conges(dist+1,xcal);
 y=recule_temps(x,xcal,durp);

 add_jour(AnPrev,MoisPrev,JourPrev,y);
 sprintf(dt2,"%02d/%02d/%02d",Jour,Mois,An%100);

 // boucle pour trouver le plus contraignant possibles

 */

 /*
 rettotal=0;


 for (i=1;i<AdvStringGrid5->RowCount;i++)
   {
    getvalue(6,i,34);  rettotalpred= atoi(adv);
    if (rettotalpred>rettotal) rettotal=rettotalpred;
   }

  getvalue(4,1,54);
  totalsucc = atoi(adv);
  totalimp = totalsucc -  rettotal;
  sprintf(tmp,"%d",totalimp);
  setvalue(4,1,53,tmp);
  setvalue(5,row_avancement,44,tmp);

  // getvalue(4,1,26); strcpy(dt1,adv);   // debut reel possible
 // getvalue(5,row_avancement,46); strcpy(dt1,adv); // date debut previsionnel //possible

 //getvalue(6,IND_PRED,36); strcpy(dt1,adv);  // date de fin du predecesseur contraignant

*/

 ////////////////////   OK  OK  OK OK

 getvalue(4,1,26); strcpy(dt1,adv);   // debut reel possible
 getvalue(4,1,56); strcpy(dt2,adv);   // debut reel
 strcpy(adv,asAvanc->Cells[14][row_avancement].c_str()); //getvalue(5,row_avancement,62);
 xcal=atoi(adv); if (xcal < 1) xcal = 1;
 // comp_dist_dates(dt1,dt2,xcal,0);

 res = comp_dist_dates(dt2,dt1,xcal,0);
 // sprintf(tmp,"Compute retard Imputable Num Tache = %d, NB1= %d,NB2= %d, NB3= %d",numtach,NB1,NB2,NB3);
 // Application->MessageBox(tmp,"Ecoplanning",MB_OK);

 // res = NB2; //  + 1;
 // sprintf(tmp,"%d",res);
 // setvalue(4,1,53,tmp);
 // setvalue(5,row_avancement,44,tmp);


 // NB2= retard imputable
 // getvalue(4,1,55); dur_tache=atoi(adv);

 // NB2=NB2-DECAL-impute;


 if (NB3 >0)  res = -NB2; else res=NB2;

 sprintf(tmp,"%d",res);
 setvalue(4,1,53,tmp);
 setvalue(5,row_avancement,44,tmp);

 return 0;
}


/*
int __fastcall TplStandard::Compute_Retard_ImputableX()
{
 char dt1[15]; char dt2[15];
 int xcal; char tmp[150];
 int j1,m1,a1,j2,m2,a2;
 int dist,durp,x,y;
 int decal;
 int dur_tache;
 int rettotalpred,rettotal,i;
 int totalsucc,totalimp;  int res;

 Compute_DateDebutPossibleX();


 getvalue(5,row_avancement,62); xcal=atoi(adv);  if (xcal <=0) xcal=1;
 getvalue(5,row_avancement,47); durp=atoi(adv);

 getvalue(5,row_avancement,48); strcpy(dt1,adv);   // date fin previsionnel
 getvalue(5,row_avancement,43); strcpy(dt2,adv);   // date fin reel

 // ********************
 getvalue(5,row_avancement,69);
 strcpy(dt1,adv);   // debut reel possible
 getvalue(5,row_avancement,41);
 strcpy(dt2,adv);   // debut reel

 comp_dist_dates(dt1,dt2,xcal,0);
 if (NB3 >0)  res = NB2;
 else
    res = -NB2;
 Convert_Res_To_Date(res);
 sprintf(tmp,"%d",res);

 setvalue(5,row_avancement,44,tmp);
 return 0;
}
*/




int __fastcall TplStandard::Compute_DateDebutPossible()
{

 int i,decal; int typl,fd,rc;
 int calp,calt, fin_plus_tot;
 int durtache;
 int max_contrainte; int mem_ind;

 char tmp[255]; //int imin=0;
 char dt1[15],dt2[15];
 char type_lien[20];

 // calcul date debut reelle possible


  // imin=1000;
  max_contrainte = 0;   DECAL=0; NUM_TACHE_PRED=0; IND_PRED=0;
  for (i=1;i<AdvStringGrid5->RowCount;i++)
     {
       getvalue(6,i,31);
       if (strcmp(adv,"")==0)
       {
        // getvalue(4,1,56);  // lire date debut reel
        // setvalue(4,1,26,adv); // l'ecrire dans date deb possible
         break;
       }
       getvalue(6,i,61);  // decalage
       decal = atoi(adv);
       getvalue(6,i,63);  // calendrier predecesseur
       calp= atoi(adv);
       strcpy(adv,asAvanc->Cells[14][row_avancement].c_str()); //getvalue(5,row_avancement,62);
       calt= atoi(adv);  if (calt < 1) calt=1;
       getvalue(6,i,36);
       strcat(adv,"01/01/00");
       elargir_annee(adv);
       date10[2]=0; JourPred=atoi(date10);
       date10[5]=0; MoisPred=atoi(date10+3);
       date10[10]=0; AnPred=atoi(date10+6);
       fin_plus_tot = convert_date(AnPred,MoisPred,JourPred);
       getvalue(4,1,55);
       durtache = atoi(adv);

       getvalue(6,i,60); strcpy(type_lien,adv);

       fd = Compute_Decalages(fin_plus_tot,type_lien,calt,calp,decal,durtache);
       if (fd > max_contrainte)
               {max_contrainte = fd ; row_contrainte=i; mem_ind = i;
               IND_PRED=i; //imin=fd;
               getvalue(6,i,31); NUM_TACHE_PRED=atoi(adv);
               getvalue(6,mem_ind,60);  DECAL = atoi(adv);
              }

      }

   if (max_contrainte !=0)
    {
     add_jour(AnPrev,MoisPrev,JourPrev,max_contrainte);
     sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);

   }
  else
   {
    sprintf(tmp,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev%100);
   }
 setvalue(4,1,26,tmp);
 Compute_Flag_Alert();
 return 0;
}

int  __fastcall TplStandard::Compute_Decalages(int base,char *typl,int caltache,int calpred,int decal,int dur)
{
 int total;
 total=0;

 if (strcmp(typl,"FD")==0) // FD
                 {
                   if ( decal > 0)
                      {
                        total = avance_conges(base+1,caltache);
                        total = avance_temps(total,caltache,decal);
                        total = avance_conges(total,caltache);
                      }
                   else if ( decal < 0)
                      {  total = avance_conges(base+1,calpred);
                         total = recule_temps(total,calpred,-decal);
                      }
                   else {
                          total = avance_conges(base+1,caltache);
                        } // pas de calcul de decalage
                  }
               if (strcmp(typl,"FF")==0)    // c est un lien ff
                  {
                   if (decal > 0)
                       {
                         total = avance_conges(base+1,calpred);
                         total = avance_temps(total,calpred,decal);
                         total = avance_conges(total,calpred);
                         total = recule_temps(total,calpred,dur);
                        }
                    else if (decal < 0)
                       {
                         total=recule_conges(base+1,calpred);
                         total=recule_temps(total,calpred,-decal);
                         total = recule_temps(total,calpred,dur);
                        }
                    else
                       {

                        total=recule_conges(base+1,caltache);
                        total = recule_temps(total,caltache,dur);
                       }
                  }

               if (strcmp(typl,"AM")==0) {} // flag_amont = ipred;
               if (strcmp(typl,"DD")==0)   // Lien DD   Decal est >= 0
                  {
                   if ( decal > 0)
                      {
                        total=avance_conges(base,calpred);
                        total = avance_temps(total,calpred,decal);
                        total= avance_conges(total,calpred);
                       }
                   else if ( decal == 0)
                        {
                         total=avance_conges(base,caltache);
                        } // pas de calcul de decalage
                  }
 return total;

}

int __fastcall TplStandard::Compute_Flag_Alert()

{

int rc;

    strcpy(adv,AdvStringGrid4->Cells[3][1].c_str()); // getvalue(4,1,53);
    rc=atoi(adv);


    if (rc>0) { AdvStringGrid4->Colors[3][1] = TColor(RGB(255,0,0));
	AdvStringGrid7->Colors[3][1] = TColor(RGB(255,0,0));
               lRetard->Caption = "Retard Imputable " + AnsiString(adv);
               Label157->Caption = "Retard Imputable " + AnsiString(adv);
             }
     else  { AdvStringGrid4->Colors[3][1] = TColor(RGB(255,255,255));
	 AdvStringGrid7->Colors[3][1] = TColor(RGB(255,255,255));
             lRetard->Caption = "";
             Label157->Caption = "";
           }



 return 0;
}

int __fastcall TplStandard::Compute_Flag_AlertPred(int row)

{
char dt1[15],dt2[15];
int rc;

 strcpy(adv,AdvStringGrid5->Cells[4][row].c_str()); // getvalue(6,row,33);
 rc=atoi(adv);
    if (rc!=0) { flag_alertPred = true; }

 return 0;
}


int __fastcall  TplStandard::elargir_annee(char *dt8)
{
 strcpy(date10,dt8);
 date10[9]=dt8[7]; date10[8]=dt8[6];
 date10[6]='2';  date10[7]='0';   date10[10]=0;
 return 0;
}

int  __fastcall TplStandard::comp_dist_dates(char *dt1, char *dt2, int calx,int offset)   // date8
{
 Word a1,a2,m1,m2,j1,j2;
 char d1[15],d2[15];
 int dist1,dist2,i;
 int annee20; int borne1; int borne2;
 bool neg;


 if (strlen(dt1)==10)
   {
    dt1[6]=dt1[8];
    dt1[7]=dt1[9];
    dt1[10]=0;
    dt1[8]=0;
   }
 if (strlen(dt2)==10)
   {
    dt2[6]=dt2[8];
    dt2[7]=dt2[9];
    dt2[10]=0;
    dt2[8]=0;
   }
 if (strlen(dt1)==0) sprintf(dt1,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
 if (strlen(dt2)==0) sprintf(dt2,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);

 if (strlen(dt1)!=8) sprintf(dt1,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
 if (strlen(dt2)!=8) sprintf(dt2,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);

 if (strlen(dt1) != 8)
   {
    Application->MessageBoxA("Format Date Incorrect",m_ecoplan,MB_OK);
    return 0;
   }
 if (strlen(dt2) != 8)
   {
    Application->MessageBoxA("Format Date Incorrect",m_ecoplan,MB_OK);
    return 0;
   }
 strcpy(d1,dt1); strcpy(d2,dt2);
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 d2[2]=0; j2=atoi(d2);  d2[5]=0; m2=atoi(d2+3);  d2[8]=0; a2=atoi(d2+6);

 if (cal !=0)  // on elargit les dates, et on calcule
   {
    annee20=2000+a1; dist1=convert_date(annee20,m1,j1);
    annee20=2000+a2; dist2=convert_date(annee20,m2,j2);
    NB1 = dist2-dist1+offset;
    NB3=NB1;
     if (NB1 < 0)  { borne1=dist2; borne2=dist1; neg=true;}
     else {borne1=dist1; borne2=dist2; neg=false;}
    // calcul de NB2 en fonction du calendrier
    NB2=0;
    for (i=borne1; i < borne2 ; i++)
      {
       if (cal[calx][i] == 'T') NB2++;
      }
   }
 if (neg) NB4=NB2; else NB4=-NB2;

 if (a2>a1) return 1;
 if (a2<a1) return -1;

 if (m2>m1) return 1;
 if (m2<m1) return -1;

 if (j2>j1) return 1;
 if (j2<j1) return -1;
 return 0;
}



int __fastcall TplStandard::Compute_Percent_RestantPred(int row)
{
 char str[50],tmp[200];
 int result,val,x,result2;
 float fdata,fdata2;

       getvalue(6,row,38);  // Jours restants
       result = atoi(adv);

       // result = atoi(valjr);
       fdata = (float) result;
       getvalue(6,row,64);  // Duree
       result2=atoi(adv);
       fdata2 = (float) result2;
       if (fdata2 != 0.0) x= ( fdata *100.0) / (fdata2);
       else x=0.0;
       sprintf(str,"%d",x);
       setvalue(6,row,37,str);
 return 0;
}
int __fastcall TplStandard::Compute_Jours_RestantsPred(int row)
{
char str[50],tmp[200];
 int result,val,x;
       getvalue(6,row,37); val=atoi(adv);
       getvalue(6,row,64); result = atoi(adv);
       if (result<0) result=0;
       //result = atoi(valperc);
       x =  ((float) val) * (((float) result) / 100.0) +0.5;
       sprintf(str,"%d",x); setvalue(6,row,38,str);
  return 0;
}

int __fastcall TplStandard::Compute_Duree_ReellePred(int row)
{ // uniquement si des date de debut ou de fin on ete changees
 int xcal; //int nbrestants,rc;
 char tmp[150],dt1[20], dt2[20];

 getvalue(6,row,63); xcal=atoi(adv);  // calendrier
 getvalue(6,row,38); // nbrestants = atoi(adv);     // nbjourrestant
 // lire date de debut reelle
 getvalue(6,row,36); strcpy(dt2,adv);   // fin reelle
 getvalue(6,row,35); strcpy(dt1,adv);   // debut reelle
 comp_dist_dates(dt1,dt2,xcal,0);
 // NB1= distance y compris conges NB2= nombre de jours travailles
 sprintf(tmp,"%d",NB2+1);
 setvalue(6,row,64,tmp);
 return 0;
}

int __fastcall TplStandard::Compute_Date_FinPred(int row)
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int dist_tot,x,y,xcal,dur;


 getvalue(6,row,63); xcal=atoi(adv);  // calendrier
 getvalue(6,row,64); dur= atoi(adv);    // duree
 getvalue(6,row,35); strcpy(d1,adv);   // debut reelle
 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 // avance conges , avance temps
 dist_tot = convert_date(a1+2000,m1,j1);   // calcule N par rapport au deb previsionnel
 x=avance_conges(dist_tot,xcal);
 y=avance_temps(x,xcal,dur);
 add_jour(AnPrev,MoisPrev,JourPrev,y-1);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(6,row,36,tmp);
 return 0;
}

int  __fastcall TplStandard::Compute_Deb_From_FinPred(int row)
{
 int xcal,x,y,dur;
 char d1[15]; int dist,a1,j1,m1;
 char tmp[35];

getvalue(6,row,63); xcal=atoi(adv);  // calendrier
getvalue(6,row,64); dur= atoi(adv);      // duree reelle
getvalue(6,row,36); strcpy(d1,adv);      // date fin reelle
d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
dist = convert_date(a1+2000,m1,j1);   //
x=recule_conges(dist+1,xcal);
y=recule_temps(x,xcal,dur);
add_jour(AnPrev,MoisPrev,JourPrev,y);
 // result dans An,Mois,Jour
sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
setvalue(6,row,35,tmp);  // deb tot reel
return 0;
}

int __fastcall TplStandard::Compute_Date_DebPred(int row)
{
 Word j1,m1,a1;
 char d1[15]; char tmp[150];
 int dist_avanc,x,y,xcal,dur,nbjrest; //,nbjfait;


 getvalue(6,row,63); xcal=atoi(adv);  // calendrier
 getvalue(6,row,64); dur= atoi(adv);
 // getvalue(4,1,52);  strcpy(d1,adv);    // date avancement

 getvalue(6,row,36); strcpy(d1,adv);  // date fin reelle

 getvalue(6,row,38); nbjrest=atoi(adv);
 // nbjfait=dur-nbjrest;

 d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
 dist_avanc = convert_date(a1+2000,m1,j1);   // calcule N par rapport au deb previsionnel
 x=recule_conges(dist_avanc+1,xcal);
 y=recule_temps(x,xcal,dur); // nbjfait);
 add_jour(AnPrev,MoisPrev,JourPrev,y);
 // result dans An,Mois,Jour
 sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
 setvalue(6,row,35,tmp);  // deb tot reel

// x=avance_conges(dist_avanc,xcal);
// y=avance_temps(x,xcal,nbjrest);
// add_jour(AnPrev,MoisPrev,JourPrev,y);
 // result dans An,Mois,Jour
// sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
// setvalue(6,row,36,tmp);  // fin temps reel
return 0;
}

int __fastcall TplStandard::Compute_Retard_TotalPred(int row)
{
 char dt1[15]; char dt2[15];
 int xcal; char tmp[150];
 int margetot;  int i,t,nt;
  int retimputsucc; int numtach;

  strcpy(tmp,AdvStringGrid5->Cells[1][row].c_str());
 numtach=atoi(tmp);

 getvalue(6,row,63); xcal=atoi(adv);
 //getvalue(6,row,35); strcpy(dt2,adv);   // date debut reel

 getvalue(6,row,36); strcpy(dt2,adv);
 getvalue(6,row,65); strcpy(dt1,adv);   // fin previsionnel

 getvalue(6,row,31); nt=atoi(adv);
 for (i=1;i<asAvanc->RowCount; i++)
   { getvalue(5,i,39); t=atoi(adv);
     if (t==nt)
      {
        getvalue(5,i,66); margetot=atoi(adv);
        getvalue(5,i,44); retimputsucc = atoi(adv);
        break;
      }
   }

 comp_dist_dates(dt1,dt2,xcal,0);
 // NB2= retard total
 // retirer la marge totale
 // NB2=NB2-margetot;

  // NB2 = NB2 - retimputsucc;

 sprintf(tmp,"%d",NB2);
 setvalue(6,row,34,tmp);
 av->Cells[25][numtach]=AnsiString(tmp);
 return 0;
}

int __fastcall TplStandard::Compute_Retard_ImputablePred(int row)
{
 char dt1[15]; char dt2[15];
 int xcal; char tmp[150];
 int i,nt,t; int durp;
 int j1,j2,m1,m2,a1,a2;
 int dist,x,y;  int retimputsucc;
 int numtach;


 strcpy(tmp,AdvStringGrid5->Cells[1][row].c_str());
 numtach=atoi(tmp);

 getvalue(6,row,63); xcal=atoi(adv);
 getvalue(6,row,36); strcpy(dt2,adv);   // date fin r�el
 // get value duree reel
 // boucle dans asAvanc pour trouver la duree previsionnelle
 // numero de taches dans

 getvalue(6,row,31); nt=atoi(adv);
 for (i=1;i<asAvanc->RowCount; i++)
   { getvalue(5,i,39); t=atoi(adv);
     if (t==nt)
      {getvalue(5,i,47); durp=atoi(adv);
       getvalue(5,i,44); retimputsucc = atoi(adv);
        break; }
   }


 //  ret imputable = fin reelle tache - duree previsonelle - date debut reelle
 //  +/- charge

 dt2[2]=0; j2=atoi(dt2);  dt2[5]=0; m2=atoi(dt2+3);  dt2[8]=0; a2=atoi(dt2+6);

 dist = convert_date(a2+2000,m2,j2);   //
 x=recule_conges(dist+1,xcal);
 y=recule_temps(x,xcal,durp);
 add_jour(AnPrev,MoisPrev,JourPrev,y);
 sprintf(dt2,"%02d/%02d/%02d",Jour,Mois,An%100);

 // getvalue(6,row,35);   strcpy(dt1,adv);      // date debut reelle

 //  getvalue(5,i,44); retimputsucc = atoi(adv);
 getvalue(6,row,32);   strcpy(dt1,adv);
 comp_dist_dates(dt1,dt2,xcal,0);

 // NB2= retard imputable
// NB2 = NB2 - DECALPRED;



 NB2 = NB2 - retimputsucc; // - NB2;
 sprintf(tmp,"%d",NB2);
 setvalue(6,row,33,tmp);

 av->Cells[24][numtach]=AnsiString(tmp);

 return 0;
}


int __fastcall TplStandard::Compute_DateDebutPossiblePred(int row)
{
 int i,decal; int typl,fd,rc;
 int calp,calt, f_plus_tot;
 int durtache;
 int max_contrainte;
 //int row_contrainte;
 char tmp[255];
 char dt1[15],dt2[15];
 char type_lien[20];

 int ch; int ind,pt,ipred;
 int mem_ind,mem_ipred;

 // calcul date debut reelle possible

  max_contrainte = 0;
  DECALPRED=0; CALP=0;

  // boucle sur les predecesseurs des predecesseurs !!!!
  getvalue(6,row,31);  ch=atoi(adv); if (ch==0) return 0;
  ind = ch;   ///exist_tache(ch,nb_taches);
  ipred = 1;
  while(pr[ind][ipred]!=0)
    {
     pt=pr[ind][ipred]; /// pt=exist_tache(pt,nb_taches);
     strcpy(type_lien,TYPL[pr_tl[ind][ipred]]);
     decal = pr_decal[ind][ipred];
     calt = tcal[ind];
     calp = tcal[pt];
     f_plus_tot = fin_plus_tot[pt];
     getvalue(6,row,64); durtache=atoi(adv);
     fd = Compute_Decalages(f_plus_tot,type_lien,calt,calp,decal,durtache);
     if ((fd )>max_contrainte)
              {max_contrainte = fd ; row_contrainte=i; mem_ind = pt;
               mem_ipred = ipred;
              }
     ipred++;
    }
  if (max_contrainte !=0)
    {
     add_jour(AnPrev,MoisPrev,JourPrev,max_contrainte);
     // on a la date dans An,Mois,Jour;
     sprintf(tmp,"%02d/%02d/%02d",Jour,Mois,An%100);
     setvalue(6,row,59,tmp);
     DECALPRED=pr_decal[mem_ind][mem_ipred];
   }
 return 0;
}


//---------------------------------------------------------------------------

void __fastcall TplStandard::asAvancContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
int X,Y;
 X = MousePos.x;
 Y = MousePos.y;
  asAvanc->MouseToCell(X, Y, CurCol, CurRow);
  AdvStringGrid6->MouseToCell(X, Y, CurCol, CurRow);
 // conversion en ligne et colonne courantes
 PopupActiv->Popup(X, Y);
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::SaveAs(bool reload)
{
 char name[256];  char tmp[250];   AnsiString Dir1,Dir2;
 char name1[256];char name2[256];
 int mb; char fn[255]; char exname[MAX_PATH];
 char *p; int ret;   char pname[MAX_PATH];
 AnsiString FName;  char OldProject[MAX_PATH];


    strcpy(exname,LocalBase);
    strcpy(OldProject,ProjectName);
    SaveDialog1->InitialDir = AnsiString(ProjectDirectory);
    if (strlen(SAVEAS)==0) SaveDialog1->FileName = "*.eco";
    else SaveDialog1->FileName = AnsiString(SAVEAS) + "*.eco";
    if (SaveDialog1->Execute())
          {
            strcpy(name,SaveDialog1->FileName.c_str());
            // ret = plST->SaveAs(name);
            strlwr(name);
            p=strstr(name,".eco");
            if (p) *p=0;
            if (FileExists(SaveDialog1->FileName))
              {
               mb = Application->MessageBoxA
               ("Le Projet existe d�j� : Voulez vous le remplacer ?",m_ecoplan,MB_YESNOCANCEL);
                 if (mb==IDCANCEL) return;
                 if (mb==IDNO) return;
                 if (mb==IDYES)
                   {
                    strcpy(fn,name); strcat(fn,".eco"); unlink(fn);
                    strcpy(fn,name); strcat(fn,".dat"); unlink(fn);
                   }
              }

            strcpy(name1,LocalBase); strcat(name1,"_crit.txt");
            strcpy(name2,name); strcat(name2,"_crit.txt");
            Copy_File(name2,name1);

            strupr(name);
            strcpy(LocalBase,name);
            strcpy(DatabaseName,name);
            strcpy(LastAppli,name);

            FName = ExtractFileName(AnsiString(DatabaseName));
            strcpy(pname,FName.c_str());
            strcpy(ProjectName,pname);

            if (VERSION) Caption= "Ecoplanning 5.1 - " + AnsiString(name);
            else Caption= "Ecoplanning 5.0 - " + AnsiString(name);
            // mettre a jour Caption of main form

            strcpy(tmp,"<caption>"); strcat(tmp,name); strcat(tmp,"</caption>");
            strcpy(m_status,tmp);

            p=strstr(name,"\\");
            if (p) p=strstr(p+1,"\\");
            if (p)
              {
                strcpy(dbcode,p+1);
                strcpy(dbtitre,dbcode);
              }
            updatedatabase(name,0,1);
            if (reload)
               ReloadAllData(LocalBase,true,false);


          }
  // Effacer l'actualisation en cours

  Dir1=ExtractFileDir(AnsiString(exname));
  Dir2=ExtractFileDir(AnsiString(name));


  Copy_Actu(Dir1.c_str(),Dir2.c_str(),OldProject,ProjectName);

  Clean_Actu();

}


void __fastcall TplStandard::Copy_Actu(char *oldnamedir,char *newnamedir,char *oldproj, char *newproj)
{
 char tmp[MAX_PATH];   int done;  int i;
 char dir[MAX_PATH];   struct ffblk ffblk;
 AnsiString FDir;   char proj[200];
 char src[MAX_PATH]; char dest[MAX_PATH];
 char old[500],newp[500];

 strcpy(proj,oldproj); strcat(proj,"-");
 strcpy(old,oldproj);strcat(old,"-");
 strcpy(newp,newproj); strcat(newp,"-");
 // FDir = ExtractFileDir(oldname);
 strcpy(dir,oldnamedir); strcat(dir,"\\*.*");

 done = findfirst(dir,&ffblk,0);
   while (!done)
   {
     strcpy(tmp,ffblk.ff_name);
     if (strstr(tmp,proj))
        {

         //  strcpy(form_result,str_replace(form_result,item,ff));

         strcpy(src,oldnamedir); strcat(src,"\\"); strcat(src,tmp);
         strcpy(dest,src); strcpy(dest,str_replace_alpha(dest,old,newp));

         if (strstr(tmp,".don") || strstr(tmp,".DON"))  Copy_Binary(dest,src);
         if (strstr(tmp,".act") || strstr(tmp,".ACT"))  Copy_Binary(dest,src);
         if (strstr(tmp,".dbr") || strstr(tmp,".DBR"))  Copy_Binary(dest,src);
          i=i;
        }

    done = findnext(&ffblk);
   }

}


char*  __fastcall TplStandard::str_replace_alpha(char *form,char *item,char *txt)
{
 char *p,*p1;
 int lenfl,lenitem;  char xform[5000],endform[5000];

 strcpy(xform,form);
 lenitem = strlen(item);
 p=strstr(xform,item);
 if (p==NULL) return form;
 p1 = p+lenitem;
 strcpy(endform,p1);
 *p=0;
 strcat(xform,txt);
 strcat(xform,endform);
 return xform;
}

void __fastcall TplStandard::Color_Avas()
{
 char tmp[200];
 strcpy(tmp,"<FONT color=\"clGreen\">% Rest"); strcat(tmp,"</FONT>");
 avas->Cells[3][0]=AnsiString(tmp);
 strcpy(tmp,"<FONT color=\"clGreen\">J Rest"); strcat(tmp,"</FONT>");
 avas->Cells[4][0]=AnsiString(tmp);
 //strcpy(tmp,"<FONT color=\"clBlue\">Du R�el."); strcat(tmp,"</FONT>");
 //avas->Cells[5][0]=AnsiString(tmp);
 strcpy(tmp,"<FONT color=\"clGreen\">Fin+tot R"); strcat(tmp,"</FONT>");
 avas->Cells[7][0]=AnsiString(tmp);

}

void __fastcall TplStandard::Clean_Actu()
{
    int i,j;

    Clean_Grid(asAvanc,2);
    Clean_Grid(AdvStringGrid6,2);
    Clean_Grid(AdvStringGrid7,2);
    Clean_Grid(avact,2);
    Clean_Grid(avas,2);
    Color_Avas();
    Clean_Grid(AdvStringGrid4,2);
    Clean_Grid(AdvStringGrid5,2);

    btSavAct->Enabled=false;
    btSaveActu->Enabled = false;
    Clear_Diagram(immGnt);
    Clear_Diagram(Image1);
    Clear_Diagram(imLand);
    Clear_Diagram(imPort);
    Clear_Diagram(imAvanc);
    Clear_Diagram(imAvGantt);
    Clear_Diagram(imAvCal);
    Clear_Diagram(imP3Screen);

     Label12->Caption = "_______";
     Label22->Caption = "0";
     Label31->Caption = "0";
     Label55->Caption = "0";
     Label97->Caption = "0";
     Label96->Caption = "___/___/___/___";
     Label18->Caption = "../../..";
     Label24->Caption = "0";
     Label28->Caption = "../../..";

}


void __fastcall TplStandard::btSauverCommeClick(TObject *Sender)
{
 strcpy(SAVEAS,"");
 SaveAs(false);
}

void __fastcall TplStandard::Copy_Binary(char *dest,char *src)
{
  FILE *fdest,*fsrc;  int lg;
 char buf[5000];

 fsrc=fopen(src,"r+b");
 if (!fsrc)
    {
     fdest =fopen(dest,"w+b");
     fclose(fdest);
    return;
    }

 fdest =fopen(dest,"w+b");
 while (!feof(fsrc))
   {
    lg = fread(buf,1,4096,fsrc);
    fwrite(buf,1,lg,fdest);
  }
 fclose(fsrc);
 fclose(fdest);

}


void __fastcall TplStandard::Copy_File(char *dest,char *src)
{
 FILE *fdest,*fsrc;
 char tmp[2500];

 fsrc=fopen(src,"r+b");
 if (!fsrc)
    {
     fdest =fopen(dest,"w+b");
     fclose(fdest);
    return;
    }

 fdest =fopen(dest,"w+b");
 while (!feof(fsrc))
   {
    tmp[0]=0;
    fgets(tmp,2490,fsrc);
    fputs(tmp,fdest);
  }
 fclose(fsrc);
 fclose(fdest);
}

void __fastcall TplStandard::Save()
{
updatedatabase(LocalBase,0,0); // 0 $$$$
strcpy(m_status,"<caption>");
strcat(m_status,LocalBase);
strcat(m_status,"</caption>");
}


//---------------------------------------------------------------------------

void __fastcall TplStandard::btSauverClick(TObject *Sender)
{
 Save();
}

//---------------------------------------------------------------------------

/*---------------------------------------------------------------------------
This function is the OnClick event of a TButton that is on a TForm.
On the TForm is also a TImage named image.
------------------------------------------------------------------------*/
void __fastcall TplStandard::Print_Image(TImage *image)
{
    TPrinter *p = Printer();
    int scale_x, scale_y;

    // create a memory dc for the image
    HDC h_dc = image->Picture->Bitmap->Canvas->Handle;
    int bmp_w = image->Picture->Bitmap->Width;
    BM_W = bmp_w;
    int bmp_h = image->Picture->Bitmap->Height;
    BM_H = bmp_h;

    HDC h_mem_dc = ::CreateCompatibleDC (h_dc);
    HBITMAP h_mem_bmp = ::CreateCompatibleBitmap (h_dc, bmp_w, bmp_h);
    HBITMAP h_old_bmp = ::SelectObject (h_mem_dc, h_mem_bmp);

    // fix up bad video drivers
    bool is_pal_dev = false;
    LOGPALETTE *pal;
    HPALETTE h_pal, h_old_pal;

    if (::GetDeviceCaps (image->Canvas->Handle, RASTERCAPS) & RC_PALETTE)
    {
        pal = static_cast <LOGPALETTE *>(malloc (sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256)));
        memset (pal, 0, sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256));
        pal->palVersion = 0x300;
        pal->palNumEntries = ::GetSystemPaletteEntries(image->Canvas->Handle, 0, 256, pal->palPalEntry);
        if (pal->palNumEntries != 0)
        {
            h_pal = ::CreatePalette (pal);
            h_old_pal = ::SelectPalette (h_mem_dc, h_pal, false);
            is_pal_dev = true;
        }
        else
        {
            free (pal);
        }
    }

    // copy the image on to the memory dc
    ::BitBlt (h_mem_dc, 0, 0, bmp_w, bmp_h, h_dc, 0, 0, SRCCOPY);

    if (is_pal_dev)
    {
        ::SelectPalette (h_mem_dc, h_old_pal, false);
        ::DeleteObject (h_pal);
    }

    // delete the mem dc
    ::SelectObject (h_mem_dc, h_old_bmp);
    ::DeleteDC (h_mem_dc);

    // get memory for a BITMAPIFO Structure
    HANDLE h_bmp_info = ::GlobalAlloc (GHND, sizeof (BITMAPINFO) + (sizeof (RGBQUAD) * 256));
    BITMAPINFO* bmp_info = static_cast <BITMAPINFO *>(::GlobalLock (h_bmp_info));
    //Set up the structure
    memset (bmp_info, NULL, sizeof (BITMAPINFO) + (sizeof (RGBQUAD) * 255));
    bmp_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info->bmiHeader.biPlanes = 1;
    bmp_info->bmiHeader.biBitCount = 8;
    bmp_info->bmiHeader.biWidth = bmp_w;
    bmp_info->bmiHeader.biHeight = bmp_h;
    bmp_info->bmiHeader.biCompression = BI_RGB;

    // find out how much memory for the bits
    ::GetDIBits (h_dc, h_mem_bmp, 0, bmp_h, NULL, bmp_info, DIB_RGB_COLORS);

    // Allocate memory for the bits
    HANDLE h_bits = GlobalAlloc (GHND, bmp_info->bmiHeader.biSizeImage);
    void *bits = ::GlobalLock (h_bits);

    // this time get the bits
    ::GetDIBits (h_dc, h_mem_bmp, 0, bmp_h, bits, bmp_info, DIB_RGB_COLORS);

    // fix up for bad video driver

    if (is_pal_dev)
    {
        for (int i = 0; i < pal->palNumEntries; i++)
        {
            bmp_info->bmiColors[i].rgbRed = pal->palPalEntry[i].peRed;
            bmp_info->bmiColors[i].rgbGreen = pal->palPalEntry[i].peGreen;
            bmp_info->bmiColors[i].rgbBlue = pal->palPalEntry[i].peBlue;
        }
        free (pal);
    }

    // begin the printing
    p->BeginDoc ();

    // scale print size

    /*
    if (P_MODE[pc]=='P')
    // if (PAGE_WIDTH < PAGE_HEIGHT)
    {
        scale_x = PAGE_WIDTH;
        scale_y = image->Picture->Height * (PAGE_WIDTH / bmp_w);
    }
    else
    {
        scale_x = image->Picture->Width * (PAGE_HEIGHT / bmp_h);
        scale_y = PAGE_HEIGHT;
    }
    */
    // fix up for print with palette
    is_pal_dev = false;
    if (::GetDeviceCaps (h_dc, RASTERCAPS) & RC_PALETTE)
    {
        pal = static_cast<LOGPALETTE *>(malloc (sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256)));
        memset (pal, 0, sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256));
        pal->palVersion = 0x300;
        pal->palNumEntries = 256;
        for (int i = 0; pal->palNumEntries; i++)
        {
            pal->palPalEntry[i].peRed = bmp_info->bmiColors[i].rgbRed;
            pal->palPalEntry[i].peGreen = bmp_info->bmiColors[i].rgbGreen;
            pal->palPalEntry[i].peBlue = bmp_info->bmiColors[i].rgbBlue;
        }
        h_pal = CreatePalette(pal);
        free (pal);
        h_old_pal = SelectPalette(p->Canvas->Handle, h_pal, false);
        is_pal_dev = true;
    }



    // $$$$ scale_y = PAGE_HEIGHT;
    // $$$$ scale_x = PAGE_WIDTH;

    if (P_MODE[pc]=='P')
    {
        scale_x = int (float(PAGE_WIDTH)*CoefFormat);
        scale_y = int (float(PAGE_HEIGHT)*CoefFormat);
        // scale_y = image->Picture->Height * (PAGE_WIDTH / bmp_w);
    }
    else
    {
        scale_x = int (float(PAGE_HEIGHT)*CoefFormat);
        scale_y = int (float(PAGE_WIDTH)*CoefFormat);


         //scale_x = PAGE_HEIGHT;
         //scale_y = PAGE_WIDTH;



        //scale_x = image->Picture->Width * (PAGE_HEIGHT / bmp_h);
        // scale_y = PAGE_HEIGHT;
    }
    SCALE_X = scale_x;
    SCALE_Y = scale_y;




    // send the bits to the printer
    StretchDIBits(p->Canvas->Handle, 0, 0, scale_x, scale_y,
		0, 0, bmp_w, bmp_h, bits,bmp_info, DIB_RGB_COLORS, SRCCOPY);

    // end the print
    p->EndDoc ();

    // clean up
    ::DeleteObject (h_mem_bmp);

    if (is_pal_dev)
    {
        ::SelectObject (p->Canvas->Handle, h_old_pal);
        ::DeleteObject (h_pal);
    }

    ::GlobalUnlock (bits);
    ::GlobalFree (h_bits);
    ::GlobalUnlock (bmp_info);
    ::GlobalFree (h_bmp_info);
}


void __fastcall TplStandard::Print()
{


}

void __fastcall TplStandard::Print_Bitmap(Graphics::TBitmap *imPP)
{
    TPrinter *p = Printer();
    int scale_x, scale_y;

    // create a memory dc for the image
    HDC h_dc = imPP->Canvas->Handle;  //image->Picture->Bitmap->Canvas->Handle;
    int bmp_w = imPP->Width;
    BM_W = bmp_w;
    int bmp_h = imPP->Height;
    BM_H = bmp_h;

    HDC h_mem_dc = ::CreateCompatibleDC (h_dc);
    HBITMAP h_mem_bmp = ::CreateCompatibleBitmap (h_dc, bmp_w, bmp_h);
    HBITMAP h_old_bmp = ::SelectObject (h_mem_dc, h_mem_bmp);

    // fix up bad video drivers
    bool is_pal_dev = false;
    LOGPALETTE *pal;
    HPALETTE h_pal, h_old_pal;

    if (::GetDeviceCaps (imPP->Canvas->Handle, RASTERCAPS) & RC_PALETTE)
    {
        pal = static_cast <LOGPALETTE *>(malloc (sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256)));
        memset (pal, 0, sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256));
        pal->palVersion = 0x300;
        pal->palNumEntries = ::GetSystemPaletteEntries(imPP->Canvas->Handle, 0, 256, pal->palPalEntry);
        if (pal->palNumEntries != 0)
        {
            h_pal = ::CreatePalette (pal);
            h_old_pal = ::SelectPalette (h_mem_dc, h_pal, false);
            is_pal_dev = true;
        }
        else
        {
            free (pal);
        }
    }

    // copy the image on to the memory dc
    ::BitBlt (h_mem_dc, 0, 0, bmp_w, bmp_h, h_dc, 0, 0, SRCCOPY);

    if (is_pal_dev)
    {
        ::SelectPalette (h_mem_dc, h_old_pal, false);
        ::DeleteObject (h_pal);
    }

    // delete the mem dc
    ::SelectObject (h_mem_dc, h_old_bmp);
    ::DeleteDC (h_mem_dc);

    // get memory for a BITMAPIFO Structure
    HANDLE h_bmp_info = ::GlobalAlloc (GHND, sizeof (BITMAPINFO) + (sizeof (RGBQUAD) * 256));
    BITMAPINFO* bmp_info = static_cast <BITMAPINFO *>(::GlobalLock (h_bmp_info));
    //Set up the structure
    memset (bmp_info, NULL, sizeof (BITMAPINFO) + (sizeof (RGBQUAD) * 255));
    bmp_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info->bmiHeader.biPlanes = 1;
    bmp_info->bmiHeader.biBitCount = 8;
    bmp_info->bmiHeader.biWidth = bmp_w;
    bmp_info->bmiHeader.biHeight = bmp_h;
    bmp_info->bmiHeader.biCompression = BI_RGB;

    // find out how much memory for the bits
    ::GetDIBits (h_dc, h_mem_bmp, 0, bmp_h, NULL, bmp_info, DIB_RGB_COLORS);

    // Allocate memory for the bits
    HANDLE h_bits = GlobalAlloc (GHND, bmp_info->bmiHeader.biSizeImage);
    void *bits = ::GlobalLock (h_bits);

    // this time get the bits
    ::GetDIBits (h_dc, h_mem_bmp, 0, bmp_h, bits, bmp_info, DIB_RGB_COLORS);

    // fix up for bad video driver

    if (is_pal_dev)
    {
        for (int i = 0; i < pal->palNumEntries; i++)
        {
            bmp_info->bmiColors[i].rgbRed = pal->palPalEntry[i].peRed;
            bmp_info->bmiColors[i].rgbGreen = pal->palPalEntry[i].peGreen;
            bmp_info->bmiColors[i].rgbBlue = pal->palPalEntry[i].peBlue;
        }
        free (pal);
    }

    // begin the printing
    p->BeginDoc ();

    // scale print size


    if (P_MODE[pc]=='P')
     if (PAGE_WIDTH < PAGE_HEIGHT)
    {
        scale_x = PAGE_WIDTH;
        scale_y = imPP->Height * (PAGE_WIDTH / bmp_w);
    }
    else
    {
        scale_x = imPP->Width * (PAGE_HEIGHT / bmp_h);
        scale_y = PAGE_HEIGHT;
    }

    // fix up for print with palette
    is_pal_dev = false;
    if (::GetDeviceCaps (h_dc, RASTERCAPS) & RC_PALETTE)
    {
        pal = static_cast<LOGPALETTE *>(malloc (sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256)));
        memset (pal, 0, sizeof (LOGPALETTE) + (sizeof (PALETTEENTRY) * 256));
        pal->palVersion = 0x300;
        pal->palNumEntries = 256;
        for (int i = 0; pal->palNumEntries; i++)
        {
            pal->palPalEntry[i].peRed = bmp_info->bmiColors[i].rgbRed;
            pal->palPalEntry[i].peGreen = bmp_info->bmiColors[i].rgbGreen;
            pal->palPalEntry[i].peBlue = bmp_info->bmiColors[i].rgbBlue;
        }
        h_pal = CreatePalette(pal);
        free (pal);
        h_old_pal = SelectPalette(p->Canvas->Handle, h_pal, false);
        is_pal_dev = true;
    }



    scale_y = p->PageHeight;  //PAGE_HEIGHT;
    scale_x = p->PageWidth;  //PAGE_WIDTH;
    
    SCALE_X = scale_x*CoefFormat;
    SCALE_Y = scale_y*CoefFormat;




    // send the bits to the printer
    StretchDIBits(p->Canvas->Handle, 0, 0, SCALE_X, SCALE_Y,
		0, 0, bmp_w, bmp_h, bits,bmp_info, DIB_RGB_COLORS, SRCCOPY);

    // end the print
    p->EndDoc ();

    // clean up
    ::DeleteObject (h_mem_bmp);

    if (is_pal_dev)
    {
        ::SelectObject (p->Canvas->Handle, h_old_pal);
        ::DeleteObject (h_pal);
    }

    ::GlobalUnlock (bits);
    ::GlobalFree (h_bits);
    ::GlobalUnlock (bmp_info);
    ::GlobalFree (h_bmp_info);
}


void __fastcall TplStandard::Timer2Timer(TObject *Sender)
{
 Timer2->Enabled=false;
 Propage(CH);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btDPClick(TObject *Sender)
{
  char snumt[30]; int prem_tache;

      strcpy(snumt,avact->Cells[1][1].c_str());
      prem_tache = atoi(snumt);
      if (prem_tache != 0)
        {
          // Remplir_asTri_Avanc();  // �������������
          //btActRapidClick(Sender);
           Display_Graphics(prem_tache, imAvCal, imAvGantt);
          // Display_Graphics(prem_tache, immCal, immGnt);
        }

}
//---------------------------------------------------------------------------





void __fastcall TplStandard::imPortMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
// mouse coordinates X and Y
int rx1,rx2,ry1,ry2;
TRect Dst,Src;
int imx; int imy;
int ext;

 if (P_MODE[pc]=='L') { P_MODE[pc]='P'; RAZ(); Prepare_Draw_Planning(); }
 ZoomCopyAv(imPort,X,Y);

 /*
imx=im->Width;
imy=im->Height;

///   imPort is assigned to Image im !!!!!

ext = (11-Zoom->Position)*ZOOM;
rx1= (imx * X / imPort->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > im->Width) {rx2=im->Width; rx1=rx2-2*ext; }

ry1= (imy * Y / imPort->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > im->Height) {ry2=im->Height; ry1=ry2-2*ext; }


Dst = Rect(0,0,imAvanc->Width,imAvanc->Height);
Src = Rect(rx1,ry1,rx2,ry2);

imAvanc->Canvas->CopyRect(Dst,imPort->Canvas,Src);
*/
}
//---------------------------------------------------------------------------

int __fastcall TplStandard::exist_prev(int t)
{
 int i; int tach=0;
 for (i=1;i<av->RowCount;i++)
   {
    sprintf(adv,"%d",sommets[i]);
    // getvalue(1,i,1);
    tach=atoi(adv);
    if (tach==t) { tach=i; break; }
   }
return tach;
}



void __fastcall TplStandard::Image_Avanc(char *title, char *dtav,int col)
{
 int isg,jsg,bcl,tprevis,trow,marg,dist; //,distPrev;
 char d1[15],dt1[15]; int a1,m1,j1;
 char dtPrev[15],dtAvanc[15];  int result,rettot;
 int distdeb,distfin,distfinprev;
 int JJ,MM,AA;

 dt1[0]=dtav[0]; dt1[1]=dtav[1];dt1[3]=dtav[3];
 dt1[4]=dtav[4];dt1[6]=dtav[6]; dt1[7]=dtav[7];
 dt1[2]='-';dt1[5]='-';dt1[8]=0;
 strcpy(dtAvanc,dt1);


 sprintf (dtPrev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
 sprintf (dtAvanc,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);
 comp_dist_dates(dtPrev,dtAvanc,1,0);
 DistAvanc = NB1;


 MAXDIST=0;
 bcl = 1; isg=1;
 while (bcl < asAvanc->RowCount)
 {
  /*

   // recuperer les donnees previsionnelles
   getvalue(5,bcl,39); tprevis = atoi(adv);

   getvalue(5,bcl,48);  // fin prev
   strcpy(d1,adv); d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
   distfinprev = convert_date(a1+2000,m1,j1);


   // retirer a result le retard total;
   //result = DistAvanc;
   // retirer retard si le reel chevauche la date avanc;

   getvalue(5,bcl,41); //  41 47date debut reel
   strcpy(d1,adv); d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
   distdeb = convert_date(a1+2000,m1,j1);
   getvalue(5,bcl,43); // 43  48date fin reelle
   strcpy(d1,adv); d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
   distfin = convert_date(a1+2000,m1,j1);

   // ���$ result = DistAvanc;
   result = 0;
   if (distdeb<=DistAvanc && distfin >= DistAvanc)
     {



      // result = nb jours restants
      if (distfin==distfinprev)
        result=0;

      else
        {
         getvalue(5,bcl,45); rettot = atoi(adv);         // retard total  // 72
         result = rettot;
        }



     }

     sg->Cells[10+col][isg] = AnsiString(result);

   // donnees reelles
   isg++; if (col==0) sg->RowCount++;
   sg->Cells[1][isg] = "";  // critere 1
   sg->Cells[2][isg] = "";  // critere 2
   sg->Cells[3][isg] = "";  // critere 3
   getvalue(5,bcl,62); sg->Cells[9][isg] = AnsiString(adv);  // cal
   getvalue(5,bcl,39); sg->Cells[4][isg] = AnsiString(adv);
   getvalue(5,bcl,40); sg->Cells[5][isg] = AnsiString(adv);
   getvalue(5,bcl,41); sg->Cells[6][isg] = AnsiString(adv);  // date debut  reelle
   getvalue(5,bcl,43); sg->Cells[7][isg] = AnsiString(adv);  // date fin  reelle

   getvalue(5,bcl,72); sg->Cells[0][isg] = AnsiString(adv);  // jours restants

   // calcul de la distance max date de fin (pour le nb de jours)
   strcpy(d1,adv);
   d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
   dist = convert_date(a1+2000,m1,j1);

   if (dist>MAXDIST) MAXDIST=dist;

   getvalue(5,bcl,73); marg=atoi(adv); // margetotale
   sg->Cells[8][isg] = 2 ; // color verte

   isg++;
   */
   bcl++;

  }


}


void __fastcall TplStandard::Graph_Avanc(int nbt,int maxdist)
{
 int espacex,espacey;
 int rx1,rx2,ry1,ry2;
 TRect Dst,Src;
 int ext; char tmp[100]; int a;

 // TPrinter *pt = Printer();
 //
 if (im) delete im;
  im = new Graphics::TBitmap();
  im->PixelFormat =pf8bit;
  im->Height = PAGE_HEIGHT;
  im->Width  = PAGE_WIDTH;
  TRect Diagram = Rect(0,0, im->Width,im->Height);
  im->Canvas->Brush->Color = clWhite; // BgColor;
  im->Canvas->FillRect(Diagram);

  Flag_Taches_Concernees();
  Compute_DD1_DD2();

 DIST_AVANC = Convert_Date(AnsiString(DATEAVANC));
 Compute_Deb_Fin_Avanc();
 if ((DFINMAX-DDEB) != 0) espacex = im->Width/(DFINMAX-DDEB)*100/120;
 else espacex=70;

 strcpy(tmp,edHoriz->Text.c_str());
 a = atoi(tmp);
 if (a!=0) espacex = a;

 Label139->Caption = AnsiString(espacex);

 if (NBTACHES_AVANC !=0) espacey=(95*im->Height/100) / NBTACHES_AVANC;
 strcpy(tmp,edVertic->Text.c_str());
 a = atoi(tmp);
 if (a!=0) espacey = a;

 Label140->Caption = AnsiString(espacey);

 if (espacey > 1200) espacey=1200;

Draw_Calendars(NBTACHES_AVANC,DFIN,espacex,espacey);
Draw_Diagram(DFIN,espacex,espacey);
Copy_Calendar();

imPort->Picture->Assign(im);
imLand->Picture->Assign(im);

///   imPort is assigned to Image im !!!!!

ext = 400;
rx1= 0;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > im->Width) {rx2=im->Width; rx1=rx2-2*ext; }

ry1= 0;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > im->Height) {ry2=im->Height; ry1=ry2-2*ext; }

Dst = Rect(0,0,imAvanc->Width,imAvanc->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imAvanc->Canvas->CopyRect(Dst,imPort->Canvas,Src);
}


void __fastcall TplStandard::Flag_Taches_Concernees()
{
 int i,cnt;int distdeb,distfin,distavanc;
 cnt = asAvanc->RowCount;

distavanc = Convert_Date(AnsiString(DATEAVANC));


 for (i=1;i<cnt;i++)
   {
    asAvanc->Cells[23][i]="";
    distdeb=Convert_Date(asAvanc->Cells[3][i]);     // 8 = debut prev 3= debut reel
    distfin = Convert_Date(asAvanc->Cells[5][i]);   // 5 = fin reelle
    if ((distavanc - distdeb) < 5 || (distdeb-distavanc) <5)  asAvanc->Cells[23][i]="X";
    if ((distavanc - distfin) < 5)  asAvanc->Cells[23][i]="X";
    if ((distdeb <= distavanc) && (distfin >= distavanc)) asAvanc->Cells[23][i]="X";

   }


}


void __fastcall TplStandard::Draw_Calendars(int nbt,int maxdist, int spx,int spy)
{
 int ix,xbase,x1base,y,x1;
 char tmp[255]; AnsiString Titre;
 TDateTime dtFirst;
 int nbtach;   char dtPrev[15],dtAvanc[15];
 int y1; int epaisseur;


 H_TITRE = 2*im->Height/100;
 H_CAL = 1*im->Height/100;  // 2 % + 2 %


 if (nbt !=0)
  { epaisseur = spy/10;  // (3*nbtt);
    // im->Canvas->Font->Size = epaisseur;
    im->Canvas->Font->Size = epaisseur*8/10;  // *12/10;
  }
 else
   { epaisseur = 15;
     im->Canvas->Font->Size = 15;
   }

 nbtach = nbt;
 Y_FIN_CAL = H_TITRE + 2*H_CAL + 2;  // 3*im->Width/100;
 Y_BAS_PLANNING = (nbtach)*spy + Y_FIN_CAL;
 TRect Diagram = Rect(0,H_TITRE,im->Width,H_TITRE+2*H_CAL);
 im->Canvas->Brush->Color = clAqua; //
 im->Canvas->FillRect(Diagram);

 im->Canvas->Pen->Width = 1;
 im->Canvas->Brush->Style = bsSolid;

 // trac� du rectangle global;
 y = Y_BAS_PLANNING + 2*H_CAL+2;
 im->Canvas->Pen->Color = clBlack;
 im->Canvas->MoveTo(0,0);
 im->Canvas->LineTo(im->Width-1,0);
 im->Canvas->LineTo(im->Width-1,y);
 im->Canvas->LineTo(0,y);  //im->Height-1);
 im->Canvas->LineTo(0,0);

//  AnAvanc = AnAvanc % 100;

 sprintf(dtAvanc,"%02d-%02d-%04d",JourAvanc,MoisAvanc,AnAvanc%100+2000);
 strcpy(tmp,"Projet ");  strcat(tmp,ProjectName);

 //strcat(tmp,LocalBase);
 strcat(tmp," - Actualisation au "); strcat(tmp,dtAvanc);
 im->Canvas->Font->Style = TFontStyles()<< fsBold;
 Compute_LH("Arial",tmp,epaisseur*6/10,"","Gras");
 im->Canvas->Font->Size = epaisseur*6/10;
 im->Canvas->Font->Name = "Arial";
 im->Canvas->Font->Color = clBlack;
 im->Canvas->Brush->Color = clWhite;
 x1 = im->Width/2 - L/2;
 y1 = 2*im->Height/1000;  // ordonn�e du titre
 Titre = AnsiString(tmp);
 im->Canvas->Brush->Style = bsClear;
 im->Canvas->TextOut(x1,y1,Titre);


 // ligne pour la bande de titre
 im->Canvas->MoveTo(0,H_TITRE);
 im->Canvas->LineTo(im->Width-1,H_TITRE);



 // lignes pour les dates

 im->Canvas->MoveTo(0,H_TITRE+ H_CAL); //2*spy);
 im->Canvas->LineTo(im->Width-1,H_TITRE + H_CAL);
 im->Canvas->MoveTo(0,H_TITRE + 2*H_CAL);
 im->Canvas->LineTo(im->Width-1,H_TITRE + 2*H_CAL);

 im->Canvas->Font->Style = TFontStyles()<< fsBold;

 /*
 if (nbtach< 15) im->Canvas->Font->Size = 30;
 else if (nbtach< 35) im->Canvas->Font->Size = 18;

 else if (nbtach< 55) im->Canvas->Font->Size = 18;
 else if (nbtach< 75) im->Canvas->Font->Size = 16;
 else if (nbtach< 95) im->Canvas->Font->Size = 16;
 else if (nbtach< 115) im->Canvas->Font->Size = 20;
 else if (nbtach< 135) im->Canvas->Font->Size = 14;
 else if (nbtach< 155) im->Canvas->Font->Size = 12;
 else if (nbtach< 175) im->Canvas->Font->Size = 12;
 else im->Canvas->Font->Size = 8;
 */
 im->Canvas->Font->Color = clBlack;
 im->Canvas->Brush->Color = clAqua;


 y= (NBTACHES_AVANC+4)*spy;
 // tenir Compte de D1,D2 ,DDEB,DFIN


 // for (ix=0;ix<= maxdist;ix++)    // tenir compte du calendrier
    for (ix=DDEB;ix<=DFINMAX+500;ix++)
   {
    xbase=(ix-DDEB)*spx + spx/3;
    add_jour(AnPrev,MoisPrev,JourPrev,ix);
    sprintf(tmp, "%02d",Jour);
    im->Canvas->Font->Color = clBlack;
    im->Canvas->Brush->Color = clAqua;
    im->Canvas->Brush->Style = bsClear;
    im->Canvas->TextOut(xbase,3+H_TITRE,(AnsiString) tmp);
    im->Canvas->Pen->Width=1;
    im->Canvas->MoveTo((ix-DDEB)*spx,H_TITRE);
    im->Canvas->LineTo((ix-DDEB)*spx,H_TITRE+H_CAL);

    // trac� des conges
      xbase= (ix+1-DDEB)*spx;
      if (cal[1][ix]=='N')
      {
       x1base  =  xbase - spx+1;
       y = Y_FIN_CAL; y1 = Y_BAS_PLANNING;
       Diagram = Rect(x1base,y+1,xbase,y1-1); //im->Height-3*spy);
       im->Canvas->Brush->Color = clSilver; //
       im->Canvas->FillRect(Diagram);
      }

     im->Canvas->Font->Color = clBlack;
     im->Canvas->Brush->Color = clWhite;

     if (spx > 50)
      {
     im->Canvas->Pen->Style = psDot;  // Dash;
     im->Canvas->MoveTo(xbase,Y_FIN_CAL);
     im->Canvas->LineTo(xbase,Y_BAS_PLANNING-1); //im->Height-3*spy);
     im->Canvas->Pen->Style = psSolid;
      }
     add_jour(AnPrev,MoisPrev,JourPrev,ix);
     dtFirst = EncodeDate(An,Mois,Jour);

     int firstday = dtFirst.DayOfWeek();
     if (firstday == 2)  // Mondays
       {
       // trace d'un trait vertical noir
        x1base  =  xbase - spx;
        sprintf(tmp,"%02d/%02d/%04d",Jour,Mois,An);
        im->Canvas->Pen->Color = clBlack;
        im->Canvas->Brush->Color = clAqua;  // White;
        im->Canvas->Brush->Style = bsClear;
        im->Canvas->TextOut(x1base+5,H_TITRE + H_CAL+3,AnsiString (tmp));
        im->Canvas->Pen->Color = clBlack;
        im->Canvas->Brush->Color = clWhite;  // White;
        im->Canvas->MoveTo(x1base,H_TITRE + H_CAL+1);
        im->Canvas->LineTo(x1base,H_TITRE + 2*H_CAL+1);  //im->Height-3*spy);
       }
     if (Jour==1)  // 1er du mois, un trait rouge;
       {
        x1base  =  xbase - spx;
        im->Canvas->Font->Color = clRed;
        im->Canvas->Brush->Color = clAqua;
        imGantt->Canvas->MoveTo(x1base,Y_FIN_CAL);
        imGantt->Canvas->LineTo(x1base,Y_BAS_PLANNING); //im->Height-3*spy);
       }
     if (ix==DistAvanc)
       {
         x1base  =  xbase - spx;
        im->Canvas->Pen->Width=3;
        im->Canvas->Pen->Color = clRed;
        im->Canvas->MoveTo(x1base,Y_FIN_CAL);
        im->Canvas->LineTo(x1base,Y_BAS_PLANNING);
       }

   }  // end for

 // copie du "rectangle bleu des dates
 Diagram = Rect(0,H_TITRE,im->Width,H_TITRE+2*H_CAL);
 y= (nbtach+4)*spy;
 y= (NBTACHES_AVANC+4)*spy+ spy;


 TRect Dst = Rect(0,Y_BAS_PLANNING+2,im->Width,Y_BAS_PLANNING+2 + 2*H_CAL);
 im->Canvas->CopyRect(Dst,im->Canvas,Diagram);
}




void __fastcall TplStandard::Draw_Diagram(int maxdist,int spx,int spy)
{
 int nbt,color,jcal,dist1,dist2,dist3,dist4;
 char tmp[200],d1[15],d2[15];  bool OK;
 int a1,a2,m1,m2,j1,j2; int m;
 int jx; int x,y,interval; int distbrisee;
 int step,k,xt,yt,nbtaches,it,y1;
 AnsiString Texte;  char dtav[20];
 char dt1[20],dtAvanc[20],dtPrev[20];
 int indx;  int YA;  int marGP,marGR;
  int DistCongesMoinsAvanc;   char tmpRT[50];  int RT,RI;
 int DistCongesPlusAvanc; int nbtt;
 int x1,ya,x2,y2;  int epaisseur;
 TRect R;   int YL1,YL2; AnsiString S;

 int pstyle;
 HPEN hPen;
 LOGBRUSH LBrush;
 DWORD pStyle;

  pstyle = psSolid;
  pStyle  = PS_GEOMETRIC | pstyle | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;

  // strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
  //     c=TColor(a);
  LBrush.lbStyle = BS_SOLID;
  LBrush.lbHatch = 0;  //HS_DIAGCROSS;

  LBrush.lbColor = clPurple;
  //      strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);
  // a = clYellow;
   hPen = ExtCreatePen(pStyle, clYellow, &LBrush, 0, NULL);
   im->Canvas->Pen->Handle = hPen;




 step = spx / 5;
 //  nbt = sg->RowCount - 1;

 nbtaches = asAvanc->RowCount - 1;  //nbt/2;
 // Count number of OK
 nbtt=0;;
 for (it=1;it<asAvanc->RowCount;it++)
     { if (asAvanc->Cells[23][it]=="X") nbtt++; }

 if (nbtt !=0)
  { epaisseur = spy/10;  // (3*nbtt);
    // im->Canvas->Font->Size = epaisseur;
    im->Canvas->Font->Size = epaisseur*8/10;  // *12/10;
  }
 else
   { epaisseur = 15;
     im->Canvas->Font->Size = 15;
   }

  YL1 = spy/3; YL2= spy*2/3;


 indx = 0;
 for (it=1;it<=nbtaches;it++)
  {
   dist1=Convert_Date(asAvanc->Cells[8][it]); // date prev
   dist2 = Convert_Date(asAvanc->Cells[10][it]); // date fin  prev
   dist3 = Convert_Date(asAvanc->Cells[3][it]); // date deb reel
   dist4 = Convert_Date(asAvanc->Cells[5][it]); // date fin reel
   OK = false;
   if (asAvanc->Cells[23][it]=="X") OK = true;
   // if ((dist1>=DDEB && dist1 <=DFINMAX) || (dist3>=DDEB && dist3 <=DFINMAX))
   if (OK)
   {
    indx++;
   interval = dist2-dist1+1;
   y = spy*(indx-1) + Y_FIN_CAL;   // 3 a cause des titres
   //  ecrire d'abord le texte
   im->Canvas->Font->Color = clBlack;
   im->Canvas->Brush->Color = clWhite;
   im->Canvas->Brush->Style = bsClear;

   // yt = y + spy*22/100;  // spy/10;  // 10 %

   yt= y+ YL1-epaisseur*20/10; //  - epaisseur/2;
   xt = (dist1-DDEB)*spx+5;
   //Texte = asAvanc->Cells[1][it] + " " + asAvanc->Cells[2][it];   // N� de tache + Design.
   Texte = asAvanc->Cells[2][it];
   im->Canvas->Brush->Style = bsClear;
   im->Canvas->TextOut(xt,yt,Texte);

   // tache previsionnelle

   YA=y+30*spy/100;
   im->Canvas->Pen->Width = 10*spy/100;
   strcpy(tmp,asAvanc->Cells[16][it].c_str());   // marge Totale  Prev
   // strcpy(tmp,"0");
   marGP=atoi(tmp);

   strcpy(tmp,asAvanc->Cells[15][it].c_str());   // marge Totale  Reelle

   // strcpy(tmp,"0");
   marGR=atoi(tmp);

   // sprintf(tmp,"N� Tache %s, Marge Prev = %d  Marge Reelle = %d", asAvanc->Cells[1][it].c_str(), marGP,marGR);
   // Application->MessageBox(tmp,"ECOPL",MB_OK);

   if (marGP==0) im->Canvas->Pen->Color = clRed;
    else im->Canvas->Pen->Color = clBlue;

   // calendrier dans val1 (col 9)
   strcpy(tmp,asAvanc->Cells[14][it].c_str()); //sg->Cells[9][ix].c_str());
   jcal= atoi(tmp);

   // TRACE DU PREVISIONNEL
   for (jx=0;jx<interval;jx++)
     {

       if (cal[jcal][dist1+jx] != 'N')
         {
           x1=(dist1-DDEB+jx) *spx;
           ya = y + YL1-epaisseur/2; //spy*(indx-1) + Y_FIN_CAL +40*spy/100;
           x2 = x1+ spx; y2=ya+epaisseur; // 10 epaisseur
           R = Rect(x1,ya,x2,y2);
           if (marGP==0) im->Canvas->Brush->Color = clRed;
           else im->Canvas->Brush->Color = clBlue;
           //im->Canvas->Brush->Color = c;
           im->Canvas->FillRect(R);
         }
     }

     im->Canvas->Brush->Color = clWhite;
     im->Canvas->Brush->Style = bsClear;
     im->Canvas->Font->Color = clPurple;

     x2=x2+10; ya=y+YL1-epaisseur*60/100;
     im->Canvas->TextOut(x2,ya,"Fin Pr�v.: " + asAvanc->Cells[10][it]);
     Texte = asAvanc->Cells[1][it];   // N� de tache
     Compute_LH("Arial",Texte,epaisseur*8/10,"Standard","Standard");
     x1=(dist1-DDEB) *spx - L-spx/4;
     im->Canvas->TextOut(x1,ya,Texte);


    /*    ANCIENNE METHODE
    for (jx=0;jx<interval;jx++)
     {
      x = (dist1-DDEB+jx) * spx;
      YA = spy*(indx-1) + Y_FIN_CAL +40*spy/100;
      im->Canvas->MoveTo(x,YA);
      if (cal[jcal][dist1+jx]== 'N')
        {
         for (k=0;k<2;k++)
           {
            x = x + step;
            im->Canvas->MoveTo(x,YA);
            x = x+step;
            im->Canvas->LineTo(x,YA);
           }

        }
      else im->Canvas->LineTo(x+spx,YA);
     }
    */
    // Trac� ligne Bris�e 1
   cbLB->Checked=true;
   im->Canvas->Pen->Color=clWhite;
   if (cbLB->Checked == true)
   {
   for (m=0; m<CntAVBefore; m++)
     {
      strcpy(dtav,TableDatesAV[m]);
      dt1[0]=dtav[6]; dt1[1]=dtav[7];dt1[3]=dtav[3];
      dt1[4]=dtav[4];dt1[6]=dtav[0]; dt1[7]=dtav[1];
      dt1[2]='-';dt1[5]='-';dt1[8]=0;
      strcpy(dtAvanc,dt1);
      //strcpy(dtAvanc,dtav);
      sprintf (dtPrev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
      comp_dist_dates(dtPrev,dtAvanc,1,0);
      DistAvanc = NB1;
      //DistCongesMoinsAvanc= recule_conges(DistAvanc,1);
      DistCongesPlusAvanc = avance_conges(DistAvanc,1);
      im->Canvas->Pen->Width = 4;
      if (dist3<= (DistAvanc-1)  &&  dist4>=(DistAvanc-1))
           {
             distbrisee = dist4 - DistCongesPlusAvanc + 1;
           }
         else distbrisee = 0;
       if (dist2==dist4) distbrisee = 0;

         if (m==0 || m==3 || m==6) im->Canvas->Pen->Color=clPurple;
         if (m==1 || m==4 || m==7) im->Canvas->Pen->Color=clBlue;
         if (m==2 || m==5 || m==8) im->Canvas->Pen->Color=clLime;

         im->Canvas->Pen->Width=5;
         x = (DistAvanc-DDEB)*spx+1;
         y1 = Y_FIN_CAL + (indx-1)*spy;
         im->Canvas->MoveTo(x,y1);
         x = (DistAvanc-DDEB)*spx+1;
         //y1 = (indx-1)* spy + Y_FIN_CAL + 15*spy/100;   // 30
         y1 = (indx-1)* spy + Y_FIN_CAL + 1*spy/100;   // 30
         im->Canvas->LineTo(x,y1);


         if (distbrisee !=0)
         x = ((dist2-DDEB) - distbrisee + 1)*spx+1;
         else x = (DistAvanc-DDEB)*spx+1;

        //y1=(indx-1)* spy + Y_FIN_CAL + 40*spy/100; ;    // 45
        y1=(indx-1)* spy + Y_FIN_CAL + 35*spy/100; ;
        im->Canvas->LineTo(x,y1);
        x = (DistAvanc-DDEB)*spx+1;
        y1= (indx-1)* spy + Y_FIN_CAL + 80*spy/100; ;   // 60
        im->Canvas->LineTo(x,y1);
        y1= (indx-1)* spy + Y_FIN_CAL + 100*spy/100;
        im->Canvas->LineTo(x,y1);

     }
    }  // If checked


   ///ix = ix+1;   // tache reelle

   im->Canvas->Pen->Color=clWhite;
   im->Canvas->Pen->Width = 10*spy/100;
   /*
   if (marGR!=0) im->Canvas->Pen->Color = clGreen;
    else im->Canvas->Pen->Color = clLime;
   */

   if (marGR!=0) im->Canvas->Brush->Color = clGreen;
    else im->Canvas->Brush->Color = clLime;
   // calendrier dans val1 (col 9)
   strcpy(tmp,asAvanc->Cells[14][it].c_str()); jcal= atoi(tmp);
   strcpy(d1,asAvanc->Cells[3][it].c_str()); // date debut  reel
   strcpy(d2,asAvanc->Cells[5][it].c_str()); // date fin reelle
   d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
   dist1 = convert_date(a1+2000,m1,j1);
   d2[2]=0; j2=atoi(d2);  d2[5]=0; m2=atoi(d2+3);  d2[8]=0; a2=atoi(d2+6);
   dist2 = convert_date(a2+2000,m2,j2);

   interval = dist2-dist1+1;


   // TRACE DU REEL
   for (jx=0;jx<interval;jx++)
     {

       if (cal[jcal][dist1+jx] != 'N')
         {
           x1=(dist1-DDEB+jx) *spx;
           ya = y + YL2 - epaisseur/2;  // spy*(indx-1) + Y_FIN_CAL +60*spy/100;    // 60 et non plus 40
           x2 = x1+ spx; y2=ya+epaisseur; // 10 epaisseur
           R = Rect(x1,ya,x2,y2);
           if (marGR!=0) im->Canvas->Brush->Color = clGreen;
           else im->Canvas->Brush->Color = clLime;
           //im->Canvas->Brush->Color = c;
           im->Canvas->FillRect(R);
         }
     }

     im->Canvas->Brush->Color = clWhite;
     im->Canvas->Brush->Style = bsClear;
     im->Canvas->Font->Color = clBlack;
     // mettre RT et RI  dessus la ligne reelle
    // last x2
    strcpy(tmp,asAvanc->Cells[7][it].c_str());   // Retard Total
    RT = atoi(tmp);
    strcpy(tmp,asAvanc->Cells[6][it].c_str());  // Retard Imputable
    RI = atoi(tmp);
    if (RI != 0)
    sprintf(tmpRT,"Retard Total :%d Retard Imputable:%d",RT,RI);
     else
     sprintf(tmpRT,"Retard Total :%d",RT);

    Compute_LH("Arial",AnsiString(tmpRT),epaisseur*8/10,"Standard","Standard");
    ya=y + YL2 - epaisseur*20/10; // -epaisseur/2;
    x2=(dist4-DDEB+1) *spx - L - spx/2; // spx/40;

    im->Canvas->TextOut(x2+5,ya,AnsiString(tmpRT));


     im->Canvas->Brush->Color = clWhite;
     im->Canvas->Brush->Style = bsClear;
     im->Canvas->Font->Color = clPurple;
     x2=(dist4-DDEB+1) *spx;
     x2=x2+10; ya=y+YL2-epaisseur*55/100;
     im->Canvas->TextOut(x2,ya,"Fin R�elle: " + asAvanc->Cells[5][it]);

     Texte = asAvanc->Cells[1][it];   // N� de tache
     Compute_LH("Arial",Texte,epaisseur*8/10,"Standard","Standard");
     x1=(dist3-DDEB) *spx - L-spx/4;
     im->Canvas->TextOut(x1,ya,Texte);



    } // If CONDITION
  }
}
/////////////////////////////////////////////////////////////////////////////


/*
if (strcmp(Cod,"LH")==0)
      {
       pStyle  = PS_GEOMETRIC | pstyle | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;

       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       LBrush.lbStyle = BS_SOLID;
       LBrush.lbColor = c;
       LBrush.lbHatch = 0;  //HS_DIAGCROSS;
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);

       hPen = ExtCreatePen(pStyle, a, &LBrush, 0, NULL);
       B->Canvas->Pen->Handle = hPen;
       B->Canvas->Pen->Width = a;

       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Pen->Color = c;
       B->Canvas->Pen->Style = pstyle;

       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[3][i].c_str()); x2= atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str()); y2=atoi(tmp);

       if (y2>(y1+1))
         {
           R = Rect(x1,y1,x2,y2);
           B->Canvas->Brush->Color = c;
           B->Canvas->FillRect(R);
         }
        else
          {
           B->Canvas->MoveTo(x1,y1);
            B->Canvas->LineTo(x2,y1);
          }
      }
*/

void __fastcall TplStandard::Copy_Calendar()
{

}

void __fastcall TplStandard::Compute_Deb_Fin_Avanc()
{
 // int  DDEB,DFIN;

 int i; int distavanc,distavanc_1;
 int dist2,dist4,dist1,dist3; int nbtaches;
 int dfinx;

 NBTACHES_AVANC = 0;
 nbtaches = asAvanc->RowCount - 1;
 DDEB=0; DFIN = 0;
 distavanc_1=Reverse_Date(DATEAVANC_1);  //  Convert_Date(AnsiString(DATEAVANC_1));
 distavanc  = Convert_Date(AnsiString(DATEAVANC));
 DDEB = DD1; // distavanc - D1;
 if (DDEB<0) DDEB = 0;
 DFIN = DD1+ distavanc + DD2;

 for (i=1;i<=nbtaches;i++)
   {
    dist1=Convert_Date(asAvanc->Cells[8][i]); // date prev
    dist2 = Convert_Date(asAvanc->Cells[10][i]); // date fin  prev
    dist3 = Convert_Date(asAvanc->Cells[3][i]); // date deb reel
    dist4 = Convert_Date(asAvanc->Cells[5][i]); // date fin reel
    if ((dist1>=DDEB && dist1 <=DFIN) || (dist3>=DDEB && dist3 <=DFIN))
     {
      NBTACHES_AVANC++;
      if (dist2 > DFINMAX) DFINMAX = dist2;
      if (dist4 > DFINMAX) DFINMAX = dist4;
     }
   }
 DFINMAX = DFINMAX+DD2;

 NBTACHES_AVANC=0;
 for (i=1;i<=nbtaches;i++)
   {
    dist1=Convert_Date(asAvanc->Cells[8][i]); // date prev
    dist2 = Convert_Date(asAvanc->Cells[10][i]); // date fin  prev
    dist3 = Convert_Date(asAvanc->Cells[3][i]); // date deb reel
    dist4 = Convert_Date(asAvanc->Cells[5][i]); // date fin reel
    if ((dist1>=DDEB && dist1 <=DFINMAX) || (dist3>=DDEB && dist3 <=DFINMAX))
     {
      NBTACHES_AVANC++;
     }
   }


}


//---------------------------------------------------------------------------

void __fastcall TplStandard::btPrAvancClick(TObject *Sender)
{
 int i;

 CoefFormat=ComputeFormat(edAjust);
// ImprimeDirect(2,imP1p,imP1l);
 //ImprimeDirect(2,imP1p,imP1l);

 for (i=0;i<CntAVBefore;i++)
   Image_Avanc("Avancement",TableDatesAV[i],i);

 Graph_Avanc(asAvanc->RowCount-1,MAXDIST);

 //ImprimeDirect(1,imPort,imLand);
 Print_Bitmap(im);

 // Print_Image(imPort);  // Port);

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::imLandMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
int rx1,rx2,ry1,ry2;
TRect Dst,Src;
int imx; int imy;
imx=im->Width;
imy=im->Height;
int ext;

///   imLand is assigned to Image im !!!!!


if (P_MODE[pc]=='P') { P_MODE[pc]='L'; RAZ(); Prepare_Draw_Planning(); }
ZoomCopyAv(imLand,X,Y);

/*
ext = (11-Zoom->Position)*100;
rx1= (imx * X / imLand->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > im->Width) {rx2=im->Width; rx1=rx2-2*ext; }

ry1= (imy * Y / imLand->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > im->Height) {ry2=im->Height; ry1=ry2-2*ext; }


Dst = Rect(0,0,imAvanc->Width,imAvanc->Height);
Src = Rect(rx1,ry1,rx2,ry2);

 imAvanc->Canvas->CopyRect(Dst,imLand->Canvas,Src);
 */
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::btFontClick(TObject *Sender)
{
 FontDialog1 = new TFontDialog(NULL);
 FontDialog1->Execute();
}


//---------------------------------------------------------------------------


void __fastcall TplStandard::asAvancGetCellColor(TObject *Sender, int ARow,
      int ACol, TGridDrawState AState, TBrush *ABrush, TFont *AFont)
{

 int perc;

 
 if (ARow==0) return;
 getvalue(5,ARow,71); perc = atoi(adv);
 if (perc != 100)
   {
    if (ACol==1)
      {  ABrush->Color=clYellow;

      }
   }

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Clean_Grid(TAdvStringGrid *sgrid, int strow)
{
 int r,c;
 int rr,cc;

 rr=sgrid->RowCount; cc=sgrid->ColCount;
 for (r=1;r<rr;r++)
  {
   for (c=1;c<cc;c++) sgrid->Cells[c][r]="";
  }

  sgrid->RowCount=strow;

}

void _fastcall TplStandard::Read_Criteres()
{
 FILE *fp,*fcopy;
 char namefp[MAX_PATH],namecopy[MAX_PATH],bidon[MAX_PATH];
 char bufc[500];
 char tmp[250]; int i,niv,ctr;
 char buffer[250];
 char c0[100],c1[100],c2[100],c3[100],c4[100];

 strcpy(namefp,LocalBase); strcat(namefp,"_crit.txt");
// strcpy(namecopy,LocalBase); strcat(namecopy,"_crit1.txt");
// strcpy(bidon,LocalBase); strcat(bidon,"_crit2.txt");
// fcopy=fopen(namecopy,"wt");
 ctr=0;
 fp=fopen(namefp,"rt");

 if (fp)
   {
    buffer[0]=0; ctr=0;
    fgets(buffer,512,fp);
     while (!feof(fp))   // lire les 8 premiers records;
      {
       ctr++;
       Ghost->ExtractValue(tmp,buffer,"l",0);
       if (strcmp(tmp,"C1")==0)
         { strcpy(tmp,"P");
           strcpy(bufc,"<c>1</c><l>P</l><d>PHASES</d><t></t>\n");
         }
         else strcpy(bufc,buffer);
       asCrit->Cells[ctr*2+1][0]=tmp;
       CRITERES[ctr-1]=AnsiString(tmp);
       S_CRIT[ctr-1][0] = AnsiString(tmp)+"-";  // NIV 0
       Ghost->ExtractValue(tmp,buffer,"d",0);
       if (strcmp(tmp,"D�signation 1")==0) strcpy(tmp,"PHASES");
       asCrit->Cells[ctr*2+2][0]=tmp;
       LIB_CRITERES[ctr-1]=AnsiString(tmp);
       if (ctr==9) break;
       //fputs(bufc,fcopy);
       fgets(buffer,512,fp);
      }
    ctr=0;

    while (!feof(fp))
     {
      ctr++;
      LibRupt[ctr] = "";
      Ghost->ExtractValue(c1,buffer,"c1",0);
      if (strcmp(c1,"C1")==0)
         { strcpy(c1,"P");
           strcpy(bufc,"<c1>P</c1><c2></c2><c3></c3><c4></c4><d>PHASES</d><r></r><t>0</t>\n");
         }
        else strcpy(bufc,buffer);
      if (strlen(c1)) { LibRupt[ctr] +=AnsiString(c1) + "-"; niv=0; } //else LibRupt[ctr] += "-";
      Ghost->ExtractValue(c2,buffer,"c2",0);
      if (strlen(c2)) { LibRupt[ctr] += AnsiString(c2) + "-"; niv=1; } //else LibRupt[ctr] += "-";
      Ghost->ExtractValue(c3,buffer,"c3",0);
      if (strlen(c3)) { LibRupt[ctr] +=AnsiString(c3) + "-";  niv=2; } //else LibRupt[ctr] += "-";
      Ghost->ExtractValue(c4,buffer,"c4",0);
      if (strlen(c4)) { LibRupt[ctr] +=AnsiString(c4) + "-";  niv=3; } //else LibRupt[ctr] += "-";

      strcat(c1,"-");
      for (i=0;i<8;i++)
        {

         if (strcmp(c1,S_CRIT[i][0].c_str()) == 0)
           {
            S_CRIT[i][niv] =  LibRupt[ctr];
           }
        }

        Ghost->ExtractValue(tmp,buffer,"d",0);
      if (strcmp(tmp,"D�signation 1")==0) strcpy(tmp,"PHASES");
      if (strlen(tmp)) LibRupt[ctr]+= "!" + AnsiString(tmp);

      
     // fputs(bufc,fcopy);
      fgets(buffer,512,fp);
     }

    LibCtr = ctr;
    fclose (fp);
//    fclose (fcopy);
   }
// RENAME ICI
// rename(namefp,bidon);
// rename(namecopy,namefp);

}


int  __fastcall TplStandard::RechercheColCrit(char *str)
{
int result;  AnsiString Test, Str; int i,j;
 result=0;
 Str = AnsiString(str);
 for (i=1;i<=8;i++)
  {
   j=2*i+2;  // libelle , et non pas le code
   Test = asCrit->Cells[j][0];

   if (asCrit->Cells[j][0]==Str) result=i;
  }
return result;

}


void __fastcall TplStandard::Load_Grille_Criteres()
{
 FILE*fp;
 int i,nt,ctr,ix; char fn[250];  char buf[2024];
 char tmp[250];

 i=av->RowCount;

 Clean_Grid(asCrit,2);
 Read_Criteres();

 for (i=1;i<=nb_taches;i++)
   {
    if (i>1) asCrit->RowCount++;
    ix = sommets[i];
    asCrit->Cells[1][i]=AnsiString(ix);
    asCrit->Cells[2][i]=av->Cells[2][ix];  //libel[i];
   }

         // Lecture du fichier _affect;
   strcpy(fn,LocalBase);
   strcat(fn,"_affect.txt");
   fp=fopen(fn,"rt");
   if (fp)
     {
      buf[0]=0; ctr=0;
      fgets(buf,2000,fp);
      while (!feof(fp))
        {
         if (strlen(buf)>5)
               {
                ctr++;

                asCrit->Colors[3][ctr] = TColor(RGB(255,255,153));
                asCrit->Colors[5][ctr] = TColor(RGB(255,255,153));
                asCrit->Colors[7][ctr] = TColor(RGB(255,255,153));

                Ghost->ExtractValue(tmp,buf,"nt",0);
                nt = atoi(tmp);

                asCrit->Cells[1][ctr]=AnsiString(nt); // tmp;

                Ghost->ExtractValue(tmp,buf,"des",0); asCrit->Cells[2][ctr]=tmp;


                Ghost->ExtractValue(tmp,buf,"c1",0); asCrit->Cells[3][ctr]=tmp;
                av->Cells[41][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d1",0); asCrit->Cells[4][ctr]=tmp;
                av->Cells[51][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"c2",0); asCrit->Cells[5][ctr]=tmp;
                av->Cells[42][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d2",0); asCrit->Cells[6][ctr]=tmp;
                av->Cells[52][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"c3",0); asCrit->Cells[7][ctr]=tmp;
                av->Cells[43][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d3",0); asCrit->Cells[8][ctr]=tmp;
                av->Cells[53][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"c4",0); asCrit->Cells[9][ctr]=tmp;
                av->Cells[44][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d4",0); asCrit->Cells[10][ctr]=tmp;
                av->Cells[54][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"c5",0); asCrit->Cells[11][ctr]=tmp;
                av->Cells[45][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d5",0); asCrit->Cells[12][ctr]=tmp;
                av->Cells[55][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"c6",0); asCrit->Cells[13][ctr]=tmp;
                av->Cells[46][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d6",0); asCrit->Cells[14][ctr]=tmp;
                av->Cells[56][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"c7",0); asCrit->Cells[15][ctr]=tmp;
                av->Cells[47][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d7",0); asCrit->Cells[16][ctr]=tmp;
                av->Cells[57][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"c8",0); asCrit->Cells[17][ctr]=tmp;
                av->Cells[48][nt]=tmp;
                Ghost->ExtractValue(tmp,buf,"d8",0); asCrit->Cells[18][ctr]=tmp;
                av->Cells[58][nt]=tmp;
                } // if strlen

                fgets(buf,2000,fp);
               } // while
            fclose(fp);
           } //  if fp
}

void __fastcall TplStandard::Regen_Criteres()
{
 int i,j,ind,indcrit,numt; int cnt; AnsiString S;
// genere ascrit d'apres av

 Clean_Grid(asCrit,2);
 Read_Criteres();

 // Examine av-> Pour attribuer  P---, I---, T---
 cnt = av->RowCount;
 for (i=1;i<cnt; i++)
   {
    if (av->Cells[41][i]=="") { av->Cells[41][i]="P---";   av->Cells[51][i]="PHASES"; }
    if (av->Cells[42][i]=="") { av->Cells[42][i]="I---";   av->Cells[52][i]="INTERVENANTS"; }
    if (av->Cells[43][i]=="") { av->Cells[43][i]="T---";   av->Cells[53][i]="TRIS"; }
    if (av->Cells[44][i]=="") { av->Cells[44][i]="R---";   av->Cells[54][i]="RESSOURCES"; }
   }




 indcrit = 0;
 for (i=1;i<=LASTTASK;i++)
 // for (i=1;i<=nb_taches;i++)
   {
    if (av->Cells[2][i] != "" && filtr[i]=='X')   // designation
     {
      indcrit++;
      if (indcrit>1) asCrit->RowCount++;
      asCrit->Cells[1][indcrit]=AnsiString(i); ////sommets[i]);
      asCrit->Cells[2][indcrit]=av->Cells[2][i];  //libel[i];

      for (j=1;j<9;j++)   // copie des colonnes
         {
          ind = j*2 + 1;
          asCrit->Cells[ind][indcrit] = av->Cells[j+40][i];  // asCrit->Cells[ind][i];
          asCrit->Colors[ind][indcrit] = TColor(RGB(255,255,153));
          ind++;
          S= av->Cells[j+50][i];
          asCrit->Cells[ind][indcrit] = av->Cells[j+50][i];  // asCrit->Cells[ind][i];
        }
     }
   }

Synchro_LibCrit();


}


void __fastcall TplStandard::trace_calChange(TObject *Sender)
{
 /*
 FILE *fp;
 int Pindex,i;    int ctr;
 char tmp[250],fn[250],buf[1024];

 Pindex = PageControl1->ActivePageIndex;

 switch (Pindex)
   {

    case 2 :
          strcpy(m_menu,"ENACoches2");
          //Load_Grille_Criteres();
          break;
    case 5 :
          pc=1;
          break;
    case 6 :
          pc=2;
          break;
    case 7 :
          pc=3;
          break;

    case 9 :
          strcpy(m_menu,"ENACoches1");
          if (flag_loadavanc)
            {
             flag_loadavanc=false;
             Load_Avanc(FileNameAvanc,true);
            }
         break;



   } // end switch
 //sprintf(tmp,"Active Page Index is %d",Pindex);
 // Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
 */
        if(PageControl1->ActivePageIndex !=1 )
        {
        TabSheet5->TabVisible = false;
        }

       imP->SaveToFile("C:\\Users\\Claude\\Desktop\\demo\\cap.bmp");

 }
//---------------------------------------------------------------------------

void __fastcall TplStandard::Coche_Decoche_Crit(int col,int row)
{
  int numtach;
  char tmp[150];

  if ((row > nb_taches) || (row < 0)) return;
  if (col==0)
     {
      strcpy(tmp,asCrit->Cells[1][row].c_str());
      numtach=atoi(tmp);
      if (numtach != 0)
        {
          if (coch[numtach]=='X') { coch[numtach]=' '; asCrit->Cells[0][row] = ' '; }
          else { coch[numtach]='X';    asCrit->Cells[0][row] = AnsiString(strcoche); }
        }
     }
 }

void __fastcall TplStandard::asCritMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  int Row,Column;
  asCrit->MouseToCell(X, Y, Column, Row);
  CrRow=Row;CrCol=Column;
  Coche_Decoche_Crit(Column,Row);
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::Cocher_Decocher_Crit()
{
 int i;
 int count;
 count = asCrit->RowCount;
 for (i=1;i<=LASTTASK;i++)
  {
   if (coch[i]==' ') coch[i] = 'X';
   else coch[i] = ' ';
  }

 for (i=1;i<=count;i++)
  {
   if (asCrit->Cells[0][i]==" ")
    {
     asCrit->Cells[0][i]=AnsiString(strcoche);
    }
   else
    {
     asCrit->Cells[0][i]=" ";

    }
  }

}

void __fastcall TplStandard::Tout_Cocher_Crit()
{
  int i;
  int count;
count = asCrit->RowCount;

 for (i=1;i<=LASTTASK;i++)  coch[i] = 'X';
 for (i=1;i<count;i++)    asCrit->Cells[0][i]=AnsiString(strcoche);  //"X";
  

}

void __fastcall TplStandard::Tout_Decocher_Crit()
{
int i;
int count;
count = asCrit->RowCount;

 for (i=1;i<=LASTTASK;i++) coch[i]=' ';
 for (i=1;i<count;i++) asCrit->Cells[0][i]=" ";

}
void __fastcall TplStandard::asCritContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
  int X,Y;
 X = MousePos.x;
 Y = MousePos.y+150;
  //AdvStringGrid1->MouseToCell(X, Y, CurCol, CurRow);
 // conversion en ligne et colonne courantes
 PopupCrit->Popup(X, Y);

}
//---------------------------------------------------------------------------




void __fastcall TplStandard::asCritClickCell(TObject *Sender, int ARow,
      int ACol)
{
if (ARow==0) return
 DispCritere(ARow,ACol);
}

void __fastcall TplStandard::DispCritere(int ARow,int ACol)
{

 FILE *fp;
 int Col,Ind,j,ctr;
 char tmp[1000],xbuffer[520];
 char c1[50],c2[50],c3[50],c4[50];
 char lib[130];
 int  cnt[5];

 if (ACol<3) return;

 lC1->Caption="";lC2->Caption="";lC3->Caption="";lC4->Caption="";
 lD1->Caption="";lD2->Caption="";lD3->Caption="";lD4->Caption="";
 lT1->Caption="";lT2->Caption="";lT3->Caption="";lT4->Caption="";
 j = ACol % 2;
  if (j==0) Col=ACol-1; else Col=ACol;

 Ind = (Col-2)/2; Ind++;
 if ((Ind<1) || (Ind>8)) return;
 strcpy(tmp,asCrit->Cells[Col][ARow].c_str());


 j=Explode('-',tmp);
 if (strlen(params[0])) lC1->Caption = AnsiString(params[0]); else lC1->Caption = "";
 if (strlen(params[1])) lC2->Caption = AnsiString(params[1]); else lC2->Caption = "";
 if (strlen(params[2])) lC3->Caption = AnsiString(params[2]); else lC3->Caption = "";
 if (strlen(params[3])) lC4->Caption = AnsiString(params[3]); else lC4->Caption = "";

 strcpy(tmp,LocalBase); strcat(tmp,"_crit.txt");

 fp=fopen(tmp,"rt");
 if (fp == NULL)  return;
 // recherche des libell�s
  xbuffer[0]=0; ctr=0;
  fgets(xbuffer,512,fp);
   while (!feof(fp))   // lire les 8 premiers records;
      {
       ctr++;
       if (ctr==8) break;
       fgets(xbuffer,512,fp);
       }

     fgets(xbuffer,512,fp);
     ctr = 0;

 while (!feof(fp))
    {

      ctr++;
      if (strlen(xbuffer) > 5)
       {
       Ghost->ExtractValue(c1,xbuffer,"c1",0); Ghost->ExtractValue(c2,xbuffer,"c2",0);
       Ghost->ExtractValue(c3,xbuffer,"c3",0); Ghost->ExtractValue(c4,xbuffer,"c4",0);
       Ghost->ExtractValue(lib,xbuffer,"d",0);
       if ((strcmp(c1,params[0])==0) && (strlen(c2)==0))
           lD1->Caption = AnsiString(lib);
       else if ((strcmp(c1,params[0])==0) && (strcmp(c2,params[1])==0)&&(strlen(c3)==0))
           lD2->Caption = AnsiString(lib);
       else if ((strcmp(c1,params[0])==0) && (strcmp(c2,params[1])==0)&&
           (strcmp(c3,params[2])==0)&& (strlen(c4)==0))
           lD3->Caption = AnsiString(lib);
       else if ((strcmp(c1,params[0])==0) && (strcmp(c2,params[1])==0) &&
           (strcmp(c3,params[2])==0)&& (strcmp(c4,params[3])==0))
            { lD4->Caption = AnsiString(lib); break; }
       }  // strlen (5);
      fgets(xbuffer,512,fp);
      } // end while
 fclose (fp);

 // Compter les taches dans les criteres et sous criteres

 strcpy(c1,params[0]);
 strcpy(c2,params[1]);
 strcpy(c3,params[2]);
 strcpy(c4,params[3]);

 ctr = asCrit->RowCount;  cnt[0]=cnt[1]=cnt[2]=cnt[3]=0;
 int i;
 for (i=1;i<ctr;i++)
   { strcpy(tmp,asCrit->Cells[Col][i].c_str());
    j=Explode('-',tmp);
    if (strcmp(c1,params[0])==0)
      { cnt[0]++;
        if (strcmp(c2,params[1])==0 && strlen(params[1]))
         {
          cnt[1]++;
          if (strcmp(c3,params[2])==0 && strlen(params[2]))
            {
             cnt[2]++;
             if (strcmp(c4,params[3])==0 && strlen(params[3])) cnt[3]++;
            }
         }
      }
   }
 lT1->Caption = AnsiString(cnt[0]);
 lT2->Caption = AnsiString(cnt[1]);
 lT3->Caption = AnsiString(cnt[2]);
 lT4->Caption = AnsiString(cnt[3]);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btAccCritClick(TObject *Sender)
{

 TCriteres *Criteres;  int lg;   AnsiString S1,S2,S3;

 CopyCriteres();

 strcpy(m_exchange,"");
 Criteres = new TCriteres(Application);
 Criteres->Height=773;
 Criteres->Width=905;
 Criteres->ShowModal();
 delete Criteres;

 lg = strlen(m_criteres);
 if (lg>0)
 {

  Exec_Criteres(m_criteres);
  m_criteres[0]=0;

  Change1();

  }

 S1 = av->Cells[51][1];
 S2 = av->Cells[52][1];
 S3 = av->Cells[53][1];


 // $$$$$$ CopyCritTSL_AV();
 Remplir_AV1();
 Remplir_asCrit();

 S1 = av->Cells[51][1];
 S2 = av->Cells[52][1];
 S3 = av->Cells[53][1];

 //Read_Criteres();



 // $$$$
 Regen_Criteres();
 CopyCriteres();


 SaveCriteres(true);

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::Save_Crit_Renum()
{
char fn[250];
 FILE *fp;  char fname[MAX_PATH]; char  name1[MAX_PATH],name2[MAX_PATH];
 int i,count,nt,cnt;  char tmp[5000],tmp1[50];
 int rc,recdata; Char Key[200],RetKey[200];

 strcpy(fn,LocalBase);
 strcat(fn,"_affect.txt");

  // strcpy(fname,m_directory); strcat(fname,"\\temp");
 strcpy(name1,m_directory);strcat(name1,"\\temp1.ndx"); rc=unlink(name1);
 strcpy(name2,m_directory);strcat(name2,"\\temp1.dat"); rc=unlink(name2);
 strcpy(fname,m_directory);strcat(fname,"\\temp1");
 temp = new realisam();
 rc = temp->OpenEngine(fname,120,"ndx","dat");
 rc = temp->SetIndexMode(0,0);
 if (rc==0) { ShowMessage("Erreur � l'ouverture fichier temp"); return; }
 cnt = asCrit->RowCount;
 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,asCrit->Cells[1][i].c_str());
    nt = atoi(tmp);
    sprintf(Key,"<T%04d>",nt);
    recdata=temp->GetNewRecordNumber();
    rc=temp->WriteKey(0,Key,recdata);

    tmp[0]=0;
    strcat(tmp,"<nt>");
    strcpy(tmp1,asCrit->Cells[1][i].c_str());
    nt = atoi(tmp1);
    sprintf(tmp1,"%d",nt);
    strcat(tmp,tmp1);
    strcat(tmp,"</nt>");
    strcat(tmp,"<des>");
    strcat(tmp,asCrit->Cells[2][i].c_str()); strcat(tmp,"</des>");
    strcat(tmp,"<c1>"); strcat(tmp,asCrit->Cells[3][i].c_str()); strcat(tmp,"</c1>");
    strcat(tmp,"<d1>"); strcat(tmp,asCrit->Cells[4][i].c_str()); strcat(tmp,"</d1>");
    strcat(tmp,"<c2>"); strcat(tmp,asCrit->Cells[5][i].c_str()); strcat(tmp,"</c2>");
    strcat(tmp,"<d2>"); strcat(tmp,asCrit->Cells[6][i].c_str()); strcat(tmp,"</d2>");
    strcat(tmp,"<c3>"); strcat(tmp,asCrit->Cells[7][i].c_str()); strcat(tmp,"</c3>");
    strcat(tmp,"<d3>"); strcat(tmp,asCrit->Cells[8][i].c_str()); strcat(tmp,"</d3>");
    strcat(tmp,"<c4>"); strcat(tmp,asCrit->Cells[9][i].c_str()); strcat(tmp,"</c4>");
    strcat(tmp,"<d4>"); strcat(tmp,asCrit->Cells[10][i].c_str()); strcat(tmp,"</d4>");
    strcat(tmp,"<c5>"); strcat(tmp,asCrit->Cells[11][i].c_str()); strcat(tmp,"</c5>");
    strcat(tmp,"<d5>"); strcat(tmp,asCrit->Cells[12][i].c_str()); strcat(tmp,"</d5>");
    strcat(tmp,"<c6>"); strcat(tmp,asCrit->Cells[13][i].c_str()); strcat(tmp,"</c6>");
    strcat(tmp,"<d6>"); strcat(tmp,asCrit->Cells[14][i].c_str()); strcat(tmp,"</d6>");
    strcat(tmp,"<c7>"); strcat(tmp,asCrit->Cells[15][i].c_str()); strcat(tmp,"</c7>");
    strcat(tmp,"<d7>"); strcat(tmp,asCrit->Cells[16][i].c_str()); strcat(tmp,"</d7>");
    strcat(tmp,"<c8>"); strcat(tmp,asCrit->Cells[17][i].c_str()); strcat(tmp,"</c8>");
    strcat(tmp,"<d8>"); strcat(tmp,asCrit->Cells[18][i].c_str()); strcat(tmp,"</d8>");
    rc = temp->WriteRecord(tmp,strlen(tmp)+1);
   }
 strcpy(fname,m_directory);strcat(fname,"\\temp1");
 rc=temp->CloseReopen(fname,120,"ndx","dat");
 rc = temp->SetIndexMode(0,0);



 fp = fopen(fn,"wt");
 if (fp)
   {
      rc = temp->ReadFirstKey(0,RetKey,&recdata);
      while (rc)
        {
         tmp[0]=0;
         rc = temp->ReadRecord(tmp,recdata);
         strcat(tmp,"\n");
         fputs(tmp,fp);
         rc=temp->ReadNextKey(RetKey,&recdata);
       }

     fclose(fp);
    // delete temp;
   }
 rc=temp->CloseEngine();
 // delete temp;  
}

void __fastcall TplStandard::SaveCriteres(bool cpy)
{
 // Sauvegarde des donn�es pr�sentes dans la grille
 // Fichier s�quentiel texte

 char fn[250];
 FILE *fp;
 int i,count,nt;  char tmp[5000],tmp1[50];
 strcpy(fn,LocalBase);
 strcat(fn,"_affect.txt");


 fp = fopen(fn,"wt");

 if (fp)
   {
    count=av->RowCount;
    for (i=1;i<count;i++)
    {
      if (av->Cells[1][i] != "")
       {
       tmp[0]=0;
       strcat(tmp,"<nt>");


       strcpy(tmp1,av->Cells[1][i].c_str());
       nt = atoi(tmp1);
       sprintf(tmp1,"%d",nt);

       strcat(tmp,tmp1);
       strcat(tmp,"</nt>");
       strcat(tmp,"<des>");


       strcat(tmp,av->Cells[2][i].c_str()); strcat(tmp,"</des>");
       strcat(tmp,"<c1>"); strcat(tmp,av->Cells[41][i].c_str()); strcat(tmp,"</c1>");
       strcat(tmp,"<d1>"); strcat(tmp,av->Cells[51][i].c_str()); strcat(tmp,"</d1>");
       strcat(tmp,"<c2>"); strcat(tmp,av->Cells[42][i].c_str()); strcat(tmp,"</c2>");
       strcat(tmp,"<d2>"); strcat(tmp,av->Cells[52][i].c_str()); strcat(tmp,"</d2>");
       strcat(tmp,"<c3>"); strcat(tmp,av->Cells[43][i].c_str()); strcat(tmp,"</c3>");
       strcat(tmp,"<d3>"); strcat(tmp,av->Cells[53][i].c_str()); strcat(tmp,"</d3>");
       strcat(tmp,"<c4>"); strcat(tmp,av->Cells[44][i].c_str()); strcat(tmp,"</c4>");
       strcat(tmp,"<d4>"); strcat(tmp,av->Cells[54][i].c_str()); strcat(tmp,"</d4>");
       strcat(tmp,"<c5>"); strcat(tmp,av->Cells[45][i].c_str()); strcat(tmp,"</c5>");
       strcat(tmp,"<d5>"); strcat(tmp,av->Cells[55][i].c_str()); strcat(tmp,"</d5>");
       strcat(tmp,"<c6>"); strcat(tmp,av->Cells[46][i].c_str()); strcat(tmp,"</c6>");
       strcat(tmp,"<d6>"); strcat(tmp,av->Cells[56][i].c_str()); strcat(tmp,"</d6>");
       strcat(tmp,"<c7>"); strcat(tmp,av->Cells[47][i].c_str()); strcat(tmp,"</c7>");
       strcat(tmp,"<d7>"); strcat(tmp,av->Cells[57][i].c_str()); strcat(tmp,"</d7>");
       strcat(tmp,"<c8>"); strcat(tmp,av->Cells[48][i].c_str()); strcat(tmp,"</c8>");
       strcat(tmp,"<d8>"); strcat(tmp,av->Cells[58][i].c_str()); strcat(tmp,"</d8>");

      strcat(tmp,"\n");
      fputs(tmp,fp);
       }
     }
 fclose(fp);
 if (cpy) CopyCriteres();
 //Application->MessageBoxA("Sauvegarde Effectu�e",m_ecoplan,MB_OK);
 }  // if fp
 else Application->MessageBoxA("Impossible d'ouvrir le fichier Affectation",m_ecoplan,MB_OK);
}

void __fastcall TplStandard::CopyCriteres()
{
 int i,count,a; char tmp[500];
 AnsiString S1,S2,S3;
 TSL->Clear();
  count=av->RowCount;
 for (i=1;i<count;i++)
    {
      if (av->Cells[2][i] != "")
       {
       stt = new T_tch;
       stt->t_numtache = i;
       stt->t_design = av->Cells[2][i];
       strcpy(tmp,av->Cells[3][i].c_str());
       a=atoi(tmp);
       if (a==0) {a=1; av->Cells[3][i]=0; }
       stt->t_duree =  a;
       stt->t_crit[0] = av->Cells[41][i];
       stt->t_crit[1] = av->Cells[42][i];
       stt->t_crit[2] = av->Cells[43][i];
       stt->t_crit[3] = av->Cells[44][i];
       stt->t_crit[4] = av->Cells[45][i];
       stt->t_crit[5] = av->Cells[46][i];
       stt->t_crit[6] = av->Cells[47][i];
       stt->t_crit[7] = av->Cells[48][i];

       stt->t_libcrit[0] = av->Cells[51][i];
       stt->t_libcrit[1] = av->Cells[52][i];
       stt->t_libcrit[2] = av->Cells[53][i];
       stt->t_libcrit[3] = av->Cells[54][i];
       stt->t_libcrit[4] = av->Cells[55][i];
       stt->t_libcrit[5] = av->Cells[56][i];
       stt->t_libcrit[6] = av->Cells[57][i];
       stt->t_libcrit[7] = av->Cells[58][i];

       S1 = av->Cells[51][i];
       S2 = av->Cells[52][i];
       S3 = av->Cells[53][i];

       TSL->AddObject(AnsiString(i),(TObject *) stt); //TObject *stt);
       }
   }

}

// Copy
void __fastcall TplStandard::CopyCritTSL_AV()
{
 int i,k,cnt; int nt; AnsiString S;
 cnt=TSL->Count;
 for (i=0;i<cnt;i++)
    {
     stt = (T_tch *) TSL->Objects[i];
        for (k=0;k<3;k++)   // 3 criteres
          {
           nt = stt->t_numtache;
           S = stt->t_crit[k];
           av->Cells[41+k][nt] = S;

           S = stt->t_libcrit[k];
           av->Cells[51+k][nt] = S;
          }



    }
    AdvStringGrid1->Refresh();


}


void __fastcall TplStandard::btSaveAffectClick(TObject *Sender)
{

 SaveCriteres(true);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btResetClick(TObject *Sender)
{
 Clean_Grid(asTri,2);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Planning_Defaut()
{
 //int count,
 int i,nt;
 char tmp[250];

 AnsiString Val;

 /*
 Load_Grille_Criteres();
 Clean_Grid(asTri,2);
 count = asAvanc->RowCount;
 if (count == 2) return;
 for (i=1;i<count;i++)
  {
   strcpy(tmp,asAvanc->Cells[1][i].c_str());    // Avanc
   nt=atoi(tmp);
   Add_Row(i,nt);    // ajoute toutes les donn�es disponibles
  }
*/

/*
asTri->SortIndexes->Clear();
asTri->SortIndexes->Add(1);
asTri->SortIndexes->Add(2);
asTri->SortIndexes->Add(3);
asTri->SortIndexes->Add(4);
asTri->SortIndexes->Add(5);
asTri->SortIndexes->Add(6);
asTri->SortIndexes->Add(7);
asTri->SortIndexes->Add(8);
asTri->QSortIndexed();
*/

asTri->QSort();

// Graph_Default('L');
// count = 0;
}


void __fastcall TplStandard::btToutClick(TObject *Sender)
{
 Planning_Defaut();
}


int __fastcall TplStandard::Add_Row(int row,int nt)
{
 int r,i,j,ncol,indx;
 int count;
 if (row>1) asTri->RowCount++;
 r=asTri->RowCount-1;

 indx=exist_tache(nt,nb_taches);
 if (indx==0) return 0;

/* asTri->Cells[0][r] = "N";  // type
 asTri->Cells[20][r] = asAvanc->Cells[1][row];  // ntache
 asTri->Cells[21][r] = asAvanc->Cells[2][row];  //  design
 asTri->Cells[22][r] = asAvanc->Cells[4][row];
 // date de but reel et fin reel
 asTri->Cells[28][r] = asAvanc->Cells[3][row];  // Reel
 asTri->Cells[29][r] = asAvanc->Cells[5][row];  // Reel
 asTri->Cells[32][r] = asAvanc->Cells[15][row];  // Marge
*/

 asTri->Cells[0][r] = " ";  // type
 count = av->ColCount;

 for (j=1;j<count;j++)  asTri->Cells[j][r]=av->Cells[j][row];

/*
 asTri->Cells[20][r] = asAvanc->Cells[1][row];  // ntache
 asTri->Cells[21][r] = asAvanc->Cells[2][row];  //  design
 asTri->Cells[22][r] = asAvanc->Cells[4][row];
 // date de but reel et fin reel
 asTri->Cells[28][r] = asAvanc->Cells[3][row];  // Reel
 asTri->Cells[29][r] = asAvanc->Cells[5][row];  // Reel
 asTri->Cells[32][r] = asAvanc->Cells[15][row];  // Marge
*/



/* //
 for (i=1;i<=8;i++)
   {
    ncol = i*2+1;
    asTri->Cells[i+1][r] = asCrit->Cells[ncol][row];   //
    ncol++;
    asTri->Cells[i+10][r] = asCrit->Cells[ncol][row];   // libelle
   }
*/

return 0;
}

//---------------------------------------------------------------------------

void __fastcall TplStandard::btTriClick(TObject *Sender)
{

asTri->SortIndexes->Clear();
asTri->SortIndexes->Add(41);
asTri->SortIndexes->Add(42);
asTri->QSortIndexed();
asTri->QSortIndexed();

//asTri->QSort();
}

//---------------------------------------------------------------------------

void __fastcall TplStandard::btPrepClick(TObject *Sender)
{
 //Prepare_Graphic(P_MODE);
}
//---------------------------------------------------------------------------


int __fastcall TplStandard::RechercheCol(char *str)
{
 int col,i;
 col=0;
 for (i=0;i<CNT_CHAMPS;i++)
   {
    if (strcmp(libchamp[i],str)==0)
     { col = atoi(cln[i]);
       strcpy(CONTROL,cnt[i]);
       break; }
   }
 return col;
}


void __fastcall TplStandard::ZoomCopy0(TImage *i,int X,int Y)
{

TRect Dst,Src;
int imx; int imy;
int ext;

if (!imP) return;
pc=0;
imx=imP->Width;
imy=imP->Height;

ext = (11-ZoomP0->Position)*100;
rx1= (imx * X / i->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }
ry1= (imy * Y / i->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }
LAST_RX1[pc] = rx1;
LAST_RX2[pc] = rx2;
LAST_RY1[pc] = ry1;
LAST_RY2[pc] = ry2;
LAST_TYPE[pc] = 'P';  // portrait

LAST_ZOOM[pc] = ZoomP0->Position;

Dst = Rect(0,0,imGantt->Width,imGantt->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imGantt->Canvas->CopyRect(Dst,imP0p->Canvas,Src);
Origin_Image=i;

}



void __fastcall TplStandard::ZoomCopy1(TImage *i,int X,int Y)
{

TRect Dst,Src;
int imx; int imy;
int ext;

if (!imP) return;
pc=1;
imx=imP->Width;
imy=imP->Height;
///   imPort is assigned to Image im !!!!!


ext = (11-ZoomP1->Position)*100; //100;
rx1= (imx * X / i->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }
ry1= (imy * Y / i->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }
LAST_RX1[pc] = rx1;
LAST_RX2[pc] = rx2;
LAST_RY1[pc] = ry1;
LAST_RY2[pc] = ry2;
LAST_TYPE[pc] = 'P';  // portrait

LAST_ZOOM[pc] = ZoomP1->Position;

Dst = Rect(0,0,imP1Screen->Width,imP1Screen->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imP1Screen->Canvas->CopyRect(Dst,imP1p->Canvas,Src);
Origin_Image=i;

}


void __fastcall TplStandard::ZoomCopy2(TImage *i,int X,int Y)
{

TRect Dst,Src;
int imx; int imy;
int ext;

if (!imP) return;
imx=imP->Width;
imy=imP->Height;
///   imPort is assigned to Image im !!!!!

pc=2;

ext = (11-ZoomP2->Position)*100;
rx1= (imx * X / i->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }
ry1= (imy * Y / i->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }
LAST_RX1[pc] = rx1;
LAST_RX2[pc] = rx2;
LAST_RY1[pc] = ry1;
LAST_RY2[pc] = ry2;
LAST_TYPE[pc] = 'P';  // portrait

LAST_ZOOM[pc] = ZoomP2->Position;

Dst = Rect(0,0,imP2Screen->Width,imP2Screen->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imP2Screen->Canvas->CopyRect(Dst,imP2p->Canvas,Src);
Origin_Image=i;

}


void __fastcall TplStandard::ZoomCopy3(TImage *i,int X,int Y)
{

TRect Dst,Src;
int imx; int imy;
int ext;

if (!imP) return;


imx=imP->Width;
imy=imP->Height;
///   imPort is assigned to Image im !!!!!

pc=3;

ext = (11-ZoomP3->Position)*100;
rx1= (imx * X / i->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }
ry1= (imy * Y / i->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }
LAST_RX1[pc] = rx1;
LAST_RX2[pc] = rx2;
LAST_RY1[pc] = ry1;
LAST_RY2[pc] = ry2;
LAST_TYPE[pc] = 'P';  // portrait

LAST_ZOOM[pc] = ZoomP3->Position;

Dst = Rect(0,0,imP3Screen->Width,imP3Screen->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imP3Screen->Canvas->CopyRect(Dst,imP3p->Canvas,Src);
Origin_Image=i;

}

void __fastcall TplStandard::ZoomCopy4(TImage *i,int X,int Y)
{

TRect Dst,Src;
int imx; int imy;
int ext;

if (!imP) return;
pc=4;
imx=imP->Width;
imy=imP->Height;
///   imPort is assigned to Image im !!!!!

ext = (11-ZoomR1->Position)*100;
rx1= (imx * X / i->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }
ry1= (imy * Y / i->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }
LAST_RX1[pc] = rx1;
LAST_RX2[pc] = rx2;
LAST_RY1[pc] = ry1;
LAST_RY2[pc] = ry2;
LAST_TYPE[pc] = 'P';  // portrait

LAST_ZOOM[pc] = ZoomR1->Position;

Dst = Rect(0,0,imR1Screen->Width,imR1Screen->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imR1Screen->Canvas->CopyRect(Dst,imR1p->Canvas,Src);
Origin_Image=i;

}

/*
void __fastcall TplStandard::ZoomCopy5(TImage *i,int X,int Y)
{

TRect Dst,Src;
int imx; int imy;
int ext;

if (!imP) return;
pc=5;
imx=imP->Width;
imy=imP->Height;
///   imPort is assigned to Image im !!!!!

ext = (11-ZoomR2->Position)*100;
rx1= (imx * X / i->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }
ry1= (imy * Y / i->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }
LAST_RX1[pc] = rx1;
LAST_RX2[pc] = rx2;
LAST_RY1[pc] = ry1;
LAST_RY2[pc] = ry2;
LAST_TYPE[pc] = 'P';  // portrait

LAST_ZOOM[pc] = ZoomR2->Position;

Dst = Rect(0,0,imR2Screen->Width,imR2Screen->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imR2Screen->Canvas->CopyRect(Dst,imR2p->Canvas,Src);
Origin_Image=i;

}
*/



void __fastcall TplStandard::imP1pMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 //TPrinter *pt = Printer();
 //pt->Orientation = poPortrait;
 CoefFormat = ComputeFormat(edAjust);
 Label116->Caption = "Mode Portrait";
 if (P_MODE[pc]=='L') { P_MODE[pc]='P'; RAZ(); Prepare_Draw_Planning(); }
 ZoomCopy1(imP1p,X,Y);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::imP1lMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  //TPrinter *pt = Printer();
  //pt->Orientation = poLandscape;
 CoefFormat = ComputeFormat(edAjust);
 Label116->Caption = "Mode Paysage";
 if (P_MODE[pc]=='P') { P_MODE[pc]='L'; RAZ(); Prepare_Draw_Planning(); }
 ZoomCopy1(imP1l,X,Y);

}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------


void __fastcall TplStandard::btPrP1Click(TObject *Sender)
{
//  SelectPrinter(0,2,imP1p,imP1l);  //$$$$
CoefFormat=ComputeFormat(edAjust);
// ImprimeDirect(2,imP1p,imP1l);
Prepare_Draw_Planning();
ImprimeDirect(2,imP1p,imP1l);

}

 float __fastcall TplStandard::ComputeFormat(TEdit *ed)
 {
  char tmp[120];
  float a;   char *p;

 strcpy(tmp,ed->Text.c_str());
 p = strstr(tmp,",");
 while (p)
   {
    *p = '.';
    p=strstr(tmp,",");
   }
 a = atof(tmp);

 if (a<0.5 || a>10.0)
   {
    Application->MessageBoxA("Le coefficient doit �tre compris entre 0.5 et 10.0","Ecoplanning",MB_OK);
    a = 1.0;
   }
 return a;
}

void __fastcall TplStandard::btRetPlanning1Click(TObject *Sender)
{
 int a;
 char x_ghost_prep[500];

 pc=1;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
 TPrepPlanning *PrepPlanning;
 pc=1;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 645;
 //PrepPlanning->Width=748;
 PrepPlanning->ShowModal();
 delete PrepPlanning;
 a=0;
 }
//---------------------------------------------------------------------------

float __fastcall TplStandard::AnsiToFloat(AnsiString str)
{
 char tmp[200]; char *p;
 float f;
 strcpy(tmp,str.c_str());

 p=strchr(tmp,',');
 if (p) *p='.';
 f=atof(tmp);
 return f;

}

int   __fastcall TplStandard::AnsiToInt(AnsiString str)
{
 char tmp[200];
 int i;
 strcpy(tmp,str.c_str());
 i=atoi(tmp);
 return i;

}


void __fastcall TplStandard::RAZ()
{
  AnsiString Debug;
  /*
  switch (pc)
   {
    case 0: { iPort=imP0p; iScreen=imGantt; break; }
    case 1: { iPort=imP1p; iLand=imP1l; iScreen=imP1Screen;  break; }
    case 2: { iPort=imP2p; iLand=imP2l; iScreen=imP2Screen;  break; }
    case 3: { iPort=imP3p; iLand=imP3l; iScreen=imP3Screen;  break; }
    case 4: { iPort=imR1p; iScreen=imR1Screen; break;  }
    case 5: { iPort=imR2p; iScreen=imR2Screen; break; }
    default : break;
   }
  */
 Debug = P_FIELD[pc][0];

 Remplir_asTri();
 // Remplir_AV1();  deja fait dans Attrib_Tri
 Attrib_Tri();

 // selection des TImages
 Clean_Grid(asT,2);

}


void __fastcall TplStandard::Prepare_Draw_Rapport(int npage)
{
 char tmp[100];
  if (NB_TACHES==0) return;

  TPrinter *pt = Printer();
  if (pc>=0) pt->Orientation = poPortrait;   // pas de landscape pour rapports
   P_H[pc] = PAGE_HEIGHT;
   P_W[pc] = PAGE_WIDTH;

  if (imP) delete imP;
  imP = new Graphics::TBitmap();
  imP->PixelFormat =pf8bit;
  imP->Height= PAGE_HEIGHT;
  imP->Width=  PAGE_WIDTH;

  TRect Diagram = Rect(0,0, imP->Width,imP->Height);
  imP->Canvas->Brush->Color = clWhite; // BgColor;
  imP->Canvas->FillRect(Diagram);

  switch (pc)
    {
     case 4 : Clear_Diagram(imR1p); break;
     //case 5 : Clear_Diagram(imR2p); break;
    }

  Clean_Grid(asT,2);
  Prepare_Variables();

  Prepare_Ruptures();
  Prepare_Colonnes();
  Prep_Rapport(npage);

  Draw_Planning(imP);
  if (pc==4) imR1p->Picture->Assign(imP);
  // if (pc==5) imR2p->Picture->Assign(imP);
  /**
  PAGE_RAPPORT[pc] = 1;
  if (pc==4)
    {
     Trace_Page_Rapport(PAGE_RAPPORT[pc],imR1p);
     sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
     Label76->Caption = AnsiString(tmp);
    }
  if (pc==5)
    {
     Trace_Page_Rapport(PAGE_RAPPORT[pc],imR2p);
     sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
     Label84->Caption = AnsiString(tmp);
    }
 */

}


void __fastcall TplStandard::Prepare_Draw_Planning()  // main routine  lance tri et filtre
{
  TCursor oldCursor;
  TRect Src,Dst;
  char dt1[12],dt2[12];
  int dist;   char tmp[100];

  // if (NB_TACHES==0) return;
  if (strcmp(P_NOM[pc],"") == 0) return;

   oldCursor = Screen->Cursor;
  Screen->Cursor=crHourGlass;

  TPrinter *pt = Printer();

   if (P_MODE[pc] == 'L') pt->Orientation = poLandscape;
   else pt->Orientation = poPortrait;

   /*
   PAGE_HEIGHT = pt->PageHeight;
   PAGE_WIDTH = pt->PageWidth;
   */

   P_H[pc] = PAGE_HEIGHT;
   P_W[pc] = PAGE_WIDTH;
  if (pc==0) P_GRILLE[0]='N';

  if (imP) delete imP;
  imP = new Graphics::TBitmap();
  imP->PixelFormat =pf8bit;
  imP->Height= P_H[pc]; //Printer()->PageHeight;
  imP->Width=  P_W[pc];  //Printer()->PageWidth;
  TRect Diagram = Rect(0,0, imP->Width,imP->Height);
  imP->Canvas->Brush->Color = clWhite; // BgColor;
  imP->Canvas->FillRect(Diagram);


  if (pc==0)
   {  m_automatic[0]='X';
      m_automatic[0]=' ';
      P_DETAIL[pc]=1;
      sprintf(dt1,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev%100);
      sprintf(dt2,"%02d/%02d/%02d",JourFin,MoisFin,AnFin%100);
      comp_dist_dates(dt1,dt2,1,0);
      if (NB1 <100) {
         P_CYN[0][0] = "Oui";
         P_CYN[0][1] = "Oui";
         P_CYN[0][2] = "Oui";
         P_CYN[0][3] = "Non";
         }
      else if (NB1 < 500)
         {
          P_CYN[0][0] = "Non";
          P_CYN[0][1] = "Oui";
          P_CYN[0][2] = "Oui";
          P_CYN[0][3] = "Non";

         }
      else
         {
          P_CYN[0][0] = "Non";
         P_CYN[0][1] = "Non";
         P_CYN[0][2] = "Oui";
         P_CYN[0][3] = "Non";

         }

   }


 //  if (pc==3) Remplir_asTri_Avanc();
  Clean_Grid(asT,2);
  Prepare_Variables();

  if (pc>=0 && pc<4 && P_ENTETE[pc]=='O') Trace_Entete();
  Y0=Y0+1;
  Insert_G('Y',"LH",D_LEFT[pc],Y0,D_RIGHT[pc],Y0+1,"DT",TColor(0),TColor(0),1,0,"","X","X","");
  //Y0=Y0+1;

  Prepare_Ruptures();
  Prepare_Colonnes();
  Prepare_Enveloppes();
  Prepare_Hauteur();   //  Calcul hauteur entre traces
  // Trace une ligne Horizontale
  if (pc<4 && P_CALHAUT[pc]=='O') Trace_Calendrier();  // cal et entete colonnes
  INITY0=Y0;
  Trace_Taches();


  // P_LIENS[pc][2]="Oui"; P_LIENS[pc][3]="Oui";

  //sprintf(tmp,"PC = %d P_DETAIL %d P_LIENS(2) %s P_LIENS(3 = %s) ",pc,P_DETAIL[pc], P_LIENS[pc][2],P_LIENS[pc][3]);
  //Application->MessageBox(tmp,"ECOPL TRACE BRISEES",MB_OK);

  if (pc<4 && P_DETAIL[pc]==1)
    {

     if (P_LIENS[pc][0]=="Oui" || P_LIENS[pc][1]=="Oui") Trace_Liens_Prev();    // 0 pour previsionnel critique
     if (P_TYPE[pc] == 'R')
       {
         if (P_LIENS[pc][2]=="Oui" || P_LIENS[pc][3]=="Oui") Trace_Liens_Reel();    // 2 pour reel critique
        }
    }


  // +++++++++ if (pc==3) Trace_Brisees();

  if (pc<4 && P_CALBAS[pc]=='O')Trace_CalBas();
  Clean_Footer();
  if (pc >0)
   {
     if (pc<4 && P_PIED[pc]=='O')Trace_Pied();
     if (pc<4 && P_MARGES[pc]=='O') Trace_Marges();
     if (pc<4 && P_CADRE[pc]=='O') Trace_Cadre();
   }


  Draw_Planning(imP);

  switch(pc)
    {
     case 0:
        imP0p->Picture->Assign(imP);
        break;
     case 1:
        imP1p->Picture->Assign(imP);
        imP1l->Picture->Assign(imP);
        //Border_Diagram(imP1p);
        //Border_Diagram(imP1l);


       break;
     case 2:
        imP2p->Picture->Assign(imP);
        imP2l->Picture->Assign(imP);
       break;

     case 3:
        imP3p->Picture->Assign(imP);
        // imLand->Picture->Assign(imP);
       break;

     }
  Screen->Cursor=oldCursor;
  //  m_automatic[pc]=' ';
}


void __fastcall TplStandard::Trace_Brisees()
{
  int color,jcal,dist1,dist2,dist3,dist4;
 char tmp[200],d1[15],d2[15];  int x1,x2,x3,x4;
 int a1,a2,m1,m2,j1,j2; int m;
 int jx; int interval; int distbrisee;
 int step,k,xt,yt,nbtaches,it,y1,y2;
 AnsiString Texte;  char dtav[20];
 char dt1[20],dtAvanc[20],dtPrev[20];
 int DistCongesMoinsAvanc;
 int DistCongesPlusAvanc;
 int base;   int H2;
 int SY;  TColor Col;
 float EP;
 int EPP;

 // nbt = sg->RowCount - 1;
 nbtaches = asAvanc->RowCount - 1;  //nbt/2;
 Y0=INITY0;
 SY=P_CDVV[pc];
base = Compute_Base();
H2= 20;

//  CntAVBefore=1;
for (m=0; m<CntAVBefore; m++)
     {
      Y0=INITY0;
      strcpy(dtav,TableDatesAV[m]);
      dt1[0]=dtav[6]; dt1[1]=dtav[7];dt1[3]=dtav[3];
      dt1[4]=dtav[4];dt1[6]=dtav[0]; dt1[7]=dtav[1];
      dt1[2]='-';dt1[5]='-';dt1[8]=0;
      strcpy(dtAvanc,dt1);
      sprintf (dtPrev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
      comp_dist_dates(dtPrev,dtAvanc,1,0);
      DistAvanc = NB1;
      DistCongesPlusAvanc = avance_conges(DistAvanc,1);
      Col = TColor(C_BRISEES[pc][m+1]);
      EP = E_BRISEES[pc][m+1]*10.0;
      EPP = EP;

      if (L_BRISEES[pc][m+1])
        {
         for (it=1;it<=nb_taches;it++)
         {
          strcpy(d1,asAvanc->Cells[8][it].c_str()); // date prev
          strcpy(d2,asAvanc->Cells[10][it].c_str()); // date fin  prev
          d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
          dist1 = convert_date(a1+2000,m1,j1);
          d2[2]=0; j2=atoi(d2);  d2[5]=0; m2=atoi(d2+3);  d2[8]=0; a2=atoi(d2+6);
          dist2 = convert_date(a2+2000,m2,j2);
          strcpy(d1,asAvanc->Cells[3][it].c_str()); // date deb reel
          strcpy(d2,asAvanc->Cells[5][it].c_str()); // date fin reel
          d1[2]=0; j1=atoi(d1);  d1[5]=0; m1=atoi(d1+3);  d1[8]=0; a1=atoi(d1+6);
          dist3 = convert_date(a1+2000,m1,j1);
          d2[2]=0; j2=atoi(d2);  d2[5]=0; m2=atoi(d2+3);  d2[8]=0; a2=atoi(d2+6);
          dist4 = convert_date(a2+2000,m2,j2);



          y1 = Y0; //  + H2*P_CDVV[pc]/100;
          if (dist3<= (DistAvanc-1)  &&  dist4>=(DistAvanc-1))
           {
             distbrisee = dist4 - DistCongesPlusAvanc + 1;
           }
         else distbrisee = 0;
         if (dist2==dist4) distbrisee = 0;

         asAvanc->Cells[22][it]=AnsiString(distbrisee);

         x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];
         y1= Y0;
         Insert_G('Y',"MO",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");

         // x = DistAvanc*spx+1;
         x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];
         y1 = Y0 + SY/3;
         Insert_G('Y',"LI",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");


         if (distbrisee !=0)
          x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist2-distbrisee-IXDEB,1)/P_MAXDIST[pc];
         else
          x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];

       y1=Y0 + 2*SY/3;  //H2*P_CDVV[pc]/(100*3);
       Insert_G('Y',"LI",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");

       x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];
       y1= Y0 + SY; // y1+H2*P_CDVV[pc]/(100*3);
       Insert_G('Y',"LI",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");
       Y0 = Y0+P_CDVV[pc];
      } // nbt
     } //L_BRISEES
   }  // for m;


}

void __fastcall TplStandard::Prepare_Variables()  // plc=planning en cours
{
 float valf;
 int cnt,i,mem;
 char dt1[15];
 int j1,m1,a1,dist; int distfin;
 char tmp[250];
 int HP,maxhh;
 AnsiString font,txt50; int ht;
 bool IX_OK;

  IXDEB=0;
  IXFIN=0;

 // Lecture de IXDEB et IXFIN dans edDEB, edFIN

  // Button26Click(NULL);
 if (Update_IX()) IX_OK=true; else { IXDEB=0; IX_OK=false; }

 mem=0;
 cnt = asTri->RowCount;  P_MAXDIST[pc]=1;   // valeur par defaut

 for (i=0;i<MAX_COL;i++) P_TRICOL[pc][i]=0;

 for (i=1;i<cnt;i++)
  {
   strcpy(dt1,asTri->Cells[5][i].c_str());  // date fin previsionnelle
   dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
   dist = convert_date(a1+2000,m1,j1);
   if (dist>P_MAXDIST[pc]) { P_MAXDIST[pc]=dist; mem=i; }
  }

if (IX_OK == true) P_MAXDIST[pc]=(IXFIN1-IXDEB)*105/100;

/////  Update_PCJT(IXFIN);

 font = PF[pc][21]; ht=PH[pc][21];
 txt50 = "AbcdefghijklmnopqrstAbcdefghi";
// Compute_LH(font,asTri->Cells[2][mem],ht,"Standard","Standard");
 Compute_LH("Arial",txt50,16,"Standard","Gras");
 P_LASTTEXT[pc] = L;

 // Les Variables Marges et cadre
 valf=AnsiToFloat(P_MH1[pc]);
 P_MARGH1[pc]= (int)(valf*COEF);
 valf=AnsiToFloat(P_MH2[pc]);
 P_MARGH2[pc]= (int)(valf*COEF);
 valf=AnsiToFloat(P_MV1[pc]);
 P_MARGV1[pc]= (int)(valf*COEF);
 valf=AnsiToFloat(P_MV2[pc]);
 P_MARGV2[pc]= (int)(valf*COEF);

 valf=AnsiToFloat(P_ELC[pc]);
 P_ELCF[pc]= (int)(valf*COEFMM);  // epaisseur en mm
 P_CLCC[pc] = AnsiToInt(P_CLC[pc]);

 D_LEFT[pc]= P_MARGH1[pc] + P_ELCF[pc];
 D_RIGHT[pc]= P_W[pc] - P_MARGH2[pc] - P_ELCF[pc];
 D_TOP[pc] = P_MARGV1[pc] + P_ELCF[pc];
 D_BOTTOM[pc]= P_H[pc] - P_MARGV2[pc] - P_ELCF[pc];


 HET =0;
 maxhh=0;
 if (PHH[pc][0] > maxhh ) maxhh=PHH[pc][0];
 if (PHH[pc][3] > maxhh) maxhh=PHH[pc][3];
 if (PHH[pc][6] > maxhh) maxhh=PHH[pc][6];
 HET = maxhh;

 maxhh=0;
 if (PHH[pc][1] > maxhh) maxhh=PHH[pc][1];
 if (PHH[pc][4] > maxhh) maxhh=PHH[pc][4];
 if (PHH[pc][7] > maxhh) maxhh=PHH[pc][7];
 HET += maxhh;
 HET += maxhh+10;
if (pc==0) HET=0;


 X0=D_LEFT[pc]; Y0=D_TOP[pc];

 H_CAL1[pc]=PHH[pc][9];
 H_CAL2[pc]=PHH[pc][10];

 if (P_ECHH[pc] == "O") Hauteur_CalHaut = H_CAL1[pc];
 if (P_ECHB[pc] == "O") Hauteur_CalHaut +=  H_CAL2[pc];
 Hauteur_CalHaut += 4;
 if (P_CALHAUT[pc] != 'O') Hauteur_CalHaut = 0;

 Hauteur_CalBas = Hauteur_CalHaut;
 if (P_CALBAS[pc] != 'O') Hauteur_CalBas=0;
 Hauteur_Pied=0;
 HP=0;
 if (PHH[pc][25]>HP && PT[pc][25]!="") HP=PHH[pc][25];
 if (PHH[pc][28]>HP && PT[pc][28]!="") HP=PHH[pc][28];
 if (PHH[pc][31]>HP && PT[pc][31]!="") HP=PHH[pc][31];
 Hauteur_Pied += HP;
 HP=0;
 if (PHH[pc][26]>HP && PT[pc][26]!="") HP=PHH[pc][26];
 if (PHH[pc][29]>HP && PT[pc][29]!="") HP=PHH[pc][29];
 if (PHH[pc][32]>HP && PT[pc][32]!="") HP=PHH[pc][32];
 Hauteur_Pied += HP;
 HP=0;
 if (PHH[pc][27]>HP && PT[pc][27]!="") HP=PHH[pc][27];
 if (PHH[pc][30]>HP && PT[pc][30]!="") HP=PHH[pc][30];
 if (PHH[pc][33]>HP && PT[pc][33]!="") HP=PHH[pc][33];
 Hauteur_Pied += HP;

 if (P_PIED[pc] !='O') Hauteur_Pied=0;

}

// ****************************************
//
//*****************************************
/*
void __fastcall TplStandard::Trace_Entete()  // plc=planning en cours
{
 int n,x1,y1,x2,y2,maxh, maxhh;
 int Y0_Init;
 char tmp[50];
 AnsiString IG;
 int Max_Y0;

  // Calcul Hauteur  Maximum pour la ligne


 maxhh=maxh=0;
 if (PHH[pc][0] > maxhh) maxhh=PHH[pc][0];
 if (PHH[pc][3] > maxhh) maxhh=PHH[pc][3];
 if (PHH[pc][6] > maxhh) maxhh=PHH[pc][6];
 if (PH[pc][0] > maxh) maxh=PH[pc][0];
 if (PH[pc][3] > maxh) maxh=PH[pc][3];
 if (PH[pc][6] > maxh) maxh=PH[pc][6];

 if (maxhh<maxh) maxhh=maxh +10;

 X0=D_LEFT[pc] + 20;
 Y0_Init=Y0;
 Max_Y0 = Y0;


 Y0=Y0+(maxhh-maxh)/2;




 //  Trace Colonnes par Colonnes (PAS PAR LIGNES)// 1ere Ligne
 n=0;
 x1 = ComputeX1(n);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
    PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=3;
 IG = PN[pc][n];
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n);  //P_W[pc]/2 -L/2;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=6;
 IG = PN[pc][n];
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n); // P_W[pc] - P_MARGH2[pc] - L - 30;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 Y0=Y0 + maxhh;

 // 2eme Ligne
 maxhh=maxh=0;
 if (PHH[pc][1] > maxhh) maxhh=PHH[pc][1];
 if (PHH[pc][4] > maxhh) maxhh=PHH[pc][4];
 if (PHH[pc][7] > maxhh) maxhh=PHH[pc][7];
 if (PH[pc][1] > maxh) maxh=PH[pc][1];
 if (PH[pc][4] > maxh) maxh=PH[pc][4];
 if (PH[pc][7] > maxh) maxh=PH[pc][7];

 if (maxhh<maxh) maxhh=maxh +10;
 X0=D_LEFT[pc] + 20;
 Y0=Y0+(maxhh-maxh)/2;

 //



 n=1;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],
   PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n); //X0+PR[pc][n];
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
    0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=4;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n); //P_W[pc]/2 -L/2;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=7;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n); //P_W[pc] - P_MARGH2[pc] - L -30;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);

 Y0=Y0+maxhh;

 // 3eme ligne
 maxhh=maxh=0;
 if (PHH[pc][2] > maxhh) maxhh=PHH[pc][2];
 if (PHH[pc][8] > maxhh) maxhh=PHH[pc][8];
 if (PH[pc][2] > maxh) maxh=PH[pc][2];
 if (PH[pc][8] > maxh) maxh=PH[pc][8];

 if (maxhh<maxh) maxhh=maxh +10;
 X0=D_LEFT[pc]+20;
 Y0=Y0+(maxhh-maxh)/2;

 n=2;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n); //X0+PR[pc][n];
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=8;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n); //P_W[pc] - P_MARGH2[pc] - L -30;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
  0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 Y0=Y0+maxhh+20;

 Hauteur_Entete = Y0_Init - Y0;

}

*/

void __fastcall TplStandard::Trace_Entete()  // plc=planning en cours
{
 int n,x1,y1,x2,y2,hh;
 int Y0_Init;
 char tmp[150];
 AnsiString IG;
 int Max_Y0;
 int blogo,width,height;
 Graphics::TBitmap *Bitmap;
  // Calcul Hauteur  Maximum pour la ligne

 height = 0;
 blogo = 0;
 if (LOGONAME[pc] != "")
 {
   // check if logo exists
   if (FileExists(LOGONAME[pc]))
     {
       Bitmap = new Graphics::TBitmap;
       Bitmap->LoadFromFile(LOGONAME[pc].c_str());
       width = Bitmap->Width;
       height = Bitmap->Height;
       X0=D_LEFT[pc] + width + 30;
       if (LOGOPOS[pc]=="G") blogo = 1;
       else blogo = 2;
     }
 }
 else X0=D_LEFT[pc] + 20;
 Y0_Init=Y0;
 Max_Y0 = Y0;


 //  Trace Colonnes par Colonnes (PAS PAR LIGNES)// 1ere Ligne
 n=0; Y0=Y0_Init;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10;
 Y0 = Y0 + H; //PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 n=1;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10;
 Y0 = Y0 + H; //PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 n=2;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10;
 Y0 = Y0 + H; //PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;


 // Colonne du centre

 n=3; Y0=Y0_Init;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10;
 Y0 = Y0 + H; //PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 n=4;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10; Y0 = Y0 + H; // PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 n=5;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10; Y0 = Y0 + H; //PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 // Colonnne de Droite

 n=6; Y0=Y0_Init;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10;Y0 = Y0 + H; // PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 n=7;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10;Y0 = Y0 + H; //PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 n=8;
 x1 = ComputeX1(n,blogo,width);
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('E',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
           PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 H = H*13/10;Y0 = Y0 + H; //PHH[pc][n];
 if (Y0 > Max_Y0) Max_Y0 = Y0;

 if (LOGONAME[pc] != "" && blogo!=0)
 {
  if (LOGOPOS[pc]=="G")  x1= D_LEFT[pc] + 15;
     else x1 = D_RIGHT[pc] - width - 15;

  Insert_G('Y',"GR",x1,Y0_Init + 10,0,0,"GR",
       TColor(0),TColor(0),1,0,"","X","X",LOGONAME[pc]);

 }

 if (height > Max_Y0) Max_Y0 = height + 5;
 Hauteur_Entete = Y0_Init + Max_Y0;
 Y0 = Y0_Init + Max_Y0;

}




int __fastcall TplStandard::ComputeX1(int n,int bl,int w)
{
int Gauche[3],Centre[3],Droite[3];
 int temp;
 int Lex;
 AnsiString Cadr;

 temp = P_W[pc] - 2*D_LEFT[pc];

 if (bl==1) Gauche[0]= D_LEFT[pc]+w+ 30; else Gauche[0]= D_LEFT[pc]+20;
 if (bl==1) Gauche[1]= D_LEFT[pc] + w + 30+ temp/8; else Gauche[1]= D_LEFT[pc] + temp/8;
 if (bl==1) Gauche[2]= D_LEFT[pc]+ w + 30  + temp/4-20; else Gauche[2]= D_LEFT[pc] + temp/4-20;

 Centre[0]= D_LEFT[pc] + temp/4+ 10;
 Centre[1]= D_LEFT[pc] + temp/2;
 Centre[2]= Centre[1] +  temp/4 - 10;

  Droite[0]= Centre[2]+20;
  Droite[1]= Centre[2] + temp/8;
  Droite[2]= Centre[2] + temp/4 - 20;



 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 switch (n)
  {
   case 0:
   case 1:
   case 2:
   Cadr = PJH[pc][n];
   if (PJH[pc][n] == "Gauche") Lex=Gauche[0];
   else if (PJH[pc][n] == "Centre") Lex=Gauche[1]-L/2;
   else if (PJH[pc][n] == "Droite") Lex=Gauche[2]-L;
   else Lex = Gauche[0];
   break;

   case 3:
   case 4:
   case 5:
   Cadr = PJH[pc][n];
   if (PJH[pc][n] == "Gauche") Lex=Centre[0];
   else if (PJH[pc][n] == "Centre") Lex=Centre[1]-L/2;
   else if (PJH[pc][n] == "Droite") Lex=Centre[2]-L;
   else Lex = Centre[0];
   break;

   case 6:
   case 7:
   case 8:
   Cadr = PJH[pc][n];
   if (PJH[pc][n] == "Gauche") Lex=Droite[0];
   else if (PJH[pc][n] == "Centre") Lex=Droite[1]-L/2;
   else if (PJH[pc][n] == "Droite") Lex=Droite[2]-L;
   else  Lex = Droite[0];
   break;

   case 25:
   case 26:
   case 27:
   Cadr = PJH[pc][n];
   if (PJH[pc][n] == "Gauche") Lex=Gauche[0];
   else if (PJH[pc][n] == "Centre") Lex=Gauche[1]-L/2;
   else if (PJH[pc][n] == "Droite") Lex=Gauche[2]-L;
   else Lex = Gauche[0];
   break;

   case 28:
   case 29:
   case 30:
   Cadr = PJH[pc][n];
   if (PJH[pc][n] == "Gauche") Lex=Centre[0];
   else if (PJH[pc][n] == "Centre") Lex=Centre[1]-L/2;
   else if (PJH[pc][n] == "Droite") Lex=Centre[2]-L;
   else Lex=Centre[0];
   break;

   case 31:
   case 32:
   case 33:
   Cadr = PJH[pc][n];
   if (PJH[pc][n] == "Gauche") Lex=Droite[0];
   else if (PJH[pc][n] == "Centre") Lex=Droite[1]-L/2;
   else if (PJH[pc][n] == "Droite") Lex=Droite[2]-L;
   else Lex = Droite[0];
   break;


  }
 return Lex;
}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Prepare_Ruptures()
{
 AnsiString Code;  char AT[500];
 int i,cnt,j,k,x,numcrit;  int cnt_nb_task;
 AnsiString Cr[8],C,Cniv; AnsiString Code1,Code2,Code3;
 int n,crit,col;  char tmp[200],xtmp[200];

 cnt=asTri->RowCount;

 for (i=1;i<MAX_RUPT;i++) {RUPT[i]="";
      for (j=0;j<9; j++)
        { NB_TASK[j][i]=0;
          ENV_MIN[j][i]=10000; ENV_MAX[j][i]=0;
          ENV_D1[j][i]=10000; ENV_D2[j][i]=0;
        }
      }

 NUM_CRIT[pc]=0;

 for (j=1;j<9;j++)
  {
   Code=asTri->Cells[70][1];
   Code=asTri->Cells[71+j-1][1];
   if (asTri->Cells[71+j-1][1]!="") NUM_CRIT[pc]=j; }
 // NUM_CRIT contient l'indice du dernier critere


 numcrit = NUM_CRIT[pc];   // ou 3deux crits par exemple

     for (k=0;k<numcrit;k++)
     {
     // Rechercher Critere en fonction du Libelle
      strcpy(tmp,P_FIELD[pc][k].c_str());
      //
      Code = "";
      INDX=0; cnt_nb_task=0;
      for (i=1;i<cnt;i++)
        {
          strcpy(AT,asTri->Cells[71+k][i].c_str());
          if (strlen(AT) > 20)  asTri->Cells[71+k][i]="";

          if (k==2)
            {
             INDX=INDX;
            }
          Cniv = ChercheCode(k,asTri->Cells[71+k][i]);
          if (k==1) { Cniv = ChercheCode(0,asTri->Cells[71][i]) + "x" + Cniv;  }
          if (k==2) { Cniv = ChercheCode(0,asTri->Cells[71][i]) + "x" + ChercheCode(1,asTri->Cells[71+1][i]) + "x" +Cniv; }
          if (k==3) { Cniv = ChercheCode(0,asTri->Cells[71][i]) + "x" + ChercheCode(1,asTri->Cells[71+1][i]) + "x"
                      + ChercheCode(2,asTri->Cells[71+2][i]) + "x" +Cniv; }

          if (Code !=  Cniv) // asTri->Cells[71+k-1][i])
            {
             INDX++;
             NB_TASK[k][INDX] = i;
             cnt_nb_task++;
            //CTR_NB_TASK[k][INDX]=cnt_nb_task;
            Code = Cniv; //asTri->Cells[71+k-1][i];
            } //for i;
     } // for k;
    INDX++; NB_TASK[k][INDX]=cnt;
    CTR_NB_TASK[k][INDX]=cnt_nb_task;
   }  // for j

  i=0;  //
  while (i<=P_COL[pc])   // P_COL ??
   {
    strcpy(tmp,P_FIELD[pc][i].c_str());
    if (P_ATTR[pc][i] !='Z')
      {
       if (i>7)
         {
           if (P_GRILLE[pc]=='N' || P_GENRE[pc]=='C') P_TRICOL[pc][i]=0;
           else
            {  if (tmp[0]=='*')
               {
                col =  RechercheColCrit(tmp+1);
                if (col==0)
                  { strcpy(xtmp,"Crit�re "); strcat(xtmp,tmp+2);
                    strcat(xtmp," Non Trouv�. Faire un nouveau Trier/Grouper avec vos Crit�res");
                    // Application->MessageBoxA(xtmp,m_ecoplan,MB_OK);
                    col=1;

                  }
               P_TRICOL[pc][i] = col + 50;
               }
               else P_TRICOL[pc][i] = RechercheCol(tmp);
            }
         }
      else
         {
            if (tmp[1]=='*')
              {
               col =  RechercheColCrit(tmp+2);
                if (col==0)
                  {
                    strcpy(xtmp,"Crit�re "); strcat(xtmp,tmp+2);
                    strcat(xtmp," Non Trouv�. Faire un nouveau Trier/Grouper avec vos Crit�res");
                    // Application->MessageBoxA(xtmp,m_ecoplan,MB_OK);
                    col=1;

                  }
              P_TRICOL[pc][i] = col + 50;
              }
              else  P_TRICOL[pc][i] = RechercheCol(tmp+1);
         }

     }
    i++;
   } //  ends while


 // Calcul de nombre de ligne a tracer

  for (i=0;i<8;i++) { Cr[i]=""; P_RUPT[pc][i]=0; }

  for (i=1;i<=cnt;i++)
   { for (crit=0; crit< 8;crit++)
     {
      if (P_ATTR[pc][crit]!='Z')
       {
        n=P_TRICOL[pc][crit];
        if (P_TRICOL[pc][crit] >40) C=asTri->Cells[crit+41][i];
          else C=asTri->Cells[n][i];

        if (C != Cr[crit])
         {
          if (P_ATTR[pc][crit]=='L') P_RUPT[pc][crit]++;
         }
       Cr[crit]=C;
      }
     } // For crit

    } // for i

 for (crit=0;crit<8;crit++) { if (P_RUPT[pc][crit]>1) P_RUPT[pc][crit]--; }
}

 AnsiString __fastcall TplStandard::ChercheCode(int k, AnsiString a)
 {
 char p0[1000],p1[1000],p2[1000],p3[1000];
 char tmp[200];
 char *xp1,*xp2; int result;

 AnsiString CCode;

 strcpy(tmp,a.c_str());
 strcat(tmp,"----");
 strcpy(p0,"");strcpy(p1,"");strcpy(p2,"");strcpy(p3,"");

 xp1=tmp;
 xp2 = strstr(xp1,"-");
 if (xp2)
   { *xp2=0; strcpy(p0,xp1); xp1=xp2+1;
      xp2 = strstr(xp1,"-");
        if (xp2)
          { *xp2=0; strcpy(p1,xp1); xp1=xp2+1;
             xp2 = strstr(xp1,"-");
             if (xp2)
               { *xp2=0; strcpy(p2,xp1); xp1=xp2+1;
                  xp2 = strstr(xp1,"-");
                  if (xp2)
                   { *xp2=0; strcpy(p3,xp1);
                   }
                  else strcpy(p3,xp1);

                }
            else strcpy(p2,xp1);
          }
      else strcpy(p1,xp1);
   }

  if (P_NIV0[pc][k] == 1) CCode = AnsiString(p0); //+ "---";
  if (P_NIV1[pc][k] == 1) CCode = AnsiString(p0)+ "-" + AnsiString(p1); // + "--";
  if (P_NIV2[pc][k] == 1) CCode = AnsiString(p0)+ "-" + AnsiString(p1) + "-" + AnsiString(p2); // + "-";
  if (P_NIV3[pc][k] == 1) CCode = AnsiString(p0)+ "-" + AnsiString(p1) + "-" + AnsiString(p2) + "-" + AnsiString(p3);
  return CCode;
 }

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Prepare_Colonnes()
{
 int i,j,cnt,a;
 AnsiString txt;   char tmp[250];
 int LastPos;
 TColor colbk,colfg;


 int x1,y1,x2,y2;

 cnt = asTri->RowCount;
 if (pc==0)
 { for (i=0;i<MAX_COL;i++) { P_TRICOL[0][i]=0; P_ATTR[0][i]='Z'; }}
 // for (i=0;i<MAX_TCHP;i++) T_TLARG[pc][i] = "0";

 i=0;  //
 P_MAXSIZE[pc][0]=0; P_LC[pc][0]=D_LEFT[pc];

 LastPos = 0;
 P_LC[pc][0] = D_LEFT[pc];
 while (i<=P_COL[pc])   // P_COL ??
   {

    P_MAXSIZE[pc][i]=0;
    strcpy(tmp,P_FIELD[pc][i].c_str());
    if (P_ATTR[pc][i]=='C' && P_TRICOL[pc][i] != 0) // !=0)     //  Donn�es a afficher en colonnes
     {

        if (Check_Automatic())
         {
          for (j=1;j<cnt-1;j++)
           {
             txt = asTri->Cells[P_TRICOL[pc][i]][j];    // $$$$$
             Compute_LH(P_FONT[pc][i],txt,P_HAUT[pc][i],P_ITAL[pc][i],P_BOLD[pc][i]);
             L=L+30;      //  % pour espacement
             if (L>P_MAXSIZE[pc][i]) P_MAXSIZE[pc][i]=L;
           }
          if (i>7) { T_LARG[pc][i] = P_MAXSIZE[pc][i]; P_LARG[pc][i] = P_MAXSIZE[pc][i]; }
            else { T_TLARG[pc][i] = P_MAXSIZE[pc][i];  }
         }  // automat

          else
           { if (i>7) P_MAXSIZE[pc][i] = T_LARG[pc][i];
              else
               { strcpy(tmp,T_TLARG[pc][i].c_str());
                 P_MAXSIZE[pc][i] = atoi(tmp);
               }
           }


           P_LC[pc][i+1]=P_LC[pc][i] + P_MAXSIZE[pc][i] + 2; // P_EPAISSV[pc][i];
           LastPos = P_LC[pc][i] + P_MAXSIZE[pc][i];

           P_LENGTH[pc][i]= P_MAXSIZE[pc][i];
          // trac� des trait separent colonnes
           x1= P_LC[pc][i];

          y1=Y0+1; x2=x1+2; y2= D_BOTTOM[pc];
          Insert_G('Y',"LV",x1,y1,x2,y2,"",TColor(0),TColor(0),P_EPAISSV[pc][i],0,"",'X','X',"");

            if ((P_TRICOL[pc][i]>40) && (P_TRICOL[pc][i]<59))
             {

             y1=Y0+1; y2= D_BOTTOM[pc];
             x2= x1 + P_MAXSIZE[pc][i];  //P_LC[pc][i-1];
             LastPos = x2;
             //x2=x1-1; x1= P_LC[pc][i-1];

             strcpy(tmp,T_CBKG[pc][i].c_str()); colbk=TColor(atoi(tmp));
             Insert_G('Y',"RT",x1,y1,x2,y2,"DT",colbk,colbk,0,0,"",' ',' ',"");
             x1= P_LC[pc][i+1]-1;
             y1=Y0+1; x2=x1+1; y2= D_BOTTOM[pc];
             Insert_G('Y',"LV",x1,y1,x2,y2,"",TColor(0),TColor(0),1,0,"",'X','X',"");
            }
          else
            {
             y1=Y0+1; x2=x1+2; y2= D_BOTTOM[pc];
             Insert_G('Y',"LV",x1,y1,x2,y2,"",TColor(0),TColor(0),1,0,"",'X','X',"");

            }

        }
      else
        {
         P_LC[pc][i+1]=P_LC[pc][i];
         P_LENGTH[pc][i]= 0;
         LastPos = P_LC[pc][i+1];
        }

    i++;
   }

 if (i==0) P_DEBUTGANTT[pc]=D_LEFT[pc];

 P_DEBUTGANTT[pc]=  LastPos; // P_LC[pc][i-1] + P_MAXSIZE[pc][i-1] + P_EPAISSV[pc][i-1]+1;
}
// ****************************************
//
//*****************************************

void __fastcall TplStandard::Format_Calendrier(AnsiString c_format,int c_an,int c_mois, int c_jour)
{
 AnsiString Ldays[8];
 AnsiString Mdays[8];
 AnsiString Sdays[8];
 AnsiString mo[13];
 AnsiString MO[13];
 AnsiString mon[13];
 AnsiString Smo[13];
 TDateTime dtFirst;
 int firstday;
 char tmp[150];
 char trim[30];

 Ldays[1]="Dimanche"; Ldays[2]="Lundi"; Ldays[3]="Mardi"; Ldays[4]="Mercredi";
 Ldays[5]="Jeudi"; Ldays[6]="Vendredi"; Ldays[7]="Samedi";

 Mdays[1]="Dim"; Mdays[2]="Lun"; Mdays[3]="Mar"; Mdays[4]="Mer";
 Mdays[5]="Jeu"; Mdays[6]="Ven"; Ldays[7]="Sam";

 Sdays[1]="D"; Sdays[2]="L"; Sdays[3]="M"; Sdays[4]="M";
 Sdays[5]="J"; Sdays[6]="V"; Sdays[7]="S";

 Smo[1]="J";  Smo[2]="F"; Smo[3]="M";  Smo[4]="A"; Smo[5]="M";  Smo[6]="J";
 Smo[7]="J";  Smo[8]="A"; Smo[9]="S";  Smo[10]="O"; Smo[11]="N";  Smo[12]="D";

 mo[1]="Jan";  mo[2]="Fev"; mo[3]="Mar";  mo[4]="Avr"; mo[5]="Mai";  mo[6]="Jui";
 mo[7]="Jui";  mo[8]="Aou"; mo[9]="Sep";  mo[10]="Oct"; mo[11]="Nov";  mo[12]="Dec";

 MO[1]="Janvier";  MO[2]="F�vrier"; MO[3]="Mars";  MO[4]="Avril"; MO[5]="Mai";  MO[6]="Juin";
 MO[7]="Juillet";  MO[8]="Aout"; MO[9]="Septembre";  MO[10]="Octobre"; MO[11]="Novembre";  MO[12]="D�cembre";

 mon[1]="01";  mon[2]="02"; mon[3]="03";  mon[4]="04"; mon[5]="05";  mon[6]="06";
 mon[7]="07";  mon[8]="08"; mon[9]="09";  mon[10]="10"; mon[11]="11";  mon[12]="12";

 dtFirst=EncodeDate(An,Mois,Jour);
 firstday = dtFirst.DayOfWeek();

 if (c_mois>0 && c_mois <4) strcpy(trim,"1");
 if (c_mois>3 && c_mois <7) strcpy(trim,"2");
 if (c_mois>6 && c_mois <10) strcpy(trim,"3");
 if (c_mois>9 && c_mois <13) strcpy(trim,"4");

 sprintf(tmp,"%02d-%02d",c_jour,c_mois);

 if (c_format=="Vendredi")     strcpy(tmp,Ldays[firstday].c_str());
 else if (c_format=="Ven")          strcpy(tmp,Mdays[firstday].c_str());
 else if (c_format=="V")            strcpy(tmp,Sdays[firstday].c_str());
 else if (c_format=="31/01/05")     sprintf(tmp,"%02d/%02d/%02d",c_jour,c_mois,c_an%100);
 else if (c_format=="31/01")        sprintf(tmp,"%02d/%02d",c_jour,c_mois);
 else if (c_format=="1,2...")       sprintf(tmp,"%d",c_jour);
 else if (c_format=="Jour 1, Jour 2...")  sprintf(tmp,"%s %d",Mdays[firstday].c_str(),c_jour);
 else if (c_format=="Aucun")        strcpy(tmp,"");
 else if (c_format=="31 Janvier 05") sprintf(tmp,"%02d %s %02d",c_jour,MO[c_mois].c_str(),c_an%100);
 else if (c_format=="31 Jan 05")     sprintf(tmp,"%02d %s %02d",c_jour,mo[c_mois].c_str(),c_an%100);
 else if (c_format=="31 Janvier")    sprintf(tmp,"%02d %s",c_jour,MO[c_mois].c_str());
 else if (c_format=="31 J")          sprintf(tmp,"%02d %s",c_jour,Smo[c_mois].c_str());
 else if (c_format=="1,2,...52")     sprintf(tmp,"%02d",Compute_Week(c_an,c_mois,c_jour));
 else if (c_format=="Semaine1,Semaine 2,..") sprintf(tmp,"Semaine %02d",Compute_Week(c_an,c_mois,c_jour));
 else if (c_format=="Janvier 05")    sprintf(tmp,"%s %02d",MO[c_mois].c_str(),c_an%100);
 else if (c_format=="Jan 05")        sprintf(tmp,"%s %02d",mo[c_mois].c_str(),c_an%100);
 else if (c_format=="Jan")           sprintf(tmp,"%s",mo[c_mois].c_str());
 else if (c_format=="J 05")          sprintf(tmp,"%s %02d",Smo[c_mois].c_str(),c_an%100);
 else if (c_format=="J")             sprintf(tmp,"%s",Smo[c_mois].c_str());
 else if (c_format=="1,2,3...")      sprintf(tmp,"%d",c_mois);
 else if (c_format=="1-12,1-12..")   sprintf(tmp,"%d",Compute_Month(c_an,c_mois,c_jour,0));
 else if (c_format=="1-12,13,...")   sprintf(tmp,"%d",Compute_Month (c_an,c_mois,c_jour,1));
 else if (c_format=="Mois 1 - Mois 12, Mois 13...")  sprintf(tmp,"Mois %d",Compute_Month (c_an,c_mois,c_jour,1));
 else if (c_format=="Mois 1, Mois 2...")  sprintf(tmp,"Mois %d",Compute_Month(c_an,c_mois,c_jour,0));
 else if (c_format=="Tri 1 05")      sprintf(tmp,"Tri %s %02d",trim,c_an%100);
 else if (c_format=="Tri 1")         sprintf(tmp,"Tri %s",trim);
 else if (c_format=="T1 05")         sprintf(tmp,"T%s %02d",trim,c_an%100);
 else if (c_format=="1T 05")         sprintf(tmp,"%sT %02d",trim,c_an%100);
 else if (c_format=="1,2,3,4")       sprintf(tmp,"%s",trim);
 //if (c_format=="1..4,1..4")
 else if (c_format=="Trimestre 1, Trimestre 2,...")  sprintf(tmp,"Trimestre %s",trim);
 else if (c_format=="2005")          sprintf(tmp,"%04d",c_an);
 else if (c_format=="05")            sprintf(tmp,"%02d",c_an%100);
 else if (c_format=="Ann�e 1-Ann�e 2...") sprintf(tmp,"Ann�e %04d",c_an);

 Format_Cal = AnsiString(tmp);
}


int __fastcall TplStandard::Compute_Week(int c_an,int c_mois,int c_jour)
{
  TDateTime dtFirst; int firstday;
  int i,totaljours,week_number;  int dow;
  int nbdays[13] = {31,28,31,30,31,30,31,31,30,31,30,31};
  if (IsLeapYear(c_an)) nbdays[1]=29;

  //dtFirst = EncodeDate(c_an,1,1);
  if (c_an >=2010)
    {
     i=i;
    }

  dtFirst = EncodeDate(c_an-1,12,31);
  // dtFirst = EncodeDate(c_an,1,1);
  firstday = dtFirst.DayOfWeek(); // Monday is 2
  totaljours=0;
  if (c_mois==1) totaljours = 0;
   else  for (i=1;i<c_mois;i++) totaljours += nbdays[i-1];
  totaljours += c_jour;
  week_number = (totaljours/7) + 1;
  dtFirst = EncodeDate(c_an,c_mois,c_jour);
  dow = dtFirst.DayOfWeek();
  if (dow < firstday) week_number++;

  if (firstday > 4 ) // semaine qui commence un vendredi ou samedi
    {week_number = week_number - 1; }

  //  ERREUR CORRIGEE LE 28 AOUT 2009
  //if (firstday > 5) // semaine qui commence un vendredi ou samedi
  //  {week_number = week_number - 1;
  //   if (week_number==0) week_number = 53;
  //  }
  if (week_number==1)
    i=i;
  return week_number;
}


/*
int __fastcall TplStandard::Compute_Week(int c_an,int c_mois,int c_jour)

const int JAN = 1;
            const int DEC = 12;
            const int LASTDAYOFDEC = 31;
            const int FIRSTDAYOFJAN = 1;
            const int THURSDAY = 4;
            bool thursdayFlag = false;

            //Get the day number since the beginning of the year
            int dayOfYear = date.DayOfYear;

            //Get the first and last weekday of the year
            int startWeekDayOfYear = (int)(new DateTime(date.Year, JAN, FIRSTDAYOFJAN)).DayOfWeek;
            int endWeekDayOfYear = (int)(new DateTime(date.Year, DEC, LASTDAYOFDEC)).DayOfWeek;

            //Compensate for using monday as the first day of the week
            if (startWeekDayOfYear == 0)
                startWeekDayOfYear = 7;
            if (endWeekDayOfYear == 0)
                endWeekDayOfYear = 7;

            //Calculate the number of days in the first week
            int daysInFirstWeek = 8 - (startWeekDayOfYear);

            //Year starting and ending on a thursday will have 53 weeks
            if (startWeekDayOfYear == THURSDAY || endWeekDayOfYear == THURSDAY)
                thursdayFlag = true;

            //We begin by calculating the number of FULL weeks between
            //the year start and our date. The number is rounded up so
            //the smallest possible value is 0.
            int fullWeeks = (int)Math.Ceiling((dayOfYear - (daysInFirstWeek)) / 7.0);
            int resultWeekNumber = fullWeeks;

            //If the first week of the year has at least four days, the
            //actual week number for our date can be incremented by one.
            if (daysInFirstWeek >= THURSDAY)
                resultWeekNumber = resultWeekNumber + 1;

            //If the week number is larger than 52 (and the year doesn't
            //start or end on a thursday), the correct week number is 1.
            if (resultWeekNumber > 52 && !thursdayFlag)
                resultWeekNumber = 1;

            //If the week number is still 0, it means that we are trying
            //to evaluate the week number for a week that belongs to the
            //previous year (since it has 3 days or less in this year).
            //We therefore execute this function recursively, using the
            //last day of the previous year.
            if (resultWeekNumber == 0)
                resultWeekNumber = GetWeekNumber(new DateTime(date.Year - 1, DEC, LASTDAYOFDEC));
            return resultWeekNumber;
        }

*/
int __fastcall TplStandard::Compute_Month(int c_an,int c_mois,int c_jour,int mode)
{
  // calcul du nb de mois depuis debut du projet
  int month_number,m1,m2;

  m1=  (AnPrev % 100)*12 + MoisPrev;
  m2 = (c_an % 100)*12 + c_mois;
  month_number = m2-m1;
  if (mode==0) month_number = month_number % 12;
  month_number++;

  return month_number;
}


void __fastcall TplStandard::Trace_Calendrier()  // cal et entete colonnes
{
 int H_Text,H_Cal;

 int col,numchamp; AnsiString txt; char tmp[250];
 int x1,y1,x2,y2;  bool flprem; int first,premcol;
 int firstday,ix,base;
 int ptr1,ptr2;
 int ep1,ep2,tmpcol;
 float valf;  int i;
 TColor coul1,coul2;
 TDateTime dtFirst;
 AnsiString mo[13];
 AnsiString MO[13];
 AnsiString mon[13];
 int itmp,zcal,style;
 int LJT;
 AnsiString xx; int t,t1;
 int pmax,vtemp,freq;
 int HTitreMax;

 H_CAL1[pc]=PHH[pc][9];
 H_CAL2[pc]=PHH[pc][10];
 first=1; premcol = 1;
 Y0=Y0+2;  Hauteur_Cal=Y0;

 HTitreMax=0;
 for(i=0; i<15;i++)
   {
    H_Text = T_HAUT[pc][i];
    if (H_Text>HTitreMax) HTitreMax= H_Text;
   }

 //  calcul du maximum entre Hauteur Titre Col et Hauteur Cal
 // H_Text = 130 * T_HAUT[pc][1]/100;  // 1/3 de plus
 H_Text=130*HTitreMax/100;
 H_Cal=0;
 if (P_ECHH[pc]=="O") H_Cal += H_CAL1[pc];
 if (P_ECHB[pc]=="O") H_Cal += H_CAL2[pc];
 if (H_Text>H_Cal) {H_CAL1[pc]= H_Text/2; H_CAL2[pc]=H_Text/2;}
 else {H_Text = H_Cal;}


 for (col=1;col<P_COL[pc];col++)        // $$$$$$$$$$$$$
     {
      numchamp = P_TRICOL[pc][col];
      //if (numchamp < 40 && numchamp!=0)
      if (numchamp != 0)
        {
         tmpcol=col;
         if (first)
           {
               x1=D_LEFT[pc]; x2=P_LC[pc][tmpcol]; y1=Y0; y2=y1+ H_Text; // $$$ H_CAL1[pc]+H_CAL2[pc];
               Insert_G('Y',"RT",x1,y1,x2,y2,"DT",TColor(T_CBCOLOR[pc][col]),TColor(T_CFCOLOR[pc][col]),0,0,"",' ',' ',"");
               x1=P_LC[pc][tmpcol]; x2= D_RIGHT[pc];
               y1=Y0; y2=y1;
               Insert_G('Y',"LH",x1,y1,x2,y2,"DT",TColor(0),TColor(0),1,0,"",'N','N',"");

               y1=Y0+ H_Text + 2; // H_CAL1[pc]+H_CAL2[pc]+2; // +2 a cause epaisseur des traits
            y2=y1+1;
            Insert_G('Y',"LH",x1,y1,x2,y2,"DT",TColor(0),TColor(0),1,0,"",'N','N',"");
            first=0;

           }
         // premiere colonne
         if (premcol && (numchamp < 40))
             {

              P_DEBUTCOL[pc] = P_LC[pc][tmpcol];
              premcol = 0;
             }

         // calcul des cadrages // centrage

         txt = T_TITRE_COL[pc][col];
         Compute_LH(T_FONT[pc][col],txt,T_HAUT[pc][col],T_ITAL[pc][col],T_BOLD[pc][col]);


          xx = T_JH[pc][col];
         //xx = PJH[pc][col];
         if (xx=="Centre") x1 = P_LC[pc][tmpcol]+ (P_LC[pc][tmpcol+1]-P_LC[pc][tmpcol])/2-L/2;
         else if (xx=="Gauche")x1 = P_LC[pc][tmpcol] + 5;
         else  x1= P_LC[pc][tmpcol+1] - L - 5;
         x2=x1+L;
         //y1=Y0+(H_CAL1[pc]+H_CAL2[pc]-H)/2;
         y1=Y0+(H_Text-H)/2;
         y2=y1+T_HAUT[pc][col];
         Insert_G('Q',"TX",x1,y1,x2,y2,"DT",TColor(T_CBCOLOR[pc][col]),
           TColor(T_CFCOLOR[pc][col]),T_HAUT[pc][col],0,T_FONT[pc][col],T_ITAL[pc][col],T_BOLD[pc][col],txt);
         // P_DEBUTGANTT[pc] = P_LC[pc][tmpcol+1];

        }  // if numchamp
     }  // fin Bcl FOR

  if (pc>3)
    {
     memy1[pc]=Y0;
     memy2[pc]= Y0+H_Text;  //H_CAL1[pc]+H_CAL2[pc];   // memorise y1 / y2   pour copie rectangle calendrier
     Y0=Y0+H_Text;  //H_CAL1[pc]+H_CAL2[pc]+4;
     Hauteur_Cal=Y0-Hauteur_Cal;
     return;
    }
 // Trac� de la ligne separatrice des calendriers et remplissage couleurs
  x1=P_DEBUTGANTT[pc]+1; x2=D_RIGHT[pc];
  y1=Y0+1;  // Garder Y0 Tel que
  y2=y1+1;  // $$$$$ Y0+1;

  if (P_ECHH[pc]=="O")
    {
      Insert_G('H',"LH",x1,y1,x2,y2,"DT",TColor(0),TColor(0),1,0,"",'N','N',"");
      y1 = y1+1;
      y2=y1+H_CAL1[pc]-1;
      Insert_G('H',"RT",x1,y1,x2,y2,"DT",PB[pc][9],PB[pc][9],0,0,"",' ',' ',"");
      y1=Y0+H_CAL1[pc]; y2=y1+1;
      Insert_G('H',"LH",x1,y1,x2,y2,"DT",TColor(0),TColor(0),1,0,"",'N','N',"");

     }

  if (P_ECHB[pc]=="O")
    {
     y1=y2; y2=y1+H_CAL2[pc]-1;
     Insert_G('H',"RT",x1,y1,x2,y2,"DT",PB[pc][10],PB[pc][10],0,0,"",' ',' ',"");
    }
 //  Remplissage calendrier   et trac� des lignes verticales

 base = Compute_Base();
 BASE = base;
 Compute_BaseCal();    // compute Increm_CAL1 and _Cal2
//  P_MAXDIST est le nombre total de jours du projet

 // cas du premier mois
 flprem= false;
 if (JourPrev >1)  flprem=true;

 pmax = P_MAXDIST[pc]*200/100;
 //  pmax = P_MAXDIST[pc];
 ptr1=0;ptr2=0;
 zcal = 1;
 freq=1;



 //Trace ligne s�paratrice des grilles et d�but de gantt
  x1=P_DEBUTGANTT[pc]; x2=x1+1;
  y1=Y0; y2=P_H[pc] - y1;
  Insert_G('H',"LV",x1,y1,x2,y2,"DT",TColor(0),TColor(0),2,0,"",'N','N',"");


  for (ix=0;ix<IXDEB;ix++)
    { if (ix==Cal1Z[ptr1] && P_ECHH[pc]=="O") ptr1++;
      if (ix==Cal2Z[ptr2] && P_ECHB[pc]=="O") ptr2++;
    }

  for (ix=IXDEB;ix<=pmax;ix=ix+freq)    // $$$$$$$ +30 ???
  {

   Trace_Vertical(ix,freq); // ligne verticale
   if (ix==Cal1Z[ptr1] && P_ECHH[pc]=="O")
     {
       // Trace_Vertical(ix,freq); // ligne verticale

      t= Compute_Trav(ix-IXDEB,1);
      x1 = P_DEBUTGANTT[pc] + t* base/P_MAXDIST[pc];

      x2 = x1+1;
      y1 = Y0; y2=y1+H_CAL1[pc]-1;
      Insert_G('H',"LV",x1,y1,x2,y2,"DT",TColor(0),TColor(0),1,0,"",'N','N',"");

      Format_Calendrier(P_FORMATCAL1[pc],An,Mois,Jour);
      txt =  Format_Cal; strcpy(tmp,txt.c_str());
      Compute_LH(PF[pc][9],tmp,PH[pc][9],PN[pc][9],PX[pc][9]);

      t = Compute_Trav(Cal1Z[ptr1+1]-IXDEB,1); t1= Compute_Trav(Cal1Z[ptr1]-IXDEB,1);
      vtemp = t - t1;
      if (vtemp==0) vtemp=1;

      if (PJH[pc][9]=="Gauche")   x1=x1+ 3; // (vtemp*base/P_MAXDIST[pc] - L)/2;
      else if (PJH[pc][9]=="Droite")
       {
         x2=vtemp*base/P_MAXDIST[pc];
         x1=x1 + x2-L-1;
       }
      else x1=x1+ (vtemp*base/P_MAXDIST[pc] - L)/2;

      if (x1<P_DEBUTGANTT[pc]) { x1=P_DEBUTGANTT[pc]; txt = ""; L=0; }


      x2=x1+L;
      x2=x1+L;
      y1=Y0+1+(H_CAL1[pc]-H)/2;
      y2=y1+PH[pc][9];
      // pas inserer si JNT = "non"

      if (P_UNIT1[pc] == "Jour" && P_CYNJNT[pc] == "Non" && cal[pc][ix]=='N') {}
       else
         Insert_G('H',"TX",x1,y1,x2,y2,"DT",PB[pc][9],PC[pc][9],PH[pc][9],0,
         PF[pc][9],PN[pc][9],PX[pc][9],txt);
      ptr1++;
     }

   if (ix==Cal2Z[ptr2] && P_ECHB[pc]=="O")  // ligne verticale pleine
     {
       Trace_Vertical(ix,freq); // ligne verticale
       t= Compute_Trav(ix-IXDEB,1);
      x1 = P_DEBUTGANTT[pc] + t*base/P_MAXDIST[pc];

      x2 = x1+1;
      if (P_ECHH[pc]=="O") y1 = Y0+H_CAL1[pc]+2; else y1=Y0;
      y2=y1+H_CAL2[pc]-1;
      if (x1>P_DEBUTGANTT[pc])
       {
        Insert_G('H',"LV",x1,y1,x2,y2,"DT",TColor(0),TColor(0),3,0,"",'N','N',"");
       }

      Format_Calendrier(P_FORMATCAL2[pc],An,Mois,Jour);
      txt =  Format_Cal;  strcpy(tmp,txt.c_str());

      Compute_LH(PF[pc][10],tmp,PH[pc][10],PN[pc][10],PX[pc][10]);
      t=   Compute_Trav(Cal2Z[ptr2+1]-IXDEB,1); t1= Compute_Trav(Cal2Z[ptr2]-IXDEB,1);
      vtemp = t - t1;
      if (vtemp==0) vtemp=1;

      xx= PJH[pc][10];

      if (PJH[pc][10]=="Gauche")   x1=x1+ 3; // (vtemp*base/P_MAXDIST[pc] - L)/2;
      else if (PJH[pc][10]=="Droite")
       {
         x2=vtemp*base/P_MAXDIST[pc];
         x1=x1 + x2-L-1;
       }
      else x1=x1+ (vtemp*base/P_MAXDIST[pc] - L)/2;

      if (x1<P_DEBUTGANTT[pc])
         {
           x1=P_DEBUTGANTT[pc];
           txt = "";
           L=0;
         }

      //x1=x1+ ((Cal2Z[ptr2+1]-Cal2Z[ptr2])*base/P_MAXDIST[pc] - L)/2;
      //  $$$$ x1=x1+ (Compute_Trav(Cal2Z[ptr2+1]-Cal2Z[ptr2],1)*base/P_MAXDIST[pc] - L)/2;
      x2=x1+L;
      if (P_ECHH[pc]=="O") y1=Y0+1+ H_CAL1[pc] + (H_CAL2[pc]-H);   // /2
      else  y1=Y0+1+ (H_CAL2[pc]-H);
      y2=y1+PH[pc][9];
      if (P_UNIT2[pc] == "Jour" && P_CYNJNT[pc] == "Non" && cal[pc][ix]=='N') {}
       else
      Insert_G('H',"TX",x1,y1,x2,y2,"DT",PB[pc][10],PC[pc][10],
           PH[pc][10],0,PF[pc][10],PN[pc][10],PX[pc][10],txt);
      flprem=false;
      ptr2++;
     }

  }  // for ix;
 memy1[pc]=Y0; Y0 = Y0 + H_Text;  memy2[pc]= Y0+4;
 Hauteur_Cal=Y0-Hauteur_Cal;
}

int __fastcall TplStandard::Compute_Trav(int ix, int zcal)
{
 int i, cnttrav; char tmp[150];
 int ixx,freq;

 cnttrav=0;
 if (ix<0)
   {
    return -1;
   }

 cnttrav=0;
 for(i=0;i<ix;i++)
   {
    if (cal[zcal][i] =='T' ||  P_CYNJNT[pc] == "Oui" )
       cnttrav++;
   }

 return cnttrav;
}

void __fastcall TplStandard::TraceJNT(int nt, int dist1,int dist2,int x1,int y1,int x2, int y2,int freq)
{
 int base,x11,x22,ep1;
 int i,zcal; int coul1;
 char tmp[150];
 int style;

 style=0;

 strcpy(tmp,av->Cells[10][nt].c_str());
 zcal = atoi(tmp); if (zcal <0 || zcal>5) zcal=1;
 base = Compute_Base();
 //IXDEB=40;
 for (i=dist1;i<dist2;i++)
   {
    if (cal[zcal][i]=='N')

         {
          coul1 = AnsiToInt(P_CCJNT[pc]);
          x11 = P_DEBUTGANTT[pc] + Compute_Trav(i-IXDEB,1)*base*freq/P_MAXDIST[pc];
          // x1 identique
           x22= x11 + base/P_MAXDIST[pc];
           ep1=x22-x11;
          Insert_G('H',"RT",x11,y1,x22,y2,"DT",TColor(coul1),TColor(coul1),ep1,style,"",'D','N',"");
         }
   }


/*
add_jour(AnPrev,MoisPrev,JourPrev,ix);
dtFirst=EncodeDate(An,Mois,Jour);
firstday = dtFirst.DayOfWeek();
x1 = P_DEBUTGANTT[pc] + Compute_Trav(ix,1)*base*freq/P_MAXDIST[pc];

x2= x1 + base/P_MAXDIST[pc];
ep1=x2-x1;
Insert_G('H',"RT",x1,y1,x2,y2,"DT",TColor(coul1),TColor(coul1),ep1,style,"",'D','N',"");
*/


}

void __fastcall TplStandard::Trace_Vertical(int ix,int freq)
{
char tmp[150]; int test;  char tmp1[10000];
float valf;
int ep1,base,coul1,x1,y1,x2,y2,zcal;
int firstday,style;
TDateTime dtFirst;
AnsiString DoTD;

char itmp;
int m_ep1,m_base,m_coul1,m_x1,m_y1,m_x2,m_y2, m_style;
bool m_flag;  int id;

base = Compute_Base();
add_jour(AnPrev,MoisPrev,JourPrev,ix);
dtFirst=EncodeDate(An,Mois,Jour);
firstday = dtFirst.DayOfWeek();
//IXDEB=40;
x1 = P_DEBUTGANTT[pc] + Compute_Trav(ix-IXDEB,1)*base*freq/P_MAXDIST[pc];
//�����x1 = P_DEBUTGANTT[pc] + ix*base/P_MAXDIST[pc];
y1 = Y0+H_CAL1[pc]+H_CAL2[pc]+5; y2=P_H[pc] - P_MARGV2[pc];
m_flag=false;
id = ix;
test = cal[1][id];
strncpy(tmp1,cal[1],9998); tmp1[9999]=0;
test = tmp1[id];


if (P_CYNJNT[pc]=="Oui")
  {
    zcal = 1;                         //if (cal[zcal][i]=='N')
    if (test==78)

         {
          coul1 = AnsiToInt(P_CCJNT[pc]);
          // x1 identique
           x2= x1 + base/P_MAXDIST[pc];
           ep1=x2-x1;
          Insert_G('H',"RT",x1,y1,x2,y2,"DT",TColor(coul1),TColor(coul1),ep1,style,"",'D','N',"");
         }
   }
if (P_CYN[pc][0]=="Oui")      // Tous les Jours
  {
    style = GetStyle(P_CCST[pc][0]);
    zcal = 1;
    itmp = cal[zcal][ix];

   valf=AnsiToFloat(P_CCEP[pc][0]);
    ep1 = (int)(valf*COEFMM);  if (ep1==0) ep1=1;
   x2 = x1+ep1;
   coul1 = AnsiToInt(P_CCFG[pc][0]);
   m_ep1 = ep1; m_coul1 = coul1; m_x1=x1; m_y1=y1;m_x2=x2; m_y2 = y2;m_style=style;
   m_flag = true;
   Insert_G('H',"LV",x1,y1,x2,y2,"DT",coul1,coul1,ep1,style,"",'D','N',"");
  }

if (P_CYN[pc][1]=="Oui")   // Toutes les semaines
  { if (firstday==2)  // debut de semaine
     {
      style = GetStyle(P_CCST[pc][1]);
      valf=AnsiToFloat(P_CCEP[pc][1]);
      ep1 = (int)(valf*COEFMM);  if (ep1==0) ep1=1;
      x2 = x1+ep1;
      coul1 = AnsiToInt(P_CCFG[pc][1]);
      m_ep1 = ep1; m_coul1 = coul1; m_x1=x1; m_y1=y1;m_x2=x2; m_y2 = y2;m_style=style;
      m_flag = true;
      Insert_G('H',"LV",x1,y1,x2,y2,"DT",coul1,coul1,ep1,style,"",'D','N',"");
     }
  }

 if (P_CYN[pc][2]=="Oui")   // Tous les Mois
  { if (Jour==1)  //
       {
        style = GetStyle(P_CCST[pc][2]);
        valf=AnsiToFloat(P_CCEP[pc][2]);
        ep1 = (int)(valf*COEFMM);  if (ep1==0) ep1=1;
        x2 = x1+ep1;
        coul1 = AnsiToInt(P_CCFG[pc][2]);
        m_ep1 = ep1; m_coul1 = coul1; m_x1=x1; m_y1=y1;m_x2=x2; m_y2 = y2;m_style=style;
        m_flag = true;
        Insert_G('H',"LV",x1,y1,x2,y2,"DT",coul1,coul1,ep1,style,"",'D','N',"");
     }
  }

 if (P_CYN[pc][3]=="Oui")   // Tous les Trimestres
  { if (Jour==1 && (Mois==1 || Mois==4 || Mois==7|| Mois ==10))  //
       {
        style = GetStyle(P_CCST[pc][3]);
        valf=AnsiToFloat(P_CCEP[pc][3]);
        ep1 = (int)(valf*COEFMM);  if (ep1==0) ep1=1;
        x2 = x1+ep1;
        coul1 = AnsiToInt(P_CCFG[pc][3]);
        m_ep1 = ep1; m_coul1 = coul1; m_x1=x1; m_y1=y1;m_x2=x2; m_y2 = y2;m_style=style;
        m_flag = true;
        Insert_G('H',"LV",x1,y1,x2,y2,"DT",coul1,coul1,ep1,style,"",'D','N',"");
     }
  }

if (P_CYN[pc][4]=="Oui")   // Tous les Ans
  { if (Jour==1 && (Mois==1))  //
       {
        style = GetStyle(P_CCST[pc][4]);
        valf=AnsiToFloat(P_CCEP[pc][4]);
        ep1 = (int)(valf*COEFMM);  if (ep1==0) ep1=1;
        x2 = x1+ep1;
        coul1 = AnsiToInt(P_CCFG[pc][4]);
        m_ep1 = ep1; m_coul1 = coul1; m_x1=x1; m_y1=y1;m_x2=x2; m_y2 = y2;m_style=style;
        m_flag = true;
        Insert_G('H',"LV",x1,y1,x2,y2,"DT",coul1,coul1,ep1,style,"",'D','N',"");
     }
  }

if (P_CYN[pc][5]=="Oui")   // Aujourd hui
  {
    DoTD = DateToStr(Date());
    strcpy(tmp,DoTD.c_str());
    verifdate(tmp);
    if (Jour==JourVerif && Mois==MoisVerif && An==AnVerif)  //
       {
        // pas de trace du jour si P_CYNJNT = "Non"
        if (P_CYNJNT[pc]=="Oui")
          {
           style = GetStyle(P_CCST[pc][5]);
           valf=AnsiToFloat(P_CCEP[pc][5]);
           ep1 = (int)(valf*COEFMM);  if (ep1==0) ep1=1;
           x2 = x1+ep1;
           coul1 = AnsiToInt(P_CCFG[pc][5]);
           m_ep1 = ep1; m_coul1 = coul1; m_x1=x1; m_y1=y1;m_x2=x2; m_y2 = y2;m_style=style;
           m_flag = true;
           Insert_G('H',"LV",x1,y1,x2,y2,"DT",coul1,coul1,ep1,style,"",'D','N',"");
         }
     }
  }
// if (m_flag) Insert_G('H',"LV",m_x1,m_y1,m_x2,m_y2,"DT",TColor(m_coul1),TColor(m_coul1),m_ep1,m_style,"",'D','N',"");
}

int __fastcall TplStandard::Compute_Base()
{
char tmp[150];
int bas,itmp;
float basfloat;
int freq;

if (P_CJT[pc]=="Oui") P_CJT[pc]="30.0";
if (P_CJT[pc]=="") P_CJT[pc]="30.0";



 if (Check_Automatic())
  {
    bas = P_W[pc] - P_MARGV2[pc] - P_DEBUTGANTT[pc] - P_LASTTEXT[pc]; // - 150;  // 100  marge pour texte
    // if (P_CYNJNT[pc]=="Oui") bas = bas*7/5;

    ///if (P_MAXDIST[pc] != 0)
    ///  { itmp = bas/(P_MAXDIST[pc]-IXDEB);
    ///    if (itmp==0) itmp=1;
    ///    itmp=itmp*1.05;
    ///    P_CJT[pc] = AnsiString(itmp)+"";
    ///   }
    ///  else P_CJT[pc] = "30.0";
  }

 else
   {
    bas = P_W[pc] - P_MARGV2[pc] - P_DEBUTGANTT[pc] - P_LASTTEXT[pc];

    strcpy(tmp,P_CJT[pc].c_str());
       basfloat = atof(tmp);
       itmp = basfloat;
       bas = itmp * (P_MAXDIST[pc]-IXDEB);

     /*
     if (P_CJTFLAG[pc] == "Yes")
      {
       bas = P_W[pc] - P_MARGV2[pc] - P_DEBUTGANTT[pc] - P_LASTTEXT[pc];

        itmp = bas/(P_MAXDIST[pc]-IXDEB);
        if (itmp==0) itmp=1;
        itmp=itmp*1.05;
        P_CJT[pc] = AnsiString(itmp)+"";
       //}
      //else P_CJT[pc] = "30.0";

          //  P_CJT[pc] = AnsiString(itmp); //+".0";

       }
     else  // Forcer la valeur de P_CJT
      {
       bas = P_W[pc] - P_MARGV2[pc] - P_DEBUTGANTT[pc] - P_LASTTEXT[pc];
       strcpy(tmp,P_CJT[pc].c_str());
       basfloat = atof(tmp);
       itmp = basfloat;
       bas = itmp * (P_MAXDIST[pc]-IXDEB);
       P_CJT[pc] = AnsiString(itmp)+"";
      }
    */
   }


//bas = atoi(P_CJT[pc].c_str());

strcpy(tmp,P_FREQ1[pc].c_str());
freq = atoi(tmp);
if (freq<1) freq=1;
// bas = bas / freq;
return bas;

}

void __fastcall TplStandard::Compute_BaseCal()
{  int i,cnt; int firstday;
   int pmaxdist,ctrfreq;
  TDateTime dtFirst;
  int freq; char tmp[150];

 pmaxdist = P_MAXDIST[pc] * 20/10;  // 20 % en plus
 ctrfreq = 0;
 strcpy(tmp,P_FREQ1[pc].c_str());
 freq = atoi(tmp);

  for (i=0;i<MAX_ZONE;i++) { Cal1Z[i]=0; Cal2Z[i]=0; }


 if (P_UNIT1[pc]=="Jour")
   { cnt = 1;
     for (i=1; i<=pmaxdist; i++)
     { if (i>MAX_ZONE) break;
      ctrfreq++;
      if (ctrfreq==freq)
        { Cal1Z[cnt]=i;
          cnt++; if (cnt>MAX_ZONE) break;
          ctrfreq = 0;
        }
     }
     Cal1Z[cnt]=Cal1Z[cnt-1]+1;
     Cal1Z[cnt+1] = Cal1Z[cnt]+1;
   }
 else if (P_UNIT1[pc]=="Semaine")
   { cnt=1;
     for (i=1;i<=pmaxdist;i++)
      { add_jour(AnPrev,MoisPrev,JourPrev,i); dtFirst=EncodeDate(An,Mois,Jour);
       firstday = dtFirst.DayOfWeek();
       if (firstday==2)
        {
         ctrfreq++;
         if (ctrfreq==freq)
          {
          Cal1Z[cnt]=i;
          cnt++;
          if (cnt>MAX_ZONE) break;
          ctrfreq = 0;
          }
        }
      }
    Cal1Z[cnt]=Cal1Z[cnt-1]+7;
    Cal1Z[cnt+1] = Cal1Z[cnt]+7;
   }
 else if (P_UNIT1[pc]=="Mois")
   {
    cnt=1;
    for (i=1;i<=pmaxdist;i++)
      {
       add_jour(AnPrev,MoisPrev,JourPrev,i);
       if (Jour==1)
         {
          ctrfreq++;
          if (ctrfreq==freq)
           {
            Cal1Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
            ctrfreq=0;
           }
         }
      }
    Cal1Z[cnt]=Cal1Z[cnt-1]+31;
    Cal1Z[cnt+1] = Cal1Z[cnt]+31;
   }
 else if (P_UNIT1[pc]=="Trimestre")
   {
    cnt=1;
    for (i=1;i<=pmaxdist;i++)
      {
       add_jour(AnPrev,MoisPrev,JourPrev,i);
       if (Jour==1)
         {
           if (Mois==1 || Mois==4 || Mois == 7 || Mois==10)
             {
              ctrfreq++;
              if (ctrfreq==freq)
                {
                  Cal1Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
                  ctrfreq=0;
                }
             }
         }
      }
    Cal1Z[cnt]=Cal1Z[cnt-1]+90;
    Cal1Z[cnt+1] = Cal1Z[cnt]+90;
   }
 else  // annee
   {
    cnt=1;
    for (i=1;i<=pmaxdist;i++)
      {
       add_jour(AnPrev,MoisPrev,JourPrev,i);
       // dtFirst=EncodeDate(An,Mois,Jour);
       if (Mois==1 && Jour==1)
         {
            ctrfreq++;
              if (ctrfreq==freq)
                {
                  Cal1Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
                  ctrfreq=0;
                }
         }
      }
    Cal1Z[cnt]=Cal1Z[cnt-1]+365;
    Cal1Z[cnt+1] = Cal1Z[cnt]+365;
   }

/// Idem pour CAL2
ctrfreq=0;
strcpy(tmp,P_FREQ2[pc].c_str());
 freq = atoi(tmp);

if (P_UNIT2[pc]=="Jour")
   { cnt=1;
     for (i=1; i<=pmaxdist; i++)
     { if (i>=MAX_ZONE) break;
       ctrfreq++;
       if (ctrfreq==freq)
         {
          Cal2Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
          ctrfreq=0;
          }

      }
     Cal2Z[cnt]=Cal2Z[cnt-1]+1;
     Cal2Z[cnt+1]=Cal2Z[cnt]+1;
   }

 else if (P_UNIT2[pc]=="Semaine")
   { cnt=1;
     for (i=1;i<=pmaxdist;i++)
      { add_jour(AnPrev,MoisPrev,JourPrev,i); dtFirst=EncodeDate(An,Mois,Jour);
       firstday = dtFirst.DayOfWeek();
       if (firstday==2)
       {
        ctrfreq++;
       if (ctrfreq==freq)
         {
          Cal2Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
          ctrfreq=0;
          }
        }
      }
    Cal2Z[cnt]=Cal2Z[cnt-1]+7;
    Cal2Z[cnt+1]=Cal2Z[cnt]+7;

   }
 else if (P_UNIT2[pc]=="Mois")
   {
    cnt=1;
    for (i=1;i<=pmaxdist;i++)
      {
       add_jour(AnPrev,MoisPrev,JourPrev,i);
      if (Jour==1)
         {
          ctrfreq++;
          if (ctrfreq==freq)
         {
          Cal2Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
          ctrfreq=0;
          }
         }
      }
    Cal2Z[cnt]=Cal2Z[cnt-1]+31;
    Cal2Z[cnt+1]=Cal2Z[cnt]+31;

   }
 else if (P_UNIT2[pc]=="Trimestre")
   {
    cnt=1;
    for (i=1;i<=pmaxdist;i++)
      {
       add_jour(AnPrev,MoisPrev,JourPrev,i);
       if (Jour==1)
         {
           if (Mois==1 || Mois==4 || Mois == 7 || Mois==10)
             {
              ctrfreq++;
              if (ctrfreq==freq)
              {
               Cal2Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
               ctrfreq=0;
              }
             }
          }
      }
    Cal2Z[cnt]=Cal2Z[cnt-1]+90;
    Cal2Z[cnt+1]=Cal2Z[cnt]+90;
   }
 else  // annee
   {
    cnt=1;
    for (i=1;i<=pmaxdist;i++)
      {
       add_jour(AnPrev,MoisPrev,JourPrev,i);

       if (Mois==1 && Jour==1)
         {
          ctrfreq++;
          if (ctrfreq==freq)
             {
              Cal2Z[cnt]=i; cnt++; if (cnt>=MAX_ZONE) break;
              ctrfreq=0;
             }
         }
      }
     Cal2Z[cnt]=Cal2Z[cnt-1]+365;
     Cal2Z[cnt+1]=Cal2Z[cnt]+365;
   }
}



// ***************************************************
//
// ***************************************************

void __fastcall TplStandard::Trace_Liens_Prev()
{
 int indx, count, nbsc;
 char tmp[500];  char marg[20];   char tmpxx[50];
 int x1,y1,x2,y2,y11,y22; int rc,mrgP,mrgR,ress;
 int loop; int tsucc,ept;  char temp[100];
 char xxx[50];
 int coul1,coul2,epaiss,style;
 char ttmp[30];
 float valf;

 NB_DETAIL[pc]=asTri->RowCount-1;
 if (NB_DETAIL[pc]==0) return;

 x1=P_DEBUTGANTT[pc];
 for (indx=1;indx<=NB_DETAIL[pc];indx++)
   {
    // if (filtr[indx] != 'X') continue;
    strcpy(xxx,asTri->Cells[63][indx].c_str());
    nbsc = atoi(xxx); // StrToInt(asTri->Cells[63][indx]);

    if (nbsc>0 && nbsc<50)
      {
       // coordonnees de fin
       strcpy(xxx,asTri->Cells[68][indx].c_str());
       x1=atoi(xxx);
       strcpy(xxx,asTri->Cells[69][indx].c_str());
       y1=atoi(xxx);
       strcpy(tmp,asTri->Cells[64][indx].c_str());
       count=Explode('|',tmp);

       for (loop=1;loop<count;loop++)
        {
          tsucc=atoi(params[loop]);
          // if (filtr[tsucc] != 'X') continue;
          if (tsucc==9999) rc=0; else rc = Lookup(tsucc);

          if (rc > 0)
           {
            strcpy(xxx,asTri->Cells[67][rc].c_str());
            x2=atoi(xxx);
             strcpy(xxx,asTri->Cells[69][rc].c_str());
            y2=atoi(xxx);
            strcpy(marg,asTri->Cells[8][rc].c_str());
            mrgP = atoi(marg);
            ress = Row_AsAvanc(rc);
            mrgR = atoi(asAvanc->Cells[15][ress].c_str());

            if  (mrgP==0 && P_LIENS[pc][0]=="Oui")
               {
                strcpy(ttmp,P_COL_LIEN[pc][0].c_str());
                coul1=atoi(ttmp);
                if (P_EPAIS_LIEN[pc][0]=="") P_EPAIS_LIEN[pc][0]="2.0";
                valf=AnsiToFloat(P_EPAIS_LIEN[pc][0]);
                epaiss =  (int)(valf*COEFMM);
                style = GetStyle(P_STYLELIEN[pc][0]);

                valf=AnsiToFloat(P_EPTRAIT[pc][0]);
                ept = (int)(valf*COEFMM);
                y11 = y1 + ept/2; y22 = y2+ept/2;
                //sprintf(temp,"MrgP=0 T=%d X1=%d X2=%d",indx,x1,x2);
                //Application->MessageBox(temp,"ECOPL",MB_OK);
                if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',"");
                }

            else if (mrgP != 0 && P_LIENS[pc][1]=="Oui")
                {
                 strcpy(ttmp,P_COL_LIEN[pc][1].c_str());
                 coul1=atoi(ttmp);
                 if (P_EPAIS_LIEN[pc][1]=="") P_EPAIS_LIEN[pc][1]="2.0";
                 valf=AnsiToFloat(P_EPAIS_LIEN[pc][1]);
                 epaiss =  (int)(valf*COEFMM);
                 style = GetStyle(P_STYLELIEN[pc][1]);

                 valf=AnsiToFloat(P_EPTRAIT[pc][1]);
                 ept = (int)(valf*COEFMM);
                 y11 = y1 + ept/2; y22 = y2+ept/2;
                 //sprintf(temp,"MrgP!=0 T=%d X1=%d X2=%d",indx,x1,x2);
                 // Application->MessageBox(temp,"ECOPL",MB_OK);

                 if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',""); // style = 2
                 }

              /*
              else if (mrgR == 0 && P_LIENS[pc][2]=="Oui")
                 {
                  strcpy(ttmp,P_COL_LIEN[pc][2].c_str());
                coul1=atoi(ttmp);
                if (P_EPAIS_LIEN[pc][2]=="") P_EPAIS_LIEN[pc][2]="2.0";
                valf=AnsiToFloat(P_EPAIS_LIEN[pc][2]);
                epaiss =  (int)(valf*COEFMM);
                style = GetStyle(P_STYLELIEN[pc][2]);

                valf=AnsiToFloat(P_EPTRAIT[pc][2]);
                ept = (int)(valf*COEFMM);
                y11 = y1 + ept/2; y22 = y2+ept/2;
                if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',"");
                }
              else if (mrgR != 0 && P_LIENS[pc][3]=="Oui")
                 {
                  strcpy(ttmp,P_COL_LIEN[pc][3].c_str());
                coul1=atoi(ttmp);
                if (P_EPAIS_LIEN[pc][3]=="") P_EPAIS_LIEN[pc][3]="2.0";
                valf=AnsiToFloat(P_EPAIS_LIEN[pc][3]);
                epaiss =  (int)(valf*COEFMM);
                style = GetStyle(P_STYLELIEN[pc][3]);

                valf=AnsiToFloat(P_EPTRAIT[pc][3]);
                ept = (int)(valf*COEFMM);
                y11 = y1 + ept/2; y22 = y2+ept/2;
                if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',"");
                }
               */



           }
        }
      }
   }
}



void __fastcall TplStandard::Trace_Liens_Reel()
{
 int indx, count, nbsc;   char tmpxx[100];
 char tmp[500];  char marg[20];
 int x1,y1,x2,y2,y11,y22; int rc,mrgP,mrgR,ress;
 int loop; int tsucc,ept;
 char xxx[50];
 int coul1,coul2,epaiss,style;
 char ttmp[30];
 float valf;

 NB_DETAIL[pc]=asTri->RowCount-1;
 if (NB_DETAIL[pc]==0) return;

 x1=P_DEBUTGANTT[pc];
 for (indx=1;indx<=NB_DETAIL[pc];indx++)
   {
    if (filtr[indx] != 'X') continue;
    strcpy(xxx,asTri->Cells[63][indx].c_str());
    nbsc = atoi(xxx); // StrToInt(asTri->Cells[63][indx]);
    // sprintf(tmpxx,"CELLS 63 %d",nbsc);
    // Application->MessageBox(tmpxx,"Trace Liens Reel",MB_OK);
    if (nbsc>0 && nbsc<50)
      {
       // coordonnees de fin
       strcpy(xxx,asTri->Cells[65][indx].c_str());
       x1=atoi(xxx);
       strcpy(xxx,asTri->Cells[66][indx].c_str());
       y1=atoi(xxx);
       strcpy(tmp,asTri->Cells[64][indx].c_str());
       //sprintf(tmpxx,"REEL tmp=%s",tmp);
       //Application->MessageBox(tmpxx,"ECOPL LIENS REELS",MB_OK);
       count=Explode('|',tmp);
       for (loop=1;loop<count;loop++)
        {


          tsucc=atoi(params[loop]);
          if (filtr[tsucc] != 'X') continue;
          if (tsucc==9999) rc=0; else rc = Lookup(tsucc);


          if (rc > 0)
           {

            strcpy(xxx,asTri->Cells[60][rc].c_str());
            x2=atoi(xxx);
             strcpy(xxx,asTri->Cells[66][rc].c_str());
            y2=atoi(xxx);
            strcpy(marg,asTri->Cells[8][rc].c_str());
            mrgP = atoi(marg);
            ress = Row_AsAvanc(rc);
            mrgR = atoi(asAvanc->Cells[15][ress].c_str());

            if (indx==11)
            {
              //sprintf(temp,"%d ",indx);
              //sprintf(tmpxx,"REEL Tache %d  Explode %d RC LOOKUP %d X1=%d X2=%d",indx,loop,rc,x1,x2);
              //sprintf(temp,"REEL Tache %d  Explode %d RC LOOKUP %d X1=%d X2=%d",indx,loop,rc,x1,x2);
              //Application->MessageBox(tmpxx,"ECOPL",MB_OK);
            }


            if  (mrgP==0 && P_LIENS[pc][0]=="Oui")
               {
                strcpy(ttmp,P_COL_LIEN[pc][0].c_str());
                coul1=atoi(ttmp);
                if (P_EPAIS_LIEN[pc][0]=="") P_EPAIS_LIEN[pc][0]="2.0";
                valf=AnsiToFloat(P_EPAIS_LIEN[pc][0]);
                epaiss =  (int)(valf*COEFMM);
                style = GetStyle(P_STYLELIEN[pc][0]);

                valf=AnsiToFloat(P_EPTRAIT[pc][0]);
                ept = (int)(valf*COEFMM);
                y11 = y1 + ept/2; y22 = y2+ept/2;
                if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',"");
                }

            else if (mrgP != 0 && P_LIENS[pc][1]=="Oui")
                {
                 strcpy(ttmp,P_COL_LIEN[pc][1].c_str());
                 coul1=atoi(ttmp);
                 if (P_EPAIS_LIEN[pc][1]=="") P_EPAIS_LIEN[pc][1]="2.0";
                 valf=AnsiToFloat(P_EPAIS_LIEN[pc][1]);
                 epaiss =  (int)(valf*COEFMM);
                 style = GetStyle(P_STYLELIEN[pc][1]);

                 valf=AnsiToFloat(P_EPTRAIT[pc][1]);
                 ept = (int)(valf*COEFMM);
                 y11 = y1 + ept/2; y22 = y2+ept/2;
                 if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',""); // style = 2
                 }
              else if (mrgR == 0 && P_LIENS[pc][2]=="Oui")
                 {
                  strcpy(ttmp,P_COL_LIEN[pc][2].c_str());
                coul1=atoi(ttmp);
                if (P_EPAIS_LIEN[pc][2]=="") P_EPAIS_LIEN[pc][2]="2.0";
                valf=AnsiToFloat(P_EPAIS_LIEN[pc][2]);
                epaiss =  (int)(valf*COEFMM);
                style = GetStyle(P_STYLELIEN[pc][2]);

                valf=AnsiToFloat(P_EPTRAIT[pc][2]);
                ept = (int)(valf*COEFMM);
                y11 = y1 + ept/2; y22 = y2+ept/2;
                if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',"");
                }
              else if (mrgR != 0 && P_LIENS[pc][3]=="Oui")
                 {
                  strcpy(ttmp,P_COL_LIEN[pc][3].c_str());
                coul1=atoi(ttmp);
                if (P_EPAIS_LIEN[pc][3]=="") P_EPAIS_LIEN[pc][3]="2.0";
                valf=AnsiToFloat(P_EPAIS_LIEN[pc][3]);
                epaiss =  (int)(valf*COEFMM);
                style = GetStyle(P_STYLELIEN[pc][3]);

                valf=AnsiToFloat(P_EPTRAIT[pc][3]);
                ept = (int)(valf*COEFMM);
                y11 = y1 + ept/2; y22 = y2+ept/2;
                if (x1>=P_DEBUTGANTT[pc] &&  x2>=P_DEBUTGANTT[pc])
                   Insert_G('T',"AR",x1,y11,x2,y22,"LK",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',"");
                }




           }
        }
      }
   }
}



// ***************************************************
//
// ***************************************************

int __fastcall TplStandard::Lookup(int t)
{
 int ind,i; int cnt,tache;
 char tmp[150];
 cnt = asTri->RowCount-1;
 if (cnt==0) return 0;
 ind = 0;
 for (i=1;i<=cnt;i++)
   {
    strcpy(tmp,asTri->Cells[1][i].c_str());
    tache=atoi(tmp);
    if (tache==t) { ind=i; break; }
   }
 return ind;
}

// ***************************************************
//
// ***************************************************



void __fastcall TplStandard::Clean_Footer()
{
 // tracer un rectangle de Y0 au bas de page
 int x1,y1,x2,y2;

 x1=0; x2=P_W[pc];
 y1=Y0-2; y2=P_H[pc];
 Insert_G('0',"RT",x1,y1,x2,y2,"DT",clWhite,clWhite,0,0,"",' ',' ',"");
 // trace un trait s�parateur
 Insert_G('0',"RT",x1,y1,x2,y1+1,"DT",clBlack,clBlack,0,0,"",' ',' ',"");


}

// ***************************************************
//
// ***************************************************

void __fastcall TplStandard::Prep_Rapport(int npage)
{
 char tmp[200];
 int n,h, maxhh;
 int Y0_Init;
 int pagecounter;

 AnsiString txt,C,Cr[8]; // criteres
 bool f[8]; char str[500];
 int i,crit; int nb_l; int col,numchamp,margt;

 // HET contient Hauteur En Tete
 // Calcul Hauteur Entete S'il y en a  hauteur_entete , hauteur_pied
 // Calcul Hauteur Pied s'il y a lieu

  // Calcul Hauteur  Maximum pour la ligne

 // Calcul Taille Pied : deja calculee dans prepare_variables


 strcpy(tmp,P_CDV[pc].c_str());
      P_CDVV[pc]=atoi(tmp);
 if (P_CDVV[pc]> 1000) { P_CDVV[pc]=1000;  P_CDV[pc] = AnsiString(P_CDVV[pc]); }

 // pour chaque page , calculer le nb de ruptures et de taches a placer
 for (i=0;i<5000;i++) SPAGE[i]=0;
 NB_DETAIL[pc] = asTri->RowCount-1;
 if (NB_DETAIL[pc] <=1) return;
 for (i=0;i<8;i++) { f[i]=true; Cr[i]=""; }

  nb_l = NB_DETAIL[pc]; // + P_RUPT0[pc];
  if (nb_l == 0) return;
  P_LST[pc]=2;             // $$$$$


pagecounter = 1;

// SAUT DE PAGE

Hauteur_Gen = D_TOP[pc];
INDXPAGE = 0;
SPAGE[INDXPAGE] = 'P';
Y[INDXPAGE] = Hauteur_Gen;

// ENTETE
// $$$$ INDXPAGE = 1;
if (P_ENTETE[pc]=='O')
 {
   // $$$$Y[INDXPAGE] = Hauteur_Gen;
   //$$$$ SPAGE[INDXPAGE] = 'E';
   if (pagecounter==npage) { Y0 = Y[INDXPAGE]; /*Trace_Entete()*/; }
   Hauteur_Gen += HET;
 }

 // TITRES DES COLONNES   //  T_LARG = hauteur du texte
//$$$$  INDXPAGE++; // 2
// $$$$ SPAGE[INDXPAGE] = 'C';

// $$$$ Y[INDXPAGE] = Hauteur_Gen;
MAX_PAGE_RAPPORT[pc]=1;
if (pagecounter==npage) { Y0=Hauteur_Gen; //Y[INDXPAGE];
   /* Trace_Calendrier(); */ }
Hauteur_Gen += H_CAL1[pc]+H_CAL2[pc]; // P_HAUT[pc][1]*3/2; // $$$$$

for (i=1;i<=NB_DETAIL[pc];i++)
   {
    for (crit=0; crit< 8;crit++)
     {
      if (P_ATTR[pc][crit]!='Z')
       {
        n=P_TRICOL[pc][crit];
        if (P_TRICOL[pc][crit] >40) C=asTri->Cells[crit+41][i]; else C=asTri->Cells[n][i];
        if (C != Cr[crit])
         { if (!f[crit])
               { }
           if (P_ATTR[pc][crit]=='L')
             {
              strcpy(tmp,T_TLARG[pc][crit].c_str());
              h = atoi(tmp);
              Update_Hauteur(h,pagecounter,npage);
              INDXPAGE++;
              SPAGE[INDXPAGE]='L';
              Y[INDXPAGE] = Hauteur_Gen;
              if (pagecounter==npage)
                { Y0 = Y[INDXPAGE];
                  if (P_TRICOL[pc][crit] >40) txt = asTri->Cells[crit+51][i];
                    else txt = asTri->Cells[n][i];
                 
                }
              T[INDXPAGE] = C;
              Hauteur_Gen += h;
             }
         }


        Cr[crit]=C;
      }  // if != Z
     } // for
    Update_Hauteur(P_CDVV[pc],pagecounter,npage);
    INDXPAGE++;
    SPAGE[INDXPAGE] = 'T';
    Y[INDXPAGE] = Hauteur_Gen;
    T[INDXPAGE] = "Tache " + AnsiString(i);
    T[INDXPAGE] = asTri->Cells[2][i];  //"Tache " + AnsiString(i);
      strcpy(tmp,asTri->Cells[1][i].c_str());
    TCH[INDXPAGE] = atoi(tmp);
    if (pagecounter == npage)
       {
        Y0=Y[INDXPAGE];
        /* Trace_Col_Rapport(i); */
       }

    Hauteur_Gen += P_CDVV[pc];  // pour chaque Tache
   }  // for
 // Inclure un Pied de page ici (fin de rapport)
 Update_Hauteur(Hauteur_Pied,pagecounter,npage);
 INDXPAGE++;
 SPAGE[INDXPAGE]='P';
 Y[INDXPAGE] =  Hauteur_Gen;
   if (pagecounter== npage && P_PIED[pc]=='O')
     {
      Y0=Y[INDXPAGE];
      /* Trace_Pied(); */
     }
 INDXPAGE++;
 SPAGE[INDXPAGE]='F';  // END !!!!
Trace_Precalcul(npage);
}

void __fastcall TplStandard::Update_Hauteur(int hl,int pagecounter,int npage)
{
 int hbas;
 int reste;
 if (P_PIED[pc]=='O') hbas = D_BOTTOM[pc] - Hauteur_Pied;
     else hbas = D_BOTTOM[pc];
 if (Hauteur_Gen + hl > hbas)
   {
    if (P_MPPIED[pc]=='O')
      {
       //INDXPAGE++;
       //SPAGE[INDXPAGE]='B';  // bottom
       //Y[INDXPAGE] = Hauteur_Gen;
       if (npage==pagecounter && P_PIED[pc]) { Y0 = Y[INDXPAGE]; /* Trace_Pied(); */}
      }
    INDXPAGE++;
    Hauteur_Gen = D_TOP[pc];
    MAX_PAGE_RAPPORT[pc]++;
    SPAGE[INDXPAGE]='P';
    Y[INDXPAGE] = D_TOP[pc];
    if (P_MPENTETE[pc]=='O')
      {
       //INDXPAGE++;
       //SPAGE[INDXPAGE]='E';
       //Y[INDXPAGE] = Hauteur_Gen;
       if (npage==pagecounter)
         { Y0 = Y[INDXPAGE]; /* Trace_Entete();   */
         Hauteur_Gen += HET;
         }
      }

    //$$$ INDXPAGE++;
    //$$$SPAGE[INDXPAGE] = 'C';
    //$$$Y[INDXPAGE] = Hauteur_Gen;
    if (pagecounter==npage) { Y0=Y[INDXPAGE]; /* Trace_Calendrier(); */}
    Hauteur_Gen += P_HAUT[pc][1]*3/2;
   }
return;
}

void __fastcall TplStandard::Trace_Precalcul(int npage)
{
 int i_sp;  // index dans SPAGE;
 int counter,from,to;
 int indpage;
 char tmp[200];
 AnsiString txt,C,Cr[8]; // criteres
 bool f[8]; char str[500];
 int i,n,h,crit; int nb_l; int col,numchamp,margt;
 // INDXPAGE pointe sur le dernier element
 i_sp =0; counter=0;
 // recherche la page a afficher
 while (i_sp<=INDXPAGE)
   {
    if (SPAGE[i_sp]=='P') counter++;
    if (counter==npage) { from=i_sp; break; }
    i_sp++;
   }
 i_sp++;
 while (i_sp<=INDXPAGE)
   {
    if (SPAGE[i_sp]=='P') { to=i_sp; break; }
    i_sp++;
   }

Y0 = Y[from];
Trace_Entete();
Trace_Calendrier();

i_sp=1;

for (i=1;i<=NB_DETAIL[pc];i++)
   {
    for (crit=0; crit< 8;crit++)
     {
      if (P_ATTR[pc][crit]!='Z')
       {
        n=P_TRICOL[pc][crit];
        if (P_TRICOL[pc][crit] >40) C=asTri->Cells[crit+41][i]; else C=asTri->Cells[n][i];
        if (C != Cr[crit])
         { if (!f[crit])
               { }
           if (P_ATTR[pc][crit]=='L')
             {
              strcpy(tmp,T_TLARG[pc][crit].c_str());
              h = atoi(tmp);
              //Update_Hauteur(h,pagecounter,npage);
              //INDXPAGE++;
              //SPAGE[INDXPAGE]='L';
              //Y[INDXPAGE] = Hauteur_Gen;
              i_sp++;
              if (i_sp>from && i_sp<to)
                { Y0 = Y[i_sp];
                  if (P_TRICOL[pc][crit] >40) txt = asTri->Cells[crit+51][i];
                    else txt = asTri->Cells[n][i];
                   Commencer(crit,txt,i,true);
                }
             }
         }


        Cr[crit]=C;
      }  // if != Z
     } // for
    strcpy(tmp,asTri->Cells[1][i].c_str());
    i_sp++;
    if (i_sp>from && i_sp<=to)
      { Y0 = Y[i_sp];
        Trace_Col_Rapport(i);
       }
   }  // for
Y0 = Y0+P_CDVV[pc];
Trace_Pied();


}


void __fastcall TplStandard::Prepare_Hauteur()
{
 


 // calcule la hauteur des taches
 char str[1500],tmp[200];
 char dt1[15]; char ital,gras;
 int j1,m1,a1,dist1,dist2,base;
 bool result; int nn;
 int x1,x2,x11,x22,xmax;
 AnsiString txt;
 int i,count,k,col,taille,ctrenv;
 int MemoY0,EstP_CDVV,Mem_PCDVV;
 AnsiString C,Code,Cr[8],CrEnv[8],ccc;; // criteres
 bool f[8];
 AnsiString Cniv,MemoText;
 int n,crit; int nb_l; //int x1,y1,x2,y2; int col,numchamp,margt;
 int xx,ptr_nbtask[9];    int nivcrit;
 int H_RUPT,H_ENVEL;  int j;

 for (i=0;i<40;i++) { for (j=0;j<10000;j++) FLAG_BRISEE[i][j]=-1; }

 ctrenv=0;
 NB_DETAIL[pc] = asTri->RowCount-1;
 if (NB_DETAIL[pc] <=1) {return; }

  for (i=0;i<8;i++) { f[i]=true; Cr[i]=""; CrEnv[i]="";}
  for (xx=0;xx<8;xx++) ptr_nbtask[xx]=1;
  Code=""; MemoText="";

 Nb_Rupt=0; Nb_Ligne=0; Nb_Envel=0;   H_RUPT = 0;  H_ENVEL = 0;
 // for (i=0;i<8;i++) Nb_Rupt = Nb_Rupt + P_RUPT[pc][i];   // calcul dans prep rupt

 Mem_PCDVV = P_CDVV[pc];
 EstP_CDVV = P_H[pc] / (NB_DETAIL[pc] +5);
 P_CDVV[pc] = EstP_CDVV;

 YY0=Y0; MemoY0=Y0;
 nivcrit=0;
 for (i=1;i<=NB_DETAIL[pc];i++)
  {
    strcpy(dt1,asTri->Cells[4][i].c_str());  // date debut prev
    dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    dist1 = convert_date(a1+2000,m1,j1);
    strcpy(dt1,asTri->Cells[5][i].c_str());  // date fin prev
    dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    dist2 = convert_date(a1+2000,m1,j1) + 1;
     base = Compute_Base();
     //IXDEB=40;
     x1 = P_DEBUTGANTT[pc]+3 + base*Compute_Trav(dist1-IXDEB,1)/P_MAXDIST[pc];
     x2 = P_DEBUTGANTT[pc]+3 + base*Compute_Trav(dist2-IXDEB,1)/P_MAXDIST[pc];


     for (k=0;k<8;k++) Compute_Enveloppe(k,i,x1,x2,dist1,dist2);
    xmax = Compute_LargeurMax(i,false);
    asTri->Cells[70][i]=xmax;
    asTri->Cells[65][i]=xmax;


    result=false;
    if (pc==3) result = Insert_Block(0,i,0);
    if (P_GENRE[pc]=='G') Nb_Ligne++;
     else
       {
        for (crit=0; crit < NUM_CRIT[pc]; crit++)
         {
         nn=P_TRICOL[pc][crit];
         ccc =  asTri->Cells[crit+71][i];
         if (P_TRICOL[pc][crit] >40)
         C = extract_rupt(asTri->Cells[crit+71][i],crit);
         else C=asTri->Cells[nn][i];

         Cniv = ChercheCode(crit,C);  // asTri->Cells[71+crit-1][i]);
         if (Cniv !=  Cr[crit]) //if (C != Cr[crit])
            {
             //if (CheckNiv(C,crit))
             //  {
                 if (P_TRICOL[pc][crit] >40)
                   txt = text_rupt(crit,C); //asTri->Cells[crit+51][i];
                 else txt = asTri->Cells[nn][i];
                 if (txt != "")
                     {
                      nivcrit = crit;

                     }
                  else
                     {
                      // Commencer(crit,"",i,true);
                     }
              if (crit==0) {Cr[1]=""; Cr[2]=""; Cr[3]="";}
              if (crit==1) {Cr[2]=""; Cr[3]="";}
              if (crit==2) {Cr[3]="";}
             }
          } // For crit   
          Cr[crit] = Cniv;
          result = Insert_Block(nivcrit,i,0);
           if (!result)
           Nb_Ligne++;

       }

    asTri->Cells[66][i]=YY0;


    strcpy(P0,"");  strcpy(P1,""); strcpy(P2,"");strcpy(P3,"");


    for (crit=0; crit < NUM_CRIT[pc]; crit++)
      {
         n=P_TRICOL[pc][crit];
         if (P_TRICOL[pc][crit] >40)
         C = extract_rupt(asTri->Cells[crit+71][i],crit);
         else C=asTri->Cells[n][i];

         Cniv = ChercheCode(crit,C);  // asTri->Cells[71+crit-1][i]);
         if (Cniv !=  Cr[crit]) //if (C != Cr[crit])
            {
             //if (CheckNiv(C,crit))
              // {

               if (P_ATTR[pc][crit]!='C' && P_ATTR[pc][crit] != 'Z')  // L,T,Espace
                    {
                     Nb_Rupt++;
                     }
               // }
             }
          Cr[crit] = Cniv;
        //   Trac� des Enveloppes
        if (NB_TASK[crit][ptr_nbtask[crit]] == i)  //
          {
           ptr_nbtask[crit]++;
           n=P_TRICOL[pc][crit];
           if (P_TRICOL[pc][crit] >40)
           C = extract_rupt(asTri->Cells[crit+71][i],crit);
           else C=asTri->Cells[n][i];
          if (C != CrEnv[crit])
            {// if (!f[crit]) { ??? }
             //if (CheckNiv(C,crit))
             //  {
              if (P_TRICOL[pc][crit] >40) txt = text_rupt(crit,C); //asTri->Cells[crit+51][i];
               else txt = asTri->Cells[n][i];
                if (MemoText != txt && P_ENV[pc][crit]==1)
                   { Nb_Envel++;
                     // H_ENVEL = H_ENVEL + StrToInt(E_TLARG[pc][crit]);
                   }
             }
         MemoText = txt; CrEnv[crit]=C;
          }  // if
      }
    } // For i

   //  Calculer l'espace qu'il reste

 if (pc==0) Nb_Ligne = NB_DETAIL[pc];
 Y0=MemoY0;

 if (P_DETAIL[pc]==0) Nb_Ligne=0;
 // Y0 n'est pas modifie
 //  HAUTEUR = P_H[pc] - MemoY0 - Hauteur_CalBas - Hauteur_Pied - P_MARGH2[pc];
 HAUTEUR = P_H[pc] - P_MARGH1[pc] - HET -  Hauteur_CalHaut - Hauteur_CalBas - Hauteur_Pied - P_MARGH2[pc];
 HAUTEUR = HAUTEUR - Nb_Rupt*3 - Nb_Envel*3;  // Epaisseur Env et Rupt
  if (pc==0) { Nb_Envel=0; Nb_Rupt=0;
       m_automatic[pc]=' ';}

 if (Check_Automatic() && pc>=0)  // cas _special pc=0
   {
     if (Nb_Ligne > 0)
      {
        P_CDVV[pc]=HAUTEUR/(Nb_Ligne*1.05 + Nb_Rupt + Nb_Envel);  //Nb_Envel);
        //P_CDVV[pc]=HAUTEUR/Nb_Ligne;  // + Nb_Rupt + Nb_Envel);  //Nb_Envel);
        P_CDV[pc] = AnsiString(P_CDVV[pc]);
       }
      else { P_CDVV[pc] = 100; P_CDV[pc] = "100"; }
      }
   else
    {
     P_CDVV[pc]=Mem_PCDVV;
     P_CDV[pc] = AnsiString(P_CDVV[pc]);
     
    }

 if (P_CDVV[pc]> 1000) { P_CDVV[pc]=1000;  P_CDV[pc] = AnsiString(P_CDVV[pc]); }
 // determiner nouvelle Hauteurs des polices

 if (Check_Automatic())
P_SIZET[pc][0][0] = P_CDVV[pc] / 5;
}

//////////////////////////////////////////////////////
int __fastcall TplStandard::Compute_LargeurMax(int i,bool real)
{
 int pos,k,count,margt,col,taille;
 int x1,x11,y1,x2,x22,x3,x4;
 char dt1[20];    int type;
 int dist1,dist2,dist3,dist4,base;
 int j1,m1,a1;
 AnsiString S; int numtache;
 char str[250];
 AnsiString txt;

 int XMAX;

 if (!real)
  {
   margt = AnsiToInt(asTri->Cells[8][i]);
   if (margt==0) type = 0; else type = 1;
  }
  else
   {
    margt = AnsiToInt(asTri->Cells[8][i]);         // ����������� 28 ???
   if (margt==0) type = 2; else type = 3;
   }
 XMAX = 0;

    // strcpy(dt1,asTri->Cells[4][i].c_str());  // date debut reelle
    // dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    // dist1 = convert_date(a1+2000,m1,j1);
     if (P_TYPE[pc]=='R')
       dist1 = Convert_Date(asTri->Cells[21][i]);
      else dist1 = Convert_Date(asTri->Cells[4][i]);

    //strcpy(dt1,asTri->Cells[5][i].c_str());  // date fin reelle
    //dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    // dist2 = convert_date(a1+2000,m1,j1) + 1;
      if (P_TYPE[pc]=='R')
     dist2 = Convert_Date(asTri->Cells[5][i]);
       else dist2 = Convert_Date(asTri->Cells[22][i]);
    //strcpy(dt1,asTri->Cells[7][i].c_str());  // date fin au plus tard  (pour la marge)
    //dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    //dist3 = convert_date(a1+2000,m1,j1) + 1;
    dist3 = Convert_Date(asTri->Cells[5][i]);

    base = Compute_Base();
    //IXDEB=40;
    x1 = P_DEBUTGANTT[pc]+3 + base*Compute_Trav(dist1-IXDEB,1)/P_MAXDIST[pc];
    x2 = P_DEBUTGANTT[pc]+3 + base*Compute_Trav(dist2-IXDEB,1)/P_MAXDIST[pc];
    x3 = x2;
    x4 = P_DEBUTGANTT[pc]+3 + base*Compute_Trav(dist3-IXDEB,1)/P_MAXDIST[pc];

    if (pc==0) x4 = x2;  // on ne trace pas la marge

    for (pos=0;pos < 4; pos++)      // Haut,Droite,Gauche,Bas
    {
      if (P_INF[pc][type][pos]=="Oui")
      {

        // Champs dans P_INFO  [prev]
        strcpy(str,P_INFO[pc][0][pos].c_str());
        count = Explode('|',str); txt="";
        for (k=0;k<count;k++)
           {
             if (strcmp(params[k],"Successeurs")==0) txt = txt + Affich_Successeurs(i) + " ";
             else  if (strcmp(params[k],"Pr�d�cesseurs")==0) txt = txt + Affich_Predecess(i) + " ";
             else
              {
               col=RechercheCol(params[k]);
               if (col>0) txt=txt+asTri->Cells[col][i];
               txt=txt+" ";
              }
           }
       taille = AnsiToInt(P_SIZET[pc][type][pos]);
       Compute_LH(P_POLT[pc][type][pos],txt,taille,P_ITALT[pc][type][pos],P_BOLDT[pc][type][pos]);
       x11=x1;
       x22=x11+L;  // H Contient la hauteur
        if (pos == 1)

          { if (P_MARGE[pc][1]=="Oui")
             {x11= x4+10; x22=x11+L;
              }
            else
             {x11= x2+10; x22=x11+L;
             }
           }
        if (pos == 2) // Gauche
             {x11= x1-L-10; x22=x11+L;
              }

     } // if P_INF = "Oui"
    if (x22>XMAX) XMAX=x22;
    }  // For Pos
 return XMAX;
}


int __fastcall TplStandard::Compute_Nb_Enveloppes()
{
int i,k; int ctr;
ctr=0;
for (k=0;k<8;k++)
{
  for (i=1;i<MAX_RUPT;i++)
   {
    if (NB_TASK[k][i]!=0) ctr++;
   }
}
return ctr;

}


void __fastcall TplStandard::Prepare_Enveloppes()
{


}

void __fastcall TplStandard::Compute_Enveloppe(int k,int i,int x1,int x2,int d1,int d2)
{
 int ind,from,nbt;
 bool ok;
 int crit; AnsiString Cr[8],C;

 // K est le numero de ruptures
 // i est le numero de tache

 ind=1;
 ok=true;
 while(ok)   // recherche tranche des taches
  {
    if (NB_TASK[k][ind] <= i)ind++ ;
    else ok=false;
    if (ind>=MAX_RUPT) { ind = 1; ok = false; }
  }
 ind--;

 if (x1<ENV_MIN[k][ind]) ENV_MIN[k][ind]=x1;
 if (x2>ENV_MAX[k][ind]) ENV_MAX[k][ind]=x2;
 if (d1<ENV_D1[k][ind]) ENV_D1[k][ind]=d1;
 if (d2>ENV_D2[k][ind]) ENV_D2[k][ind]=d2;

 /*
 for (crit=0; crit< 8;crit++)
     {
      if (P_ATTR[pc][crit]!='Z')
       {
        if (P_ENV[pc][crit]==1 && i==NB_TASK[k][ind]) Nb_Envel++;
       }
     }
 */
}

AnsiString __fastcall TplStandard::extract_rupt(AnsiString Crit,int numcrit)
{
 char tmp[200];
 char *p0,*p1,*p2;

 strcpy(tmp,Crit.c_str());
 // extraire le niveau  d'apres les criteres
 p0=p1=p2=NULL;
  p0 =strstr(tmp,"-");
  if (p0)
    { p1=strstr(p0+1,"-");
      if (p1)
        {
         p2 = strstr(p1+1,"-");
        }
    }
if (P_NIV0[pc][numcrit] && p0) *(p0+1) = 0;
if (P_NIV1[pc][numcrit] && p1) *(p1+1) = 0;
if (P_NIV2[pc][numcrit] && p2) *(p2+1) = 0;

return AnsiString(tmp);
}

AnsiString __fastcall TplStandard::text_rupt(int critnum ,AnsiString Crit)
{
 int cnt,niv,i,len; bool ok;
 char tmp[100],tmp1[100],tmp2[100];
 char *p,*p2;

 strcpy(tmp,Crit.c_str());
 len = strlen(tmp);
 if (len==0) return "";
 ok = true;
 while (ok)
   {
    if (tmp[len-1] == '-') tmp[len-1]=0;
      else ok=false;
    len = strlen(tmp);
   }

 // compter le nombre de "-" restants
 p=NULL; cnt=0;
 p2=tmp;
 p=strstr(p2,"-");
 while (p)
   {
    cnt++;
    p2=p+1;
    p=strstr(p2,"-");
   }

 if (critnum > 0)
   {
    if (P_NIV0[pc][critnum] == 1  && cnt != 0) return "";
    if (P_NIV1[pc][critnum] == 1  && cnt != 1) return "";
    if (P_NIV2[pc][critnum] == 1  && cnt != 2) return "";
    if (P_NIV3[pc][critnum] == 1  && cnt != 3) return "";
   }
  //// 
  strcat(tmp,"-!");


  for (i=0;i<LibCtr;i++)
   {
    strcpy(tmp1,LibRupt[i].c_str());
    p=NULL;
    p=strstr(tmp1,tmp);
    if (p != NULL)
      {
       p= strstr(tmp1,"!");
       if (p != NULL) strcpy(tmp2,p+1);
       else strcpy(tmp2,"");

       if (strlen(tmp2)==0)
         {
          tmp2[0]=0;
         }
       // strcpy(tmp2,p+2);
       return (AnsiString(tmp2));
       //break;
      }

   }
 return "";  // Crit
}

AnsiString __fastcall TplStandard::text_criteres(int critnum ,AnsiString Crit)
{
 int cnt,niv,i,len; bool ok;
 char tmp[200],tmp1[100],tmp2[100];
 char *p,*p2;

 strcpy(tmp,Crit.c_str());
 len = strlen(tmp);
 if (len==0) return "";
 ok = true;
 while (ok)
   {
    if (tmp[len-1] == '-') tmp[len-1]=0;
      else ok=false;
    len = strlen(tmp);
   }

 // compter le nombre de "-" restants
 p=NULL; cnt=0;
 p2=tmp;
 p=strstr(p2,"-");
 while (p)
   {
    cnt++;
    p2=p+1;
    p=strstr(p2,"-");
   }

  strcat(tmp,"-!");


  for (i=0;i<LibCtr;i++)
   {
    strcpy(tmp1,LibRupt[i].c_str());
    p=NULL;
    p=strstr(tmp1,tmp);
    if (p != NULL)
      {
       p= strstr(tmp1,"!");
       if (p != NULL) strcpy(tmp2,p+1);
       else strcpy(tmp2,"");

       if (strlen(tmp2)==0)
         {
          tmp2[0]=0;
         }
       // strcpy(tmp2,p+2);
       return (AnsiString(tmp2));
       //break;
      }

   }
 return "";  // Crit
}






// ***************************************************
//
// ***************************************************

void __fastcall TplStandard::Trace_Taches()
{
 AnsiString xxx,txt,C,Code,Cr[8],CrEnv[8];; // criteres
 char CTXT[2000];
 bool f[8]; char str[500],text[200];
 TRect Src,Dst; AnsiString Cniv,MemoText;
 int i,n,crit; int nb_l; int x1,y1,x2,y2;
 int tmpcol,col,numchamp,margt;
 TColor couleur1,couleur2;
 int xx,ptr_nbtask[9];
 int nblignes,lcol;
 bool res,ok; int maxloop;
 int MaxSizeText;
 int nivcrit;  char *p;
 char tmp[500]; AnsiString S; int len;
 AnsiString Temp,ccc;  bool OK;

 nblignes = 0;

 LastY0 = Y0;
 NB_DETAIL[pc] = asTri->RowCount-1;
 if (NB_DETAIL[pc] <=1) {//Application->MessageBoxA("Pas de Taches",m_ecoplan,MB_OK);
                         return; }
 for (i=0;i<8;i++) { f[i]=true; Cr[i]=""; CrEnv[i]="";}
  for (xx=0;xx<8;xx++) ptr_nbtask[xx]=1;

Code=""; MemoText="";
nivcrit = 0;
for (i=1;i<=NB_DETAIL[pc];i++)
 {
     ///// NON !!!!!!! Application->ProcessMessages();
     strcpy(P0,"");  strcpy(P1,""); strcpy(P2,"");strcpy(P3,"");

     for (crit=0; crit < NUM_CRIT[pc]; crit++)
      {
         n=P_TRICOL[pc][crit];
         ccc =  asTri->Cells[crit+71][i];
         if (P_TRICOL[pc][crit] >40)
         C = extract_rupt(asTri->Cells[crit+71][i],crit);
         else C=asTri->Cells[n][i];

         Cniv = ChercheCode(crit,C);  // asTri->Cells[71+crit-1][i]);
         if (Cniv !=  Cr[crit]) //if (C != Cr[crit])
            {
             //if (CheckNiv(C,crit))
             //  {
                 if (P_TRICOL[pc][crit] >40)
                   txt = text_rupt(crit,C); //asTri->Cells[crit+51][i];
                 else txt = asTri->Cells[n][i];
                 if (txt != "")
                     {
                      Commencer(crit,txt,i,true);
                      nivcrit = crit;
                     //YY0 = Y0;
                     //asTri->Cells[66][i]=Y0;

                     }
                  else
                     {
                      Commencer(crit,"",i,true);

                     }
              if (crit==0) {Cr[1]=""; Cr[2]=""; Cr[3]="";}
              if (crit==1) {Cr[2]=""; Cr[3]="";}
              if (crit==2) {Cr[3]="";}
             }
          Cr[crit] = Cniv;
        //   Trac� des Enveloppes
        if (NB_TASK[crit][ptr_nbtask[crit]] == i && pc !=0)  // ==> commencer
          {
           ptr_nbtask[crit]++;
           n=P_TRICOL[pc][crit];
           if (P_TRICOL[pc][crit] >40)
           C = extract_rupt(asTri->Cells[crit+71][i],crit);
           else C=asTri->Cells[n][i];
          if (C != CrEnv[crit])
            {// if (!f[crit]) { ??? }
             //if (CheckNiv(C,crit))
             //  {
              if (P_TRICOL[pc][crit] >40) txt = text_rupt(crit,C); //asTri->Cells[crit+51][i];
               else txt = asTri->Cells[n][i];
                //if (MemoText != txt)
                 if (MemoText != txt && P_ENV[pc][crit]==1)
                 {
                  Trace_Enveloppe(crit,i,txt);
                 }

             }
         MemoText = txt;
         CrEnv[crit]=C;

          }  // if

      }  // For crit;

    // Trac� des colonnes
    // Y0 est toujours la base

  if (P_DETAIL[pc]==1)
   { for (col=0;col<P_COL[pc];col++)
     { tmpcol=col; numchamp = P_TRICOL[pc][col];
       if (P_GENRE[pc]=='G' && P_GRILLE[pc]=='O')
         {
        //  if (numchamp==64) numchamp=65;
        OK = false;
        if (numchamp < 40 && numchamp > 0)  OK = true;
        if (numchamp == 62 || numchamp==64) OK=true;
        if (OK && P_ATTR[pc][col]=='C') OK=true;

        if (OK)
        // if  (((numchamp <40 && numchamp>0) || (numchamp==62) || numchamp==64)) && P_ATTR[pc][col]=='C') //(numchamp < 40 && numchamp !=0)

        {
         txt = asTri->Cells[numchamp][i];

         strncpy(CTXT,txt.c_str(),1990); CTXT[1990]=0;
         p=strstr(CTXT,"|");
         while (p)
           {
            *p=' ';
            p=strstr(CTXT,"|");
           }
         txt=AnsiString(CTXT);

         S=txt;
         MaxSizeText = (P_LC[pc][tmpcol+1]- P_LC[pc][tmpcol]);
         if (MaxSizeText > 0)
           {
            ok = false; 
            while (!ok)
             {
              Compute_LH(P_FONT[pc][col],S,P_HAUT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col]);
              // Compute_LH(Police,S,realsize,bold,ital);
               if (L>MaxSizeText)
               {
                  strcpy(tmp,S.c_str());
                  len = strlen(tmp);
                  if (len>0)
                   { tmp[len-1]=0; S=AnsiString(tmp); }
                  else ok=true;

               }
              else ok=true;
             } // end while
           }

         txt=S;
         Compute_LH(P_FONT[pc][col],txt,P_HAUT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col]);

         xxx = P_JH[pc][col];;
         if (xxx=="Centre") x1 = (P_LC[pc][tmpcol]+ P_LC[pc][tmpcol+1])/2 - L/2;
         else if (xxx=="Gauche")x1 = P_LC[pc][tmpcol] + 5;
         else  x1= P_LC[pc][tmpcol+1] - L - 5;



         x2=x1+L;

         lcol = P_LC[pc][col+1] - P_LC[pc][col];
         if (lcol <=0) lcol = 200;



         strcpy(text,txt.c_str());

         /*
         ok=false; maxloop=0;
         while (!ok)
            {
              maxloop++; if (maxloop > 20) break;
              Compute_LH(P_FONT[pc][col],text,StrToInt(P_HAUT[pc][col]),P_ITAL[pc][col],P_BOLD[pc][col]);
               if (L >= lcol-10) text[strlen(text) -1] = 0;
              else ok = true;
            }
         */

         y1=Y0+ (P_CDVV[pc] - P_HAUT[pc][col]-2)/2;  // -2 pour epaisseur des traits
         y2=y1+P_HAUT[pc][col];

         Insert_G('Q',"TX",x1,y1,x2,y2,"DT",P_CBCOLOR[pc][col],P_CFCOLOR[pc][col],
                  P_HAUT[pc][col],0,P_FONT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col],AnsiString(text));

         x1=P_LC[pc][tmpcol]; x2=P_LC[pc][col];
         y1=Y0+P_CDVV[pc];
         y2=y1+5;
         Insert_G('Q',"LH",x1,y1,x2,y2,"DT",TColor(0),TColor(0),1,0,"",'N','N',"");
        }
      } // P_GENRE
     }  // For col


    res = Insert_Block(nivcrit,i,1);
    if (!res)
       nblignes++;
    }  // if P_DETAIL
  }  // for i NB_DETAIL;

nblignes = nblignes;
MEMOY0=Y0;
//  Fin du trac�  : Cleaner le bas, inserer un deuxieme calendrier

}

int __fastcall  TplStandard::CheckNiv(AnsiString ccrit,int crit)
{
 char p0[100],p1[100],p2[100],p3[100];
 char tmp[200];
 char *xp1,*xp2; int result;
 strcpy(tmp,ccrit.c_str());
 strcpy(p0,"");strcpy(p1,"");strcpy(p2,"");strcpy(p3,"");

 xp1=tmp;
 xp2 = strstr(xp1,"-");
 if (xp2)
   { *xp2=0; strcpy(p0,xp1); xp1=xp2+1;
      xp2 = strstr(xp1,"-");
        if (xp2)
          { *xp2=0; strcpy(p1,xp1); xp1=xp2+1;
             xp2 = strstr(xp1,"-");
             if (xp2)
               { *xp2=0; strcpy(p2,xp1); xp1=xp2+1;
                  xp2 = strstr(xp1,"-");
                  if (xp2)
                   { *xp2=0; strcpy(p3,xp1);
                   }
                }
           }
   }

 result = 0;
 strcpy(BRUPT,"NNNNN");

 /*
 if ((strcmp(p0,P0)==0))
   { if ((strcmp(p1,P1) == 0))
      {  if ((strcmp(p2,P2)== 0))
         {  if ((strcmp(p3,P3)== 0))  {}
              else if (P_NIV3[pc][crit]==1) {BRUPT[3] = 'Y'; result = 1; }
         }
       else {  if (P_NIV2[pc][crit]==1) {BRUPT[2] = 'Y'; result = 1; }
      }
    else  if (P_NIV1[pc][crit]==1) {BRUPT[1] = 'Y'; result = 1; }
   }
  else if (P_NIV0[pc][crit]==1)  {BRUPT[0] = 'Y'; result = 1; }
*/


 if ((strcmp(p0,P0)!= 0)  && (P_NIV0[pc][crit]==1)) {BRUPT[0] = 'Y'; result = 1; }
 if ((strcmp(p1,P1)!= 0)  && (P_NIV1[pc][crit]==1)) {BRUPT[1] = 'Y'; result = 1; }
 if ((strcmp(p2,P2)!= 0)  && (P_NIV2[pc][crit]==1)) {BRUPT[2] = 'Y'; result = 1; }
 if ((strcmp(p3,P3)!= 0)  && (P_NIV3[pc][crit]==1)) {BRUPT[3] = 'Y'; result = 1; }
 

 strcpy(P0,p0);  strcpy(P1,p1);  strcpy(P2,p2);  strcpy(P3,p3);
 return result;
}




void __fastcall TplStandard::Trace_Col_Rapport(int tache)
{
 int i,col,numchamp;
 AnsiString txt;
 int x1,y1,x2,y2,tmpcol;
 i=tache;

     for (col=0;col<P_COL[pc];col++)
     {
      tmpcol = col;
      numchamp = P_TRICOL[pc][col];
      if  (numchamp <41 && numchamp>0 && P_ATTR[pc][col]=='C') //(numchamp < 40 && numchamp !=0)
        {
         txt = asTri->Cells[numchamp][i];
         Compute_LH(P_FONT[pc][col],txt,P_HAUT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col]);
         x1 = P_LC[pc][tmpcol]+ (P_LC[pc][col]-P_LC[pc][tmpcol])/2-L/2;
         x2=x1+L;
         y1=Y0+ (P_CDVV[pc] - P_HAUT[pc][col]-2)/2;  // -2 pour epaisseur des traits
         y2=y1+P_HAUT[pc][col];
         Insert_G('Q',"TX",x1,y1,x2,y2,"DT",TColor(0xffffff),TColor(0),
                  P_HAUT[pc][col],0,P_FONT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col],txt);

         x1=P_LC[pc][tmpcol]; x2=P_LC[pc][col];
         y1=Y0+P_CDVV[pc]; y2=y1+1;
         Insert_G('Q',"LH",x1,y1,x2,y2,"DT",TColor(0),TColor(0),1,0,"",'N','N',"");
        }
      }

}


void __fastcall TplStandard::Trace_Enveloppe(int k, int i, AnsiString txt)
{
 int ind,from;
 bool ok;
 int x1,x2,y1,y2,col,tmpcol;
 int coul1,coul2,epaiss;
 float valf; char tmp[150];
 int dist1,dist2,taille,style;
 char dt1[15],dt2[15],durenv;
 char ital,bold;
 AnsiString xtxt;

 xtxt = txt;
 ind=1;
 ok=true;
 while(ok)   // recherche tranche des taches
  { if (NB_TASK[k][ind] <= i)ind++ ;
    else ok=false;
  }
 ind--;
 if (NB_TASK[k][ind]==i)
  {
   x1=ENV_MIN[k][ind];  //Env_Min;
   x2=ENV_MAX[k][ind]; //Env_Max;
   dist1 = add_jour(AnPrev,MoisPrev,JourPrev,ENV_D1[k][ind]);
   sprintf(dt1,"%02d-%02d-%02d",Jour,Mois,An%100);

   dist2 = add_jour(AnPrev,MoisPrev,JourPrev,ENV_D2[k][ind]);
   sprintf(dt2,"%02d-%02d-%02d",Jour,Mois,An%100);
   durenv= comp_dist_dates(dt1,dt2,1,0);

   x1 = P_DEBUTGANTT[pc];  y1=Y0+1;
   y2 = y1+1; x2=D_RIGHT[pc];
   Insert_G('S',"RT",x1,y1,x2,y2,"DT",TColor(0),TColor(0),0,0,"",' ',' ',"");
   Y0=Y0+2;



   if (E_ENCADR[pc][k]=="X")
   xtxt = "D�b: " + AnsiString(dt1) + " " +txt +" Fin:" + AnsiString(dt2) + " Dur:" +AnsiString(NB2);
   else
    if (E_DESIGN[pc][k] == "X") xtxt = txt; else xtxt = "";



  // Place les donn�es dans les colonnes
   if (P_GRILLE[pc]=='O')
   {
   for (col=1;col<=P_COL[pc];col++)
     {
      tmpcol=col;
      txt = T_TITRE_COL[pc][col];
      strcpy(tmp,txt.c_str());
      Compute_LH(P_FONT[pc][col],txt,P_HAUT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col]);
      x1 = P_LC[pc][tmpcol]+10; // �����
      x2=x1+L;
      y1=Y0+ (P_CDVV[pc] - P_HAUT[pc][col]-2)/2;  // -2 pour epaisseur des traits
      y2=y1+P_HAUT[pc][col];

      if (strstr(tmp,"Dur�e")|| strstr(tmp,"Dur"))
         {
          Insert_G('Q',"TX",x1,y1,x2,y2,"DT",TColor(0xffffff),TColor(0),
                  P_HAUT[pc][col],0,P_FONT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col],AnsiString(NB2));
         }
      if (strstr(tmp,"D�but")|| strstr(tmp,"D�b"))
         {
          Insert_G('Q',"TX",x1,y1,x2,y2,"DT",TColor(0xffffff),TColor(0),
                  P_HAUT[pc][col],0,P_FONT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col],AnsiString(dt1));
         }
      if (strstr(tmp,"Fin"))
         {
          Insert_G('Q',"TX",x1,y1,x2,y2,"DT",TColor(0xffffff),TColor(0),
                  P_HAUT[pc][col],0,P_FONT[pc][col],P_ITAL[pc][col],P_BOLD[pc][col],AnsiString(dt2));
         }
     }
     } // if P_GRILLE

    x1=ENV_MIN[k][ind];
   if (x1 !=10000)
    {
      y1=Y0+ 15*P_CDVV[pc]/100; y2=y1+P_CDVV[pc]*35/100;

      //y2=y1+P_CDVV[pc]/2;
      //taille = y2-y1;
      x1=ENV_MIN[k][ind];  //Env_Min;
      strcpy(tmp,E_CTEXT[pc][k].c_str());
      coul1 = atoi(tmp);
      strcpy(tmp,E_CBKG[pc][k].c_str());
      coul2 = atoi(tmp);
      strcpy(tmp,E_HAUTEUR[pc][k].c_str());
      taille = atoi(tmp);
      Insert_G('V',"TX",x1,y1,x2,y2,"DT",TColor(coul2),TColor(coul1),taille,
      0,E_POL[pc][k],E_TITAL[pc][k],E_TBOLD[pc][k],xtxt);

      y1=Y0+ P_CDVV[pc]*2/3;
      y2=y1+5;

      // Couleur Enveloppe

      if (E_COULTRAIT[pc][k]=="") E_COULTRAIT[pc][k-1]=AnsiString(clGreen);
      strcpy(tmp,E_COULTRAIT[pc][k].c_str());
      coul1 = atoi(tmp);

      x1=ENV_MIN[k][ind];  //Env_Min;
      x2=ENV_MAX[k][ind]; //Env_Max;

      if (E_EPAISS[pc][k]=="") E_EPAISS[pc][k]="2.0";
      valf=AnsiToFloat(E_EPAISS[pc][k]);
      epaiss = (int)(valf*COEFMM);
      y2 = y1 + epaiss;
      style = GetStyle(E_STYLE[pc][k]);
      Insert_G('T',"LH",x1,y1,x2,y2,"DT",TColor(coul1),TColor(coul1),epaiss,style,"",'N','N',"XXX");
      Y0=Y0+P_CDVV[pc];

      // Trait Separateur

      //x1=P_DEBUTGANTT[pc];
      /*
      x1=P_DEBUTCOL[pc];
      x2=D_RIGHT[pc];
      y1=Y0; y2= y1+2;// LSB_EPAISSEUR[pc][rupt];
     Insert_G('T',"LH",x1,y1,x2,y2,"",TColor(0),TColor(0),2,2,"",' ',' ',"");
      */
     Y0=Y0+2;

    }
  }


}


// ****************************************
//
//*****************************************

void __fastcall TplStandard::Trace_Pied()
{
  int n,x1,y1,x2,y2,maxh, maxhh;


  Y0 += 10; // provisoire
  // Calcul Hauteur  Maximum pour la ligne

 maxhh=maxh=0;
 if (PHH[pc][25] > maxhh) maxhh=PHH[pc][25];
 if (PHH[pc][28] > maxhh) maxhh=PHH[pc][28];
 if (PHH[pc][31] > maxhh) maxhh=PHH[pc][31];
 if (PH[pc][25] > maxh) maxh=PH[pc][25];
 if (PH[pc][28] > maxh) maxh=PH[pc][28];
 if (PH[pc][31] > maxh) maxh=PH[pc][31];

 if (maxhh<maxh) maxhh=maxh +10;

 X0=D_LEFT[pc];
 Y0=Y0+(maxhh-maxh)/2;

 n=25;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n,0,0); //X0+PR[pc][n];
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],
    PH[pc][n],0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);

 n=28;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n,0,0); //P_W[pc]/2 -L/2;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=31;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n,0,0); //P_W[pc] - P_MARGH2[pc] - L -10;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 Y0=Y0 + maxhh;

 maxhh=maxh=0;
 if (PHH[pc][26] > maxhh) maxhh=PHH[pc][26];
 if (PHH[pc][29] > maxhh) maxhh=PHH[pc][29];
 if (PHH[pc][32] > maxhh) maxhh=PHH[pc][32];
 if (PH[pc][26] > maxh) maxh=PH[pc][26];
 if (PH[pc][29] > maxh) maxh=PH[pc][29];
 if (PH[pc][32] > maxh) maxh=PH[pc][32];

 if (maxhh<maxh) maxhh=maxh +10;
 X0=D_LEFT[pc];
 Y0=Y0+(maxhh-maxh)/2;

 n=26;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],
   PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n,0,0); //X0+PR[pc][n];
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
    0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=29;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n,0,0); //P_W[pc]/2 -L/2;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=32;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=ComputeX1(n,0,0); //P_W[pc] - P_MARGH2[pc] - L -10;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);

 Y0=Y0+maxhh;
 maxhh=maxh=0;
 if (PHH[pc][27] > maxhh) maxhh=PHH[pc][27];
 if (PHH[pc][33] > maxhh) maxhh=PHH[pc][33];
 if (PH[pc][27] > maxh) maxh=PH[pc][27];
 if (PH[pc][33] > maxh) maxh=PH[pc][33];

 if (maxhh<maxh) maxhh=maxh +10;
 X0=D_LEFT[pc];
 Y0=Y0+(maxhh-maxh)/2;

 n=27;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=x1=ComputeX1(n,0,0); //X0+PR[pc][n];
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
   0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);
 n=33;
 Compute_LH(PF[pc][n],PT[pc][n],PH[pc][n],PN[pc][n],PX[pc][n]);
 x1=x1=ComputeX1(n,0,0); //P_W[pc] - P_MARGH2[pc] - L -10;
 x2=x1+L; y1=Y0;y2=y1+H;
 Insert_G('P',"TX",x1,y1,x2,y2,"G1",PB[pc][n],PC[pc][n],PH[pc][n],
  0,PF[pc][n],PN[pc][n],PX[pc][n],PT[pc][n]);

 Y0=Y0 + maxhh + 10;

}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Trace_CalBas()
{
 int x1,x2,y1,y2;

Y0 = MEMOY0;

x1=D_LEFT[pc]; x2=D_RIGHT[pc];
Y0=Y0+30;
y1=Y0; y2= Y0+ H_CAL1[pc]+ H_CAL2[pc];
Insert_G('H',"RT",x1,y1,x2,y2,"DT",clWhite,clWhite,0,0,"",' ',' ',"");
y2=y1+1;
Insert_G('H',"LH",x1,y1,x2,y2,"DT",clBlack,clBlack,1,0,"",' ',' ',"");   // ligne noire
Y0++;
x1=P_DEBUTGANTT[pc];  // x2 reste le meme
y1=Y0; y2=y1+H_CAL1[pc] + H_CAL2[pc]+1;
// copy rectangle, meme X1,X2  et memy1 , memy2 pour les Y sources
Insert_G('H',"CR",x1,y1,x2,y2,"",clBlack,clBlack,0,0,"",' ',' ',"");
Y0=Y0+H_CAL1[pc]+H_CAL2[pc];
}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Trace_Cadre()
{
 int x1,x2,y1,y2,cc;
 TColor c;

 // if (P_CLC[pc]=="") P_CLC[pc]=0; cc =  StrtoInt(P_CLC; c=(TColor)cc;

  c = TColor(P_CLCC[pc]);
  x1= P_MARGH1[pc]; y1=P_MARGV1[pc]; x2=D_RIGHT[pc]; y2=y1+P_ELCF[pc];
 Insert_G('C',"LH",x1,y1,x2,y2,"",c,c,P_ELCF[pc],0,"",' ',' ',"");


  x1= P_MARGH1[pc]; y1=P_MARGV1[pc]; x2=x1+P_ELCF[pc]; y2=P_H[pc]- P_MARGV2[pc];
  Insert_G('C',"LV",x1,y1,x2,y2,"",c,c,P_ELCF[pc],0,"",' ',' ',"");


 x1= P_W[pc]-P_MARGH2[pc]-P_ELCF[pc]; y1=P_MARGV1[pc]; x2=x1+P_ELCF[pc]; y2=P_H[pc]-P_MARGV2[pc];
 Insert_G('C',"LV",x1,y1,x2,y2,"",c,c,P_ELCF[pc],0,"",' ',' ',"");

 x1= P_MARGH1[pc]; y1=P_H[pc]-P_MARGV2[pc]; x2=x1+ P_ELCF[pc];y2=y1+P_ELCF[pc];
 Insert_G('C',"LH",x1,y1,x2,y2,"",c,c,P_ELCF[pc],0,"",' ',' ',"");

 x1= P_MARGH1[pc]; y1=P_H[pc]-P_MARGV2[pc]; x2=D_RIGHT[pc]; y2=y1+P_ELCF[pc];
 Insert_G('C',"LH",x1,y1,x2,y2,"",c,c,P_ELCF[pc],0,"",' ',' ',"");


}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Trace_Marges()
{
 int x1,x2,y1,y2;
 x1=0; x2=P_W[pc];
 y1=0; y2=D_TOP[pc];
 Insert_G('M',"RT",x1,y1,x2,y2,"DT",clWhite,clWhite,0,0,"",' ',' ',"");

 x1=0; x2=D_LEFT[pc];
 y1=0; y2=P_H[pc];
 Insert_G('M',"RT",x1,y1,x2,y2,"DT",clWhite,clWhite,0,0,"",' ',' ',"");

 x1=D_RIGHT[pc]; x2=P_W[pc];
 y1=0; y2=P_H[pc];
 Insert_G('M',"RT",x1,y1,x2,y2,"DT",clWhite,clWhite,0,0,"",' ',' ',"");

 x1=0; x2=P_W[pc];
 y1=D_BOTTOM[pc]; y2=P_H[pc];
 Insert_G('M',"RT",x1,y1,x2,y2,"DT",clWhite,clWhite,0,0,"",' ',' ',"");

}

 int __fastcall TplStandard::Compute_Taille(int type,int location)
 {
  int size; char xtmp[200]; AnsiString txt;  int taille;
  txt ="AZERTYUIOP";
  if (P_INF[pc][type][location]=="Oui")    //Haut
         {
          strcpy(xtmp,P_SIZET[pc][type][location].c_str());
          taille = atoi(xtmp);
          Compute_LH(P_POLT[pc][type][location],txt,taille,P_ITALT[pc][type][location],P_BOLDT[pc][type][location]);
          size = H;
         }
       else size=0;
 return size;

 }



bool __fastcall TplStandard::Insert_Block(int cr,int i,int xwrite)
{
    int x1,y1,x2,y2,x3,x4;
    int x11,y11,x22,y22;
    int xmax;  int bcl;
    char dt1[15],xtmp[100];
    int j1,m1,a1,dist1,dist2,dist3,base;
    bool result;
    char tmp[200];
    int epaiss1,epaiss2;
    int H1,H2,H3,H4,H5,H6;   // Origine du Texte Haut,Droite,Gauche, Bas
    int numtache; int MARG;
    int pos,type;   int BaseY1A,BaseY1B;
     AnsiString S;
    char str[500]; AnsiString txt;
    int col,margtP,margtR;
    int k,count;   int zz,TRAITS;
    TColor couleur1,couleur2,couleur3,couleur4,couleur5,couleur6;
    int style,taille;
    char ital,gras;
    float fx,ff;    int ress;
    int isucc; AnsiString Texte,temp;
    int succtaille,h2;
    int tailleHaut,tailleBas,tailleMiddleG,tailleMiddleD,espace;
    bool TRACE;
    bool TRI; int RI;
    char mess[200];


    if (P_DETAIL[pc]==0) return 0;
    strcpy(tmp,asTri->Cells[1][i].c_str());
    numtache = atoi(tmp);


if (pc != 3) P_TYPE[pc]='P';
if (pc == 3) P_TYPE[pc]='R';

if (P_TYPE[pc]=='R')  TRAITS=2; else TRAITS=1;



 for (zz=1;zz<=TRAITS;zz++)
   {
    TRACE=true;
    if (zz==1)
    {

     dist1 = Convert_Date(asTri->Cells[4][i]);
     dist2 = Convert_Date(asTri->Cells[5][i])+1;
     dist3 = Convert_Date(asTri->Cells[7][i])+1;
     }
    else
     {
      dist1 = Convert_Date(asTri->Cells[21][i]);
      dist2 = Convert_Date(asTri->Cells[22][i])+1;
      dist3 = Convert_Date(asTri->Cells[7][i])+1;
     }

    base = Compute_Base();
    //IXDEB=40;

    x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist1-IXDEB,1)/P_MAXDIST[pc];
    x2 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist2-IXDEB,1)/P_MAXDIST[pc];
    x3 = x2;
    x4 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist3-IXDEB,1)/P_MAXDIST[pc];

    if (base*Compute_Trav(dist1-IXDEB,1) < 0 && base*Compute_Trav(dist2-IXDEB,1) >0) x1=P_DEBUTGANTT[pc]+1;
    if (base*Compute_Trav(dist2-IXDEB,1)<0) { TRACE=false; continue; }
    // P_INF Contient "Oui" ou Non

    // if (zz==1)
      margtP = AnsiToInt(asTri->Cells[8][i]);
      //  margtR = AnsiToInt(asTri->Cells[28][i]);
      ress = Row_AsAvanc(i);
      margtR = AnsiToInt(asTri->Cells[28][i]);
      // margtR = AnsiToInt(asAvanc->Cells[15][ress]);

     if (zz==1 && margtP==0) type = 0;
      if (zz==1 && margtP!=0) type = 1;
      if (zz==2 && margtR==0) type = 2;  // PROVISOIRE 2;
      if (zz==2 && margtR!=0) type = 3;  // PROVISOIRE 3;



     if (pc == 0)  {P_INF[pc][type][3]=="Non";}     // On force pas de message Bas
     if (pc == 3)  {}
     //  0 = HAUT  2 = DROITE  1 = GAUCHE  3 = BAS

    //if (zz==1)
    //   {
       if (TRAITS==1) H1 = P_CDVV[pc]*15/100;
       else if (TRAITS==2 && zz==2)H1 = P_CDVV[pc]*55/100;
       else if (TRAITS==2 && zz==1)H1 = P_CDVV[pc]*5/100;
       txt ="AZERTYUIOP";
       if (P_INF[pc][type][0]=="Oui")    // info Haut
         {
          strcpy(xtmp,P_SIZET[pc][type][0].c_str());
          taille = atoi(xtmp);
          Compute_LH(P_POLT[pc][type][0],txt,taille,P_ITALT[pc][type][0],P_BOLDT[pc][type][0]);
          tailleHaut = H;//H2 = H1 + 3;
         }
       else tailleHaut=0; //H2=H1;
       ff=AnsiToFloat(P_EPTRAIT[pc][type]); epaiss1 =  ff*((float)COEFMM);

       if (P_INF[pc][type][3]=="Oui")   // info bas
         {
          strcpy(xtmp,P_SIZET[pc][type][3].c_str());
          taille = atoi(xtmp);
          Compute_LH(P_POLT[pc][type][3],txt,taille,P_ITALT[pc][type][3],P_BOLDT[pc][type][3]);
          tailleBas = H;
         }   else tailleBas=0;

       if (TRAITS==1)
          {
           if (tailleBas==0) espace = (P_CDVV[pc] - (tailleHaut+tailleBas)) / 3;
           else  espace = (P_CDVV[pc] - (tailleHaut+tailleBas)) / 4;
           }
        else
           {
            if (tailleBas==0) espace = (P_CDVV[pc]/2 - (tailleHaut+tailleBas)) / 3;
           else  espace = (P_CDVV[pc]/2 - (tailleHaut+tailleBas)) / 4;
           }

       H2=H1+tailleHaut+espace/2;
       H4 = H2+epaiss1+espace/2;
       H6 = H2+epaiss1+espace/2;

       if (P_INF[pc][type][1]=="Oui")   // info gauche
         {
          strcpy(xtmp,P_SIZET[pc][type][1].c_str());
          taille = atoi(xtmp);
          Compute_LH(P_POLT[pc][type][3],txt,taille,P_ITALT[pc][type][1],P_BOLDT[pc][type][1]);
          tailleMiddleG = H;
         }   else tailleMiddleG=0;

       if (P_INF[pc][type][2]=="Oui")   // info droite
         {
          strcpy(xtmp,P_SIZET[pc][type][2].c_str());
          taille = atoi(xtmp);
          Compute_LH(P_POLT[pc][type][2],txt,taille,P_ITALT[pc][type][2],P_BOLDT[pc][type][2]);
          tailleMiddleD = H;
         }   else tailleMiddleD=0;
        H3 = H2+epaiss1/2 - tailleMiddleG/2;
        H5 = H2+epaiss1/2 - tailleMiddleD/2;
         //H5 = H2+epaiss1 - tailleMiddleD/2;      // $$$$$$$$$$$$$$$$$$$$$



     xmax=Compute_LargeurMax(i,false);  /// $$$$ i;
     asTri->Cells[70][i]=xmax; asTri->Cells[65][i]=xmax;

   if (P_GENRE[pc]=='G') YY0=Y0;
       else  { k = cr;  //NUM_CRIT[pc]-1;
               LastY0 = Y0;
               if (zz==1)  // ne pas inserer si zz=2;
                 {
                   result= Insert_Compress(k,i,x1,xmax,x2);  //  $$$$$ i//YY0 est calcule
                   if (result==false)
                     { YY0=Y0;
                     }
                 }
              }
        asTri->Cells[66][i]=YY0;


   for (pos=0;pos < 4; pos++)    //  Dessus Gauche Droite Bas
    {

      S = P_INF[pc][type][pos];
      if (P_INF[pc][type][pos]=="Oui")
      {
        // Champs dans P_INFO  [prev]
        strcpy(str,P_INFO[pc][type][pos].c_str());
        count = Explode('|',str); txt="";
        for (k=0;k<count;k++)
           {
             TRI=false;
             if (strcmp(params[k],"Successeurs")==0)
                 txt = txt + Affich_Successeurs(i) + " ";
              else  if (strcmp(params[k],"Pr�d�cesseurs")==0)
                 txt = txt + Affich_Predecess(i) + " ";
             else
              {
               RI=0;
               if (strstr(params[k],"Retard Tot")) txt = txt + " Retard Total:";
               /*
               if (strstr(params[k],"Retard Imp"))
                  {
                    TRI=true;
                    strcpy(tmp,asTri->Cells[col][i].c_str());
                    RI = atoi(tmp);
                    if (RI !=0)
                    txt = txt + " Retard Imputable:";

                  }
                */
               if (strstr(params[k],"Dur�e Pr")) txt = txt + " DP:";
               if (strstr(params[k],"Dur�e R�")) txt = txt + " DR:";
               col=RechercheCol(params[k]);
               if (col>0)
                 {
                  TRI=false;
                  if (strstr(params[k],"Retard Imp"))
                  {
                    TRI=true;
                    strcpy(tmp,asTri->Cells[col][i].c_str());
                    RI = atoi(tmp);
                    if (RI !=0)
                    txt = txt + " Retard Imputable:";

                  }

                  if (!TRI) txt=txt+asTri->Cells[col][i];
                  else
                  if (RI !=0) txt=txt+asTri->Cells[col][i];
                 }
               txt=txt+" ";
              }
            }
       couleur1=(TColor) AnsiToInt(P_COLTXTF[pc][type][pos]);
       couleur2=(TColor) AnsiToInt(P_COLTXTB[pc][type][pos]);

       if (zz==1) MARG= AnsiToInt(asTri->Cells[8][i]);
       else MARG= AnsiToInt(asTri->Cells[28][i]);


       couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
       couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);


       // sprintf(mess,"Tache= %d MargtP= %d MargtR= %d Color3=%d Color4=%d",i,margtP,margtR,couleur3,couleur4);

       // Application->MessageBox(mess,"ECOPL",MB_OK);


       if (!Check_Automatic())  epaiss1 =  ff*((float)COEFMM);
         else epaiss1 = P_CDVV[pc] / 10;
        style = GetStyle(P_STYLEM[pc][type]);


      /*
      if (margtP==0 || margtR==0)
       {
        if (zz==1)
          {couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
           couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
          }
         else
          {
           couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
           couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
          }
        if (!Check_Automatic())  epaiss1 =  ff*((float)COEFMM);
         else epaiss1 = P_CDVV[pc] / 10;
        style = GetStyle(P_STYLEM[pc][type]);
        } //
       else
        {
         if (Check_Automatic() && pos==0 && pc==0) txt = txt + " m=" + AnsiString(MARG);
        if (zz==1)
          {
           couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
           couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
           }
         else
           {
            couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
            couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
           }
        if (!Check_Automatic())
         epaiss1 =  ff*((float)COEFMM);
          else epaiss1 = P_CDVV[pc] / 10;
          style = GetStyle(P_STYLEM[pc][type]);
        }
      */




       if (Check_Automatic())
          {
            taille  = P_CDVV[pc]/ 5 ; // y2-y1;
            if (taille > 40) taille = 40;
            if (pos == 1 || pos == 2) taille = taille * 2 / 3;   // gauche ou droite
            P_SIZET[pc][0][pos] = AnsiString(taille);
            P_SIZET[pc][1][pos] = AnsiString(taille);
            P_EPTRAIT[pc][0] = FloatToStr(epaiss1/10.0);
            P_EPTRAIT[pc][1] = FloatToStr(epaiss1/10.0);
           }
        else
          {
           if (P_SIZET[pc][0][pos]=="") P_SIZET[pc][0][pos]="0";
           if (P_SIZET[pc][1][pos]=="") P_SIZET[pc][1][pos]="0";
           strcpy(xtmp,P_SIZET[pc][type][pos].c_str());
           taille = atoi(xtmp);
          }


       Compute_LH(P_POLT[pc][type][pos],txt,taille,P_ITALT[pc][type][pos],P_BOLDT[pc][type][pos]);
       x11=x1;
       x22=x11+L;  // H Contient la hauteur
       if (x22>x2) xmax=x22; else xmax=x2;
       asTri->Cells[70][i]=xmax; asTri->Cells[65][i]=xmax;
        if (pos == 0)  // dessus
         {

           y1=YY0+H1;
         }
        if (pos == 3)   // bas
           {
             y1 = YY0+ H6;
           }
        h2 = epaiss1/2 - taille*55/100; //taille/2;
        if (pos == 2 ) // droite
          {
            if (P_MARGE[pc][1]=="Oui" && pc != 0)
             {
               x11= x4+10; x22=x11+L;
               y1= YY0+H5;
              }
            else
             {x11= x2+10; x22=x11+L;
              y1= YY0 + H5;
              y2= y1+10;
              }
           }
        if (pos==1) //   gauche
             { x11= x1-L-10; x22=x11+L;
               y1= YY0+H3;
             }


       if (x11 >= P_DEBUTGANTT[pc])
         {
           if (Check_Automatic())
          {
            if (xwrite)
            {Insert_G('T',"TX",x11,y1,x22,y2,"DT",couleur2,couleur1,taille,0,"Arial",
            "Standard","Gras",txt); //
            }
          }
        else
          {
           if (xwrite)
             {
              Insert_G('T',"TX",x11,y1,x22,y2,"DT",couleur2,couleur1,taille,0,P_POLT[pc][type][pos],
              P_ITALT[pc][type][pos],P_BOLDT[pc][type][pos],txt); //asTri->Cells[2][i]);
             }
           }
        }
     } // if P_INF = "Oui"
    }  // For Pos



    if (TRAITS == 1)  y1 = YY0 + H2; //60*P_CDVV[pc]/100;
     else if (TRAITS==2)
       {
        if (zz==1) y1 = YY0 + H2;
        else  y1 = YY0 + H2; // 1;  // H4; //70*P_CDVV[pc]/100;
        }
      y2 = y1+epaiss1;

    if (xwrite) // && zz==1)
      { Insert_G('T',"LH",x1,y1,x2,y2,"DT",couleur4,couleur3,epaiss1,style,"",'N','N',"XXX");
        if (TRAITS==2 && zz==1)
         {
          BaseY1B = YY0;
          Faire_Brisee(BaseY1B,numtache,H2,epaiss1);
         }

      }


    if (xwrite && P_CYNJNT[pc]=="Oui") TraceJNT(numtache,dist1,dist2,x1,y1,x2,y2,1);

    if (pc==0) P_MARGE[0][1] = "Non";
    if (P_MARGE[pc][1]=="Oui" && MARG != 0 && pc != 0)   // trac� de la marge
      {

       couleur3=(TColor) AnsiToInt(P_COLMARGE[pc][1]);
       x3=x2;
       if (xwrite) Insert_G('T',"LH",x3,y1,x4,y2,"DT",couleur4,couleur3,epaiss1,0,"",'N','N',"XXX");
       if (xwrite && P_CYNJNT[pc]=="Oui") TraceJNT(numtache,dist1,dist2,x1,y1,x2,y2,1);
      }


   if (zz==1)
      {
        asTri->Cells[69][i]=y1;    // memorise y1
    asTri->Cells[67][i]=x1;    //memorise debut
    asTri->Cells[68][i]=x2;    //memorise fin

      }
   if (zz==2)
      {
        asTri->Cells[66][i]=y1;    // memorise y1
    asTri->Cells[60][i]=x1;    //memorise debut
    asTri->Cells[65][i]=x2;    //memorise fin

      }


  // �����������������������
  } //For ZZ

   if (P_GENRE[pc]=='G')
     {
      if (TASK_UNIQUE==0) Y0 = Y0+P_CDVV[pc];
      else
      {Y0=Y0+ P_CDVV[pc] + HAUT_RUPT_UNIQUE; TASK_UNIQUE=0; }
     }
   if (P_GENRE[pc]=='C' && result==false) Y0 = Y0+P_CDVV[pc];
   return result;
}

// ****************************************
//
//*****************************************

/*
bool __fastcall TplStandard::Insert_Block(int cr,int i,int xwrite)
{
    int x1,y1,x2,y2,x3,x4;
    int x11,y11,x22,y22;
    int xmax;  int bcl;
    char dt1[15],xtmp[100];
    int j1,m1,a1,dist1P,dist2P,dist3P,dist1R,dist2R,dist3R,base;
    bool result; int dist1,dist2,dist3;
    char tmp[200];

//    int H1[2],H2[2],H3[2],H4[2],H5[2],H6[2];   // Origine du Texte Haut,Droite,Gauche, Bas
    int H1,H2,H3,H4,H5,H6,H7,H8;
    int numtache;
    int pos,type;   int BaseY1A,BaseY1B;
     AnsiString S;
    char str[500]; AnsiString txt;
    int col,margt;
    int k,count;   int zz,TRAITS;
    TColor couleur1,couleur2,couleur3,couleur4,couleur5,couleur6;
    int style,taille;
    char ital,gras;
    float fx,ff;
    int isucc; AnsiString Texte,temp;
    int succtaille,h2;      int location;
    int espace[2];  // espace pour previsionnel crtitique et non critique

    int tailleHaut[4], tailleBas[4], tailleGauche[4], tailleDroite[4];
    int epaiss[4];  bool critique; int crit;

    if (P_DETAIL[pc]==0) return 0;
    strcpy(tmp,asTri->Cells[1][i].c_str());
    numtache = atoi(tmp);


if (pc != 3) P_TYPE[pc]='P';  // prev
if (pc == 3) P_TYPE[pc]='R';  // reel

if (P_TYPE[pc]=='R')  TRAITS=2; else TRAITS=1;

 // preparer les donn�es pour calculer H1...H6  et espace
margt = AnsiToInt(asTri->Cells[8][i]);
if (margt==0) bool critique = true; else critique=false;

for (type=0;type<4;type++)
   {
    location = 0; tailleHaut[type]= Compute_Taille(type,location);
    location = 1; tailleGauche[type] = Compute_Taille(type,location);
    location = 2; tailleDroite[type]= Compute_Taille(type,location);
    location = 3; tailleBas[type] = Compute_Taille(type,location);
    ff=AnsiToFloat(P_EPTRAIT[pc][type]); epaiss[type] =  ff*((float)COEFMM);
   }
 if (TRAITS==1)
   {
    if (critique) crit=0; else crit=1;
    espace[crit] = P_CDVV[pc] - (tailleHaut[crit]+epaiss[crit]+tailleBas[crit]);
    H1=espace[crit];
    H2= H1+ tailleHaut[crit]+espace[crit]/2;
    H4= H2+epaiss[crit]+espace[crit];
    H3 = H2-epaiss[crit]/2+tailleGauche[crit]/2;
    H5 = H2-epaiss[crit]/2+tailleDroite[crit]/2;
   }
if (TRAITS==2)
   {
    if (critique) crit=2; else crit=3;
    espace[crit] = P_CDVV[pc] - (tailleHaut[crit-2]+epaiss[crit-2]+tailleBas[crit-2]);
    espace[crit] = espace[crit] - (tailleHaut[crit]+epaiss[crit]+tailleBas[crit]);
    H1=espace[crit]/2;
    H2= H1+ tailleHaut[crit-2]+espace[crit]/2;
    H4= H2+epaiss[crit-2]+espace[crit]/2;
    H3 = H2-epaiss[crit-2]/2+tailleGauche[crit-2]/2;
    H5 = H2-epaiss[crit-2]/2+tailleDroite[crit-2]/2;
    H6 = H4-epaiss[crit]/2+tailleGauche[crit]/2;
    H7 = H4-epaiss[crit]/2+tailleDroite[crit]/2;
    H8=  H4+epaiss[crit]/2+espace[crit]/2;
   }

 dist1P = Convert_Date(asTri->Cells[4][i]); dist2P = Convert_Date(asTri->Cells[5][i])+1; dist3P = Convert_Date(asTri->Cells[7][i])+1;
 dist1R = Convert_Date(asTri->Cells[21][i]); dist2R = Convert_Date(asTri->Cells[22][i])+1; dist3R = Convert_Date(asTri->Cells[7][i])+1;

  base = Compute_Base();
  x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist1P,1)/P_MAXDIST[pc];
  x2 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist2P,1)/P_MAXDIST[pc];
  x3 = x2;
  x4 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist3P,1)/P_MAXDIST[pc];



 for (zz=1;zz<=TRAITS;zz++)
   {

    base = Compute_Base();
    if (zz==1) {dist1=dist1P; dist2=dist2P; dist3=dist3P; }
    if (zz==21) {dist1=dist1R; dist2=dist2R; dist3=dist3R; }

    if (zz==2)
      {
        x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist1R,1)/P_MAXDIST[pc];
        x2 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist2R,1)/P_MAXDIST[pc];
        x3 = x2;
        x4 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist3R,1)/P_MAXDIST[pc];
      }
     margt = AnsiToInt(asTri->Cells[8][i]);
      if (zz==1 && margt==0) type = 0;     // pas de margeT
      if (zz==1 && margt!=0) type = 1;
      if (zz==2 && margt==0) type = 2;  // pas de marge T
      if (zz==2 && margt!=0) type = 3;  //


     //if (pc == 0)  {P_INF[pc][type][3]=="Non";}     // On force pas de message Bas
     //if (pc == 3)  {}
     //  0 = HAUT  2 = DROITE  1 = GAUCHE  3 = BAS

 //    if (zz==1)  //
 //      {

        xmax=Compute_LargeurMax(i,false);  /// $$$$ i;
        asTri->Cells[70][i]=xmax; asTri->Cells[65][i]=xmax;

       if (P_GENRE[pc]=='G') YY0=Y0;
       else  { k = cr;  //NUM_CRIT[pc]-1;
               LastY0 = Y0;
               if (zz==1)  // ne pas inserer si zz=2;
                 {
                   result= Insert_Compress(k,i,x1,xmax,x2);  //  $$$$$ i//YY0 est calcule
                   if (result==false)  YY0=Y0;
                 }
              }
      asTri->Cells[66][i]=YY0;


    for (pos=0;pos < 4; pos++)    //  Dessus Gauche Droite Bas
    {

      S = P_INF[pc][type][pos];
      if (P_INF[pc][type][pos]=="Oui")
      {
        // Champs dans P_INFO  [prev]
        strcpy(str,P_INFO[pc][type][pos].c_str());
        count = Explode('|',str); txt="";
        for (k=0;k<count;k++)
           {
             if (strcmp(params[k],"Successeurs")==0)
                txt = txt + Affich_Successeurs(i) + " ";
              else  if (strcmp(params[k],"Pr�d�cesseurs")==0) txt = txt + Affich_Predecess(i) + " ";
             else
              {
               col=RechercheCol(params[k]);
               if (col>0) txt=txt+asTri->Cells[col][i];
               txt=txt+" ";
              }
            }
       couleur1=(TColor) AnsiToInt(P_COLTXTF[pc][type][pos]);
       couleur2=(TColor) AnsiToInt(P_COLTXTB[pc][type][pos]);

       margt= AnsiToInt(asTri->Cells[8][i]);
      if (margt==0)
       {
        if (zz==1)
          {couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
           couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
          }
         else
          {
           couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
           couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
          }
        if (!Check_Automatic())  epaiss[type] =  ff*((float)COEFMM);
         else epaiss[type] = P_CDVV[pc] / 10;
        style = GetStyle(P_STYLEM[pc][type]);
        }
       else
        {
         if (Check_Automatic() && pos==0 && pc==0) txt = txt + " m=" + AnsiString(margt);
        if (zz==1)
          {
           couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
           couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
           }
         else
           {
            couleur3=(TColor) AnsiToInt(P_TRAIT[pc][type]);  // depend si haut bas, gauche droite
            couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][type]);
           }
        if (!Check_Automatic())epaiss[type] =  ff*((float)COEFMM);
          else epaiss[type] = P_CDVV[pc] / 10;

          style = GetStyle(P_STYLEM[pc][type]);
   //   }   // ?? zz==1



       if (Check_Automatic())
          {
            taille  = P_CDVV[pc]/ 5 ; // y2-y1;
            if (taille > 40) taille = 40;
            if (pos == 1 || pos == 2) taille = taille * 2 / 3;   // gauche ou droite
            P_SIZET[pc][0][pos] = AnsiString(taille);
            P_SIZET[pc][1][pos] = AnsiString(taille);
            P_EPTRAIT[pc][0] = FloatToStr(epaiss[type]/10.0);
            P_EPTRAIT[pc][1] = FloatToStr(epaiss[type]/10.0);
           }
        else
          {
           if (P_SIZET[pc][type][pos]=="") P_SIZET[pc][type][pos]="0";
           if (P_SIZET[pc][1][pos]=="") P_SIZET[pc][1][pos]="0";
           strcpy(xtmp,P_SIZET[pc][type][pos].c_str());
           taille = atoi(xtmp);
          }


       Compute_LH(P_POLT[pc][type][pos],txt,taille,P_ITALT[pc][type][pos],P_BOLDT[pc][type][pos]);
       x11=x1;
       x22=x11+L;  // H Contient la hauteur
       if (x22>x2) xmax=x22; else xmax=x2;
       asTri->Cells[70][i]=xmax; asTri->Cells[65][i]=xmax;
        if (pos == 0) y1=YY0+H1;
        if (pos == 3) y1 = YY0+ H6;
           }
        h2 = epaiss[type]/2 - taille*55/100; //taille/2;
        if (pos == 2 ) // droite
          {
            if (P_MARGE[pc][1]=="Oui" && pc != 0)
             {
               x11= x4+10; x22=x11+L;
               y1= YY0+H5;
              }
            else
             {x11= x2+10; x22=x11+L;
              y1= YY0 + H5;
              y2= y1+10; }
           }
        if (pos==1) //
             { x11= x1-L-10; x22=x11+L;
               y1= YY0+H3;
             }


       if (x11 >= P_DEBUTGANTT[pc])
         {
           if (Check_Automatic())
          {
            if (xwrite)
            {Insert_G('T',"TX",x11,y1,x22,y2,"DT",couleur2,couleur1,taille,0,"Arial",
            "Standard","Gras",txt); //
            }
          }
        else
          {
           if (xwrite)
             {
              Insert_G('T',"TX",x11,y1,x22,y2,"DT",couleur2,couleur1,taille,0,P_POLT[pc][type][pos],
              P_ITALT[pc][type][pos],P_BOLDT[pc][type][pos],txt); //asTri->Cells[2][i]);
             }
           }
        }
     } // if P_INF = "Oui"
    }  // For Pos



    if (TRAITS == 1)  y1 = YY0 + H2; //60*P_CDVV[pc]/100;
     else if (TRAITS==2)
       {
        if (zz==1) y1 = YY0 + H2;
        else  y1 = YY0 + H4; //70*P_CDVV[pc]/100;
        }
      y2 = y1+epaiss[type];

    if (xwrite) // && zz==1)
      { Insert_G('T',"LH",x1,y1,x2,y2,"DT",couleur4,couleur3,epaiss[type],style,"",'N','N',"XXX");
        if (TRAITS==2 && zz==1)
         {
          BaseY1B = YY0;
          Faire_Brisee(BaseY1B,numtache,H2,epaiss[type]);
         }

      }


    if (xwrite && P_CYNJNT[pc]=="Oui") TraceJNT(numtache,dist1P,dist2P,x1,y1,x2,y2,1);

    if (pc==0) P_MARGE[0][1] = "Non";
    if (P_MARGE[pc][1]=="Oui" && margt != 0 && pc != 0)   // trac� de la marge
      {
       couleur3=(TColor) AnsiToInt(P_COLMARGE[pc][1]);
       x3=x2;
       if (xwrite) Insert_G('T',"LH",x3,y1,x4,y2,"DT",couleur4,couleur3,epaiss[type],0,"",'N','N',"XXX");
       if (xwrite && P_CYNJNT[pc]=="Oui") TraceJNT(numtache,dist1,dist2,x1,y1,x2,y2,1);
      }
    asTri->Cells[69][i]=y1;    // memorise y1
    asTri->Cells[67][i]=x1;    //memorise debut
    asTri->Cells[68][i]=x2;    //memorise fin

 } //For ZZ

   if (P_GENRE[pc]=='G') Y0 = Y0+P_CDVV[pc];
   if (P_GENRE[pc]=='C' && result==false) Y0 = Y0+P_CDVV[pc];
   return result;
}
*/

void __fastcall TplStandard::Faire_Brisee(int ORD,int numtache,int H2,int epaiss)
{
 // recherche
//  CntAVBefore=1;

int m; char tmp[50]; int nt;
int indx;  int SY,base,i,it;
char dtav[20],dt1[20],dt2[20],d1[20],d2[20];
char dtAvanc[20],dtPrev[20];
int dist1,dist2,dist3,dist4,distbrisee; int DistCongesPlusAvanc;
 TColor Col;  float EP;int EPP;
 int j1,m1,a1,j2,m2,a2;
 int x1,x2,y1,y2;  int ff,gg,nxx;
 bool flag_skip;  int DBRIS; int m0;


nxx=av->RowCount;
indx=0;   // recherchge tache dans asAvanc
for (i=1;i<asAvanc->RowCount;i++)
  {
   strcpy(tmp,asAvanc->Cells[1][i].c_str());
   nt = atoi(tmp);
   if (nt==numtache) { indx=i; break;}
  }
 if (indx==0) return;

  SY=P_CDVV[pc];
base = Compute_Base();
m0=-1;
if (FLAG_BRISEE[0][ORD]!=-1)m0= FLAG_BRISEE[0][ORD]; //flag_skip=true; DBRIS=FLAG_BRISEE[ff][ORD]; break; }

for (m=0; m<CntAVBefore; m++)
     {
      strcpy(dtav,TableDatesAV[m]);
      dt1[0]=dtav[6]; dt1[1]=dtav[7];dt1[3]=dtav[3];
      dt1[4]=dtav[4];dt1[6]=dtav[0]; dt1[7]=dtav[1];
      dt1[2]='-';dt1[5]='-';dt1[8]=0;
      strcpy(dtAvanc,dt1);
      sprintf (dtPrev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
      comp_dist_dates(dtPrev,dtAvanc,1,0);
      DistAvanc = NB1;
      DistCongesPlusAvanc = avance_conges(DistAvanc,1);
      Col = TColor(C_BRISEES[pc][m+1]);
      EP = E_BRISEES[pc][m+1]*10.0;
      EPP = EP;
      if (L_BRISEES[pc][m+1])
        {
         //for (it=1;it<=nbt;it++)
         //{
          it = indx;
          dist1 = Convert_Date(asAvanc->Cells[8][it]);
          dist2 = Convert_Date(asAvanc->Cells[10][it]); // date fin  prev;
          dist3 = Convert_Date(asAvanc->Cells[3][it]); // date debut reel
          dist4 = Convert_Date(asAvanc->Cells[5][it]); // date fin reel);

          y1 = ORD;
          if (dist3<= (DistAvanc-1)  &&  dist4>=(DistAvanc-1))
           {
             distbrisee = dist4 - DistCongesPlusAvanc + 1;
             FLAG_BRISEE[0][ORD]=m;  // distbrisee;
           }
         else distbrisee = 0;
         if (dist2==dist4) distbrisee = 0;

         flag_skip=false;
         DBRIS=0;
         /*
         if (m>0)
          {
            if (FLAG_BRISEE[0][ORD]!=-1)
                flag_skip=true;
            // distbrisee = FLAG_BRISEE[m-1][ORD]; // flag_skip=true;  break; }
          }
         */

         //for (ff=0;ff<m;ff++)
         // {
         //   if (FLAG_BRISEE[ff][ORD]!=-1) { flag_skip=true; DBRIS=FLAG_BRISEE[ff][ORD]; break; }

           /*
           for (gg=1;gg<=nxx;gg++)
             {
              if (FLAG_BRISEE[ff][gg]!=-1)
              { flag_skip=true;  break; }

             }
          */

         // }

          // { if  (FLAG_BRISEE[ff][numtache] !=-1)  distbrisee=FLAG_BRISEE[ff][numtache]; break; }
         //if  (FLAG_BRISEE[m][numtache] !=-1)  distbrisee=FLAG_BRISEE[m][numtache];



         // IXDEB=40;
         asAvanc->Cells[22][it] = AnsiString(distbrisee);

         if (m>m0)
          {
         x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];
         y1= ORD;
         Insert_G('Y',"MO",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");

         // x = DistAvanc*spx+1;
         x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];
         y1 = ORD + H2/2; // SY/3;
         Insert_G('Y',"LI",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");


         if (distbrisee !=0)
          x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(dist2-distbrisee+1-IXDEB,1)/P_MAXDIST[pc];
         else
          {

            x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];

          }


          y1=ORD + H2+epaiss/2;
          Insert_G('Y',"LI",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");
          x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];
          y1= ORD + 2*H2; //3*H2/2;
          Insert_G('Y',"LI",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");
          x1 = P_DEBUTGANTT[pc]+1 + base*Compute_Trav(DistAvanc-IXDEB,1)/P_MAXDIST[pc];
          y1= ORD + SY;
          Insert_G('Y',"LI",x1,y1,0,0,"",Col,Col,EPP,0,"",'X','X',"");
         } // > m0
       // Y0 = Y0+P_CDVV[pc];
      //  } // nbt
     } //L_BRISEES
   }  // for m;

}

AnsiString __fastcall TplStandard::Affich_Predecess(int i)
{
int ipred,numpred;
 AnsiString Texte;
 char tmp[50];
 int numtache;
 strcpy(tmp,asTri->Cells[1][i].c_str());
 numtache = atoi(tmp);
 if (numtache==0) return "";

 ipred=1;  Texte="  ";
 while ((pr[numtache][ipred]!=0) && (ipred <PS))
         {
          numpred = pr[numtache][ipred];
          Texte = Texte + IntToStr(pr[numtache][ipred]);
          if (pr_tl[numtache][ipred] == 0) Texte = Texte + " FD ";
          if (pr_tl[numtache][ipred] == 1) Texte = Texte + " FF ";
          if (pr_tl[numtache][ipred] == 2) Texte = Texte + " AM ";
          if (pr_tl[numtache][ipred] == 3) Texte = Texte + " DD ";
          Texte = Texte + IntToStr(pr_decal[numtache][ipred])+ "  ";
          ipred++;
          }
return Texte;

}

AnsiString __fastcall TplStandard::Affich_Successeurs(int i)
{
 int isucc,numsucc;
 AnsiString Texte;
 char tmp[50];
 int numtache;
 strcpy(tmp,asTri->Cells[1][i].c_str());
 numtache = atoi(tmp);
 if (numtache==0) return "";



 isucc=1;  Texte="  ";
 while ((su[numtache][isucc]!=0) && (isucc <PS))
         {
          numsucc = su[numtache][isucc];
          if (numsucc != 9999) //  && flag_souhaites[numsucc] != 'Y')
            {
              Texte = Texte + IntToStr(su[numtache][isucc]);
              if (su_tl[numtache][isucc] == 0) Texte = Texte + " FD ";
              if (su_tl[numtache][isucc] == 1) Texte = Texte + " FF ";
              if (su_tl[numtache][isucc] == 2) Texte = Texte + " AM ";
              if (su_tl[numtache][isucc] == 3) Texte = Texte + " DD ";
              Texte = Texte + IntToStr(su_decal[numtache][isucc])+ "  ";
            }
            isucc++;
          }
return Texte;
}


//////////////////////////////////////////////

void __fastcall TplStandard::Insert_Col(int i)
{
    int x1,y1,x2,y2;
    int x11,y11,x22,y22;
    int xmax;
    char dt1[15];
    int j1,m1,a1,dist1,dist2,base;
    bool result;
    char tmp[200];

    char str[500]; AnsiString txt;
    int col,margt;
    int k,count;
    TColor couleur1,couleur2,couleur3,couleur4;
    int style,taille;
    char ital,gras;
    float fx,ff;


    if (P_DETAIL[pc]==0) return;

    /*
    strcpy(dt1,asTri->Cells[4][i].c_str());  // date debut reelle
    dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    dist1 = convert_date(a1+2000,m1,j1);
    strcpy(dt1,asTri->Cells[5][i].c_str());  // date fin reelle
    dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
    dist2 = convert_date(a1+2000,m1,j1) + 1;
    //if (pc<7) // ZBASE
    if (Check_Automatic())
       base = D_RIGHT[pc] - P_DEBUTGANTT[pc] - P_LASTTEXT[pc] - 150;  // 100  marge pour texte
    else
      {
       if (P_CJT[pc]=="Oui") P_CJT[pc]="30";
       if (P_CJT[pc]=="") P_CJT[pc]="30";
       strcpy(tmp,P_CJT[pc].c_str());
       base = atoi(tmp);
       base = base * P_MAXDIST[pc];

       }


    x1 = P_DEBUTGANTT[pc]+3 + base*dist1/P_MAXDIST[pc];
    x2 = P_DEBUTGANTT[pc]+3 + base*dist2/P_MAXDIST[pc];
    //x2 = x1+200;   // taille du texte

    //if (x1<Env_Min) Env_Min=x1;
    //if (x2>Env_Max) Env_Max=x2;

    // Champs dans P_INFO
    strcpy(str,P_INFO[pc][0][0].c_str());
    count = Explode('|',str);
    txt="";
    for (k=0;k<count;k++)
     { col=RechercheCol(params[k]);
       txt=txt+asTri->Cells[col][i];
       txt=txt+" ";
     }

    couleur1=(TColor) AnsiToInt(P_COLTXTF[pc][0][0]);  // depend si haut bas, gauche droite
    couleur2=(TColor) AnsiToInt(P_COLTXTB[pc][0][0]);
    taille = AnsiToInt(P_SIZET[pc][0][0]);
    ital='N'; if (P_ITALT[pc][0][0]=="Italique") ital='I';
    gras='N'; if (P_BOLDT[pc][0][0]=="Gras") gras='B';
    Compute_LH(P_POLT[pc][0][0],txt,taille,ital,gras);
    x11=x1;
    x22=x11+L;
    if (x22>x2) xmax=x22; else xmax=x2;
    asTri->Cells[70][i]=xmax;
    asTri->Cells[65][i]=xmax;
    result=false;
    if (P_GENRE[pc]=='G') YY0=Y0;
     else
     {
      //for (k=1;k<9;k++)
      k = NUM_CRIT[pc];

      result= Insert_Compress(k,i,x1,xmax,x2);  //YY0 est calcule
      if (result==false) YY0=Y0;
     }
    asTri->Cells[66][i]=YY0;

    y1=YY0+4; y2=y1+P_CDVV[pc]/2;

    //Insert_G('T',"TX",x11,y1,x22,y2,"DT",couleur2,couleur1,taille,0,P_POLT[pc][0][0],ital,gras,txt); //asTri->Cells[2][i]);

    // Graphique a tracer  GANTT
    y1=y1+2*P_CDVV[pc]/3+2;  y2=y2+P_CDVV[pc]/2;
    x2 = P_DEBUTGANTT[pc]+3 + base*dist2/P_MAXDIST[pc];
    margt= AnsiToInt(asTri->Cells[8][i]);
    if (margt==0)
     {  couleur3=(TColor) AnsiToInt(P_TRAIT[pc][0]);  // depend si haut bas, gauche droite
        couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][0]);
        taille =  ff*((float)COEFMM);
     }
     else
     {
        couleur3=(TColor) AnsiToInt(P_TRAIT[pc][1]);  // depend si haut bas, gauche droite
        couleur4=clWhite; ff=AnsiToFloat(P_EPTRAIT[pc][1]);
        taille =  ff*((float)COEFMM);
     }

   //Insert_G('T',"LH",x1,y1,x2,y2,"DT",couleur4,couleur3,taille,0,"",'N','N',"XXX");

    asTri->Cells[69][i]=y1;    // memorise y1
    asTri->Cells[67][i]=x1;     //memorise debut
    asTri->Cells[68][i]=x2;   //memorise fin
    */
   if (P_GENRE[pc]=='G') Y0 = Y0+P_CDVV[pc]+4;
   if (P_GENRE[pc]=='C' && result==false) Y0 = Y0+P_CDVV[pc]+4;


}



// ****************************************
//
//*****************************************

bool __fastcall TplStandard::Insert_Compress(int k,int i,int deb,int xmax,int x2)
{
 int from; int exxtr,extr;
 int farx;
 int ind; bool ok;
 int x,xx; int y0,yy0;
 char tmp[150],xxx[50];
 int m,l;

 // on boucle pour chercher  la tranche de taches
 ind=1;
 LastY0 = Y0;
 ok=true;
 while(ok)
  { if (NB_TASK[k][ind] <= i)ind++ ;
    else ok=false;
  }
 ind--;
 from = NB_TASK[k][ind];
 if (from==i) { asTri->Cells[65][i]=xmax; return false; }
 ok=false;
 int dist;

  strcpy(xxx,P_CDH[pc].c_str());
  dist = atoi(xxx);  //StrToInt(P_CDH[pc]);

  LastY0=0;
  for (x=from;x<i;x++)
   {
    strcpy(tmp,asTri->Cells[65][x].c_str());
    extr = atoi(tmp);
    strcpy(tmp,asTri->Cells[66][x].c_str());
    y0= atoi(tmp);

    if (y0 < Y0)
      {
       y0= y0;
      }
    // rechercher le max pour cette valeur y0
    farx = 0;
    for (xx=from;xx < i; xx++)
      {
       strcpy(tmp,asTri->Cells[66][xx].c_str());
       yy0= atoi(tmp);
       if (yy0>LastY0) LastY0=yy0;
       if (yy0==y0)
         {
           strcpy(tmp,asTri->Cells[65][xx].c_str());
           exxtr = atoi(tmp);
           if (exxtr>farx) farx = exxtr;
         }
      }

    if (y0>LastY0) LastY0 = y0;
    if (deb>(farx+dist))
     {asTri->Cells[65][x]= xmax; // asTri->Cells[70][i];   // XFIN
      ok=true;
      YY0=y0;
      break;
     }

   }
 return ok;
}





// ****************************************
//
//*****************************************

void __fastcall TplStandard::imP1ScreenMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   int rx,ry;int XX,YY;  char tmp[200];
   char dlg;


   pc=1;

  rx= (LAST_RX2[pc]-LAST_RX1[pc])*X/imP1Screen->Width;
  rx = rx + LAST_RX1[pc];
  ry= (LAST_RY2[pc]-LAST_RY1[pc])*Y/imP1Screen->Height;
  ry = ry + LAST_RY1[pc];
  if (Shift.Contains(ssLeft))
    {

  //  rx,ry=coordonnees dans la bitmap IMP
  sprintf (tmp,"rx=%d ry=%d",rx,ry);
  //lxy->Caption = AnsiString(tmp); //Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
  Call_Dialog(rx,ry,0);
  RAZ(); Prepare_Draw_Planning();
  XX= rx*imP1p->Width/imP->Width;
  YY= ry*imP1p->Height/imP->Height;
  ZoomCopy1(imP1p,0,0);  //XX,YY);
  }

 else   Display_Aide(rx,ry);



}


void __fastcall TplStandard::Display_Aide(int rx, int ry)
{
 char dlg;
  Thelp *Aide;

   dlg = Call_Dialog(rx,ry,1);
   switch (dlg)
     {
      case 'V' :
      case 'S' :
       strcpy(m_help,"TrierGrouper.html#TG4");
       break;
      case 'C' :
       strcpy(m_help,"Plannings.html#Pl42");
       break;
      case 'E' :
       strcpy(m_help,"Plannings.html#Pl43");
       break;
      case 'H' :
       strcpy(m_help,"Plannings.html#Pl44");
       break;
      case 'T' :
       strcpy(m_help,"Plannings.html#Pl45");
       break;
      case 'K' :
       strcpy(m_help,"Plannings.html#Pl46");
       break;
      case 'L' :
       strcpy(m_help,"TrierGrouper.html#TG4");

       break;

       case 'P' :
       strcpy(m_help,"Plannings.html#Pl49");

       break;
      case 'Q' :
       strcpy(m_help,"TrierGrouper.html#TG4");
       break;

      default :
       strcpy(m_help,"Plannings.html#Pl46");
       break;

     }
    Aide = new Thelp(Application);
    Aide->ShowModal();
    delete Aide;
}

// ****************************************
//
//*****************************************

/*
void __fastcall TplStandard::G_PrepareHV()
{
 int cnt,i,mem;
 char dt1[15];
 int j1,m1,a1,dist;

 mem=0;
 cnt = asTri->RowCount;  P_MAXDIST[pc]=0;
 for (i=1;i<cnt;i++)
  {
   strcpy(dt1,asTri->Cells[22][i].c_str());  // date fin reelle
   dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
   dist = convert_date(a1+2000,m1,j1);
   if (dist>P_MAXDIST[pc]) { P_MAXDIST[pc]=dist; mem=i; }
  }
  AnsiString font; int ht;
  font = PF[pc][21]; ht=PH[pc][21];

 Compute_LH(font,asTri->Cells[2][mem],ht,'N','N');
 P_LASTTEXT[pc] = L;
 // %%%%%
}
*/

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Compute_LH(AnsiString FName, AnsiString txt,int h,AnsiString ital,AnsiString bold)
{
 TFont *F;
 F = new TFont;
 F->Name =FName;
 F->Size= h;
 F->Style = TFontStyles();
 if(ital=="Italique") F->Style = F->Style << fsItalic;
 if (bold=="Gras")F->Style = F->Style << fsBold;

 iTest->Canvas->Font = F;

 iTest->Canvas->Font->Name = FName;
 iTest->Canvas->Font->Size = h;
 H = iTest->Canvas->TextHeight(txt);
 L = iTest->Canvas->TextWidth(txt);
}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Commencer(int rupt,AnsiString txt,int notask,bool traits)
{
     Faire_LSH(rupt);  // Critere en ligne
     Draw_Rupture(rupt,txt,notask);
     // if (traits)
     Faire_LSB(rupt);

}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Finir(int rupt,AnsiString txt)
{
 int x1,x2,y1,y2;
 TColor col1,col2;

 /// P_ENV[pc][rupt]=1;
/*
if (P_ENV[pc][rupt]==1)
  {
   x1=ENV_MIN[rupt];  //Env_Min;
   x2=ENV_MAX[rupt]; //Env_Max;
   if (x1 !=10000)
    {
      y1=Y0;
      y2=y1+P_CDVV[pc];
      Insert_G('Q',"TX",x1,y1,x2,y2,"DT",clRed,clWhite,P_HAUT[pc][rupt],
      0,P_FONT[pc][rupt],P_ITAL[pc][rupt],P_BOLD[pc][rupt],txt);



      y1=y1+ P_CDVV[pc]/2;
      y2=y2+5;
      Insert_G('Y',"LH",x1,y1,x2,y2,"DT",clGreen,clGreen,5,0,"",'N','N',"XXX");

      Y0=Y0+P_CDVV[pc];
    }
  }
*/
}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Faire_LSH(int rupt)
{

  int x1,y1,x2,y2; int style;  char tmp[50];
  int epaiss; AnsiString EPSS; char epais[50];
  float fep;  int color;

  if (T_LS1[pc][rupt] == "" ||  T_LS1[pc][rupt] == "Non") return;

  if (LSH[pc][rupt])   //
   {
    if (rupt==0) X0=D_LEFT[pc];
         else
          {
           if (P_ATTR[pc][rupt+1]=='C') X0 = P_LC[pc][rupt] +2 ; else X0 = P_LC[pc][rupt] +2;
          }

    style = GetStyle(T_LS2ST[pc][rupt]);
    EPSS  = T_LS1EP[pc][rupt];
    strcpy(epais,EPSS.c_str());
    fep = atoxx(epais);
    epaiss = int (fep*10);
    if (epaiss==0) epaiss=1;

    // color
    strcpy(tmp,T_LS1CL[pc][rupt].c_str());
    color = atoi(tmp);


    x1=X0; x2=D_RIGHT[pc]; //P_W[pc] - P_MARGH2[pc];
    y1=Y0;
    // if (TASK_UNIQUE >0) { y1=y1+50; TASK_UNIQUE=0; }  // y1=y1+P_CDVV[pc]/8;
    y2= y1+ epaiss;
    Insert_G('A',"LH",x1,y1,x2,y2,"",TColor(color),TColor(color),epaiss,style,"",' ',' ',"");
    Y0=Y0+epaiss;
   }



  /*
  int x1,y1,x2,y2;  bool flagdone;


  flagdone=false;
  if (P_ATTR[pc][rupt] == 'L')
   {
    x1 = P_LC[pc][rupt];

    x2=D_RIGHT[pc];
    y1=Y0; y2= y1+ 1; // LSH_EPAISSEUR[pc][rupt];   // 5
    Insert_G('A',"LH",x1,y1,x2,y2,"",TColor(0),TColor(0),LSH_EPAISSEUR[pc][rupt],0,"",' ',' ',"");
    //Insert_G('A',"LH",x1,y1,x2,y2,"",TColor(0),TColor(0),1,0,"",' ',' ',"");
    // Y0 = Y0+1;
    flagdone=true;

   }



  if (LSH[pc][rupt] && !flagdone)   // On traite ligne  LSH  Ligne Horizontale Haute
   {

   x1 = P_LC[pc][rupt];

    x2=D_RIGHT[pc]; // P_W[pc] - P_MARGH2[pc];
    y1=Y0; y2= y1+ 1; // LSH_EPAISSEUR[pc][rupt];   // 5
    Insert_G('A',"LH",x1,y1,x2,y2,"",TColor(0),TColor(0),LSH_EPAISSEUR[pc][rupt],0,"",' ',' ',"");
    // Insert_G('A',"LH",x1,y1,x2,y2,"",TColor(0),TColor(0),1,0,"",' ',' ',"");
    // Y0 = Y0 + 1;  //LSH_EPAISSEUR[pc][rupt];
   }
 */
}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Faire_LSB(int rupt)
{

  int x1,y1,x2,y2; int style;  char tmp[50];
  int epaiss; AnsiString EPSS; char epais[50];
  float fep;  int color;

  if (T_LS2[pc][rupt] == "" ||  T_LS2[pc][rupt] == "Non") return;

  if (LSB[pc][rupt])   //
   {
    if (rupt==0) X0=D_LEFT[pc];
         else
          {
           if (P_ATTR[pc][rupt+1]=='C') X0 = P_LC[pc][rupt] +2 ; else X0 = P_LC[pc][rupt] +2;
          }

    style = GetStyle(T_LS2ST[pc][rupt]);
    EPSS  = T_LS2EP[pc][rupt];
    strcpy(epais,EPSS.c_str());
    fep = atoxx(epais);
    epaiss = int (fep*10);
    if (epaiss==0) epaiss=1;

    // color
    strcpy(tmp,T_LS2CL[pc][rupt].c_str());
    color = atoi(tmp);                           

    // if (TASK_UNIQUE > 0) Y0=Y0+TASK_UNIQUE;
    x1=X0; x2=D_RIGHT[pc]; //P_W[pc] - P_MARGH2[pc];
    y1=Y0;
    // if (TASK_UNIQUE >0) y1=y1+50; //  y1=y1+P_CDVV[pc]/8;
    y2= y1+ epaiss;
    Insert_G('A',"LH",x1,y1,x2,y2,"",TColor(color),TColor(color),epaiss,style,"",' ',' ',"");
    Y0=Y0+epaiss;
   }

}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Draw_Rupture(int rupt, AnsiString txt,int notask)
{

 int i,x1,y1,x2,y2,a;
 TFont *F;
 TColor col1,col2;
 char xxx[50]; char xtmp[100];
 int taille,lcol;
 F = new TFont; boolean ok;
 char text[250];
 AnsiString ccc; int nbtaskrupt;
 int xx;
 // Draw Envelopp

 TASK_UNIQUE = 0;
 HAUT_RUPT_UNIQUE = 0;
  strcpy(xxx,T_CBKG[pc][rupt].c_str()); col1=TColor(atoi(xxx));// col1=StrToInt(PL_CBKG[pc]);
  strcpy(xxx,T_CTEXT[pc][rupt].c_str()); col2=TColor(atoi(xxx));// col2=StrToInt(PL_CTEXT[pc]);

 F->Name = T_POL[pc][rupt];
 if (T_HAUTEUR[pc][rupt]=="") T_HAUTEUR[pc][rupt]="0";

 strcpy(xtmp,T_HAUTEUR[pc][rupt].c_str());
 a =atoi(xtmp);
 F->Size = a;  // StrToInt(T_HAUTEUR[pc][rupt]);
 HAUT_RUPT_UNIQUE = a;
 
 F->Style = TFontStyles();
 if(T_TITAL[pc][rupt]=="Italique")  F->Style = F->Style << fsItalic;

 if (T_TBOLD[pc][rupt]=="Gras")  F->Style = F->Style << fsBold;


 lcol = P_LC[pc][rupt+1] - P_LC[pc][rupt];
 if (lcol <=0) lcol = 200;


 strcpy(text,txt.c_str());
 ok=false;
 if (P_ATTR[pc][rupt] == 'C')
  {
   while (!ok)
    {
     strcpy(xtmp,T_HAUTEUR[pc][rupt].c_str());
     a =atoi(xtmp);
     Compute_LH(F->Name,text,a,T_TITAL[pc][rupt],T_TBOLD[pc][rupt]);
     if (L > lcol) text[strlen(text) -1] = 0;
     else ok = true;
    }
  }

if (P_ATTR[pc][rupt]=='L')
    {
      ccc = T_JSH[pc][rupt];
      if (rupt==0) x1=D_LEFT[pc];
         else
          {
           if (P_ATTR[pc][rupt+1]=='C') x1 = P_LC[pc][rupt] +2 ; else x1 = P_LC[pc][rupt]+2; //-1] +2;
          }
      y1=Y0;

      if (rupt==0) { x2= D_RIGHT[pc];  }
        else x2= P_DEBUTGANTT[pc]; // D_RIGHT[pc]; //P_DEBUTGANTT[pc]; // -3;
      // trace Ligne au dessus du texte
      x2= D_RIGHT[pc];
      y2=y1+2;
      Y0 = Y0+2;
      // $$$$ XXXXXXXXXXXX YYYYYYYYYYYYY Insert_G('Y',"LH",x1,y1,x2,y2,"",TColor(0),TColor(0),1,0,"",' ',' ',"");
      y1=Y0;
       strcpy(xtmp,T_HAUTEUR[pc][rupt].c_str());
       a =atoi(xtmp);
      Compute_LH(F->Name,text,a,T_TITAL[pc][rupt],T_TBOLD[pc][rupt]);
      y2 = y1+H;

     Insert_G('S',"RT",x1,y1,x2,y2,"DT",col1,col1,0,0,"",' ',' ',AnsiString(text));    // 'L'

    }

 if (P_ATTR[pc][rupt]=='L')
 {
  // x1= D_LEFT[pc];

  ccc = T_JSH[pc][rupt];
  if (rupt==0)
       {

        if (ccc=="Gauche") x1=D_LEFT[pc];
        else if (ccc=="Centre")  x1= (D_LEFT[pc]+D_RIGHT[pc])/2 - L/2;
        else x1=D_RIGHT[pc] - L-10;

        }
   else
          {
           if (P_ATTR[pc][rupt+1]=='C') x1 = P_LC[pc][rupt] +2 ; else x1 = P_LC[pc][rupt]+2; // -1] +2;
           if (ccc=="Centre")  x1= (D_LEFT[pc]+D_RIGHT[pc])/2 - L/2;

          }

  x2= x1 + L;
  y1 = Y0+2; y2=y1+H;
  strcpy(xxx,T_HAUTEUR[pc][rupt].c_str()); taille=atoi(xxx);

  Insert_G('S',"TX",x1,y1,x2,y2,"DT",col1,col2,taille,
           0,T_POL[pc][rupt],
           T_TITAL[pc][rupt],T_TBOLD[pc][rupt],AnsiString(text));       // 'L'
  //if (rupt==0)
    Y0=Y0+H;
    }
 else  if (P_ATTR[pc][rupt]=='C')
  {

   x1 = P_LC[pc][rupt] +2 ;
   x2=  P_LC[pc][rupt+1] - x1; // + lcol;
   // Haut , Centre, Bas
   ccc = T_JSV[pc][rupt];
   y1 = Y0+2;
   //  if (ccc=="Haut") { y1 = Y0+2;}

   if (P_GENRE[pc]=='G' && (ccc=="Centre" || ccc=="Bas"))
     {

       if (ccc=="Centre")
         {
           for (xx=0;xx<500;xx++)
            {
             if (NB_TASK[rupt][xx]==notask)
              {
               nbtaskrupt = (NB_TASK[rupt][xx+1]-notask);
              if (nbtaskrupt==1)
                   y1 = Y0 + P_CDVV[pc]/3; //nbtaskrupt=1;
              else y1 = Y0 + P_CDVV[pc]*(nbtaskrupt-1)/2 + 2;
              break;
              }
            }
         }
        if (ccc=="Bas")
         {
          for (xx=0;xx<500;xx++)
            {
             if (NB_TASK[rupt][xx]==notask)
              {
               nbtaskrupt = (NB_TASK[rupt][xx+1] - NB_TASK[rupt][xx]);
              if (nbtaskrupt==1)
                 y1 = Y0 + P_CDVV[pc]/3; //nbtaskrupt=1;
              else y1 = Y0 + P_CDVV[pc]*(nbtaskrupt-1)+ P_CDVV[pc]/2  + 2;
             break;
              }
            }

         }

       strcpy(xxx,T_HAUTEUR[pc][rupt].c_str()); taille=atoi(xxx);
       if (nbtaskrupt==1)
           TASK_UNIQUE = taille;
          else TASK_UNIQUE = 0;

      }


    y2=y1+H;
   strcpy(xxx,T_CBKG[pc][rupt].c_str()); col1=TColor(atoi(xxx));// col1=StrToInt(PL_CBKG[pc]);
   strcpy(xxx,T_CTEXT[pc][rupt].c_str()); col2=TColor(atoi(xxx));// col2=StrToInt(PL_CTEXT[pc]);
   strcpy(xxx,T_HAUTEUR[pc][rupt].c_str()); taille=atoi(xxx);

   Insert_G('S',"TX",x1,y1,x2,y2,"DT",col1,col2,taille,
           0,T_POL[pc][rupt],
           T_TITAL[pc][rupt],T_TBOLD[pc][rupt],AnsiString(text));             // 'K'
           
  }
 else if (P_ATTR[pc][rupt]=='T')
  {

   x1 = P_LC[pc][rupt]+2;  y1=Y0;

   strcpy(xxx,T_HAUTEUR[pc][rupt].c_str()); H=atoi(xxx);
   strcpy(xxx,T_CTEXT[pc][rupt].c_str()); col1=TColor(atoi(xxx));

   y2 = y1+H; x2=D_RIGHT[pc];
   Insert_G('S',"RT",x1,y1,x2,y2,"DT",col1,col1,0,0,"",' ',' ',"");
   Y0=Y0+H;
  }
 else if (P_ATTR[pc][rupt]=='B')
  {


   x1 = P_LC[pc][rupt]+2;  y1=Y0;
   strcpy(xxx,T_HAUTEUR[pc][rupt].c_str()); H=atoi(xxx);
   strcpy(xxx,T_CTEXT[pc][rupt].c_str()); col1=TColor(atoi(xxx));
   y2 = y1+H; x2=D_RIGHT[pc];

   Insert_G('S',"RT",x1,y1,x2,y2,"DT",col1,col1,0,0,"",' ',' ',"");
   Y0=Y0+H;
  }
  //if (P_ATTR[pc][rupt]=='L' )
}


// ****************************************
//
//*****************************************

void __fastcall TplStandard::ZoomP1Change(TObject *Sender)
{
 ZoomCopy1(imP1p,0,0);

}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Fleche(Graphics::TBitmap *B,char sens,int x,int y)
{

 if (sens=='G')
   {
    B->Canvas->MoveTo(x,y);
    B->Canvas->LineTo(x+12,y-6);
    B->Canvas->LineTo(x+12,y+6);
    B->Canvas->LineTo(x,y);
   }
 else
   {
    B->Canvas->MoveTo(x,y);
    B->Canvas->LineTo(x-12,y-6);
    B->Canvas->LineTo(x-12,y+6);
    B->Canvas->LineTo(x,y);
   }
}

// ****************************************
//
//*****************************************

void __fastcall TplStandard::Draw_Planning(Graphics::TBitmap *B)
{
 TRect R; int cnt,i; char Cod[20];
 TColor c; char tmp[25000]; int a;
 int x1,y1,x2,y2;
 TFont *F;
 TRect Dst,Src;
 int pstyle;
 int x3,y3,x4,y4;
 int A1,B1,A2,B2;
 int v1,v2;
 float rayon;
 int icolor,height,width;
 char xtmp[100];

 Graphics::TBitmap *Bitmap;
 TRect MyRect,MyOther;
 AnsiString fn;

 F = new TFont;
 // Clear Graphics area


 cnt=asT->RowCount;
 if (cnt==2) return;

 HPEN hPen;
 LOGBRUSH LBrush;

 //LOGBRUSH lBrush;
 //lBrush.lbStyle = bsSolid; //BS_HATCHED;

 //lBrush.lbHatch = HS_DIAGCROSS;
 DWORD pStyle;


 for (i=1;i<cnt;i++)
   {

    strcpy(tmp,asT->Cells[13][i].c_str());
    pstyle = atoi(tmp);
    if (pstyle==0) pstyle=psSolid;
    else if (pstyle==1) pstyle=psSolid;
    else if (pstyle==2) pstyle=psDash;
    else if (pstyle==3) pstyle=psDot;
    else if (pstyle==4) pstyle=psDashDot;
    else if (pstyle==5) pstyle=psDashDotDot;
    else if (pstyle==6) pstyle=psClear;
    else pstyle=psSolid;

    strcpy(Cod,asT->Cells[0][i].c_str());
    if (strcmp(Cod,"LH")==0)
      {
       pStyle  = PS_GEOMETRIC | pstyle | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;

       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       LBrush.lbStyle = BS_SOLID;
       LBrush.lbColor = c;
       LBrush.lbHatch = 0;  //HS_DIAGCROSS;
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);

       hPen = ExtCreatePen(pStyle, a, &LBrush, 0, NULL);
       B->Canvas->Pen->Handle = hPen;
       B->Canvas->Pen->Width = a;

       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Pen->Color = c;
       B->Canvas->Pen->Style = pstyle;

       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[3][i].c_str()); x2= atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str()); y2=atoi(tmp);

       if (y2>(y1+1))
         {
           R = Rect(x1,y1,x2,y2);
           B->Canvas->Brush->Color = c;
           B->Canvas->FillRect(R);
         }
        else
          {
           B->Canvas->MoveTo(x1,y1);
            B->Canvas->LineTo(x2,y1);
          }
      }

    if (strcmp(Cod,"MO")==0)
      {
       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       B->Canvas->MoveTo(x1,y1);
      }
    if (strcmp(Cod,"LI")==0)
      {
       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Pen->Color = c;
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);

       B->Canvas->Pen->Width = a;
       B->Canvas->LineTo(x1,y1);

      }
    if (strcmp(Cod,"LV")==0)
      {
       //pStyle  = PS_GEOMETRIC | pstyle | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;

       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       LBrush.lbStyle = BS_SOLID;
       LBrush.lbColor = c;
       LBrush.lbHatch = HS_DIAGCROSS;
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);
       hPen = ExtCreatePen(pstyle, a, &LBrush, 0, NULL);

       B->Canvas->Pen->Handle = hPen;
       B->Canvas->Pen->Width = a;

       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Pen->Color = c;

       strcpy(tmp,asT->Cells[10][i].c_str()); a= atoi(tmp);
       /*
       if (strcmp(tmp,"D")==0)
           {B->Canvas->Brush->Color = clWhite;
            B->Canvas->Pen->Style = psDot;
           }
       //else
       */
         B->Canvas->Pen->Style = TPenStyle(pstyle);

       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[3][i].c_str()); x2= atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str()); y2=atoi(tmp);
       B->Canvas->MoveTo(x1,y1);
       B->Canvas->LineTo(x1,y2);
      }

    else if (strcmp(Cod,"TX")==0)
      {
       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Brush->Color = c;
       B->Canvas->Brush->Style = bsClear;
       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a); F->Color=c;
       F->Name = asT->Cells[9][i];
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);
       F->Size = a;
       F->Style = TFontStyles();


       if (asT->Cells[10][i] == "Italique")  F->Style = F->Style << fsItalic;
       if (asT->Cells[11][i] == "Gras")  F->Style = F->Style << fsBold;

       B->Canvas->Font = F;
       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[12][i].c_str());
       if (strstr(tmp,"$date"))
           { B->Canvas->TextOut(x1,y1,DateToStr(Date()));
           }
       else  B->Canvas->TextOut(x1,y1,asT->Cells[12][i]);
      }
   else if (strcmp(Cod,"RT")==0)   // rectangle couleur
      {

       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[3][i].c_str()); x2= atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str()); y2=atoi(tmp);
       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       R = Rect(x1,y1,x2,y2);
       B->Canvas->Brush->Color = c;
       B->Canvas->FillRect(R);
      }
   else if (strcmp(Cod,"CR")==0)   // copie rect
      {
       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[3][i].c_str()); x2= atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str()); y2= atoi(tmp);
       Src = Rect(x1,memy1[pc],x2,memy2[pc]);
       Dst = Rect(x1,y1,x2,y2);
       B->Canvas->CopyRect(Dst,B->Canvas,Src);
      }

   else if (strcmp(Cod,"GR")==0)
      {
       strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
       strcpy(tmp,asT->Cells[3][i].c_str()); x2= atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str()); y2=atoi(tmp);
       MyRect = Rect(x1,y1,x2,y2);
       MyOther = Rect(200,200,300,300);
       Bitmap = new Graphics::TBitmap;
       fn = asT->Cells[12][i].c_str();
       Bitmap->LoadFromFile(asT->Cells[12][i].c_str());
       width = Bitmap->Width;
       height = Bitmap->Height;
       //B->Canvas->CopyRect(MyOther,Bitmap->Canvas,MyRect);

        B->Canvas->Draw(x1,y1, Bitmap);
       delete Bitmap;
      }

   else if (strcmp(Cod,"AR")==0)
      {
       strcpy(tmp,asT->Cells[1][i].c_str()); A1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); B1= atoi(tmp);
       strcpy(tmp,asT->Cells[3][i].c_str()); A2= atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str()); B2= atoi(tmp);

       B->Canvas->Brush->Color = clWhite;
       strcpy(xtmp,asT->Cells[7][i].c_str());
       a =atoi(xtmp);

       icolor = a; // StrToInt(asT->Cells[7][i]);
       B->Canvas->Pen->Color = TColor(icolor);  //clGreen;
       B->Canvas->Pen->Style = TPenStyle(pstyle);

       strcpy(xtmp,asT->Cells[8][i].c_str());
       a =atoi(xtmp);
       B->Canvas->Pen->Width=  a; // StrToInt(asT->Cells[8][i]);



       // dessiner les decalages !
       if (B2>B1)  //  successeur est plus bas
         {
          if (A1 < A2)
           {
            B->Canvas->MoveTo(A1,B2);
            B->Canvas->LineTo(A2,B2);

            Fleche(B,'D',A1,B2);
            x3=A1;y3=B1;  x4=A1;y4=B2;

           }
          else if (A1==A2)
           {
            Fleche(B,'D',A2,B2);
            x3=A1;y3=B1;  x4=A2;y4=B2;
           }
          else      // Trait negatif en dessous
           {
            B->Canvas->MoveTo(A1,B1);
            B->Canvas->LineTo(A1,B1+20);
            B->Canvas->LineTo(A2,B1+20);

            Fleche(B,'D',A2,B2);
            x3=A2;y3=B1+20;  x4=A2;y4=B2;
           }
         }

       else // succ est plus haut
         {
          if (A1 > A2)  // trait negatif au dessus
           {
            B->Canvas->MoveTo(A1,B1);
            B->Canvas->LineTo(A1,B1-10);
            B->Canvas->LineTo(A2,B1-10);
            //Fleche(B,'G',A2,B1-10);
            Fleche(B,'D',A2,B2);
            x3=A2; y3=B2; x4=A2; y4=B1-10;
           }
          else if (A1==A2)
           {
             Fleche(B,'D',A2,B2);
             x3=A2;y3=B2;  x4=A1;y4=B1;}
          else
           {
            B->Canvas->MoveTo(A1,B1);
            B->Canvas->LineTo(A2,B1);
            //Fleche(B,'D',A2,B1);
            Fleche(B,'D',A2,B2);
            x3=A2; y3=B2; x4=A2; y4=B1;
           }
         }

       //B->Canvas->Pen->Style = psDot;
       rayon = 1.414*float((y4-y3)/2);

       if (rayon >0)
        {
         x1= x3 -(int) rayon + (y4-y3)/2;
         x2 = x1+ int (2.0*rayon);
         y1= (y3+y4)/2 - int (rayon);
         y2 = y1+ int (2.0 * rayon);
         if (x1 >= P_DEBUTGANTT[pc]) B->Canvas->Arc(x1,y1,x2,y2,x3,y3,x4,y4);
        }
      }
   } // boucle for
}


int __fastcall TplStandard::GetStyle(AnsiString STYLE)
{
 int astyl; char styl[50];
 astyl = 1;
 strcpy(styl,STYLE.c_str());
 if (strstr(styl,"____")) astyl=1;
 else if (strstr(styl,"_  _")) astyl=2;
 else if (strstr(styl,". . .")) astyl=3;
 else if (strstr(styl,"_ . _")) astyl=4;
 else if (strstr(styl,"_ .. _")) astyl=5;
 return astyl;
}

void __fastcall TplStandard::Compute_Marqueur()
{
 /*
 int i;  char tmp[50];
 int rap,cnt;
 rap=pc-4;
 int PAGRAP;
 int y1,item;

 if (rap < 0) {pc=4; rap=0; }
 if (rap > 1) {pc=5; rap=1; }

 for (i=0;i<200;i++) { MARQUEUR[rap][i]=0; NEWY[rap][i]=0; }


 // examiner le tableau   ar;
 cnt=ar->RowCount;

 PAGRAP=1; MARQUEUR[rap][1]= 0;   // 2 eme element de asT;
 for (i=1; i<cnt;i++)
   {
    y1 = atoi(ar->Cells[2][i].c_str());

    if (ar->Cells[0][i] != "")

    { PAGRAP++;  item = atoi(ar->Cells[7][i].c_str());
      MARQUEUR[rap][PAGRAP] = item;
      y1 = atoi(ar->Cells[3][i].c_str());
      NEWY[rap][PAGRAP] = y1;
    }
    ar->Cells[8][i] = PAGRAP;
   }

MAX_PAGE_RAPPORT[pc]=PAGRAP;
PAGE_RAPPORT[pc]=1;
sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
if (pc==4) Label76->Caption = AnsiString(tmp);
if (pc==5) Label84->Caption = AnsiString(tmp);
 */
}



void __fastcall TplStandard::Draw_Rapport(Graphics::TBitmap *B, int pagenum)
{
 TRect R; int cnt,i; char Cod[20];
 TColor c; char tmp[250]; int a;
 int x1,y1,x2,y2;
 TFont *F;
 TRect Dst,Src;
 int pstyle;
 int x3,y3,x4,y4;
 int A1,B1,A2,B2;
 int v1,v2;
 int DEB,FIN,rap;


 F = new TFont;
 // Clear Graphics area

 cnt=asT->RowCount-1;
 if (cnt==2) return;
 rap = pc-4;
 //DEB=MARQUEUR[rap][pagenum];
 //FIN=MARQUEUR[rap][pagenum+1];

// if (FIN>=cnt) FIN=cnt;
// if (FIN==0) FIN=cnt;


 HPEN hPen;
 LOGBRUSH LBrush;

 DWORD pStyle;
/*
 i=1;
 while (i<FIN)

   {
     if (i<=ITEM_ENTETE[rap] || i>=DEB)
     {

     // On traite y1 et y2  en fonction du numero de page
     strcpy(tmp,asT->Cells[1][i].c_str()); x1= atoi(tmp);
     strcpy(tmp,asT->Cells[2][i].c_str()); y1= atoi(tmp);
     strcpy(tmp,asT->Cells[3][i].c_str()); x2= atoi(tmp);
     strcpy(tmp,asT->Cells[4][i].c_str()); y2=atoi(tmp);

    // calculer y1 et y2 en fonction de la page en cours
    if (i<=ITEM_ENTETE[rap])
       {

       }
     else
        {
          y1 = y1 - NEWY[pc-4][PAGE_RAPPORT[pc]];
          //  if (PAGE_RAPPORT[pc]>1) y1= y1;  // + P_MARGH1[pc];
          if (pagenum>1) y1 = y1 + Y_ENTETE[pc-4]; // *(pagenum-1);
          y2 = y2 - NEWY[pc-4][PAGE_RAPPORT[pc]]+ P_MARGH1[pc];
          if (PAGE_RAPPORT[pc]>1) y2= y2+ P_MARGH1[pc];
         if (pagenum>1) y2 = y2 + Y_ENTETE[pc-4]; //*(pagenum-1);
        }
 */

    strcpy(Cod,asT->Cells[0][i].c_str());
    if (strcmp(Cod,"LH")==0)
      {
       strcpy(tmp,asT->Cells[13][i].c_str());
       pstyle = atoi(tmp);
       if (pstyle==0) pstyle=psSolid;
       else if (pstyle==1) pstyle=psSolid;
       else if (pstyle==2) pstyle=psDash;
       else if (pstyle==3) pstyle=psDot;
       else if (pstyle==4) pstyle=psDashDot;
       else if (pstyle==5) pstyle=psDashDotDot;
       else if (pstyle==6) pstyle=psClear;
       else pstyle=psSolid;

       pStyle  = PS_GEOMETRIC | pstyle | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;

       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       LBrush.lbStyle = BS_SOLID;
       LBrush.lbColor = c;
       LBrush.lbHatch = HS_DIAGCROSS;
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);
       hPen = ExtCreatePen(pStyle, a, &LBrush, 0, NULL);

       B->Canvas->Pen->Handle = hPen;
       B->Canvas->Pen->Width = a;

       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Pen->Color = c;
       B->Canvas->MoveTo(x1,y1);
       B->Canvas->LineTo(x2,y1);

      }
    if (strcmp(Cod,"LV")==0)
      {

       strcpy(tmp,asT->Cells[13][i].c_str());
       pstyle = atoi(tmp);
       if (pstyle==0) pstyle=psSolid;
       else if (pstyle==1) pstyle=psSolid;
       else if (pstyle==2) pstyle=psDash;
       else if (pstyle==3) pstyle=psDot;
       else if (pstyle==4) pstyle=psDashDot;
       else if (pstyle==5) pstyle=psDashDotDot;
       else if (pstyle==6) pstyle=psClear;
       else pstyle=psSolid;

       pStyle  = PS_GEOMETRIC | pstyle | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL;

       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       LBrush.lbStyle = BS_SOLID;
       LBrush.lbColor = c;
       LBrush.lbHatch = HS_DIAGCROSS;
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);
       hPen = ExtCreatePen(pStyle, a, &LBrush, 0, NULL);

       B->Canvas->Pen->Handle = hPen;
       B->Canvas->Pen->Width = a;

       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Pen->Color = c;

       strcpy(tmp,asT->Cells[10][i].c_str()); a= atoi(tmp);
       B->Canvas->MoveTo(x1,y1);
       B->Canvas->LineTo(x1,y2);
      }

    else if (strcmp(Cod,"TX")==0)
      {
       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       B->Canvas->Brush->Color = c;
       B->Canvas->Brush->Style = bsClear;
       strcpy(tmp,asT->Cells[7][i].c_str()); a= atoi(tmp);
       c=TColor(a); F->Color=c;
       F->Name = asT->Cells[9][i];
       strcpy(tmp,asT->Cells[8][i].c_str()); a= atoi(tmp);
       F->Size = a;
       F->Style = TFontStyles();
       strcpy(tmp,asT->Cells[10][i].c_str());
       if (strcmp(tmp,"I")==0) F->Style = F->Style << fsItalic;
       strcpy(tmp,asT->Cells[11][i].c_str()); a= atoi(tmp);
       if (strcmp(tmp,"B")==0) F->Style = F->Style << fsBold;

       B->Canvas->Font = F;
       strcpy(tmp,asT->Cells[1][i].c_str()); //x1= atoi(tmp);
       strcpy(tmp,asT->Cells[2][i].c_str()); //y1= atoi(tmp);
       strcpy(tmp,asT->Cells[12][i].c_str());

       if (strcmp(tmp,"$page")==0)
          {
            sprintf(tmp,"Page %d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
            B->Canvas->TextOut(x1,y1,AnsiString(tmp));
          }
       else if (strcmp(tmp,"$date")==0) B->Canvas->TextOut(x1,y1,DateToStr(Date()));
       else  B->Canvas->TextOut(x1,y1,asT->Cells[12][i]);
      }

   else if (strcmp(Cod,"RT")==0)   // rectangle couleur
      {
       strcpy(tmp,asT->Cells[6][i].c_str()); a= atoi(tmp);
       c=TColor(a);
       R = Rect(x1,y1,x2,y2);
       B->Canvas->Brush->Color = c;
       B->Canvas->FillRect(R);
      }
   else if (strcmp(Cod,"CR")==0)   // copie rect
      {

       Src = Rect(x1,memy1[pc],x2,memy2[pc]);
       Dst = Rect(x1,y1,x2,y2);
       B->Canvas->CopyRect(Dst,B->Canvas,Src);
      }
}



void __fastcall TplStandard::BitBtn3Click(TObject *Sender)
{
 int XRow,i; int cc;
 int count;
 AnsiString exch[81];

 if (OldRow==0)
   {Application->MessageBoxA("Pas de ligne s�lectionn�e",m_ecoplan,MB_OK);
    return;
   }
 count=asTri->RowCount-1;
 if(OldRow == 1) return;
 // l0= asFC->Cells[0][OldRow];

 cc=asTri->ColCount-1;
 for (i=0;i<cc;i++) { exch[i]= asTri->Cells[i][OldRow]; }
 XRow=OldRow-1;
 for (i=0;i<cc;i++) { asTri->Cells[i][OldRow]=asTri->Cells[i][XRow]; asTri->Cells[i][XRow]=exch[i];}
  OldRow=XRow;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::BitBtn4Click(TObject *Sender)
{
int XRow;
 int indx,count,i,cc;;
 AnsiString exch[81];

 if (OldRow==0)
   {Application->MessageBoxA("Pas de ligne s�lectionn�e",m_ecoplan,MB_OK);
    return;
   }
 count = asTri->RowCount-1;
 if(OldRow == count) return;
 if (OldRow ==0) return;

 cc=asTri->ColCount-1;
 for (i=0;i<cc;i++) { exch[i]= asTri->Cells[i][OldRow]; }
 XRow=OldRow+1;
 for (i=0;i<cc;i++) { asTri->Cells[i][OldRow]=asTri->Cells[i][XRow]; asTri->Cells[i][XRow]=exch[i]; }
 OldRow=XRow;

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::asTriClickCell(TObject *Sender, int ARow,
      int ACol)
{
 OldRow=ARow;
 OldCol=ACol;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Insert_G(char ctx,char *typ,int x1,int y1,int x2,int y2,
        char *ref, TColor bg, TColor fg, int epais, int style, AnsiString font, AnsiString  ital,
        AnsiString bold, AnsiString text)
{
 int r;

 asT->RowCount++;
 r=asT->RowCount-1;
 asT->Cells[0][r] = typ;
 asT->Cells[1][r] = x1;
 asT->Cells[2][r] = y1;
 asT->Cells[3][r] = x2;
 asT->Cells[4][r] = y2;
 asT->Cells[5][r] = ref;
 asT->Cells[6][r] = (int)bg;
 asT->Cells[7][r] = (int)fg;
 asT->Cells[8][r] = epais;
 asT->Cells[9][r] = font;      // ou style trait pour LH ou LV
 asT->Cells[10][r] = ital;
 asT->Cells[11][r] = bold;
 asT->Cells[12][r] = text;
 asT->Cells[13][r] = style;
 asT->Cells[18][r] = ctx;


}


void __fastcall TplStandard::btRenumClick(TObject *Sender)
{

 TRenum *Ren;
 Ren = new TRenum(Application);
 Ren->ShowModal();
 delete Ren;


 // Renum();
}

void __fastcall TplStandard::Renum()
{
 int cnt,t,n,i,lg;
 char tmp[MAX_PATH],tmp1[MAX_PATH],buf[15000],buf1[15000];
 FILE *fp,*fp1;
 char name1[MAX_PATH],name2[MAX_PATH];
 char *p,*p1;
 AnsiString fname;
 int rc,recdata; char Key[100],RetKey[100];
 int n1,ipred,isucc,j,compt;
 int x,ii,nt;
 char cdebsouh[20],cfinsouh[20],xtmp[300];
 char format[15000];  int nbpred,nbsucc;
 char strpred[200],strsucc[200];
 char grostmp[15000];

  if (Application->MessageBoxA("Votre renum�rotation pr�c�dente dans votre projet sera �cras�e par celle-ci. OK ?",
   m_ecoplan,MB_YESNO)==IDNO) return;

 // cnt = asTri->RowCount;
 cnt = AdvStringGrid1->RowCount;
 cnt--;
 if (cnt != nb_taches)
   {
    if (Application->MessageBoxA("Attention : Toutes les t�ches ne sont pas pr�sentes. Voulez vous Continuer ?",
     m_ecoplan,MB_YESNO)==IDNO) return;
   }

Regen_Criteres();
CopyCriteres();

for (i=0;i<MAX_T;i++)  renum[i]=0;
// renum stocke le nouvel ordre
for (i=1;i<=cnt;i++)
  {
   strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
   renum[i]=atoi(tmp);
  }

// Renum Crit�res


for (i=1;i<=cnt;i++)
  {
   strcpy(tmp,asCrit->Cells[1][i].c_str());
   nt = atoi(tmp);
   for (j=1;j<=cnt;j++)
     {
      if (renum[j]==nt) break;
     }

   asCrit->Cells[1][i]= AnsiString(j);
  }

 Save_Crit_Renum();


  //  Boucle pour zone memoire Predecesseurs et Successeurs
  //  Ecriture dans un fichier Isam

  strcpy(name1,m_directory);strcat(name1,"\\temp.ndx"); rc=unlink(name1);
  strcpy(name2,m_directory);strcat(name2,"\\temp.dat"); rc=unlink(name2);

  strcpy(tmp,m_directory);strcat(tmp,"\\temp");
  temp = new realisam();
  rc = temp->OpenEngine(tmp,120,"ndx","dat");
  rc = temp->SetIndexMode(0,0);
  if (rc==0) { ShowMessage("Erreur � l'ouverture fichier temp"); return; }
  for (i=1;i<=nb_taches;i++)
  //  for (i=1;i<=LASTTASK;i++)
   {


       n=renum[i];
       sprintf(Key,"<T%04d>P",i);   // dans l'ordre de ADV1 exemple tache 10 devient  1
       recdata=temp->GetNewRecordNumber();
       rc=temp->WriteKey(0,Key,recdata);

       ipred=1; buf[0]=0;
       while ((pr[n][ipred]!=0) && (ipred<PS))   // $$ i au lieu de N
         {  x = pr[n][ipred];
            n1=Posit(x);
            sprintf(buf1,"<P%02d><n>%d</n><t>%d</t><d>%d</d></P%02d>",
               ipred,n1,pr_tl[n][ipred],pr_decal[n][ipred],ipred);
            strcat(buf,buf1);
          ipred++;
         }
       rc = temp->WriteRecord(buf,strlen(buf)+1);

       sprintf(Key,"<T%04d>S",i);
       recdata=temp->GetNewRecordNumber();
       rc=temp->WriteKey(0,Key,recdata);

       isucc=1; buf[0]=0;
       while ((su[n][isucc]!=0)  && (isucc<PS))
          {
           x = su[n][isucc];
           if (x==9999)
             n1=9999;
             else n1=Posit(x);
           sprintf(buf1,"<S%02d><n>%d</n><t>%d</t><d>%d</d></S%02d>",
               isucc,n1,su_tl[n][isucc],su_decal[n][isucc],isucc);
            strcat(buf,buf1);
          isucc++;
         }
     rc=temp->WriteRecord(buf,strlen(buf)+1);
    }

  // relire le fichier sequentiellement et recreer pr,su,pr_decal, su_decal, pr_tl,su_tl

   strcpy(tmp,m_directory);strcat(tmp,"\\temp");
   rc=temp->CloseReopen(tmp,120,"ndx","dat");
   rc = temp->SetIndexMode(0,0);

   // RAZ des pr�d et Succeseeurs
   for (i=1;i<=LASTTASK;i++)
     {
      for (j=1;j<PS;j++)  { pr[i][j]=0; su[i][j]=0; }
     }

   rc = temp->ReadFirstKey(0,RetKey,&recdata);
   while (rc)
    {  buf[0]=0;
       rc = temp->ReadRecord(buf,recdata);  // read record in buffer
       buf[rc]=0;
       strncpy(tmp,RetKey+2,4); tmp[4]=0; n=atoi(tmp);
       if (RetKey[7]=='P')  // predecesseur
         {
         for (j=1;j<PS;j++)
          {
           sprintf(tmp,"P%02d",j);
           lg=Ghost->ExtractValue(buf1,buf,tmp,0);
           if (lg==0) { pr[n][j]=0; break; }
           lg=Ghost->ExtractValue(tmp,buf1,"n",0);
           pr[n][j]=atoi(tmp);
           lg=Ghost->ExtractValue(tmp,buf1,"t",0);
           pr_tl[n][j]=atoi(tmp);
           lg=Ghost->ExtractValue(tmp,buf1,"d",0);
           pr_decal[n][j]=atoi(tmp);
          }
         }
       else // successeurs
         {
          for (j=1;j<PS;j++)
          {
           sprintf(tmp,"S%02d",j);
           lg=Ghost->ExtractValue(buf1,buf,tmp,0);
           if (lg==0) { su[n][j]=0; break; }
           lg=Ghost->ExtractValue(tmp,buf1,"n",0);
           su[n][j]=atoi(tmp);
           lg=Ghost->ExtractValue(tmp,buf1,"t",0);
           su_tl[n][j]=atoi(tmp);
           lg=Ghost->ExtractValue(tmp,buf1,"d",0);
           su_decal[n][j]=atoi(tmp);
          }

         }
       rc=temp->ReadNextKey(RetKey,&recdata);
     }  // while

 // Detruire les fichiers temp.
  temp->CloseEngine();
  // delete temp;
  strcpy(name1,m_directory);strcat(name1,"\\temp.ndx"); rc=unlink(name1);
  strcpy(name2,m_directory);strcat(name2,"\\temp.dat"); rc=unlink(name2);

// $$$$$ RENUMEROTER LES AVANCEMENTS  (PHASE 2)

// Refaire la Data Base

    proj->CloseEngine();
    // delete proj;

    strcpy(name2,LocalBase); strcat(name2,".dat");
    rc=unlink(name2);
    strcpy(name2,LocalBase); strcat(name2,".eco");
    rc=unlink(name2);

    strcpy(name1,LocalBase);
    proj = new realisam;
    rc = proj->OpenEngine(name1,120,"eco","dat");   //512 - 8
    rc = proj->SetIndexMode(0,0);

    Write_Debut_DB(0);  // on garde la date avancement

    cnt=AdvStringGrid1->RowCount;

    for (int i=1;i<cnt;i++)
      {
       //if (av->Cells[1][i] != "")
       //{
       strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
       // ii=atoi(tmp);     // doit etre 1,2,3
       ii=i;
       n=renum[i];
       strcpy(strpred,"|");
       sprintf(Key,"<PR%04d>",ii);
       recdata=proj->GetNewRecordNumber();
       rc = proj->WriteKey(0,Key,recdata);
       ipred=1;
       buf[0]=0; sprintf(buf,"<PR%04d>",ii);
       while ((pr[ii][ipred]!=0) && (ipred<PS))       //
         {

            sprintf(tmp,"<P%02d><num>%d</num><typ>%d</typ><decal>%d</decal></P%02d>",
               ipred,pr[ii][ipred],pr_tl[ii][ipred],pr_decal[ii][ipred],ipred);
            strcat(buf,tmp);
          //sprintf(ss,"%d|",pr[ii][ipred]);
          //strcat(strpred,ss);
          ipred++;
         }
       //len =strlen(strpred);
       nbpred=ipred-1;
       sprintf(tmp,"</PR%04d>",ii); strcat(buf,tmp);
       rc = proj->WriteRecord(buf,strlen(buf)+1);

       //  successeurs
       sprintf(Key,"<SU%04d>",ii);
       recdata=proj->GetNewRecordNumber();
       rc = proj->WriteKey(0,Key,recdata);
       isucc=1; strcpy(strsucc,"|");
       buf[0]=0; sprintf(buf,"<SU%04d>",ii);
       while ((su[ii][isucc]!=0)  && (isucc<PS))
         {
             sprintf(tmp,"<S%02d><num>%d</num><typ>%d</typ><decal>%d</decal></S%02d>",
               isucc,su[ii][isucc],su_tl[ii][isucc],su_decal[ii][isucc],isucc);
            strcat(buf,tmp);
            //sprintf(ss,"%d|",su[ii][isucc]);
            // strcat(strsucc,ss);
          isucc++;
         }
       //len=strlen(strsucc);
       nbsucc=isucc-1;
       sprintf(tmp,"</SU%04d>",ii); strcat(buf,tmp);
       rc = proj->WriteRecord(buf,strlen(buf)+1);

       sprintf(Key,"<T%04d>",ii);
       recdata=proj->GetNewRecordNumber();
       rc = proj->WriteKey(0,Key,recdata);


       strcpy(cdebsouh,av->Cells[12][n].c_str()); //dds[i],
       strcpy(cfinsouh,av->Cells[13][n].c_str()); //dfs[i],

       strcpy(xtmp,av->Cells[2][n].c_str());
       p=strstr(xtmp,"\r");
       if (p) *p=0;
       p=strstr(xtmp,"\n");
       if (p) *p=0;
       //  av->Cells[2][i] = AnsiString(xtmp);

       strcpy(buf,av->Cells[2][n].c_str());     // $$$$$ PAS II


       strcpy(format,"<T>%d</T><nom>%s</nom><duree>%s</duree><cal>%s</cal><mom>%s</mom>");
       strcat(format,"<ct>%s</ct><da>%s</da><dtr>%s</dtr><ftr>%s</ftr><pc>%s</pc><nbj>%s</nbj>");
       strcat(format,"<dds>%s</dds><dfs>%s</dfs>");
       strcat(format,"<nbpred>%d</nbpred><nbsucc>%d</nbsucc>");
       strcat(format,"<pred>%s</pred><succ>%s</succ><coutp>%s</coutp><coutn>%s</coutn>");
       strcat(format,"<cdtch>%s</cdtch><chl1>%s</chl1><chl2>%s</chl2><chl3>%s</chl3>");
       sprintf(grostmp,format,
           ii,
           buf,       // OK
           av->Cells[3][n].c_str(),    // duree[i],
           av->Cells[10][n].c_str(),     // tcal[i],
           tmom[n],        ///    i ou ii = les memes ?
           av->Cells[11][n].c_str(),  //codetache[i],
           da[n],
           av->Cells[4][n].c_str(),    /// debut plus tot
           av->Cells[5][n].c_str(),    // fin plus tot
           av->Cells[31][n].c_str(),   // pourcentage avancementpca[i],
           av->Cells[32][n].c_str(),    // Nb Jour Restant nbj[i],

           av->Cells[12][n].c_str(),    // cdebsouh,
           av->Cells[13][n].c_str(),    //cfinsouh,
           nbpred,             // OK
           nbsucc,             // OK
           strpred,            // OK
           strsucc,            // OK
           av->Cells[18][n].c_str(),      /// couts pos et neg
           av->Cells[19][n].c_str(),      /// cout neg
           av->Cells[11][n].c_str(),      // code tache
           av->Cells[15][n].c_str(),      /// Champ1
           av->Cells[16][n].c_str(),      /// champ2
           av->Cells[17][n].c_str()       /// Champ3

           );

       // }  // if (ii !=0)
       rc = proj->WriteRecord(grostmp,strlen(grostmp)+1);

     }      // boucle for

     rc = proj->CloseReopen(name1,120,"eco","dat");    //  CloseEngine();
     rc = proj->SetIndexMode(0,0);






   ReloadAllData(LocalBase,false,false);

}
//---------------------------------------------------------------------------

int  __fastcall TplStandard::Posit(int n)
{
 int i,res;
 i=1; res = 0;
 while (renum[i] != 0)
  {
   if (n==renum[i])
     {
      res=i;
      break;
     }
   i++;
  }
 return res;
}

int __fastcall TplStandard::exist_renum(int t)
{
 int i;
 int val;
 val=0;
 for (i=1;i<=nb_taches;i++)
   {
    if (renum[i]==t) val=i;
   }
 return val;

}

void __fastcall TplStandard::SaveAv()
{
 FILE *fp; int i,j,cntrow,cntcol;
 char filename[MAX_PATH];
 char record[5000];  char tmp[500];
 strcpy(filename,LocalBase);
 strcat(filename,"_av.txt");

 fp=fopen(filename,"wb");
 if (fp==NULL) return;
 cntrow=av->RowCount;
 cntcol=59;

 for (i=1;i<cntrow;i++)
   {
    strcpy(record,"");
    for (j=0;j<cntcol;j++)
      {
       sprintf(tmp,"<c%02d>%s</c%02d>",j,av->Cells[j][i].c_str(),j);
       strcat(record,tmp);
      }
    strcat(record,"\n");
    fputs(record,fp);
   }
 fclose(fp);
}
void __fastcall TplStandard::Button6Click(TObject *Sender)
{
 SaveAv();
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::btAideSuiviClick(TObject *Sender)
{
 Ghost->Help("index.html");
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button2Click(TObject *Sender)
{

 TTri *Tri;

 strcpy(m_exchange,"<util>utiliser</util><tri>");
  strcat(m_exchange,P_TRI[pc]);
 strcat(m_exchange,"</tri>");
 Tri = new TTri(Application);
 Tri->Height=619;
 Tri->Width=754;
 Tri->ShowModal(); //Modal();
 delete Tri;
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::Load_Couts()
{
 int cnt,i,indx;
 Clean_Grid(avc,2);
 cnt = av->RowCount;
 indx=0;

 for (i=1;i<cnt;i++)
   {
    if (av->Cells[1][i] != "" && av->Cells[1][i] != " ")
      {
        indx++; if (indx>1) avc->RowCount++;

        avc->Cells[1][indx]=av->Cells[1][i];
        avc->Cells[2][indx]=av->Cells[2][i];
        avc->Cells[3][indx]=av->Cells[3][i];
        avc->Cells[4][indx]=av->Cells[4][i];
        avc->Cells[5][indx]=av->Cells[5][i];
        avc->Cells[6][indx]=av->Cells[84][i];
        avc->Cells[7][indx]=av->Cells[83][i];
       avc->Cells[8][indx]=asCrit->Cells[9][i];
       avc->Cells[9][indx]=asCrit->Cells[3][i];
       avc->Cells[10][indx]=asCrit->Cells[5][i];
       avc->Cells[11][indx]=asCrit->Cells[7][i];
       avc->Cells[12][indx]=av->Cells[18][i];
       avc->Cells[13][indx]=av->Cells[19][i];
	   }
   }
 Calcul_Echeancier();
}
void __fastcall TplStandard::Button7Click(TObject *Sender)
{
 int i,cnt,tch; char tmp[500]; int nt;
 char cp[20],cn[20]; char flottant[30]; double fl;
 char name[MAX_PATH]; FILE *fp;
 return;
 cnt=avc->RowCount;
 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,avc->Cells[1][i].c_str());
    tch = atoi(tmp);
    if (tch !=0)
      {
       strcpy(tmp,avc->Cells[6][i].c_str());  fl=atof(tmp);
       sprintf(flottant,"%8.2f",fl);
       av->Cells[18][tch]= AnsiString(flottant);

       strcpy(tmp,avc->Cells[7][i].c_str());  fl=atof(tmp);
       sprintf(flottant,"%8.2f",fl);
       av->Cells[19][tch]= AnsiString(flottant);
      }
   }

 // Maintenant on sauvegarde
 strcpy(name,LocalBase);
 strcat(name,"_couts.txt");
 fp=fopen(name,"wb");
 if (fp)
   {
    cnt=av->RowCount;
    for (i=1;i<cnt;i++)
      {
       strcpy(cp,av->Cells[18][i].c_str());
       strcpy(cn,av->Cells[19][i].c_str());
       sprintf(tmp,"<t>%d</t><p>%s</p><n>%s</n>\n",i,cp,cn);
       fputs(tmp,fp);
      }

    // boucle pour l'�cheancier

    fclose(fp);
   }
 Change1();

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::avcCanEditCell(TObject *Sender, int ARow,
      int ACol, bool &CanEdit)
{

 CanEdit = false;
 if (ARow ==0) return;

 if (ACol==6 || ACol==7)
 {
 CanEdit=true;
 AdvStringGrid1->Cells[14][ARow] = avc->Cells[ACol][ARow];
 }
AdvStringGrid1->Refresh();
}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::refresh_couts()
{
 // refresh couts pour toute modif de dur�e , de dates, designation, etc ...
 int cnt,i, tch; char tmp[150];

 cnt=avc->RowCount;
 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,avc->Cells[1][i].c_str());
    tch=atoi(tmp);
    if (tch!=0)
     {
      avc->Cells[1][i]=av->Cells[1][tch];
      avc->Cells[2][i]=av->Cells[2][tch];
      avc->Cells[3][i]=av->Cells[3][tch];
      avc->Cells[4][i]=av->Cells[4][tch];
      avc->Cells[5][i]=av->Cells[5][tch];

      avc->Cells[8][i]=av->Cells[41][tch];
      avc->Cells[9][i]=av->Cells[42][tch];
      avc->Cells[10][i]=av->Cells[43][tch];

     }
   }
}
*/

void __fastcall TplStandard::refresh_couts()
{
 // refresh couts pour toute modif de dur�e , de dates, designation, etc ...
 int cnt,indx,i, tch; char tmp[150];

 Clean_Grid(avc,2);
 cnt=av->RowCount;
 indx=0;
 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,av->Cells[1][i].c_str());
    tch=atoi(tmp);
    if (tch!=0)
     {
      indx++; if (indx>1) avc->RowCount++;

      avc->Cells[1][indx]=av->Cells[1][i];
      avc->Cells[2][indx]=av->Cells[2][i];
      avc->Cells[3][indx]=av->Cells[3][i];
      avc->Cells[4][indx]=av->Cells[4][i];
      avc->Cells[5][indx]=av->Cells[5][i];

       avc->Cells[8][indx]=asCrit->Cells[9][i];
       avc->Cells[9][indx]=asCrit->Cells[3][i];
       avc->Cells[10][indx]=asCrit->Cells[5][i];
       avc->Cells[11][indx]=asCrit->Cells[7][i];
     }
   }
}

void __fastcall TplStandard::RadioButton1Click(TObject *Sender)
{
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::RadioButton2Click(TObject *Sender)
{
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::RadioButton3Click(TObject *Sender)
{
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Calcul_Echeancier()
{
 int mode; int dp1,dp2; char tmp[100],mt[100];
 char xdat[50]; int i,j,cnt;  int cntt;  int dur,dst; bool ok;
 TDateTime dtFirst; int day,dist1;
 double montpos,montneg,mttot,cumul;
 double daypos,dayneg;

 int nbdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

 int sp1[2000],sp2[2000], ctrsp;
 if (nb_taches==0) return;


 //////����������

 mode =1;
 if (RadioButton1->Checked==true) mode = 1;
 if (RadioButton2->Checked==true) mode = 2;
 if (RadioButton3->Checked==true) mode = 3;


 sprintf(xdat,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev%100);
 dp1=convert_date(AnPrev,MoisPrev,JourPrev);

 sprintf(xdat,"%02d/%02d/%02d",JourFin,MoisFin,AnFin%100);
 dp2=convert_date(AnFin,MoisFin,JourFin);
 if (dp2 > MAX_JOURS+10) dp2= MAX_JOURS-10;

 dtFirst = EncodeDate(AnPrev,MoisPrev,JourPrev);
 int firstday = dtFirst.DayOfWeek();
 int dow;  // day of week
 dow = firstday-1;

 Clean_Grid(ave,2);
 ave->ColWidths[5]=0;
 ave->ColWidths[6]=0;
 cnt=0;
 switch (mode)
   {
    case 1:
        sp1[cnt]=0;
        for (i=dp1;i<=dp2;i++)
          {
           add_jour(AnPrev,MoisPrev,JourPrev,i);
           sprintf(xdat,"%02d/%02d/%02d",Jour,Mois,An%100);
           if (cal[1][i]=='T')
             {
              cnt++;  if (cnt>1) ave->RowCount++;
              ave->Cells[1][cnt]=AnsiString(xdat);
              ave->Cells[2][cnt]=AnsiString(xdat);
              sp1[cnt]=i;
             }
          }

        break;

    case 2:
        day=dp1;
        add_jour(AnPrev,MoisPrev,JourPrev,day);
        dtFirst = EncodeDate(AnPrev,MoisPrev,JourPrev);
        dow= dtFirst.DayOfWeek()-1; // dernier jour de la semaine -1;
        sp1[cnt]=day;
        cnt++;
        sprintf(xdat,"%02d/%02d/%02d",Jour,Mois,An%100);
        ave->Cells[1][cnt]=AnsiString(xdat);

        add_jour(AnPrev,MoisPrev,JourPrev,day+4);
        sprintf(xdat,"%02d/%02d/%02d",Jour,Mois,An%100);
        ave->Cells[2][cnt]=AnsiString(xdat);


        day++;
        while (day<=dp2)
          {
           add_jour(AnPrev,MoisPrev,JourPrev,day);
           dtFirst = EncodeDate(An,Mois,Jour);
           dow= dtFirst.DayOfWeek()-1;
           if (dow==1)
            {
             cnt++; if (cnt>1) ave->RowCount++;
             sprintf(xdat,"%02d/%02d/%02d",Jour,Mois,An%100);
             ave->Cells[1][cnt]=AnsiString(xdat);
             sp1[cnt]=day;


             add_jour(AnPrev,MoisPrev,JourPrev,day+4);
             sprintf(xdat,"%02d/%02d/%02d",Jour,Mois,An%100);
             ave->Cells[2][cnt]=AnsiString(xdat);
            }
           day++;
          }


        break;

    case 3:
        day=dp1;
        add_jour(AnPrev,MoisPrev,JourPrev,day);
        dtFirst = EncodeDate(AnPrev,MoisPrev,JourPrev);
        dow= dtFirst.DayOfWeek()-1;
        cnt++;
        //dist1=avance_conges(day,1);
        sp1[cnt]=day; // day;
        sprintf(xdat,"%02d/%02d/%02d",Jour,Mois,An%100);
        ave->Cells[1][cnt]=AnsiString(xdat);

        if (Mois==2){ if (IsLeapYear(An)) nbdays[1] = 29;}

        sprintf(xdat,"%02d/%02d/%02d",nbdays[Mois-1],Mois,An%100);
        ave->Cells[2][cnt]=AnsiString(xdat);

        day++;
        while (day<=dp2)
          {
           add_jour(AnPrev,MoisPrev,JourPrev,day);
           if (Jour==1)  // debut de mois
            {
             cnt++; if (cnt>1) ave->RowCount++;
             day=avance_conges(day,1);
             add_jour(AnPrev,MoisPrev,JourPrev,day);
             sprintf(xdat,"%02d/%02d/%02d",Jour,Mois,An%100);
             ave->Cells[1][cnt]=AnsiString(xdat);
             sp1[cnt]=day;

             if (Mois==2){ if (IsLeapYear(An)) nbdays[1] = 29;}
             sprintf(xdat,"%02d/%02d/%02d",nbdays[Mois-1],Mois,An%100);
             ave->Cells[2][cnt]=AnsiString(xdat);


            }
           day++;
          }
        break;
   }

sp1[cnt+1]=sp1[cnt]+2000;   // overflow

// boucle pour distribuer les montants des taches dans la grille echeancier

cntt= avc->RowCount;
sprintf(tmp,"%.6f",0.0);
for (i=1;i<=cnt;i++) { ave->Cells[5][i]=AnsiString(tmp); ave->Cells[6][i]=AnsiString(tmp); }


 TOTAL_NEG = 0.0;
 TOTAL_POS = 0.0;
for (i=1;i<cntt;i++)
  {
   strcpy(tmp,avc->Cells[3][i].c_str());
   dur = atoi(tmp);
   if (dur <=0) dur = 1;
   // calcul dist1 en fonction de la date
   strcpy(xdat,avc->Cells[4][i].c_str());
   dist1=convert_datex(xdat);
  if(modeal==1){
   strcpy(mt,avc->Cells[13][i].c_str());}
   if(modeal==2){
    strcpy(mt,AdvStringGrid1->Cells[23][i].c_str());
   }
   montpos=atof(mt);
   TOTAL_POS += montpos;
   if(modeal==1){
   strcpy(mt,avc->Cells[14][i].c_str());}
   if(modeal==2){

    strcpy(mt,AdvStringGrid1->Cells[24][i].c_str());
   }
   montneg=atof(mt);
   TOTAL_NEG += montneg;
   if (dur != 0)
     {
       daypos=montpos/((double) dur);
       dayneg=montneg/((double) dur);
     }

   ok = true;
   dst=dist1;
   while (ok)
     {
      if (dur==0) { ok = false; break; }
      dst=avance_conges(dst,1);
      for (j=1;j<=cnt;j++)
       {
        if (dst>=sp1[j] && dst<sp1[j+1])
         {
          strcpy(mt,ave->Cells[5][j].c_str());
          mttot=atof(mt);
          if (cbPos->Checked==true)  mttot = mttot + daypos;
          else mttot = mttot - dayneg;
          //sprintf(tmp,"<P align=\"right\">%10.2f</P>",mttot);
          sprintf(tmp,"%.6f",mttot);

          ave->Cells[5][j]=AnsiString(tmp);
          sprintf(tmp,"%.2f",mttot);
          ave->Cells[3][j]=AnsiString(tmp);

          dur=dur-1;
          if (dur < 1) {ok = false; break; }
          }

       }
      dst++;
      if (dst>dp2) ok=false;
     } //while ok


  }  // for cntt



 cumul=0.0;
 for (j=1;j<=cnt;j++)
     {
         strcpy(mt,ave->Cells[5][j].c_str());
         mttot=atof(mt);
         cumul += mttot;
         // sprintf(tmp,"<P align=\"right\">%10.2f</P>",cumul);
         sprintf(tmp,"%.6f",cumul);
         ave->Cells[6][j]=AnsiString(tmp);
         sprintf(tmp,"%.2f",cumul);
         ave->Cells[4][j]=AnsiString(tmp);
     }
}


int  __fastcall TplStandard::convert_datex(char *dat)
{
 int n1,n2,bcl,N;
 int initval,year,month,day;
 char dt[20];

 dt[0]=dat[0]; dt[1]=dat[1]; dt[2]=0;  day=atoi(dt);
 dt[0]=dat[3]; dt[1]=dat[4]; dt[2]=0;  month=atoi(dt);
 dt[0]=dat[6]; dt[1]=dat[7]; dt[2]=0;  year=atoi(dt)+2000;

  n1 = (AnPrev%100)*365 + MoisPrev*28 + JourPrev;
  n2 = (year%100)*365 + month*28 + day;
 if (n2<n1) n2=n1+30;
  initval = n2-n1-30; N=initval;
  for (bcl=0;bcl<100;bcl++)
    {
     add_jour(AnPrev,MoisPrev,JourPrev,N);
     if ((An==year) && (Mois==month) && (Jour==day)) break;
     else N++;
    }
 return N;
}

void __fastcall TplStandard::avcCellValidate(TObject *Sender, int ACol,
      int ARow, AnsiString &Value, bool &Valid)
{
 //
 int tach_ech,tach_adv,i,count;
 char tmp[100];

 Calcul_Echeancier();
 Change1();
 if (ARow==0) return;

 strcpy(tmp,avc->Cells[1][ARow].c_str());
 tach_ech=atoi(tmp);
 if (tach_ech==0) return;

 // MAJ dans AdvStringgrid1;
 count = AdvStringGrid1->RowCount;
 for (i=1;i<count;i++)
   {
    strcpy(tmp,AdvStringGrid1->Cells[1][i].c_str());
    tach_adv = atoi(tmp);
    if (tach_ech == tach_adv)
       { if (ACol== 6)
        { AdvStringGrid1->Cells[24][i] = avc->Cells[6][ARow];
          av->Cells[84][tach_ech] = avc->Cells[6][ARow]; }

         if (ACol== 7)
           {av->Cells[83][tach_ech] = avc->Cells[7][ARow]; }
        break;
       }
   }
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::cbPosClick(TObject *Sender)
{
 if (cbPos->Checked == true) cbNeg->Checked = false;
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::aveGetAlignment(TObject *Sender, int ARow,
      int ACol, TAlignment &HAlign, TVAlignment &VAlign)
{
 HAlign = taRightJustify;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button8Click(TObject *Sender)
{
TxFilter *filt;
 char tmp[250];
 char strfilter[100];

 strcpy(m_exchange,"<util>utiliser</util>");

 strcpy(m_project,DatabaseName);
 filt = new TxFilter(Application);
 filt->Height = 705;
 filt->Width = 774;

//  filt->Visible=true;
 m_filtre[0]=0;
 filt->ShowModal(); //Modal();
 delete filt;

}
//---------------------------------------------------------------------------



void __fastcall TplStandard::AffecterDonnesauxTachesCoches1Click(
      TObject *Sender)
{
 Affect_Data();
}

 void __fastcall TplStandard::Affect_Data()
{

TAffData *Aff;
int ix,cnt,ctask,Col,dur;
char champ[200],cod[200],col[20],des[200];
char xtask[20]; char tmp[200];

 Aff = new TAffData(Application);
 Aff->ShowModal();
 delete Aff;

 if (strlen(m_exchange)==0) return;

// Application->MessageBoxA(m_exchange,m_ecoplan,MB_OK);

  Ghost->ExtractValue(champ,m_exchange,"champ",0);
  Ghost->ExtractValue(cod,m_exchange,"cod",0);
  Ghost->ExtractValue(col,m_exchange,"col",0);
  Ghost->ExtractValue(des,m_exchange,"des",0);

  Col=atoi(col);
  if (Col<=0)
    {
     Application->MessageBoxA("Champ non identifi�",m_ecoplan,MB_OK);
     return;
    }

 if (Col>=15) Col=Col+3;     // D�calage a cause des 3 Crit�res

 new_nb_taches=nb_taches;
 cnt=AdvStringGrid1->RowCount;

 for (ix=1;ix<cnt;ix++)
  {
   strcpy(xtask,AdvStringGrid1->Cells[1][ix].c_str());
   ctask = atoi(xtask);
   if (ctask!=0 && coche[ctask]=='X')
    {
     if (strlen(cod))  // c'est un crit�re
       {
        //sprintf(tmp,"Cod = %s Col= %s  ",cod,col);
        //Application->MessageBox(tmp,"ecopl",MB_OK);
        AdvStringGrid1->Cells[Col+40][ix]=AnsiString(cod);
        av->Cells[Col+40][ctask]=AnsiString(cod);
        AdvStringGrid1->Cells[Col+50][ix]=AnsiString(des);
        av->Cells[Col+50][ctask]=AnsiString(des);

       }

     else  // c'est un champ Normal
       {
        if (Col==3)  // duree
          {
           dur = atoi(des);
           if (dur <=0) strcpy(des,"1");
          }
        AdvStringGrid1->Cells[Col][ix]=AnsiString(des);
        av->Cells[Col][ctask]=AnsiString(des);
       }
     Change1();
     grefresh(ctask,'0');
    }
  }
 Remplir_AV1();
 Regen_Criteres();
 Ghost->ExtractValue(tmp,m_exchange,"decoch",0);
 if (strcmp(tmp,"O")==0) Tout_Decocher();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::ComplterDsignationTachesCoches1Click(
      TObject *Sender)
{
 Affect_Des();

}

void __fastcall TplStandard::Affect_Des()
{
 int ix,ctask; char src[100],dst[100];
 int cnt; char xtask[50];
 char tmp[200];

 TRemplTC *Remp;
 Remp= new TRemplTC(Application);
 Remp->Height=239;
 Remp->Width=340;
 Remp->ShowModal();
 delete Remp;

 // resultat dans m_exchange
 if (strlen(m_exchange)==0) return;
 Ghost->ExtractValue(src,m_exchange,"src",0);
 if (strlen(src)==0) return;
 Ghost->ExtractValue(dst,m_exchange,"dst",0);

 new_nb_taches=nb_taches;
 cnt=AdvStringGrid1->RowCount;

 for (ix=1;ix<cnt;ix++)
  {
   strcpy(xtask,AdvStringGrid1->Cells[1][ix].c_str());
   ctask = atoi(xtask);
   if (ctask!=0 && coche[ctask]=='X')
    {
     strcpy(tmp,AdvStringGrid1->Cells[2][ix].c_str());
     str_replace(tmp,src,dst);

     AdvStringGrid1->Cells[2][ix]=AnsiString(tmp);
     av->Cells[2][ctask]=AnsiString(tmp);
     Change1();
    }
  }

}

void __fastcall TplStandard::str_replace(char *result,char *src,char *dst)
{
 char *p;  int lensrc,lendst,lmove;
 char tmp[200];
 AnsiString sresult,ssrc,sdst,defin;

 strcpy(tmp,result);
 p=strstr(tmp,src);
 if (p==NULL) return;
 sresult=AnsiString(tmp);
 ssrc=AnsiString(src);
 sdst=AnsiString(dst);


 defin = StringReplace(sresult,ssrc,sdst,TReplaceFlags() << rfReplaceAll);
 strcpy(result,defin.c_str());


}



//---------------------------------------------------------------------------

void __fastcall TplStandard::LierTacheavecPrdcesseurs1Click(
      TObject *Sender)
{

 Affect_Pred();
}


void __fastcall TplStandard::Affect_Pred()
{

 int ix,indx,ctask;
 char tache[50],type[50],decal[50];
/*

 TAffPred *afp;
 afp = new TAffPred(Application);
 afp->ShowModal();
 delete afp;

 if (strlen(m_exchange)==0) return;


  Ghost->ExtractValue(tache,m_exchange,"tache",0);
  Ghost->ExtractValue(decal,m_exchange,"decal",0);
  Ghost->ExtractValue(type,m_exchange,"type",0);

  ctask = atoi(tache);
  if (ctask <1 || ctask > nb_taches)
    {
     Application->MessageBoxA("Numero de tache incorrect",m_ecoplan,MB_OK);
     return;
    }
 */


 ctask = current_task;
 new_nb_taches=nb_taches;
 for (ix=1;ix<=LASTTASK;ix++)
   {
     if (coche[ix]=='X')
         {
          if (ctask == ix)
            {
             Application->MessageBoxA("La tache en cours est un des pr�decesseurs",m_ecoplan,MB_OK);
             return;

            }
          else
            {
             InsertPredSeul(ix,ctask,0,0,false);
             InsertSuccSeul(ctask,ix,0,0,false);
            }
         }
   }
 Change1();
 current_task=1;
 grefresh(current_task,'1');

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::LiertacheavecSuccesseurs1Click(
      TObject *Sender)
{
 Affect_Succ();
}

void __fastcall TplStandard::Affect_Succ()
{
 int ix,indx,ctask;
 char tache[50],type[50],decal[50];

/*
TAffSucc *afs;
 afs = new TAffSucc(Application);
 afs->ShowModal();
 delete afs;

 if (strlen(m_exchange)==0) return;


  Ghost->ExtractValue(tache,m_exchange,"tache",0);
  Ghost->ExtractValue(decal,m_exchange,"decal",0);
  Ghost->ExtractValue(type,m_exchange,"type",0);

  ctask = atoi(tache);
  if (ctask <1 || ctask > nb_taches)
    {
     Application->MessageBoxA("Numero de tache incorrect",m_ecoplan,MB_OK);
     return;
    }

*/

ctask = current_task;
new_nb_taches=nb_taches;
for (ix=1;ix<=LASTTASK;ix++)
   {
     if (coche[ix]=='X')
         {
           if (ix == ctask)
             {
              Application->MessageBoxA("La tache en cours est un des successeurs Coch�",m_ecoplan,MB_OK);
              return;

             }
          else
            {
              InsertSuccSeul(ix,ctask,0,0,false);
              InsertPredSeul(ctask,ix,0,0,false);
            }

         }
   }
 Change1();
 current_task=1;
 grefresh(current_task,'1');

}
//---------------------------------------------------------------------------


char __fastcall TplStandard::Call_Dialog(int x,int y,int mode)
{
 TFDmarges *Marges;
 TCadre *Cadre;
 TEntete *Entete;
 TFDcal *Cal;
 TFDtaches *Taches;
 TFDcorps *Corps;
 TFDpied *Pied;
 TFDTcol *TtCol;
 TFDTligne *TtLigne;
 TDescEnvelop *Envelop;
 TColonnes *Col;
 TTri *Tri;

 int cnt,i; char dlg; char tmp[150];
 int xa,xb,ya,yb;

 dlg='0';

 cnt= asT->RowCount;
 for (i=2;i<cnt;i++)
   {
    strcpy(tmp,asT->Cells[1][i].c_str());  xa=atoi(tmp);
    strcpy(tmp,asT->Cells[3][i].c_str());  xb=atoi(tmp);
    if (x>=xa && x<=xb)
      {
       strcpy(tmp,asT->Cells[2][i].c_str());  ya=atoi(tmp);
       strcpy(tmp,asT->Cells[4][i].c_str());  yb=atoi(tmp);
       if (y>=ya && y<=yb)
          {
           strcpy(tmp,asT->Cells[18][i].c_str());
           dlg=tmp[0];
           break;
          }
      }
   }

  if (mode == 1) return dlg;

  switch (dlg)
     {
       case 'V' :

         strcpy(m_exchange,"<util>utiliser</util><tri>");
         strcat(m_exchange,P_TRI[pc]);
         strcat(m_exchange,"</tri>");
         strcpy(m_mode,"orgplanning");
         Tri = new TTri(Application);
         Tri->Height=619;
         Tri->Width=754;
         Tri->ShowModal();
         delete Tri;

         //Envelop = new TDescEnvelop(Application);
         //Envelop->ShowModal();
         //delete Envelop;

         break;

       case 'M':
         Marges = new TFDmarges(Application);
         Marges->ShowModal();
         delete Marges;
         break;

       case 'C':
         Cadre = new TCadre(Application);
         Cadre->ShowModal();
         delete Cadre;
         break;

       case 'E':
         Entete = new TEntete(Application);
         Entete->ShowModal();
         delete Entete;
         break;

       case 'H':
         Cal = new TFDcal(Application);
         Cal->ShowModal();
         delete Cal;
         break;

       case 'T':
         Taches = new TFDtaches(Application);
         Taches->ShowModal();
         delete Taches;
         break;

       case 'K':
         strcpy(m_exchange,"");
         TtCol = new TFDTcol(Application);
         TtCol->ShowModal();
         delete TtCol;
         break;

       case 'P':
         Pied = new TFDpied(Application);
         Pied->ShowModal();
         delete Pied;
         break;

       case 'L':
         strcpy(m_exchange,"");
         TtLigne = new TFDTligne(Application);
         TtLigne->ShowModal();
         delete TtLigne;
         break;

       case 'Q':

         strcpy(m_exchange,"<col>"); strcat(m_exchange,P_COLONNES[pc]); strcat(m_exchange,"</col>");
         strcat(m_exchange,"<util>utiliser</util>");
         strcpy(m_mode,"orgplanning");
         Col = new TColonnes(Application);
         //Col->Height=705;
         //Col->Width=778;
         Col->ShowModal();
         delete Col;

         break;

       case 'Y': // ne rien faire
         break;

       case 'S':  // Tri
         strcpy(m_exchange,"<util>utiliser</util><tri>");
         strcat(m_exchange,P_TRI[pc]);
         strcat(m_exchange,"</tri>");
         strcpy(m_mode,"orgplanning");
         Tri = new TTri(Application);
         Tri->Height=619;
         Tri->Width=754;
         Tri->ShowModal();
         delete Tri;

         break;

       default:
         Corps = new TFDcorps(Application);
         Corps->ShowModal();
         delete Corps;
         break;

    }
 return dlg;

}
void __fastcall TplStandard::btP1PDFClick(TObject *Sender)
{
 CoefFormat = ComputeFormat(edAjust);
 // SelectPrinter(1,2,imP1p,imP1l);
 Prepare_Draw_Planning();
 SelectPrinter(1,2,imP1p,imP1l);
}

void __fastcall TplStandard::ImprimeDirect(int dual,TImage *IM1, TImage *IM2)
{

/*
 if (dual==2)
     {
     if (P_MODE[pc]=='L')  Print_Image(IM1); else Print_Image(IM2);
     //  if (P_MODE[pc]=='L')  Print_Image(IM2); else Print_Image(IM1);

     }
   else  Print_Image(IM1);
*/

 Print_Bitmap(imP);

 Application->MessageBoxA("Impression en Cours",m_ecoplan,MB_OK);
}


void __fastcall TplStandard::SelectPrinter(int modepdf,int dual,TImage *IM1, TImage *IM2)
{
HGLOBAL hMem;
LPDEVMODE lpdm;
AnsiString PrName;
char device[MAX_PATH], driver[MAX_PATH], port[MAX_PATH];

int j; int prnum;
prnum=0;

TPrinter *pt = Printer();
prnum = 0;
if (modepdf == 1)
  { for (j=0; j<=pt->Printers->Count-1; j++)
    {
      PrName = pt->Printers->Strings[j];
      if ((pt->Printers->Strings[j]) == "CutePDF Writer") prnum=j;
      // else if ((pt->Printers->Strings[j]) == "PDFCreator") prnum=j;
    }
  }
 else
   {
    for (j=0; j<=pt->Printers->Count-1; j++)
    {
      if ((pt->Printers->Strings[j]) != "CutePDF Writer")
      //  && (pt->Printers->Strings[j]) != "PDFCreator")
      prnum=j;
    }

   }
 pt->PrinterIndex = prnum;
 pt->GetPrinter(device, driver, port,
 reinterpret_cast<THandle>(hMem));
 pt->SetPrinter(device, driver, port, NULL);


if (PrintDialog1->Execute())
  {
   /*
   if (dual==2)
     {

      if (P_MODE[pc]=='L') Print_Image(IM1); else Print_Image(IM2);

      // if (P_MODE[pc]=='L') Print_Image(IM2); else Print_Image(IM1);

     }
   else  Print_Image(IM1);
   */

   Print_Bitmap(imP);
  }



}


void __fastcall TplStandard::SelectPrinterAA(int modepdf,int dual,TImage *IM1, TImage *IM2)
{
HGLOBAL hMem;
LPDEVMODE lpdm;
AnsiString PrName;
char device[MAX_PATH], driver[MAX_PATH], port[MAX_PATH];

int j; int prnum;
prnum=0;

TPrinter *pt = Printer();
prnum = 0;
if (modepdf == 1)
  { for (j=0; j<=pt->Printers->Count-1; j++)
    {
      PrName = pt->Printers->Strings[j];
      if ((pt->Printers->Strings[j]) == "CutePDF Writer") prnum=j;
      // else if ((pt->Printers->Strings[j]) == "PDFCreator") prnum=j;
    }
  }
 else
   {
    for (j=0; j<=pt->Printers->Count-1; j++)
    {
      if ((pt->Printers->Strings[j]) != "CutePDF Writer")
      //  && (pt->Printers->Strings[j]) != "PDFCreator")
      prnum=j;
    }

   }
 pt->PrinterIndex = prnum;
 pt->GetPrinter(device, driver, port,
 reinterpret_cast<THandle>(hMem));
 pt->SetPrinter(device, driver, port, NULL);


if (PrintDialog1->Execute())
  {
   /*
   if (dual==2)
     {

      if (P_MODE[pc]=='L') Print_Image(IM1); else Print_Image(IM2);

      // if (P_MODE[pc]=='L') Print_Image(IM2); else Print_Image(IM1);

     }
   else  Print_Image(IM1);
   */

   //Print_Image(imPort);
   Print_Bitmap(im);
  }



}


/*
/////////////////////////////////////////////////
Can I use PDF Creator Pilot from C++ Builder?

Yes, you can use PDF library from C++ Builder as well.

You should import Type Library as in example for Delphi
(http://www.colorpilot.com/pdfsample_delphi.html) in steps 2 and 3.

Then create a new application, go to Standard Control Panel and place
button on the Form1.

Then go to ActiveX components palette and select piPDFDocument
component and place it on Form1 too.

Then click Button1 on Form1 and add this code to "Button1Click"
function:

// initialize PDF Engine
piPDFDocument1->StartEngine("demo@demo", "demo");
// set PDF output filename
piPDFDocument1->FileName = "HelloPDF_DELPHI.pdf";
// set option to auto-open generated pdf document
piPDFDocument1->AutoLaunch = True;
// start document generation
piPDFDocument1->BeginDoc();
// draw 'HELLO, PDF' message on the current PDF page
piPDFDocument1->PDFPAGE_BeginText();
piPDFDocument1->PDFPAGE_SetActiveFont("Verdana", True, False, False, False, 14,
piPDFDocument1->charsetANSI_CHARSET);
piPDFDocument1->PDFPAGE_TextOut(10, 20, 0, "HELLO, PDF!");
piPDFDocument1->PDFPAGE_EndText();
// finalize document generation
piPDFDocument1->EndDoc();
*/

//---------------------------------------------------------------------------

void __fastcall TplStandard::btPrepR1Click(TObject *Sender)
{

 TPrepRapport *PrepRapport;
 pc=4;
 PrepRapport = new TPrepRapport(Application);
 PrepRapport->Height = 577;
 PrepRapport->Width=679;
 PrepRapport->ShowModal();
 delete PrepRapport;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet8Show(TObject *Sender)
{
 char tmp[250];
 if (strlen(P_NOM[1])==0)
   {
    Clear_Diagram(imP1p);
    Clear_Diagram(imP1l);
    Clear_Diagram(imP1Screen);
    return;
   }
 pc=1;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet10Show(TObject *Sender)
{
char tmp[250];
 if (strlen(P_NOM[2])==0)
   {
    Clear_Diagram(imP2p);
    Clear_Diagram(imP2l);
    Clear_Diagram(imP2Screen);
   return;
   }
 pc=2;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);

 Exec_Planning(tmp,true);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet12Show(TObject *Sender)
{
 char tmp[250];
 if (strlen(P_NOM[3])==0)
  {
    Clear_Diagram(imP3p);
   //Clear_Diagram(imP3l);
    Clear_Diagram(imP3Screen);
   return;
  }
 pc=3;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);

 Exec_Planning(tmp,true);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet11Show(TObject *Sender)
{
char tmp[250];
strcpy(P_NOM[3],"Avancementt");
  if (strlen(P_NOM[3])==0)
   {
    Clear_Diagram(imP3p);
    Clear_Diagram(imP3Screen);
    return;
   }

 // Save_Actualisation(false);
 // Load_Avanc(FileNameAvanc,false);


 Remplir_asTri();

 Graph_Avanc(asAvanc->RowCount-1,MAXDIST);

 pc=3;

 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);



}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet4Show(TObject *Sender)
{
 char tmp[250];
 pc=4;
 if (strlen(P_NOM[4])==0)
   {
    Clear_Diagram(imR1p);
    Clear_Diagram(imR1Screen);
    return;
   }
 pc=4;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);

 Exec_Rapport(tmp,true);
 }
//---------------------------------------------------------------------------

void __fastcall TplStandard::btRetPlanning2Click(TObject *Sender)
{

 char x_ghost_prep[500];

 pc=2;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning2</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);

TPrepPlanning *PrepPlanning;
 pc=2;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 //delete PrepPlanning;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btPrP2Click(TObject *Sender)
{

// SelectPrinter(0,2,imP2p,imP2l);
ComputeFormat(edAjust2);
// ImprimeDirect(2,imP2p,imP2l);
// Remettre taille du graphique � Normal
Prepare_Draw_Planning();
ImprimeDirect(2,imP2p,imP2l);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btRetPlanning3Click(TObject *Sender)
{
 char x_ghost_prep[500];

 pc=3;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning3</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);

TPrepPlanning *PrepPlanning;
 pc=3;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();  //Modal();
 delete PrepPlanning;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::imP2pMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 pc=2;
 //TPrinter *pt = Printer();
 // pt->Orientation = poPortrait;
 Label115->Caption = "Mode Portrait"; 
if (P_MODE[pc]=='L') { P_MODE[pc]='P'; RAZ(); Prepare_Draw_Planning(); }
 ZoomCopy2(imP2p,X,Y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TplStandard::imP2lMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 //TPrinter *pt = Printer();
 //pt->Orientation = poLandscape;
 Label115->Caption = "Mode Paysage";
 if (P_MODE[pc]=='P') { P_MODE[pc]='L'; RAZ(); Prepare_Draw_Planning(); }
 ZoomCopy2(imP2l,X,Y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TplStandard::imR1pMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 //TPrinter *pt = Printer();
 //pt->Orientation = poPortrait;
 if (P_MODE[pc]=='L') { P_MODE[pc]='P'; RAZ(); Prepare_Draw_Rapport(PAGE_RAPPORT[pc]); }
 ZoomCopy4(imR1p,X,Y);
}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::imR2pMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 //TPrinter *pt = Printer();
 //pt->Orientation = poPortrait;
if (P_MODE[pc]=='L') { P_MODE[pc]='P'; RAZ(); Prepare_Draw_Rapport(PAGE_RAPPORT[pc]); }
 ZoomCopy5(imR2p,X,Y);
}
*/
//---------------------------------------------------------------------------

void __fastcall TplStandard::btPrepR2Click(TObject *Sender)
{
 TPrepRapport *PrepRapport;
 pc=5;
 PrepRapport = new TPrepRapport(Application);
 PrepRapport->Height = 547;
 PrepRapport->Width=679;
 PrepRapport->ShowModal(); //Modal();
 delete PrepRapport;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::cbEchelleChange(TObject *Sender)
{
 int x1,x2;
 x2=SB1->Position - 50;
 //$$$$x1=int (ScrollBar1->Position*nb_taches/100)+1;

 DrawMiniGantt(1,0,immGantt,false);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::ZoomR1Change(TObject *Sender)
{
 ZoomCopy4(imR1p,0,0);
}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::ZoomR2Change(TObject *Sender)
{
 ZoomCopy5(imR2p,0,0);
}
*/
//---------------------------------------------------------------------------

void __fastcall TplStandard::ZoomP2Change(TObject *Sender)
{
 ZoomCopy2(imP2p,0,0);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::ZoomP3Change(TObject *Sender)
{
 ZoomCopy3(imP3p,0,0);
}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::arCanEditCell(TObject *Sender, int ARow,
      int ACol, bool &CanEdit)
{

 CanEdit = false;
 if (ARow ==0) return;
 if (ACol>0) return;

 }
*/

void __fastcall TplStandard::Trace_Page_Rapport(int Page_Rapport, TImage *imRapport)
{

  Trace_Col_Rapport(Page_Rapport);
  TRect Diagram = Rect(0,0, imP->Width,imP->Height);
  imP->Canvas->Brush->Color = clWhite; // BgColor;
  imP->Canvas->FillRect(Diagram);
  Clear_Diagram(imRapport);
  Draw_Rapport(imP,Page_Rapport);
  iPort->Picture->Assign(imP);
  if (pc==4)  ZoomCopy4(imRapport,0,0);
  // if (pc==5)  ZoomCopy5(imRapport,0,0);

}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::arMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{

if (Column==0)
     { SG1Row=Row; SG1Col=0;
      if (coche[numt]=='X') { coche[numt]=' '; AdvStringGrid1->Cells[0][Row] = ' '; }
       else { coche[numt]='X';    AdvStringGrid1->Cells[0][Row] = AnsiString(strcoche); }

}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::arMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 int Column, Row, numt,cnt;
  // char *p,tmp[250];


  ar->MouseToCell(X, Y, Column, Row);
   if (Row < 1) return;

  if (Column==0)
     {
      if (ar->Cells[0][Row] == "")
        ar->Cells[0][Row] = AnsiString(strcoche);
        else ar->Cells[0][Row] = "";
      Compute_Marqueur();
     }
}

//---------------------------------------------------------------------------

void __fastcall TplStandard::Button11Click(TObject *Sender)
{
 int i;
 int cnt;

 cnt = ar->RowCount;
 for (i=1;i<cnt;i++)  ar->Cells[0][i]="";
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button12Click(TObject *Sender)
{
 DefaultPage();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::DefaultPage()
{
int i,y,h;
 int cnt,page,mempage;
 cnt = ar->RowCount;
 for (i=1;i<cnt;i++)  ar->Cells[0][i]="";

 h= P_H[pc];
 mempage=1;

 for (i=2;i<cnt;i++)
   {
    y=atoi(ar->Cells[3][i].c_str());
    y=y+P_MARGH2[pc];
    page= (y/h) + 1;
    if (mempage != page) ar->Cells[0][i-1] = AnsiString(strcoche);
    mempage=page;
   }
Compute_Marqueur();
}
*/

void __fastcall TplStandard::b1PrevClick(TObject *Sender)
{
 char tmp[100];
 PAGE_RAPPORT[pc]--;
 if (PAGE_RAPPORT[pc] < 1) PAGE_RAPPORT[pc]=1;
  Prepare_Draw_Rapport(PAGE_RAPPORT[pc]);  //Trace_Page_Rapport(PAGE_RAPPORT[pc],imR1p);
   sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
  Label76->Caption = AnsiString(tmp);


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::b1NextClick(TObject *Sender)
{
 char tmp[100];

  PAGE_RAPPORT[pc]++;
  if (PAGE_RAPPORT[pc] > MAX_PAGE_RAPPORT[pc]) PAGE_RAPPORT[pc]=MAX_PAGE_RAPPORT[pc];
  Prepare_Draw_Rapport(PAGE_RAPPORT[pc]);
  //Trace_Page_Rapport(PAGE_RAPPORT[pc],imR1p);
  sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
  Label76->Caption = AnsiString(tmp);

}

void __fastcall TplStandard::b2PrevClick(TObject *Sender)
{
 char tmp[100];
 PAGE_RAPPORT[pc]--;
 if (PAGE_RAPPORT[pc] < 1) PAGE_RAPPORT[pc]=1;
 Prepare_Draw_Rapport(PAGE_RAPPORT[pc]);
 //Trace_Page_Rapport(PAGE_RAPPORT[pc],imR2p);
  sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
  //Label84->Caption = AnsiString(tmp);


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::b2NextClick(TObject *Sender)
{
 char tmp[100];

 PAGE_RAPPORT[pc]++;
 if (PAGE_RAPPORT[pc] > MAX_PAGE_RAPPORT[pc]) PAGE_RAPPORT[pc]=MAX_PAGE_RAPPORT[pc];
 Prepare_Draw_Rapport(PAGE_RAPPORT[pc]);
 // Trace_Page_Rapport(PAGE_RAPPORT[pc],imR2p);
 sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid2ClickCell(TObject *Sender,
      int ARow, int ACol)
{
 char tmp[100]; int xrow;
 if (ARow>0 && ACol==6)  MemoDecal2=AdvStringGrid2->Cells[ACol][ARow];
 if (ARow>0 && ACol==5)  MemoTypl2=AdvStringGrid2->Cells[ACol][ARow];
 if (ARow > 0 && ACol==0)
   {

     strcpy(tmp,AdvStringGrid2->Cells[1][ARow].c_str());
    xrow = ChercheRow(AnsiString(tmp));
    if (xrow !=0)
      {

        //AdvStringGrid1->ClearRowSelect();
        //AdvStringGrid1->SelectRows(xrow,1);
        AdvStringGrid1->Row = xrow;
        current_task=atoi(tmp); //ARow;
        grefresh(current_task,'0');

      }

   }

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid3ClickCell(TObject *Sender,
      int ARow, int ACol)
{
 char tmp[150]; int xrow;
 if (ARow>0 && ACol==6)  MemoDecal3=AdvStringGrid3->Cells[ACol][ARow];
 if (ARow>0 && ACol==5)  MemoTypl3=AdvStringGrid3->Cells[ACol][ARow];
 if (ARow > 0 && ACol==0)
   {

    strcpy(tmp,AdvStringGrid3->Cells[1][ARow].c_str());
    //AdvStringGrid1->SetFocus();  // AdvStringGrid1ClickCell(Sender,1,1);

    xrow = ChercheRow(AnsiString(tmp));

    if (xrow !=0)
      {

        //AdvStringGrid1->ClearRowSelect();
        //AdvStringGrid1->SelectRows(xrow,1);
        AdvStringGrid1->Row = xrow;
        current_task=atoi(tmp); //ARow;
        grefresh(current_task,'0');

      }

   }
}
//---------------------------------------------------------------------------

int __fastcall TplStandard::ChercheRow(AnsiString numtache)
{
 int i,cnt,res;
 cnt = AdvStringGrid1->RowCount;

 res = 0;
 for (i=1;i<cnt;i++)
   {
    if (AdvStringGrid1->Cells[1][i] == numtache)
      {
       res = i;
       break;
      }
   }
 return res;

}

void __fastcall TplStandard::asCritCanEditCell(TObject *Sender, int ARow,
      int ACol, bool &CanEdit)
{
 CanEdit=false;
 if (ARow < 1) return;
 if (ACol < 3) return;
 CanEdit=true;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet5Show(TObject *Sender)
{
strcpy(m_menu,"ENACoches2");
// asCrit->ColWidths[9]=0;
// asCrit->ColWidths[10]=0;
asCrit->ColWidths[11]=0;
asCrit->ColWidths[12]=0;
asCrit->ColWidths[13]=0;
asCrit->ColWidths[14]=0;
asCrit->ColWidths[15]=0;
asCrit->ColWidths[16]=0;
asCrit->ColWidths[17]=0;
asCrit->ColWidths[18]=0;

asCrit->ColWidths[19]=0;
asCrit->ColWidths[20]=0;


/*
// Load_Grille_Criteres();
Regen_Criteres();
CopyCriteres();
*/

//  Exec filter


// Libelle_Taches();
}

void __fastcall TplStandard::Libelle_Taches()
{
int cnt,i,j,ind,numt;
char tmp[150];
AnsiString crt;

cnt = asCrit->RowCount;
for (i=1;i<cnt;i++)
  {
   strcpy(tmp,asCrit->Cells[1][i].c_str());
   numt = atoi(tmp);
   if (numt>0 && numt<=LASTTASK)
     {
       asCrit->Cells[2][i] = av->Cells[2][numt];
       for (j=1;j<9;j++)  // on recopie les 8 Criteres
         {
           ind = j*2 + 1;
           asCrit->Cells[ind][i] = av->Cells[j+40][numt];  // asCrit->Cells[ind][i];
           asCrit->Colors[ind][i] = TColor(RGB(255,255,153));
           ind++;
           asCrit->Cells[ind][i] = av->Cells[j+50][numt];


         }

     }
  }
}

//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet5Hide(TObject *Sender)
{
strcpy(m_menu,"ENACoches1");        
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

void __fastcall TplStandard::btPrR1Click(TObject *Sender)
{
 int i;
 char tmp[100];

 for (i=1;i<=MAX_PAGE_RAPPORT[pc];i++)
    {
     PAGE_RAPPORT[pc]=i;
     Trace_Page_Rapport(PAGE_RAPPORT[pc],imR1p);
     sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
     Label76->Caption = AnsiString(tmp);
     Print_Image(imR1p);
    }
}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::btPrR2Click(TObject *Sender)
{

  int i;
 char tmp[100];

 for (i=1;i<=MAX_PAGE_RAPPORT[pc];i++)
    {
     PAGE_RAPPORT[pc]=i;
     Trace_Page_Rapport(PAGE_RAPPORT[pc],imR2p);
     sprintf(tmp,"%d/%d",PAGE_RAPPORT[pc],MAX_PAGE_RAPPORT[pc]);
     Label84->Caption = AnsiString(tmp);
     Print_Image(imR2p);
    }

}
*/
//---------------------------------------------------------------------------


void __fastcall TplStandard::imR1ScreenMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  int rx,ry;
  //int XX,YY;
  char tmp[200];
   pc=4;

 if (Shift.Contains(ssLeft))
  {

  rx= (LAST_RX2[pc]-LAST_RX1[pc])*X/imR1Screen->Width;
  rx = rx + LAST_RX1[pc];
  ry= (LAST_RY2[pc]-LAST_RY1[pc])*Y/imR1Screen->Height;
  ry = ry + LAST_RY1[pc];
  //  rx,ry=coordonnees dans la bitmap IMP
  sprintf (tmp,"rx=%d ry=%d",rx,ry);
  //lxy->Caption = AnsiString(tmp); //Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
  Call_Dialog(rx,ry,0);
  PAGE_RAPPORT[pc]=1;
  RAZ(); Prepare_Draw_Rapport(PAGE_RAPPORT[pc]);
  //XX= rx*imR1p->Width/imP->Width;
  //YY= ry*imR1p->Height/imP->Height;

  ZoomCopy4(imR1p,0,0);  //XX,YY);
  }

 else
  {
   Application->MessageBoxA("Temporaire : Aide Associ�e",m_ecoplan,MB_OK);
  }

}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::imR2ScreenMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  int rx,ry;
  //int XX,YY;
  char tmp[200];
   pc=5;

 if (Shift.Contains(ssLeft))
  {
  rx= (LAST_RX2[pc]-LAST_RX1[pc])*X/imR2Screen->Width;
  rx = rx + LAST_RX1[pc];
  ry= (LAST_RY2[pc]-LAST_RY1[pc])*Y/imR2Screen->Height;
  ry = ry + LAST_RY1[pc];
  //  rx,ry=coordonnees dans la bitmap IMP
  sprintf (tmp,"rx=%d ry=%d",rx,ry);
  //lxy->Caption = AnsiString(tmp); //Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
  Call_Dialog(rx,ry,0);
  PAGE_RAPPORT[pc]=1;
  RAZ(); Prepare_Draw_Rapport(PAGE_RAPPORT[pc]);
  //XX= rx*imR2p->Width/imP->Width;
  //YY= ry*imR2p->Height/imP->Height;
  ZoomCopy5(imR2p,0,0);  //XX,YY);
  }

 else
  {
   Application->MessageBoxA("Temporaire : Aide Associ�e",m_ecoplan,MB_OK);
  }

}
*/
//---------------------------------------------------------------------------


void __fastcall TplStandard::imP2ScreenMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  int rx,ry;
  //int XX,YY;
  char tmp[200];
   pc=2;

 rx= (LAST_RX2[pc]-LAST_RX1[pc])*X/imP2Screen->Width;
  rx = rx + LAST_RX1[pc];
  ry= (LAST_RY2[pc]-LAST_RY1[pc])*Y/imP2Screen->Height;
  ry = ry + LAST_RY1[pc];
 if (Shift.Contains(ssLeft))
  {

  //  rx,ry=coordonnees dans la bitmap IMP
  sprintf (tmp,"rx=%d ry=%d",rx,ry);
  //lxy->Caption = AnsiString(tmp); //Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
  Call_Dialog(rx,ry,0);
  RAZ(); Prepare_Draw_Planning();
  //XX= rx*imP2p->Width/imP->Width;
  //YY= ry*imP2p->Height/imP->Height;

  ZoomCopy2(imP2p,0,0);  //XX,YY);
  }

 else  Display_Aide(rx,ry);

}
//---------------------------------------------------------------------------



/*
void __fastcall TplStandard::OnResize(TObject *Sender)
{
 Resize(1024,768);

}
*/

void __fastcall TplStandard::EcoResize(int w,int h)
{
 int HNorm,WNorm;
 int pcw,pch;
 HNorm = 768; WNorm = 1024;
 PageControl1->Width = w*(969-0)/WNorm;
 PageControl1->Height = h*640/ HNorm; //  ;
 pcw = PageControl1->Width;
 pch = pl1->Height;
 imGantt->Width = pcw - 20 - imGantt->Left;
 imGantt->Height = pch - 20 - imGantt->Top;
 immGantt->Width = pcw - 20 - immGantt->Left;
 immGantt->Height = pch - 20 - immGantt->Top;
 imP1Screen->Width = pcw - 20 - imP1Screen->Left;
 imP1Screen->Height = pch - 20 - imP1Screen->Top;
 imP2Screen->Width = pcw - 20 - imP2Screen->Left;
 imP2Screen->Height = pch - 20 - imP2Screen->Top;
 imP3Screen->Width = pcw - 20 - imP3Screen->Left;
 imP3Screen->Height = pch - 20 - imP3Screen->Top;
 AdvStringGrid1->Width = pcw-20-AdvStringGrid1->Left;

}


//---------------------------------------------------------------------------

void __fastcall TplStandard::FormShow(TObject *Sender)
{
 
  TabSheet3->TabVisible = false;

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet1Show(TObject *Sender)
{
char tmp[250];
// if (strlen(P_NOM[0])==0) return;
 pc=0;

 Label46->Caption = Label15->Caption;
 Label48->Caption = Label17->Caption;

 //strcpy(P_NOM[0],"Defaut");
 // strcpy(P_NOM[0],"");
 if (strlen(P_NOM[0])==0)
   {
    Clear_Diagram(imP0p);
    // Clear_Diagram(imP0l);
    Clear_Diagram(imGantt);
    return;
   }
 pc=0;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);
 /*
 //if (strcmp(P_NOM[0],"Defaut")==0) strcpy(tmp,"");
 //else
 // {
   sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
   P_TRI[1],P_COLONNES[1],P_FILTRE[1]);
 // }

//  Exec_Planning(tmp,true);
*/
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::ZoomP0Change(TObject *Sender)
{

 ZoomCopy0(imP0p,0,0);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::imP0pMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 pc=0;
 TPrinter *pt = Printer();
 pt->Orientation = poPortrait;
 P_MODE[pc]='P';
 //RAZ(); Prepare_Draw_Planning();

 ZoomCopy0(imP0p,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btP2PDFClick(TObject *Sender)

{
 CoefFormat = ComputeFormat(edAjust2);
 //SelectPrinter(1,2,imP2p,imP2l);
 Prepare_Draw_Planning();
 SelectPrinter(1,2,imP2p,imP2l);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btP3PDFClick(TObject *Sender)
{
 SelectPrinter(1,1,imP3p,NULL);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btPrP3Click(TObject *Sender)
{
 SelectPrinter(0,1,imP3p,NULL);
 ImprimeDirect(1,imP3p,NULL);
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::btImpGanttClick(TObject *Sender)
{

CoefFormat=ComputeFormat(edAjust);
Prepare_Draw_Planning();

 ImprimeDirect(1,imP0p,imP0p);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btEchelleClick(TObject *Sender)
{

TFDDistH *Haut;
Haut = new TFDDistH(Application);
Haut->ShowModal();
delete Haut;
m_automatic[0] = ' ';
RAZ(); Prepare_Draw_Planning();

  ZoomCopy0(imP0p,0,0);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::asCritSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
  DispCritere(ARow,ACol);
}
//---------------------------------------------------------------------------


bool __fastcall TplStandard::Check_Automatic()
{
 bool res;
 res=false;
 if (m_automatic[pc]=='X') res=true;
 return res;
}



void __fastcall TplStandard::btMemor1Click(TObject *Sender)
{
 int a;
 char x_ghost_prep[500];

 /*
 pc=1;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
TPrepPlanning *PrepPlanning;
 pc=1;
 strcpy(m_memoriser,"memo");
 PrepPlanning = new TPrepPlanning(Application);
 PrepPlanning->Height = 669;
 PrepPlanning->Width=742;
 PrepPlanning->Show();  //Modal();
 //delete PrepPlanning;
 */



 pc=1;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
TPrepPlanning *PrepPlanning;
 pc=1;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 delete PrepPlanning;
 a=0;


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btmemor2Click(TObject *Sender)
{
int a;
 char x_ghost_prep[500];

 pc=2;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
TPrepPlanning *PrepPlanning;
 pc=2;
 strcpy(m_memoriser,"memo");
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();  //Modal();
 delete PrepPlanning;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btMemor3Click(TObject *Sender)
{
int a;
 char x_ghost_prep[500];

 pc=3;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
TPrepPlanning *PrepPlanning;
 pc=3;
 strcpy(m_memoriser,"memo");
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 delete PrepPlanning;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btMemor4Click(TObject *Sender)
{
 strcpy(m_memoriser,"memo");
TPrepRapport *PrepRapport;
 pc=4;
 PrepRapport = new TPrepRapport(Application);
 PrepRapport->Height = 577;
 PrepRapport->Width=679;
 PrepRapport->ShowModal();  delete PrepRapport;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btMemor5Click(TObject *Sender)
{
strcpy(m_memoriser,"memo");
TPrepRapport *PrepRapport;
 pc=5;
 PrepRapport = new TPrepRapport(Application);
 PrepRapport->Height = 577;
 PrepRapport->Width=679;
 PrepRapport->ShowModal(); delete PrepRapport;
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::btMiseenPage1Click(TObject *Sender)
{
 TMiseEnPage  *Mep;
 Mep = new TMiseEnPage(Application);
 pc=4;
 Mep->ShowModal(); delete Mep;

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btMiseEnPage2Click(TObject *Sender)
{
TMiseEnPage  *Mep;
 Mep = new TMiseEnPage(Application);
 pc=5;
 Mep->ShowModal(); delete Mep;
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::AdvStringGrid1GetAlignment(TObject *Sender,
      int ARow, int ACol, TAlignment &HAlign, TVAlignment &VAlign)
{
 if ((ACol >= 4 && ACol <=7) || (ACol==10) || (ACol==12) || ACol==13) HAlign = taCenter;
 if (ACol==1 || ACol==3 || ACol==8) HAlign=taRightJustify;
 }
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button11Click(TObject *Sender)
{
 int i;
 for (i=1;i<=av->RowCount;i++)
   {
    av->Cells[12][i]="";
    av->Cells[13][i]="";
   }
for (i=1;i<=AdvStringGrid1->RowCount;i++)
   {
    AdvStringGrid1->Cells[12][i]="";
    AdvStringGrid1->Cells[13][i]="";
   }

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::Button13Click(TObject *Sender)
{
int a;
 char x_ghost_prep[500];

 pc=0;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
 TPrepPlanning *PrepPlanning;
 pc=0;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 delete PrepPlanning;
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::btAideSClick(TObject *Sender)
{

 Thelp *Aide;
 strcpy(m_help,"CreatModif.html#Cre1");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;
 
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::imGanttMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
TRect Dst,Src;
int imx; int imy;
int ext,rx1,rx2,ry1,ry2;

if (!imP) return;
/*
imx=imP->Width;
imy=imP->Height;

///   imPort is assigned to Image im !!!!!

ext = (11-ZoomP0->Position)*200;
rx1= (imx * X / imP0p->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }

ry1= (imy * Y / imP0p->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }


Dst = Rect(0,0,imGantt->Width,imGantt->Height);
Src = Rect(rx1,ry1,rx2,ry2);

imGantt->Canvas->CopyRect(Dst,imP0p->Canvas,Src);
*/
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::imGanttMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
TRect Dst,Src;
int imx; int imy;
int ext,rx1,rx2,ry1,ry2;

if (!imP) return;

return;

/*
imx=imP->Width;
imy=imP->Height;

///   imPort is assigned to Image im !!!!!

ext = (11-ZoomP0->Position)*100;
rx1= (imx * X / imP0p->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > imP->Width) {rx2=imP->Width; rx1=rx2-2*ext; }

ry1= (imy * Y / imP0p->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > imP->Height) {ry2=imP->Height; ry1=ry2-2*ext; }


Dst = Rect(0,0,imGantt->Width,imGantt->Height);
Src = Rect(rx1,ry1,rx2,ry2);

imGantt->Canvas->CopyRect(Dst,imP0p->Canvas,Src);
*/
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Label71DblClick(TObject *Sender)
{
  char x_ghost_prep[500];

 pc=0;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
TPrepPlanning *PrepPlanning;
 pc=0;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 delete PrepPlanning;

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::btAideGanttClick(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"CreatModif.html#Cre6");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btAideCritClick(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"AffectCriteres.html#");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button10Click(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"Plannings.html#Pl2");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btPL2HelpClick(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"Plannings.html#Pl2");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;        
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btPL3HelpClick(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"Plannings.html#Pl2");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;        
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button14Click(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"Couts.html#CT0");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;
}
//---------------------------------------------------------------------------



void __fastcall TplStandard::CocherToutesLesTaches2Click(TObject *Sender)
{
 strcpy(m_coche,"<21>");
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::DcocherToutesLestaches2Click(TObject *Sender)
{
strcpy(m_coche,"<22>");
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::CocherDcocher1Click(TObject *Sender)
{
strcpy(m_coche,"<23>");        
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AffecterCritre1Click(TObject *Sender)
{
 TCriteres *Criteres;
 Criteres = new TCriteres(Application);
 Criteres->Height=652;
 Criteres->Width=882;
 Criteres->ShowModal();
 delete Criteres;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button5Click(TObject *Sender)
{
 //  Ouverture d'un fichier pour exportation au format CSV

FILE *fp;
int i,count; char tmp[250];
char record[200];

strcpy(tmp,LocalBase);  // m_directory);
if  (RadioButton1->Checked == true) strcat(tmp,"_ech_jour");
if  (RadioButton2->Checked == true) strcat(tmp,"_ech_semaine");
if  (RadioButton3->Checked == true) strcat(tmp,"_ech_mois");
if (cbPos->Checked == true) strcat(tmp,"_pos"); else strcat(tmp,"_neg");
strcat(tmp,".csv");
count=ave->RowCount;
fp = fopen(tmp,"wt");


for (i=1;i<count;i++)
{
 strcpy(record,"");
 strcat(record,"\""); strcat(record,ave->Cells[1][i].c_str()); strcat(record,"\";");
 strcat(record,"\""); strcat(record,ave->Cells[2][i].c_str()); strcat(record,"\";");
 strcat(record,"\""); strcat(record,ave->Cells[3][i].c_str()); strcat(record,"\";");
 strcat(record,"\""); strcat(record,ave->Cells[4][i].c_str()); strcat(record,"\";\n");
fputs(record,fp);
}
fclose(fp);
strcpy(record,"Fichier "); strcat(record,tmp); strcat(record," Sauvegard�");
Application->MessageBoxA(record,m_ecoplan,MB_OK);

}
//---------------------------------------------------------------------------





void __fastcall TplStandard::RadioButton3ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 //TabSheet7->TabVisible = true;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button12Click(TObject *Sender)
{
 // TabSheet7->TabVisible = false;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet9Show(TObject *Sender)
{
 // Rafraichir en fonction des couts dans les colonnes
Cout_Ressources();



}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::Refresh_Couts()
{
int cnt,i,j,ind,numt;
char tmp[150];
AnsiString crt;

cnt = avc->RowCount;
for (i=1;i<cnt;i++)
  {
   strcpy(tmp,avc->Cells[1][i].c_str());
   numt = atoi(tmp);
   if (numt>0 && numt<=LASTTASK)
     {
       avc->Cells[2][i] = av->Cells[2][numt];
       avc->Cells[3][i] = av->Cells[3][numt];
       avc->Cells[4][i] = av->Cells[4][numt];
       avc->Cells[5][i] = av->Cells[5][numt];
       avc->Cells[12][i] = av->Cells[18][numt];
       avc->Cells[13][i] = av->Cells[19][numt];
       avc->Cells[8][i] = av->Cells[41][numt];
       avc->Cells[9][i] = av->Cells[42][numt];
       avc->Cells[10][i] = av->Cells[43][numt];



       for (j=1;j<9;j++)  // on recopie les 8 Criteres
         {
           ind = j*2 + 1;
           asCrit->Cells[ind][i] = av->Cells[j+40][numt];  // asCrit->Cells[ind][i];
           asCrit->Colors[ind][i] = TColor(RGB(255,255,153));
           ind++;
           asCrit->Cells[ind][i] = av->Cells[j+50][numt];


         }

     }
  }
 Calcul_Echeancier();
}
*/


void __fastcall TplStandard::Refresh_Couts()
{
int cnt,i,j,ind,ix,numt;
char tmp[150];
AnsiString crt;
int cntech;


Clean_Grid(avc,2);
ix=0;

cnt = av->RowCount;
for (i=1;i<cnt;i++)
  {
   strcpy(tmp,av->Cells[1][i].c_str());
   numt = atoi(tmp);
   if (numt>0 && numt<=LASTTASK)
     { ix++; if (ix>1) avc->RowCount++;
       avc->Cells[1][ix] = av->Cells[1][i];
       avc->Cells[2][ix] = av->Cells[2][i];
       avc->Cells[3][ix] = av->Cells[3][i];
       avc->Cells[4][ix] = av->Cells[4][i];
       avc->Cells[5][ix] = av->Cells[5][i];
       avc->Cells[12][ix] = av->Cells[18][i];
       avc->Cells[13][ix] = av->Cells[19][i];
       avc->Cells[8][ix]=asCrit->Cells[9][i];
       avc->Cells[9][ix]=asCrit->Cells[3][i];
       avc->Cells[10][ix]=asCrit->Cells[5][i];
       avc->Cells[11][ix]=asCrit->Cells[7][i];



       for (j=1;j<9;j++)  // on recopie les 8 Criteres
         {
           ind = j*2 + 1;
           asCrit->Cells[ind][i] = av->Cells[j+40][i];  // asCrit->Cells[ind][i];
           asCrit->Colors[ind][i] = TColor(RGB(255,255,153));
           ind++;
           asCrit->Cells[ind][i] = av->Cells[j+50][i];
         }
     }
  }
 Calcul_Echeancier();
}

void __fastcall TplStandard::avcGetFormat(TObject *Sender, int ACol,
      TSortStyle &AStyle, AnsiString &aPrefix, AnsiString &aSuffix)
{

//TSortStyle = (ssAutomatic, ssAlphabetic, ssNumeric, ssDate, ssAlphaNoCase, ssAlphaCase,
//ssShortDateEU, ssShortDateUS, ssCustom, ssFinancial);

switch(ACol) {

case 4: ssShortDateEU; //  AStyle=ssDate;
case 5: ssShortDateEU;
break;
 }

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::RadioButton2ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
 TabSheet6->TabVisible = true;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button15Click(TObject *Sender)
{
 TabSheet6->TabVisible = false;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet6Show(TObject *Sender)
{
 // Recopie des taches de av
Remplir_asTri();

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::avGetFormat(TObject *Sender, int ACol,
      TSortStyle &AStyle, AnsiString &aPrefix, AnsiString &aSuffix)
{
 switch (ACol)
  {
   case 1 : AStyle  = ssNumeric;
   break;
  }
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::asTriGetFormat(TObject *Sender, int ACol,
      TSortStyle &AStyle, AnsiString &aPrefix, AnsiString &aSuffix)
{

switch (ACol)
  {
   case 1 : AStyle  = ssNumeric;
   break;
   case 3 : AStyle  = ssNumeric;
   break;
   case 4 : AStyle  = ssShortDateEU;
   break;
   case 5 :  AStyle  = ssShortDateEU;
   break;
  }

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::Button16Click(TObject *Sender)
{
 // pour simuler IMPORT Criteres
// strcpy(m_importcriteres,"c:\\ecoplanning5M\\criteres.txt");
// Build_Criteres();




}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btDateClick(TObject *Sender)
{

 BTACTUAL = 2;
 Exec_Avanc(false);
 // Remplir_asTri();  // �������������
}

void __fastcall TplStandard::Build_DateAvanc()
{


}


void __fastcall TplStandard::FillListBoxA()
{
 struct ffblk ffblk;
 int done; char tmp[250]; int distprev, nlast;
 char dir[250];  char APrev[20],MPrev[20],JPrev[20];;
 int aprev,mprev,jprev; char mess[200];
   AnsiString FDir,FName; char Project[500];
   char *p;  char fmt[100];
   char A[10],M[10],J[10],N[10];
  int  a,m,j,n;   char lastsuffix[50];
  int DISTLAST,NB_ACT,FIRST;
  int nb; int dist,maxdist;

  int maxd, distd;

 int adate,atime;  int i;   AnsiString Res;
 int year,month,day,hour,minute,second;

  sprintf(tmp,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
  p=tmp; strcpy(JPrev,p); JPrev[2]=0;  jprev=atoi(JPrev);
          p=p+3; strcpy(MPrev,p); MPrev[2]=0;  mprev=atoi(MPrev);
  p=p+3; strcpy(APrev,p); APrev[2]=0;  aprev=atoi(APrev);
  distprev = aprev*365 + mprev*31 + jprev;


   FDir = ExtractFileDir(DatabaseName);
   strcpy(dir,FDir.c_str()); strcat(dir,"\\*.act");
   // ListBox1->Clear();
   strcpy(dateavanc,"");
   nlast=0; strcpy(lastsuffix,"");
   nb=0;

 DISTLAST=0;
 nlast = 0; maxdist=0; maxd=0;
 strcpy(Project,ProjectName); strcat(Project,"-");
 done = findfirst(dir,&ffblk,0);
   while (!done)
   {
     strcpy(tmp,ffblk.ff_name);
     if (strstr(tmp,".act") && strstr(tmp,Project) && !strstr(tmp,"Copie"))
       {
        // Ajouter si meme projet
        //prepare dateavanc with existing files
        p=strstr(tmp,".act");
        if (p)
           {
            A[0] = *(p-8); A[1]= *(p-7); A[2]=0; a=atoi(A)%100;
            M[0] = *(p-5); M[1]= *(p-4); M[2]=0;   m=atoi(M);
            J[0] = *(p-2); J[1]= *(p-1); J[2]=0;   j=atoi(J);
            dist = a*365 + m*31 + j;

            if (dist <= distprev)  // <=   = pour eviter de charger l'actualisation a la date de d�but pr�visionnel
               {
                 /*if (FIRST)
                  {
                   Beep();
                   strcpy(mess,"La date d'actualisation du fichier ");
                   strcat(mess,tmp); strcat(mess," est ant�rieure � la date de d�but de projet\n");
                   strcat(mess,"Ce fichier n'est plus accessible et doit �tre supprim�");
                   Application->MessageBoxA(mess,m_ecoplan,MB_OK);
                  }
                 */
               }

            else
              {
               nb++;
               // ListBox1->Items->Add(AnsiString(tmp));

               sprintf(fmt,"%02d-%02d-%02d",a,m,j);
               distd = a*365 + m*31 + j;
               if (distd>DISTLAST) DISTLAST=distd;
               strcat(dateavanc,fmt); strcat(dateavanc,"@");
              }
           }

       }
      done = findnext(&ffblk);
   }
 if (nb==0)
   {
    NB_ACT=0;
    // Button1->Enabled = false;
    // btEff->Enabled = false;
   }
  else
   {
    NB_ACT=nb;
    // Button1->Enabled = true;
    // btEff->Enabled = true;
    //strcpy(lastfile,ListBox1->Items->Strings[nb-1].c_str());

   }
 FIRST = false;
}


//---------------------------------------------------------------------------
 void __fastcall TplStandard::Refresh_Avanc_All()
{
 int cntavanc,ctrow, i;  char tmp[150];
  int nbpr;   char dt1[20]; int dist1,a1,m1,j1,a2,m2,j2;
  int tcon; bool select;  char dt[20];  int distfin;
  char dtfin[20];    char numt[20]; int nt;
  int distavanc;  int percentrestant;



  Clear_Diagram(imAvCal); Clear_Diagram(imAvGantt);
  
   Clear_Diagram(immGnt);

  Clean_Grid(avact,2); Clean_Grid(avas,2);
  avact->RowCount = 2;

  cntavanc=asAvanc->RowCount;

  strcpy(dt1,DATEAVANC);
  dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
  distavanc = convert_date(a1+2000,m1,j1);


  ctrow=0;
  tcon=0;
  for (i=1;i<cntavanc;i++)
    {
         strcpy(tmp,asAvanc->Cells[18][i].c_str());  // % restant
         percentrestant = atoi(tmp);
         strcpy(adv,asAvanc->Cells[3][i].c_str()); strcpy(dt1,adv);   // debut reel

         strcpy(numt,asAvanc->Cells[1][i].c_str());
         nt = atoi(numt);
         if (nt != 0) av->Cells[21][nt] = AnsiString(adv);
         strcpy(dt,dt1);
         dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
         dist1 = convert_date(a1+2000,m1,j1);
         dt[2]='-'; dt[5]='-';

         strcpy(adv,asAvanc->Cells[5][i].c_str()); strcpy(dtfin,adv);   // fin reel
         if (nt != 0) av->Cells[22][nt] = AnsiString(adv);
         strcpy(dt1,dtfin);
         dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
         distfin = convert_date(a1+2000,m1,j1);


         ctrow++; if (ctrow > 1) avact->RowCount++;
         avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
         avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
         avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];

         avact->Cells[3][ctrow]=  asAvanc->Cells[20][i];
         avact->Cells[4][ctrow]=  asAvanc->Cells[21][i];


         avact->Cells[6][ctrow]= AnsiString(dist1); // AnsiString(10000-dist1);
         tcon++;
    }

  // tri avact par date de fin Descending
  /*
  avact->SortIndexes->Clear();
  avact->SortIndexes->Add(6);
  avact->QSortIndexed();
  */

  Update_Aff_Concernees(tcon);

  // Remplir_asTri_Avanc();  // �������������
  // Tracer Diagram
  btDPClick(0);

  CNTACTUAL = 1;

}

void __fastcall TplStandard::Refresh_Concern(bool strict)
{
  int cntavanc,ctrow, i;  char tmp[150];
  int nbpr;   char dt1[20]; int dist1,a1,m1,j1,a2,m2,j2;
  int tcon; bool select;  char dt[20];  int distfin;
  char dtfin[20]; bool OK;    int nt; char snumt[20];
  int distavanc;  int percentrestant;

   /////////////////
   // concern =true;
  if (strict) btActRapid->Caption = "Option / Toutes les t�ches en cours";
  else btActRapid->Caption = "Option / T�ches concern�es";

  Clear_Diagram(imAvCal); Clear_Diagram(imAvGantt);
  Clear_Diagram(immGnt);

  Clean_Grid(avact,2);
  Clean_Grid(avas,2);
  cntavanc=asAvanc->RowCount;

  strcpy(dt1,DATEAVANC);
  dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
  distavanc = convert_date(a1+2000,m1,j1);

  ctrow=0;
  tcon=0;
  for (i=1;i<cntavanc;i++)
    {
      strcpy(snumt,asAvanc->Cells[1][i].c_str());
      nt = atoi(snumt);

      if (asAvanc->Cells[20][i] != "X")
        {
         strcpy(tmp,asAvanc->Cells[18][i].c_str());  // % restant
         percentrestant = atoi(tmp);
         strcpy(adv,asAvanc->Cells[3][i].c_str()); strcpy(dt1,adv);   // debut reel
         strcpy(dt,dt1);

         dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
         dist1 = convert_date(a1+2000,m1,j1);
         dt[2]='-'; dt[5]='-';

         strcpy(adv,asAvanc->Cells[5][i].c_str()); strcpy(dtfin,adv);   // fin reel
         strcpy(dt1,dtfin);
         dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
         distfin = convert_date(a1+2000,m1,j1);

         //if (distfin < distavanc) continue;
         if (dist1>=distavanc)  //  || distfin < distavanc)
          continue;

         // enlever les croix si percentrestant >0 et < 100

   //      if (percentrestant == 0)  { asAvanc->Cells[20][i]="X"; asAvanc->Cells[21][i]=" "; }
   //      if (percentrestant == 100) { asAvanc->Cells[21][i]="X"; asAvanc->Cells[20][i]=" "; }
   //       if (percentrestant > 0 && percentrestant < 100) { asAvanc->Cells[21][i]=" "; asAvanc->Cells[20][i]=" "; }

         if (concern==false)
           {

               ctrow++; if (ctrow > 1) avact->RowCount++;
               avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
               avact->Cells[2][ctrow] = asAvanc->Cells[2][i];

               avact->Cells[3][ctrow] = asAvanc->Cells[20][i];
               avact->Cells[4][ctrow] = asAvanc->Cells[21][i];

               avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
               avact->Cells[6][ctrow]= AnsiString(10000-dist1);
               tcon++;
             }
         else  // concern  == true;
           {
             if (!strict)
               {
                OK = false;
                asAvanc->Cells[23][i]="";
               // if ((dist1 <= distavanc && distfin < distavanc)
               // || (dist1 <= distavanc && distfin >= distavanc))


               if (IsConcern(nt))   OK = true;
                 if (distfin > (distavanc-2) && dist1 < (distavanc + 2)) OK =true;
                 if (dist1 < distavanc && distfin > distavanc) OK = true;


               if (OK)

                 {
                  ctrow++; if (ctrow > 1) avact->RowCount++;
                  avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                  avact->Cells[2][ctrow] = asAvanc->Cells[2][i];

                   avact->Cells[3][ctrow]=  asAvanc->Cells[20][i];
                   avact->Cells[4][ctrow]=  asAvanc->Cells[21][i];

                  avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                  avact->Cells[6][ctrow]= AnsiString(dist1); // AnsiString(10000-dist1);
                  tcon++;
                  asAvanc->Cells[23][i]="X";
                  avact->Colors[1][ctrow]=clYellow;
                  Label132->Caption = AnsiString(ctrow);
                 }
                }
               else // strict
                 {
                  OK = false;
                  asAvanc->Cells[23][i]="";
                  // if (asAvanc->Cells[20][i] != "X" && asAvanc->Cells[21][i] != "X") OK=true;
                  //    && (dist1 <= distavanc)) //  && distfin >= distavanc))

                  // if(distfin <= distavanc && dist1 <=distavanc) OK = true;
                  if (percentrestant ==0) OK=false;
                  if (dist1 >= distavanc) OK=false;

                  else
                    {
                     if (asAvanc->Cells[20][i] != "X" && asAvanc->Cells[21][i] != "X") OK=true;
                     else  if (dist1 >= distavanc) OK=false;
                    }


                  if (OK)
                 {
                  ctrow++; if (ctrow > 1) avact->RowCount++;
                  avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                  avact->Cells[2][ctrow] = asAvanc->Cells[2][i];

                  avact->Cells[3][ctrow]=  asAvanc->Cells[20][i];
                  avact->Cells[4][ctrow]=  asAvanc->Cells[21][i];

                  avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                  avact->Cells[6][ctrow]= AnsiString(dist1); // AnsiString(10000-dist1);
                  tcon++;
                  asAvanc->Cells[23][i]="X";
                  avact->Colors[1][ctrow]=clYellow;
                  Label132->Caption = AnsiString(ctrow);
                 }
              }
          }

       }

     /*
     nbpr = atoi(tmp);
     if (concern == true)
       {
        if (mode==0)
         {  if (nbpr <= 100)
           {
           ctrow++; if (ctrow > 1) avact->RowCount++;
           avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
           avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
           avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
           avact->Cells[6][ctrow]= AnsiString(10000-dist1);
           tcon++;
          }
         }
       else
         {
          //if (nbpr >=0 && nbpr <= 100)
          //  {
             if (strcmp(DATEAVANC,dt)==0)
              {
                ctrow++; if (ctrow > 1) avact->RowCount++;
                avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
                avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                avact->Cells[6][ctrow]= AnsiString(10000-dist1);
                tcon++;
              }
            else if  (nbpr >0 && nbpr < 100)
            {
            ctrow++; if (ctrow > 1) avact->RowCount++;
                avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
                avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                avact->Cells[6][ctrow]= AnsiString(10000-dist1);
                tcon++;
            }



         }

       }
     else
       {
        ctrow++; if (ctrow > 1) avact->RowCount++;
        avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
        avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
        avact->Cells[5][ctrow]= asAvanc->Cells[18][i];
        avact->Cells[6][ctrow]= AnsiString(10000-dist1);
        tcon++;
       }
     */
    }

  // tri avact par date de fin Descending
  /*
  avact->SortIndexes->Clear();
  avact->SortIndexes->Add(6);
  avact->QSortIndexed();
  */

  Update_Aff_Concernees(tcon);



  // Tracer Diagram
  btDPClick(0);



}

 void __fastcall TplStandard::Refresh_Avanc(bool concern,bool strict)
 {
  int cntavanc,ctrow, i;  char tmp[150];
  int nbpr;   char dt1[20]; int dist1,a1,m1,j1,a2,m2,j2;
  int tcon; bool select;  char dt[20];  int distfin;
  char dtfin[20]; bool OK;    int nt; char snumt[20];
  int distavanc;  int percentrestant;

   /////////////////
   // concern =true;


  Clear_Diagram(imAvCal); Clear_Diagram(imAvGantt);
   Clear_Diagram(immGnt);

  Clean_Grid(avact,2);
  Clean_Grid(avas,2);
  cntavanc=asAvanc->RowCount;

  strcpy(dt1,DATEAVANC);
  dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
  distavanc = convert_date(a1+2000,m1,j1);

  ctrow=0;
  tcon=0;
  for (i=1;i<cntavanc;i++)
    {
      strcpy(snumt,asAvanc->Cells[1][i].c_str());
      nt = atoi(snumt);

      //if (asAvanc->Cells[20][i] != "X")
      /// {
         strcpy(tmp,asAvanc->Cells[18][i].c_str());  // % restant
         percentrestant = atoi(tmp);
         strcpy(adv,asAvanc->Cells[3][i].c_str()); strcpy(dt1,adv);   // debut reel
         strcpy(dt,dt1);
         dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
         dist1 = convert_date(a1+2000,m1,j1);
         dt[2]='-'; dt[5]='-';

         strcpy(adv,asAvanc->Cells[5][i].c_str()); strcpy(dtfin,adv);   // fin reel
         strcpy(dt1,dtfin);
         dt1[2]=0; j1=atoi(dt1);  dt1[5]=0; m1=atoi(dt1+3);  dt1[8]=0; a1=atoi(dt1+6);
         distfin = convert_date(a1+2000,m1,j1);

         // enlever les croix si percentrestant >0 et < 100

   //      if (percentrestant == 0)  { asAvanc->Cells[20][i]="X"; asAvanc->Cells[21][i]=" "; }
   //      if (percentrestant == 100) { asAvanc->Cells[21][i]="X"; asAvanc->Cells[20][i]=" "; }
   //       if (percentrestant > 0 && percentrestant < 100) { asAvanc->Cells[21][i]=" "; asAvanc->Cells[20][i]=" "; }

         if (concern==false)
           {

               ctrow++; if (ctrow > 1) avact->RowCount++;
               avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
               avact->Cells[2][ctrow] = asAvanc->Cells[2][i];

               avact->Cells[3][ctrow] = asAvanc->Cells[20][i];
               avact->Cells[4][ctrow] = asAvanc->Cells[21][i];

               avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
               avact->Cells[6][ctrow]= AnsiString(10000-dist1);
               tcon++;
             }
         else  // concern  == true;
           {
             if (!strict)
               {
                OK = false;
                asAvanc->Cells[23][i]="";
               // if ((dist1 <= distavanc && distfin < distavanc)
               // || (dist1 <= distavanc && distfin >= distavanc))

               if (IsConcern(nt))   OK = true;
                    //if (distfin > (distavanc-2) && dist1 < (distavanc + 2)) OK =true;
                    // if (dist1 < distavanc && distfin > distavanc) OK = true;
                   if (dist1 > distavanc) OK=false;


               if (OK)

                 {
                  ctrow++; if (ctrow > 1) avact->RowCount++;
                  avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                  avact->Cells[2][ctrow] = asAvanc->Cells[2][i];

                   avact->Cells[3][ctrow]=  asAvanc->Cells[20][i];
                   avact->Cells[4][ctrow]=  asAvanc->Cells[21][i];

                  avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                  avact->Cells[6][ctrow]= AnsiString(dist1); // AnsiString(10000-dist1);
                  tcon++;
                  asAvanc->Cells[23][i]="X";
                 }
                }
               else // strict
                 {
                  OK = false;
                  asAvanc->Cells[23][i]="";
                  //if ((asAvanc->Cells[20][i] != "X" && asAvanc->Cells[21][i] != "X")
                  //    && (dist1 <= distavanc)) //  && distfin >= distavanc))
                  if (dist1 <= distavanc && distfin >= distavanc) OK = true;


                  if (OK)
                 {
                  ctrow++; if (ctrow > 1) avact->RowCount++;
                  avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                  avact->Cells[2][ctrow] = asAvanc->Cells[2][i];

                  avact->Cells[3][ctrow]=  asAvanc->Cells[20][i];
                  avact->Cells[4][ctrow]=  asAvanc->Cells[21][i];

                  avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                  avact->Cells[6][ctrow]= AnsiString(dist1); // AnsiString(10000-dist1);
                  tcon++;
                  asAvanc->Cells[23][i]="X";
                 }
              }
          }

       //}

     /*
     nbpr = atoi(tmp);
     if (concern == true)
       {
        if (mode==0)
         {  if (nbpr <= 100)
           {
           ctrow++; if (ctrow > 1) avact->RowCount++;
           avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
           avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
           avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
           avact->Cells[6][ctrow]= AnsiString(10000-dist1);
           tcon++;
          }
         }
       else
         {
          //if (nbpr >=0 && nbpr <= 100)
          //  {
             if (strcmp(DATEAVANC,dt)==0)
              {
                ctrow++; if (ctrow > 1) avact->RowCount++;
                avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
                avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                avact->Cells[6][ctrow]= AnsiString(10000-dist1);
                tcon++;
              }
            else if  (nbpr >0 && nbpr < 100)
            {
            ctrow++; if (ctrow > 1) avact->RowCount++;
                avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
                avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
                avact->Cells[5][ctrow]=  asAvanc->Cells[18][i];
                avact->Cells[6][ctrow]= AnsiString(10000-dist1);
                tcon++;
            }



         }

       }
     else
       {
        ctrow++; if (ctrow > 1) avact->RowCount++;
        avact->Cells[1][ctrow] = asAvanc->Cells[1][i];
        avact->Cells[2][ctrow] = asAvanc->Cells[2][i];
        avact->Cells[5][ctrow]= asAvanc->Cells[18][i];
        avact->Cells[6][ctrow]= AnsiString(10000-dist1);
        tcon++;
       }
     */
    }

  // tri avact par date de fin Descending
  /*
  avact->SortIndexes->Clear();
  avact->SortIndexes->Add(6);
  avact->QSortIndexed();
  */

  Update_Aff_Concernees(tcon);



  // Tracer Diagram
  btDPClick(0);

 }



void __fastcall TplStandard::avactClickCell(TObject *Sender, int ARow,
      int ACol)
{
 // envoyer donn�es dans zone de saisie
 int numt,nt; int i,cnt,rowav; char snumt[200];
 int numavanc;    char tmp[150]; int ch,xcal;
 int x,y;   bool valid; AnsiString Temp;
 int prem_tache;

  if (ARow==0) return;
  strcpy(snumt,avact->Cells[1][ARow].c_str());
  numt = atoi(snumt);
  NUMT = numt;
  if (numt==0)
  {Beep();
  return; }

  cnt = asAvanc->RowCount;
    row_avancement = 0;
    for (i=1;i<cnt;i++)
    {
      strcpy(snumt,asAvanc->Cells[1][i].c_str());
      numavanc = atoi(snumt);
      if (numavanc == numt) row_avancement = i;
     }
  if (row_avancement ==0) return;


    // Row Avancement : copie data dans Adv4  et Pred
   ClickasAvanc();

  if (ACol==4)  // Reste � 100 %
    {

     if (avact->Cells[ACol][ARow] == "X")
      {
         avact->Cells[ACol][ARow] = "";
         asAvanc->Cells[21][row_avancement]="";
         asAvanc->Cells[20][row_avancement]="";
         AdvStringGrid6->Cells[20][row_avancement]="";
         av->Cells[65][numt]="";
         av->Cells[66][numt]="";
         avact->Cells[3][ARow] = "";
         avact->Colors[1][ARow]=clYellow;
         // AdvStringGrid4->Cells[5][1]="100";

       }
     else
       {
         avact->Cells[ACol][ARow] = "X";
         avact->Cells[3][ARow] = "";
         av->Cells[31][numt]="100";



         asAvanc->Cells[18][row_avancement]="100";
         asAvanc->Cells[20][row_avancement]="";
AdvStringGrid6->Cells[20][row_avancement]="";
		asAvanc->Cells[21][row_avancement]="X";

         av->Cells[65][numt]="";
         av->Cells[66][numt]="X";

         avact->Colors[1][ARow]=clWhite;
         // lire dur�e previsionnelle  // r�elle
         strcpy(tmp,asAvanc->Cells[4][row_avancement].c_str());    // Duree reelle
         x=atoi(tmp);
         asAvanc->Cells[19][row_avancement]= AnsiString(tmp);  // nbj Restants
         AdvStringGrid6->Cells[19][row_avancement]= AnsiString(tmp);  // nbj Restants
		 AdvStringGrid4->Cells[5][1]="100";
		 AdvStringGrid7->Cells[5][1]="100";
         AdvStringGrid4->Cells[6][1]=AnsiString(tmp);
         AdvStringGrid7->Cells[6][1]=AnsiString(tmp);

       }
    }
  if (ACol==3)  // Reste  0%
    {
     if ( avact->Cells[ACol][ARow] == "X")
        { avact->Cells[ACol][ARow] = "";
          avact->Cells[4][ARow] = "";
          asAvanc->Cells[20][row_avancement]="";
		  AdvStringGrid6->Cells[20][row_avancement]= "";
          asAvanc->Cells[21][row_avancement]="";
          av->Cells[65][numt]="";
          av->Cells[66][numt]="";
		  asAvanc->Cells[18][row_avancement]="";
          AdvStringGrid6->Cells[18][row_avancement]="";
		  asAvanc->Cells[19][row_avancement]="";
          AdvStringGrid6->Cells[19][row_avancement]="";
          avact->Colors[1][ARow]=clYellow;
        }
         else

         {avact->Cells[ACol][ARow] = "X";
         avact->Cells[4][ARow] = "";
         asAvanc->Cells[20][row_avancement]="X";
		 AdvStringGrid6->Cells[20][row_avancement]="X";
         asAvanc->Cells[21][row_avancement]="";
         av->Cells[65][numt]="X";
         av->Cells[66][numt]="";
         av->Cells[31][numt]="0";
         asAvanc->Cells[18][row_avancement]="0";
          AdvStringGrid6->Cells[18][row_avancement]="0";
         asAvanc->Cells[19][row_avancement]="0";
          AdvStringGrid6->Cells[19][row_avancement]="0";
         avact->Colors[1][ARow]=clWhite;
         AdvStringGrid4->Cells[5][1]="0";
         AdvStringGrid4->Cells[6][1]="0";
         AdvStringGrid7->Cells[5][1]="0";
         AdvStringGrid7->Cells[6][1]="0";
       }
    }



  avas->Cells[1][1] = asAvanc->Cells[1][row_avancement];
  avas->Cells[2][1] = asAvanc->Cells[2][row_avancement];
  Temp= asAvanc->Cells[18][row_avancement];
  avas->Cells[3][1] = asAvanc->Cells[18][row_avancement];
  Temp= asAvanc->Cells[19][row_avancement];
  avas->Cells[4][1] = asAvanc->Cells[19][row_avancement];
  //if (asAvanc->Cells[23][row_avancement]=="")
  //     asAvanc->Cells[23][row_avancement] = asAvanc->Cells[3][row_avancement];

  if (asAvanc->Cells[4][row_avancement]=="")
     asAvanc->Cells[4][row_avancement]=asAvanc->Cells[9][row_avancement];
AdvStringGrid6->Cells[4][row_avancement]=AdvStringGrid6->Cells[9][row_avancement];
  Temp = asAvanc->Cells[4][row_avancement];
  avas->Cells[5][1]  = Temp;
  avas->Cells[6][1]  = asAvanc->Cells[3][row_avancement];
  avas->Cells[7][1]  = asAvanc->Cells[5][row_avancement];
  avas->Cells[8][1]  = asAvanc->Cells[8][row_avancement];
  avas->Cells[9][1]  = asAvanc->Cells[10][row_avancement];
  avas->Cells[10][1] = asAvanc->Cells[7][row_avancement];
  avas->Cells[11][1] = asAvanc->Cells[6][row_avancement];

  xcal=1;



// $$$$  if (ACol==3) valid = Call_Process(29,xcal,"100");    // tmp est la valeur du champ cliqu�
// $$$$  else  if (ACol==4) valid = Call_Process(29,xcal,"0");

 ch=28;

//  if (ACol==3)  ch=100;
//  if (ACol==4)  ch=101;
  valid = true;
  if ((ACol==3 || ACol==4) && valid)
    {
      ORIGINE_CLICK=1;  // Click une croix

      CH=ch;
      Timer2->Enabled=true;
       // Propage(ch);
    }


Graphics::TBitmap *  imP2 = new Graphics::TBitmap();
//imP2->LoadFromFile("C:\\Users\\Claude\\Desktop\\demo\\cap2.bmp");
  imP2->PixelFormat =pf32bit;
//imP2->Canvas->StretchDraw(Rect(0,0,Image2->Width, Image2->Height),imP2);
imP2->Width = Image2->Width;
imP2->Height = Image2->Height;

Image2->Picture->Assign(imP2);
delete imP2;
////

/*
  /// $$$$$$$$$
  if (valid==false)
    {
     Refresh_Avanc(true,1);
    }
  /// $$$$$
*/




  // recherche tache avec numt ... pour trouver row_avancement


 /*  $$$
 if (row_avancement !=0)
  ClickasAvanc(); // row_avancement);
 */

}


/*
void __fastcall TplStandard::avactClickCell(TObject *Sender, int ARow,
      int ACol)
{
 // envoyer donn�es dans zone de saisie
 int numt,nt; int i,cnt,rowav; char snumt[200];
 int numavanc;    char tmp[150]; int ch,xcal;
 int x,y;   bool valid; AnsiString Temp;
 int prem_tache;

  if (ARow==0) return;
  strcpy(snumt,avact->Cells[1][ARow].c_str());
  numt = atoi(snumt);
  NUMT = numt;
  if (numt==0)
  {Beep();
  return; }

  cnt = asAvanc->RowCount;
    row_avancement = 0;
    for (i=1;i<cnt;i++)
    {
      strcpy(snumt,asAvanc->Cells[1][i].c_str());
      numavanc = atoi(snumt);
      if (numavanc == numt) row_avancement = i;
     }


  // ClickasAvanc();   // Row Avancement : copie data dans Adv4  et Pred


  if (ACol==4)  // Reste � 100 %
    {

     if (avact->Cells[ACol][ARow] == "X")
      {
         avact->Cells[ACol][ARow] = "";
         asAvanc->Cells[21][row_avancement]="";
         asAvanc->Cells[20][row_avancement]="";
         avact->Cells[3][ARow] = "";
         avact->Colors[1][ARow]=clYellow;

         AdvStringGrid6->Cells[20][row_avancement]="";

       }
     else
       {
         avact->Cells[ACol][ARow] = "X";
         avact->Cells[3][ARow] = "";
         av->Cells[31][numt]="100";
  AdvStringGrid6->Cells[ACol][ARow] = "X";
         AdvStringGrid6->Cells[3][ARow] = "";

          asAvanc->Cells[18][row_avancement]="100";
          asAvanc->Cells[21][row_avancement]="X";
          asAvanc->Cells[20][row_avancement]="";

          AdvStringGrid6->Cells[18][row_avancement]="100";
          AdvStringGrid6->Cells[20][row_avancement]="";
          avact->Colors[1][ARow]=clWhite;
         // lire dur�e previsionnelle  // r�elle
         strcpy(tmp,asAvanc->Cells[9][row_avancement].c_str());
         x=atoi(tmp);
         asAvanc->Cells[19][row_avancement]= AnsiString(tmp);  // nbj Restants
       }
    }
  if (ACol==3)  // Reste  0%
    {
     if ( avact->Cells[ACol][ARow] == "X")
        { avact->Cells[ACol][ARow] = "";
          avact->Cells[4][ARow] = "";
          asAvanc->Cells[20][row_avancement]="";
          asAvanc->Cells[21][row_avancement]="";
          asAvanc->Cells[18][row_avancement]="";
          asAvanc->Cells[19][row_avancement]="";

          AdvStringGrid6->Cells[20][row_avancement]="";
          AdvStringGrid6->Cells[18][row_avancement]="";
          AdvStringGrid6->Cells[19][row_avancement]="";
          avact->Colors[1][ARow]=clYellow;
        }
         else

         {avact->Cells[ACol][ARow] = "X";
         avact->Cells[4][ARow] = "";
         asAvanc->Cells[20][row_avancement]="X";
         asAvanc->Cells[21][row_avancement]="";
         av->Cells[31][numt]="0";
         asAvanc->Cells[18][row_avancement]="0";
         asAvanc->Cells[19][row_avancement]="0";
		  AdvStringGrid6->Cells[20][row_avancement]="X";
          AdvStringGrid6->Cells[18][row_avancement]="0";
          AdvStringGrid6->Cells[19][row_avancement]="0";

         avact->Colors[1][ARow]=clWhite;
       }
    }



  avas->Cells[1][1] = asAvanc->Cells[1][row_avancement];
  avas->Cells[2][1] = asAvanc->Cells[2][row_avancement];
  Temp= asAvanc->Cells[18][row_avancement];
  avas->Cells[3][1] = asAvanc->Cells[18][row_avancement];
  Temp= asAvanc->Cells[19][row_avancement];
  avas->Cells[4][1] = asAvanc->Cells[19][row_avancement];
  //if (asAvanc->Cells[23][row_avancement]=="")
  //     asAvanc->Cells[23][row_avancement] = asAvanc->Cells[3][row_avancement];

  if (asAvanc->Cells[4][row_avancement]=="")
     asAvanc->Cells[4][row_avancement]=asAvanc->Cells[9][row_avancement];

  Temp = asAvanc->Cells[4][row_avancement];
  avas->Cells[5][1]  = Temp;
  avas->Cells[6][1]  = asAvanc->Cells[3][row_avancement];
  avas->Cells[7][1]  = asAvanc->Cells[5][row_avancement];
  avas->Cells[8][1]  = asAvanc->Cells[8][row_avancement];
  avas->Cells[9][1]  = asAvanc->Cells[10][row_avancement];
  avas->Cells[10][1] = asAvanc->Cells[7][row_avancement];
  avas->Cells[11][1] = asAvanc->Cells[6][row_avancement];


  xcal=1;


  if (ACol==3) valid = Call_Process(29,xcal,"100");    // tmp est la valeur du champ cliqu�
  else  if (ACol==4) valid = Call_Process(29,xcal,"0");


//
//  if (ACol==3)  ch=100;
//  if (ACol==4)  ch=101;

//  if ((ACol==3 || ACol==4) && valid)
//    {
//      CH=ch;
//      Timer2->Enabled=true;
//       // Propage(ch);
//    }

////

//
  /// $$$$$$$$$
//  if (valid==false)
//    {
//     Refresh_Avanc(true,1);
//    }
//  /// $$$$$
//


  // tri pour placer la tache selectionn�e en haut

//  for (i=1;i<ARow;i++)
//   { strcpy(tmp,avact->Cells[1][i].c_str()); nt=atoi(tmp);
//     sprintf(tmp,"%04d-%d",9999,nt); avact->Cells[6][i] = AnsiString(tmp); }
//  for (i=ARow;i<avact->RowCount;i++) { sprintf(tmp,"%04d",i); avact->Cells[6][i] = AnsiString(tmp); }


//  avact->SortIndexes->Clear();
//  avact->SortIndexes->Add(6);
//  avact->QSortIndexed();




  // recherche tache avec numt ... pour trouver row_avancement


 //  $$$
 //if (row_avancement !=0)
 // ClickasAvanc(); // row_avancement);
 //

//}

*/

//---------------------------------------------------------------------------

int __fastcall TplStandard::Reverse_Date(char *dt)
{
 char tmp[20]; int dist;
 strcpy(tmp,dt);
 dt[0]=tmp[6]; dt[1]=tmp[7];
 dt[6]=tmp[0]; dt[7]=tmp[1];
 dist = Convert_Date(AnsiString(dt));
 return dist;
}

void __fastcall TplStandard::Display_Graphics(int prt,TImage *imAC, TImage *imGNT )
{
 int i,nt,cntrow,from; char snumt[50]; char tmp[150];
 int distprev,distreel,maxdprev,maxdreel,maxdist;   int pas;
 int PasV,v1,v2,v3; int x1,x2,y1,y2,pasx,step;
 AnsiString Avant,Apres;  int res;
 char dt1[20],dt2[20];  TRect diagram;
 int AnREF,MoisREF,JourREF; int dist;
 char dtPrev[15],dtAvanc[15]; int indcal;
 TDateTime dtFirst; int dow;
 int  distavanc;


 Clear_Diagram(imGNT);
 Clear_Diagram(imAC);

 //  NBCells = 20;  // $$$$
 // parcourir la table pour trouver les taches, et calculer distances;
 from=0;
 cntrow= avact->RowCount;
 maxdprev=0;
 for (i=0; i<cntrow;i++)
   {
    strcpy(snumt,avact->Cells[1][i].c_str());
    nt = atoi(snumt);
    if (nt==prt) { from=nt; }

   }

 Compute_D1_D2();
 Prepare_Tableau(from,D1);
 Copy_Calendar(1,D1);

 Update_Aff_Concernees(NBCourb);

 TRect Diagram;
 Diagram = Rect(1,1, imAC->Width-1,imAC->Height);
 imAC->Canvas->Brush->Color = clAqua; // BgColor;
 imAC->Canvas->FillRect(Diagram);
 // imAC->Canvas->Brush->Color = clAqua; // BgColor;
 // imAC->Canvas->FillRect(Diagram);

 if (DMAX >= 0)
    { pas = imGNT->Width / (DMAX+D1+D2);
      if (DMAX<5) step=1; else if (DMAX<25) step=7; else if (DMAX<62) step=14;
    }
   else { pas = imGNT->Width / (-DMAX+D1+D2);  step=1; }

    // Trac� des lignes verticales calendrier

 PasV = 21;  v1=-13; v2=-7; v3=-21;



 for (i=1;i<=NBCourb;i++)
  {
    if (i>1)
     {
    imGNT->Canvas->Pen->Width=1;
    imGNT->Canvas->Pen->Color = clBlack;
    imGNT->Canvas->MoveTo(0,i*PasV + v3);
    imGNT->Canvas->LineTo(imGNT->Width,i*PasV + v3);
     }
    imGNT->Canvas->Pen->Width=3;
    imGNT->Canvas->Pen->Color = clRed;
    imGNT->Canvas->MoveTo((ddprev[i]-D1)*pas,i*PasV +v1);
    imGNT->Canvas->LineTo((dfprev[i]-D1)*pas,i*PasV +v1);
    imGNT->Canvas->Pen->Width=3;
    imGNT->Canvas->Pen->Color = clGreen;
    imGNT->Canvas->MoveTo((ddreel[i]-D1)*pas,i*PasV +v2);
    imGNT->Canvas->LineTo((dfreel[i]-D1)*pas,i*PasV +v2);
  }  // for i
  // derniere ligne noire
    imGNT->Canvas->Pen->Width=1;
    imGNT->Canvas->Pen->Color = clBlack;
    imGNT->Canvas->MoveTo(0,i*PasV + v3);
    imGNT->Canvas->LineTo(imGNT->Width,i*PasV + v3);
   if (DMAX >= 0)
    {
     pasx = pas;  //

     add_jour(AnPrev,MoisPrev,JourPrev,D1);
     sprintf(dt2,"%02d/%02d",Jour,Mois);
     AnREF=An; MoisREF=Mois; JourREF = Jour;


     for (i=D1;i<(D1+DMAX+D2+2);i++) // i=i+step)
      {
       y1=imAC->Height/2; y2=imAC->Height;
       x1=(i-D1)*pasx;
       add_jour(AnPrev,MoisPrev,JourPrev,i);
       imAC->Canvas->Pen->Width=1;
       imAC->Canvas->Brush->Style = bsClear;
       imAC->Canvas->Pen->Color = clBlack;
       imAC->Canvas->MoveTo(x1,y1);
       imAC->Canvas->LineTo(x1,y2);
       sprintf(dt2,"%02d",Jour);
       y1=y1+2; imAC->Canvas->TextOutA(x1+2,y1,AnsiString(dt2));
       imAC->Canvas->Font->Size = 8;
      }

      imAC->Canvas->MoveTo(0,imAC->Height/2);
      imAC->Canvas->LineTo(imAC->Width,imAC->Height/2);


     for (i=D1;i<(D1+DMAX+D2);i++) // i=i+step)
      {
       y1=0; y2=imAC->Height;
       x1=(i-D1)*pasx;



       add_jour(AnPrev,MoisPrev,JourPrev,i);
       dtFirst = EncodeDate(An,Mois,Jour);
       int firstday = dtFirst.DayOfWeek();
       dow = firstday-1;
       if (dow==1 || i==D1)
         {
           imAC->Canvas->Pen->Width=1;
           imAC->Canvas->Brush->Style = bsClear;
           imAC->Canvas->Pen->Color = clBlack;
           imAC->Canvas->MoveTo(x1,y1);
           imAC->Canvas->LineTo(x1,y2);
           sprintf(dt2,"%02d/%02d",Jour,Mois);
           imAC->Canvas->Font->Size = 8;
           y1=2; imAC->Canvas->TextOutA(x1+2,y1,AnsiString(dt2));
           y1=0; y2=NBCourb*21;
           x1=i*pasx;
           imGNT->Canvas->Pen->Width=1;
           imGNT->Canvas->Pen->Color = clBlack;
           imGNT->Canvas->MoveTo(x1,y1);
           imGNT->Canvas->LineTo(x1,y2);
         }
      }
      // calculer indice i d'origine

      sprintf (dtPrev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
      sprintf (dtAvanc,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);

      comp_dist_dates(dtPrev,dtAvanc,1,0);

       indcal = NB1-D1;

      for (i=D1;i<(D1+DMAX+D2);i++)      /// i=0
      {
       add_jour(AnREF,MoisREF,JourREF,i);
       indcal = i; // D1+i;
       // indcal = i;
       if (ACAL[indcal]=='N')
         { y2=NBCourb*21;
           x1=(i-D1)*pasx;
           diagram = Rect(x1,2,x1+pas-1,y2);
           imGNT->Canvas->Brush->Color = clSilver; //
           imGNT->Canvas->FillRect(diagram);
         }
      }


     x1= (DMAX-D1)*pas;
     y1=0; y2=NBCourb*21; //y2=imGNT->Height;
     /*
     imGNT->Canvas->Pen->Width=2;
     imGNT->Canvas->Pen->Color = clRed;
     imGNT->Canvas->MoveTo(x1,0);
     imGNT->Canvas->LineTo(x1,y2);
     x1 = (D1+ DMAX)*pas;
     imGNT->Canvas->MoveTo(x1,0);
     imGNT->Canvas->LineTo(x1,y2);
     */
     imGNT->Canvas->Brush->Style = bsClear;
     imGNT->Canvas->Pen->Width=1;
     imGNT->Canvas->Pen->Color = clBlack;
     for (i=1;i<=NBCourb;i++)
       {
         sprintf(tmp,"T�che %d",avtask[i]);
         x1= (dfreel[i]-D1)*pas+5; y1= i*PasV +v3+5;
         imGNT->Canvas->TextOutA(x1,y1,AnsiString(tmp));
       }
     x1= (DIST_AVANC_1-D1)*pas + 1; y1=0; y2=NBCourb*21; //y2=imGNT->Height;
     imGNT->Canvas->Pen->Width=4;
     imGNT->Canvas->Pen->Color = clGreen;
     imGNT->Canvas->MoveTo(x1,0);
     imGNT->Canvas->LineTo(x1,y2);

    // int  distavanc = convert_date(AnAvanc,MoisAvanc,JourAvanc);

      if (CntAVBefore > 0)
        {
          strcpy(tmp,TableDatesAV[CntAVBefore-1]);   // Avancement Pr�c�dent
           distavanc = Reverse_Date(tmp);


           x1= (distavanc-D1)*pas+1; y1=0; y2=NBCourb*21; //y2=imGNT->Height;
           imGNT->Canvas->Pen->Width=4;
           imGNT->Canvas->Pen->Color = clGreen;
           imGNT->Canvas->MoveTo(x1,0);
           imGNT->Canvas->LineTo(x1,y2);
        }
   }


}




 void __fastcall TplStandard::Compute_D1_D2()
 {
  int i,dist; int distav_1;   AnsiString Avant,Apres;
  char tmp[200]; char an[20],mois[20],jour[20];

  if (CntAVBefore == 0)
   { strcpy(FNAVANC,""); D1=0; DIST_AVANC_1=0;
      an_1=AnPrev; mois_1=MoisPrev; jour_1=JourPrev;
   }

  else if (CntAVBefore == 1)
    {

      strcpy(tmp,TableDatesAV[0]);
      strcpy(FNAVANC,DatabaseName);
      strcat(FNAVANC,"-");
      //REVERSE_DATE(tmp);
      strcat(FNAVANC,tmp);

           strcpy(tmp,TableDatesAV[0]);   // Avancement en cours
           dist = Reverse_Date(tmp);

           DIST_AVANC_1 = dist;
           an_1=AnPrev; mois_1=MoisPrev; jour_1=JourPrev;

           Avant = cbAvant->Text;
           if (Avant == "1 Semaine Avant") D1=dist - 7;
           else if ( Avant == "2 Semaines Avant") D1=dist - 14;
           else if ( Avant == "1 Mois Avant") D1=dist - 30;
           else if (Avant == "D�but Actualisation") D1=0; //dist;
           else D1=0;  // pr�visionnel
         //  }
       if (D1<0) D1=0;
          D1=0;


    }
  else
     for (i=1;i<CntAVBefore;i++)
      {
        strcpy(tmp,TableDatesAV[i]);
        // build FNAVANC;

        strcpy(FNAVANC,DatabaseName);
        // strcat(FNAVANC,ProjectName);
        strcat(FNAVANC,"-");
        tmp[8]=0;
        //REVERSE_DATE(tmp);
        // strcat(FNAVANC,tmp);
        strcat(FNAVANC,tmp);

        if (strcmp(tmp,DATEAVANC)==0)
          {
           strcpy(tmp,TableDatesAV[i-1]);   // Avancement Pr�c�dent
           dist = Reverse_Date(tmp);
           DIST_AVANC_1 = dist;
           strcpy(jour,tmp); jour[2]=0; jour_1=atoi(jour);
           strcpy(mois,tmp+3); mois[2]=0; mois_1=atoi(mois);
           strcpy(an,tmp+6); an[2]=0; an_1=atoi(an);


           Avant = cbAvant->Text;
           if (Avant == "1 Semaine Avant") D1=dist - 7;
           else if ( Avant == "2 Semaines Avant") D1=dist - 14;
           else if ( Avant == "1 Mois Avant") D1=dist-30;
           else if (Avant == "D�but Actualisation") D1=dist;
           else D1=0;  // pr�visionnel
          }
       else
          {



          }


       if (D1<0) { D1=0; an_1=AnPrev; mois_1=MoisPrev; jour_1=JourPrev; }
   }


 Apres = cbApres->Text;
 if (Apres == "1 Semaine Apr�s") D2=7;
 else if ( Apres == "2 Semaines Apr�s") D2=14;
 else if ( Apres == "1 Mois Apr�s") D2=30;
 else if ( Apres == "2 Mois Apr�s") D2=61;
 else D2=0;

}


void __fastcall TplStandard::REVERSE_DATE(char *dt)
{
 char c1,c2;
 c1=dt[0];c2=dt[1];
 dt[0]=dt[6];dt[1]=dt[7];
 dt[6]=c1; dt[7]=c2;

}

 void __fastcall TplStandard::Compute_DD1_DD2()
 {
  int i,dist;    AnsiString Avant,Apres;
  char tmp[200];  int distav_1;

   char an[20],mois[20],jour[20];

  sprintf(DatePrev,"%02d-%02d-%02d",AnPrev%100,MoisPrev,JourPrev);

  if (CntAVBefore == 0)
   { strcpy(FNAVANC,""); DD1=0; DIST_AVANC_1=0;
     strcpy(DATEAVANC_1,DatePrev);
      an_1=AnPrev%100; mois_1=MoisPrev; jour_1=JourPrev;
   }
  else if (CntAVBefore == 1)
    {

      strcpy(tmp,TableDatesAV[0]);
      strcpy(FNAVANC,DatabaseName);
      strcat(FNAVANC,"-");
      //REVERSE_DATE(tmp);
      strcat(FNAVANC,tmp);
      //REVERSE_DATE(tmp);
      strcpy(DATEAVANC_1,DatePrev);
      tmp[8]=0;
        distav_1 = Reverse_Date(tmp);



           strcpy(tmp,TableDatesAV[0]);   // Avancement en cours
           dist = Reverse_Date(tmp);

           DIST_AVANC_1 = dist;
           an_1=AnPrev%100; mois_1=MoisPrev; jour_1=JourPrev;

           Avant = cbAvantX->Text;
           if (Avant == "1 Semaine Avant") DD1=dist - 7;
           else if ( Avant == "2 Semaines Avant") DD1=dist - 14;
           else if ( Avant == "1 Mois Avant") DD1=dist - 30;
           else if (Avant == "D�but Actualisation") DD1=0; //dist;
           else DD1=0;  // pr�visionnel
         //  }
       if (DD1<0) DD1=0;
          DD1=0;


    }

  else    // Cnt>=2
    {
    for (i=1;i<CntAVBefore;i++)
      {
        strcpy(DATEAVANC_1,TableDatesAV[i-1]);
        DATEAVANC_1[8]=0;
        strcpy(tmp,TableDatesAV[i]);
        strcpy(FNAVANC,DatabaseName);

        strcat(FNAVANC,"-");
        REVERSE_DATE(tmp);
        strcat(FNAVANC,tmp);
        REVERSE_DATE(tmp);

        distav_1 = Reverse_Date(tmp);

        if (strcmp(tmp,DATEAVANC)==0)
          {
           strcpy(tmp,TableDatesAV[i-1]);   // Avancement Pr�c�dent
           dist = Reverse_Date(tmp);
           DIST_AVANC_1 = dist;
           strcpy(jour,tmp); jour[2]=0; jour_1=atoi(jour);
           strcpy(mois,tmp+3); mois[2]=0; mois_1=atoi(mois);
           strcpy(an,tmp+6); an[2]=0; an_1=atoi(an);


           Avant = cbAvantX->Text;
           if (Avant == "1 Semaine Avant") DD1=dist - 7;
           else if ( Avant == "2 Semaines Avant") DD1=dist - 14;
           else if ( Avant == "1 Mois Avant") DD1=dist-30;
           else if (Avant == "D�but Actualisation") DD1=dist;
           else DD1=0;  // pr�visionnel
          }

       if (DD1<0) { DD1=0; an_1=AnPrev; mois_1=MoisPrev; jour_1=JourPrev; }
     } // end FOR
   }  // ELSE




 Apres = cbApresX->Text;
 if (Apres == "1 Semaine Apr�s") DD2=7;
 else if ( Apres == "2 Semaines Apr�s") DD2=14;
 else if ( Apres == "1 Mois Apr�s") DD2=30;
 else if ( Apres == "2 Mois Apr�s") DD2=61;
 else DD2=0;

 }


/*
void __fastcall TplStandard::Prepare_Tableau(int from,int deb)
{
 int i,cnt; int numt; char snumt[30]; int indx;
 int j, inda, numa; char  dtprev[20],dt1[20];
 int cntasv,res;  char dtdeb[20],dtavanc[20];
 char ddr[20],dfr[20];
 int rc,recdata; char Key[200],RetKey[200]; char buf[2000];


 DMAX=0;

 if (strlen(FNAVANC)==0) return;

 for (i=0;i<NBCells;i++)
   { ddprev[i]=0; dprev[i]=0; dfprev[i]=0; ddreel[i]=0; dreel[i]= 0;ddreel[i]=0; avtask[i]=0;}

 cnt=avact->RowCount;
 cntasv = asAvanc->RowCount;

 add_jour(AnPrev,MoisPrev,JourPrev,0);
 sprintf (dtprev,"%02d-%02d-%02d",Jour,Mois,An%100);
 sprintf (dtavanc,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);

 rc = comp_dist_dates(dtprev,dtavanc,1,0);
 if (NB1-deb <0)  {Jour = JourPrev; Mois=MoisPrev; An=AnPrev;}
 else add_jour(AnPrev,MoisPrev,JourPrev,NB1-deb);

 sprintf (dtdeb,"%02d-%02d-%02d",Jour,Mois,An%100);

for(i=1;i<cnt;i++)
   {
    strcpy(snumt,avact->Cells[1][i].c_str());
    numt = atoi(snumt);
    if (numt==from) indx=i;
   }


// Ouvrir avant derniere actualisation dans FileAvanc

  // rc = avanc->OpenEngine(FileNameAvanc,120,"act","don");   //512 - 8
  rc = avanc->OpenEngine(FNAVANC,120,"act","don");
  rc = avanc->SetIndexMode(0,0);
  int nbk = avanc->NumberOfKeys(0);


NBCourb=0;
 for(i=indx;i<cnt;i++)
   {
    if (i>=NBCells) break;
    strcpy(snumt,avact->Cells[1][i].c_str());
    numt = atoi(snumt);
    avtask[i]=numt;
    inda=0;
    for (j=1;j<cntasv;j++)
      {
       strcpy(snumt,asAvanc->Cells[1][j].c_str());
       numa = atoi(snumt);
       if (numa==numt)
          { //  on prend les dates col 8,10, 3,5

           sprintf(Key,"T%04d",numa);
           rc=avanc->ReadDirectKey(0,Key,RetKey,&recdata);
           if (rc)
              {
               rc = avanc->ReadRecord(buf,recdata);
               Ghost->ExtractValue(ddr,buf,"ddr",0);
               Ghost->ExtractValue(dfr,buf,"dfr",0);
              }

           strcpy(dt1,asAvanc->Cells[8][j].c_str());
           //res = comp_dist_dates(dtavanc,dt1,1,0);
           res = comp_dist_dates(dtdeb,dt1,1,0);
           ddprev[i] = NB3;   // NB3=NB1
           if (NB3>DMAX) DMAX=NB3;
           strcpy(dt1,asAvanc->Cells[10][j].c_str());
           res = comp_dist_dates(dtdeb,dt1,1,0);

           dfprev[i] = NB3+1;
           if (NB3>DMAX) DMAX=NB3+1;

           strcpy(dt1,asAvanc->Cells[3][j].c_str());
           res = comp_dist_dates(dtavanc,dt1,1,0);

           ddreel[i] = NB3;
           if (NB3>DMAX) DMAX=NB3;
           strcpy(dt1,asAvanc->Cells[5][j].c_str());
           res = comp_dist_dates(dtavanc,dt1,1,0);

           dfreel[i] = NB3+1;
           if (NB3>DMAX) DMAX=NB3+1;
           NBCourb++;
          }

      }


   }

 avanc->CloseEngine();
 // delete avanc;



}
*/




void __fastcall TplStandard::Prepare_Tableau(int from,int deb)
{
 int i,cnt; int numt; char snumt[30]; int indx;
 int j, inda, numa; char  dtprev[20],dt1[20];
 int cntasv,res;  char dtdeb[20],dtavanc[20];
 char ddr[20],dfr[20];  char DATE_AMJ[20];
 int rc,recdata; char Key[200],RetKey[200]; char buf[2000];
 char datavanc[10];

  for (i=0;i<NBCells;i++)
   { ddprev[i]=0; dprev[i]=0; dfprev[i]=0; ddreel[i]=0; dreel[i]= 0;ddreel[i]=0; avtask[i]=0;}
 DMAX=0;

 sprintf (dtprev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
 sprintf (dtavanc,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);
 rc = comp_dist_dates(dtprev,dtavanc,1,0);
 if (strlen(FNAVANC)==0)
  {
   DMAX = NB1+1;
   sprintf (DATE_AVANC_1,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
   an_1=AnPrev; mois_1=MoisPrev; jour_1=JourPrev;
   return;
  }
 cnt=avact->RowCount;
 cntasv = asAvanc->RowCount;

 add_jour(AnPrev,MoisPrev,JourPrev,0);
 sprintf (dtprev,"%02d-%02d-%02d",Jour,Mois,An%100);
 //sprintf (dtavanc,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);
 sprintf (dtavanc,"%02d-%02d-%02d",jour_1,mois_1,an_1%100);

 rc = comp_dist_dates(dtprev,dtavanc,1,0);
 if (NB1-deb <0)  {Jour = JourPrev; Mois=MoisPrev; An=AnPrev;}
 else add_jour(AnPrev,MoisPrev,JourPrev,NB1-deb);

 sprintf (dtdeb,"%02d-%02d-%02d",Jour,Mois,An%100);

for(i=1;i<cnt;i++)
   {
    strcpy(snumt,avact->Cells[1][i].c_str());
    numt = atoi(snumt);
    if (numt==from) indx=i;
   }


// Ouvrir avant derniere actualisation dans FileAvanc

 



  // rc = avanc->OpenEngine(FNAVANC,120,"act","don");
  rc = avanc->OpenEngine(FileNameAvanc,120,"act","don");
  rc = avanc->SetIndexMode(0,0);
  int nbk = avanc->NumberOfKeys(0);


NBCourb=0;
 for(i=indx;i<cnt;i++)
   {
    if (i>=NBCells) break;
    strcpy(snumt,avact->Cells[1][i].c_str());
    numt = atoi(snumt);
    avtask[i]=numt;
    inda=0;
    for (j=1;j<cntasv;j++)
      {
       strcpy(snumt,asAvanc->Cells[1][j].c_str());
       numa = atoi(snumt);
       if (numa==numt)
          { //  on prend les dates col 8,10, 3,5

           sprintf(Key,"T%04d",numa);
           rc=avanc->ReadDirectKey(0,Key,RetKey,&recdata);
           if (rc)
              {
               rc = avanc->ReadRecord(buf,recdata);
               Ghost->ExtractValue(ddr,buf,"ddr",0);
               Ghost->ExtractValue(dfr,buf,"dfr",0);
              }

           strcpy(dt1,asAvanc->Cells[8][j].c_str());
           //res = comp_dist_dates(dtavanc,dt1,1,0);
           res = comp_dist_dates(dtdeb,dt1,1,0);
           ddprev[i]=Convert_Date(AnsiString(dt1));
           //��� ddprev[i] = NB3;   // NB3=NB1
           if (NB3>DMAX) DMAX=NB3;
           strcpy(dt1,asAvanc->Cells[10][j].c_str());
           res = comp_dist_dates(dtdeb,dt1,1,0);

           dfprev[i]=Convert_Date(AnsiString(dt1))+1;
           // dfprev[i] = NB3+1;
           if (NB3>DMAX) DMAX=NB3+1;

           strcpy(dt1,asAvanc->Cells[3][j].c_str());
           res = comp_dist_dates(dtavanc,dt1,1,0);
           ddreel[i]=Convert_Date(AnsiString(dt1));
           //ddreel[i] = NB3;
           if (NB3>DMAX) DMAX=NB3;
           strcpy(dt1,asAvanc->Cells[5][j].c_str());
           res = comp_dist_dates(dtavanc,dt1,1,0);

           dfreel[i]=Convert_Date(AnsiString(dt1))+1;
           //dfreel[i] = NB3+1;
           if (NB3>DMAX) DMAX=NB3+1;
           NBCourb++;
          }
      }
   }

 avanc->CloseEngine();
 // delete avanc;
}



void __fastcall TplStandard::avasCanEditCell(TObject *Sender, int ARow,
      int ACol, bool &CanEdit)
{
 CanEdit=false;
 if (ACol==3 || ACol==4 || ACol==7) CanEdit = true;   // || ACol==5
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::avasCellValidate(TObject *Sender, int ACol,
      int ARow, AnsiString &Value, bool &Valid)
{
 char tmp[200],dt1[20],dt2[20];
 int ch,val;  char str[200];
 int xcal,rc,result,cnt,i,numavanc,numt;
 char snumt[50];
 float x;  bool valid;

 Valid = false;
 strcpy(tmp,Value.c_str());

 ch=29;
 if (ACol==3) ch=29;
 if (ACol==4) ch=28;
 // if (ACol==5) ch= 23; // Duree reelle
 if (ACol==7) ch=57;  // Fin Reelle

 strcpy(snumt,avas->Cells[1][1].c_str());
  numt = atoi(snumt);
  NUMT = numt;
  if (numt==0) {Beep(); return; }

 cnt = asAvanc->RowCount;
 row_avancement = 0;
 for (i=1;i<cnt;i++)
  {
   strcpy(snumt,asAvanc->Cells[1][i].c_str());
   numavanc = atoi(snumt);
   if (numavanc == numt) row_avancement = i;
  }

  // ch=29;

 // Modifify asAvanc  before
    if (ch==29) { asAvanc->Cells[18][row_avancement]=AnsiString(tmp);AdvStringGrid6->Cells[18][row_avancement]=AnsiString(tmp);  av->Cells[13][numt]=""; }
    if (ch==28) { asAvanc->Cells[19][row_avancement]=AnsiString(tmp);AdvStringGrid6->Cells[19][row_avancement]=AnsiString(tmp); av->Cells[13][numt]=""; }
    if (ch==57) {
                  asAvanc->Cells[5][row_avancement]=AnsiString(tmp);
                  AdvStringGrid6->Cells[5][row_avancement]=AnsiString(tmp);
                  av->Cells[13][numt]=avas->Cells[7][1];  // Fin souhait�e

                }
    // if (ch==23) asAvanc->Cells[4][row_avancement]=AnsiString(tmp);


 ClickasAvanc(); // to fill AdvStringGrid4

 strcpy(adv,asAvanc->Cells[14][row_avancement].c_str());  // Calendrier
 xcal=atoi(adv); if (xcal < 1) xcal = 1;
 // strcpy(tmp, av->Cells[ACol][numt].c_str());
 valid = Call_Process(ch,xcal,tmp);    // tmp est la valeur du champ cliqu�

 if (valid==true)
   {
    ORIGINE_CLICK=2;
    CH=ch;
    Timer2->Enabled=true;
    // Propage(ch);
  }



 strcpy(tmp,avas->Cells[ACol][1].c_str());
 getvalue(5,row_avancement,62); // Calendrier
 xcal=atoi(adv); if (xcal < 1) xcal = 1;

 valid = Call_Process(ch,xcal,tmp);

 Valid = valid;

Graphics::TBitmap *  imP2 = new Graphics::TBitmap();
imP2->LoadFromFile("C:\\Users\\Claude\\Desktop\\demo\\cap2.bmp");
imP2->Canvas->StretchDraw(Rect(0,0,Image2->Width, Image2->Height),imP2);
imP2->Width = Image2->Width;
imP2->Height = Image2->Height;

Image2->Picture->Assign(imP2);
delete imP2;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btSaveActuClick(TObject *Sender)
{
 FillListBoxA();
 updatedatabase(LocalBase,0,0);
 Save_Actualisation(true);
 ReloadAllData(LocalBase,true,false);  // xxxxx
 /// Load_Avanc(FileNameAvanc,false);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btEraseCritClick(TObject *Sender)
{
 char tmp[250];
 int i,cnt;

 strcpy(tmp,"Cette op�ration Efface tous les Crit�res affect�s aux T�ches\n");
 strcat(tmp,"Voulez-Vous Continuer ? ");
 if (Application->MessageBoxA(tmp,m_ecoplan,MB_YESNO)==IDNO) return;

 strcpy(tmp,"Confirmez la remise � blanc de tous les crit�res affect�s aux t�ches\n");
 strcat(tmp,"Voulez-Vous Continuer ? ");
 if (Application->MessageBoxA(tmp,m_ecoplan,MB_YESNO)==IDNO) return;

 cnt=av->RowCount;
 for (i=1;i<cnt;i++)
   {
    av->Cells[41][i]="";
    av->Cells[42][i]="";
    av->Cells[43][i]="";
    av->Cells[44][i]="";
    av->Cells[45][i]="";
    av->Cells[46][i]="";
    av->Cells[47][i]="";
    av->Cells[48][i]="";
    av->Cells[51][i]="";
    av->Cells[52][i]="";
    av->Cells[53][i]="";
    av->Cells[54][i]="";
    av->Cells[55][i]="";
    av->Cells[56][i]="";
    av->Cells[57][i]="";
    av->Cells[58][i]="";
   }

 TabSheet5Show(Sender);
 //Regen_Criteres();
 //CopyCriteres();
 Change1();

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btFilesizeClick(TObject *Sender)
{
  char fname[250]; char tmp[200];
  FILE *fp;  int handle;
 // FileSize
 OpenDialog1->Title = "Calcul taille fichier"; //"New Project Name ";  //"Create New Folder";

 OpenDialog1->FileName = "*.txt";
 //OpenDialog1->InitialDir = AnsiString(HomeDirectory); // "c:\\";
 if (OpenDialog1->Execute())
  {
    strcpy(fname,OpenDialog1->FileName.c_str());
    handle = open(fname, O_RDWR);



    //fp = fopen(fname,"rb");
    sprintf(tmp,"File Size = %ld",filelength(handle));
    Application->MessageBoxA(tmp,"Utils",MB_OK);

    close(handle);
   }


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btCrit1Click(TObject *Sender)
{
 int i,cnt,nt;  char tmp[200];
 if (Application->MessageBoxA("Confirmez la remise � blanc du 1er Crit�re sur les t�ches Coch�es",m_ecoplan,MB_YESNO)==IDNO) return;
 cnt = asCrit->RowCount;
 for (i=1;i<cnt;i++)
   {
    if (asCrit->Cells[0][i] != "")
      {
       asCrit->Cells[3][i] = "";
       asCrit->Cells[4][i] = "";
       asCrit->Cells[0][i] = "";
       strcpy(tmp,asCrit->Cells[1][i].c_str());
       nt = atoi(tmp);
       if (nt >0)
         {
          av->Cells[41][nt]= "";
          av->Cells[51][nt]= "";
         }
      }
   }


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btCrit2Click(TObject *Sender)
{
int i,cnt,nt;  char tmp[200];
if (Application->MessageBoxA("Confirmez la remise � blanc du 2�me Crit�re sur les t�ches Coch�es",m_ecoplan,MB_YESNO)==IDNO) return;
 cnt = asCrit->RowCount;
 for (i=1;i<cnt;i++)
   {
    if (asCrit->Cells[0][i] != "")
      {
       asCrit->Cells[5][i] = "";
       asCrit->Cells[6][i] = "";
       asCrit->Cells[0][i] = "";
       strcpy(tmp,asCrit->Cells[1][i].c_str());
       nt = atoi(tmp);
       if (nt >0)
         {
          av->Cells[42][nt]= "";
          av->Cells[52][nt]= "";
         }
      }
   }

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btCrit3Click(TObject *Sender)
{
int i,cnt,nt;  char tmp[200];
if (Application->MessageBoxA("Confirmez la remise � blanc du 3�me Crit�re sur les t�ches Coch�es",m_ecoplan,MB_YESNO)==IDNO) return;
 cnt = asCrit->RowCount;
 for (i=1;i<cnt;i++)
   {
    if (asCrit->Cells[0][i] != "")
      {
       asCrit->Cells[7][i] = "";
       asCrit->Cells[8][i] = "";
       asCrit->Cells[0][i] = "";
       strcpy(tmp,asCrit->Cells[1][i].c_str());
       nt = atoi(tmp);
       if (nt >0)
         {
          av->Cells[43][nt]= "";
          av->Cells[53][nt]= "";
         }
      }
   }

}
//---------------------------------------------------------------------------



void __fastcall TplStandard::cbAvantChange(TObject *Sender)
{
 btDPClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::cbApresChange(TObject *Sender)
{
btDPClick(Sender);
}
//---------------------------------------------------------------------------

int  __fastcall TplStandard::Deduire_Date(char *dt1, char *dtresult, int nb, bool jnt)
{
 // Format dt1  jj-mm-aa    dtresult jj mm aa      nb + ou -
 /*
 add_jour(AnPrev,MoisPrev,JourPrev,duree[numt]);
       sprintf (dt1,"%02d-%02d-%02d",Jour,Mois,An%100);
 */
}

void __fastcall TplStandard::Copy_Calendar(int zcal,int marge)
{
 // ACAL
 char *p; int lg;  int i,ctr;
 /*
 strcpy(ACAL,cal[zcal]);
 return;
 */

 strcpy(ACAL,cal[zcal]);
 return;

 /*
 // marge = 35; // multiple de 7
 p=ACAL+marge;
 lg = strlen(cal[zcal]);
 strcpy(p,cal[zcal]);
 if (marge==0) return;
 ctr=6;
 for (i=marge-1;i>=0;i--)
   {
    if (i<0) break;
    ACAL[i] = *(p+ctr);
    ctr--;
    if (ctr<0) ctr= 6;
   }
// ACAL[0]='X';
//  sub_jour(2007,7,16,12);
*/

}


void __fastcall TplStandard::ZoomChange(TObject *Sender)
{
  ZoomCopyAv(imPort,0,0);
}
//---------------------------------------------------------------------------



void __fastcall TplStandard::ZoomCopyAv(TImage *i,int X,int Y)
{
TRect Dst,Src;
int imx; int imy;
int ext;

if (!im) return;
imx=im->Width;
imy=im->Height;
///   imPort is assigned to Image im !!!!!


ext = (11-Zoom->Position)*100; //100;
rx1= (imx * X / i->Width) -ext;
if (rx1 <0) rx1=0;
rx2 = rx1+2*ext;
if (rx2 > im->Width) {rx2=im->Width; rx1=rx2-2*ext; }
ry1= (imy * Y / i->Height) - ext;
if (ry1 <0) ry1=0;
ry2 = ry1+2*ext;
if (ry2 > im->Height) {ry2=im->Height; ry1=ry2-2*ext; }
LAST_X1 = rx1;
LAST_X2 = rx2;
LAST_Y1 = ry1;
LAST_Y2 = ry2;
LAST_TYP = 'P';  // portrait

LAST_ZOM = Zoom->Position;

Dst = Rect(0,0,imAvanc->Width,imAvanc->Height);
Src = Rect(rx1,ry1,rx2,ry2);
imAvanc->Canvas->CopyRect(Dst,imPort->Canvas,Src);
}



void __fastcall TplStandard::btPrintPDFAVClick(TObject *Sender)
{
 int i;


 CoefFormat=ComputeFormat(edAjust);
// ImprimeDirect(2,imP1p,imP1l);
 //ImprimeDirect(2,imP1p,imP1l);

 for (i=0;i<CntAVBefore;i++)
   Image_Avanc("Avancement",TableDatesAV[i],i);

 Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
 Graph_Avanc(AdvStringGrid6->RowCount-1,MAXDIST);
 SelectPrinterAA(1,1,imPort,imLand);
}
//---------------------------------------------------------------------------



/*
void __fastcall TplStandard::btActRapidClick(TObject *Sender)
{
 int i,cnt,cntav;   int ch;
 int numavanc,numt; int j; char snumt[50];
 cnt = avact->RowCount;
 cntav = asAvanc->RowCount;
 concern = !concern;
 for (i=1;i<cnt;i++)
   {
    strcpy(snumt,avact->Cells[1][i].c_str());
    numt=atoi(snumt);

    row_avancement = 0;
    for (j=1;j<cntav;j++)
    {
      strcpy(snumt,asAvanc->Cells[1][j].c_str());
      numavanc = atoi(snumt);
      if (numavanc == numt) row_avancement = j;
     }


  //   ClickasAvanc();

 if (avact->Cells[3][i]=="X" || avact->Cells[4][i]=="X")
  {
    if (row_avancement != 0)
      {
        asAvanc->Cells[20][row_avancement]="X";
		AdvStringGrid6->Cells[20][row_avancement]="X";
        ClickasAvanc();
        ch=29;
        CH=ch;
        Propage(ch);
       }
    else // row_av = 0  ; Pas normal
       {
        ch=ch;
       }
   }
} // for

 Refresh_Avanc(concern,1);
 btDPClick(NULL);

}
*/


void __fastcall TplStandard::btActRapidClick(TObject *Sender)
{
 int i,cnt,cntav;   int ch;
 int numavanc,numt; int j; char snumt[50];
 cnt = avact->RowCount;
 cntav = asAvanc->RowCount;

 // concern=true;

 concern = !concern;

 for (i=1;i<cnt;i++)
   {
    // calculer distance par rapport a data avancement

    strcpy(snumt,avact->Cells[1][i].c_str());
    numt=atoi(snumt);
    row_avancement = 0;
    for (j=1;j<cntav;j++)
    {
      strcpy(snumt,asAvanc->Cells[1][j].c_str());
      numavanc = atoi(snumt);
      if (numavanc == numt) row_avancement = j;
     }


   } // for


 STRICT=!STRICT;

 Refresh_Concern(STRICT);
 btDPClick(NULL);
}



//---------------------------------------------------------------------------

void __fastcall TplStandard::Button3Click(TObject *Sender)
{
  int i,j,m,n;  int task,succ;
  char tmp[200],xtmp[200]; bool ok;
  int cnt;  int resp;
 Organise_Niveaux();


// rechercher si toutes les taches sont pr�sentes

for (m=1;m<=LASTTASK;m++)
 {
  if (av->Cells[1][m] != "")
    {
     strcpy(xtmp,av->Cells[1][m].c_str());
     task = atoi(xtmp);
     ok = false;
     for (i=0;i<iniv;i++)
        {
         for (j=0;j<NN;j++)
           {
            succ = niveaux[i][j];
            if (succ==0) break;
            if (succ==task)
              {
               ok=true;
              }

           }
        }
    if (!ok)
           {
            sprintf(tmp,"Bouclage problable impliquant la tache %d",task);
            Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
            resp = Recherche_Responsable(task);
            if (resp != 0)
              {
               sprintf(tmp,"Tache responsable:  %d",resp);
               Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
               return;
              }
            return;
           }

    }


 }

Application->MessageBoxA("Pas de bouclage d�tect�",m_ecoplan,MB_OK);

}


int  __fastcall TplStandard::Recherche_Responsable(int t)
{
 // recherche quelle tache a le successeur t;
 int numt,i,cnt,result;  int isucc,tsucc;
 char tmp[50];

 result = 0;
 cnt = av->RowCount;



 for (i=1;i<cnt;i++)
   {
    strcpy(tmp,av->Cells[1][i].c_str());
    numt=atoi(tmp);
    if (numt>=190)
      {
       numt=numt;
      }
    if (numt !=0)
      {
       isucc=1;
       while (su[numt][isucc]!=0 )
        { tsucc=su[numt][isucc];
          if (tsucc==t)
            {
             return numt;
            }
          isucc++;
        }
      }
   }
}

//---------------------------------------------------------------------------



void __fastcall TplStandard::Button17Click(TObject *Sender)
{
Exec_Export_Excel(false);
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::BitBtn1Click(TObject *Sender)
{
int XRow;
 int i,count;
 AnsiString L[80];
 if (SG1Row==0)
   {Application->MessageBoxA("Pas de ligne s�lectionn�e",m_ecoplan,MB_OK);
    return;
   }
 count=AdvStringGrid1->RowCount-1;
 if(SG1Row == 1) return;

 for (i=0;i<AdvStringGrid1->ColCount;i++)  L[i] = AdvStringGrid1->Cells[i][SG1Row];

 XRow=SG1Row-1;
 for (i=0;i<AdvStringGrid1->ColCount; i++) { AdvStringGrid1->Cells[i][SG1Row]= AdvStringGrid1->Cells[i][XRow];
  AdvStringGrid1->Cells[i][XRow] = L[i];
  }

 SG1Row=XRow;
 refresh_marges();


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::BitBtn2Click(TObject *Sender)
{
int XRow;
 int i,indx,count;

 AnsiString L[80];

 if (SG1Row==0)
   {Application->MessageBoxA("Pas de ligne s�lectionn�e",m_ecoplan,MB_OK);
    return;
   }
 count = AdvStringGrid1->RowCount-1;
 if(SG1Row == count) return;
 if (SG1Row ==0) return;

 for (i=0;i<AdvStringGrid1->ColCount;i++)  L[i] = AdvStringGrid1->Cells[i][SG1Row];

 XRow=SG1Row+1;
 for (i=0; i<AdvStringGrid1->ColCount; i++) { AdvStringGrid1->Cells[i][SG1Row]= AdvStringGrid1->Cells[i][XRow];
  AdvStringGrid1->Cells[i][XRow] = L[i];
  }

 SG1Row=XRow;
 refresh_marges();
}
//---------------------------------------------------------------------------



void __fastcall TplStandard::btBclClick(TObject *Sender)
{
 int i,j,m,n;  int task,succ;
  char tmp[200],xtmp[200]; bool ok;
  int cnt;  int resp;
 Organise_Niveaux();


// rechercher si toutes les taches sont pr�sentes

grosbuf[0]=0;
cnt=0;
for (m=1;m<=LASTTASK;m++)
 {
  if (av->Cells[1][m] != "")
    {
     strcpy(xtmp,av->Cells[1][m].c_str());
     task = atoi(xtmp);
     ok = false;
     for (i=0;i<iniv;i++)
        {
         for (j=0;j<NN;j++)
           {
            succ = niveaux[i][j];
            if (succ==0) break;
            if (succ==task)
              {
               ok=true;
              }

           }
        }
    if (!ok)
           {

            resp = Recherche_Responsable(task);
            // if (resp==1) resp=0;
            // if (resp<task) resp=0;

            if (resp != 0)
            //if (resp >= task || resp==task)
              {
               cnt++;
               sprintf(tmp,"Tache responsable:  %d, qui appelle la tache %d comme successeur",resp,task);
               Application->MessageBoxA(tmp,m_ecoplan,MB_OK);
                return;
              }

           }

    }


 }
if (Display_Bcl)
Application->MessageBoxA("Pas de bouclage d�tect�",m_ecoplan,MB_OK);


}
//---------------------------------------------------------------------------

// Predecesseurs  RENUM
 /*   INUTILE
  for (i=1;i<cnt;i++)
  {

   strcpy(buf,av->Cells[62][i].c_str()); strcpy(buf1,"|"); p1 = strchr(buf,'|');
   while(p1)
    { p=strchr(p1+1,'|');     // struct is  nn,tl,decal,|
      if (p)
      { *p=0;
        strcpy(tmp,p1+1); t=atoi(tmp); n=Posit(t); //renum[t];
        sprintf(tmp,"%d|",n); strcat(buf1,tmp); p1=p;
        av->Cells[62][i]=AnsiString(buf1);                   // $$$$$$$$$ PB
      }
     else break;
    }
  }  // for i

 // Idem pour successeurs;

 for (i=1;i<cnt;i++)
  {
   // les successeurs a changer
   strcpy(buf,av->Cells[64][i].c_str()); strcpy(buf1,"|"); p1 = strchr(buf,'|');
   while(p1)
    { p=strchr(p1+1,'|');
      if (p)
      { *p=0; strcpy(tmp,p1+1); t=atoi(tmp); if (t==9999) n=9999; else n=Posit(t); //renum[t];
        sprintf(tmp,"%d|",n); strcat(buf1,tmp); p1=p;   //  $$$$$$$$$ PB
        av->Cells[64][i]=AnsiString(buf1);
      }
     else break;
    }
  }  // for i

 */

void __fastcall TplStandard::btPrepAvancClick(TObject *Sender)
{
 char x_ghost_prep[500];

 pc=3;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning3</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
 TPrepPlanning *PrepPlanning;
 pc=3;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 627;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 delete PrepPlanning;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::TabSheet3Show(TObject *Sender)
{
char tmp[250];
// concern=true;

/*
Save_Actualisation(false);
Load_Avanc(FileNameAvanc,concern);
*/

Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
Graph_Avanc(AdvStringGrid6->RowCount-1,MAXDIST);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button18Click(TObject *Sender)
{
// concern=false;
// Load_Avanc(FileNameAvanc,concern);
//Compute_DD1_DD2();
Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
Graph_Avanc(AdvStringGrid6->RowCount-1,MAXDIST);
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::cbLBClick(TObject *Sender)
{
Button18Click(Sender);
}
//---------------------------------------------------------------------------



void __fastcall TplStandard::btPrepActClick(TObject *Sender)
{
 char x_ghost_prep[500];

 pc=3;
 strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
 strcat(x_ghost_prep,"<org>plStd:RetPlanning1</org>");
 m_ghost_prep[pc] = AnsiString(x_ghost_prep);
 TPrepPlanning *PrepPlanning;
 pc=3;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 735;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 delete PrepPlanning;
 
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::imP3pMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if (P_MODE[pc]=='L') { P_MODE[pc]='P'; RAZ(); Prepare_Draw_Planning(); }
 ZoomCopy3(imP3p,X,Y);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btLBRISEEClick(TObject *Sender)
{

 TLBrisee *lbris;
 char tmp[1000];

 lbris = new TLBrisee(Application);
 // preparer chaine des dates (dateavanc ? )
 //  strcpy(tmp,dateavanc);

 lbris->ShowModal();
 delete lbris;


  if (strlen(P_NOM[3])==0)
   {
    Clear_Diagram(imP3p);
    Clear_Diagram(imP3Screen);
    return;
   }


 pc=3;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);

 // Remplir_asTri();
 Exec_Planning(tmp,true);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button21Click(TObject *Sender)
{
CoefFormat=ComputeFormat(edAjust);
Prepare_Draw_Planning();
ImprimeDirect(1,imP3p,imP3p);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button22Click(TObject *Sender)
{
  CoefFormat = ComputeFormat(edAjust);
  Prepare_Draw_Planning();
  SelectPrinter(1,1,imP3p,imP3p);


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button23Click(TObject *Sender)
{
 TabSheet3->TabVisible = false;
 if (Origin_Actu==0) PageControl1->ActivePageIndex = 7;
 else PageControl1->ActivePageIndex = 13;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button24Click(TObject *Sender)
{
 Origin_Actu=1;
 TabSheet3->TabVisible= true;
 Button23->Caption = "Revenir Actualisation Rapide";
 PageControl1->ActivePageIndex = 8;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button25Click(TObject *Sender)
{
Origin_Actu=0;
TabSheet3->TabVisible= true;
Button23->Caption = "Revenir Actualisation";
PageControl1->ActivePageIndex = 8;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::cbAvantXChange(TObject *Sender)
{
char tmp[250];

// concern=true;

// Save_Actualisation(false);
// Load_Avanc(FileNameAvanc,concern);

//Compute_DD1_DD2();
Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
Label53->Caption = Label50->Caption;
Label50->Caption = Label50->Caption;
Label87->Caption = Label50->Caption;
Label68->Caption = Label12->Caption;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::cbApresXChange(TObject *Sender)
{
char tmp[250];

// concern=true;

// Save_Actualisation(false);
// Load_Avanc(FileNameAvanc,concern);
//Compute_DD1_DD2();
Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
Label53->Caption = Label50->Caption;
Label50->Caption = Label50->Caption;
Label87->Caption = Label50->Caption;

Label68->Caption = Label12->Caption;        
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------


void __fastcall TplStandard::asAvancSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
 char tmp[500];  int task;
 CanSelect=true;
 if (ARow==0) return;
 row_avancement = ARow;
 ClickasAvanc(); // row_avancement);

 strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
 task = atoi(tmp);
 DrawMiniGantt(task,0,immGnt,true);
 DrawMiniGantt(task,0,Image1,true);

}
//---------------------------------------------------------------------------

/*
void __fastcall TplStandard::btRAZClick(TObject *Sender)
{

}
*/


/*
void __fastcall TplStandard::btRAZClick(TObject *Sender)
{
 AnsiString FDir; char dir[MAX_PATH];
 int nlast,done,maxdist,maxd;
 struct ffblk ffblk;  char tmp[500];
  char APrev[20],MPrev[20],JPrev[20];;
 int aprev,mprev,jprev; char mess[200];
 char *p;  char fmt[100]; int nb,n;
 char A[10],M[10],J[10],N[10]; char Suffixe[50]; char lastsuffix[50];
 char lastfile[MAX_PATH];  int a,m,j; int dist,distprev;
 int adate,atime;  int i;   AnsiString Res;
 int year,month,day,hour,minute,second;
 char buffer[5000]; char numtt[50]; int ntt;
 char nameavanc[MAX_PATH];
 int rc, recdata; char Key[200],RetKey[200];
 char NEWPREV[100]; int DISTPREV,PREMPREV;

 char tmp1[50];

 // Affectation des dates reelles au pr�visionnel
 if (strlen(ProjectName)==0) return;
 if (strlen(DatabaseName)==0) return;
 if (Application->MessageBoxA("Confirmez le remplacement des dates pr�visonnelles par les dates du dernier avancement",
     m_ecoplan,MB_YESNO) == IDNO) return;

 // fermeture des fichiers avancements et du fichier previsionnel


 sprintf(tmp,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
 // strcpy(tmp,m_dateprev);  // format JJ-MM-AA
 // compute dist previsionnelle
 p=tmp; strcpy(JPrev,p); JPrev[2]=0;  jprev=atoi(JPrev);
 p=p+3; strcpy(MPrev,p); MPrev[2]=0;  mprev=atoi(MPrev);
 p=p+3; strcpy(APrev,p); APrev[2]=0;  aprev=atoi(APrev);
 distprev = aprev*365 + mprev*31 + jprev;

 FDir = ExtractFileDir(DatabaseName);
 strcpy(dir,FDir.c_str()); strcat(dir,"\\*.act");
 nlast=0; strcpy(lastsuffix,"");
   nb=0;

 nlast = 0; maxdist=0; maxd=0;
 done = findfirst(dir,&ffblk,0);
   while (!done)
   {
     strcpy(tmp,ffblk.ff_name);
     if (strstr(tmp,".act") && strstr(tmp,ProjectName) && !strstr(tmp,"Copie"))
       {
        //prepare dateavanc with existing files
        p=strstr(tmp,".act");
        if (p)
           {
            A[0] = *(p-8); A[1]= *(p-7); A[2]=0; a=atoi(A)%100;
            M[0] = *(p-5); M[1]= *(p-4); M[2]=0;   m=atoi(M);
            J[0] = *(p-2); J[1]= *(p-1); J[2]=0;   j=atoi(J);
            dist = a*365 + m*31 + j;

            if (dist <= distprev)
               {

               }

            else
              {
               nb++;
               Suffixe[0]=*(p-3); Suffixe[1]=*(p-2); Suffixe[2]=*(p-1); Suffixe[3]= 0;
               sprintf(fmt,"%02d-%02d-%02d-%s",a,m,j,Suffixe);


               sprintf(fmt,"%02d-%02d-%02d",a,m,j);

               strcpy(lastfile,fmt);
               strcpy(lastsuffix,"");
               // Application->MessageBox(fmt,"FMT",MB_OK);

               //n= atoi(Suffixe);
               // if (n>nlast) { nlast=n;  strcpy(lastfile,tmp); strcpy(lastsuffix,fmt);}

              }
           }


       }
      done = findnext(&ffblk);
   }

 if (nb==0) {Application->MessageBoxA("Pas de fichier actualisation valide trouv�",m_ecoplan,MB_OK);
             return; }

 // fichier actualisation in lastfile.

 strcpy(nameavanc,DatabaseName); strcat(nameavanc,"-");
 strcat(nameavanc,lastfile);

Application->MessageBox(nameavanc,"REMISE A ZERO . NAMEAVANC",MB_OK);


 avanc = new  realisam();    // ne pas enlever, sinon erreur
  rc = avanc->OpenEngine(nameavanc,120,"act","don");   //512 - 8
  rc = avanc->SetIndexMode(0,0);
  PREMPREV = 10000;
 // int nbk = avanc->NumberOfKeys(0);
   rc = avanc->ReadFirstKey(0,RetKey,&recdata);
    while (rc)
   {
      rc=avanc->ReadRecord(buffer,recdata);
      buffer[rc]=0;
      Ghost->ExtractValue(tmp,buffer,"nt",0);
      ntt=atoi(tmp);
      if (ntt==0)
        {
         //Application->MessageBox(tmp,"NO TACHE",MB_OK);
         continue;

        }

      Ghost->ExtractValue(tmp,buffer,"ddr",0);

      Application->MessageBox(buffer,"RAZ ... BUFFER APRES DDR",MB_OK);

      av->Cells[4][ntt]=AnsiString(tmp); // date deb previsionnel

      // av->Cells[12][ntt]=AnsiString(tmp);   // d�but souhaite


      DISTPREV=Convert_Date(AnsiString(tmp));

      Application->MessageBox(tmp,"CONV DATE",MB_OK);
      if (DISTPREV < PREMPREV)
         {
          PREMPREV=DISTPREV;

         }
      p=strstr(buffer,"<ddp>");
      if (p) { p=p+5;  strncpy(p,tmp,8); }

      Ghost->ExtractValue(tmp,buffer,"dfr",0);
      av->Cells[5][ntt]=AnsiString(tmp);
      p=strstr(buffer,"<dfp>");
      if (p) { p=p+5;  strncpy(p,tmp,8); }
      rc=avanc->RewriteRecord(buffer,strlen(buffer)+1,recdata);
      rc = avanc->ReadNextKey(RetKey,&recdata);
   }

 avanc->CloseEngine();
 */

 /*
 strcpy(NEWPREV,lastfile);
 Application->MessageBox(NEWPREV,"RAZ ... NEWPREV",MB_OK);

 strcpy(tmp1,NEWPREV);

 tmp1[0] = NEWPREV[6];
 tmp1[1] = NEWPREV[7];

 tmp1[3] = NEWPREV[3];
 tmp1[4] = NEWPREV[4];

 tmp1[6] = '2';
 tmp1[7] = '0';

 tmp1[8] = NEWPREV[0];
 tmp1[9] = NEWPREV[1];

 tmp1[2] = tmp1[5] = '-';
 */


 /*
 tmp1[8]=NEWPREV[6];
 tmp1[9]=NEWPREV[7];
 tmp1[6]='2'; tmp1[7]='0';
 */
 // tmp1[10]=0;
//  strcpy(m_dateprev,tmp);

 // Application->MessageBox(tmp1,"tmp1 date ",MB_OK);

 // ModifDatePrev(tmp1);


 ///Modif_All_Links();
 // xxxxxx
 ///Save();
 ///ReloadAllData(LocalBase,true,false);
 
// }
//---------------------------------------------------------------------------


void __fastcall TplStandard::Modif_All_Links()
{
 int cnt,indx; int ipred,tpred; int decal;
 char dfinpred[20],ddeb[20];  int cal;  int distmin;
 char tmp[50];  int pred_select; int cnt_asav,tach_avanc; char *p;
 // colonne 0 a vide
 char JDEB[10]; char MDEB[10]; char ADEB[10];  int dist, jdeb,mdeb, adeb;


 cnt_asav = asAvanc->RowCount;
 sprintf(tmp,"cnt_asavanc %d", cnt_asav);
 Application->MessageBox(tmp,"Modif all links",MB_OK);
 for (indx=1;indx<cnt_asav;indx++)
   {

    pred_select=0;
    if (asAvanc->Cells[24][indx] == "X")
      {
       strcpy(tmp,asAvanc->Cells[1][indx].c_str());
       tach_avanc = atoi(tmp);
       sprintf(tmp,"Tache %d  Recherche pr�d�cesseurs ",tach_avanc);
       Application->MessageBox(tmp,"Ecopl",MB_OK);
       // boucle pour retrouver le meilleur pr�d�cesseur
       // ����� pred_select = Best_Pred(indx,tach_avanc);
       // ����� sprintf(tmp,"Best Pred %d", pred_select);
       // Application->MessageBoxA(tmp,"Ecopl ", MB_OK);
       // if (pred_select==0) continue;

       Effacer_Predecessurs(tach_avanc);

       // compute distance




      }



   }


 cnt = av->RowCount;
 for (indx=1;indx<cnt;indx++)  av->Cells[0][indx]="";
 for (indx=1;indx<cnt;indx++)
   {
    ipred = 1;
    tpred=pr[indx][ipred];

    //pred_contraignant = 0;

    while (pr[indx][ipred]!=0) //  && su[indx][isucc]!=9999)
       {
        tpred=pr[indx][ipred];
        strcpy(dfinpred,av->Cells[22][tpred].c_str());  // date de fin reelle de la tache predecesseur
        strcpy(ddeb,av->Cells[21][indx].c_str());
        strcpy(tmp,av->Cells[10][indx].c_str());   cal=atoi(tmp);  if (cal==0) cal = 1;
        comp_dist_dates(dfinpred,ddeb,cal,0);
        if (NB1<0) // decalage FD <0     valeur dans NB2
         {
          decal=-NB2;
         }
        else if (NB1==0)
         {
          decal=0;
         }
        else // NB > 0
         {
          decal=NB2;
         }
         //if (av->Cells[0][tpred] == "")
         //  {
         av->Cells[0][tpred]="X";
         InsertPredSeul(indx,tpred,0,decal,0);
         InsertSuccSeul(tpred,indx,0,decal,0);

        ipred++;
       } // fin while
   } // end FOR


}


void __fastcall TplStandard::Effacer_Predecessurs(int tach)
{

 int ipred; int tpred;   int pred_contraignant;  char dtdebposs[20];
 int cnt; int xxx,yyy; int dist; int distmin;  int row_pred; int i;
 char snumt[50];  int num_tache;  char JDEB[10],MDEB[10],ADEB[10];
 char *p;  char tmp[200]; int jdeb,mdeb,adeb;

    ipred = 1;
    tpred=pr[tach][ipred];

    while (pr[tach][ipred]!=0) //  && su[indx][isucc]!=9999)
       {
        if (pr[tach][ipred] == 1) continue;
        cnt++;
        tpred=pr[tach][ipred];
        RemovePredSeul(tach,tpred);
        sprintf(tmp,"Tache %d  Predecesseur effac� %d ",tach,tpred);
        Application->MessageBox(tmp,"Ecopl",MB_OK);

       }
}

int  __fastcall TplStandard::Best_Pred(int indx,int tach)
{
 int ipred; int tpred;   int pred_contraignant;  char dtdebposs[20];
 int cnt; int xxx,yyy; int dist; int distmin;  int row_pred; int i;
 char snumt[50];  int num_tache;  char JDEB[10],MDEB[10],ADEB[10];
 char *p;  char tmp[100]; int jdeb,mdeb,adeb;
  ipred = 1;
    tpred=pr[tach][ipred];

    pred_contraignant = 0;
    dist = 0;  distmin = 1000000;
    cnt = 0;   BEST_PRED=0;
    while (pr[tach][ipred]!=0) //  && su[indx][isucc]!=9999)
       {
        cnt++;
        tpred=pr[tach][ipred];


        xxx = asAvanc->RowCount;
        row_pred = 0;
        for (i=1;i<xxx;i++)
           {
            strcpy(snumt,asAvanc->Cells[1][i].c_str());
            num_tache = atoi(snumt);
            if (num_tache == tpred) row_pred = i;
           }


        if (row_pred >0)
           {
             strcpy(dtdebposs,asAvanc->Cells[17][row_pred].c_str());  // date de d�but possible
             strcpy(tmp,asAvanc->Cells[17][row_pred].c_str());  // date de d�but possible
             // Application->MessageBoxA(dtdebposs,"Date d�but possible du pr�decesseur",MB_OK);


        // sprintf(tmp,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
             p=tmp; strcpy(JDEB,p); JDEB[2]=0;  jdeb=atoi(JDEB);
             p=p+3; strcpy(MDEB,p); MDEB[2]=0;  mdeb=atoi(MDEB);
             p=p+3; strcpy(ADEB,p); ADEB[2]=0;  adeb=atoi(ADEB);
             dist = adeb*365 + mdeb*31 + jdeb;
             if (dist < distmin)  { distmin = dist; BEST_PRED=tpred; }

          }

         //if (av->Cells[0][tpred] == "")
         //  {
         // av->Cells[0][tpred]="X";
         //InsertPredSeul(indx,tpred,0,decal,0);
         // InsertSuccSeul(tpred,indx,0,decal,0);

        ipred++;
       } // fin while

  if (cnt==0)  return 0;
  sprintf(tmp,"Pr�decesseur avec la meilleure date d�but possible : %d",BEST_PRED);
  // Application->MessageBoxA(tmp,"Ecopl",MB_OK);
  return BEST_PRED;
}


void __fastcall TplStandard::btImportLiensClick(TObject *Sender)
{
 //Ouvrir fichier liens
 char fname[MAX_PATH];

 OpenDialog1->Title = "Importation Fichier Liens CSV"; //"New Project Name ";  //"Create New Folder";

 OpenDialog1->FileName = "*.csv";
 if (OpenDialog1->Execute())
  {
    strcpy(fname,OpenDialog1->FileName.c_str());


  }


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::exec_Pl(TObject *Sender)
{
 char tmp[500];
 /*
 pc=1;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);
 */
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::edFINChange(TObject *Sender)
{
 char tmp[500];

 /*
 pc=1;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);
 */
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button26Click(TObject *Sender)
{
  char tmp[500];  TPeriod *period;

  strcpy(m_exchange,"<dd>"); strcat(m_exchange,Label109->Caption.c_str()); strcat(m_exchange,"</dd>");
  strcat(m_exchange,"<df>"); strcat(m_exchange,Label117->Caption.c_str()); strcat(m_exchange,"</df>");

  period=new TPeriod(Application);
  period->ShowModal();
  delete period;

  if (strlen(m_exchange)==0) return;
   pc=1;
  Ghost->ExtractValue(edDEB[pc],m_exchange,"dd",0);
  Ghost->ExtractValue(edFIN[pc],m_exchange,"df",0);
  P_CJTFLAG[pc]="Yes";

 if (!Update_IX()) return;
 pc=1;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);

}
//---------------------------------------------------------------------------

bool __fastcall TplStandard::Update_IX()
{
  char tmp[50]; char yy[10],mm[10],dd[10];
 int y,m,d; int dist1,dist2;  int base;

 strcpy(tmp,edDEB[pc]); // ->Text.c_str());
 if (strlen(tmp)==8)
   {
    tmp[2]=0; tmp[5]=0;
    strcpy(yy,tmp+6); strcpy(mm,tmp+3); strcpy(dd,tmp);
    y=atoi(yy); m=atoi(mm); d=atoi(dd);
    dist1 = convert_date(y+2000,m,d);


    if (dist1==-1)
      {Beep();
       sprintf(tmp,"%02d/%02d/%02d",JourPrev,MoisPrev,AnPrev%100);
       strcpy(edDEB[pc],tmp);
       dist1=0;

      }
    if (pc==1) Label109->Caption = AnsiString(edDEB[pc]);
    else if (pc==3) Label134->Caption = AnsiString(edDEB[pc]);
   }

 else
   {
    Application->MessageBoxA("Format Date de D�but Incorrect (JJ/MM/AA)",m_ecoplan,MB_OK);
    return false;
   }

 strcpy(tmp,edFIN[pc]);  // ->Text.c_str());
 if (strlen(tmp)==8)
   {
    tmp[2]=0; tmp[5]=0;
    strcpy(yy,tmp+6); strcpy(mm,tmp+3); strcpy(dd,tmp);
    y=atoi(yy); m=atoi(mm); d=atoi(dd);
    dist2 = convert_date(y+2000,m,d);
    if (dist2==-1)
      {
       Beep();
       strcpy(edFIN[pc],Label44->Caption.c_str());
       strcpy(tmp,edFIN[pc]); // ->Text.c_str());
       strcpy(yy,tmp+6); strcpy(mm,tmp+3); strcpy(dd,tmp);
       y=atoi(yy); m=atoi(mm); d=atoi(dd);
       dist2 = convert_date(y+2000,m,d);
      }
    if (pc==1) Label117->Caption = AnsiString(edFIN[pc]);
      else if (pc==3) Label135->Caption = AnsiString(edFIN[pc]);
   }

 else
   {
    Application->MessageBoxA("Format Date de Fin Incorrect (JJ/MM/AA)",m_ecoplan,MB_OK);
    return false;
   }
 if (dist2<=dist1)
   {
    // Application->MessageBoxA("Date de fin ant�rieure � la date de d�but (bouclage probable)",m_ecoplan,MB_OK);
    return false;
   }
 IXDEB=dist1;
 IXFIN=dist2;

 
 IXDEB1= Compute_Trav(IXDEB,1);
 IXFIN1= Compute_Trav(IXFIN,1);
 //  IXFIN1 = IXFIN1*1.2;
 return true;
}

/*
void __fastcall TplStandard::Update_PCJT(int ixfin)
{
 int base;
 base = Compute_Base();
 if (ixfin-IXDEB !=0)
 P_CJT[pc] = AnsiString(base/(ixfin-IXDEB));
}
*/

void __fastcall TplStandard::TabSheet14Show(TObject *Sender)
{
 Color_Avas();
 avact->ColWidths[4]=0;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btChoixClick(TObject *Sender)
{
 char tmp[500];  TPeriod *period;

  strcpy(m_exchange,"<dd>"); strcat(m_exchange,Label134->Caption.c_str()); strcat(m_exchange,"</dd>");
  strcat(m_exchange,"<df>"); strcat(m_exchange,Label135->Caption.c_str()); strcat(m_exchange,"</df>");

  period=new TPeriod(Application);
  period->ShowModal();
  delete period;

  if (strlen(m_exchange)==0) return;

  pc=3;

  Ghost->ExtractValue(edDEB[pc],m_exchange,"dd",0);
  Ghost->ExtractValue(edFIN[pc],m_exchange,"df",0);
  P_CJTFLAG[pc]="Yes";

 if (!Update_IX()) return;
 pc=3;
 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::Button27Click(TObject *Sender)
{

Graph_Avanc(asAvanc->RowCount-1,MAXDIST);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::btPreviewClick(TObject *Sender)
{
 Call_Preview();
}

void __fastcall TplStandard::Call_Preview()
{
 TPreview *preview;
 preview = new TPreview(Application);

 preview->Width=Screen->Width-50;
 preview->Height=Screen->Height-50;
 preview->Top=10;
 preview->Left=10;

 preview->Image1->Width=preview->Width - preview->Image1->Left-50;
 preview->Image1->Height=preview->Height- preview->Image1->Top-50;

 if(model == 1)
 {
 }
 strcpy(m_exchange,"<or>Portrait</or>");

 preview->ShowModal();
 delete preview;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button28Click(TObject *Sender)
{
 Call_Preview();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button29Click(TObject *Sender)
{
 Call_Preview();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button30Click(TObject *Sender)
{
 Call_Preview();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button31Click(TObject *Sender)
{

PageControl1->ActivePageIndex=2;
Sleep(500);
btPreviewClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::rbGanttClick(TObject *Sender)
{
char tmp[1000];
 pc=0;
 strcpy(P_NOM[0],"Ordonnancement des t�ches en Gantt");
 strcpy(tmp,P_NOM[0]);
 // strcpy(current_planning,tmp);
 strcpy(LastPlanning[pc],tmp);
 // strcpy(P_NOM[pc],current_planning);
 Ghost->LoadPlanningFromFile(tmp);



 // Ghost->LoadPlanningFromFile(P_NOM[0]);

 //Ghost->Global_Planning(P_NOM[0]);

 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::rbComprClick(TObject *Sender)
{
char tmp[250];
 pc=0;
 strcpy(P_NOM[0],"Ordonnancement des t�ches en planning compress�");

 strcpy(tmp,P_NOM[0]);
 // strcpy(current_planning,tmp);
 strcpy(LastPlanning[pc],tmp);
 // strcpy(P_NOM[pc],current_planning);
 Ghost->LoadPlanningFromFile(tmp);



 // Ghost->LoadPlanningFromFile(P_NOM[0]);

// Ghost->Global_Planning(P_NOM[0]);

 sprintf(tmp,"<pl>%d</pl><tri>%s</tri><col>%s</col><filt>%s</filt>",pc,
 P_TRI[pc],P_COLONNES[pc],P_FILTRE[pc]);
 Exec_Planning(tmp,true);

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::rbCoutClick(TObject *Sender)
{
Cout_Ressources() ;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::rbRessClick(TObject *Sender)
{
 Cout_Ressources();
 }
//---------------------------------------------------------------------------


void __fastcall TplStandard::Cout_Ressources()
{

  if (rbCout->Checked == true)
   {

   }
 if(rbRess->Checked==true)
   {
    Label77->Caption = "Saisie des Ressources des T�ches";
    Label143->Caption = "Ech�ancier des Ressources de la S�lection";
    Button7->Caption = "Sauvegarde des Ressources";
    avc->Cells[6][0]="Ress. / Jour";
    avc->Cells[7][0]="Ress. Total";
   }
  Refresh_Couts();
  }
void __fastcall TplStandard::btCrit4Click(TObject *Sender)
{
int i,cnt,nt;  char tmp[200];
if (Application->MessageBoxA("Confirmez la remise � blanc du 4�me Crit�re sur les t�ches Coch�es",m_ecoplan,MB_YESNO)==IDNO) return;
 cnt = asCrit->RowCount;
 for (i=1;i<cnt;i++)
   {
    if (asCrit->Cells[0][i] != "")
      {
       asCrit->Cells[9][i] = "";
       asCrit->Cells[10][i] = "";
       asCrit->Cells[0][i] = "";
       strcpy(tmp,asCrit->Cells[1][i].c_str());
       nt = atoi(tmp);
       if (nt >0)
         {
          av->Cells[44][nt]= "";
          av->Cells[54][nt]= "";
         }
      }
   }
}
//---------------------------------------------------------------------------

bool __fastcall TplStandard::Open_Dump()
{
 AnsiString Today;
 Today = DateTimeToStr(Now());
 char tmp[500]; char fname[1000];
 strcpy(fname,m_directory); strcat (fname,"\\DUMP.TXT");
 dump = fopen(fname,"ab");
 if (dump==NULL)
    {
     Application->MessageBox("Erreur Ouverture Fichier DUMP.TXT",m_ecoplan,MB_OK);
     return false;
    }
 sprintf(tmp,"%s Heure ouverture Dump.txt\n",Today.c_str());
 fputs(tmp,dump);
}

void __fastcall TplStandard::Close_Dump()
{
 AnsiString Today;
 Today = DateTimeToStr(Now());
 char tmp[500];

 sprintf(tmp,"%s Heure de Fermeture DUMP.TXT\n",Today.c_str());
 fputs(tmp,dump);
 fclose(dump);
}



void __fastcall TplStandard::btRAZClick(TObject *Sender)
{
 int i,cntAv;  int totimp;
 int nt;   char tmp[100];


 if (Application->MessageBox("Voulez vous effectuer la remise � z�ro ? ","Ecopl",MB_OK)== IDNO) return;


strcpy(SAVEAS,"raz");
 SaveAs(false);

 cntAv=asAvanc->RowCount;
 for (i=1;i<cntAv;i++)
   {
    strcpy(tmp,asAvanc->Cells[1][i].c_str());
    nt = atoi(tmp);

    av->Cells[4][nt]=asAvanc->Cells[3][i];  // debut
    av->Cells[5][nt]=asAvanc->Cells[5][i];  //

    // comp_dist_dates(asAvanc->Cells[3]1,0);


    asAvanc->Cells[8][i]=asAvanc->Cells[3][i];
    asAvanc->Cells[10][i]=asAvanc->Cells[5][i];

    //Mise � jour du lien "FD" ou autre
    // comp_dist_dates(1,0);


    // cas marge n�gative
    strcpy(tmp,asAvanc->Cells[6][i].c_str());
    totimp = atoi(tmp);
    //if (totimp <0)
    //  {
       av->Cells[12][nt]=asAvanc->Cells[3][i];
       av->Cells[13][nt]="";
    //  }




   }





 Modif_All_Links();
 Save();
 Save_Actualisation(true);
 // Compute_PlusTotA();
 ReloadAllData(LocalBase,true,false);
 Load_Avanc(FileNameAvanc,false);
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::AdvStringGrid4ClickCell(TObject *Sender,
      int ARow, int ACol)
{
 if (ARow != 1) return;

 /*
 if (ACol==12 && AdvStringGrid4->Cells[12][ARow] != "")
    { AdvStringGrid4->Cells[12][ARow]  = "";
	AdvStringGrid7->Cells[12][ARow]  = "";
      asAvanc->Cells[25][row_avancement]="";

    }
    else
    {
     AdvStringGrid4->Cells[12][ARow]   = AnsiString(strcoche);
     AdvStringGrid7->Cells[12][ARow]   = AnsiString(strcoche);
     asAvanc->Cells[25][row_avancement]= AnsiString(strcoche);
    }

 if (ACol==12)
   {
    //AdvStringGrid4->Cells[12][ARow]  = "";
     AdvStringGrid7->Cells[12][ARow]   = "";
      asAvanc->Cells[25][row_avancement]=AdvStringGrid4->Cells[12][ARow]"";


   }

 */
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::Button33Click(TObject *Sender)
{
btChoixClick(Sender);               
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::AdvStringGrid6CanEditCell(TObject *Sender,
      int ARow, int ACol, bool &CanEdit)
{
CanEdit=false;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid6ClickCell(TObject *Sender,
      int ARow, int ACol)
{
int task; char tmp[50];

 if (ARow==0) return;
 row_avancement = ARow;
 ClickasAvanc(); // row_avancement);

 strcpy(tmp,asAvanc->Cells[1][row_avancement].c_str());
 task = atoi(tmp);
 MEMO_TASK=task;
 DrawMiniGantt(task,0,Image1,true);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid6ContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{

int X,Y;
 X = MousePos.x;
 Y = MousePos.y;
  asAvanc->MouseToCell(X, Y, CurCol, CurRow);
 // conversion en ligne et colonne courantes
 PopupActiv->Popup(X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid6GetCellColor(TObject *Sender,
      int ARow, int ACol, TGridDrawState AState, TBrush *ABrush,
      TFont *AFont)
{
int perc;

 
 if (ARow==0) return;
 getvalue(5,ARow,71); perc = atoi(adv);
 if (perc != 100)
   {
    if (ACol==1)
      {  ABrush->Color=clYellow;

      }
   }        
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid6SelectCell(TObject *Sender,
      int ACol, int ARow, bool &CanSelect)
{
char tmp[500];  int task;
 CanSelect=true;
 if (ARow==0) return;
 row_avancement = ARow;
 ClickasAvanc(); // row_avancement);

 strcpy(tmp,AdvStringGrid6->Cells[1][row_avancement].c_str());
 task = atoi(tmp);
 DrawMiniGantt(task,0,Image1,true);


}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid7CanEditCell(TObject *Sender,
      int ARow, int ACol, bool &CanEdit)
{

 int ch;
 CanEdit = false;
 if (ARow ==0) return;
 if (ACol==7)
   {
    CanEdit=true; return;
   }
 ch=vl_chp[4][ACol];
 if (ch_sai[ch][0]=='N') return;
 CanEdit = true;
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid7CellValidate(TObject *Sender,
      int ACol, int ARow, AnsiString &Value, bool &Valid)
{
  // retrouver le champ
 int ch;
 char tmp[255]; int xcal;
 bool valid;



 ch=vl_chp[4][ACol];

 strcpy(tmp,AdvStringGrid7->Cells[ACol][1].c_str());
 getvalue(5,row_avancement,62); // Calendrier
 xcal=atoi(adv); if (xcal < 1) xcal = 1;
 valid = Call_Process(ch,xcal,tmp);    // tmp est la valeur du champ cliqu�

 if (ACol==12)
   {
    //AdvStringGrid4->Cells[12][ARow]  = "";
      Valid=true;
      asAvanc->Cells[25][row_avancement]=AdvStringGrid7->Cells[12][ARow];

   }


 if (valid==true)
   { Valid=true;
    setvalue(4,1,ch,tmp);
    if (ch==28) setvalue(5,row_avancement,72,tmp);
    if (ch==29) setvalue(5,row_avancement,71,tmp);
    if (ch==57) setvalue(5,row_avancement,43,tmp);
    if (ACol==7) { ch= 23; AdvStringGrid6->Cells[4][row_avancement]=AnsiString(tmp); }

    CH=ch;
    Timer2->Enabled=true;
    // Propage(ch);
   }
 else Valid=false;

}
//---------------------------------------------------------------------------

void __fastcall TplStandard::AdvStringGrid7ClickCell(TObject *Sender,
      int ARow, int ACol)
{
if (ARow != 1) return;

 /*
 if (ACol==12 && AdvStringGrid7->Cells[12][ARow] != "")
    { AdvStringGrid7->Cells[12][row_avancement]  = "";
      AdvStringGrid6->Cells[25][row_avancement]="";

    }
    else
    {
     AdvStringGrid7->Cells[12][row_avancement]   = AnsiString(strcoche);
     AdvStringGrid6->Cells[25][row_avancement]= AnsiString(strcoche);
    }

 if (ACol==12)
   {
    //AdvStringGrid7->Cells[12][row_avancement]  = "";
      asAvanc->Cells[25][row_avancement]=AdvStringGrid7->Cells[12][ARow]"";


   }

 */
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::asAvancCanEditCell(TObject *Sender, int ARow,
      int ACol, bool &CanEdit)
{CanEdit=false;

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::AdvStringGrid4CanEditCell(TObject *Sender,
      int ARow, int ACol, bool &CanEdit)
{

 int ch;
 CanEdit = false;
 if (ARow ==0) return;
 if (ACol==7)
   {
    CanEdit=true; return;
   }
 ch=vl_chp[4][ACol];
 if (ch_sai[ch][0]=='N') return;
 CanEdit = true;
}


void __fastcall TplStandard::Button35Click(TObject *Sender)
{
BTACTUAL = 1;
Exec_Avanc(false);

}



void __fastcall TplStandard::Button36Click(TObject *Sender)
{
FillListBoxA();
 updatedatabase(LocalBase,0,0);
 Save_Actualisation(true);
 ReloadAllData(LocalBase,true,false);
 // Load_Avanc(FileNameAvanc,false);
}


void __fastcall TplStandard::Button37Click(TObject *Sender)
{
 int i,cntAv;  int totimp;
 int nt;   char tmp[100];


 if (Application->MessageBox("Voulez vous effectuer la remise � z�ro ? ","Ecopl",MB_OK)== IDNO) return;


strcpy(SAVEAS,"raz");
 SaveAs(false);

 cntAv=asAvanc->RowCount;
 for (i=1;i<cntAv;i++)
   {
    strcpy(tmp,asAvanc->Cells[1][i].c_str());
    nt = atoi(tmp);

    av->Cells[4][nt]=asAvanc->Cells[3][i];  // debut
    av->Cells[5][nt]=asAvanc->Cells[5][i];  //

    // comp_dist_dates(asAvanc->Cells[3]1,0);


    asAvanc->Cells[8][i]=asAvanc->Cells[3][i];
    asAvanc->Cells[10][i]=asAvanc->Cells[5][i];

    //Mise � jour du lien "FD" ou autre
    // comp_dist_dates(1,0);


    // cas marge n�gative
    strcpy(tmp,asAvanc->Cells[6][i].c_str());
    totimp = atoi(tmp);
    //if (totimp <0)
    //  {
       av->Cells[12][nt]=asAvanc->Cells[3][i];
       av->Cells[13][nt]="";
    //  }




   }





 Modif_All_Links();
 Save();
 Save_Actualisation(true);
 // Compute_PlusTotA();
 ReloadAllData(LocalBase,true,false);
 Load_Avanc(FileNameAvanc,false);
}

void __fastcall TplStandard::Button38Click(TObject *Sender)
{
Origin_Actu=0;
TabSheet3->TabVisible= true;
Button23->Caption = "Revenir Actualisation";
PageControl1->ActivePageIndex = 8;
}




void __fastcall TplStandard::BitBtn5Click(TObject *Sender)
{
switch(cpter)
{
case 0:
lnbPred->Visible = false;
lnbSucc->Visible = false;
Label3->Visible = false;
Label26->Visible = false;
Label19->Visible = false;
Label102->Visible = false;
Label4->Visible = false;
Label81->Visible = false;
Label82->Visible = false;
btBcl->Visible= false;
AdvStringGrid2->Visible = false;
Label78->Visible = false;
Label144->Visible = false;
Button31->Visible = false;
SB1->Visible = false;
immGantt->Visible = false;
AdvStringGrid3->Visible = false;
AdvStringGrid1->Height = 545;
BitBtn5->Caption = "Agrandir/Reduire";
cpter=+1;
break;

case 1:
lnbPred->Visible = true;
lnbSucc->Visible = true;
Label3->Visible = true;
Label26->Visible = true;
Label19->Visible = true;
Label102->Visible = true;
Label4->Visible = true;
Label81->Visible = true;
Label82->Visible = true;
btBcl->Visible= true;
AdvStringGrid2->Visible = true;
Label78->Visible = true;
Label144->Visible = true;
Button31->Visible = true;
SB1->Visible = true;
immGantt->Visible = true;
AdvStringGrid3->Visible = true;
AdvStringGrid1->Height = 241;
BitBtn5->Caption = "Agrandir/Reduire";
cpter=0;
break;
}

}
//---------------------------------------------------------------------------




void __fastcall TplStandard::BitBtn6Click(TObject *Sender)
{
switch(cpter2){
case 0:

AdvStringGrid1->HideColumns(6,7);
AdvStringGrid1->HideColumns(12,13);
cpter2++;
break;

case 1:

AdvStringGrid1->UnHideColumns(6,7);
AdvStringGrid1->UnHideColumns(12,13);
cpter2--;
break;
}
}
//---------------------------------------------------------------------------


//J'ai activ�  puis desactiv� cbPos pour avoir le calcul des Ressources Pr�visionnel,
//ce sera pareil pour les ressources R�els
/*void __fastcall TplStandard::CheckBox1Click(TObject *Sender)
{if (cbPos->Checked == true)
cbPos->Checked = false;
cbNeg->Checked = false;
 CheckBox2->Checked= false;
 rbRess->Checked=true;
 CheckBox1->Checked = true;
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::CheckBox2Click(TObject *Sender)
{
cbPos->Checked = false;
cbNeg->Checked = false;
 CheckBox1->Checked= false;
 rbRess->Checked=true;
 CheckBox2->Checked = true;
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::CheckBox3Click(TObject *Sender)
{

    if (cbNeg->Checked == true) cbPos->Checked = false;
 CheckBox1->Checked= false;
 CheckBox2->Checked= false;
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------

void __fastcall TplStandard::CheckBox4Click(TObject *Sender)
{
    if (cbNeg->Checked == true) cbPos->Checked = false;
 CheckBox1->Checked= false;
 CheckBox2->Checked= false;

 Calcul_Echeancier();

}
//---------------------------------------------------------------------------
 */




void __fastcall TplStandard::ComboBox2Change(TObject *Sender)
{
      String itemsel=  ComboBox2->Items->Strings[ComboBox2->ItemIndex].c_str();

      if(itemsel == "Cout Previsionnel")
      {
        rbCout->Checked = true;
        cbPos->Checked = true;
        modeal = 1;
        RadioButton1->Checked = true;
      }

      if(itemsel == "Cout Reel")
      { rbCout->Checked = true;
        cbPos->Checked = true;
        modeal = 1;
        RadioButton1->Checked = true;
         }



      if(itemsel == "Ressources Previsionnelles")
      {
        rbRess->Checked = true;
        cbPos->Checked =true;
        modeal=2;
                RadioButton1->Checked = true;
      }
        if(itemsel == "Ressources Reeles")
      {
     
        rbRess->Checked = true;
        cbPos->Checked =true;
        modeal=2;
                RadioButton1->Checked = true;
      }

}
//---------------------------------------------------------------------------



void __fastcall TplStandard::cbNegClick(TObject *Sender)
{

 if (cbNeg->Checked == true) cbPos->Checked = false;
 Calcul_Echeancier();
}
//---------------------------------------------------------------------------



void __fastcall TplStandard::Display_Graphics2(int prt,TImage *imAC, TImage *imGNT)
{

 int i,nt,cntrow,from; char snumt[50]; char tmp[150];
 int distprev,distreel,maxdprev,maxdreel,maxdist;   int pas;
 int PasV,v1,v2,v3; int x1,x2,y1,y2,pasx,step;
 AnsiString Avant,Apres;  int res;
 char dt1[20],dt2[20];  TRect diagram;
 int AnREF,MoisREF,JourREF; int dist;
 char dtPrev[15],dtAvanc[15]; int indcal;
 TDateTime dtFirst; int dow;
 int  distavanc;


 Clear_Diagram(imGNT);
 Clear_Diagram(imAC);

 //  NBCells = 20;  // $$$$
 // parcourir la table pour trouver les taches, et calculer distances;
 from=0;
 cntrow= avact->RowCount;
 maxdprev=0;
 for (i=prt; i<cntrow;i++)
   {
    strcpy(snumt,avact->Cells[1][i].c_str());
    nt = atoi(snumt);

    if (nt==prt) { from=nt; }

   }

 Compute_D1_D2();
 Prepare_Tableau(from,D1);
 Copy_Calendar(1,D1);

 Update_Aff_Concernees(NBCourb);

 TRect Diagram;
 Diagram = Rect(1,1, imAC->Width-1,imAC->Height);
 imAC->Canvas->Brush->Color = clAqua; // BgColor;
 imAC->Canvas->FillRect(Diagram);
 // imAC->Canvas->Brush->Color = clAqua; // BgColor;
 // imAC->Canvas->FillRect(Diagram);

 if (DMAX >= 0)
    { pas = imGNT->Width / (DMAX+D1+D2);
      if (DMAX<5) step=1; else if (DMAX<25) step=7; else if (DMAX<62) step=14;
    }
   else { pas = imGNT->Width / (-DMAX+D1+D2);  step=1; }

    // Trac� des lignes verticales calendrier

 PasV = 21;  v1=-13; v2=-7; v3=-21;



 for (i=1;i<=NBCourb;i++)
  {
    if (i>1)
     {
    imGNT->Canvas->Pen->Width=1;
    imGNT->Canvas->Pen->Color = clBlack;
    imGNT->Canvas->MoveTo(0,i*PasV + v3);
    imGNT->Canvas->LineTo(imGNT->Width,i*PasV + v3);
     }
    imGNT->Canvas->Pen->Width=3;
    imGNT->Canvas->Pen->Color = clRed;
    imGNT->Canvas->MoveTo((ddprev[i]-D1)*pas,i*PasV +v1);
    imGNT->Canvas->LineTo((dfprev[i]-D1)*pas,i*PasV +v1);
    imGNT->Canvas->Pen->Width=3;
    imGNT->Canvas->Pen->Color = clGreen;
    imGNT->Canvas->MoveTo((ddreel[i]-D1)*pas,i*PasV +v2);
    imGNT->Canvas->LineTo((dfreel[i]-D1)*pas,i*PasV +v2);
  }  // for i
  // derniere ligne noire
    imGNT->Canvas->Pen->Width=1;
    imGNT->Canvas->Pen->Color = clBlack;
    imGNT->Canvas->MoveTo(0,i*PasV + v3);
    imGNT->Canvas->LineTo(imGNT->Width,i*PasV + v3);
   if (DMAX >= 0)
    {
     pasx = pas;  //
     add_jour(An,Mois,Jour,D1);
     sprintf(dt2,"%02d/%02d",Jour,Mois);
     AnREF=An; MoisREF=Mois; JourREF = Jour;

     ShowMessage(IntToStr(D1));
     ShowMessage(IntToStr(D2));
     ShowMessage(IntToStr(DMAX));

   for (i=D1;i<(D1+DMAX+D2+2);i++) // i=i+step)
      {
       y1=imAC->Height/2; y2=imAC->Height;
       x1=(i-D1)*pasx;
       add_jour(AnPrev,MoisPrev,JourPrev,i);
       imAC->Canvas->Pen->Width=1;
       imAC->Canvas->Brush->Style = bsClear;
       imAC->Canvas->Pen->Color = clBlack;
       imAC->Canvas->MoveTo(x1,y1);
       imAC->Canvas->LineTo(x1,y2);
       sprintf(dt2,"%02d",Jour);
       ShowMessage(IntToStr(Jour));
       y1=y1+2; imAC->Canvas->TextOutA(x1+2,y1,AnsiString(dt2));
       imAC->Canvas->Font->Size = 8;
      }

      imAC->Canvas->MoveTo(0,imAC->Height/2);
      imAC->Canvas->LineTo(imAC->Width,imAC->Height/2);


     for (i=D1;i<(D1+DMAX+D2);i++) // i=i+step)
      {
       y1=0; y2=imAC->Height;
       x1=(i-D1)*pasx;



       add_jour(AnPrev,MoisPrev,JourPrev,i);
       dtFirst = EncodeDate(An,Mois,Jour);
       int firstday = dtFirst.DayOfWeek();
       dow = firstday-1;
       if (dow==1 || i==D1)
         {
           imAC->Canvas->Pen->Width=1;
           imAC->Canvas->Brush->Style = bsClear;
           imAC->Canvas->Pen->Color = clBlack;
           imAC->Canvas->MoveTo(x1,y1);
           imAC->Canvas->LineTo(x1,y2);
           sprintf(dt2,"%02d/%02d",Jour,Mois);
           imAC->Canvas->Font->Size = 8;
           y1=2; imAC->Canvas->TextOutA(x1+2,y1,AnsiString(dt2));
           y1=0; y2=NBCourb*21;
           x1=i*pasx;
           imGNT->Canvas->Pen->Width=1;
           imGNT->Canvas->Pen->Color = clBlack;
           imGNT->Canvas->MoveTo(x1,y1);
           imGNT->Canvas->LineTo(x1,y2);
         }
      }
      // calculer indice i d'origine

      sprintf (dtPrev,"%02d-%02d-%02d",JourPrev,MoisPrev,AnPrev%100);
      sprintf (dtAvanc,"%02d-%02d-%02d",JourAvanc,MoisAvanc,AnAvanc%100);

      comp_dist_dates(dtPrev,dtAvanc,1,0);

       indcal = NB1-D1;

      for (i=D1;i<(D1+DMAX+D2);i++)      /// i=0
      {
       add_jour(AnREF,MoisREF,JourREF,i);
       indcal = i; // D1+i;
       // indcal = i;
       if (ACAL[indcal]=='N')
         { y2=NBCourb*21;
           x1=(i-D1)*pasx;
           diagram = Rect(x1,2,x1+pas-1,y2);
           imGNT->Canvas->Brush->Color = clSilver; //
           imGNT->Canvas->FillRect(diagram);
         }
      }


     x1= (DMAX-D1)*pas;
     y1=0; y2=NBCourb*21; //y2=imGNT->Height;
     /*
     imGNT->Canvas->Pen->Width=2;
     imGNT->Canvas->Pen->Color = clRed;
     imGNT->Canvas->MoveTo(x1,0);
     imGNT->Canvas->LineTo(x1,y2);
     x1 = (D1+ DMAX)*pas;
     imGNT->Canvas->MoveTo(x1,0);
     imGNT->Canvas->LineTo(x1,y2);
     */
     imGNT->Canvas->Brush->Style = bsClear;
     imGNT->Canvas->Pen->Width=1;
     imGNT->Canvas->Pen->Color = clBlack;
     for (i=1;i<=NBCourb;i++)
       {
         sprintf(tmp,"T�che %d",avtask[i]);
         x1= (dfreel[i]-D1)*pas+5; y1= i*PasV +v3+5;
         imGNT->Canvas->TextOutA(x1,y1,AnsiString(tmp));
       }
     x1= (DIST_AVANC_1-D1)*pas + 1; y1=0; y2=NBCourb*21; //y2=imGNT->Height;
     imGNT->Canvas->Pen->Width=4;
     imGNT->Canvas->Pen->Color = clGreen;
     imGNT->Canvas->MoveTo(x1,0);
     imGNT->Canvas->LineTo(x1,y2);

    // int  distavanc = convert_date(AnAvanc,MoisAvanc,JourAvanc);

      if (CntAVBefore > 0)
        {
          strcpy(tmp,TableDatesAV[CntAVBefore-1]);   // Avancement Pr�c�dent
           distavanc = Reverse_Date(tmp);


           x1= (distavanc-D1)*pas+1; y1=0; y2=NBCourb*21; //y2=imGNT->Height;
           imGNT->Canvas->Pen->Width=4;
           imGNT->Canvas->Pen->Color = clGreen;
           imGNT->Canvas->MoveTo(x1,0);
           imGNT->Canvas->LineTo(x1,y2);
        }
   }

}


void __fastcall TplStandard::BitBtn7Click(TObject *Sender)
{
TabSheet5->TabVisible = true;
PageControl1->ActivePageIndex=1;
}
//---------------------------------------------------------------------------


void __fastcall TplStandard::BitBtn8Click(TObject *Sender)
{

TabSheet5->TabVisible = false;
PageControl1->ActivePageIndex=0;
}
//---------------------------------------------------------------------------




void __fastcall TplStandard::BitBtn10Click(TObject *Sender)
{

TFDtaches *Taches;
 Taches = new TFDtaches(Application);
  Taches->ShowModal();

 btRetPlanning1Click(Sender);


 delete Taches;

}
//---------------------------------------------------------------------------


void __fastcall TplStandard::BitBtn9Click(TObject *Sender)
{

TFDcorps *Corps;
 Corps = new TFDcorps(Application);

 Corps->ShowModal();


 btRetPlanning1Click(Sender);


 delete Corps;
}
//---------------------------------------------------------------------------
void __fastcall TplStandard::cochertt()
{
 for (int i = 0; i <AdvStringGrid1->RowCount; i++){
  if(AdvStringGrid1->Cells[0][i]!=""){
 asCrit->Cells[0][i]=AnsiString(strcoche);
  }
  }
}








void __fastcall TplStandard::Button32Click(TObject *Sender)
{
imP3p->Picture->SaveToFile("C:\\Users\\Claude\\Desktop\\demo\\cap2.bmp");
        
}
//---------------------------------------------------------------------------

