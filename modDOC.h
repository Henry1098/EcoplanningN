//---------------------------------------------------------------------------

#ifndef modDOCH
#define modDOCH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
//---------------------------------------------------------------------------
class TDOC : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TButton *Button2;
        TCppWebBrowser *CppWebBrowser1;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TDOC(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDOC *DOC;
//---------------------------------------------------------------------------
#endif
