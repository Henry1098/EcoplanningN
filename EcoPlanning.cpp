//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------
USEFORM("Main.cpp", MainForm);
USEFORM("ChildWin.cpp", MDIChild);
USEFORM("about.cpp", AboutBox);
USEFORM("projectwin.cpp", fProjet);
USEFORM("plstd.cpp", plStandard);
USEFORM("num_pred.cpp", Num_Tache_Pred);
USEFORM("num_succ.cpp", Num_Tache_Succ);
USEFORM("inserttask.cpp", NewTask);
USEFORM("copytask.cpp", Copy);
USEFORM("liencc.cpp", LienCoche);
USEFORM("calendars.cpp", fCalendar);
USEFORM("AffCal.cpp", AffectCal);
USEFORM("actualisation.cpp", Actual);
USEFORM("C:\ECODOC\modhelp.cpp", help);
USEFORM("modplanches.cpp", Planches);
USERES("dessins.res");
USEFORM("modFilter.cpp", xFilter);
USEFORM("modCriteres.cpp", Criteres);
USEFORM("modChamps.cpp", Champs);
USEFORM("modTri.cpp", Tri);
USEFORM("modTriElem.cpp", TriCrit);
USEFORM("modGhost.cpp", Ghost);
USEFORM("modLib.cpp", Libel);
USEFORM("modHauteur.cpp", Hauteur);
USEFORM("modJust.cpp", Just);
USEFORM("modRetrait.cpp", Retrait);
USEFORM("modResProj.cpp", ResProj);
USEFORM("modPrepPlanning.cpp", PrepPlanning);
USEFORM("modPrepRapport.cpp", PrepRapport);
USEFORM("modEbauchPlan.cpp", EbauchePlan);
USEFORM("modAffinParam.cpp", AffineParam);
USEFORM("modFormat.cpp", Format);
USEFORM("modEntete.cpp", Entete);
USEFORM("modeCadre.cpp", Cadre);
USEFORM("modDcal.cpp", FDcal);
USEFORM("modTaches.cpp", FDtaches);
USEFORM("modPied.cpp", FDpied);
USEFORM("modMarges.cpp", FDmarges);
USEFORM("modColonnes.cpp", Colonnes);
USEFORM("modChcol.cpp", ChCol);
USEFORM("modUnitForm.cpp", FDUnitForm);
USEFORM("modStyle.cpp", FDStyle);
USEFORM("modTligne.cpp", FDTligne);
USEFORM("modTcol.cpp", FDTcol);
USEFORM("modEpaisseur.cpp", FDEpaisseur);
USEFORM("modResume.cpp", Resume);
USEFORM("modDebut.cpp", DateProjet);
USEFORM("modAffTC.cpp", AffData);
USEFORM("modRempl.cpp", RemplTC);
USEFORM("modCsim.cpp", SimulOpen);
USELIB("iKeyAPI.lib");
USEFORM("modAffPred.cpp", AffPred);
USEFORM("modAffSucc.cpp", AffSucc);
USEFORM("modDescEnvelop.cpp", DescEnvelop);
USEFORM("modMiseEnPage.cpp", MiseEnPage);
USEFORM("DescTrait.cpp", FDTrait);
USEFORM("DescEsp.cpp", FDEspace);
USEFORM("modDistVert.cpp", FDDistH);
USEFORM("modCorps.cpp", FDcorps);
USEFORM("modImport.cpp", Import);
USELIB("C:\Program Files\Borland\CBuilder5\Lib\cp32mt.lib");
USEFORM("modRenum.cpp", Renum);
USEUNIT("FinalIsam.cpp");
USEFORM("modLignesBrisees.cpp", LBrisee);
USEFORM("modPeriode.cpp", Period);
USEFORM("modPreview.cpp", Preview);
USEFORM("modXLS.cpp", TableCSV);
USEFORM("modregistr.cpp", Regist);
USEFORM("modDOC.cpp", DOC);
USEFORM("Choix.cpp", Form1);
USERES("EcoPlanning.res");
USEUNIT("sqlite3.c");
USE("sqlite3.h", File);
USE("sqlite3ext.h", File);
USELIB("..\A124639821-8429\okbase.net\tmspck_v4.3\gdiplus.lib");
USE("IKEYAPI.DLL", File);
USE("..\..\demo23\StrUtils.hpp", File);
//---------------------------------------------------------------------------
#include <excpt.h>

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE hMutex;

        try
        {
        hMutex = CreateMutex (NULL,FALSE,"Ecoplanning Version 5.0");
          if (GetLastError() == ERROR_ALREADY_EXISTS){
          ShowMessage("Il y a d�j� une instance d'Ecoplanning ouvert!");
           return 0;                              }
	Application->Initialize();
	Application->Title = "Ecoplanning Version 5.0";
                 Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->CreateForm(__classid(TGhost), &Ghost);
                 Application->CreateForm(__classid(TDOC), &DOC);
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();

        return 0;
        }
       catch (...)
        {
         Application->MessageBoxA("Ennui","Ecoplanning",MB_OK);
                 //Application->ShowException(&exception);
         return 1;
        }


        
}


//---------------------------------------------------------------------
