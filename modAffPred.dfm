object AffPred: TAffPred
  Left = 592
  Top = 164
  Width = 376
  Height = 251
  Caption = 'Affecter Pr�decesseur aux taches Coch�es'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 224
    Top = 16
    Width = 46
    Height = 13
    Caption = 'D�calage'
  end
  object Label2: TLabel
    Left = 16
    Top = 24
    Width = 86
    Height = 13
    Caption = 'Num�ro de Tache'
  end
  object Button1: TButton
    Left = 224
    Top = 104
    Width = 75
    Height = 25
    Caption = 'Executer'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 224
    Top = 136
    Width = 75
    Height = 25
    Caption = 'Annuler'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 224
    Top = 168
    Width = 75
    Height = 25
    Caption = 'Aide'
    TabOrder = 2
    OnClick = Button3Click
  end
  object Edit1: TEdit
    Left = 216
    Top = 40
    Width = 73
    Height = 21
    TabOrder = 3
    Text = '0'
  end
  object RadioGroup1: TRadioGroup
    Left = 24
    Top = 72
    Width = 129
    Height = 129
    Caption = 'Liens'
    TabOrder = 4
  end
  object rb1: TRadioButton
    Left = 32
    Top = 88
    Width = 113
    Height = 17
    Caption = 'Fin - D�but'
    Checked = True
    TabOrder = 5
    TabStop = True
  end
  object rb2: TRadioButton
    Left = 32
    Top = 128
    Width = 113
    Height = 17
    Caption = 'Fin - Fin'
    TabOrder = 6
  end
  object rb3: TRadioButton
    Left = 32
    Top = 168
    Width = 113
    Height = 17
    Caption = 'D�but - D�but'
    TabOrder = 7
  end
  object Edit2: TEdit
    Left = 120
    Top = 24
    Width = 41
    Height = 21
    TabOrder = 8
  end
  object CheckBox1: TCheckBox
    Left = 184
    Top = 72
    Width = 145
    Height = 17
    Caption = 'D�cocher apr�s traitement'
    Checked = True
    State = cbChecked
    TabOrder = 9
  end
end