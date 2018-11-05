//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "modTaches.h"
#include "globals.h"
#include "modChCol.h"
#include "modStyle.h"
#include "modHelp.h"
#include "modEpaisseur.h"
#include <windows.h>
#include <Lmcons.h>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGrid"
#pragma link "BaseGrid"
#pragma resource "*.dfm"
TFDtaches *FDtaches;
//---------------------------------------------------------------------------
__fastcall TFDtaches::TFDtaches(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFDtaches::btOKClick(TObject *Sender)
{
char username[UNLEN+1];
DWORD username_len = UNLEN+1;
GetUserName(username, &username_len);
AnsiString c = "C:\\Users\\";
AnsiString d ="\\Desktop\\Rapport";
AnsiString e = AnsiString(username);

int jk = 1;

AppendStr(c,e);
AppendStr(c,d);

 int i; char tmp[250];

 P_TRAIT[pc][indice]=avv->Cells[1][1];
 P_EPTRAIT[pc][indice]=avv->Cells[1][2] ;
 P_MARGE[pc][indice]=avv->Cells[1][3];
 P_STYLEM[pc][indice]= avv->Cells[1][4];
 P_COLMARGE[pc][indice]=avv->Cells[1][5];
 P_LIENS[pc][indice] = avv->Cells[1][6];
 P_STYLELIEN[pc][indice]= avv->Cells[1][7];
 P_COL_LIEN[pc][indice]=avv->Cells[1][8];
 P_EPAIS_LIEN[pc][indice]=avv->Cells[1][9];


 for (i=0;i<4;i++)
   {

    P_INF[pc][indice][i]=avt->Cells[1][i+1];
    P_INFO[pc][indice][i]=avt->Cells[2][i+1];
    P_POLT[pc][indice][i]=avt->Cells[3][i+1];
    P_COLTXTF[pc][indice][i]=avt->Cells[4][i+1];
    P_ITALT[pc][indice][i]=avt->Cells[5][i+1];
    P_BOLDT[pc][indice][i]=avt->Cells[6][i+1];
    P_SIZET[pc][indice][i]=avt->Cells[7][i+1];
    P_COLTXTB[pc][indice][i]=avt->Cells[8][i+1];
   }
  strcpy(tmp,"Caractéristiques pour le "); strcat(tmp,cbTache->Text.c_str());
  strcat(tmp," mémorisées");


AppendStr(c,IntToStr(jk));
   avv->SaveToFile(c);
   jk++;

AppendStr(c,IntToStr(jk));
   avt->SaveToFile(c);
  Application->MessageBoxA(tmp,m_ecoplan,MB_OK);

}
//---------------------------------------------------------------------------
void __fastcall TFDtaches::btAnnulClick(TObject *Sender)
{
Close();
}

void __fastcall TFDtaches::Display(int indx)
{
 int i;
 int cool;
 char tmp[50];
 int color;
 Label2->Caption =  cbTache->Items->Strings[indx];

 avv->Cells[1][1]= P_TRAIT[pc][indx];
 avv->Cells[1][2]= P_EPTRAIT[pc][indx];
 avv->Cells[1][3]= P_MARGE[pc][indx];
 avv->Cells[1][4]= P_STYLEM[pc][indx];
 avv->Cells[1][5]= P_COLMARGE[pc][indx];
 avv->Cells[1][6]= P_LIENS[pc][indx];
 avv->Cells[1][7]= P_STYLELIEN[pc][indx];
 avv->Cells[1][8]= P_COL_LIEN[pc][indx];
 avv->Cells[1][9]= P_EPAIS_LIEN[pc][indx];


 for (i=0;i<4;i++)
   {
    avt->Cells[1][i+1] = P_INF[pc][indx][i];
    avt->Cells[2][i+1]= P_INFO[pc][indx][i];
    avt->Cells[3][i+1]= P_POLT[pc][indx][i];
    avt->Cells[4][i+1]=  P_COLTXTF[pc][indx][i];
    strcpy(tmp,P_COLTXTF[pc][indx][i].c_str());
    color = atoi(tmp);
    avt->Colors[4][i+1] = TColor(color);  

    avt->Cells[5][i+1]= P_ITALT[pc][indx][i];
    avt->Cells[6][i+1]= P_BOLDT[pc][indx][i];
    avt->Cells[7][i+1]= P_SIZET[pc][indx][i];
    avt->Cells[8][i+1]=  P_COLTXTB[pc][indx][i];
   }

  strcpy(tmp,avv->Cells[1][1].c_str());
  cool=atoi(tmp);
  FillColor(cool,imTrait);

  strcpy(tmp,avv->Cells[1][5].c_str());
  cool=atoi(tmp);
  FillColor(cool,imMarge);

  strcpy(tmp,avv->Cells[1][8].c_str());
  cool=atoi(tmp);
  FillColor(cool,imBColor);


}

//---------------------------------------------------------------------------
void __fastcall TFDtaches::FormCreate(TObject *Sender)
{
 int i;
 cbTache->Clear();

 if (P_TYPE[pc]=='P')
   {
    cbTache->Items->Add("Prévisionnel Critique");
    cbTache->Items->Add("Prévisionnel non Critique");
   }
 else
   {
    cbTache->Items->Add("Prévisionnel Critique");
    cbTache->Items->Add("Prévisionnel non Critique");
    cbTache->Items->Add("Réel Critique");
    cbTache->Items->Add("Réel non Critique");
   }



 cbTache->ItemIndex = 0;;
 Display(0);
 indice=0;

 cbTache->Text = cbTache->Items->Strings[0];
 //Display(0);   // display Prev critique par defaut
 //indice=0;

}
//---------------------------------------------------------------------------

void __fastcall TFDtaches::FillColor(int color,TImage *im)
{
 TRect Diagram;  TColor coul;

 coul=TColor(color);
 Diagram = Rect(0,0, im->Width,im->Height);
 im->Canvas->Brush->Color = coul; // BgColor;
 im->Canvas->FillRect(Diagram);
 im->Canvas->Pen->Color = clBlack;
 im->Canvas->MoveTo(0,0);
 im->Canvas->LineTo(0,im->Height-1);
 im->Canvas->LineTo(im->Width-1,im->Height-1);
 im->Canvas->LineTo(im->Width-1,0);
 im->Canvas->LineTo(0,0);
}


int __fastcall TFDtaches::ExtractValue(char *result, char *buff, char *tag, int posdeb)
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


int __fastcall TFDtaches::Explode(char sep,char *str)
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

void __fastcall TFDtaches::CStrip(char *s)
{
int count; char tmp[2500];

ExtractValue(tmp,s,"champs",0);
avt->Cells[1][OldRow] = AnsiString(tmp);
}

void __fastcall TFDtaches::avtClickCell(TObject *Sender, int ARow,
      int ACol)
{
 int cool;  char tmp[250];
 TColor Color;
 TChCol *ChCol;

 TFont *Fonte;
 AnsiString Name;
 int Size;

  char bold[50],italic[50];
  TFontStyles  Style;


 OldCol=ACol; OldRow=ARow;

 if (OldCol==0)
      {
       strcpy(tmp,avt->Cells[4][OldRow].c_str());
       cool=atoi(tmp);
       FillColor(cool,imFColor);

      }
 else  // Old Col > 0
   {
     if (OldCol==1)
      {
       if (avt->Cells[1][ARow]=="Oui")  avt->Cells[1][ARow]="Non";
       else avt->Cells[1][ARow]="Oui";
       }
     if (OldCol==2)
       {
        strcpy(m_exchange,"<champs>");   // m_exchange est vide, pour un nouveau
        strcat(m_exchange,avt->Cells[2][OldRow].c_str());
        strcat(m_exchange,"</champs>");

        ChCol= new TChCol(Application);
        ChCol->ShowModal();
        delete ChCol;
        if (strlen(m_exchange)==0) return;
        ExtractValue(tmp,m_exchange,"champs",0);
        avt->Cells[2][OldRow] = AnsiString(tmp);
       }

     if (OldCol>2 && OldCol<8)
       {
         strcpy(m_exchange,"");

         FontDialog1->Font->Name = avt->Cells[3][OldRow];
         FontDialog1->Font->Color = StrToInt(avt->Cells[4][OldRow]);
         FontDialog1->Font->Size = StrToInt(avt->Cells[7][OldRow]);

       if (avt->Cells[6][OldRow]=="Gras" && avt->Cells[5][OldRow]=="Italique")
       FontDialog1->Font->Style = TFontStyles()<< fsBold << fsItalic;
       else if (avt->Cells[6][OldRow]=="Gras" && avt->Cells[5][OldRow]!="Italique")
       FontDialog1->Font->Style = TFontStyles()<< fsBold;
       else if (avt->Cells[6][OldRow]!="Gras" && avt->Cells[5][OldRow]=="Italique")
       FontDialog1->Font->Style = TFontStyles() << fsItalic;

         if (FontDialog1->Execute())
           { Fonte = FontDialog1->Font;
             Color = Fonte->Color;
             Size = Fonte->Size;
             Name = Fonte->Name;
             Style = Fonte->Style;
             // recherche si bold et italique

             if (Style.Contains(fsBold))  strcpy(bold,"Gras"); else strcpy(bold,"Standard");
      if (Style.Contains(fsItalic))  strcpy(italic,"Italique"); else strcpy(italic,"Standard");

             avt->Cells[3][OldRow]=Name;
             avt->Cells[4][OldRow]=AnsiString(Color);
             avt->Colors[4][OldRow]=Color;

             avt->Cells[5][OldRow]=AnsiString(italic);
             avt->Cells[6][OldRow]=AnsiString(bold);
             avt->Cells[7][OldRow]=AnsiString(Size);
             strcpy(tmp,avt->Cells[4][OldRow].c_str());
             cool=atoi(tmp);
             FillColor(cool,imFColor);
           }
        }
     if (OldCol==8)
       {

       }

   }  // Else OldCol

}
//---------------------------------------------------------------------------

void __fastcall TFDtaches::cbTacheClick(TObject *Sender)
{
 int i;
 i=cbTache->ItemIndex;
 if (i==-1) return;
 Display(i);
 indice=i;

}
//---------------------------------------------------------------------------


void __fastcall TFDtaches::avvClickCell(TObject *Sender, int ARow,
      int ACol)
{
 char tmp[250]; int cool;
 TColor Color;
 TFDStyle *Style;
 TFDEpaisseur *Ep;


 if (ARow==0) return;

 if (ACol==0)
  {
   if (ARow==1){strcpy(tmp,avv->Cells[1][ARow].c_str()); cool=atoi(tmp); FillColor(cool,imTrait);}
   if (ARow==5){strcpy(tmp,avv->Cells[1][ARow].c_str()); cool=atoi(tmp); FillColor(cool,imMarge);}
   if (ARow==8){strcpy(tmp,avv->Cells[1][ARow].c_str()); cool=atoi(tmp); FillColor(cool,imBColor);}
  }


 if (ACol==1)
  {
   if (ARow==1)
    { if (ColorDialog1->Execute())
        {
         Color = ColorDialog1->Color;
         avv->Cells[ACol][ARow]=AnsiString(Color);
         strcpy(tmp,avv->Cells[ACol][ARow].c_str());
         cool=atoi(tmp);
         FillColor(cool,imTrait);
        }
    }

   if (ARow==2)
     {
       strcpy(m_exchange,"<ep>");  strcat(m_exchange,avv->Cells[ACol][ARow].c_str());
       strcat(m_exchange,"</ep>");

       Ep=new TFDEpaisseur(Application);
       Ep->ShowModal();
       delete Ep;
       ExtractValue(tmp,m_exchange,"ep",0);
       avv->Cells[ACol][ARow]=AnsiString(tmp);
     }


   if (ARow==3)
    {
     if (avv->Cells[1][ARow]=="Oui")  avv->Cells[1][ARow]="Non";
       else avv->Cells[1][ARow]="Oui";
    }

   if (ARow==4)
     {
      Style= new TFDStyle(Application);
      Style->ShowModal();
      delete Style;
      if (strlen(m_exchange)==0) return;
      ExtractValue(tmp,m_exchange,"style",0);
      avv->Cells[1][ARow]=AnsiString(tmp);
     }
   if (ARow==5)
    { if (ColorDialog1->Execute())
        {
         Color = ColorDialog1->Color;
         avv->Cells[ACol][ARow]=AnsiString(Color);
         strcpy(tmp,avv->Cells[ACol][ARow].c_str());
         cool=atoi(tmp);
         FillColor(cool,imMarge);
        }
    }

   if (ARow==6)
    {
     if (avv->Cells[1][ARow]=="Oui")  avv->Cells[1][ARow]="Non";
       else avv->Cells[1][ARow]="Oui";
    }

   if (ARow==7)
     {
      Style= new TFDStyle(Application);
      Style->ShowModal();
      delete Style;
      if (strlen(m_exchange)==0) return;
      ExtractValue(tmp,m_exchange,"style",0);
      avv->Cells[1][ARow]=AnsiString(tmp);
     }
   if (ARow==8)
    { if (ColorDialog1->Execute())
        {
         Color = ColorDialog1->Color;
         avv->Cells[ACol][ARow]=AnsiString(Color);
         strcpy(tmp,avv->Cells[ACol][ARow].c_str());
         cool=atoi(tmp);
         FillColor(cool,imBColor);
        }
    }

   if (ARow==9)
     {
       strcpy(m_exchange,"<ep>");  strcat(m_exchange,avv->Cells[ACol][ARow].c_str());
       strcat(m_exchange,"</ep>");

       Ep=new TFDEpaisseur(Application);
       Ep->ShowModal();
       delete Ep;
       ExtractValue(tmp,m_exchange,"ep",0);
       avv->Cells[ACol][ARow]=AnsiString(tmp);
     }
  }

}
//---------------------------------------------------------------------------



void __fastcall TFDtaches::btAideClick(TObject *Sender)
{
Thelp *Aide;
 strcpy(m_help,"Plannings.html#Pl45");
 Aide = new Thelp(Application);
 Aide->ShowModal();
 delete Aide;
}
//---------------------------------------------------------------------------


