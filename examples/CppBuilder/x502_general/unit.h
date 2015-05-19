//---------------------------------------------------------------------------

#ifndef unitH
#define unitH

#include "l502api.h"
#include "e502api.h"
#include "x502_thread.h"

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TButton *btnRefreshDeviceList;
    TComboBox *cbbDevList;
    TButton *btnStart;
    TGroupBox *GroupBox1;
    TLabel *Label2;
    TLabel *Label1;
    TComboBox *cbbLCh1_Channel;
    TComboBox *cbbLCh1_Range;
    TComboBox *cbbLCh1_Mode;
    TEdit *edtLCh1_Result;
    TComboBox *cbbLCh2_Channel;
    TComboBox *cbbLCh2_Range;
    TComboBox *cbbLCh2_Mode;
    TEdit *edtLCh2_Result;
    TComboBox *cbbLCh3_Channel;
    TComboBox *cbbLCh3_Range;
    TComboBox *cbbLCh3_Mode;
    TComboBox *cbbSyncMode;
    TComboBox *cbbSyncStartMode;
    TEdit *edtLCh3_Result;
    TButton *btnStop;
    TLabel *Label3;
    TLabel *Label4;
    TCheckBox *chkSyncDin;
    TEdit *edtDin_Result;
    TButton *btnOpen;
    TButton *btnAsyncDigOut;
    TEdit *edtAsyncDigOut;
    TEdit *edtAsyncDigIn;
    TButton *btnAsyncDigIn;
    TGroupBox *GroupBox2;
    TCheckBox *chkDacPresent;
    TCheckBox *chkGalPresent;
    TCheckBox *chkDspPresent;
    TEdit *edtFpgaVer;
    TLabel *Label6;
    TEdit *edtPldaVer;
    TLabel *labelPldaVer;
    TEdit *edtAsyncDac1;
    TButton *btnAsyncDac1;
    TButton *btnAsyncDac2;
    TEdit *edtAsyncDac2;
    TButton *btnAsyncAdcFrame;
    TGroupBox *GroupBox3;
    TButton *btnSetAdcFreq;
    TLabel *Label5;
    TEdit *edtAdcFreq;
    TEdit *edtAdcFreqLch;
    TEdit *edtDinFreq;
    TLabel *Label7;
    TLabel *Label8;
    TComboBox *cbbLChCnt;
    TLabel *Label9;
    TLabel *lbl11;
    TEdit *edtMcuVer;
    TCheckBox *chkEthPresent;
    TButton *btnOpenByIP;
    TEdit *edtIpAddr;

    void __fastcall btnRefreshDeviceListClick(TObject *Sender);
    void __fastcall btnStartClick(TObject *Sender);
    void __fastcall btnStopClick(TObject *Sender);
    void __fastcall btnOpenClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnAsyncDigOutClick(TObject *Sender);
    void __fastcall adcFreqEditChange(TObject *Sender);
    void __fastcall chkSyncDinClick(TObject *Sender);
    void __fastcall btnAsyncDigInClick(TObject *Sender);
    void __fastcall btnAsyncDac1Click(TObject *Sender);
    void __fastcall btnAsyncDac2Click(TObject *Sender);
    void __fastcall btnAsyncAdcFrameClick(TObject *Sender);
    void __fastcall cbbDevListChange(TObject *Sender);
    void __fastcall cbbLChCntChange(TObject *Sender);
    void __fastcall btnOpenByIPClick(TObject *Sender);
private:	// User declarations
    t_x502_hnd hnd;  //описатель модуля
    bool threadRunning; //признак, запущен ли поток сбора данных
    X502_ProcessThread *thread; //поток для синхронного ввода

    DynamicArray<t_x502_devrec> devrecList;
    void freeDevList();
    void refreshDeviceList();
    void closeDevice();
    void showDevInfo();
    uint32_t parseIpAddr(AnsiString addr);

    int32_t setSyncDinStream();
    int32_t setAdcFreq();
    int32_t setupParams();

    void updateControls();

    __fastcall void OnThreadTerminate(TObject *obj);



public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
