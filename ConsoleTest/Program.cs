using System;
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
            foreach(var elem in processes)
            {
                Console.WriteLine("{0} {1}",elem.Name,elem.Id);
            }
        }
    }
}
