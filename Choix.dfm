object Form1: TForm1
  Left = 192
  Top = 124
  Width = 215
  Height = 294
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object RadioButton1: TRadioButton
    Left = 32
    Top = 48
    Width = 113
    Height = 17
    Caption = 'Configuration 1'
    TabOrder = 0
  end
  object RadioButton2: TRadioButton
    Left = 32
    Top = 72
    Width = 113
    Height = 17
    Caption = 'Configuration 2'
    TabOrder = 1
  end
  object RadioButton3: TRadioButton
    Left = 32
    Top = 96
    Width = 113
    Height = 17
    Caption = 'Remise � zero'
    TabOrder = 2
  end
  object Button1: TButton
    Left = 56
    Top = 136
    Width = 75
    Height = 25
    Caption = 'Valider'
    TabOrder = 3
    OnClick = Button1Click
  end
end
