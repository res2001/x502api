//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "unit.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

/* метод определяет какие управляющие элементы разрешены, а какие запрещены */
void TForm1::updateControls() {
    /* кнопка обновить список доступна пока не открыли устройство */
    btnRefreshDeviceList->Enabled = (hnd==NULL);
    cbbDevList->Enabled = (hnd==NULL);

    btnOpen->Caption = hnd == NULL ? "Установить связь с модулем" :
                        "Разорвать связь с модулем";
    btnOpen->Enabled = (hnd!=NULL) || (cbbDevList->ItemIndex >= 0);
    btnOpenByIP->Enabled = (hnd==NULL);
    edtIpAddr->Enabled = (hnd==NULL);

    chkSyncDin->Enabled = (hnd!=NULL) && !threadRunning;
    btnStart->Enabled = (hnd!=NULL) && !threadRunning;
    btnStop->Enabled = (hnd!=NULL) && threadRunning;

    btnSetAdcFreq->Enabled = (hnd!=NULL) && !threadRunning;

    btnAsyncDigOut->Enabled = (hnd!=NULL);
    btnAsyncDigIn->Enabled = (hnd!=NULL);
    btnAsyncDac1->Enabled = (hnd!=NULL) && chkDacPresent->Checked;
    btnAsyncDac2->Enabled = (hnd!=NULL) && chkDacPresent->Checked;
    btnAsyncAdcFrame->Enabled = (hnd!=NULL) && !threadRunning;

}

void TForm1::freeDevList() {
    if (devrecList.Length != 0) {
        X502_FreeDevRecordList(&devrecList[0], devrecList.Length);
        devrecList.Length = 0;
    }
    cbbDevList->Items->Clear();
}



/* обновление списка подключенных плат L502 и E502*/
void TForm1::refreshDeviceList(void) {
    uint32_t pci_devcnt = 0;
    uint32_t usb_devcnt = 0;
    int32_t fnd_devcnt = 0;

    int old_index = cbbDevList->ItemIndex;

    /* очищаем ресурсы записей, если были выделены */
    freeDevList();

    /* получаем количество подключенных устройств по интерфейсам PCI и USB */
    L502_GetDevRecordsList(NULL, 0, 0, &pci_devcnt);
    E502_UsbGetDevRecordsList(NULL, 0, 0, &usb_devcnt);

    if ((pci_devcnt+usb_devcnt) != 0) {
        /* выделяем память для массива для сохранения найденного
            количества записей */
        devrecList.Length  = pci_devcnt + usb_devcnt;

        /* получаем записи о модулях L502, но не больше pci_devcnt */
        if (pci_devcnt!=0) {
            int32_t res = L502_GetDevRecordsList(&devrecList[fnd_devcnt],
                                                  pci_devcnt, 0, NULL);
            if (res >= 0) {
                fnd_devcnt += res;
            }
        }
        /* добавляем записи о модулях E502, подключенных по USB, в конец массива */
        if (usb_devcnt!=0) {
            int32_t res = E502_UsbGetDevRecordsList(&devrecList[fnd_devcnt],
                                                        usb_devcnt, 0, NULL);
            if (res >= 0) {
                fnd_devcnt += res;
            }
        }

        devrecList.Length = fnd_devcnt;
        if (fnd_devcnt > 0) {
            /* добавляем все найденные записи в список ComboBox'а для выбора */
            for (int32_t i=0; i < fnd_devcnt; i++) {
                cbbDevList->Items->Add(AnsiString(devrecList[i].devname) + ", " + AnsiString(devrecList[i].serial));
            }
            /* если до обновления списка был выбран действительный индекс - восстанавливаем его */
            cbbDevList->ItemIndex = (old_index >= 0) && (old_index < fnd_devcnt) ? old_index : 0;
        }
    }
    updateControls();
}


