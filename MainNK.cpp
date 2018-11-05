//---------------------------------------------------------------------------
#include <vcl.h>
#include <utilcls.h>
#pragma hdrstop

#include <dos.h>
#include "MainNK.h"
#include "About.h"
#include "projectwin.h"
#include "calendars.h"
#include "modhelp.h"
#include "modplanches.h"
#include "modFilter.h"
#include "modCriteres.h"
#include "modTri.h"
#include "modGhost.h"
#include "mdchoix.h"
// #include "modDump.h"
#include "modResume.h"
#include "modDebut.h"
#include "modCSim.h"
#include "modAffTC.h"
#include "modRempl.h"
#include "modExcel.h"
#include "modColonnes.h"

#include "modPrepPlanning.h"
#include "modPrepRapport.h"
#include "modImport.h"
#include "modExcelCSV.h"

#include "modregistr.h"

// #include "ikey.h"

#define  HPLST 742
#define  WPLST 1060


#include "global.h"

//---------------------------------------------------------------------------
//#pragma link "Excel_97_SRVR"
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent *Owner)
    : TForm(Owner)
{

}
//---------------------------------------------------------------------------


bool __fastcall TMainForm::Check_Config()
{
 FILE *fp; char FileName[MAX_PATH];
 int days;  int shift;
 struct date d; int month,year,day;
 int month1,year1,day1;  int m,y,dd;
 int dist,dist1;  char LK[500];  int i,delta;
 char EXPDATE[50],tmp[100],str[100];
 char *p; char mess[200];  int rv; char base[10];
 TRegist *reg;


 getdate(&d);
 year =   d.da_year;
 month =  d.da_mon;
 day   =  d.da_day;
 dist =  (year-2000)*365 + month*31+day;

 if (!Enum_MAC())
   {
    Application->MessageBox("First Installation : don't forget to run the prompt command 'ipconfig /all > c:\ipconfig.txt'","AVSA",MB_OK);
   Close();
   };

 strcpy(MAC_ADDR,mac_addr.c_str());

 // strcpy(FileName,HomeDirectory);

 strcpy(FileName,m_directory);
 strcat(FileName,"\\cst.sys");
 fp = fopen(FileName,"rb");

 if (fp==NULL)   // new installation
    {

     reg= new TRegist(Application);
     reg->ShowModal();
     delete reg;

     // if (!LICKEY) Close();

     /*
     days = 31;
     fp = fopen(FileName,"wb");
     // On ajoute 30 jours
     dd=day;
     m=month+1;
     y=year;
     if (m>12) { m=1; y=y+1; }
     if (m==2){ if (dd>28)dd=28; }
     rv = random(20);
     base[0]=rv+65;
     base[1]=0;


     sprintf(LK,"%s<m>%s</m><e>%04d/%02d/%02d</e>",base,MAC_ADDR,y,m,dd);

     // faire un random 0-20;



     strcat(LK,"<1></1><2></2><3></3><4></4><5></5>");
     for (i=1;i<strlen(LK);i++)  LK[i]=LK[i]+rv;
     strcat(LK,"\n");
     fputs(LK,fp);
     fclose (fp);
     Application->MessageBoxA(M[545].c_str(),"AVSA",MB_OK);
     REGIST = false;
     */
     fp = fopen(FileName,"rb");
    }
 else LICKEY=true;

if (LICKEY)
 {
 fgets(LK,5000,fp);

// l=strlen(LK);

 shift=LK[0] - 65;


 for (i=1;i<strlen(LK);i++) LK[i]=LK[i]-shift;
 fclose(fp);

 Ghost->ExtractValue(tmp,LK,"m",0);

 if (strcmp(tmp,"AA:BB:CC:DD:EE:00")==0) return true;

 if (strcmp(MAC_ADDR,tmp) != 0)
    {
     Application->MessageBox("Code MAC incorrect","ECOPL",MB_OK); // "Licence key doesn't match your Client Code"
     return false;
    }

 Ghost->ExtractValue(EXPDATE,LK,"e",0);   // Expiration date
 if (strlen(EXPDATE) != 10)
     {
      Application->MessageBox("Date Expiration atteinte","AVSA",MB_OK); // "Invalid date format - Contact your distributor"
      return false;
     }
  strcpy(tmp,EXPDATE);
  p=tmp;
  tmp[4]=0; tmp[7]=0;
  strcpy(str,p);   year1 = atoi(str);
  p=p+5; strcpy(str,p);   month1 = atoi(str);
  p=p+3; strcpy(str,p);   day1 = atoi(str);


  if (year1 < 2010) { Application->MessageBox("Ann�e incorrecte","AVSA",MB_OK); return false ; }    // "Invalid year"
  if (month1 < 1 || month1 >13) { Application->MessageBox("Mois Incorrect","AVSA",MB_OK); return false ;  }   // "Invalid Month"
  if (day1< 1 || day1 >31){ Application->MessageBox("Jour Incorrect","AVSA",MB_OK); return false;  }   // "Invalid Day"

   dist1 =  (year1-2000)*365 + month1*31+day1;
   if (dist > dist1)  { Application->MessageBox("Date Expiration atteinte","ECOPL",MB_OK);return false; }     // "Licence has expired - Please contact your distributor"
   if (dist > dist1-5)
        {
          delta = dist-dist1-5;
          strcpy(mess,"Votre licence expirera le ");strcat(mess,EXPDATE);    // "Warning : you licence will expire the "
          Application->MessageBox(mess,"ECOPL",MB_OK);
          // return true;
        }
    // if (dist < dist1) { Register1->Visible = false; }
  return true;
 }  // IF LICKEY
 else return false;
}


bool __fastcall TMainForm::Enum_MAC()
{
 FILE *fp;  int i;
 char content[10000];
 char fname[MAX_PATH];
 char *p,*p1;

 for (i=0;i<10;i++)  strcpy(MACS[i],"");
 NB_MACS=0;

 strcpy(fname,"c:\\ipconfig.txt");
 fp = fopen(fname,"rb");
 if (fp==NULL) return false;

 fread(content,9999,1,fp);
 strupr(content);
 p1 = content;
 p=strstr(p1,"PHYSI");
 while (p)
   {
    p1 = strstr(p,": ");
    p1=p1+2; if (*(p1+2) == '-')
      {
       NB_MACS++;
       strncpy(MACS[NB_MACS],p1,17);  MACS[NB_MACS][17]=0;
       MACS[NB_MACS][2]=':';  MACS[NB_MACS][5]=':'; MACS[NB_MACS][8]=':';
       MACS[NB_MACS][11]=':'; MACS[NB_MACS][14]=':';

        if (NB_MACS==1) mac_addr = AnsiString(MACS[NB_MACS]);
      }
    p = strstr(p1,"PHYSI");
   }

 if (NB_MACS==0)
    {
     Application->MessageBox("No MAC address found in the ipconfig.txt file","AVSA",MB_OK);
     return false;
    }

 fclose(fp);
 return true;
}




void __fastcall TMainForm::CreateMDIChild(String Name)
{
    TMDIChild *Child;

    //--- create a new MDI child window ----
    Child = new TMDIChild(Application);
    Child->Caption = Name;
    if (FileExists (Name))
        Child->Memo1->Lines->LoadFromFile(Name);
}
//--------------------------------------------------------------------------

