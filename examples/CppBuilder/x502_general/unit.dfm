object Form1: TForm1
  Left = 374
  Top = 120
  Width = 593
  Height = 667
  Caption = 'L502/E502 test'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object btnRefreshDeviceList: TButton
    Left = 16
    Top = 8
    Width = 185
    Height = 25
    Caption = #1054#1073#1085#1086#1074#1080#1090#1100' '#1089#1087#1080#1089#1082#1086' '#1091#1089#1090#1088#1086#1081#1089#1090#1074
    TabOrder = 0
    OnClick = btnRefreshDeviceListClick
  end
  object cbbDevList: TComboBox
    Left = 216
    Top = 8
    Width = 153
    Height = 21
    ItemHeight = 13
    TabOrder = 1
    OnChange = cbbDevListChange
  end
  object btnStart: TButton
    Left = 16
    Top = 96
    Width = 185
    Height = 25
    Caption = #1047#1072#1087#1091#1089#1082' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093
    TabOrder = 2
    OnClick = btnStartClick
  end
  object GroupBox1: TGroupBox
    Left = 16
    Top = 304
    Width = 513
    Height = 153
    Caption = #1050#1072#1085#1072#1083#1099
    TabOrder = 3
    object Label2: TLabel
      Left = 8
      Top = 40
      Width = 31
      Height = 13
      Caption = #1050#1072#1085#1072#1083
    end
    object Label1: TLabel
      Left = 64
      Top = 40
      Width = 51
      Height = 13
      Caption = #1044#1080#1072#1087#1072#1079#1086#1085
    end
    object Label3: TLabel
      Left = 152
      Top = 40
      Width = 94
      Height = 13
      Caption = #1056#1077#1078#1080#1084' '#1080#1079#1084#1077#1088#1077#1085#1080#1103
    end
    object Label4: TLabel
      Left = 336
      Top = 40
      Width = 52
      Height = 13
      Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090
    end
    object Label9: TLabel
      Left = 16
      Top = 16
      Width = 164
      Height = 13
      Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1083#1086#1075#1080#1095#1077#1089#1082#1080#1093' '#1082#1072#1085#1072#1083#1086#1074
    end
    object cbbLCh1_Channel: TComboBox
      Left = 8
      Top = 56
      Width = 41
      Height = 21
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 0
      Text = '1'
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '31'
        '32')
    end
    object cbbLCh1_Range: TComboBox
      Left = 64
      Top = 56
      Width = 57
      Height = 21
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 1
      Text = '10 '#1042
      Items.Strings = (
        '10 '#1042
        '5 '#1042
        '2 '#1042
        '1 '#1042
        '0.5 '#1042
        '0.2 '#1042)
    end
    object cbbLCh1_Mode: TComboBox
      Left = 136
      Top = 56
      Width = 169
      Height = 21
      ItemHeight = 13
      ItemIndex = 1
      TabOrder = 2
      Text = #1044#1080#1092#1092#1077#1088#1077#1085#1094#1080#1072#1083#1100#1085#1099#1081
      Items.Strings = (
        #1057' '#1086#1073#1097#1077#1081' '#1079#1077#1084#1083#1077#1081
        #1044#1080#1092#1092#1077#1088#1077#1085#1094#1080#1072#1083#1100#1085#1099#1081
        #1048#1079#1084#1077#1088#1077#1085#1080#1077' '#1085#1091#1083#1103)
    end
    object edtLCh1_Result: TEdit
      Left = 328
      Top = 56
      Width = 121
      Height = 21
      TabOrder = 3
    end
    object cbbLCh2_Channel: TComboBox
      Left = 8
      Top = 80
      Width = 41
      Height = 21
      ItemHeight = 13
      ItemIndex = 1
      TabOrder = 4
      Text = '2'
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '31'
        '32')
    end
    object cbbLCh2_Range: TComboBox
      Left = 64
      Top = 80
      Width = 57
      Height = 21
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 5
      Text = '10 '#1042
      Items.Strings = (
        '10 '#1042
        '5 '#1042
        '2 '#1042
        '1 '#1042
        '0.5 '#1042
        '0.2 '#1042)
    end
    object cbbLCh2_Mode: TComboBox
      Left = 136
      Top = 80
      Width = 169
      Height = 21
      ItemHeight = 13
      ItemIndex = 1
      TabOrder = 6
      Text = #1044#1080#1092#1092#1077#1088#1077#1085#1094#1080#1072#1083#1100#1085#1099#1081
      Items.Strings = (
        #1057' '#1086#1073#1097#1077#1081' '#1079#1077#1084#1083#1077#1081
        #1044#1080#1092#1092#1077#1088#1077#1085#1094#1080#1072#1083#1100#1085#1099#1081
        #1048#1079#1084#1077#1088#1077#1085#1080#1077' '#1085#1091#1083#1103)
    end
    object edtLCh2_Result: TEdit
      Left = 328
      Top = 80
      Width = 121
      Height = 21
      TabOrder = 7
    end
    object cbbLCh3_Channel: TComboBox
      Left = 8
      Top = 104
      Width = 41
      Height = 21
      ItemHeight = 13
      ItemIndex = 2
      TabOrder = 8
      Text = '3'
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '31'
        '32')
    end
    object cbbLCh3_Range: TComboBox
      Left = 64
      Top = 104
      Width = 57
      Height = 21
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 9
      Text = '10 '#1042
      Items.Strings = (
        '10 '#1042
        '5 '#1042
        '2 '#1042
        '1 '#1042
        '0.5 '#1042
        '0.2 '#1042)
    end
    object cbbLCh3_Mode: TComboBox
      Left = 136
      Top = 104
      Width = 169
      Height = 21
      ItemHeight = 13
      ItemIndex = 1
      TabOrder = 10
      Text = #1044#1080#1092#1092#1077#1088#1077#1085#1094#1080#1072#1083#1100#1085#1099#1081
      Items.Strings = (
        #1057' '#1086#1073#1097#1077#1081' '#1079#1077#1084#1083#1077#1081
        #1044#1080#1092#1092#1077#1088#1077#1085#1094#1080#1072#1083#1100#1085#1099#1081
        #1048#1079#1084#1077#1088#1077#1085#1080#1077' '#1085#1091#1083#1103)
    end
    object edtLCh3_Result: TEdit
      Left = 328
      Top = 104
      Width = 121
      Height = 21
      TabOrder = 11
    end
    object chkSyncDin: TCheckBox
      Left = 120
      Top = 128
      Width = 185
      Height = 17
      Caption = #1056#1072#1079#1088#1072#1096#1077#1085#1080#1077' '#1089#1080#1085#1093#1088#1086#1085#1085#1086#1075#1086' '#1074#1074#1086#1076#1072
      TabOrder = 12
      OnClick = chkSyncDinClick
    end
    object edtDin_Result: TEdit
      Left = 328
      Top = 128
      Width = 121
      Height = 21
      TabOrder = 13
    end
    object cbbLChCnt: TComboBox
      Left = 192
      Top = 16
      Width = 73
      Height = 21
      ItemHeight = 13
      ItemIndex = 2
      TabOrder = 14
      Text = '3'
      OnChange = cbbLChCntChange
      Items.Strings = (
        '1'
        '2'
        '3')
    end
  end
  object btnStop: TButton
    Left = 16
    Top = 128
    Width = 185
    Height = 25
    Caption = #1054#1089#1090#1072#1085#1086#1074' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093
    Enabled = False
    TabOrder = 4
    OnClick = btnStopClick
  end
  object btnOpen: TButton
    Left = 16
    Top = 64
    Width = 185
    Height = 25
    Caption = #1054#1090#1082#1088#1099#1090#1100' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1086
    TabOrder = 5
    OnClick = btnOpenClick
  end
  object btnAsyncDigOut: TButton
    Left = 16
    Top = 528
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1099#1074#1086#1076' '#1094#1080#1092#1088#1086#1074#1099#1093' '#1083#1080#1085#1080#1081
    TabOrder = 6
    OnClick = btnAsyncDigOutClick
  end
  object edtAsyncDigOut: TEdit
    Left = 248
    Top = 528
    Width = 121
    Height = 21
    TabOrder = 7
    Text = '0x0000'
  end
  object edtAsyncDigIn: TEdit
    Left = 248
    Top = 496
    Width = 121
    Height = 21
    Enabled = False
    TabOrder = 8
    Text = '0x0000'
  end
  object btnAsyncDigIn: TButton
    Left = 16
    Top = 496
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1074#1086#1076' '#1094#1080#1092#1088#1086#1099#1093' '#1083#1080#1085#1080#1081
    TabOrder = 9
    OnClick = btnAsyncDigInClick
  end
  object GroupBox2: TGroupBox
    Left = 264
    Top = 40
    Width = 257
    Height = 129
    Caption = #1048#1085#1092#1086#1088#1084#1072#1094#1080#1103' '#1086'  '#1084#1086#1076#1091#1083#1077
    TabOrder = 10
    object Label6: TLabel
      Left = 104
      Top = 77
      Width = 124
      Height = 13
      Caption = #1042#1077#1088#1089#1080#1103' '#1087#1088#1086#1096#1080#1074#1082#1080' '#1055#1051#1048#1057
    end
    object labelPldaVer: TLabel
      Left = 104
      Top = 101
      Width = 121
      Height = 13
      Caption = #1042#1077#1088#1089#1080#1103' '#1087#1088#1086#1096#1080#1074#1082#1080' PLDA'
    end
    object chkDacPresent: TCheckBox
      Left = 16
      Top = 24
      Width = 97
      Height = 17
      Caption = #1053#1072#1083#1080#1095#1080#1077' '#1062#1040#1055
      Enabled = False
      TabOrder = 0
    end
    object edtFpgaVer: TEdit
      Left = 16
      Top = 77
      Width = 81
      Height = 21
      Enabled = False
      TabOrder = 1
    end
    object edtPldaVer: TEdit
      Left = 16
      Top = 101
      Width = 81
      Height = 21
      Enabled = False
      TabOrder = 2
    end
  end
  object chkGalPresent: TCheckBox
    Left = 280
    Top = 80
    Width = 169
    Height = 17
    Caption = #1053#1072#1083#1080#1095#1080#1077' '#1075#1072#1083#1100#1074#1072#1085#1086#1088#1072#1079#1074#1103#1079#1082#1080
    Enabled = False
    TabOrder = 11
  end
  object chkDspPresent: TCheckBox
    Left = 280
    Top = 96
    Width = 209
    Height = 17
    Caption = #1053#1072#1083#1080#1095#1080#1077' '#1089#1080#1075#1085#1072#1083#1100#1085#1086#1075#1086' '#1087#1088#1086#1094#1077#1089#1089#1086#1088#1072
    Enabled = False
    TabOrder = 12
  end
  object edtAsyncDac1: TEdit
    Left = 248
    Top = 568
    Width = 81
    Height = 21
    TabOrder = 13
    Text = '5'
  end
  object btnAsyncDac1: TButton
    Left = 16
    Top = 568
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1099#1074#1086#1076' '#1085#1072' '#1062#1040#1055'1 ('#1042#1086#1083#1100#1090#1099')'
    TabOrder = 14
    OnClick = btnAsyncDac1Click
  end
  object btnAsyncDac2: TButton
    Left = 16
    Top = 600
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1099#1074#1086#1076' '#1085#1072' '#1062#1040#1055'2 ('#1042#1086#1083#1100#1090#1099')'
    TabOrder = 15
    OnClick = btnAsyncDac2Click
  end
  object edtAsyncDac2: TEdit
    Left = 248
    Top = 600
    Width = 81
    Height = 21
    TabOrder = 16
    Text = '5'
  end
  object btnAsyncAdcFrame: TButton
    Left = 16
    Top = 464
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1074#1086#1076' '#1082#1072#1076#1088#1072' '#1040#1062#1055
    TabOrder = 17
    OnClick = btnAsyncAdcFrameClick
  end
  object GroupBox3: TGroupBox
    Left = 16
    Top = 168
    Width = 513
    Height = 65
    Caption = #1063#1072#1089#1090#1086#1090#1099' '#1089#1080#1085#1093#1088#1086#1085#1085#1086#1075#1086' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093
    TabOrder = 18
    object Label5: TLabel
      Left = 16
      Top = 20
      Width = 136
      Height = 13
      Caption = #1063#1072#1089#1090#1086#1090#1072' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093' ('#1043#1094')'
    end
    object Label7: TLabel
      Left = 168
      Top = 20
      Width = 111
      Height = 13
      Caption = #1063#1072#1089#1090#1086#1090#1072' '#1085#1072' '#1082#1072#1085#1072#1083' ('#1043#1094')'
    end
    object Label8: TLabel
      Left = 296
      Top = 20
      Width = 163
      Height = 13
      Caption = #1063#1072#1089#1090#1086#1090#1072' '#1089#1080#1085#1093#1088#1086#1085#1085#1086#1075#1086' '#1074#1074#1086#1076#1072' ('#1043#1094')'
    end
    object btnSetAdcFreq: TButton
      Left = 432
      Top = 32
      Width = 73
      Height = 25
      Caption = #1059#1089#1090#1072#1085#1086#1074#1080#1090#1100
      TabOrder = 0
      OnClick = adcFreqEditChange
    end
    object edtAdcFreq: TEdit
      Left = 16
      Top = 36
      Width = 105
      Height = 21
      TabOrder = 1
      Text = '2000000'
    end
    object edtAdcFreqLch: TEdit
      Left = 176
      Top = 36
      Width = 105
      Height = 21
      TabOrder = 2
      Text = '2000000'
    end
    object edtDinFreq: TEdit
      Left = 296
      Top = 36
      Width = 105
      Height = 21
      TabOrder = 3
      Text = '2000000'
    end
  end
  object GroupBox4: TGroupBox
    Left = 16
    Top = 239
    Width = 513
    Height = 65
    Caption = #1057#1080#1085#1093#1088#1086#1085#1080#1079#1072#1094#1080#1103
    TabOrder = 19
    object Label10: TLabel
      Left = 16
      Top = 20
      Width = 172
      Height = 13
      Caption = #1048#1089#1090#1086#1095#1085#1080#1082' '#1095#1072#1089#1090#1086#1090#1099' '#1089#1080#1085#1093#1088#1086#1085#1080#1079#1072#1094#1080#1080
    end
    object Label11: TLabel
      Left = 264
      Top = 20
      Width = 238
      Height = 13
      Caption = #1048#1089#1090#1086#1095#1085#1080#1082' '#1079#1072#1087#1091#1089#1082#1072'  '#1089#1080#1085#1093#1088#1086#1085#1085#1086#1075#1086' '#1074#1074#1086#1076#1072'/'#1074#1099#1074#1086#1076#1072
    end
    object cbbSyncMode: TComboBox
      Left = 16
      Top = 38
      Width = 169
      Height = 21
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 0
      Text = #1042#1085#1091#1090#1088#1077#1085#1085#1080#1081
      Items.Strings = (
        #1042#1085#1091#1090#1088#1077#1085#1085#1080#1081
        #1054#1090' '#1074#1085#1077#1096#1085#1077#1075#1086' '#1084#1072#1089#1090#1077#1088#1072
        #1060#1088#1086#1085#1090' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN1'
        #1060#1088#1086#1085#1090' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN2'
        #1057#1087#1072#1076' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN1'
        #1057#1087#1072#1076' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN2')
    end
    object cbbSyncStartMode: TComboBox
      Left = 264
      Top = 38
      Width = 169
      Height = 21
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 1
      Text = #1042#1085#1091#1090#1088#1077#1085#1085#1080#1081
      Items.Strings = (
        #1042#1085#1091#1090#1088#1077#1085#1085#1080#1081
        #1054#1090' '#1074#1085#1077#1096#1085#1077#1075#1086' '#1084#1072#1089#1090#1077#1088#1072
        #1060#1088#1086#1085#1090' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN1'
        #1060#1088#1086#1085#1090' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN2'
        #1057#1087#1072#1076' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN1'
        #1057#1087#1072#1076' '#1089#1080#1075#1085#1072#1083#1072' DI_SYN2')
    end
  end
end
