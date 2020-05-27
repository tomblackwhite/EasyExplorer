using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows.Data;

namespace EasyExplorer
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private static ProcessCollect processCollect = new ProcessCollect();
        

        public static ICollection<ProcessInformation> processList
        {
            get { return processCollect.GetProcesses(); }
        }
        
    }
}
