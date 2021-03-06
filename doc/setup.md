Подключение библиотеки {#sect_setup}
==============================================================

Для написания собственного программного обеспечения, работающего с модулями L-502 и E-502, необходимо выполнить следующее:
1. Установить драйвер для модулей:
      - Для работы с модулями, подключенными по интерфейсу PCI Express, необходим специальный драйвер, предоставляемый фирмой "Л Кард" 
      - Для работы с модулями, подключенными по интерфейсу USB, используется библиотека [libusb-1.0](http://libusb.info). Под Windows сама  библиотека включена в стандартную библиотеку \filename{e502api.dll}, а в качестве драйвера используется стандртный драйвер [WinUSB](https://msdn.microsoft.com/en-us/library/windows/hardware/ff540196%28v=vs.85%29.aspx). Под ОС Linux необходимо установить библиотеку [libusb-1.0](http://libusb.info), при этом никаких специальных драйверов не требуется.
      - Для работы с модулями, подключенными по интерфейсу Ethernet (TCP/IP), специальных драйверов не требуется. Для возможности выполнять поиск модулей в локальной сети должна быть установлена соответствующая служба (см. главу @ref sect_eth_browsing).
2. Установить необходимые динамические библиотеки  (.dll для Windows или .so для Linux) в директорию, присутствующую в соответствующей переменной окружения, либо в директорию с проектом. Динамическая библиотека необходима при написании программ на любом языке программирования, так как все привязки к языкам работают через указанные библиотеки.. Всего предоставляется три библиотеки:
      - \filename{x502api} --- содержит общие функции для обоих модулей. Должна включаться в любой проект, работающий с одним из модулей, за исплючением проектов, написанных только для L-502 до появления библиотеки x502api, которые могут использовать только l502api
      - \filename{l502api} --- содержит специфические функции для модуля L-502, а также функции, оставленные для совместимости с проектами, написанными до появления x502api.
      - \filename{e502api} --- содержитспецифические функции для модуля E-502.
3. Подключить библиотеку к проекту.

Для ОС Windows предоставляется общий установщик @lpcie_sdk, который  автоматически устанавливает все необходимые драйвера, динамические библиотеки в системную директорию, а также все файлы, необходимые для подключения библиотеки к проекту и примеры в указанную директорию. В дальнейшем в данной главе с помощью @lpcie_sdk_dir будет обозначаться указанная при установке @lpcie_sdk директория.

Установка для ОС Linux описана в разделе @ref sect_setup_linux.

Подключение к проекту зависит от используемого языка и среды программирования.

Подключение библиотеки при написании программы на языках C/C++ {#sect_setup_c}
=========================================================================

При написании на C/С++ при подключении необходимо выполнить следующее:
1. Включить в проект заголовочный файл "l502api.h" и/или "e502api.h", добавив при этом в проекте к путям для заголовочных файлов директорию @lpcie_sdk_dir/include.
2. Добавить в проект файл линкера нужных библиотек для используемого компилятора:
    - <B> Microsoft Visual С++ </B>: @lpcie_sdk_dir/lib/msvc
    - <B> Microsoft Visual С++ </B> 64-битный компилятор (подробнее о 64-битной версии [описано ниже](@ref sect_setup_64)): @lpcie_sdk_dir/lib/msvc64
    - <B> Borland C++/Borland C++ Builder </B>: @lpcie_sdk_dir/lib/borland
    - <B> Borland C++/Borland C++ Builder </B> 64-битный компилятор: @lpcie_sdk_dir/lib/borland64
    - <B> MinGW </B>: @lpcie_sdk_dir/lib/mingw
    - <B> MinGW </B> 64-битный компилятор: @lpcie_sdk_dir/lib/mingw64

 Примеры программ на @langname{C} находятся  в @filename{@lpcie_sdk_dir/examples/c}. Специальные примеры для <B> Borland C++ Builder </B> --- в @filename{@lpcie_sdk_dir/examples/CppBuilder}.


Использование библиотеки в проекте на Delphi {#sect_setup_delphi}
=========================================================================

Для написания программ на @langname{Delphi} с использованием библиотеки для работы с модулями L-502 и E-502, необходимо включить в проект программы файл @filename{@lpcie_sdk_dir/pas/x502api.pas}, а также файл @filename{@lpcie_sdk_dir/pas/l502api.pas} и/или @filename{@lpcie_sdk_dir/pas/e502api.pas}, которые представляют собой обертку над библиотеками на @langname{C}. 
В файлах проекта, которые  используют типы и функции из этого документа, необходимо подключить модуль x502api, а также l502api и/или e502api с помощью `uses x502api;`, `uses l502api;` и `uses e502api;` соответственно. При этом для 64-битного компилятора используются те же файлы, что и для 32-битного (см. @ref sect_setup_64).

Следует отметить, что по сравнению с версиями до поддержки E-502, файл @lpcie_sdk_dir/pas/lpcieapi.pas больше не используется. Т.к. этот файл не должен был использоваться в проектах напрямую, то он не включен в новых версиях библиотеки.

Все функции, типы и константы библиотеки отображаются в @langname{Delphi} один к одному, за исключением следующих моментов:
- все строки (серийные номера, строки с описанием кодов ошибок) преобразуются оберткой в тип `string`, который используется стандартно для представления строк в @langname{Delphi} (следует не забывать, что в последних версиях среды этот тип представляет собой юникодную строку). Исключением является структура #t_x502_info с информацией о модуле, в которой строки представлены массивом `AnsiChar` фиксированной длины.
- все функции, работающие с массивами, принимают в качестве параметра открытый массив (open array parameter), что означает, что в эти функции можно передать как статический массив, так и динамический (установив предварительно его длину с помощью `SetLength()`). При этом, так как массивы Delphi содержат в себе длину, то в функции L502_GetSerialList() и E502_UsbGetSerialList(), а также L502_GetDevRecordsList() и E502_UsbGetDevRecordsList() отдельно передавать размер массива не требуется. Однако в функциях для работы с данными (например, X502_Recv()) длина передается так же, как и в функциях @langname{C}, чтобы можно было использовать для приема не обязательно весь массив. При этом дополнительно проверяется, что переданная отдельным параметром длина не превышает реальную длину массива. В случае превышения будет возвращена ошибка #X502_ERR_INSUFFICIENT_ARRAY_SIZE.

 Пример программы на @langname{Delphi} находится в @filename{@lpcie_sdk_dir/examples/Delphi}.

Использование библиотеки в проекте на C# {#sect_setup_sharp}
=========================================================================

Для написания программ, работающих с модулями L-502 и E-502, на языке @langname{C#} (или на любом другом, поддерживаемым NetFramework), реализована специальная библиотека-обертка lpcieNet.dll. Она использует описанные ранее библиотеки на @langname{C}, в которых реализована вся логика работы с устройством.
Установщик позволяет установить lpcieNet.dll в системный кэш (GAC), что позволяет не копировать библиотеку вместе с Вашим проектом. Однако, к сожалению, Visual Studio не позволяет добавлять в проект ссылки из системного кеша и Вам все равно придется саму ссылку делать на локальную копию (просто ее не обязательно будет распространять вместе с проектом). Библиотека в кеше имеет преимущество перед локальной и будет всегда использованна именно она, если установлена.

Для использования библиотеки достаточно добавить в проект ссылку на lpcieNet.dll и в исходниках подключить нужные пространства имен:

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cs}        
  using x502api;
  using lpcieapi;
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
Для совместимости с программами, написанными до включения поддержки L-502, оставлена старая версия всех определений в пространстве имен l502api со старой версией класса L502.

По сравнению с функциями языка @langname{C} в обертке @langname{C#} сделаны следующие изменения:
 - Так как @langname{С#} --- объектно ориентированный язык, то для управления модулями созданы специальные классы L502 и E502, которые наследуются от общего класса X502. 
 - Все функции, которые принимают описатель модуля первым параметром реализованы методами классов L502, E502 или X502, при этом сам префикс L502_, E502_ или X502_ не используется.
   Например, вместо `L502_Open(hnd, serial)`, используется `hnd.Open(serial)`. Для сетевой конфигурации модуля E-502 используется отдельный класс E502.EthConfig.
 - Функции X502_Create() и X502_Free() вызывается в конструкторе и деструкторе класса X502, отдельными функциями не реализованы. Аналогично обстоит дело c X502_FreeDevRecordList() и классом X502.DevRec, а также с функциями E502_EthConfigCreate() и E502_EthConfigFree() и классом E502.EthConfig.
 - Для создания нужного объекта устройства (L502 или E502) по имени модуля реализован статический метод `X502.Create(devname)`.
 - Функции, которые не требуют экземпляра модуля (не принимают описатель первым параметром), реализованы как статические функции соответствующих классов. Например `X502_GetErrorString(err)` раеализована как `X502.GetErrorString(err)`.
 - Функции типа Get/Set, которые принимают описатель модуля и один параметр, реализованы в виде свойств (properties). Например, вместо `X502_SetLChannelCount(hnd, value)`, используется `hnd.LChannelCount = value`. Однако следует быть внимательным, так как неправильно устанавливаемое значение вызовет исключение `X502.Exception`.
 - Константы объявлены внутри классов и без префикса L502_, E502_ или X502_.
 - Перечисления также объявлены как перечисления внутри класса и без префикса `X502_ПЕРЕЧИСЛЕНИЕ`. Например не #X502_SYNC_INTERNAL, а `X502.Sync.INTERNAL`.
 - Коды ошибок, так как планируется использовать общие коды ошибок и для будущих модулей, вынесены в перечисление ERR в классе lpcie.
 - Все функции, использующие в C строки в виде `char *`, используют в обертке строки типа `String`.
 - Функции `L502.GetSerialList()`, `E502.UsbGetSerialList()`, а также функции `L502.GetDevRecordsList()` и `E502.UsbGetDevRecordsList()` возвращают созданный внутри функции динамический массив строк/записей о устройстве (а не заполняет переданный), который уже содержит длину. Поэтому дополнительный параметр размера массива не требуется.
 - Как и в Delphi, при работе с массивами данных передается длина дополнительным параметром, так как можно принимать меньше данных, чем в выделенном массиве.
 - Функции, принимающие указатели в @langname{C}, принимают параметры со спецификаторами out или ref, в зависимости от того, должна ли быть переменная проинциализирована перед вызовом функции или является выходным параметром.

 Пример программы на @langname{C#} находится в @filename{@lpcie_sdk_dir/examples/cs}.
   

Использование библиотеки в проекте LabView {#sect_setup_labview}
=========================================================================

Вы можете управлять модулями L-502 и E-502 из @langname{LabView}, используя тот факт, что @langname{LabView} поддерживает управляемые библиотеки NetFramework. Соответственно, Вы имеете доступ ко всем функциям, которые реализует оболочка @langname{C#} lpcieNet.dll, т.е. все доступные функции библиотеки, с учетом отличий, описанных в [предыдущем разделе](@ref sect_setup_sharp)).

В отличие от Visual Studio LabView автоматически подхватывает .Net библиотеки из системного кэша (GAC) и Вы можете ссылаться на нее, а не хранить локальную копию вместе с программой.

Для работы с классами .Net в LabView есть специальная панель <B> Connectivity -> .Net </B>.

Вам необходимо использовать следующие блоки:
 - <B> Constructor Node </B> - создает объект. Должен быть создан для каждого модуля L-502 или E-502, с которым будете работать. При создании LabView предложит выбрать библиотеку и класс (нужно выбрать lpcieNet.dll и L-502 или E-502 из пространства x502api). Одним из выходов этого блока является ссылка на объект, которая используются как вход для остальных блоков для работы с модулем. Также с помощью конструктора создается объект логического канала со всеми настройками. Альтернативным вариантом создания объекта модуля является использование метода X502.Create, который создает нужный объект по имени модуля.
 - <B> Close Reference </B> - закрывает и удаляет объект. Должен вызываться для каждого созданного объекта по завершению работы.
 - <B> Invoke Node </B> - вызов функции (метода класса). При работе с объектом на вход подается ссылка и входные параметры, а на выходе --- выходные параметры и обновленная ссылка (которая должна использоваться для блоков, которые будут вызваны после текущего). Входная ссылка и определяет, методы какого объекта используются (после заведения ссылки на вход имя класса появится в верхней строке, а при нажатии на вторую будет предложен на выбор его метод). Для функций, которые не работают с конкретным объектом (GetErrorString, GetSerialList и т.д. --- эти функции статические и при выборе помечены [S] в начале) ссылку на вход подавать не обязательно. Однако они все равно принадлежат классу, который надо выбрать нажав правой кнопкой на блок и далее <B> Select Class/.Net </B>
 - <B> Property Node </B> - используется для установки или получения свойств. Через свойства устанавливается часть параметров (логическая таблица, режим синхронизации) и также можно получить информацию о модуле (в виде класса, каждое поле которого является также отдельным свойством). Можно устанавливать несколько свойств одним блоком, расширив его вниз. Также с помощью свойств можно задавать константы из перечислений (что может быть более понятно, чем просто подавать на вход числа) - в этом случае надо выбрать класс перечисление, а каждому значению соответствует свое свойство.
 
 Следует отметить особенность передачи массивов в качестве выходных параметров. Сами функции библиотеки для эффективности не выделяют массивов данных внутри себя, а используют переданные массивы для сохранения результатов. Поэтому такие параметры в @langname{LabView} являются входными и выходными одновременно.  На вход необходимо подать массив размера, достаточного для сохранения результатов (при этом сами данные не имеют значения), а в качестве выходного параметра возвращается тот же массив, но уже заполненный результатами выполнения функции. Примером таких параметров могут служить параметр <B> buf </B> функции X502_Recv(), параметры <B> adc_data </B> и <B> din_data </B> функции X502_ProcessData(), а также параметр <B> out_buf </B> функции X502_PrepareData().
 
Примеры программ на @langname{LabView} находится в @filename{@lpcie_sdk_dir/examples/LabView}.

Использование библиотеки в  Visual Basic 6 {#sect_setup_vb6}
=========================================================================
Для работы с модулями из программы на @langname{Visual Basic 6} нужно в проект добавить файлы модулей @filename{x502api.bas}, @filename{e502api.bas} и @filename{l502api.bas}, которые можно взять из примера @filename{@lpcie_sdk_dir/examples/vb6/x502_general}. Файлы содержат объявления всех типов, констант и функций.
Параметры функций совпадают с функциями языка @langname{C}, за исключением следующих особенностей:
- так как в Visual Basic не используются указатели, то для всех описателей используется просто тип `Long`
- Функции L502_GetSerialList(), E502_UsbGetSerialList(), а также функции L502_GetDevRecordsList() и E502_UsbGetDevRecordsList() принимают в качестве параметра динамический массив и изменяют его размер в соответствии с количеством найденных элементов.  Соответственно, дополнительные явные параметры, задающие размер массива на вход и количество найденных элементов не требуются.
- Строки в функциях преобразовываются в строки Visual Basic автоматически. Строки же в структурах представлены  массивом байт. Для перевода их в String можно воспользоваться функцией `X502_StrConvert()`
- Для освобождения ресурсов одной записи дополнительно введена функция `X502_FreeDevRecord()`, вызывающая X502_FreeDevRecordList() для одного элемента, чтобы не преобразовывать элемент в массив вручную

Следует отметить, что при отладке из среды @langname{Visaul Basic 6} при останове незавершенной программы, если на момент останова были незакрытые соединения с модулями, то эти соединения могут не закрываться автоматически до перезапуска среды. Соответственно, т.к. количество одновременных соединений ограничено, то модуль может быть не видим в списке найденных устройств или с ним нельзя будет установить соединение до перезапуска среды.


64-битная версия библиотеки {#sect_setup_64}
=========================================================================
На 64-биной версии Windows могут выполняться программы, как собранные 32-битным, так и 64-битным компилятором, поэтому большинство программ для Windows существует только в 32-битном варианте. 64-битный компилятор используют как правило для программ, работающих с большим количеством данных, так как это позволяет иметь процессу виртуальное пространство больше 4 Гбайт.

Для 64-битной Windows установщик @lpcie_sdk ставит в соответствующие системные директории  32-битную версию библиотек, так и 64-битную.  При этом директория @filename{ Windows/system32} указывает на одну из этих директорий в зависимости от разрядности самого приложения, которое обращается по указанному пути. Для 32-битного приложения в @filename{Windows/system32} находятся 32-битные библиотеки, а 64-битные в@filename{ Windows/Sysnative}, а для 64-битного в @filename{Windows/system32} находятся 64-битные, а @filename{ Windows/Sysnative} не существует. При этом в @filename{Windows/SysWOW64} всегда лежат 32-битные библиотеки и она всегда существует независимо от разрядности приложения.

При загрузке приложения, если используются системные библиотеки, то они ищутся по путям из переменной окружения PATH, среди которых есть @filename{Windows/system32}. Так как эта директория ссылается на разные места в зависимости от разрядности запускаемого приложения, то выбор библиотеки нужной разрядности из @filename{Windows/system32} происходит автоматически. Если библиотеки распространяются вместе с программой, то нужно следить, чтобы разрядность собранного приложения и библиотек в одной директории совпадала.

Единственным отличием при написании программ на @langname{C или C++} является необходимость подключить lib-файл в соответствии с разрядностью используемого компилятора.

Для программ на @langname{Delphi} необходимо только указать, для какой платформы будет собираться проект (win32 или win64) и собранная программа будет использовать библиотеку той разрядности, для которой программа была скомпилирована.

Программы на языке @langname{C#}  (или на любом другом, использующем NetFramework)  компилируются в машинный код при выполнении. При этом один раз созданная программа может выполняться как на 32-битной версии, так и на 64-битной версии виртуальной машины NetFramework (в проекте можно указать явно, для какой разрядности NetFramework предназначена программа). Таким образом, одна и та же программа в 32-битной версии Windows будет выполняться с использованием 32-битной версии библиотек, а в 64-битной --- с использованием 64-битной. Для библиотеки .Net разрядность определяется разрядностью использующего его приложения.

Соответственно, в проекте на @langname{LabView}, который использует .Net библиотеку,  разрядность используемой библиотеки определяется разрядностью используемой среды @langname{LabView}.

Установка библиотеки и драйвера для ОС Linux {#sect_setup_linux}
=========================================================================
Для установки драйвера и библиотеки под ОС Linux  существует два варианта:
  - Воспользоваться готовыми собранными пакетами, предоставляемыми "Л Кард". Это рекомендованный способ для дистрибутивов, для которых предоставляются собранные пакеты. Список поддерживаемых дистрибутивов можно посмотреть в документе [\"Использование внешних репозиториев 'Л Кард' для дистрибутивов Linux\"](http://www.lcard.ru/download/lcard_linux_distributions.pdf)
  - Скачать исходные коды @lpcie_sdk и собрать их самостоятельно (подробнее [в следующем разделе](@ref sect_sdk_src)).

Для примеров работы с модулями L-502 и E-502 на @langname{C} под Linux  можно скачать архив с исходниками SDK и посмотреть на примеры в  @filename{api/x502api/examples/msvc}. Несмотря на название, они могут быть собраны GCC под ОС Linux. Для каждого примера есть makefile (с комментариями), а также файл CMakeList.txt для предпочитающих сборку с использованием cmake .


О том, как подключить внешний репозиторий, установить собранные пакеты и о преимуществах данного метода установки описано в документе [\"Использование внешних репозиториев 'Л Кард' для дистрибутивов Linux\"](http://www.lcard.ru/download/lcard_linux_distributions.pdf). Здесь же будет приведен список самих пакетов, использующихся при работе с модулями L-502 и E-502, с указанием зависимостей. При подключении внешнего репозитория зависимости разрешаются автоматически (за исключением пакета @filename{lpcie-dkms} , о чем описано ниже).  При установке пакетов вручную без подключения внешнего репозитория, следует учитывать зависимости при установке пакетов (например библиотеки следует ставить в следующем порядке: сперва @filename{libx502api1}, затем @filename{libl502api1} и @filename{libe502api1}, и только затем при необходимости @filename{libx502api1-dev} или @filename{libx502api1-devel}).


Для работы с модулями L-502 и E-502 используются пакеты:
  - @filename{libx502api1-dev} или @filename{libx502api1-devel} --- Пакет с файлами для разработчика: заголовочные файлы и ссылки на библиотеки нужной версии. Нужен при написании своих программ с использованием описанных в данном документе библиотек (зависит от @filename{libx502api1}, @filename{libl502api1} и @filename{libe502api1}, благодаря чему сами библиотеки тоже ставятся автоматически при установке файлов разработчика)
  - @filename{libx502api1}, @filename{libl502api1} и @filename{libe502api1} --- Пакеты непосредственно с библиотеками нужной версии. Если вы распространяете свою программу, то Вам достаточно включить в зависимости только пакет, соответствующие используемым библиотекам (без пакета с файлами для разработчика), что при создании rpm и deb пакетов выполняется автоматически. Пакет @filename{libx502api1} содержит библиотеку общих функций, которая используются в @filename{libl502api1} и @filename{libe502api1}, поэтому последние зависят от первой.  Пакет @filename{libe502api1} также зависит от пакета с библиотекой [libusb-1.0](http://libusb.info) для данного дистрибутива, чтобы она устанавливалась автоматически, а также ставит правила udev для предоставления прав доступа к устройству E-502, подключенного по USB.
  - @filename{lpcie-dkms} --- Пакет с исходниками драйвера (модуля ядра) для работы с модулями по интерфейсу PCI-Express (L-502), использующий систему сборки внешних модулей dkms (подробнее описано ниже).
  - @filename{lxfw-update} --- Утилита для обновления прошивок ПЛИС модулей L-502 и E-502. Пакет включает в себя последнюю версию прошивок и скрипты l502-fpga-update-all.sh и e502-fpga-update-all.sh для обновления прошивок всех найденных устройств L-502 или E-502 соответственно.

  Так как драйвер должен быть собран под конкретную версию ядра (а ядро может обновляться в одной версии дистрибутива или даже могут использоваться разные варианты ядра), то драйвер не может распространятся в уже собранном виде. Его сборка выполняется непосредственно при установке пакета. При этом необходимо предварительно поставить пакет с заголовочными файлами текущего ядра (обычно в пакетах с именами @filename{linux-headers} или @filename{kernel-devel}). Для некоторых дистрибутивов может быть несколько вариантов ядра (и соответственно несколько пакетов), более того Вы можете использовать свое ядро. Именно по этой причине пакет не задан зависимостью для @filename{lpcie-dkms}, в отличие от других зависимостей. Узнать текущую версию ядра можно командой: `uname -r`. Убедиться, что нужные файлы установлены можно проверив наличие файлов в директории @filename{/lib/modules/}\`uname -r\`@filename{/build} (обычно это ссылка на заголовки ядра в @filename{/usr/src/linux-\<version\>} или @filename{/usr/src/kernels/\<version\>}).

  Если заголовки текущего ядра установлены, то при установке пакета @filename{lpcie-dkms} будет выполнена сборка драйверов с использованием DKMS (пакет dkms входит в зависимости @filename{lpcie-dkms}, как и make и gcc, необходимые для сборки). DKMS это достаточно широко распространенная система сборки и управления внешними модулями ядра (она находится в основном репозитории большинства дистрибутивов Linux, хотя ее нет в OpenSuse, но для нее распространяется  пакет dkms через тот же Open Build System).

  DKMS позволяет:
       - централизованно отслеживать, какие сторонние модули ядра, какие их версии и для каких версий ядра установлены (dkms status)
       - хранит всегда исходники драйвера разных версий в централизованном месте (@filename{/usr/src/lpcie-\<version\>})
       - позволяет автоматически пересобирать драйвер при переходе на новое ядро
       - позволяет в любой момент удалить драйвер или любую его версию и все связанные с ним файлы (dkms remove -m lpcie -v \<version\> --all)

  Таким образом, хотя в пакете находятся исходники драйвера, а не собранный драйвер, установка мало чем отличается от установки других пакетов, кроме того, что требует дополнительной установки заголовочных файлов ядра и установка пакета требует значительного времени на сборку.

  При установки нового ядра модуль будет пересобран под него автоматически либо при установке пакета, либо при первом входе в систему с новым ядром.  
  
Исходные коды SDK {#sect_sdk_src}
=========================================================================

Исходные коды всех составных компонентов SDK открыты. Пользователю предоставляется доступ на чтение к репозиторию системы контроля версий [Mercurial](http://mercurial.selenic.com), расположенному по адресу https://bitbucket.org/lcard/lpcie_sdk. Подробнее об использовании открытых репозиториев исходных кодов "Л Кард" на bitbucket.org Вы можете прочитать в документе [\"Использование открытых репозиториев исходных кодов 'Л Кард' на bitbucket.org\"](http://www.lcard.ru/download/lcard_bitbucket_repos.pdf).

Вы также можете скачать архив lpcie_sdk_src.zip со всеми исходными кодами из [прикрепленных файлов проекта репозитория](https://bitbucket.org/lcard/lpcie_sdk/downloads).

Инструкции по сборке находятся в файле исходников INSTALL.txt.
