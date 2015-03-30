//---------------------------------------------------------------------------

#ifndef X502_THREAD_H
#define X502_THREAD_H


#include "l502api.h"
#include "e502api.h"

#include <vcl.h>


class X502_ProcessThread : public TThread {

protected:
    void __fastcall Execute();

public:
    bool stop;  //запрос на останов (устанавливается из основного потока)
    int err;  //код ошибки при выполнении потока сбора

    t_x502_hnd hnd; //описатель модуля
    TEdit* lchResEdits[X502_LTABLE_MAX_CH_CNT];
    TEdit* dinResEdit;

    __fastcall X502_ProcessThread(bool CreateSuspended);

private:
     void __fastcall updateData() ;
     double* adcData;
     uint32_t firstLch; /* номер первого логического канала в буфере АЦП */
     uint32_t adcSize;
     uint32_t* dinData;
     uint32_t dinSize;

};



//---------------------------------------------------------------------------
#endif