/* Установка пераметров сбора модуля в соответствии с элементами GUI */
int32_t TForm1::setupParams() {
    int32_t err = 0;
    uint32_t lch_cnt = cbbLChCnt->ItemIndex+1;
    /* таблица соответствия индексов в ComboBox и кодов режима измерения */
    static uint32_t f_mode_tbl[] = {X502_LCH_MODE_COMM, X502_LCH_MODE_DIFF, X502_LCH_MODE_ZERO};
    /* таблица соответствия индексов в ComboBox и кодов диапазонов АЦП */
    static uint32_t f_range_tbl[] = {X502_ADC_RANGE_10, X502_ADC_RANGE_5, X502_ADC_RANGE_2,
                                     X502_ADC_RANGE_1, X502_ADC_RANGE_05, X502_ADC_RANGE_02};
    /* таблица соответствия индексов в ComboBox и кодов источника синхронизации */
    static uint32_t f_sync_tbl[] = {X502_SYNC_INTERNAL, X502_SYNC_EXTERNAL_MASTER,
                                    X502_SYNC_DI_SYN1_RISE, X502_SYNC_DI_SYN2_RISE,
                                    X502_SYNC_DI_SYN1_FALL, X502_SYNC_DI_SYN2_FALL};

    /* Устанавливаем кол-во логических каналов */
    err = X502_SetLChannelCount(hnd, lch_cnt);
    if (err == X502_ERR_OK) {
        err = X502_SetLChannel(hnd, 0, cbbLCh1_Channel->ItemIndex,
                               f_mode_tbl[cbbLCh1_Mode->ItemIndex], f_range_tbl[cbbLCh1_Range->ItemIndex],0);
    }
    if ((err == X502_ERR_OK) && (lch_cnt>=2)) {
        err = X502_SetLChannel(hnd, 1, cbbLCh2_Channel->ItemIndex,
                                f_mode_tbl[cbbLCh2_Mode->ItemIndex], f_range_tbl[cbbLCh2_Range->ItemIndex],0);
    }
    if ((err == X502_ERR_OK) && (lch_cnt>=3)) {
        err = X502_SetLChannel(hnd, 2, cbbLCh3_Channel->ItemIndex,
              f_mode_tbl[cbbLCh3_Mode->ItemIndex], f_range_tbl[cbbLCh3_Range->ItemIndex],0);
    }

    /* Настраиваем источник частоты синхронизации и запуска сбора */
    if (err == X502_ERR_OK)
        err = X502_SetSyncMode(hnd, f_sync_tbl[cbbSyncMode->ItemIndex]);
    if (err == X502_ERR_OK)
        err = X502_SetSyncStartMode(hnd, f_sync_tbl[cbbSyncStartMode->ItemIndex]);

    /* настраиваем частоту сбора с АЦП */
    if (err == X502_ERR_OK)
        err = setAdcFreq();

    /* Записываем настройки в модуль */
    if (err == X502_ERR_OK)
        err = X502_Configure(hnd, 0);

    return err;
}


int32_t TForm1::setAdcFreq() {
    double f_acq = StrToFloat(edtAdcFreq->Text);
    double f_lch = StrToFloat(edtAdcFreqLch->Text);
    double f_din = StrToFloat(edtDinFreq->Text);
    /* устанавливаем требуемую частоту сбора. */
    int32_t err = X502_SetAdcFreq(hnd, &f_acq, &f_lch);
    if (err == X502_ERR_OK) {
        /* обновляем значение индикатора, для отображения
           реально установившейся частоты */
        edtAdcFreq->Text = FloatToStr(f_acq);
        edtAdcFreqLch->Text = FloatToStr(f_lch);

        /* Устанавливаем частоту синхронного сбора */
        err = X502_SetDinFreq(hnd, &f_din);
        if (err == X502_ERR_OK)
            edtDinFreq->Text = FloatToStr(f_din);
    }
    return err;
}


int32_t TForm1::setSyncDinStream() {
    int32_t err;
    /* разрешаем или запрещаем поток синхронного ввода
       в зависимости от состояния переключателя */
    if (chkSyncDin->Checked) {
        err = X502_StreamsEnable(hnd, X502_STREAM_DIN);
    } else {
        err = X502_StreamsDisable(hnd, X502_STREAM_DIN);
    }
    return err;
}

