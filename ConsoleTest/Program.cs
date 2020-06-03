using System;
using System.Linq;
using EasyExplorerLib;
namespace ConsoleTest
{
    class Program
    {
        static void Main(string[] args)
        {
            var processSet = new ProcessSet();
            bool result=processSet.Refresh();
            if(!result)
            {
                Console.WriteLine("Get process error");
            }
            var processes = processSet.GetProcesses();
            //foreach(var elem in processes)
            //{
            //    Console.WriteLine("{0} {1}",elem.Name,elem.Id);
            //}
            processSet.RefreshOwnedHandles();
            var handles = processSet.GetAllHandles();

            var handlesSet = from elem in handles
                             where elem.UniqueProcessId.ToInt64() == 17896 || elem.UniqueProcessId.ToInt64() == 10716
                             select elem;
            var sameAddressHandles = from elem in handlesSet
                                     group elem by elem.ObjectAddress.ToInt64();

            foreach(var addresses in sameAddressHandles)
            {
                foreach(var elem in addresses)
                {
                    Console.WriteLine("0x{0:x} {1} {2}",
                        elem.ObjectAddress.ToInt64(),
                        elem.UniqueProcessId.ToInt64(),
                        elem.HandleValue.ToInt64());
                    
                }
                Console.WriteLine();
            }

            //foreach(var elem in handlesSet)
            //{
            //    Console.WriteLine("0x{0:x} {1} {2}",
            //        elem.ObjectAddress.ToInt64(),
            //        elem.UniqueProcessId.ToInt64(),
            //        elem.HandleValue.ToInt64());
            //}

            //foreach (var elem in handles)
            //{
            //    if(elem.UniqueProcessId.ToInt64()==17896||elem.UniqueProcessId.ToInt64()==10716)
            //    Console.WriteLine("0x{0:x} {1} {2}", 
            //        elem.ObjectAddress.ToInt64(),
            //        elem.UniqueProcessId.ToInt64(),
            //        elem.HandleValue.ToInt64());
            //}
        }
    }
}
