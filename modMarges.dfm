object FDmarges: TFDmarges
  Left = 599
  Top = 200
  Width = 361
  Height = 277
  Caption = 'Description des Marges'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 24
    Top = 24
    Width = 185
    Height = 161
    Caption = 'Marges (en cm)'
    TabOrder = 0
    object Label1: TLabel
      Left = 8
      Top = 24
      Width = 23
      Height = 13
      Caption = 'Haut'
    end
    object Label2: TLabel
      Left = 8
      Top = 56
      Width = 18
      Height = 13
      Caption = 'Bas'
    end
    object Label3: TLabel
      Left = 8
      Top = 88
      Width = 38
      Height = 13
      Caption = 'Gauche'
    end
    object Label4: TLabel
      Left = 8
      Top = 120
      Width = 28
      Height = 13
      Caption = 'Droite'
    end
    object Edit1: TEdit
      Left = 72
      Top = 24
      Width = 89
      Height = 21
      TabOrder = 0
      Text = '0.5'
    end
    object Edit2: TEdit
      Left = 72
      Top = 56
      Width = 89
      Height = 21
      TabOrder = 1
      Text = '0.5'
    end
    object Edit3: TEdit
      Left = 72
      Top = 88
      Width = 89
      Height = 21
      TabOrder = 2
      Text = '0.5'
    end
    object Edit4: TEdit
      Left = 72
      Top = 120
      Width = 89
      Height = 21
      TabOrder = 3
      Text = '0.5'
    end
  end
  object Button1: TButton
    Left = 248
    Top = 32
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 248
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Annuler'
    TabOrder = 2
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 248
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Aide'
    TabOrder = 3
    OnClick = Button3Click
  end
  object ColorDialog1: TColorDialog
    Ctl3D = True
    Left = 264
    Top = 152
  end
end
