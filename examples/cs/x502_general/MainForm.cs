using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Net;

using x502api;
using lpcieapi;

namespace x502_example
{
    public partial class MainForm : Form
    {
        const uint RECV_BUF_SIZE = 8*1024*1024;
        const uint RECV_TOUT = 250;

        X502.DevRec[] devrecs;

        X502 hnd; /* Описатель модуля с которым работаем (null, если связи нет) */
        Thread thread; /* Объект потока для синхронного сбора */
        bool reqStop; /* Запрос на останов потока сбора данных */
        bool threadRunning; /* Признак, идет ли сбор данных в отдельном потоке */

        UInt32[] rcv_buf; /* буфер для приема сырых данных */
        double[] adcData; /* буфер для данных АЦП */
        UInt32[] dinData; /* буфер для отсчетов цифровых входов */
        UInt32 adcSize, dinSize; /* размер действительных данных в adcData и dinData */
        UInt32 firstLch; /* номер логического канала,  которому соответствует отсчет в adcData[0] */





        private delegate void finishThreadDelegate(lpcie.Errs err);
        private delegate void updateDateDelegate();
        
        /* Обновление элементов управления, значениями, снятыми при синхронном вводе.
         * Так работа с интерфейсом может быть только из основного потока, а
         * фунция вызывается из потока сбора данных, то если мы находимся
         * в потоке сбора данных, планируем ее выполнение в основном потоке */
        private void UpdateData()
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new updateDateDelegate(this.UpdateData));
            }
            else
            {
                TextBox[] lchResEdits = {edtLCh1_Result, edtLCh2_Result, edtLCh3_Result};
                UInt32 lch_cnt = hnd.LChannelCount;
                /* устанавливаем в индикаторах значение первого отсчета из массива */
                for (uint i = 0; (i < lch_cnt) && (i < adcSize); i++)
                    lchResEdits[(firstLch + i) % lch_cnt].Text = adcData[i].ToString("F7");

                /* если есть данные цифрового выхода, то устанавливаем индикатор
                    в соответствии с первым значением */
                if (dinSize>0)
                    edtDin_Result.Text = dinData[0].ToString("X5");
                else
                    edtDin_Result.Text = "";
            }
        }

        /* Функция, вызываемая по завершению потока сбора данных.
         * Так как она работает с интерфейсом, то при вызове из другого потока,
         * она планируется на выполнения в основном потоке, как и UpdateData */
        private void finishThread(lpcie.Errs err)
        {
            if (this.InvokeRequired)
                this.Invoke(new finishThreadDelegate(this.finishThread), err);
            else
            {
                if (err != lpcie.Errs.OK)
                {
                    MessageBox.Show(X502.GetErrorString(err), "Сбор данных завершен с ошибкой",
                               MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                threadRunning = false;
                updateControls();
            }
        }


        /* Функция синхронного сбора данных. Выполняется в отдельном потоке */
        private void threadFunc()
        {
            reqStop = false;
            /* запускаем синхронные потоки ввода-вывода*/
            lpcie.Errs err = hnd.StreamsStart();
            if (err == lpcie.Errs.OK)
            {
                /* выполняем прием пока не произойдет ошибка или
                    не будет запроса на останов от основного приложения */
                while (!reqStop && (err == lpcie.Errs.OK))
                {
                    /* принимаем данные синхронного ввода */
                    Int32 rcv_size = hnd.Recv(rcv_buf, RECV_BUF_SIZE, RECV_TOUT);
                    /* значение меньше нуля означает ошибку... */
                    if (rcv_size < 0)
                        err = (lpcie.Errs)rcv_size;
                    else if (rcv_size > 0)
                    {
                        /* если больше нуля - значит приняли новые данные */
                        dinSize = RECV_BUF_SIZE;
                        adcSize = RECV_BUF_SIZE;
                        /* получаем номер лог. какнала, соответствующий первому
                            отсчету АЦП, так как до этого могли обработать
                            некратное количество кадров */
                        firstLch = hnd.NextExpectedLchNum;
     
                        /* разбираем данные на синхронный ввод и отсчеты АЦП и
                            переводим АЦП в Вольты */
                        err = hnd.ProcessData(rcv_buf, (uint)rcv_size, X502.ProcFlags.VOLT,
                                              adcData, ref adcSize, dinData, ref dinSize);

                        if (err == lpcie.Errs.OK)
                        {
                            /* обновляем значения элементов управления */
                            UpdateData();
                        }
                    }
                }

                /* по выходу из цикла отсанавливаем поток.
                   Чтобы не сбросить код ошибки (если вышли по ошибке)
                   результат останова сохраняем в отдельную переменную */
                lpcie.Errs stop_err = hnd.StreamsStop();
                if (err == lpcie.Errs.OK)
                    err = stop_err;
            }

            /* завершаем поток */
            finishThread(err);
        }





        /* обновление состояния элементов управления (какие разрешены, какие нет) */
        private void updateControls()
        {
            btnRefreshDeviceList.Enabled = hnd==null;
            cbbSerialList.Enabled = hnd==null;

            btnOpen.Text = hnd==null ? "Установить связь с устройством" :
                                                "Разорвать связь с устройством";
            
            btnOpen.Enabled = (hnd!=null) || (cbbSerialList.SelectedItem != null);
            btnOpenByIP.Enabled = (hnd == null);

            chkSyncDin.Enabled = (hnd != null) && !threadRunning;
            btnStart.Enabled = (hnd != null) && !threadRunning;
            btnStop.Enabled = (hnd != null) && threadRunning;

            btnSetAdcFreq.Enabled = (hnd != null) && !threadRunning;

            btnAsyncDigOut.Enabled = (hnd != null);
            btnAsyncDigIn.Enabled = (hnd != null);
            btnAsyncDac1.Enabled = (hnd != null) && chkDacPresent.Checked;
            btnAsyncDac2.Enabled = (hnd != null) && chkDacPresent.Checked;
            btnAsyncAdcFrame.Enabled = (hnd != null) && !threadRunning;
        }


        private void deviceClose()
        {
            if (hnd != null)
            {
                //останов сбора данных 
                if (threadRunning)
                {
                    reqStop = true;
                    
                    /* ожидаем завершения потока. Так как 
                     * поток работает с GUI и планирует выполнение части
                     * функций в основном потоке, то мы не можем сдесь просто
                     * сделать Join, вызываем Application.DoEvents(), чтобы
                     * в нем обработать запланированные UpdateData()/finishThread() */
                    while (threadRunning)
                    {
                        Application.DoEvents();
                    }
                }

                // закрытие связи с модулем
                hnd.Close();
                // память освободится диспетчером мусора, т.к. нет больше ссылок
                hnd = null;
            }
        }

        private void refreshDevList()
        {
            cbbSerialList.Items.Clear();
            
            //получаем список серийных номеров
            X502.DevRec[] pci_devrecs;
            X502.DevRec[] usb_devrecs;
            Int32 res = L502.GetDevRecordsList(out pci_devrecs, 0);
            res = E502.UsbGetDevRecordsList(out usb_devrecs, 0);

            devrecs = new X502.DevRec[pci_devrecs.Length + usb_devrecs.Length];
            pci_devrecs.CopyTo(devrecs, 0);
            usb_devrecs.CopyTo(devrecs, pci_devrecs.Length);

            
            /* заполняем полученные серийные номера в ComboBox */
            for (int i = 0; i < devrecs.Length; i++)
                cbbSerialList.Items.Add(devrecs[i].DevName + ", " + devrecs[i].Serial);
            cbbSerialList.SelectedIndex = 0;
        
            
            updateControls();
        }

        /* установка частот сбора данных из элементов управления и обновление их реально
         * установленными значениями */
        lpcie.Errs setAdcFreq()
        {
            lpcie.Errs err = lpcie.Errs.OK;
            double f_acq, f_lch, f_din;

            f_acq = Convert.ToDouble(edtAdcFreq.Text);
            f_lch = Convert.ToDouble(edtAdcFreqLch.Text);
            f_din = Convert.ToDouble(edtDinFreq.Text);

            // устанавливаем требуемую частоту сбора.
            err = hnd.SetAdcFreq(ref f_acq, ref f_lch);
            if (err == lpcie.Errs.OK)
            {
                // обновляем значение индикатора, для отображения
                // реально установившейся частоты
                edtAdcFreq.Text = f_acq.ToString();
                edtAdcFreqLch.Text = f_lch.ToString();
                // Устанавливаем частоту синхронного сбора
                err = hnd.SetDinFreq(ref f_din);
                if (err == lpcie.Errs.OK)
                    edtDinFreq.Text = f_din.ToString();
            }

            return err;
        }

        /* настройка параметров модуля значениями из элементов управления */
        lpcie.Errs setupParams()
        {
            lpcie.Errs err = lpcie.Errs.OK;
            /* таблица соответствия индексов в ComboBox и кодов режима измерения */
            X502.LchMode[] f_mode_tbl = {X502.LchMode.COMM, X502.LchMode.DIFF, X502.LchMode.ZERO};
             /* таблица соответствия индексов в ComboBox и кодов диапазонов АЦП */
            X502.AdcRange[] f_range_tbl = {X502.AdcRange.RANGE_10, X502.AdcRange.RANGE_5, X502.AdcRange.RANGE_2,
									    X502.AdcRange.RANGE_1, X502.AdcRange.RANGE_05, X502.AdcRange.RANGE_02};
	        /* таблица соответствия индексов в ComboBox и кодов источника синхронизации */
	        X502.Sync[] f_sync_tbl = {X502.Sync.INTERNAL, X502.Sync.EXTERNAL_MASTER,									
									 X502.Sync.DI_SYN1_RISE, X502.Sync.DI_SYN2_RISE,
                                     X502.Sync.DI_SYN1_FALL, X502.Sync.DI_SYN2_FALL};
            
            UInt32 lch_cnt = Convert.ToUInt32(cbbLChCnt.Text);

            /* Устанавливаем кол-во логических каналов */
            hnd.LChannelCount = lch_cnt;

            /* Настраниваем таблицу логических каналов */
            err = hnd.SetLChannel(0, Convert.ToUInt32(cbbLCh1_Channel.Text) - 1,
                    f_mode_tbl[cbbLCh1_Mode.SelectedIndex], f_range_tbl[cbbLCh1_Range.SelectedIndex], 0);
            if ((err == lpcie.Errs.OK) && (lch_cnt >= 2))
            {
                err = hnd.SetLChannel(1, Convert.ToUInt32(cbbLCh2_Channel.Text) - 1,
                    f_mode_tbl[cbbLCh2_Mode.SelectedIndex], f_range_tbl[cbbLCh2_Range.SelectedIndex], 0);
            }
            if ((err == lpcie.Errs.OK) && (lch_cnt >= 3))
            {
                err = hnd.SetLChannel(2, Convert.ToUInt32(cbbLCh3_Channel.Text) - 1,
                    f_mode_tbl[cbbLCh3_Mode.SelectedIndex], f_range_tbl[cbbLCh3_Range.SelectedIndex], 0);
            }

            /* Настраиваем источник частоты синхронизации и запуска сбора */
            if (err == lpcie.Errs.OK)
            {
                hnd.SyncMode = f_sync_tbl[cbbSyncMode.SelectedIndex];
                hnd.SyncMode = f_sync_tbl[cbbSyncStartMode.SelectedIndex];
            }

            /* настраиваем частоту сбора с АЦП */
            if (err == lpcie.Errs.OK)
                err = setAdcFreq();

            /* Записываем настройки в модуль */
            if (err == lpcie.Errs.OK)
                err = hnd.Configure(0);

            return err;
        }

        lpcie.Errs setSyncDinStream()
        {
            lpcie.Errs err;
            /* разрешаем или запрещаем поток синхронного ввода
               в зависимости от состояния переключателя */
            if (chkSyncDin.Checked)
                err = hnd.StreamsEnable(X502.Streams.DIN);
            else
                err = hnd.StreamsDisable(X502.Streams.DIN);
            return err;
        }



        public MainForm()
        {
            InitializeComponent();

            cbbLChCnt.SelectedIndex = 2;
            cbbLCh1_Channel.SelectedIndex = 0;
            cbbLCh2_Channel.SelectedIndex = 1;
            cbbLCh3_Channel.SelectedIndex = 2;

            cbbLCh1_Range.SelectedIndex = 0;
            cbbLCh2_Range.SelectedIndex = 0;
            cbbLCh3_Range.SelectedIndex = 0;

            cbbLCh1_Mode.SelectedIndex = 1;
            cbbLCh2_Mode.SelectedIndex = 1;
            cbbLCh3_Mode.SelectedIndex = 1;

            cbbSyncMode.SelectedIndex = 0;
            cbbSyncStartMode.SelectedIndex = 0;

            rcv_buf = new UInt32[RECV_BUF_SIZE];
            dinData = new UInt32[RECV_BUF_SIZE];
            adcData = new double[RECV_BUF_SIZE];

            
            threadRunning = false;


            refreshDevList();
        }

        private void btnRefreshDeviceList_Click(object sender, EventArgs e)
        {
            refreshDevList();            
        }

        private void showDevInfo()
        {
            /* получаем информацию о модуле */
            X502.Info devinfo = hnd.DevInfo;
            /* отображаем ее на панели */
            chkBfPresent.Checked = (devinfo.DevFlags & X502.DevFlags.BF_PRESENT) != 0;
            chkDacPresent.Checked = (devinfo.DevFlags & X502.DevFlags.DAC_PRESENT) != 0;
            chkGalPresent.Checked = (devinfo.DevFlags & X502.DevFlags.GAL_PRESENT) != 0;
            chkEthSupport.Checked = (devinfo.DevFlags & X502.DevFlags.IFACE_SUPPORT_ETH) != 0;

            edtFpgaVer.Text = devinfo.FpgaVerString;
            edtPldaVer.Text = string.Format("{0}", devinfo.PldaVer);
            edtMcuVer.Text = devinfo.McuFirmwareVerString;
        }
        

        private void btnOpen_Click(object sender, EventArgs e)
        {
            if (hnd == null)
            {
                lpcie.Errs res;              
                
                int idx = cbbSerialList.SelectedIndex;
                if (idx >= 0)
                {
                    /* создаем описатель модуля */
                    hnd = X502.Create(devrecs[idx].DevName);
                    /* устанавливаем связь по выбранному серийному номеру */
                    res = hnd.Open(devrecs[idx]);
                    if (res == lpcie.Errs.OK)
                    {
                        showDevInfo();   
                    }
                    else
                    {
                        MessageBox.Show(X502.GetErrorString(res), "Ошибка открытия модуля", MessageBoxButtons.OK,
                                    MessageBoxIcon.Error);
                        hnd = null;
                    }
                }
            }
            else
            {
                deviceClose();
            }

            updateControls();
        }

        private void btnOpenByIP_Click(object sender, EventArgs e)
        {
            /* создаем запись, соответствующую заданному адресу */
            X502.DevRec rec = E502.MakeDevRecordByIpAddr(IPAddress.Parse(edtIpAddr.Text), 0, 5000);
            if (rec != null)
            {
                /* создание объекта */
                hnd = X502.Create(rec.DevName);
                /* устанавливаем связь по выбранному серийному номеру */
                lpcie.Errs res = hnd.Open(rec);
                if (res == lpcie.Errs.OK)
                {
                    showDevInfo();
                }
                else
                {
                    MessageBox.Show(X502.GetErrorString(res), "Ошибка открытия модуля", MessageBoxButtons.OK,
                                                        MessageBoxIcon.Error);
                    hnd = null;
                }
            }
            updateControls();

        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            deviceClose();
        }

        private void btnSetAdcFreq_Click(object sender, EventArgs e)
        {
            if (hnd != null)
            {
                hnd.LChannelCount = Convert.ToUInt32(cbbLChCnt.Text);

                lpcie.Errs err = setAdcFreq();
                if (err != lpcie.Errs.OK)
                {
                    MessageBox.Show(X502.GetErrorString(err), "Ошибка установки частоты сбора",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void btnAsyncAdcFrame_Click(object sender, EventArgs e)
        {
            if (hnd != null)
            {
                /* устанавливаем параметры модуля */
                lpcie.Errs err = setupParams();

                if (err != lpcie.Errs.OK)
                {
                    MessageBox.Show(X502.GetErrorString(err), "Ошибка настройки параметров АЦП",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

                if (err == lpcie.Errs.OK)
                {
                    UInt32 lch_cnt = hnd.LChannelCount;
                    /* Создаем массив для приема количества отсчетов, равному количеству
                     * логических каналов */
                    double[] adc_data = new double[lch_cnt];
                    err = hnd.AsyncGetAdcFrame(X502.ProcFlags.VOLT, 1000, adc_data);
                    if (err != lpcie.Errs.OK)
                    {
                        MessageBox.Show(X502.GetErrorString(err), "Ошибка приема кадра АЦП",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    else
                    {
                        /* выводим результат */
                        edtLCh1_Result.Text = adc_data[0].ToString("F7");
                        if (lch_cnt >= 2)
                            edtLCh2_Result.Text = adc_data[1].ToString("F7");
                        else
                            edtLCh2_Result.Text = "";

                        if (lch_cnt >= 3)
                            edtLCh3_Result.Text = adc_data[2].ToString("F7");
                        else
                            edtLCh3_Result.Text = "";

                    }
                }

            }

        }

        private void btnAsyncDigIn_Click(object sender, EventArgs e)
        {
            if (hnd != null)
            {
                UInt32 din;
                lpcie.Errs err = hnd.AsyncInDig(out din);
                if (err != lpcie.Errs.OK)
                {
                    MessageBox.Show(X502.GetErrorString(err), "Ошибка асинхронного ввода с цифровых линий",
                                MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    edtAsyncDigIn.Text = din.ToString("X5");
                }
            }
        }

        private void btnAsyncDigOut_Click(object sender, EventArgs e)
        {
            if (hnd != null)
            {
                UInt32 val = Convert.ToUInt32(edtAsyncDigOut.Text, 16);
                lpcie.Errs err = hnd.AsyncOutDig(val, 0);
                if (err != lpcie.Errs.OK)
                {
                    MessageBox.Show(X502.GetErrorString(err), "Ошибка асинхронного вывода на цифровые линии",
                               MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void btnAsyncDac1_Click(object sender, EventArgs e)
        {
            if (hnd != null)
            {
                double val = Convert.ToDouble(edtAsyncDac1.Text); ;
                lpcie.Errs err = hnd.AsyncOutDac(X502.DacCh.CH1, val, X502.DacOutFlags.CALIBR |
                                                                X502.DacOutFlags.VOLT);
                if (err != lpcie.Errs.OK)
                {
                    MessageBox.Show(X502.GetErrorString(err), "Ошибка вывода на ЦАП",
                               MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

        }

        private void btnAsyncDac2_Click(object sender, EventArgs e)
        {
            if (hnd != null)
            {
                double val = Convert.ToDouble(edtAsyncDac2.Text); ;
                lpcie.Errs err = hnd.AsyncOutDac(X502.DacCh.CH2, val, X502.DacOutFlags.CALIBR |
                                                                X502.DacOutFlags.VOLT);
                if (err != lpcie.Errs.OK)
                {
                    MessageBox.Show(X502.GetErrorString(err), "Ошибка вывода на ЦАП",
                               MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            /* настраиваем все параметры в соответствии с элементами управления */
            lpcie.Errs err = setupParams();
           

            /* разрешаем синхронный ввод АЦП */
            if (err == lpcie.Errs.OK)
                err = hnd.StreamsEnable(X502.Streams.ADC);
            /* разрешаем синхронный ввод с цифровых линий в зависимости от переключателя */
            if (err == lpcie.Errs.OK)
                err = setSyncDinStream();

            if (err != lpcie.Errs.OK)
            {
                MessageBox.Show(X502.GetErrorString(err), "Ошибка настройки параметров модуля",
                               MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                /* создаем новый поток для сбора в нем данных */ 
                thread = new Thread(this.threadFunc);
                threadRunning = true;
                thread.Start();
                updateControls();
            }
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            if (threadRunning)
                reqStop = true;
            btnStop.Enabled = false;
        }


    }
}