void __fastcall TMainForm::FileNew1Execute(TObject *Sender)
{
 CreateMDIChild("NONAME" + IntToStr(MDIChildCount + 1));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileOpen1Execute(TObject *Sender)
{
 char *p;  int i;
 char name[MAX_PATH];
 char pname[MAX_PATH];
 AnsiString FName;

 if (strlen(DatabaseName))
    {
     for(i = MDIChildCount-1; i >= 0; i--) MDIChildren[i]->Close();
     Build_History();
     WriteLast();
     RAZ_Appli();

    }

    OpenDialog->FileName = "*.eco";
    if (OpenDialog->Execute())
          {
            strcpy(name,OpenDialog->FileName.c_str());
            p=strstr(name,".eco");
            if (p)
              {
               *p=0;
               strcpy(DatabaseName,name);
              }
               else strcpy(DatabaseName,"");

             NouveauProjet1->Enabled=false;
             FileOpenItem->Enabled=false;
             FileCloseItem->Enabled=true;
             Sauver1->Enabled=true;
             SauverComme1->Enabled=true;
             strcpy(LastAppli,DatabaseName);



             Launch(DatabaseName);
             First(DatabaseName);

          }

}


void __fastcall TMainForm::Launch(char *s)
{

  int w,h; char *p;
  char pname[MAX_PATH];
  AnsiString FDir,FName;
  // Remplir Project Name

  FName = ExtractFileName(AnsiString(DatabaseName));
  strcpy(pname,FName.c_str());
  strcpy(ProjectName,pname);



  plST = new TplStandard(Application);
  plST->Caption = AnsiString(s);
   w = Screen->Width;
   h=  Screen->Height;


  /*
  if (w>=1024)
   {
    plST->Width = w*980/1024;
    plST->Height = h*600/768;
   }
  */

  plST->Height=HPLST; // 700; //822;
  plST->Width=WPLST; //1000; // 1083;

  Filtres1->Visible=true;
  Critres1->Visible = true;
  Cocher1->Visible = true;
  //TrierTches1->Visible=true;
  Renumrotertches1->Visible=false;
  Prparationimpressionrapport1->Visible=false;
  Prparationimpressionplanning1->Visible=true;
  // Remisezroavancement1->Visible=true;
  Simulation1->Visible=true;
  Rsumduprojet1->Visible=true;
  Datededbutduprojet1->Visible=true;
  Critres2->Visible=true;

}



void __fastcall TMainForm::First (char *s)
{
 int i,indx; char tmp[250];
 char *p,*p1; bool exist; int memcount;
 // mettre Path s en premiere position
 // count contient le nombre de postes
 for (i=9;i>0;i--)
   {
    strcpy(hist[i],hist[i-1]);
    strcpy(hpath[i],hpath[i-1]);
   }
// inserer le nouvel �lement
 strcpy(hpath[0],s);

 // extraire le nom de l'application
 p=s;
 p1=strchr(p,'\\');
 while (p1)
  {
   p=p1+1;
   p1=strchr(p,'\\');
  }

  strcpy(hist[0],p);
  strupr(hist[0]);
 // regarder s'il existait deja

 memcount=count;
 exist = false;
 for (i=1;i<=count;i++)
   {
    if (strcmp(hpath[i],s)==0)
      { exist=true; indx=i;
        break;
      }
   }
 // effacer si le chemin existait deja
 if (exist)
  {
   for (i=indx;i<=count;i++)
     {
      strcpy(hist[i],hist[i+1]);
      strcpy(hpath[i],hpath[i+1]);
     }
  }
 else count++;


for (i=0;i<memcount;i++) File1->Delete(pos);  // pas POS+I !!
Build_History();
History();
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::HelpAbout1Execute(TObject *Sender)
{
 TAboutBox *About;
 About = new TAboutBox(Application);
 About->ShowModal();
 delete About;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileExit1Execute(TObject *Sender)
{
  int i; char tmp[200];

  if (!FCLOSE)
    {
      if (Application->MessageBoxA("Voulez-vous fermer Ecoplanning ?",m_ecoplan,MB_YESNO) == IDNO) return;
    }
  for(i = MDIChildCount-1; i >= 0; i--) MDIChildren[i]->Close();
  Build_History();
  WriteLast();
  RAZ_Appli();
  Close();
}


void __fastcall TMainForm::WriteLast()
{
 FILE *fp;  char fn[MAX_PATH];  int i;

 char tmp[MAX_PATH]; char tout[MAX_PATH];
 strcpy(fn,m_directory); strcat(fn,"\\lastopen.txt");
 strcpy(tout,"");
 fp=fopen(fn,"wb");
 if (fp)
  {
   strcpy(tmp,"<app>"); strcat(tmp,DatabaseName); strcat(tmp,"</app>");  strcat(tout,tmp);

   /*
   for (i=1;i<6;i++)
     { sprintf(tmp,"<pl%d>%s</pl%d>",i,LastPlanning[i],i);
       strcat(tout,tmp);
     }
   //strcpy(tmp,"<pl>"); strcat(tmp,LastPlanning); strcat(tmp,"</pl>"); strcat(tout,tmp);
   //strcpy(tmp,"<rp>"); strcat(tmp,LastRapport); strcat(tmp,"</rp>"); strcat(tout,tmp);
   strcpy(tmp,"<tr>"); strcat(tmp,LastTri); strcat(tmp,"</tr>"); strcat(tout,tmp);
   strcpy(tmp,"<cl>"); strcat(tmp,LastCol); strcat(tmp,"</cl>"); strcat(tout,tmp);
   strcpy(tmp,"<ft>"); strcat(tmp,LastFilter); strcat(tmp,"</ft>"); strcat(tout,tmp);
   */


   fwrite(tout,strlen(tout)+1,1,fp);
   fclose(fp);
  }
strcpy(DatabaseName,"");

}

//---------------------------------------------------------------------------

void __fastcall TMainForm::NouveauProjet1Click(TObject *Sender)
{
 TfProjet *Project;
 char name[MAX_PATH];
 char *p; int i,w,h;
 AnsiString FName;
 char pname[MAX_PATH];

 if (strlen(DatabaseName))
    {
     for(i = MDIChildCount-1; i >= 0; i--) MDIChildren[i]->Close();
     Build_History();
     WriteLast();
     RAZ_Appli();
    }

 RAZ_Appli();
 strcpy(m_exchange,"");
 Project = new TfProjet(Application);
      Project->Caption = "Nouveau Projet � d�finir";
          Project->ShowModal();
 delete Project;

 if (Cancel==1) return;

 Inserer_Menu(DatabaseName);
  // nom dans Database Name
 plST = new TplStandard(Application);
 plST->Caption = AnsiString(DatabaseName);
 MainForm->Caption = AnsiString(DatabaseName);

 FName = ExtractFileName(AnsiString(DatabaseName));
 strcpy(pname,FName.c_str());
 strcpy(ProjectName,pname);


   w = Screen->Width;
   h=  Screen->Height;


  /*
  if (w>=1024)
   {
    plST->Width = w*1000/1024;
    plST->Height = h*700/768;
   }
 */

 plST->Width = WPLST;
 plST->Height = HPLST;

 Sauver1->Enabled=true;
 SauverComme1->Enabled=true;
 FileCloseItem->Enabled = true;
 // plST->ClientHeight=759;
 Critres1->Visible = true;
 Filtres1->Visible = true;
 Cocher1->Visible = true;
 //TrierTches1->Visible=true;

}

void __fastcall TMainForm::Inserer_Menu(char *s)
{
 char tmp[250]; char *p,*p1;
 int i; FILE *fp;
 // d�caler de une position
 for (i=9;i>0;i--)
   {
    strcpy(hist[i],hist[i-1]);
    strcpy(hpath[i],hpath[i-1]);
   }
 // inserer le nouvel �lement
 strcpy(hpath[0],s);
 // extraire le nom de l'application
 p=s;
 p1=strchr(p,'\\');
 while (p1)
  {
   p=p1+1;
   p1=strchr(p,'\\');
  }

 strcpy(hist[0],p);
 strupr(hist[0]);

 // effacer les items menu;
 for (i=0;i<=count;i++) File1->Delete(pos);  // pas POS+I !!
Build_History();
History();
}


void __fastcall TMainForm::Build_History()
{

 FILE *fp; int i,j; char tmp[250];  char memo[250];
 AnsiString prjname;
 // Fabriquer un fichier sortie

 strcpy(tmp,m_directory); strcat(tmp,"\\history.txt");
 fp = fopen(tmp,"w+t");
 for (i=0;i<count;i++)
 {
  if (strlen(hist[i]))
   {

    prjname = AnsiString(hpath[i]) + ".eco";
     if (FileExists(prjname))
     {
         strcpy(tmp,"<name>"); strcat(tmp,hist[i]); strcat(tmp,"</name>");
         fputs(tmp,fp);
         strcpy(tmp,"<path>"); strcat(tmp,hpath[i]); strcat(tmp,"</path>");
         fputs(tmp,fp); fputs("\r\n",fp);
     }
   }
 }
fclose(fp);
}


//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
 int i;

 //if (CLEF==1)
 //  {
 //    ikey_CloseDevice( handle );
 //    ikey_DeleteContext( handle );
 //  }
 delete m_modcrit;
 delete m_coche;
 delete m_filtre;
 delete m_project;
 delete m_exchange;
 delete m_exchange2;
 delete m_ecoplan;
 delete m_menu;
 delete DatabaseName;
 delete m_pasapas;
 delete m_criteres;
 delete m_sender;
 delete m_params;
 delete m_sort;
 delete m_tridetail;
 delete m_reports;
 delete m_origin;
 delete m_directory;
 delete m_message;
 delete m_planning;
 delete m_rapport;
 delete m_assign;

 delete m_saveAV;
 delete m_help;
 delete m_status;
 delete m_excel;

 delete m_zero;

 delete m_simul;
 delete m_savesimul;
 delete m_renum;
 delete m_save;
 delete m_memoriser;
 delete m_datefinreelle;
 delete m_mode;
 delete m_chcrit;
 delete m_impt;
 delete m_resize;
 delete m_affect_crit;

 delete m_importtaches;
 delete m_importliaisons;
 delete m_importcriteres;


 for (i=0;i<MAX_PLAN;i++)
 {
  delete P_TRI[i];
  delete P_FILTRE[i];
  delete P_COLONNES[i];
 }


for (i = 0; i < TSL->Count; i++)
{
delete (T_tch *)TSL->Objects[i]; //->Free();
}
TSL->Free();
TSLCRIT->Free();

Action=caFree;
}


//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *Sender)
{
 int i;
 AnsiString Dir;

 Timer1->Enabled = false;

 db = new realisam();
 rc = db->OpenEngine("Ecobase",120,"ndx","dat");   //512 - 8
 rc = db->SetIndexMode(0,0);
 if (rc==0) { ShowMessage("Error Opening Ecobase database"); exit(1); }
 DatabaseName = new char[MAX_PATH]; DatabaseName[0]=0;
 m_status = new char[500];  m_status[0]=0;
 m_filtre = new char[2500];  m_filtre[0]=0;
 m_coche = new char[2500];   m_coche[0]=0;
 m_modcrit = new char[2000]; m_modcrit[0]=0;
 m_project = new char[200];  m_project[0]=0;
 m_exchange = new char[2000]; m_exchange[0]=0;
 m_exchange2 = new char[2000]; m_exchange2[0]=0;

 m_menu = new char[250];  m_menu[0]=0;
 m_pasapas = new char[250]; m_pasapas[0]=0;
 m_criteres = new char[250]; m_criteres[0]=0;
 m_sort = new char[1000]; m_sort[0]=0;
 m_tridetail = new char[1000]; m_tridetail[0]=0;
 m_params = new char[1000]; m_params[0]=0;
 m_sender = new char[250]; m_sender[0]=0;
 m_reports = new char[512]; m_reports[0]=0;
 m_origin = new char[250]; m_origin[0]=0;
 m_directory = new char[250]; m_directory[0]=0;
 m_message = new char[2000]; m_message[0]=0;
 m_planning = new char[2000]; m_planning[0]=0;
 m_rapport  = new char[2000]; m_rapport[0]=0;

 m_assign = new char[10000]; m_assign[0]=0;
 for (i=0;i<7;i++) m_ghost_prep[i]="";
 m_saveAV = new char[250];  m_saveAV[0]=0;
 m_zero = new char[250]; m_zero[0]=0;
 m_help = new char[250];  m_help[0]=0;
 m_dateprev = new char[200];   m_dateprev[0]=0;
 m_datefinreelle = new char [200];;

 m_simul = new char[10000];  m_simul[0]=0; // stocke les periodes
 m_savesimul = new char[1000]; m_savesimul[0]=0; // nom du nouveau simulateur;
 m_renum = new char[200];  m_renum[0]=0;

 m_excel = new char[100];  m_excel[0]=0;
 m_importtaches = new char[256]; m_importtaches[0]=0;
 m_importliaisons = new char[256]; m_importliaisons[0]=0;
 m_importcriteres = new char[256]; m_importcriteres[0]=0;
 m_save = new char[100];  m_save[0]=0;
 m_memoriser = new char[50]; m_memoriser[0]=0;
 m_mode = new char[200];  m_mode[0]=0;
 m_chcrit = new char [200]; m_chcrit[0]=0;
 m_impt    = new char[200]; m_impt[0]= 0;
 m_resize  = new char[200]; m_resize[0]=0;
 m_affect_crit  = new char[200]; m_affect_crit[0]=0;

 VERSION = true;  // true;
 if (VERSION==false) { m_ecoplan = new char[250]; strcpy(m_ecoplan,"Ecoplanning Version 5.0"); }
 else  { m_ecoplan = new char[250]; strcpy(m_ecoplan,"Ecoplanning Version 5.1"); }



 TSL = new TStringList;
 TSLCRIT = new TStringList;

 Dir = GetCurrentDir();
 strcpy(m_directory,Dir.c_str());
 strcpy(HelpDirectory,m_directory);
 strcat(HelpDirectory,"\\Docs\\");


 // Init Planning and reports

pc=0;
//rc=0;

for (i=0;i<MAX_PLAN;i++)
 {
  P_PLANNING[i] = false;
  P_TRI[i] = new char[250];
  P_FILTRE[i]= new char[250];
  P_COLONNES[i]= new char[250];
 }


indx_proj=0;
FileOpenItem->Enabled=true;
FileCloseItem->Enabled=false;
Sauver1->Enabled=false;
SauverComme1->Enabled=false;
// Timer1->Enabled=true;
Filtres1->Visible=false;
Cocher1->Visible=false;
Cocher2->Visible=false;
//TrierTches1->Visible = false;
Renumrotertches1->Visible=false;
Prparationimpressionrapport1->Visible=false;
Prparationimpressionplanning1->Visible=false;
// Remisezroavancement1->Visible=true;
Simulation1->Visible=true;
//InterfaceExcel1->Visible=false;
Rsumduprojet1->Visible=false;
Datededbutduprojet1->Visible=false;
Critres2->Visible = false;
PlanchesVolets1->Visible=true;
pos=18;  // POSITION WHERE HISTO STARTS
History();
flag_last=1;

timercnt=0;
FCLOSE = false;
CLEF=0;    // //////////////////////////
 if (CLEF)
    { i = InitKey();
      if (i==0) FCLOSE=true;
      else
         {
          i = DecCounter();
          if (i != 0x7fff)  FCLOSE = true; else FCLOSE =false;//Close();
         }
    }

}

void __fastcall TMainForm::History()
{
 FILE *fp; char *p; int cnt;
 char tmp[250],tmp1[250],tmp2[250];  char previous[250];
 AnsiString prjname;
 TMenuItem *MenuItem;
 // read the history file
 strcpy(tmp,m_directory); strcat(tmp,"\\history.txt");
 fp = fopen(tmp,"r+t");
 if (fp==NULL) return;
 for (cnt=0;cnt<10;cnt++) {hist[cnt][0]=0; hpath[cnt][0]=0; }

 cnt=0; count=0;
 strcpy(previous,"");
 while (!feof(fp))
  {
   tmp[0]=0;
   fgets(tmp,200,fp);
   p=strchr(tmp,'\r');
   if (p) *p=0;
   p=strchr(tmp,'\n');
   if (p) *p=0;
   if (strlen(tmp))
   {
    if (count <10)
    {
     Ghost->ExtractValue(tmp1,tmp,"name",0);
     if (strcmp(tmp1,previous)==0) tmp1[0]=0; else strcpy(previous,tmp1);
     if (strlen(tmp1))
      {
        Ghost->ExtractValue(tmp2,tmp,"path",0);
        strupr(tmp1);
        strcpy(hist[cnt],tmp1);
        strcpy(hpath[cnt],tmp2);

        prjname = AnsiString(hpath[cnt]) + ".eco";
        if (FileExists(prjname))
          {
           MenuItem = new TMenuItem(File1);
           MenuItem->Caption = AnsiString(tmp1);
           File1->Insert(pos+cnt,MenuItem);
           MenuItem->OnClick = File1Click;
           cnt++;count++;
          }
      }
    }
   }
  }
 MenuItem = new TMenuItem(File1);
 MenuItem->Caption = "-";
 File1->Insert(pos+count+1,MenuItem);
 fclose(fp);

}

//---------------------------------------------------------------------------

void __fastcall TMainForm::RestorerBasedeConnaissances1Click(
      TObject *Sender)
{
  int rc;  // code retour
  FILE *fd; // fichier texte
  char bufp[2050];
  char retkey[100];
  char Key[20];
  int recdata;

  // demander Confirmation
  if ( Application->MessageBox("Cette Operation Efface la Base de Connaissances et Restore les donnees ",
        "Restoration de la Base de Connaissances", MB_OKCANCEL) == IDOK)
     {
       fd = fopen("Ecobase.txt","rb");
       if (fd)
        {
         if (db)
          { db->CloseEngine(); db->~realisam();
           unlink("Ecobase.NDX");
           unlink("Ecobase.DAT");
          }
           db = new realisam();
           db->OpenEngine("Ecobase",120,"ndx","dat");   //512 - 8
           db->SetIndexMode(0,0);
           // lecture du fichier s�quentiel
           while (!feof(fd))
             {  memset(bufp,0x00,2048);
                fgets(bufp,2040,fd);

                strncpy(Key,bufp,6); Key[5]=0;
                recdata = db->GetNewRecordNumber();
                rc = db->WriteKey(0,Key,recdata);
                if (rc)
                    {
                      db->WriteRecord(bufp,strlen(bufp)+1);
                    }
                 else ShowMessage("Write Error in DB : Duplicate key");

              }
            fclose (fd);
            Application->MessageBox("Op�ration Terminee",
        "Restoration de la Base de Connaissances", MB_OK);

        } // if fd
       else  Application->MessageBox("Fichier non trouve - Base non effacee ",
        "Restoration de la Base de Connaissances", MB_OK);
     }
}
//---------------------------------------------------------------------------




void __fastcall TMainForm::SauverComme1Click(TObject *Sender)
{
  //  possibilite de sauver sous un autre nom !
 strcpy(SAVEAS,"");
 strcpy(m_save,"saveas");


}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Calendriers1Click(TObject *Sender)
{
TfCalendar *Calend;
 Calend = new TfCalendar(Application);
      Calend->Caption = "Calendriers";
          Calend->ShowModal();
 delete Calend;
}
//---------------------------------------------------------------------------


int __fastcall TMainForm::Recherche_Window(AnsiString str)
{
 int result,i;  AnsiString TMP;
 result=-1;
 for (i=0;i<MDIChildCount;i++)
   {
     TMP= MDIChildren[i]->Caption;
     if (str==MDIChildren[i]->Caption) result=i;
   }
 return result;
   }
//---------------------------------------------------------------------------


void __fastcall TMainForm::Timer1Timer(TObject *Sender)
{
 int l,xl,indx;
 char caption[255];
 char tmp[250];  int lg,res;

 Timer1->Enabled = false;

if (FCLOSE)
   {
     FileExit1Execute(NULL);
     return;
   }


 l=strlen(m_status);
 if (flag_last) OpenLast();

 if (l)   // analyser status
   {
    //strcpy(caption,"Ecoplanning Version 5.0 ");
    strcpy(caption,m_ecoplan);
    xl=Ghost->ExtractValue(tmp,m_status,"create",0);
    if (xl) { strcat(caption,tmp);
               MainForm->Caption = AnsiString (caption);
               plST->Caption = AnsiString (caption);
               Insert_Project(tmp);
              }

    xl=Ghost->ExtractValue(tmp,m_status,"close",0);
    if (xl) {
              MainForm->Caption = AnsiString (caption);
              Delete_Project(tmp);
              FileOpenItem->Enabled=true;
              FileCloseItem->Enabled=false;
             }

    xl=Ghost->ExtractValue(tmp,m_status,"caption",0);
    if (xl) { strcat(caption,tmp);
              MainForm->Caption = AnsiString (caption);
              plST->Caption = AnsiString (caption);
              // and insert in history
              Insert_Project(tmp);

            }
    xl=Ghost->ExtractValue(tmp,m_status,"ch0",0);    //  changement
    if (xl) {
             Changed(tmp,0);
            }
    xl=Ghost->ExtractValue(tmp,m_status,"ch1",0);
    if (xl) {
             Changed(tmp,1);
            }
    // Minimize

    xl=Ghost->ExtractValue(tmp,m_status,"minim",0);  // nom de la fenetre a minimiser
    if (xl)
      {
       //  rechercher la MDI Child
       indx=Recherche_Window(AnsiString(tmp));
       if (indx >= 0)
          MDIChildren[indx]->WindowState=wsMinimized;
      }
   // Taille Normale
    xl=Ghost->ExtractValue(tmp,m_status,"normal",0);  // nom de la fenetre a restaurer
    if (xl)
      {
       //  rechercher la MDI Child
       indx=Recherche_Window(AnsiString(tmp));
       if (indx >= 0) { MDIChildren[indx]->WindowState=wsNormal; MDIChildren[indx]->SetFocus(); }
      }

    m_status[0]=0;
   }

  l=strlen(m_menu);
  if (l)
   {
    if (strcmp(m_menu,"ENACoches1")==0)
      { Cocher1->Visible=true; Cocher2->Visible=false; }
    if (strcmp(m_menu,"ENACoches2")==0)
      { Cocher2->Visible=true; Cocher1->Visible=false; }

    m_menu[0]=0;
   }

 timercnt++;
 if (timercnt>=20000)
    { lg = (rand() % 17)+2;
      if (CLEF) { res = check_key(lg); if (res) { Close();}
     }
     timercnt=0;
    }

 Timer1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Changed(char *nm,int val)
{
 int i;
 for (i=0;i<indx_proj;i++)
  {
   if (strcmp(nm,list_proj[i]) == 0)
    { stat_proj[i]=val; break;}
  }
  return;
 }



//int  indx_proj;

void __fastcall TMainForm::Insert_Project(char *nm)
{
 int i;
 bool trouve;
 trouve = false;
 for (i=0;i<indx_proj;i++)
  {
   if (strcmp(nm,list_proj[i]) == 0) { trouve=true; break;}
  }
 if (trouve==false) { strcpy(list_proj[indx_proj],nm); stat_proj[indx_proj]=0;indx_proj++; }

 return;
 }



void __fastcall TMainForm::Delete_Project(char *nm)
{
 int i;

 for (i=0;i<indx_proj;i++)
   {
    if (strcmp(nm,list_proj[i]) == 0) { list_proj[i][0]=0; break;}
   }

}
void __fastcall TMainForm::Aide1Click(TObject *Sender)
{
 Thelp *Aide;
 strcpy(m_help,"Sommaire.html");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;

}
//---------------------------------------------------------------------------


void __fastcall TMainForm::PlanchesVolets1Click(TObject *Sender)
{
 // Gestion des planches et volets
 TPlanches *pl;
 pl = new TPlanches(Application);
 pl->ShowModal();
 delete pl;
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::FiltrerAucuneTche1Click(TObject *Sender)
{
 strcpy(m_filtre,"<N>");  // <N> pour tout

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FiltrerTouteslesTches1Click(TObject *Sender)
{
 char tmp[250];
 strcpy(m_filtre,"<T>");  // <T> pour tout

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FiltrerTches1Click(TObject *Sender)
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

void __fastcall TMainForm::Copierlestchescoches1Click(TObject *Sender)
{
 strcpy(m_coche,"<C>");  // <C> copier les taches coch�es
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Supprimerlestchescoches1Click(TObject *Sender)
{
 char tmp[250];
 strcpy(m_coche,"<02>");  // D�cocher  // <S> supprimer les taches coch�es
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Critres1Click(TObject *Sender)
{
 TCriteres *Criteres;

 if (plST != NULL)
   {
    plST->CopyCriteres();
    Criteres = new TCriteres(Application);
    Criteres->Height=773;
    Criteres->Width=905;
    Criteres->ShowModal();
    delete Criteres;
    plST->CopyCritTSL_AV();
    plST->Remplir_AV1();
    plST->SaveCriteres(true);
   }
 else
   {
    Application->MessageBoxA("Pas de projet ouvert",m_ecoplan,MB_OK);
   }

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CocherTouteslestches1Click(TObject *Sender)
{
strcpy(m_coche,"<01>");      // cocher tout
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Dcochertouteslestches1Click(TObject *Sender)
{
strcpy(m_coche,"<02>");   // decocher tout
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::CocherDcocher2Click(TObject *Sender)
{
 strcpy(m_coche,"<03>");  // cocher decocher tout
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::CocherTouteslesTches2Click(TObject *Sender)
{
 strcpy(m_coche,"<21>");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DcocherTouteslesTches2Click(TObject *Sender)
{
 strcpy(m_coche,"<22>");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CocherDcocher1Click(TObject *Sender)
{
 strcpy(m_coche,"<23>");
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::TrierTches1Click(TObject *Sender)
{
 TTri *Tri;
 Tri = new TTri(Application);
 Tri->Height=619;
 Tri->Width=754;
 Tri->ShowModal(); //Modal();
 delete Tri;

}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Configurationimpression1Click(TObject *Sender)
{
 PrinterSetupDialog1 = new TPrinterSetupDialog(NULL);
 if (PrinterSetupDialog1->Execute())
   {



   }

}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Prparationimpressionplanning1Click(
      TObject *Sender)
{
 char x_ghost_prep[500];
 if (pc<0 || pc>3) pc=1;
strcpy(x_ghost_prep,"<pl>");strcat(x_ghost_prep, P_NOM[pc]); strcat(x_ghost_prep,"</pl>");
strcat(x_ghost_prep,"<org>Main:MenuPrepPlann</org>");
m_ghost_prep[pc] = AnsiString(x_ghost_prep);

 TPrepPlanning *PrepPlanning;
 PrepPlanning = new TPrepPlanning(Application);
 //PrepPlanning->Height = 735;
 //PrepPlanning->Width=742;
 PrepPlanning->ShowModal();
 delete PrepPlanning;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Prparationimpressionrapport1Click(
      TObject *Sender)
{
TPrepRapport *PrepRapport;
 PrepRapport = new TPrepRapport(Application);
 PrepRapport->ShowModal();
 delete PrepRapport;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Test1Click(TObject *Sender)
{
 //   Hide  les MDI
 for (int i=MDIChildCount-1;i>=0;i--)
   { MDIChildren[i]->WindowState=wsMinimized;
     //ShowMessage(MDIChildren[i]->Caption);
     //MDIChildren[i]->SetFocus(); //WindowState=wsMinimized;
   }



}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TestShow1Click(TObject *Sender)
{
for (int i=0; i<MDIChildCount;i++)
    MDIChildren[i]->WindowState=wsNormal;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ComplterCrerfiltresmmoriss1Click(
      TObject *Sender)
{
 TxFilter *filt;
 char tmp[250];
 char strfilter[100];

 strcpy(m_exchange,"<util>completer</util>");

 strcpy(m_project,DatabaseName);
 filt = new TxFilter(Application);
 filt->Height = 705;
 filt->Width = 774;
//  filt->Visible=true;
 m_filtre[0]=0;
 filt->ShowModal();  //Modal();
 delete filt;

}
//---------------------------------------------------------------------------


void __fastcall TMainForm::ImportExport1Click(TObject *Sender)
{
 strcpy(m_excel,"export");
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Variables1Click(TObject *Sender)
{
/*
 TDump *d;
 d=new TDump(Application);
 d->Height=800; d->Width=946;
 d->ShowModal();
 delete d;
*/
}

void __fastcall TMainForm::RAZ_Appli()
{
 int i;
 strcpy(DatabaseName,"");
 strcpy(ProjectName,"");
 for (i=0;i<6;i++)
   {
    strcpy(P_NOM[i],"");
    strcpy(P_TRI[i],"");
    strcpy(P_COLONNES[i],"");
    strcpy(P_FILTRE[i],"");
   }
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::InterfaceExcel1Click(TObject *Sender)
{

 TfProjet *Project;
 char name[255];
 char *p; int i,w,h;

/*
if (strlen(DatabaseName))
  { Application->MessageBoxA("Veuillez fermer le projet en cours",m_ecoplan,MB_OK);
    return;
  }
 */

 if (strlen(DatabaseName))
    {
     for(i = MDIChildCount-1; i >= 0; i--) MDIChildren[i]->Close();
     Build_History();
     WriteLast();
     RAZ_Appli();

    }

 strcpy(m_exchange,"import");
 Project = new TfProjet(Application);
      Project->Caption = "Importation Donn�es CSV";
          Project->ShowModal();
 delete Project;

 if (Cancel==1) return;

 Inserer_Menu(DatabaseName);
  // nom dans Database Name
 plST = new TplStandard(Application);
 plST->Caption = AnsiString(DatabaseName);
  w = Screen->Width;
  h=  Screen->Height;

  /*
  if (w>=1024)
   {
    plST->Width = w*1000/1024;
    plST->Height = h*700/768;
   }
  */

  plST->Width = WPLST;
 plST->Height = HPLST;

  Filtres1->Visible=true;
  Critres1->Visible = true;
  Cocher1->Visible = true;
  //TrierTches1->Visible=true;
  Renumrotertches1->Visible=false;
  Prparationimpressionrapport1->Visible=true;
  Prparationimpressionplanning1->Visible=true;
  Simulation1->Visible=true;
  Rsumduprojet1->Visible=true;
  Datededbutduprojet1->Visible=true;
  Critres2->Visible = true;
  FileCloseItem->Visible = true;
  FileOpenItem->Visible = false;

 Sauver1->Enabled=true;
 SauverComme1->Enabled=true;
 // plST->ClientHeight=759;
 strcpy(m_excel,"import");
}

//---------------------------------------------------------------------------


int __fastcall TMainForm::Extraire_CVS(char *fname)
{
  int rc;
  int recdata;
  char buffer[2000];
  char RetKey[100];
  char Key[100];
  char tmp[2000];
  char format[500];
  int ipred,isucc;
  int ii,cnt;
  char ss[20];
  char strpred[1000],strsucc[1000];
  char name[256];
  char dbtitre[200];
  FILE *fp,*fcrit;
  int nbpar,i;
  char str[150];
  int numtache;
  int cntache;


    strcpy(name,DatabaseName);
    strcpy(buffer,name); strcat(buffer,".dat");
    unlink(buffer);
    strcpy(buffer,name); strcat(buffer,".eco");
    unlink(buffer);

    proj = new realisam;
    rc = proj->OpenEngine(name,120,"eco","dat");   //512 - 8

    if (rc==0) { ShowMessage("Erreur � l'ouverture des fichiers projets"); return 0; }
     proj->SetIndexMode(0,0);
    // titre de la base
     strcpy(dbtitre,DatabaseName);
     strcpy(Key,"<dbtitre>"); recdata=proj->GetNewRecordNumber();
     proj->WriteKey(0,Key,recdata);
     proj->WriteRecord(dbtitre,strlen(dbtitre)+1);

    fp = fopen(fname,"rb");
    strcat(name,"_affect.txt");
    fcrit = fopen(name,"wb");

    if (fp==NULL)
      {
       Application->MessageBoxA("Erreur � l'ouverture du fichier CSV",m_ecoplan,MB_OK);
       return 0;
      }

    cntache=0;
    buffer[0]=0;
    fgets(buffer,2000,fp);  // 1 er record  // colonnes

    while (!feof(fp))
      {

       strcat(buffer,",");
       nbpar=Explode(',',buffer);
       for (i=0;i<nbpar;i++) removequotes(params[i]);
       // fariquer l'enregistrement

       numtache = atoi(params[0]);
       if (numtache !=0)
        {
         sprintf(Key,"<T%04d>",numtache);
         recdata=proj->GetNewRecordNumber();
         proj->WriteKey(0,Key,recdata);

         strcpy(format,"<T>%d</T><nom>%s</nom><duree>%s</duree><cal>%s</cal><mom>%s</mom>");
         strcat(format,"<ct>%s</ct><da>%s</da><dtr>%s</dtr><ftr>%s</ftr><pc>%s</pc><nbj>%s</nbj>");
         strcat(format,"<dds>%s</dds><dfs>%s</dfs>");
         strcat(format,"<nbpred>%s</nbpred><nbsucc>%s</nbsucc>");
         strcat(format,"<pred>%s</pred><succ>%s</succ><coutp>%s</coutp><coutn>%s</coutn>");
         sprintf(tmp,format,
           numtache,
           params[1],   // design
           params[2],   // duree
           params[5],   // cal,
           "0",         // Mom
           "",          //codetache[i],
           "",          //da[i],
           params[3],    // deb+tot
           params[4],    // fin+tot
           "",           //pca[i],
           "",           //nbj[i],
           "",           //dds[i],
           "",           //dfs[i],
           params[8],    // nbpred,
           params[10],   // nbsucc,
           params[9],    // strpred,
           params[11],   // strsucc,
           params[6],    //av->Cells[18][i],      /// couts pos et neg
           params[7]     //av->Cells[19][i]
           );
         proj->WriteRecord(tmp,strlen(tmp)+1);

         // preparer le fichier criteres
         tmp[0]=0;
         strcat(tmp,"<nt>"); strcat(tmp,params[0]); strcat(tmp,"</nt>");
         strcat(tmp,"<des>"); strcat(tmp,params[1]); strcat(tmp,"</des>");
         strcat(tmp,"<c1>"); strcat(tmp,params[12]); strcat(tmp,"</c1>");
         strcat(tmp,"<d1>"); strcat(tmp,params[20]); strcat(tmp,"</d1>");
         strcat(tmp,"<c2>"); strcat(tmp,params[13]); strcat(tmp,"</c2>");
         strcat(tmp,"<d2>"); strcat(tmp,params[21]); strcat(tmp,"</d2>");
         strcat(tmp,"<c3>"); strcat(tmp,params[14]); strcat(tmp,"</c3>");
         strcat(tmp,"<d3>"); strcat(tmp,params[22]); strcat(tmp,"</d3>");
         strcat(tmp,"<c4>"); strcat(tmp,params[15]); strcat(tmp,"</c4>");
         strcat(tmp,"<d4>"); strcat(tmp,params[23]); strcat(tmp,"</d4>");
         strcat(tmp,"<c5>"); strcat(tmp,params[16]); strcat(tmp,"</c5>");
         strcat(tmp,"<d5>"); strcat(tmp,params[24]); strcat(tmp,"</d5>");
         strcat(tmp,"<c6>"); strcat(tmp,params[17]); strcat(tmp,"</c6>");
         strcat(tmp,"<d6>"); strcat(tmp,params[25]); strcat(tmp,"</d6>");
         strcat(tmp,"<c7>"); strcat(tmp,params[18]); strcat(tmp,"</c7>");
         strcat(tmp,"<d7>"); strcat(tmp,params[26]); strcat(tmp,"</d7>");
         strcat(tmp,"<c8>"); strcat(tmp,params[19]); strcat(tmp,"</c8>");
         strcat(tmp,"<d8>"); strcat(tmp,params[27]); strcat(tmp,"</d8>");
         strcat(tmp,"\n");
         fputs(tmp,fcrit);


         cntache++;
         }  // numtache !=0
       buffer[0]=0;
       fgets(buffer,2000,fp);
      }
    fclose(fp);
    fclose(fcrit);

    // date previsionnelle

    struct date d;

    getdate(&d);
    sprintf(tmp,"%02d/%02d/%04d",d.da_day,d.da_mon,d.da_year);


     strcpy(Key,"<dateprevdebut>"); recdata=proj->GetNewRecordNumber();
     proj->WriteKey(0,Key,recdata);
     proj->WriteRecord(tmp,strlen(tmp)+1);


     // nombre de taches

     strcpy(Key,"<taches>"); recdata=proj->GetNewRecordNumber();
     proj->WriteKey(0,Key,recdata);
     sprintf(tmp,"%04d",cntache);
     proj->WriteRecord(tmp,strlen(tmp)+1);

    proj->CloseEngine();
 return 1;

}


void __fastcall TMainForm::removequotes(char *str)
{
 char tmp[200];
 char *p;
 int lg;
 if (strlen(str)==0) return;
 strcpy(tmp,str);  p=strchr(tmp+1,'"'); if (p==NULL) return;
 *p=0;
 lg=strlen(tmp);
 memmove(str,tmp+1,lg);
}

int __fastcall TMainForm::Explode(char sep,char *str)
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
     { params[count][ip]=0; count++; ip=0; if (count > 49) count=49; }
   else
     { params[count][ip]=c;  ip++; }
   i++; c=str[i];
  }
 if (cpred==sep) count--; return (count+1);
}



void __fastcall TMainForm::File1Click(TObject *Sender)
{
  int Pos,i;
 int indx;  char name[MAX_PATH];
 // traitement ouverture projets
 Pos = File1->IndexOf((TMenuItem *)Sender);
 // Name = File1->Strings[Pos];

 //
 if (Pos != -1)
  {
   if (Pos <pos) return;
   if (Pos>(pos+count)) return;
   indx = Pos-pos;
   strcpy(name,hpath[indx]);
   if (strcmp(name,DatabaseName)==0) return;

   for(i = MDIChildCount-1; i >= 0; i--) MDIChildren[i]->Close();
   RAZ_Appli();
   strcpy(DatabaseName,hpath[indx]);
   NouveauProjet1->Enabled=false;
   FileOpenItem->Enabled=false;
   FileCloseItem->Enabled=true;
   Sauver1->Enabled=true;
   SauverComme1->Enabled=true;
   First(DatabaseName);
   Launch(DatabaseName);
  }

}
//---------------------------------------------------------------------------



void __fastcall TMainForm::Rsumduprojet1Click(TObject *Sender)
{
 TResume *Resum;
 Resum = new TResume(Application);
 Resum->ShowModal();
 delete Resum;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Datededbutduprojet1Click(TObject *Sender)
{
TDateProjet *DPrj;
DPrj = new TDateProjet(Application);
DPrj->ShowModal();
delete DPrj;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Affecterdonnetachescoches1Click(TObject *Sender)
{

/*
 TAffData *af;
 af = new TAffData(Application);
 af->ShowModal();
 delete af;
*/
 strcpy(m_coche,"<V>");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Simulation1Click(TObject *Sender)
{
 if (VERSION)
   {
    TSimulOpen *so;
    so = new TSimulOpen(Application);
    so->ShowModal();
    delete so;
   }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Complterdsignationdestches1Click(
      TObject *Sender)
{
 /*
 TRemplTC *rt;
 rt= new TRemplTC(Application);
 rt->ShowModal();
 delete rt;
 */
 strcpy(m_coche,"<T>");
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::Lier1Click(TObject *Sender)
{
 strcpy(m_coche,"<L>");
  // lier
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Dlier1Click(TObject *Sender)
{
 // delier
 strcpy(m_coche,"<D>");

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileCloseItemClick(TObject *Sender)
{
 int i;
 for(i = MDIChildCount-1; i >= 0; i--)
  MDIChildren[i]->Close();
  Build_History();
  FileOpenItem->Enabled=true;
  WriteLast();
  RAZ_Appli();
  if (plST) plST=NULL;
  strcpy(ProjectName,"");
  NouveauProjet1->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Sauver1Click(TObject *Sender)
{
 strcpy(m_save,"save");

//plST->updatedatabase(LocalBase,0,0);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::OpenLast()
{
 FILE *fp;  char fn[MAX_PATH]; int i;
 char tmp[512];
 char prjname[MAX_PATH];

 flag_last=0;
 strcpy(LastAppli,"");
 for (i=0;i<6;i++) strcpy(LastPlanning[i],"");
 //strcpy(LastRapport,"");
 strcpy(LastTri,"");
 strcpy(LastCol,"");
 strcpy(LastFilter,"");



 strcpy(fn,m_directory); strcat(fn,"\\lastopen.txt");
 fp=fopen(fn,"rt");
 if (fp)
  {
   fread(tmp,500,1,fp);
   Ghost->ExtractValue(LastAppli,tmp,"app",0);
   strcpy(prjname,LastAppli); strcat(prjname,".eco");
   if (FileExists(AnsiString(prjname)) && strlen(LastAppli))
     {
       Ghost->ExtractValue(LastPlanning[1],tmp,"pl1",0);
       strcpy(P_NOM[1],LastPlanning[1]);
       Ghost->ExtractValue(LastPlanning[2],tmp,"pl2",0);
       strcpy(P_NOM[2],LastPlanning[2]);
       Ghost->ExtractValue(LastPlanning[3],tmp,"pl3",0);
       strcpy(P_NOM[3],LastPlanning[3]);
       Ghost->ExtractValue(LastPlanning[4],tmp,"pl4",0);
       strcpy(P_NOM[4],LastPlanning[4]);
       Ghost->ExtractValue(LastPlanning[5],tmp,"pl5",0);
       strcpy(P_NOM[5],LastPlanning[5]);
       // Ghost->ExtractValue(LastRapport,tmp,"rp",0);
       Ghost->ExtractValue(LastTri,tmp,"tr",0);
       Ghost->ExtractValue(LastCol,tmp,"cl",0);
       Ghost->ExtractValue(LastFilter,tmp,"ft",0);


       for (pc=1;pc<6;pc++)       // PC = indice des tableaux
        {
         AUTOMATE[pc]=' ';
         if (strlen(LastPlanning[pc]))
          {
           if (pc<4)
              Ghost->Global_Planning(LastPlanning[pc]);
            else  Ghost->Global_Report(LastPlanning[pc]);
          }
        }


       pc=1;
       strcpy(DatabaseName,LastAppli);
       NouveauProjet1->Enabled=false;
       FileOpenItem->Enabled=false;
       FileCloseItem->Enabled=true;
       Sauver1->Enabled=true;
       SauverComme1->Enabled=true;
       Launch(DatabaseName);
       First(DatabaseName);
       }
    }
   else
    {
     if (strlen(LastAppli))
       {
       Application->MessageBoxA("Le Dernier Projet n'est plus disponible",m_ecoplan,MB_OK);
       }
    }

   fclose(fp);


  return;
 }


void __fastcall TMainForm::Renumrotertches1Click(TObject *Sender)
{
 strcpy(m_renum,"YES");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Calendrier1Click(TObject *Sender)
{
TfCalendar *Calend;
 Calend = new TfCalendar(Application);
      Calend->Caption = "Calendriers";
          Calend->ShowModal();
 delete Calend;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Critres2Click(TObject *Sender)
{
 TCriteres *Criteres;
 Criteres = new TCriteres(Application);
 Criteres->Height=652;
 Criteres->Width=882;
 Criteres->ShowModal(); //Modal();
 delete Criteres;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Affecteruncalendrier1Click(TObject *Sender)
{
 strcpy(m_coche,"<K>");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Crertachesamont1Click(TObject *Sender)
{
 strcpy(m_coche,"<A>");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Affecterplusieursprdcesseurs1Click(
      TObject *Sender)
{
 strcpy(m_coche,"<X>");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Affecterplusieurssuccesseurs1Click(
      TObject *Sender)
{
 strcpy(m_coche,"<Y>");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UtiliserTrierGrouper1Click(TObject *Sender)
{
  TTri *Tri;

  strcpy(m_exchange,"<util>utiliser</util><tri>");
  strcat(m_exchange,P_TRI[pc]);
  strcat(m_exchange,"</tri>");

 Tri = new TTri(Application);
 Tri->Height=619;
 Tri->Width=754;
 Tri->ShowModal();
 delete Tri;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CrerModifierSupprimerTrisGroupes1Click(
      TObject *Sender)
{
 TTri *Tri;

 strcpy(m_exchange,"<util>completer</util><tri>");
  strcat(m_exchange,P_TRI[pc]);
 strcat(m_exchange,"</tri>");
 Tri = new TTri(Application);
 Tri->Height=619;
 Tri->Width=754;
 Tri->ShowModal();
 delete Tri;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CrationRpartitionColonnes1Click(TObject *Sender)
{
TColonnes *Col;
 strcpy(m_exchange,"<col>"); strcat(m_exchange,P_COLONNES[pc]); strcat(m_exchange,"</col>");
 strcat(m_exchange,"<util>completer</util>");
 Col = new TColonnes(Application);
 Col->Height=705;
 Col->Width=778;

 Col->ShowModal();
 delete Col;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ImportPlanning1Click(TObject *Sender)
{
 TImport *Imp;
 strcpy(m_impt,"<type>"); strcat(m_impt,"pln"); strcat(m_impt,"</type>");

 Imp = new TImport(Application);
 Imp->Caption = "Import Donn�es (Plannings,Tris,Filtres,Colonnes)";
 Imp->ShowModal();
 delete Imp;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ImportTri1Click(TObject *Sender)
{
TImport *Imp;
 strcpy(m_impt,"<type>"); strcat(m_impt,"tri"); strcat(m_impt,"</type>");

 Imp = new TImport(Application);
 Imp->Caption = "Import Donn�es Tri";
 Imp->ShowModal();
 delete Imp;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ImportFiltre1Click(TObject *Sender)
{
TImport *Imp;
 strcpy(m_impt,"<type>"); strcat(m_impt,"fil"); strcat(m_impt,"</type>");

 Imp = new TImport(Application);
 Imp->Caption = "Import Donn�es Filtre";
 Imp->ShowModal();
 delete Imp;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ImportColonnes1Click(TObject *Sender)
{
TImport *Imp;
 strcpy(m_impt,"<type>"); strcat(m_impt,"col"); strcat(m_impt,"</type>");

 Imp = new TImport(Application);
 Imp->Caption = "Import Donn�es Colonnes";
 Imp->ShowModal();
 delete Imp;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::Rsolution1024x7681Click(TObject *Sender)
{
 strcpy(m_resize,"<w>1024</w><h>768</h>");
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::RsolutionMaximum1Click(TObject *Sender)
{
 int w,h;
 char tmp[200];

 w = Screen->Width;
 h = Screen->Height;
 sprintf(tmp,"<w>%d</w><h>%d</h>",w,h);
 strcpy(m_resize,tmp);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExportFichierExcel1Click(TObject *Sender)
{
 strcpy(m_excel,"expCSV");

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExportExcelCritres1Click(TObject *Sender)
{
strcpy(m_excel,"expCRIT");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExportExcelCotsetRessources1Click(
      TObject *Sender)
{
strcpy(m_excel,"expCOUT");
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RemiseZro1Click(TObject *Sender)
{
 strcpy(m_zero,"xxxxxxxxxxxxxx");
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::ImportFormatCSV1Click(TObject *Sender)
{

 TfProjet *Project;
 char name[255];
 char *p; int i,w,h;

/*
if (strlen(DatabaseName))
  { Application->MessageBoxA("Veuillez fermer le projet en cours",m_ecoplan,MB_OK);
    return;
  }
 Application->MessageBoxA("Pas encore op�rationnel",m_ecoplan,MB_OK);
    return;
*/

 if (strlen(DatabaseName))
    {
     for(i = MDIChildCount-1; i >= 0; i--) MDIChildren[i]->Close();
     Build_History();
     WriteLast();
     RAZ_Appli();

    }

 strcpy(m_exchange,"importCSV");
 Project = new TfProjet(Application);
      Project->Caption = "Importation Donn�es CSV";
          Project->ShowModal();
 delete Project;

 if (Cancel==1) return;

 Inserer_Menu(DatabaseName);
  // nom dans Database Name
 plST = new TplStandard(Application);
 plST->Caption = AnsiString(DatabaseName);
  w = Screen->Width;
  h=  Screen->Height;

  /*
  if (w>=1024)
   {
    plST->Width = w*1000/1024;
    plST->Height = h*700/768;
   }
  */

  plST->Width = WPLST;
  plST->Height = HPLST;

  Filtres1->Visible=true;
  Critres1->Visible = true;
  Cocher1->Visible = true;
  //TrierTches1->Visible=true;
  Renumrotertches1->Visible=false;
  Prparationimpressionrapport1->Visible=true;
  Prparationimpressionplanning1->Visible=true;
  Simulation1->Visible=true;
  Rsumduprojet1->Visible=true;
  Datededbutduprojet1->Visible=true;
  Critres2->Visible = true;
  FileCloseItem->Visible = true;
  FileOpenItem->Visible = false;

 Sauver1->Enabled=true;
 SauverComme1->Enabled=true;
 // plST->ClientHeight=759;
 strcpy(m_excel,"import_CSV");
}
//---------------------------------------------------------------------------


/*
 TFlashWInfo *FWinfo;

 FWinfo = new TFlashWInfo;
 FWinfo->cbSize = 20;
 FWinfo->hwnd = Application->Handle; // Handle of Window to flash
 FWinfo->dwFlags = FLASHW_ALL;
 FWinfo->uCount = 100; // number of times to flash
 FWinfo->dwTimeout = 0; // speed in ms, 0 default blink cursor rate
 FlashWindowEx(FWinfo); // make it flash!
*/

//---------------------------------------------------------------------------

void __fastcall TMainForm::Enregistrer1Click(TObject *Sender)
{

 TRegist *reg;
 reg= new TRegist(Application);
 reg->ShowModal();
 delete reg;

}
//---------------------------------------------------------------------------


int __fastcall   TMainForm::InitKey()
{
 int ct,i,j;
 /*
 strcpy(APP_NAME,"Ecoplanning");
  strcpy(APP_GUID_STR,"265F7550-F85C-11d3-B981-0020AFDB9414");
  // Default SO PIN and User PIN set by Rainbow Manufacturing
  strcpy(SO_PIN,"rainbow");
  SO_PIN_LEN= strlen(SO_PIN); //  = (sizeof(SO_PIN)-sizeof(unsigned char));
  USER_PIN[0]= 0x78;
  USER_PIN[1]= 0x56;
  USER_PIN[2]= 0x34;
  USER_PIN[3]= 0x12;
  // little endian format
  USER_PIN_LEN   = strlen(USER_PIN);

  // init Files

  // file 0 and 1 are reserved for token Name
  // and  number of uses

  //  GENERATES random files
  ct=32;
  srand(31255);
  for (i=2;i<20;i++)
   {
    for (j=0;j<30;j++)         // 250
      {
       filex[i][j] = rand() % 64 + 32;
      }
    filex[i][j]=0;
    //  hash table
    switch (i)
      {
       case  2: filex[i][i]=ct+4 ; break;            // i*5  trop grand ==> i
       case  3: filex[i][i]=ct+8 ; break;
       case  4: filex[i][i]=ct+9 ; break;
       case  5: filex[i][i]=ct+12 ; break;
       case  6: filex[i][i]=ct+5 ; break;
       case  7: filex[i][i]=ct+11 ; break;
       case  8: filex[i][i]=ct+19 ; break;
       case  9: filex[i][i]=ct+3 ; break;
       case 10: filex[i][i]=ct+16 ; break;
       case 11: filex[i][i]=ct+15 ; break;
       case 12: filex[i][i]=ct+17 ; break;
       case 13: filex[i][i]=ct+10 ; break;
       case 14: filex[i][i]=ct+14 ; break;
       case 15: filex[i][i]=ct+7 ; break;
       case 16: filex[i][i]=ct+18 ; break;
       case 17: filex[i][i]=ct+2 ; break;
       case 18: filex[i][i]=ct+13 ; break;
       case 19: filex[i][i]=ct+6 ; break;
      }
   }

//  PageControl1->ActivePageIndex=0;
//  Button2->Enabled = false;

        // Global flag used for selecting a device.
  DeviceSelected = 0;
  handle = NULL;
  devicesAvailable = 0;
  devicesInUse     = 0;
  tokenName[0]    = 0; //{""};


  status = ikey_CreateContext( &handle,         // handle created
                               0,               // flags
                               IKEY_API_VERSION // library version
                               );

 if ( RB_SUCCESS == status ) { }
  else
   {
    Application->MessageBoxA("Erreur Sur Cl� de Protection",m_ecoplan,MB_OK);
    exit(1); //Close();
   }
    status = CountDevices( handle,0,NULL,NULL,NULL,&devicesAvailable );
 if ( RB_SUCCESS != status && RB_UNIT_NOT_FOUND != status )
      {
       Application->MessageBoxA("Cl� de protection non trouv�e",m_ecoplan,MB_OK);
      }
 if (devicesAvailable==0)
      { Application->MessageBoxA("Pas de cl� de protection",m_ecoplan,MB_OK);
        return 0;  // exit(1); // return;
      }
 if (devicesAvailable>1)
      {
       Application->MessageBoxA("Plusieurs Cl�s de Protection",m_ecoplan,MB_OK);
        return 0; //return;
      }
  int count = devicesAvailable;

  status = ikey_OpenDevice( handle,IKEY_OPEN_FIRST, NULL );



status = ikey_ChangeDir( handle,IKEY_DIR_BY_LONG_ID,IKEY_ROOT_DIR, NULL );

 memset( &dirInfo, 0, sizeof(dirInfo) );
         dirInfo.Id           = 0;           // start id for auto create
         dirInfo.Flags        = 0;           // reserved, set to zero.
         dirInfo.MaxSize      = DIRECTORY_SIZE;      // max space required
     dirInfo.MaxEntries   = NUMBER_OF_FILES;


strcpy(APP_NAME,"Ecoplanning");
status = ikey_ChangeDir( handle,IKEY_DIR_BY_LONG_ID,61440,APP_NAME);



// ====================================================
    datalen= 32;  // 256;

    fileInfo.Id          = 0;        // the file ID of the file to read
    fileInfo.FileSize    = datalen;                 // file size, must be word align
    fileInfo.FileType    = IKEY_FILETYPE_DATA;  // type of file
    fileInfo.ReadAccess  = IKEY_ACCESS_ANYONE;  // read access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.WriteAccess = IKEY_ACCESS_ANYONE;  // write access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.CryptAccess = IKEY_ACCESS_ANYONE;  // data files always have this crypt access


 status = ikey_OpenFile( handle,(IKEY_FILE_READ), 0,&fileInfo, sizeof(fileInfo));

 if ( RB_SUCCESS != status )
   { Application->MessageBoxA("Erreur  Lecture fichier cl�.",m_ecoplan,MB_OK);
           exit(1); }

 status = ikey_Read( handle,0,0, dataf,fileInfo.FileSize, &bytesRead);

  if ( RB_SUCCESS != status )
     { Application->MessageBoxA("Erreur sur lecture Fichier Cl� de Protection.",m_ecoplan,MB_OK);
       exit;
     }
 strcpy(sserial,dataf);

// ===================================================

    datalen= 32;  //256;

    fileInfo.Id          = 1;        // the file ID of the file to read
    fileInfo.FileSize    = datalen;                 // file size, must be word align
    fileInfo.FileType    = IKEY_FILETYPE_DATA;  // type of file
    fileInfo.ReadAccess  = IKEY_ACCESS_ANYONE;  // read access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.WriteAccess = IKEY_ACCESS_ANYONE;  // write access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.CryptAccess = IKEY_ACCESS_ANYONE;  // data files always have this crypt access


 status = ikey_OpenFile( handle,(IKEY_FILE_READ),1,&fileInfo,sizeof(fileInfo));

 if ( RB_SUCCESS != status )
     {  Application->MessageBox("Erreur a l'ouverture d'un fichier sur la Cl�.",m_ecoplan,MB_OK);
        exit(1);
        }

 status = ikey_Read( handle,0,0,dataf,fileInfo.FileSize,&bytesRead );

  if ( RB_SUCCESS != status )
     { //DError( status, "Error in reading IkeyData from file." );
       //Close();
       Application->MessageBox("Erreur a la lecture d'un fichier sur la Cl�.",m_ecoplan,MB_OK);
       return 0; //exit(1);
     }



  status = ikey_CloseFile( handle );
   if ( RB_SUCCESS != status )
     {
       Application->MessageBox("Erreur a la fermeture d'un fichier sur la Cl�.",m_ecoplan,MB_OK);
       return 0; //exit(1);
     }
*/
return 1;
}



int __fastcall TMainForm::check_key(int val)
{
 // int playcode;
 /*
 request=val;

 if (CLEF==0) return 0;
   // process the request
   // read the file
    datalen= 32; // 256;

    fileInfo.Id          = request;        // the file ID of the file to read
    fileInfo.FileSize    = datalen;                 // file size, must be word align
    fileInfo.FileType    = IKEY_FILETYPE_DATA;  // type of file
    fileInfo.ReadAccess  = IKEY_ACCESS_ANYONE;  // read access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.WriteAccess = IKEY_ACCESS_ANYONE;  // write access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.CryptAccess = IKEY_ACCESS_ANYONE;  // data files always have this crypt access


 status = ikey_OpenFile( handle,     // handle to the device
         (IKEY_FILE_READ), // flags, set for read access
          request,           //DATA_FILE_ID,     // file id to open
          &fileInfo,        // file information returned
          sizeof(fileInfo)  // size of the file information block
        );

 if ( RB_SUCCESS != status )
  { Application->MessageBox("Erreur lecture fichier Cl� ou Cl� absente.",m_ecoplan,MB_OK);
    return 1;
  }
 status = ikey_Read( handle,         // handle to the file
          0,              // flags, reserved set to zero.
          0,              // offset to read from
          dataf,         // buffer to receive data
          fileInfo.FileSize, // amount of data to read, returned from open
          &bytesRead      // number of bytes read
         );

  if ( RB_SUCCESS != status )
   { Application->MessageBox("Erreur lecture fichier Cl� ou Cl� absente.",m_ecoplan,MB_OK);
     return 1;
   }
  status = ikey_CloseFile( handle );

  /*
  if (strcmp(dataf,filex[request])== 0)      // egalite !!!
    {
     playcode = filex[request][request]-32;  // [5*request] - 32;
    }
  */

 return 0;
  /*
 case  2 => 4 ;         Create Event Buff_Done
 case  3 =>8 ;          Create Event Buff_Play
 case  4 =>9 ;          OpenWaveIn();
 case  5 =>12 ;         StartRecording()
 case  6 =>5 ;          AllowButtons
 case  7 =>11 ;         ComputeSize()
 case  8 =>19 ;         DisplayWav()
 case  9 =>3 ;          DisplayText()
 case 10 =>16 ;         SaveTextFile()
 case 11 =>15 ;         InitGraphics()
 case 12 =>17 ;         Application->ProcessMessages()
 case 13 =>10 ;         ComputeStart()
 case 14 =>14 ;         SavePrintFile()
 case 15 =>7 ;          ResetButtons (0) //
 case 16 =>18 ;         Application->ProcessMessages()
 case 17 =>2 ;          DrawTest;
 case 18 =>13 ;         LoadColors()
 case 19 =>6 ;          LoadTextFile()


     switch (playcode)
       {
        case 2: DrawTest(); break;
        case 3: DisplayText(); break;
        case 4: Buff_Done = CreateEvent(0,FALSE,FALSE,0); break;
        case 5: AllowButtons(); break ;
        case 6: LoadTextFile(); break;
        case 7: ResetButtons(0); break;
        case 8: Buff_Play = CreateEvent(0,FALSE,FALSE,0); break;
        case 9: OpenWaveIn(); break;
        case 10: ComputeStart(); break;
        case 11: ComputeSize(); break;
        case 12: StartRecording(); break;
        case 13: LoadColors(); break;
        case 14: SavePrintFile(); break;
        case 15: InitGraphics(); break;
        case 16: SaveTextFile(); break;
        case 17: Application->ProcessMessages(); break;
        case 18: Application->ProcessMessages(); break;
        case 19: DisplayWav(); break;
       }
    }
   else // display erreur
    {
     Application->MessageBox(W[102].c_str(),"Lantern Professional",MB_OK);
     //Application->MessageBoxA("Protection Key Not detected - Restart your Computer","Diogenes", MB_OK);
     while (1) sleep(60);
    }
    request =0;
  }
 request=0;
*/
}

int __fastcall TMainForm::DecCounter()
{
  /*
  char tmp[MAX_PATH];

   if (CLEF==0) return 0x7fff;

   strcpy(APP_NAME,"Ecoplanning");
   strcpy(APP_GUID_STR,"7BD2D0A9-F91E-11d3-912C-00C04F607622");

  status = ikey_OpenFile( handle,     // handle to the device
         (IKEY_FILE_READ), // flags, set for read access
          1,           //DATA_FILE_ID,     // file id to open
          &fileInfo,        // file information returned
          sizeof(fileInfo)  // size of the file information block
        );

   if ( RB_SUCCESS != status )
     {
        Application->MessageBox("Erreur Ouverture fichier Cl� ou Cl� absente.",m_ecoplan,MB_OK);
        return 0;
     }

   strcpy(tmp,DatabaseName);   ////   ikey_Read Destroys CurrentDir !!!!!!!!!!!!!!!!!
   status = ikey_Read( handle,         // handle to the file
          0,              // flags, reserved set to zero.
          0,              // offset to read from
          nb_uses,         // buffer to receive data
          fileInfo.FileSize, // amount of data to read, returned from open
          &bytesRead      // number of bytes read
         );
   strcpy(DatabaseName,tmp);

  if ( RB_SUCCESS != status )
     {
       Application->MessageBox("Erreur Lecture Fichier Clef de Protection.",m_ecoplan,MB_OK);
        return 0;
     }
  nb_tries=atoi(nb_uses);

  if (nb_tries < 50000)
   {
    nb_tries--;
    if (nb_tries < 0)
       { nb_tries = 0;
         Application->MessageBox("Clef de ptrotection : P�riode d'essai termin�e.",m_ecoplan,MB_OK);
         // trial period expired
         Close();
       }
   datalen = 32;

//  Delete the file

   status = ikey_DeleteFile(handle,0,1); // effacement du fichier
   /////////////////////

    memset( &fileInfo, 0, sizeof(fileInfo) );
    // len = DATA_LEN;
    fileInfo.Id          = 1; // DATA_FILE_ID;        // the file ID of the file to create
    fileInfo.FileSize    = datalen;                 // file size, must be word align
    fileInfo.FileType    = IKEY_FILETYPE_DATA;  // type of file
    fileInfo.ReadAccess  = IKEY_ACCESS_ANYONE;  // read access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.WriteAccess = IKEY_ACCESS_ANYONE;  // write access - ALWAYS, NEVER, USER PIN, or SO-PIN
    fileInfo.CryptAccess = IKEY_ACCESS_ANYONE;  // data files always have this crypt access
    status = ikey_CreateFile( handle,           // handle to the device
                              0,                // flags, reserved set to zero
                              &fileInfo,        // File info to create
                              sizeof(fileInfo)  // Size of file info structure
                        );

   if ( RB_SUCCESS != status )
     {
       Application->MessageBox("Erreur R�-�criture fichier Clef.",m_ecoplan,MB_OK);
       return 0;
     }

  sprintf(nb_uses,"%06d",nb_tries);
  status = ikey_Write( handle,            // handle to the device
                       0,                 // flags,
                       0,                 // offset
                       nb_uses,              // data to write
                       datalen,               // length of data
                       &bytesWritten      // number of bytes written
                      );

  status = ikey_CloseFile( handle );
  if (nb_tries < 50000)
     // Label100->Caption = AnsiString(nb_uses);
     StatusBar->SimpleText = "Nombre d'essais restants: " + AnsiString(nb_uses);
  }
 // return 0x7FFF if OK, else return 0;
 */
 return 0x7fff;
}

void __fastcall TMainForm::FormShow(TObject *Sender)
{
 Timer1->Enabled=true;

 /*
 if (!Check_Config())
   {
    Close();
   }
 */

   
}
//---------------------------------------------------------------------------

