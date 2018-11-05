object ChCol: TChCol
  Left = 492
  Top = 306
  Width = 528
  Height = 387
  Caption = 'Champ Colonnes'
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
  object Label1: TLabel
    Left = 16
    Top = 32
    Width = 81
    Height = 13
    Caption = 'Tous les Champs'
  end
  object Label2: TLabel
    Left = 224
    Top = 32
    Width = 102
    Height = 13
    Caption = 'Champs Sélectionnés'
  end
  object Label3: TLabel
    Left = 88
    Top = 8
    Width = 200
    Height = 13
    Caption = 'Sélection des Champs en Colonnes'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbTout: TListBox
    Left = 16
    Top = 48
    Width = 193
    Height = 217
    ItemHeight = 13
    TabOrder = 0
  end
  object lbSel: TListBox
    Left = 264
    Top = 48
    Width = 193
    Height = 217
    ItemHeight = 13
    TabOrder = 1
  end
  object btRight: TBitBtn
    Left = 224
    Top = 128
    Width = 25
    Height = 25
    TabOrder = 2
    OnClick = btRightClick
    Glyph.Data = {
      72010000424D7201000000000000760000002800000015000000150000000100
      040000000000FC00000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
      8888888880008888886888888888888880008888886688888888888880008888
      8866688888888888800088888866668888888888800088888866666888888888
      8000888888666666888888888000888888666666688888888000888888666666
      6688888880008888886666666668888880008888886666666666888880008888
      8866666666688888800088888866666666888888800088888866666668888888
      8000888888666666888888888000888888666668888888888000888888666688
      8888888880008888886668888888888880008888886688888888888880008888
      88688888888888888000888888888888888888888000}
  end
  object btLeft: TBitBtn
    Left = 224
    Top = 160
    Width = 25
    Height = 25
    TabOrder = 3
    OnClick = btLeftClick
    Glyph.Data = {
      72010000424D7201000000000000760000002800000015000000150000000100
      040000000000FC00000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
      8888888880008888888888888868888880008888888888888668888880008888
      8888888866688888800088888888888666688888800088888888886666688888
      8000888888888666666888888000888888886666666888888000888888866666
      6668888880008888886666666668888880008888866666666668888880008888
      8866666666688888800088888886666666688888800088888888666666688888
      8000888888888666666888888000888888888866666888888000888888888886
      6668888880008888888888886668888880008888888888888668888880008888
      88888888886888888000888888888888888888888000}
  end
  object btUp: TBitBtn
    Left = 472
    Top = 128
    Width = 25
    Height = 25
    TabOrder = 4
    OnClick = btUpClick
    Glyph.Data = {
      72010000424D7201000000000000760000002800000015000000150000000100
      040000000000FC00000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
      8888888880008888888888888888888880008888888888888888888880008888
      8888888888888888800088888888888888888888800088888888888888888888
      8000866666666666666666668000886666666666666666688000888666666666
      6666668880008888666666666666688880008888866666666666888880008888
      8866666666688888800088888886666666888888800088888888666668888888
      8000888888888666888888888000888888888868888888888000888888888888
      8888888880008888888888888888888880008888888888888888888880008888
      88888888888888888000888888888888888888888000}
  end
  object btDown: TBitBtn
    Left = 472
    Top = 160
    Width = 25
    Height = 25
    TabOrder = 5
    OnClick = btDownClick
    Glyph.Data = {
      72010000424D7201000000000000760000002800000015000000150000000100
      040000000000FC00000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
      8888888880008888888888888888888880008888888888888888888880008888
      8888888888888888800088888888888888888888800088888888886888888888
      8000888888888666888888888000888888886666688888888000888888866666
      6688888880008888886666666668888880008888866666666666888880008888
      6666666666666888800088866666666666666688800088666666666666666668
      8000866666666666666666668000888888888888888888888000888888888888
      8888888880008888888888888888888880008888888888888888888880008888
      88888888888888888000888888888888888888888000}
  end
  object btOK: TButton
    Left = 144
    Top = 296
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 6
    OnClick = btOKClick
  end
  object btAnnul: TButton
    Left = 232
    Top = 296
    Width = 75
    Height = 25
    Caption = 'Annulation'
    TabOrder = 7
    OnClick = btAnnulClick
  end
  object btAide: TButton
    Left = 320
    Top = 296
    Width = 75
    Height = 25
    Caption = 'Aide'
    TabOrder = 8
    OnClick = btAideClick
  end
end
