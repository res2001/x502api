using System;
using System.Net;
using lpcieapi;
using x502api;

/* Данный пример представляет из себя консольную программу на языке C#,
   демонстрирующую, как можно выполнить поиск устройств E502 в локальной сети.

   Пример запускается и непрерывно отслеживает появление (или исчезновение)
   устройств в локальной сети, выводя информацию на консоль.

   Поиск выполняется до нажатия любой клавиши. */
namespace e502_eth_svc_browse
{
    class Program
    {
        static void Main(string[] args)
        {
            /* функции для поиска представлены в виде методов отдельного класса EthSvcBrowser */
            E502.EthSvcBrowser sb = new E502.EthSvcBrowser();
            /* запуск поиска сервисов */
            lpcie.Errs err = sb.Start();
            if (err != lpcie.Errs.OK)
            {
                Console.WriteLine("Ошибка запуска поиска устройств в сети {0}: {1}", err, X502.GetErrorString(err));
            }
            else
            {
                bool end = false;
                Console.WriteLine("Запущен поиск устройств в локальной сети. Для останова нажмите любую клавишу");

                while (!end && (err == lpcie.Errs.OK))
                {
                    E502.EthSvcEvent svc_evt;
                    /* Метод EthSvcRecord предоставляет доступ ко функциям для работы с описателем сервиса.
                     * В отличие от С не нужно освобождать память вручную, т.к. освобождение выполняется
                     * в деструкторе */
                    E502.EthSvcRecord svc_rec;
                    err = sb.GetEvent(out svc_rec, out svc_evt, 300);
                    if (err != lpcie.Errs.OK)
                    {
                        Console.WriteLine("Ошибка получения записи о найденном устройстве {0}: {1}", err, X502.GetErrorString(err));
                    }
                    else if (svc_evt != E502.EthSvcEvent.NONE)
                    {                       
                        /* Адрес мы можем получить только для присутствующего устройства */
                        if ((svc_evt == E502.EthSvcEvent.ADD) || (svc_evt == E502.EthSvcEvent.CHANGED))
                        {
                            IPAddress addr;
                            lpcie.Errs cur_err = svc_rec.ResolveIPv4Addr(out addr, 2000);
                            if (cur_err != lpcie.Errs.OK)
                            {
                                Console.WriteLine("Ошибка получения IP-адреса устройтсва {0}: {1}", cur_err, X502.GetErrorString(cur_err));
                            }
                            else 
                            {
                                Console.WriteLine("{0}: {1}, S/N: {2}, Адрес = {3}", svc_evt == E502.EthSvcEvent.ADD ?
                                    "Новое устройтсво" : "Изм. параметров", svc_rec.InstanceName, svc_rec.DevSerial, addr.ToString());
                            }
                        }
                        else if (svc_evt == E502.EthSvcEvent.REMOVE)
                        {
                            Console.WriteLine("Устройство отключено: {0}, S/N: {1}", svc_rec.InstanceName, svc_rec.DevSerial);
                        }                        
                    }

                    /* вход по нажатию клавиши */
                    if (Console.KeyAvailable)
                        end = true;
                }

                lpcie.Errs stop_err = sb.Stop();
                if (stop_err != lpcie.Errs.OK)
                {
                    Console.WriteLine("Ошибка останова поиска сервисов {0}: {1}", stop_err, X502.GetErrorString(stop_err));                    
                }
                else
                {
                    Console.WriteLine("Останов поиска сервисов выполнен успешно!\n");
                }
            }
        }
    }
}
