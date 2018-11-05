//---------------------------------------------------------------------------

#ifndef modBandesH
#define modBandesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TFDbandes : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFDbandes(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFDbandes *FDbandes;
//---------------------------------------------------------------------------
#endif