void __fastcall TForm1::OnThreadTerminate(TObject *obj) {
    if (thread->err != X502_ERR_OK) {
        MessageDlg("Сбор данных завершен с ошибкой: " + String(X502_GetErrorString(thread->err)),
                    mtError, TMsgDlgButtons() << mbOK,NULL);
    }

    threadRunning = false;
    updateControls();
}



//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner), hnd(NULL) {
    refreshDeviceList();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::btnRefreshDeviceListClick(TObject *Sender) {
    refreshDeviceList();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnStartClick(TObject *Sender)  {
    /* настраиваем все параметры в соответствии с элементами управления */
    int32_t err = setupParams();
    if (err != X502_ERR_OK) {
        MessageDlg("Ошибка настройки параметров платы: " + String(X502_GetErrorString(err)),
            mtError, TMsgDlgButtons() << mbOK,NULL);
    }

    /* разрешаем синхронный ввод АЦП */
    if (err == X502_ERR_OK)
        err = X502_StreamsEnable(hnd, X502_STREAM_ADC);
    /* разрешаем синхронный ввод с цифровых линий в зависимости от переключателя */
    if (err == X502_ERR_OK)
        err = setSyncDinStream();


    if (err == X502_ERR_OK) {
        /* если остался не удаленный объект потока - удаляем */
        if (thread) {
            delete thread;
            thread = NULL;
        }
        //создание класса потока
        thread = new X502_ProcessThread(true);
        thread->hnd = hnd;

        //передача потоку элементов управления для отображения результатов сбора
        thread->lchResEdits[0] = edtLCh1_Result;
        thread->lchResEdits[1] = edtLCh2_Result;
        thread->lchResEdits[2] = edtLCh3_Result;
        thread->dinResEdit = edtDin_Result;

        edtLCh1_Result->Text="";
        edtLCh2_Result->Text="";
        edtLCh3_Result->Text="";
        edtDin_Result->Text="";

        /* устанавливаем функцию на событие завершения потока (в частности
            чтобы отследить, если поток завершился сам из-за ошибки при сборе
            данных) */
        thread->OnTerminate = OnThreadTerminate;
        thread->Resume();  //запуск потока
        threadRunning = true;

        updateControls();
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnStopClick(TObject *Sender) {
    /* устанавливаем запрос на завершение потока */
    if (threadRunning)
        thread->stop = true;
    btnStop->Enabled = false;
}


void TForm1::showDevInfo(void) {
    t_x502_info info;
    int32_t err = X502_GetDevInfo(hnd, &info);
    if (err == X502_ERR_OK) {
        chkDacPresent->Checked = info.devflags & X502_DEVFLAGS_DAC_PRESENT;
        chkGalPresent->Checked = info.devflags & X502_DEVFLAGS_GAL_PRESENT;
        chkDspPresent->Checked = info.devflags & X502_DEVFLAGS_BF_PRESENT;
        chkEthPresent->Checked = info.devflags & X502_DEVFLAGS_IFACE_SUPPORT_ETH;

        edtPldaVer->Text = IntToStr(info.plda_ver);
        edtFpgaVer->Text = IntToStr((info.fpga_ver>>8)&0xFF) + "."
                            + IntToStr(info.fpga_ver & 0xFF);
        if (info.mcu_firmware_ver != 0) {
			edtMcuVer->Text = IntToStr((int)(info.mcu_firmware_ver>>24)&0xFF) + "." +
							  IntToStr((int)(info.mcu_firmware_ver>>16)&0xFF) + "." +
							  IntToStr((int)(info.mcu_firmware_ver>>8)&0xFF) + "." +
                             IntToStr((int)info.mcu_firmware_ver & 0xFF);
        } else {
            edtMcuVer->Text = "";
        }                           
    } else {
        MessageDlg("Ошибка получения информации о модуле: " + String(X502_GetErrorString(err)),
                              mtError, TMsgDlgButtons() << mbOK,NULL);

        X502_Close(hnd);
        X502_Free(hnd);
        hnd = NULL;
    }
}
/* функция открытия устройства с выбранным серийным номером или закрытия его,
    если оно уже было открыто */
void __fastcall TForm1::btnOpenClick(TObject *Sender)   {

    /* если устройство было закрыто - создаем новый описатель и открываем его */
    if (hnd==NULL) {
        int idx = cbbDevList->ItemIndex;
        if ((idx >= 0) && (idx < devrecList.Length)) {
            hnd = X502_Create();
            if (hnd==NULL) {
                MessageDlg("Ошибка создания описателя модуля", mtError, TMsgDlgButtons() << mbOK,NULL);
            } else {
                /* открываем модуль, соответствующий записи с выбранным индексом */
                int32_t err = X502_OpenByDevRecord(hnd, &devrecList[idx]);
                if (err != X502_ERR_OK) {
                    MessageDlg("Ошибка открытия модуля: " + String(X502_GetErrorString(err)),
                                mtError, TMsgDlgButtons() << mbOK,NULL);
                    X502_Free(hnd);
                    hnd = NULL;
                } else {
                    showDevInfo();
                }
            }
        }
    } else {
        /* если устройство было открыто - закрываем */
        closeDevice();
    }
    updateControls();
}

uint32_t TForm1::parseIpAddr(AnsiString addr) {
    int a[4],i;
    if (sscanf(addr.c_str(), "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3])!=4) {
        throw new Exception("Неверный формат IP-адреса!!");
    }

    for (i=0; i < 4; i++) {
        if ((a[i]<0) || (a[i] > 255)) {
            throw new Exception("Недействительный IP-адрес!!");
        }
    }


    return (a[0] << 24) | (a[1]<<16) | (a[2]<<8) | a[3];
}
void __fastcall TForm1::btnOpenByIPClick(TObject *Sender) {
    t_x502_devrec devrec;
    int32_t err = E502_MakeDevRecordByIpAddr(&devrec, parseIpAddr(edtIpAddr->Text), 0, 5000);
    if (err != X502_ERR_OK) {
         MessageDlg("Ошибка создания записи: " + String(X502_GetErrorString(err)),
                                                mtError, TMsgDlgButtons() << mbOK,NULL);
    } else {
        hnd = X502_Create();
        if (hnd==NULL) {
            MessageDlg("Ошибка создания описателя модуля", mtError, TMsgDlgButtons() << mbOK,NULL);
        } else {
            /* устанавливаем связь по созданной записи */
            int32_t err = X502_OpenByDevRecord(hnd, &devrec);
            if (err != X502_ERR_OK) {
                 MessageDlg("Ошибка открытия модуля: " + String(X502_GetErrorString(err)),
                              mtError, TMsgDlgButtons() << mbOK,NULL);
                 X502_Free(hnd);
                 hnd = NULL;
            } else {
                showDevInfo();
            }
        }
    }
    updateControls();
}


//---------------------------------------------------------------------------

void TForm1::closeDevice() {
    if (hnd != NULL) {
        /* если запущен поток сбора данных - то останавливаем его */
        if (threadRunning) {
            thread->stop=1;
            thread->WaitFor();
        }
        /* закрытие связи с модулем */
        X502_Close(hnd);
        /* освобождение описателя */
        X502_Free(hnd);
        hnd = NULL;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action) {
    closeDevice();
    freeDevList();
    if (thread)
        delete thread;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAsyncDigOutClick(TObject *Sender) {
    if (hnd!=NULL) {
        uint32_t val = StrToInt(edtAsyncDigOut->Text);
        int32_t err = X502_AsyncOutDig(hnd, val, 0);
        if (err != X502_ERR_OK) {
            MessageDlg("Ошибка асинхронного вывода на цифровые линии: " + String(X502_GetErrorString(err)),
                        mtError, TMsgDlgButtons() << mbOK,NULL);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::adcFreqEditChange(TObject *Sender) {
    if (hnd != NULL) {
        /* устанавливаем кол-во логических каналов, чтобы правильно
           расчитать частоту на канал */
        int32_t err = X502_SetLChannelCount(hnd, cbbLChCnt->ItemIndex+1);
        if (err == X502_ERR_OK)
            err = setAdcFreq();
        if (err != X502_ERR_OK) {
            MessageDlg("Ошибка установки частоты сбора: " + String(X502_GetErrorString(err)),
                        mtError, TMsgDlgButtons() << mbOK,NULL);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::chkSyncDinClick(TObject *Sender) {
    if (hnd != NULL) {
        int32_t err = setSyncDinStream();
        if (err != X502_ERR_OK) {
            MessageDlg("Ошибка разрешения/запрета синхронного ввода: " + String(X502_GetErrorString(err)),
                        mtError, TMsgDlgButtons() << mbOK,NULL);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAsyncDigInClick(TObject *Sender) {
    if (hnd != NULL) {
        uint32_t din;
        int32_t err = X502_AsyncInDig(hnd, &din);
        if (err != X502_ERR_OK) {
            MessageDlg("Ошибка асинхронного ввода с цифровых линий: " + String(X502_GetErrorString(err)),
                        mtError, TMsgDlgButtons() << mbOK,NULL);
        } else {
            edtAsyncDigIn->Text = IntToHex((int)din&0x3FFFF, 5);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAsyncDac1Click(TObject *Sender) {
    if (hnd != NULL) {
        double val = StrToFloat(edtAsyncDac1->Text);;
        int32_t err;
        err = X502_AsyncOutDac(hnd, X502_DAC_CH1, val, X502_DAC_FLAGS_CALIBR |
                                                        X502_DAC_FLAGS_VOLT);
        if (err != X502_ERR_OK) {
            MessageDlg("Ошибка вывода на ЦАП: " + String(X502_GetErrorString(err)),
                        mtError, TMsgDlgButtons() << mbOK,NULL);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAsyncDac2Click(TObject *Sender) {
    if (hnd != NULL) {
        double val = StrToFloat(edtAsyncDac2->Text);;
        int32_t err;
        err = X502_AsyncOutDac(hnd, X502_DAC_CH2, val, X502_DAC_FLAGS_CALIBR |
                                                        X502_DAC_FLAGS_VOLT);
        if (err != X502_ERR_OK) {
            MessageDlg("Ошибка вывода на ЦАП: " + String(X502_GetErrorString(err)),
                        mtError, TMsgDlgButtons() << mbOK,NULL);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnAsyncAdcFrameClick(TObject *Sender) {
    if (hnd != NULL) {
        uint32_t lch_cnt;
        /* настраиваем логическую таблицу АЦП */
        int32_t err = setupParams();
        if (err == X502_ERR_OK)
            err = X502_GetLChannelCount(hnd, &lch_cnt);

        if (err != X502_ERR_OK) {
            MessageDlg("Ошибка настройки параметров АЦП: " + String(X502_GetErrorString(err)),
                        mtError, TMsgDlgButtons() << mbOK,NULL);
        }

        if (err == X502_ERR_OK) {
            /* в примере используем 3 канала - создаем массив для приема
               отсчета на каждый канал */
            double adc_data[3];
            err = X502_AsyncGetAdcFrame(hnd, X502_PROC_FLAGS_VOLT, 1000, adc_data);
            if (err != X502_ERR_OK) {
                MessageDlg("Ошибка приема кадра АЦП: " + String(X502_GetErrorString(err)),
                            mtError, TMsgDlgButtons() << mbOK,NULL);
            } else {
                edtLCh1_Result->Text = FloatToStrF(adc_data[0], ffFixed, 4, 8);
                if (lch_cnt>=2) {
                    edtLCh2_Result->Text = FloatToStrF(adc_data[1], ffFixed, 4, 8);
                } else {
                    edtLCh2_Result->Text ="";
                }

                if (lch_cnt>=3) {
                    edtLCh3_Result->Text = FloatToStrF(adc_data[2], ffFixed, 4, 8);
                } else {
                    edtLCh3_Result->Text = "";
                }
            }
        }
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::cbbDevListChange(TObject *Sender)  {
    //refreshDeviceList();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::cbbLChCntChange(TObject *Sender) {
    if (cbbLChCnt->ItemIndex < 0)
        cbbLChCnt->ItemIndex = 0;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

