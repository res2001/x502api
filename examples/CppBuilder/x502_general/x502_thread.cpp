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

/* ���������� ����������� ����� ������������ ���������� ���������.
   ����� ������ ����������� ������ ����� Syncronize, ������� �����
   ��� ������� � ��������� VCL �� �� ��������� ������ */
void __fastcall X502_ProcessThread::updateData()  {
    uint32_t lch_cnt;
    err = X502_GetLChannelCount(hnd, &lch_cnt);
    if (err == X502_ERR_OK) {
        /* ������������� � ����������� �������� ������� ������� �� ������� */
        for (uint32_t i=0; (i < lch_cnt) && (i < adcSize); i++)
            lchResEdits[(firstLch + i)%lch_cnt]->Text = FloatToStrF(adcData[i], ffFixed, 4, 8);

        /* ���� ���� ������ ��������� ������, �� ������������� ���������
            � ������������ � ������ ��������� */
        if (dinSize) {
            dinResEdit->Text = IntToHex((int)dinData[0]&0x3FFFF, 5);
        } else {
            dinResEdit->Text = "";
        }
    }
}

 void __fastcall X502_ProcessThread::Execute() {
    /* �������� ������ ��� ����������� ������ */
    uint32_t *rcv_buf = new uint32_t[RECV_BUF_SIZE];
    dinData = new uint32_t[RECV_BUF_SIZE];
    adcData = new double[RECV_BUF_SIZE];


    if (!rcv_buf || !dinData || !adcData) {
        err = X502_ERR_MEMORY_ALLOC;
    } else {
        /* ��������� ���������� ������ */
        err = X502_StreamsStart(hnd);
        if (err == X502_ERR_OK) {
            /* ��������� ����� ���� �� ���������� ������ ���
                �� ����� ������� �� ������� �� ��������� ���������� */
            while (!stop && (err == X502_ERR_OK)) {
                /* ��������� ������ ����������� ����� */
                int32_t rcv_size = X502_Recv(hnd, rcv_buf, RECV_BUF_SIZE, RECV_TOUT);
                /* �������� ������ ���� �������� ������... */
                if (rcv_size < 0) {
                    err = rcv_size;
                } else if (rcv_size>0) {
                    /* ���� ������ ���� - ������ ������� ����� ������ */
                    dinSize=RECV_BUF_SIZE;
                    adcSize=RECV_BUF_SIZE;
                    /* �������� ����� ���. �������, ��������������� �������
                        ������� ���, ��� ��� �� ����� ����� ����������
                        ��������� ���������� ������ */
                    err = X502_GetNextExpectedLchNum(hnd, &firstLch);
                    if (err == X502_ERR_OK) {
                        /* ��������� ������ �� ���������� ���� � ������� ��� �
                            ��������� ��� � ������ */
                        err = X502_ProcessData(hnd, rcv_buf, rcv_size, L502_PROC_FLAGS_VOLT,
                                            adcData, &adcSize, dinData, &dinSize);
                    }

                    if (err == X502_ERR_OK) {
                        /* ��������� �������� ��������� ���������� */
                        Synchronize(updateData);
                    }
                }
            }

            /* �� ������ �� ����� ������������� �����.
               ����� �� �������� ��� ������ (���� ����� �� ������)
               ��������� �������� ��������� � ��������� ���������� */
            int32_t stop_err = X502_StreamsStop(hnd);
            if (err == X502_ERR_OK)
                err = stop_err;
        }
    }


    /* ����������� ���������� ������ */
    delete rcv_buf;
    delete dinData;
    delete adcData;
 }
