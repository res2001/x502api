object MainForm: TMainForm
  Left = 630
  Top = 162
  Width = 585
  Height = 774
  Caption = 'X502 Example'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object btnRefreshDeviceList: TButton
    Left = 8
    Top = 8
    Width = 217
    Height = 25
    Caption = #1054#1073#1085#1086#1074#1080#1090#1100' '#1089#1087#1080#1089#1086#1082' '#1091#1089#1090#1088#1086#1081#1089#1090#1074
    TabOrder = 0
    OnClick = btnRefreshDeviceListClick
  end
  object cbbDevList: TComboBox
    Left = 256
    Top = 8
    Width = 241
    Height = 21
    ItemHeight = 13
    TabOrder = 1
  end
  object btnOpen: TButton
    Left = 8
    Top = 48
    Width = 217
    Height = 25
    Caption = #1054#1090#1082#1088#1099#1090#1100' '#1091#1089#1090#1088#1086#1081#1089#1090#1074#1086
    TabOrder = 2
    OnClick = btnOpenClick
  end
  object grpDevInfo: TGroupBox
    Left = 256
    Top = 40
    Width = 241
    Height = 169
    Caption = #1048#1085#1092#1086#1088#1084#1072#1094#1080#1103' '#1086' '#1084#1086#1076#1091#1083#1077
    TabOrder = 3
    object lbl1: TLabel
      Left = 104
      Top = 96
      Width = 119
      Height = 13
      Caption = #1042#1077#1088#1089#1080#1103' '#1087#1088#1086#1096#1080#1074#1082#1080' '#1055#1051#1048#1057
    end
    object lbl2: TLabel
      Left = 104
      Top = 120
      Width = 116
      Height = 13
      Caption = #1042#1077#1088#1089#1080#1103' '#1087#1088#1086#1096#1080#1074#1082#1080' PLDA'
    end
    object lbl11: TLabel
      Left = 104
      Top = 144
      Width = 113
      Height = 13
      Caption = #1042#1077#1088#1089#1080#1103' '#1087#1088#1086#1096#1080#1074#1082#1080' ARM'
    end
    object chkDacPresent: TCheckBox
      Left = 8
      Top = 24
      Width = 97
      Height = 17
      Caption = #1053#1072#1083#1080#1095#1080#1077' '#1062#1040#1055
      Enabled = False
      TabOrder = 0
    end
    object chkGalPresent: TCheckBox
      Left = 8
      Top = 40
      Width = 177
      Height = 17
      Caption = #1053#1072#1083#1080#1095#1080#1077' '#1075#1072#1083#1100#1074#1072#1085#1086#1088#1072#1079#1074#1103#1079#1082#1080
      Enabled = False
      TabOrder = 1
    end
    object chkDspPresent: TCheckBox
      Left = 8
      Top = 56
      Width = 209
      Height = 17
      Caption = #1053#1072#1083#1080#1095#1080#1077' '#1089#1080#1075#1085#1072#1083#1100#1085#1086#1075#1086' '#1087#1088#1086#1094#1077#1089#1089#1086#1088#1072
      Enabled = False
      TabOrder = 2
    end
    object edtFpgaVer: TEdit
      Left = 8
      Top = 96
      Width = 89
      Height = 21
      ReadOnly = True
      TabOrder = 3
    end
    object edtPldaVer: TEdit
      Left = 8
      Top = 120
      Width = 89
      Height = 21
      ReadOnly = True
      TabOrder = 4
    end
    object chkEthPresent: TCheckBox
      Left = 8
      Top = 72
      Width = 209
      Height = 17
      Caption = #1053#1072#1083#1080#1095#1080#1077' '#1089#1077#1090#1077#1074#1086#1075#1086' '#1080#1085#1090#1077#1088#1092#1077#1081#1089#1072
      Enabled = False
      TabOrder = 5
    end
    object edtMcuVer: TEdit
      Left = 8
      Top = 144
      Width = 89
      Height = 21
      ReadOnly = True
      TabOrder = 6
    end
  end
  object btnAsyncDigIn: TButton
    Left = 8
    Top = 560
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1074#1086#1076' '#1094#1080#1092#1088#1086#1099#1093' '#1083#1080#1085#1080#1081
    TabOrder = 4
    OnClick = btnAsyncDigInClick
  end
  object edtAsyncDigIn: TEdit
    Left = 248
    Top = 560
    Width = 121
    Height = 21
    Enabled = False
    TabOrder = 5
    Text = '0x0000'
  end
  object btnAsyncDigOut: TButton
    Left = 8
    Top = 592
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1099#1074#1086#1076' '#1094#1080#1092#1088#1086#1074#1099#1093' '#1083#1080#1085#1080#1081
    TabOrder = 6
    OnClick = btnAsyncDigOutClick
  end
  object edtAsyncDigOut: TEdit
    Left = 248
    Top = 592
    Width = 121
    Height = 21
    TabOrder = 7
    Text = '0x0000'
  end
  object grp1: TGroupBox
    Left = 8
    Top = 224
    Width = 513
    Height = 65
    Caption = #1063#1072#1089#1090#1086#1090#1099' '#1089#1080#1085#1093#1088#1086#1085#1085#1086#1075#1086' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093
    TabOrder = 8
    object lbl3: TLabel
      Left = 16
      Top = 20
      Width = 139
      Height = 13
      Caption = #1063#1072#1089#1090#1086#1090#1072' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093' ('#1043#1094')'
    end
    object lbl4: TLabel
      Left = 168
      Top = 20
      Width = 113
      Height = 13
      Caption = #1063#1072#1089#1090#1086#1090#1072' '#1085#1072' '#1082#1072#1085#1072#1083' ('#1043#1094')'
    end
    object lbl5: TLabel
      Left = 296
      Top = 20
      Width = 166
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
      OnClick = btnSetAdcFreqClick
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
  object grp2: TGroupBox
    Left = 8
    Top = 368
    Width = 513
    Height = 153
    Caption = #1050#1072#1085#1072#1083#1099
    TabOrder = 9
    object lbl6: TLabel
      Left = 8
      Top = 40
      Width = 31
      Height = 13
      Caption = #1050#1072#1085#1072#1083
    end
    object lbl7: TLabel
      Left = 64
      Top = 40
      Width = 49
      Height = 13
      Caption = #1044#1080#1072#1087#1072#1079#1086#1085
    end
    object lbl8: TLabel
      Left = 152
      Top = 40
      Width = 88
      Height = 13
      Caption = #1056#1077#1078#1080#1084' '#1080#1079#1084#1077#1088#1077#1085#1080#1103
    end
    object lbl9: TLabel
      Left = 336
      Top = 40
      Width = 53
      Height = 13
      Caption = #1056#1077#1079#1091#1083#1100#1090#1072#1090
    end
    object lbl10: TLabel
      Left = 16
      Top = 16
      Width = 166
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
      Items.Strings = (
        '1'
        '2'
        '3')
    end
  end
  object btnAsyncAdcFrame: TButton
    Left = 8
    Top = 528
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1074#1086#1076' '#1082#1072#1076#1088#1072' '#1040#1062#1055
    TabOrder = 10
    OnClick = btnAsyncAdcFrameClick
  end
  object btnStop: TButton
    Left = 8
    Top = 112
    Width = 217
    Height = 25
    Caption = #1054#1089#1090#1072#1085#1086#1074' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093
    Enabled = False
    TabOrder = 11
    OnClick = btnStopClick
  end
  object btnStart: TButton
    Left = 8
    Top = 80
    Width = 217
    Height = 25
    Caption = #1047#1072#1087#1091#1089#1082' '#1089#1073#1086#1088#1072' '#1076#1072#1085#1085#1099#1093
    TabOrder = 12
    OnClick = btnStartClick
  end
  object btnAsyncDac1: TButton
    Left = 8
    Top = 624
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1099#1074#1086#1076' '#1085#1072' '#1062#1040#1055'1 ('#1042#1086#1083#1100#1090#1099')'
    TabOrder = 13
    OnClick = btnAsyncDac1Click
  end
  object edtAsyncDac1: TEdit
    Left = 248
    Top = 624
    Width = 81
    Height = 21
    TabOrder = 14
    Text = '5'
  end
  object edtAsyncDac2: TEdit
    Left = 248
    Top = 656
    Width = 81
    Height = 21
    TabOrder = 15
    Text = '5'
  end
  object btnAsyncDac2: TButton
    Left = 8
    Top = 656
    Width = 217
    Height = 25
    Caption = #1040#1089#1080#1085#1093#1088#1086#1085#1085#1099#1081' '#1074#1099#1074#1086#1076' '#1085#1072' '#1062#1040#1055'2 ('#1042#1086#1083#1100#1090#1099')'
    TabOrder = 16
    OnClick = btnAsyncDac2Click
  end
  object GroupBox4: TGroupBox
    Left = 8
    Top = 295
    Width = 513
    Height = 65
    Caption = #1057#1080#1085#1093#1088#1086#1085#1080#1079#1072#1094#1080#1103
    TabOrder = 17
    object Label10: TLabel
      Left = 16
      Top = 20
      Width = 173
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
  object btnOpenByIP: TButton
    Left = 8
    Top = 160
    Width = 217
    Height = 25
    Caption = #1059#1089#1090#1072#1085#1086#1074#1080#1090#1100' '#1089#1086#1077#1076#1080#1085#1077#1085#1080#1077' '#1087#1086' IP-'#1072#1076#1088#1077#1089#1091
    TabOrder = 18
    OnClick = btnOpenByIPClick
  end
  object edtIpAddr: TEdit
    Left = 8
    Top = 192
    Width = 217
    Height = 21
    TabOrder = 19
    Text = '192.168.0.1'
  end
end
