object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Vergleichsprogramm Berliner Adressen'
  ClientHeight = 1816
  ClientWidth = 3011
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -24
  Font.Name = 'Segoe UI'
  Font.Style = []
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 192
  TextHeight = 32
  object Panel1: TPanel
    Left = 2445
    Top = 54
    Width = 566
    Height = 1762
    Margins.Left = 6
    Margins.Top = 6
    Margins.Right = 6
    Margins.Bottom = 6
    Align = alRight
    TabOrder = 0
    object Button1: TButton
      Left = 60
      Top = 24
      Width = 390
      Height = 70
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      Caption = 'read sequential'
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 60
      Top = 102
      Width = 390
      Height = 70
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      Caption = 'sort place'
      TabOrder = 1
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 60
      Top = 180
      Width = 390
      Height = 70
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      Caption = 'calculate'
      TabOrder = 2
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 60
      Top = 258
      Width = 390
      Height = 70
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      Caption = 'write to file'
      TabOrder = 3
      OnClick = Button4Click
    end
    object Button20: TButton
      Left = 58
      Top = 920
      Width = 390
      Height = 70
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      Caption = 'all'
      TabOrder = 4
      OnClick = Button20Click
    end
    object Button5: TButton
      Left = 60
      Top = 336
      Width = 390
      Height = 70
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      Caption = 'sort distance'
      TabOrder = 5
      OnClick = Button5Click
    end
    object Button6: TButton
      Left = 60
      Top = 414
      Width = 390
      Height = 70
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      Caption = 'sort standard'
      TabOrder = 6
      OnClick = Button6Click
    end
    object Button8: TButton
      Left = 60
      Top = 570
      Width = 390
      Height = 70
      Margins.Left = 8
      Margins.Top = 8
      Margins.Right = 8
      Margins.Bottom = 8
      Caption = 'write to directories'
      TabOrder = 7
      OnClick = Button8Click
    end
    object Button7: TButton
      Left = 60
      Top = 492
      Width = 390
      Height = 62
      Margins.Left = 8
      Margins.Top = 8
      Margins.Right = 8
      Margins.Bottom = 8
      Caption = 'delete directories'
      TabOrder = 8
      OnClick = Button7Click
    end
    object Button9: TButton
      Left = 60
      Top = 650
      Width = 390
      Height = 62
      Margins.Left = 8
      Margins.Top = 8
      Margins.Right = 8
      Margins.Bottom = 8
      Caption = 'free data'
      TabOrder = 9
      OnClick = Button9Click
    end
    object Button10: TButton
      Left = 64
      Top = 726
      Width = 390
      Height = 62
      Margins.Left = 6
      Margins.Top = 6
      Margins.Right = 6
      Margins.Bottom = 6
      Caption = 'read from directories'
      TabOrder = 10
      OnClick = Button10Click
    end
    object Button11: TButton
      Left = 64
      Top = 808
      Width = 390
      Height = 62
      Margins.Left = 6
      Margins.Top = 6
      Margins.Right = 6
      Margins.Bottom = 6
      Caption = 'partition'
      TabOrder = 11
      OnClick = Button11Click
    end
  end
  object Memo1: TMemo
    Left = 0
    Top = 54
    Width = 2445
    Height = 1762
    Margins.Left = 6
    Margins.Top = 6
    Margins.Right = 6
    Margins.Bottom = 6
    Align = alClient
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -30
    Font.Name = 'Courier New'
    Font.Style = []
    Lines.Strings = (
      'Memo1')
    ParentFont = False
    TabOrder = 1
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 3011
    Height = 54
    Margins.Left = 6
    Margins.Top = 6
    Margins.Right = 6
    Margins.Bottom = 6
    Align = alTop
    TabOrder = 2
    object Edit1: TEdit
      Left = 0
      Top = 0
      Width = 4240
      Height = 40
      Margins.Left = 16
      Margins.Top = 16
      Margins.Right = 16
      Margins.Bottom = 16
      TabOrder = 0
      Text = 'Edit1'
    end
  end
end
