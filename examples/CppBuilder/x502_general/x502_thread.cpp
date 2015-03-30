//---------------------------------------------------------------------------


#pragma hdrstop

#include "x502_thread.h"


//---------------------------------------------------------------------------

#pragma package(smart_init)

#define RECV_BUF_SIZE  8*1024*1024
#define RECV_TOUT      250


 __fastcall X502_ProcessThread::X502_ProcessThread(bool CreateSuspended)
    : TThread(CreateSuspended), stop(false), err (X502_ERR_OK) {

 }

/* обновление индикаторов формы результатами последнего измерения.
   Метод должен выполняться только через Syncronize, который нужен
   для доступа к элементам VCL не из основного потока */
void __fastcall X502_ProcessThread::updateData()  {
    uint32_t lch_cnt;
    err = X502_GetLChannelCount(hnd, &lch_cnt);
    if (err == X502_ERR_OK) {
        /* устанавливаем в индикаторах значение первого отсчета из массива */
        for (uint32_t i=0; (i < lch_cnt) && (i < adcSize); i++)
            lchResEdits[(firstLch + i)%lch_cnt]->Text = FloatToStrF(adcData[i], ffFixed, 4, 8);

        /* если есть данные цифрового выхода, то устанавливаем индикатор
            в соответствии с первым значением */
        if (dinSize) {
            dinResEdit->Text = IntToHex((int)dinData[0]&0x3FFFF, 5);
        } else {
            dinResEdit->Text = "";
        }
    }
}

 void __fastcall X502_ProcessThread::Execute() {
    /* выделяем буферы под принимаемые данные */
    uint32_t *rcv_buf = new uint32_t[RECV_BUF_SIZE];
    dinData = new uint32_t[RECV_BUF_SIZE];
    adcData = new double[RECV_BUF_SIZE];


    if (!rcv_buf || !dinData || !adcData) {
        err = X502_ERR_MEMORY_ALLOC;
    } else {
        /* запускаем синхронные потоки */
        err = X502_StreamsStart(hnd);
        if (err == X502_ERR_OK) {
            /* выполняем прием пока не произойдет ошибка или
                не будет запроса на останов от основного приложения */
            while (!stop && (err == X502_ERR_OK)) {
                /* принимаем данные синхронного ввода */
                int32_t rcv_size = X502_Recv(hnd, rcv_buf, RECV_BUF_SIZE, RECV_TOUT);
                /* значение меньше нуля означает ошибку... */
                if (rcv_size < 0) {
                    err = rcv_size;
                } else if (rcv_size>0) {
                    /* если больше нуля - значит приняли новые данные */
                    dinSize=RECV_BUF_SIZE;
                    adcSize=RECV_BUF_SIZE;
                    /* получаем номер лог. какнала, соответствующий первому
                        отсчету АЦП, так как до этого могли обработать
                        некратное количество кадров */
                    err = X502_GetNextExpectedLchNum(hnd, &firstLch);
                    if (err == X502_ERR_OK) {
                        /* разбираем данные на синхронный ввод и отсчеты АЦП и
                            переводим АЦП в Вольты */
                        err = X502_ProcessData(hnd, rcv_buf, rcv_size, L502_PROC_FLAGS_VOLT,
                                            adcData, &adcSize, dinData, &dinSize);
                    }

                    if (err == X502_ERR_OK) {
                        /* обновляем значения элементов управления */
                        Synchronize(updateData);
                    }
                }
            }

            /* по выходу из цикла отсанавливаем поток.
               Чтобы не сбросить код ошибки (если вышли по ошибке)
               результат останова сохраняем в отдельную переменную */
            int32_t stop_err = X502_StreamsStop(hnd);
            if (err == X502_ERR_OK)
                err = stop_err;
        }
    }


    /* освобождаем выделенную память */
    delete rcv_buf;
    delete dinData;
    delete adcData;
 }
