namespace x502_example
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnRefreshDeviceList = new System.Windows.Forms.Button();
            this.cbbSerialList = new System.Windows.Forms.ComboBox();
            this.btnOpen = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.edtPldaVer = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.edtFpgaVer = new System.Windows.Forms.TextBox();
            this.chkBfPresent = new System.Windows.Forms.CheckBox();
            this.chkGalPresent = new System.Windows.Forms.CheckBox();
            this.chkDacPresent = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnSetAdcFreq = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.edtDinFreq = new System.Windows.Forms.TextBox();
            this.edtAdcFreqLch = new System.Windows.Forms.TextBox();
            this.edtAdcFreq = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.chkSyncDin = new System.Windows.Forms.CheckBox();
            this.edtDin_Result = new System.Windows.Forms.TextBox();
            this.edtLCh3_Result = new System.Windows.Forms.TextBox();
            this.edtLCh2_Result = new System.Windows.Forms.TextBox();
            this.cbbLCh3_Mode = new System.Windows.Forms.ComboBox();
            this.label10 = new System.Windows.Forms.Label();
            this.cbbLCh3_Range = new System.Windows.Forms.ComboBox();
            this.cbbLCh2_Mode = new System.Windows.Forms.ComboBox();
            this.cbbLCh3_Channel = new System.Windows.Forms.ComboBox();
            this.label9 = new System.Windows.Forms.Label();
            this.cbbLCh2_Range = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.cbbLCh2_Channel = new System.Windows.Forms.ComboBox();
            this.edtLCh1_Result = new System.Windows.Forms.TextBox();
            this.cbbLCh1_Mode = new System.Windows.Forms.ComboBox();
            this.cbbLCh1_Range = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.cbbLCh1_Channel = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.cbbLChCnt = new System.Windows.Forms.ComboBox();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnAsyncAdcFrame = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.cbbSyncStartMode = new System.Windows.Forms.ComboBox();
            this.label12 = new System.Windows.Forms.Label();
            this.cbbSyncMode = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.btnAsyncDigIn = new System.Windows.Forms.Button();
            this.btnAsyncDigOut = new System.Windows.Forms.Button();
            this.btnAsyncDac1 = new System.Windows.Forms.Button();
            this.btnAsyncDac2 = new System.Windows.Forms.Button();
            this.edtAsyncDigIn = new System.Windows.Forms.TextBox();
            this.edtAsyncDigOut = new System.Windows.Forms.TextBox();
            this.edtAsyncDac1 = new System.Windows.Forms.TextBox();
            this.edtAsyncDac2 = new System.Windows.Forms.TextBox();
            this.chkEthSupport = new System.Windows.Forms.CheckBox();
            this.label13 = new System.Windows.Forms.Label();
            this.edtMcuVer = new System.Windows.Forms.TextBox();
            this.edtIpAddr = new System.Windows.Forms.TextBox();
            this.btnOpenByIP = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnRefreshDeviceList
            // 
            this.btnRefreshDeviceList.Location = new System.Drawing.Point(27, 12);
            this.btnRefreshDeviceList.Name = "btnRefreshDeviceList";
            this.btnRefreshDeviceList.Size = new System.Drawing.Size(212, 23);
            this.btnRefreshDeviceList.TabIndex = 0;
            this.btnRefreshDeviceList.Text = "Обновить список модулей";
            this.btnRefreshDeviceList.UseVisualStyleBackColor = true;
            this.btnRefreshDeviceList.Click += new System.EventHandler(this.btnRefreshDeviceList_Click);
            // 
            // cbbSerialList
            // 
            this.cbbSerialList.FormattingEnabled = true;
            this.cbbSerialList.Location = new System.Drawing.Point(258, 14);
            this.cbbSerialList.Name = "cbbSerialList";
            this.cbbSerialList.Size = new System.Drawing.Size(139, 21);
            this.cbbSerialList.TabIndex = 1;
            // 
            // btnOpen
            // 
            this.btnOpen.Location = new System.Drawing.Point(27, 55);
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Size = new System.Drawing.Size(212, 23);
            this.btnOpen.TabIndex = 2;
            this.btnOpen.Text = "Установить связь с модулем";
            this.btnOpen.UseVisualStyleBackColor = true;
            this.btnOpen.Click += new System.EventHandler(this.btnOpen_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.edtMcuVer);
            this.groupBox1.Controls.Add(this.chkEthSupport);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.edtPldaVer);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.edtFpgaVer);
            this.groupBox1.Controls.Add(this.chkBfPresent);
            this.groupBox1.Controls.Add(this.chkGalPresent);
            this.groupBox1.Controls.Add(this.chkDacPresent);
            this.groupBox1.Location = new System.Drawing.Point(258, 40);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(230, 183);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Информация о модуле";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(74, 131);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(128, 13);
            this.label2.TabIndex = 8;
            this.label2.Text = "Версия прошивки PLDA";
            // 
            // edtPldaVer
            // 
            this.edtPldaVer.Enabled = false;
            this.edtPldaVer.Location = new System.Drawing.Point(6, 128);
            this.edtPldaVer.Name = "edtPldaVer";
            this.edtPldaVer.Size = new System.Drawing.Size(62, 20);
            this.edtPldaVer.TabIndex = 7;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(74, 105);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(131, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Версия прошивки ПЛИС";
            // 
            // edtFpgaVer
            // 
            this.edtFpgaVer.Enabled = false;
            this.edtFpgaVer.Location = new System.Drawing.Point(6, 102);
            this.edtFpgaVer.Name = "edtFpgaVer";
            this.edtFpgaVer.Size = new System.Drawing.Size(62, 20);
            this.edtFpgaVer.TabIndex = 5;
            // 
            // chkBfPresent
            // 
            this.chkBfPresent.AutoSize = true;
            this.chkBfPresent.Enabled = false;
            this.chkBfPresent.Location = new System.Drawing.Point(6, 65);
            this.chkBfPresent.Name = "chkBfPresent";
            this.chkBfPresent.Size = new System.Drawing.Size(199, 17);
            this.chkBfPresent.TabIndex = 4;
            this.chkBfPresent.Text = "Наличие сигнального процессора";
            this.chkBfPresent.UseVisualStyleBackColor = true;
            // 
            // chkGalPresent
            // 
            this.chkGalPresent.AutoSize = true;
            this.chkGalPresent.Enabled = false;
            this.chkGalPresent.Location = new System.Drawing.Point(6, 42);
            this.chkGalPresent.Name = "chkGalPresent";
            this.chkGalPresent.Size = new System.Drawing.Size(167, 17);
            this.chkGalPresent.TabIndex = 1;
            this.chkGalPresent.Text = "Наличие гальваноразвязки";
            this.chkGalPresent.UseVisualStyleBackColor = true;
            // 
            // chkDacPresent
            // 
            this.chkDacPresent.AutoSize = true;
            this.chkDacPresent.Enabled = false;
            this.chkDacPresent.Location = new System.Drawing.Point(6, 19);
            this.chkDacPresent.Name = "chkDacPresent";
            this.chkDacPresent.Size = new System.Drawing.Size(95, 17);
            this.chkDacPresent.TabIndex = 0;
            this.chkDacPresent.Text = "Наличие ЦАП";
            this.chkDacPresent.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btnSetAdcFreq);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.edtDinFreq);
            this.groupBox2.Controls.Add(this.edtAdcFreqLch);
            this.groupBox2.Controls.Add(this.edtAdcFreq);
            this.groupBox2.Location = new System.Drawing.Point(27, 247);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(518, 60);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Частоты синхронного сбора";
            // 
            // btnSetAdcFreq
            // 
            this.btnSetAdcFreq.Location = new System.Drawing.Point(437, 32);
            this.btnSetAdcFreq.Name = "btnSetAdcFreq";
            this.btnSetAdcFreq.Size = new System.Drawing.Size(75, 23);
            this.btnSetAdcFreq.TabIndex = 10;
            this.btnSetAdcFreq.Text = "Установить";
            this.btnSetAdcFreq.UseVisualStyleBackColor = true;
            this.btnSetAdcFreq.Click += new System.EventHandler(this.btnSetAdcFreq_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(291, 16);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(170, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Частота синхронного ввода (Гц)";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(159, 15);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(118, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Частота на канал (Гц)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 15);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(143, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "Частота сбора данных (Гц)";
            // 
            // edtDinFreq
            // 
            this.edtDinFreq.Location = new System.Drawing.Point(294, 34);
            this.edtDinFreq.Name = "edtDinFreq";
            this.edtDinFreq.Size = new System.Drawing.Size(126, 20);
            this.edtDinFreq.TabIndex = 6;
            this.edtDinFreq.Text = "2000000";
            // 
            // edtAdcFreqLch
            // 
            this.edtAdcFreqLch.Location = new System.Drawing.Point(162, 34);
            this.edtAdcFreqLch.Name = "edtAdcFreqLch";
            this.edtAdcFreqLch.Size = new System.Drawing.Size(126, 20);
            this.edtAdcFreqLch.TabIndex = 5;
            this.edtAdcFreqLch.Text = "2000000";
            // 
            // edtAdcFreq
            // 
            this.edtAdcFreq.Location = new System.Drawing.Point(6, 34);
            this.edtAdcFreq.Name = "edtAdcFreq";
            this.edtAdcFreq.Size = new System.Drawing.Size(126, 20);
            this.edtAdcFreq.TabIndex = 0;
            this.edtAdcFreq.Text = "2000000";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.chkSyncDin);
            this.groupBox3.Controls.Add(this.edtDin_Result);
            this.groupBox3.Controls.Add(this.edtLCh3_Result);
            this.groupBox3.Controls.Add(this.edtLCh2_Result);
            this.groupBox3.Controls.Add(this.cbbLCh3_Mode);
            this.groupBox3.Controls.Add(this.label10);
            this.groupBox3.Controls.Add(this.cbbLCh3_Range);
            this.groupBox3.Controls.Add(this.cbbLCh2_Mode);
            this.groupBox3.Controls.Add(this.cbbLCh3_Channel);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.cbbLCh2_Range);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.cbbLCh2_Channel);
            this.groupBox3.Controls.Add(this.edtLCh1_Result);
            this.groupBox3.Controls.Add(this.cbbLCh1_Mode);
            this.groupBox3.Controls.Add(this.cbbLCh1_Range);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.cbbLCh1_Channel);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.cbbLChCnt);
            this.groupBox3.Location = new System.Drawing.Point(27, 377);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(518, 166);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Каналы АЦП";
            // 
            // chkSyncDin
            // 
            this.chkSyncDin.AutoSize = true;
            this.chkSyncDin.Location = new System.Drawing.Point(182, 140);
            this.chkSyncDin.Name = "chkSyncDin";
            this.chkSyncDin.Size = new System.Drawing.Size(189, 17);
            this.chkSyncDin.TabIndex = 17;
            this.chkSyncDin.Text = "Разрешение синхронного ввода";
            this.chkSyncDin.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.chkSyncDin.UseVisualStyleBackColor = true;
            // 
            // edtDin_Result
            // 
            this.edtDin_Result.Location = new System.Drawing.Point(380, 138);
            this.edtDin_Result.Name = "edtDin_Result";
            this.edtDin_Result.Size = new System.Drawing.Size(107, 20);
            this.edtDin_Result.TabIndex = 16;
            // 
            // edtLCh3_Result
            // 
            this.edtLCh3_Result.Location = new System.Drawing.Point(380, 112);
            this.edtLCh3_Result.Name = "edtLCh3_Result";
            this.edtLCh3_Result.Size = new System.Drawing.Size(107, 20);
            this.edtLCh3_Result.TabIndex = 15;
            // 
            // edtLCh2_Result
            // 
            this.edtLCh2_Result.Location = new System.Drawing.Point(380, 85);
            this.edtLCh2_Result.Name = "edtLCh2_Result";
            this.edtLCh2_Result.Size = new System.Drawing.Size(107, 20);
            this.edtLCh2_Result.TabIndex = 11;
            // 
            // cbbLCh3_Mode
            // 
            this.cbbLCh3_Mode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh3_Mode.FormattingEnabled = true;
            this.cbbLCh3_Mode.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh3_Mode.Items.AddRange(new object[] {
            "С общей землей",
            "Дифференциальный",
            "Измерение нуля"});
            this.cbbLCh3_Mode.Location = new System.Drawing.Point(182, 111);
            this.cbbLCh3_Mode.Name = "cbbLCh3_Mode";
            this.cbbLCh3_Mode.Size = new System.Drawing.Size(175, 21);
            this.cbbLCh3_Mode.TabIndex = 14;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(386, 41);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(59, 13);
            this.label10.TabIndex = 10;
            this.label10.Text = "Результат";
            // 
            // cbbLCh3_Range
            // 
            this.cbbLCh3_Range.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh3_Range.FormattingEnabled = true;
            this.cbbLCh3_Range.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh3_Range.Items.AddRange(new object[] {
            "10 В",
            "5 В",
            "2 В",
            "1 В",
            "0.5 В",
            "0.2 В"});
            this.cbbLCh3_Range.Location = new System.Drawing.Point(73, 111);
            this.cbbLCh3_Range.Name = "cbbLCh3_Range";
            this.cbbLCh3_Range.Size = new System.Drawing.Size(75, 21);
            this.cbbLCh3_Range.TabIndex = 13;
            // 
            // cbbLCh2_Mode
            // 
            this.cbbLCh2_Mode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh2_Mode.FormattingEnabled = true;
            this.cbbLCh2_Mode.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh2_Mode.Items.AddRange(new object[] {
            "С общей землей",
            "Дифференциальный",
            "Измерение нуля"});
            this.cbbLCh2_Mode.Location = new System.Drawing.Point(182, 84);
            this.cbbLCh2_Mode.Name = "cbbLCh2_Mode";
            this.cbbLCh2_Mode.Size = new System.Drawing.Size(175, 21);
            this.cbbLCh2_Mode.TabIndex = 10;
            // 
            // cbbLCh3_Channel
            // 
            this.cbbLCh3_Channel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh3_Channel.FormattingEnabled = true;
            this.cbbLCh3_Channel.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh3_Channel.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "17",
            "18",
            "19",
            "20",
            "21",
            "22",
            "23",
            "24",
            "25",
            "26",
            "27",
            "28",
            "29",
            "30",
            "31",
            "32"});
            this.cbbLCh3_Channel.Location = new System.Drawing.Point(8, 111);
            this.cbbLCh3_Channel.Name = "cbbLCh3_Channel";
            this.cbbLCh3_Channel.Size = new System.Drawing.Size(46, 21);
            this.cbbLCh3_Channel.TabIndex = 12;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(185, 41);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(101, 13);
            this.label9.TabIndex = 9;
            this.label9.Text = "Режим измерения";
            // 
            // cbbLCh2_Range
            // 
            this.cbbLCh2_Range.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh2_Range.FormattingEnabled = true;
            this.cbbLCh2_Range.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh2_Range.Items.AddRange(new object[] {
            "10 В",
            "5 В",
            "2 В",
            "1 В",
            "0.5 В",
            "0.2 В"});
            this.cbbLCh2_Range.Location = new System.Drawing.Point(73, 84);
            this.cbbLCh2_Range.Name = "cbbLCh2_Range";
            this.cbbLCh2_Range.Size = new System.Drawing.Size(75, 21);
            this.cbbLCh2_Range.TabIndex = 9;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(72, 41);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(58, 13);
            this.label8.TabIndex = 8;
            this.label8.Text = "Диапазон";
            // 
            // cbbLCh2_Channel
            // 
            this.cbbLCh2_Channel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh2_Channel.FormattingEnabled = true;
            this.cbbLCh2_Channel.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh2_Channel.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "17",
            "18",
            "19",
            "20",
            "21",
            "22",
            "23",
            "24",
            "25",
            "26",
            "27",
            "28",
            "29",
            "30",
            "31",
            "32"});
            this.cbbLCh2_Channel.Location = new System.Drawing.Point(8, 84);
            this.cbbLCh2_Channel.Name = "cbbLCh2_Channel";
            this.cbbLCh2_Channel.Size = new System.Drawing.Size(46, 21);
            this.cbbLCh2_Channel.TabIndex = 8;
            // 
            // edtLCh1_Result
            // 
            this.edtLCh1_Result.Location = new System.Drawing.Point(380, 58);
            this.edtLCh1_Result.Name = "edtLCh1_Result";
            this.edtLCh1_Result.Size = new System.Drawing.Size(107, 20);
            this.edtLCh1_Result.TabIndex = 7;
            // 
            // cbbLCh1_Mode
            // 
            this.cbbLCh1_Mode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh1_Mode.FormattingEnabled = true;
            this.cbbLCh1_Mode.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh1_Mode.Items.AddRange(new object[] {
            "С общей землей",
            "Дифференциальный",
            "Измерение нуля"});
            this.cbbLCh1_Mode.Location = new System.Drawing.Point(182, 57);
            this.cbbLCh1_Mode.Name = "cbbLCh1_Mode";
            this.cbbLCh1_Mode.Size = new System.Drawing.Size(175, 21);
            this.cbbLCh1_Mode.TabIndex = 6;
            // 
            // cbbLCh1_Range
            // 
            this.cbbLCh1_Range.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh1_Range.FormattingEnabled = true;
            this.cbbLCh1_Range.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh1_Range.Items.AddRange(new object[] {
            "10 В",
            "5 В",
            "2 В",
            "1 В",
            "0.5 В",
            "0.2 В"});
            this.cbbLCh1_Range.Location = new System.Drawing.Point(73, 57);
            this.cbbLCh1_Range.Name = "cbbLCh1_Range";
            this.cbbLCh1_Range.Size = new System.Drawing.Size(75, 21);
            this.cbbLCh1_Range.TabIndex = 5;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 41);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(38, 13);
            this.label7.TabIndex = 4;
            this.label7.Text = "Канал";
            // 
            // cbbLCh1_Channel
            // 
            this.cbbLCh1_Channel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLCh1_Channel.FormattingEnabled = true;
            this.cbbLCh1_Channel.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbLCh1_Channel.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "17",
            "18",
            "19",
            "20",
            "21",
            "22",
            "23",
            "24",
            "25",
            "26",
            "27",
            "28",
            "29",
            "30",
            "31",
            "32"});
            this.cbbLCh1_Channel.Location = new System.Drawing.Point(8, 57);
            this.cbbLCh1_Channel.Name = "cbbLCh1_Channel";
            this.cbbLCh1_Channel.Size = new System.Drawing.Size(46, 21);
            this.cbbLCh1_Channel.TabIndex = 2;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(25, 22);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(171, 13);
            this.label6.TabIndex = 1;
            this.label6.Text = "Количество логических каналов";
            // 
            // cbbLChCnt
            // 
            this.cbbLChCnt.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbLChCnt.FormattingEnabled = true;
            this.cbbLChCnt.Items.AddRange(new object[] {
            "1",
            "2",
            "3"});
            this.cbbLChCnt.Location = new System.Drawing.Point(211, 14);
            this.cbbLChCnt.Name = "cbbLChCnt";
            this.cbbLChCnt.Size = new System.Drawing.Size(49, 21);
            this.cbbLChCnt.TabIndex = 0;
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(27, 92);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(212, 23);
            this.btnStart.TabIndex = 6;
            this.btnStart.Text = "Запуск сбора данных";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnStop
            // 
            this.btnStop.Location = new System.Drawing.Point(27, 121);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(212, 23);
            this.btnStop.TabIndex = 7;
            this.btnStop.Text = "Останов сбора данных";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // btnAsyncAdcFrame
            // 
            this.btnAsyncAdcFrame.Location = new System.Drawing.Point(27, 549);
            this.btnAsyncAdcFrame.Name = "btnAsyncAdcFrame";
            this.btnAsyncAdcFrame.Size = new System.Drawing.Size(212, 23);
            this.btnAsyncAdcFrame.TabIndex = 8;
            this.btnAsyncAdcFrame.Text = "Асинхронный ввод кадра АЦП";
            this.btnAsyncAdcFrame.UseVisualStyleBackColor = true;
            this.btnAsyncAdcFrame.Click += new System.EventHandler(this.btnAsyncAdcFrame_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.cbbSyncStartMode);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.cbbSyncMode);
            this.groupBox4.Controls.Add(this.label11);
            this.groupBox4.Location = new System.Drawing.Point(27, 307);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(518, 64);
            this.groupBox4.TabIndex = 9;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Синхронизация";
            // 
            // cbbSyncStartMode
            // 
            this.cbbSyncStartMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbSyncStartMode.FormattingEnabled = true;
            this.cbbSyncStartMode.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbSyncStartMode.Items.AddRange(new object[] {
            "Внутренний",
            "От внешнего мастера",
            "Фронт сигнала DI_SYN1",
            "Фронт сигнала DI_SYN2",
            "Спад сигнала DI_SYN1",
            "Спад сигнала DI_SYN2"});
            this.cbbSyncStartMode.Location = new System.Drawing.Point(228, 37);
            this.cbbSyncStartMode.Name = "cbbSyncStartMode";
            this.cbbSyncStartMode.Size = new System.Drawing.Size(176, 21);
            this.cbbSyncStartMode.TabIndex = 11;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(228, 16);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(242, 13);
            this.label12.TabIndex = 10;
            this.label12.Text = "Источник запуска синхронного ввода/вывода";
            // 
            // cbbSyncMode
            // 
            this.cbbSyncMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbbSyncMode.FormattingEnabled = true;
            this.cbbSyncMode.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.cbbSyncMode.Items.AddRange(new object[] {
            "Внутренний",
            "От внешнего мастера",
            "Фронт сигнала DI_SYN1",
            "Фронт сигнала DI_SYN2",
            "Спад сигнала DI_SYN1",
            "Спад сигнала DI_SYN2"});
            this.cbbSyncMode.Location = new System.Drawing.Point(10, 37);
            this.cbbSyncMode.Name = "cbbSyncMode";
            this.cbbSyncMode.Size = new System.Drawing.Size(176, 21);
            this.cbbSyncMode.TabIndex = 9;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(7, 16);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(179, 13);
            this.label11.TabIndex = 8;
            this.label11.Text = "Источник частоты синхронизации";
            // 
            // btnAsyncDigIn
            // 
            this.btnAsyncDigIn.Location = new System.Drawing.Point(27, 578);
            this.btnAsyncDigIn.Name = "btnAsyncDigIn";
            this.btnAsyncDigIn.Size = new System.Drawing.Size(212, 23);
            this.btnAsyncDigIn.TabIndex = 10;
            this.btnAsyncDigIn.Text = "Асинхронный ввод цифровых линий";
            this.btnAsyncDigIn.UseVisualStyleBackColor = true;
            this.btnAsyncDigIn.Click += new System.EventHandler(this.btnAsyncDigIn_Click);
            // 
            // btnAsyncDigOut
            // 
            this.btnAsyncDigOut.Location = new System.Drawing.Point(27, 607);
            this.btnAsyncDigOut.Name = "btnAsyncDigOut";
            this.btnAsyncDigOut.Size = new System.Drawing.Size(212, 23);
            this.btnAsyncDigOut.TabIndex = 11;
            this.btnAsyncDigOut.Text = "Асинхронный вывод цифровых линий";
            this.btnAsyncDigOut.UseVisualStyleBackColor = true;
            this.btnAsyncDigOut.Click += new System.EventHandler(this.btnAsyncDigOut_Click);
            // 
            // btnAsyncDac1
            // 
            this.btnAsyncDac1.Location = new System.Drawing.Point(27, 636);
            this.btnAsyncDac1.Name = "btnAsyncDac1";
            this.btnAsyncDac1.Size = new System.Drawing.Size(212, 23);
            this.btnAsyncDac1.TabIndex = 12;
            this.btnAsyncDac1.Text = "Асинхронный вывод на ЦАП1 (Вольты)";
            this.btnAsyncDac1.UseVisualStyleBackColor = true;
            this.btnAsyncDac1.Click += new System.EventHandler(this.btnAsyncDac1_Click);
            // 
            // btnAsyncDac2
            // 
            this.btnAsyncDac2.Location = new System.Drawing.Point(27, 665);
            this.btnAsyncDac2.Name = "btnAsyncDac2";
            this.btnAsyncDac2.Size = new System.Drawing.Size(212, 23);
            this.btnAsyncDac2.TabIndex = 13;
            this.btnAsyncDac2.Text = "Асинхронный вывод на ЦАП2 (Вольты)";
            this.btnAsyncDac2.UseVisualStyleBackColor = true;
            this.btnAsyncDac2.Click += new System.EventHandler(this.btnAsyncDac2_Click);
            // 
            // edtAsyncDigIn
            // 
            this.edtAsyncDigIn.Location = new System.Drawing.Point(255, 580);
            this.edtAsyncDigIn.Name = "edtAsyncDigIn";
            this.edtAsyncDigIn.Size = new System.Drawing.Size(101, 20);
            this.edtAsyncDigIn.TabIndex = 14;
            // 
            // edtAsyncDigOut
            // 
            this.edtAsyncDigOut.Location = new System.Drawing.Point(255, 610);
            this.edtAsyncDigOut.Name = "edtAsyncDigOut";
            this.edtAsyncDigOut.Size = new System.Drawing.Size(101, 20);
            this.edtAsyncDigOut.TabIndex = 15;
            this.edtAsyncDigOut.Text = "0x0000";
            // 
            // edtAsyncDac1
            // 
            this.edtAsyncDac1.Location = new System.Drawing.Point(255, 639);
            this.edtAsyncDac1.Name = "edtAsyncDac1";
            this.edtAsyncDac1.Size = new System.Drawing.Size(58, 20);
            this.edtAsyncDac1.TabIndex = 16;
            this.edtAsyncDac1.Text = "5";
            // 
            // edtAsyncDac2
            // 
            this.edtAsyncDac2.Location = new System.Drawing.Point(255, 665);
            this.edtAsyncDac2.Name = "edtAsyncDac2";
            this.edtAsyncDac2.Size = new System.Drawing.Size(58, 20);
            this.edtAsyncDac2.TabIndex = 17;
            this.edtAsyncDac2.Text = "5";
            // 
            // chkEthSupport
            // 
            this.chkEthSupport.AutoSize = true;
            this.chkEthSupport.Enabled = false;
            this.chkEthSupport.Location = new System.Drawing.Point(6, 85);
            this.chkEthSupport.Name = "chkEthSupport";
            this.chkEthSupport.Size = new System.Drawing.Size(182, 17);
            this.chkEthSupport.TabIndex = 9;
            this.chkEthSupport.Text = "Наличие сетевого интерфейса";
            this.chkEthSupport.UseVisualStyleBackColor = true;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(74, 157);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(124, 13);
            this.label13.TabIndex = 11;
            this.label13.Text = "Версия прошивки ARM";
            // 
            // edtMcuVer
            // 
            this.edtMcuVer.Enabled = false;
            this.edtMcuVer.Location = new System.Drawing.Point(6, 154);
            this.edtMcuVer.Name = "edtMcuVer";
            this.edtMcuVer.Size = new System.Drawing.Size(62, 20);
            this.edtMcuVer.TabIndex = 10;
            // 
            // edtIpAddr
            // 
            this.edtIpAddr.Location = new System.Drawing.Point(27, 203);
            this.edtIpAddr.Name = "edtIpAddr";
            this.edtIpAddr.Size = new System.Drawing.Size(212, 20);
            this.edtIpAddr.TabIndex = 18;
            this.edtIpAddr.Text = "192.168.0.1";
            // 
            // btnOpenByIP
            // 
            this.btnOpenByIP.Location = new System.Drawing.Point(27, 174);
            this.btnOpenByIP.Name = "btnOpenByIP";
            this.btnOpenByIP.Size = new System.Drawing.Size(212, 23);
            this.btnOpenByIP.TabIndex = 19;
            this.btnOpenByIP.Text = "Установить соединение по IP-адресу";
            this.btnOpenByIP.UseVisualStyleBackColor = true;
            this.btnOpenByIP.Click += new System.EventHandler(this.btnOpenByIP_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(553, 736);
            this.Controls.Add(this.btnOpenByIP);
            this.Controls.Add(this.edtIpAddr);
            this.Controls.Add(this.edtAsyncDac2);
            this.Controls.Add(this.edtAsyncDac1);
            this.Controls.Add(this.edtAsyncDigOut);
            this.Controls.Add(this.edtAsyncDigIn);
            this.Controls.Add(this.btnAsyncDac2);
            this.Controls.Add(this.btnAsyncDac1);
            this.Controls.Add(this.btnAsyncDigOut);
            this.Controls.Add(this.btnAsyncDigIn);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.btnAsyncAdcFrame);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnOpen);
            this.Controls.Add(this.cbbSerialList);
            this.Controls.Add(this.btnRefreshDeviceList);
            this.Name = "MainForm";
            this.Text = "L502/E502 example";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRefreshDeviceList;
        private System.Windows.Forms.ComboBox cbbSerialList;
        private System.Windows.Forms.Button btnOpen;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox chkBfPresent;
        private System.Windows.Forms.CheckBox chkGalPresent;
        private System.Windows.Forms.CheckBox chkDacPresent;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox edtPldaVer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox edtFpgaVer;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnSetAdcFreq;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox edtDinFreq;
        private System.Windows.Forms.TextBox edtAdcFreqLch;
        private System.Windows.Forms.TextBox edtAdcFreq;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ComboBox cbbLChCnt;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbbLCh1_Channel;
        private System.Windows.Forms.ComboBox cbbLCh1_Mode;
        private System.Windows.Forms.ComboBox cbbLCh1_Range;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.CheckBox chkSyncDin;
        private System.Windows.Forms.TextBox edtDin_Result;
        private System.Windows.Forms.TextBox edtLCh3_Result;
        private System.Windows.Forms.TextBox edtLCh2_Result;
        private System.Windows.Forms.ComboBox cbbLCh3_Mode;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox cbbLCh3_Range;
        private System.Windows.Forms.ComboBox cbbLCh2_Mode;
        private System.Windows.Forms.ComboBox cbbLCh3_Channel;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ComboBox cbbLCh2_Range;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cbbLCh2_Channel;
        private System.Windows.Forms.TextBox edtLCh1_Result;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Button btnAsyncAdcFrame;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.ComboBox cbbSyncStartMode;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox cbbSyncMode;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Button btnAsyncDigIn;
        private System.Windows.Forms.Button btnAsyncDigOut;
        private System.Windows.Forms.Button btnAsyncDac1;
        private System.Windows.Forms.Button btnAsyncDac2;
        private System.Windows.Forms.TextBox edtAsyncDigIn;
        private System.Windows.Forms.TextBox edtAsyncDigOut;
        private System.Windows.Forms.TextBox edtAsyncDac1;
        private System.Windows.Forms.TextBox edtAsyncDac2;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox edtMcuVer;
        private System.Windows.Forms.CheckBox chkEthSupport;
        private System.Windows.Forms.TextBox edtIpAddr;
        private System.Windows.Forms.Button btnOpenByIP;
    }
}

