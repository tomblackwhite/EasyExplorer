using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Diagnostics;
using System.Collections.ObjectModel;
namespace EasyExplorer
{
    public class ProcessInformation :INotifyPropertyChanged
    {
        public ProcessInformation(Process p)
        {
            process = p;
        }
        public event PropertyChangedEventHandler PropertyChanged;

        public void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, e);
        }
        private Process process;

        public int Id
        {
            get
            {
                return process.Id;
            }
        }

        public string Name
        {
            get { return process.ProcessName; }
        }

        public ProcessModule MainModule
        {
            get { return process.MainModule; }
        }
        
        public string WorkingSet
        {
            get 
            {
                double re = (double)process.WorkingSet64 / (1024 * 1024);
                
                return string.Format("{0:F2}MB", re);
            }
        }
        public string ModuleName
        {
            get { return MainModule.ModuleName; }
        }
        public string FileName
        {
            get { return MainModule.FileName; }
        }
    }

    public class ProcessCollect
    {
        public ProcessCollect()
        { }
        public ICollection<ProcessInformation> GetProcesses()
        {
            ObservableCollection<ProcessInformation> processes = new ObservableCollection<ProcessInformation>();
            Process[] tempProcesses = Process.GetProcesses();
            foreach(var elem in tempProcesses)
            {
                processes.Add(new ProcessInformation(elem));
            }
            return processes;
        }
    }
}
