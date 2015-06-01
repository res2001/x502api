unit e502api;
interface
uses Windows, SysUtils, x502api;

  const
   { ������� ������ ������� �������� }
   E502_ETH_SVC_EVENT_NONE     = 0; // �� ������ ������� �� ���������
   E502_ETH_SVC_EVENT_ADD      = 1; // ���������� ��������� ������ �������� �������
   E502_ETH_SVC_EVENT_REMOVE   = 2; // ���������� ������������ ����� ���������� �������� �������
   E502_ETH_SVC_EVENT_CHANGED  = 3;  // ��������� ���������� ����� ������������� �������� �������

    // ��������� ������������ �������� ����������.
  type st_e502_eth_config_state = record
  end;
  type t_e502_eth_config_hnd = ^st_e502_eth_config_state;

  type t_e502_mac_addr = array[0..X502_MAC_ADDR_SIZE-1] of byte;

  //  ��������� ��������� ������ ��������� � ����
  type st_e502_eth_svc_browse_context = record
  end;
  type t_e502_eth_svc_browse_hnd = ^st_e502_eth_svc_browse_context;
  // ��������� �������� �������
  type st_e502_eth_svc_record = record
  end;
  type t_e502_eth_svc_record_hnd = ^st_e502_eth_svc_record;

  //��������� ������ �������� ������� ������� E502, ������������ �� USB
  function E502_UsbGetSerialList(out serials: array of string; flags: LongWord; out devcnt: LongWord) : LongInt; overload;
  function E502_UsbGetSerialList(out serials: array of string; flags: LongWord) : LongInt; overload;
  //�������� ������ E502, ������������� �� USB, �� ��� ��������� ������.
  function E502_OpenUsb(hnd: t_x502_hnd; serial: string): LongInt; stdcall;
  //�������� ������ E502 �� IP-������
  function E502_OpenByIpAddr(hnd : t_x502_hnd; ip_addr: LongWord; flags : LongWord;  tout: LongWord) : LongInt; stdcall;

  //�������� ������ �������, ��������������� ������������ ������� E502
  function E502_UsbGetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord;  out devcnt: LongWord) : LongInt; overload;
  function E502_UsbGetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord) : LongInt; overload;
  //�������� ������ � ���������� � ��������� IP-�������
  function E502_MakeDevRecordByIpAddr(var devrec: t_x502_devrec; ip_addr: LongWord; flags : LongWord;  tout: LongWord) : LongInt; stdcall;
  //�������� ������ � ���������� �� ��������� �������� �������
  function E502_MakeDevRecordByEthSvc(var devrec: t_x502_devrec; svc : t_e502_eth_svc_record_hnd; flags : LongWord;  tout: LongWord) : LongInt; stdcall;


  //��������� �������� IP-������ ����������
  function E502_GetIpAddr(hnd: t_x502_hnd; out ip_addr : LongWord) : LongInt; stdcall;



  // �������� ��������� ������������ �������� ����������.
  function E502_EthConfigCreate() : t_e502_eth_config_hnd; stdcall;
  // ������������ ��������� ������������ �������� ����������.
  function E502_EthConfigFree(cfg: t_e502_eth_config_hnd): LongInt; stdcall;
  // ������ ������� ������� ������������ ����������
  function E502_EthConfigRead(hnd: t_x502_hnd; cfg: t_e502_eth_config_hnd): LongInt; stdcall;
  // ������ ������� ������������ ����������
  function E502_EthConfigWrite(hnd: t_x502_hnd; cfg: t_e502_eth_config_hnd; passwd : string): LongInt; stdcall;
  // ����������� ����������� ������� ������������ ����������
  function E502_EthConfigCopy(src_cfg: t_e502_eth_config_hnd; dst_cfg: t_e502_eth_config_hnd): LongInt; stdcall;
  // �����������, �������� �� ��������� Ethernet
  function E502_EthConfigGetEnabled(cfg: t_e502_eth_config_hnd; out en : LongBool): LongInt; stdcall;
  // ���������� ���������� Ethernet
  function E502_EthConfigSetEnabled(cfg: t_e502_eth_config_hnd; en : LongBool): LongInt; stdcall;
  // �����������, ��������� �� �������������� ��������� ���������� IP
  function E502_EthConfigGetAutoIPEnabled(cfg: t_e502_eth_config_hnd; out en: LongBool): LongInt; stdcall;
  // ���������� ��������������� ��������� ���������� IP
  function E502_EthConfigSetAutoIPEnabled(cfg: t_e502_eth_config_hnd; en: LongBool): LongInt; stdcall;
  // �����������, �������� �� ���������������� MAC-�����
  function E502_EthConfigGetUserMACEnabled(cfg: t_e502_eth_config_hnd; out en: LongBool): LongInt; stdcall;
  // �����������, �������� �� ���������������� MAC-�����
  function E502_EthConfigSetUserMACEnabled(cfg: t_e502_eth_config_hnd; en : LongBool): LongInt; stdcall;
  // ��������� �������������� ������������ IP-������
  function E502_EthConfigGetIPv4Addr(cfg: t_e502_eth_config_hnd; out ip_addr : LongWord): LongInt; stdcall;
  // ��������� ������������ IP-������
  function E502_EthConfigSetIPv4Addr(cfg: t_e502_eth_config_hnd; ip_addr: LongWord): LongInt; stdcall;
  // ��������� ������������� ����������� ����� �������
  function E502_EthConfigGetIPv4Mask(cfg: t_e502_eth_config_hnd; out mask : LongWord): LongInt; stdcall;
  // ��������� ����������� ����� �������
  function E502_EthConfigSetIPv4Mask(cfg: t_e502_eth_config_hnd; mask : LongWord): LongInt; stdcall;
  // ��������� �������������� ������������ ������ �����
  function E502_EthConfigGetIPv4Gate(cfg: t_e502_eth_config_hnd; out gate: LongWord): LongInt; stdcall;
  // ��������� ������������ ������ �����
  function E502_EthConfigSetIPv4Gate(cfg: t_e502_eth_config_hnd; gate: LongWord): LongInt; stdcall;
  // ��������� �������������� ����������������� MAC-������
  function E502_EthConfigGetUserMac(cfg: t_e502_eth_config_hnd; mac : t_e502_mac_addr): LongInt; stdcall;
  // ��������� ����������������� MAC-������
  function E502_EthConfigSetUserMac(cfg: t_e502_eth_config_hnd; mac: t_e502_mac_addr): LongInt; stdcall;
  // ��������� ���������� MAC-������ ����������
  function E502_EthConfigGetFactoryMac(cfg: t_e502_eth_config_hnd; mac : t_e502_mac_addr): LongInt; stdcall;
  // ��������� �������������� ����� ���������� ����������
  function E502_EthConfigGetInstanceName(cfg: t_e502_eth_config_hnd; out name: string): LongInt; stdcall;
  // ��������� ����� ���������� ����������
  function E502_EthConfigSetInstanceName(cfg: t_e502_eth_config_hnd; const name: string): LongInt; stdcall;
  // ��������� ������ ������ ��� ����� ������������
  function E502_EthConfigSetNewPassword(cfg: t_e502_eth_config_hnd; const new_passwd: string): LongInt; stdcall;

  // ������� ������ E502 � ����� ����������
  function E502_SwitchToBootloader(hnd: t_x502_hnd): LongInt; stdcall;
  // ������������ �������� ����
  function E502_ReloadFPGA(hnd: t_x502_hnd): LongInt; stdcall;
  // �������� ����������� ������� ����������� Cortex-M4.
  function E502_CortexExecCmd(hnd: t_x502_hnd; cmd_code: LongWord; par: LongWord;
                                        const snd_data : array of byte; snd_size : LongWord;
                                        rcv_data : array of byte; rcv_size : LongWord;
                                        tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;

  // ������ ������ ������ ������� � ��������� ����
  function E502_EthSvcBrowseStart(out context : t_e502_eth_svc_browse_hnd; flags : LongWord): LongInt; stdcall;
  // ��������� ���������� � ��������� ����������� ������� � ��������� ����
  function E502_EthSvcBrowseGetEvent(context : t_e502_eth_svc_browse_hnd; out svc: t_e502_eth_svc_record_hnd; out event: LongWord; out flags : LongWord; tout : LongWord): LongInt; stdcall;
  // ������� ������ ������ ������� � ��������� ����
  function E502_EthSvcBrowseStop(context : t_e502_eth_svc_browse_hnd): LongInt; stdcall;
  // ������������ ��������� �������� �������
  function E502_EthSvcRecordFree(svc : t_e502_eth_svc_record_hnd): LongInt; stdcall;
  // �������� ��� ���������� �� ��������� �������
  function E502_EthSvcRecordGetInstanceName(svc : t_e502_eth_svc_record_hnd; out name: string): LongInt; stdcall;
  // �������� �������� ����� ������ �� ��������� �������� �������
  function E502_EthSvcRecordGetDevSerial(svc : t_e502_eth_svc_record_hnd; out serial : string): LongInt; stdcall;
  // �������� IP ����� �������� �������
  function E502_EthSvcRecordResolveIPv4Addr(svc : t_e502_eth_svc_record_hnd; out addr :LongWord; tout : LongWord): LongInt; stdcall;
  // ��������, ��������� �� ��� ��������� �� ���� ��������� �������
  function E502_EthSvcRecordIsSameInstance(svc1 : t_e502_eth_svc_record_hnd; svc2 : t_e502_eth_svc_record_hnd): LongInt; stdcall;

implementation

  function _get_serials( ser_arr: p_x502_serial_array; size:LongWord;
                         flags:LongWord; out devcnt: LongWord) : LongInt;
                         stdcall; external 'e502api.dll' name 'E502_UsbGetSerialList';
  function _get_dev_records_list(out list; size:LongWord;
                          flags : LongWord;  out devcnt: LongWord) : LongInt;
                          stdcall; external 'e502api.dll' name 'E502_UsbGetDevRecordsList';

  function _open_usb(hnd: t_x502_hnd; serial: PAnsiChar) : LongInt; stdcall; external 'e502api.dll' name 'E502_OpenUsb';
  function E502_OpenByIpAddr(hnd : t_x502_hnd; ip_addr: LongWord; flags : LongWord;  tout: LongWord) : LongInt; stdcall; external 'e502api.dll';
  function E502_MakeDevRecordByIpAddr(var devrec: t_x502_devrec; ip_addr: LongWord; flags : LongWord;  tout: LongWord) : LongInt; stdcall; external 'e502api.dll';
  function E502_MakeDevRecordByEthSvc(var devrec: t_x502_devrec; svc : t_e502_eth_svc_record_hnd; flags : LongWord;  tout: LongWord) : LongInt; stdcall; external 'e502api.dll';
  function E502_GetIpAddr(hnd: t_x502_hnd; out ip_addr : LongWord) : LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigCreate() : t_e502_eth_config_hnd; stdcall; external 'e502api.dll';
  function E502_EthConfigFree(cfg: t_e502_eth_config_hnd): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigRead(hnd: t_x502_hnd; cfg: t_e502_eth_config_hnd): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigWrite(hnd: t_x502_hnd; cfg: t_e502_eth_config_hnd; passwd : string): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigCopy(src_cfg: t_e502_eth_config_hnd; dst_cfg: t_e502_eth_config_hnd): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetEnabled(cfg: t_e502_eth_config_hnd; out en : LongBool): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigSetEnabled(cfg: t_e502_eth_config_hnd; en : LongBool): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetAutoIPEnabled(cfg: t_e502_eth_config_hnd; out en: LongBool): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigSetAutoIPEnabled(cfg: t_e502_eth_config_hnd; en: LongBool): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetUserMACEnabled(cfg: t_e502_eth_config_hnd; out en: LongBool): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigSetUserMACEnabled(cfg: t_e502_eth_config_hnd; en : LongBool): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetIPv4Addr(cfg: t_e502_eth_config_hnd; out ip_addr : LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigSetIPv4Addr(cfg: t_e502_eth_config_hnd; ip_addr: LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetIPv4Mask(cfg: t_e502_eth_config_hnd; out mask : LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigSetIPv4Mask(cfg: t_e502_eth_config_hnd; mask : LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetIPv4Gate(cfg: t_e502_eth_config_hnd; out gate: LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigSetIPv4Gate(cfg: t_e502_eth_config_hnd; gate: LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetUserMac(cfg: t_e502_eth_config_hnd; mac : t_e502_mac_addr): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigSetUserMac(cfg: t_e502_eth_config_hnd; mac: t_e502_mac_addr): LongInt; stdcall; external 'e502api.dll';
  function E502_EthConfigGetFactoryMac(cfg: t_e502_eth_config_hnd; mac : t_e502_mac_addr): LongInt; stdcall; external 'e502api.dll';

  function _eth_config_get_instance_name(cfg: t_e502_eth_config_hnd; name: PAnsiChar): LongInt; stdcall; external 'e502api.dll' name 'E502_EthConfigGetInstanceName';
  function _eth_config_set_instance_name(cfg: t_e502_eth_config_hnd; name: PAnsiChar): LongInt; stdcall; external 'e502api.dll' name 'E502_EthConfigSetInstanceName';
  function _eth_config_set_new_password(cfg: t_e502_eth_config_hnd; new_passwd: PAnsiChar): LongInt; stdcall; external 'e502api.dll' name 'E502_EthConfigSetNewPassword';

  function E502_SwitchToBootloader(hnd: t_x502_hnd): LongInt; stdcall;  external 'e502api.dll';
  function E502_ReloadFPGA(hnd: t_x502_hnd): LongInt; stdcall;  external 'e502api.dll';
  function _cortex_exec_cmd(hnd: t_x502_hnd; cmd_code: LongWord; par: LongWord;
                            const snd_data; snd_size : LongWord;
                            out rcv_data; rcv_size : LongWord;
                            tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;  external 'e502api.dll' name 'E502_CortexExecCmd';

  function E502_EthSvcBrowseStart(out context : t_e502_eth_svc_browse_hnd; flags : LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthSvcBrowseGetEvent(context : t_e502_eth_svc_browse_hnd; out svc: t_e502_eth_svc_record_hnd; out event: LongWord; out flags : LongWord; tout : LongWord): LongInt; stdcall; external 'e502api.dll';
  function E502_EthSvcBrowseStop(context : t_e502_eth_svc_browse_hnd): LongInt; stdcall; external 'e502api.dll';
  function E502_EthSvcRecordFree(svc : t_e502_eth_svc_record_hnd): LongInt; stdcall; external 'e502api.dll';
  function _eth_svc_record_get_instance_name(svc : t_e502_eth_svc_record_hnd; name: PAnsiChar): LongInt; stdcall; external 'e502api.dll' name 'E502_EthSvcRecordGetInstanceName';
  function _eth_svc_record_get_dev_serial(svc : t_e502_eth_svc_record_hnd; serial : PAnsiChar): LongInt; stdcall; external 'e502api.dll' name 'E502_EthSvcRecordGetDevSerial';
  function E502_EthSvcRecordResolveIPv4Addr(svc : t_e502_eth_svc_record_hnd; out addr :LongWord; tout : LongWord): LongInt; stdcall;  external 'e502api.dll';
  function E502_EthSvcRecordIsSameInstance(svc1 : t_e502_eth_svc_record_hnd; svc2 : t_e502_eth_svc_record_hnd): LongInt; stdcall;  external 'e502api.dll';



  function E502_UsbGetSerialList(out serials: array of string; flags: LongWord; out devcnt: LongWord) : LongInt; overload;
  var
    ser_arr : p_x502_serial_array;
    res, i : LongInt;
  begin
    if (Length(serials) > 0) then
    begin
      ser_arr:=GetMemory(Length(serials)*X502_SERIAL_SIZE);
      // �������� ���������� ��������
      res := _get_serials(ser_arr, Length(serials), flags, devcnt);
      if res >= 0 then
      begin
        //��������� ���������� ������ � ������������ ������ �����
        for i:=0 to res-1 do
          serials[i] := string(ser_arr[i]);
      end;
      //����������� ������, ���������� ��� ��������� ������
      FreeMemory(ser_arr);
    end
    else
    begin
      res:= _get_serials(nil, 0, flags, devcnt);
    end;
    E502_UsbGetSerialList:=res;
  end;

  function E502_UsbGetSerialList(out serials: array of string; flags: LongWord) : LongInt; overload;
  begin
    E502_UsbGetSerialList:= E502_UsbGetSerialList(serials, flags, PCardinal(nil)^);
  end;

  function E502_UsbGetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord;  out devcnt: LongWord) : LongInt; overload;
  begin
    E502_UsbGetDevRecordsList := _get_dev_records_list(list, Length(list), flags, devcnt);
  end;
  function E502_UsbGetDevRecordsList(out list: array of t_x502_devrec; flags : LongWord) : LongInt; overload;
  begin
    E502_UsbGetDevRecordsList:= E502_UsbGetDevRecordsList(list, flags, PCardinal(nil)^);
  end;

  function E502_OpenUsb(hnd: t_x502_hnd; serial: string) : LongInt;
  begin
    E502_OpenUsb:=_open_usb(hnd, PAnsiChar(AnsiString(serial)));
  end;

  function E502_EthConfigGetInstanceName(cfg: t_e502_eth_config_hnd; out name: string): LongInt; stdcall;
  var
    strptr: PAnsiChar;
    res: LongInt;
  begin
    strptr:=GetMemory(X502_INSTANCE_NAME_SIZE);
    res:=_eth_config_get_instance_name(cfg, strptr);
    if res = X502_ERR_OK then
      name:=string(Utf8Decode(strptr));
    FreeMemory(strptr);
    E502_EthConfigGetInstanceName:= res;
  end;


  function E502_EthConfigSetInstanceName(cfg: t_e502_eth_config_hnd; const name: string): LongInt; stdcall;
  begin
    E502_EthConfigSetInstanceName:=_eth_config_set_instance_name(cfg, PAnsiChar(Utf8Encode(AnsiString(name))));
  end;

  function E502_EthConfigSetNewPassword(cfg: t_e502_eth_config_hnd; const new_passwd: string): LongInt; stdcall;
  begin
    E502_EthConfigSetNewPassword:=_eth_config_set_new_password(cfg, PAnsiChar(AnsiString(new_passwd)));
  end;

  function E502_CortexExecCmd(hnd: t_x502_hnd; cmd_code: LongWord; par: LongWord;
                                        const snd_data : array of byte; snd_size : LongWord;
                                        rcv_data : array of byte; rcv_size : LongWord;
                                        tout: LongWord; out recvd_size: LongWord): LongInt; stdcall;
  begin
    if (LongWord(Length(snd_data)) < snd_size) or
       (LongWord(Length(rcv_data)) < rcv_size)  then
      E502_CortexExecCmd := X502_ERR_INSUFFICIENT_ARRAY_SIZE
    else
      E502_CortexExecCmd:=_cortex_exec_cmd(hnd, cmd_code, par, snd_data, snd_size, rcv_data, rcv_size, tout, recvd_size);
  end;

  function E502_EthSvcRecordGetInstanceName(svc : t_e502_eth_svc_record_hnd; out name: string): LongInt; stdcall;
  var
    strptr: PAnsiChar;
    res: LongInt;
  begin
    strptr:=GetMemory(X502_INSTANCE_NAME_SIZE);
    res:=_eth_svc_record_get_instance_name(svc, strptr);
    if res = X502_ERR_OK then
      name:=string(Utf8Decode(strptr));
    FreeMemory(strptr);
    E502_EthSvcRecordGetInstanceName:= res;
  end;

  function E502_EthSvcRecordGetDevSerial(svc : t_e502_eth_svc_record_hnd; out serial : string): LongInt; stdcall;
  var
    strptr: PAnsiChar;
    res: LongInt;
  begin
    strptr:=GetMemory(X502_SERIAL_SIZE);
    res:=_eth_svc_record_get_dev_serial(svc, strptr);
    if res = X502_ERR_OK then
      serial:=string(strptr);
    FreeMemory(strptr);
    E502_EthSvcRecordGetDevSerial:= res;
  end;
end.  
