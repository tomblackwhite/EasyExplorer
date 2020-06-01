#pragma once
#include"Tools.h"
using namespace System;
using namespace System::Runtime::InteropServices;
namespace EasyExplorerLib {
#pragma managed
	public ref class Process
	{
	public:
		
		Process(IntPtr process) :
			m_Process(reinterpret_cast<NativeProcess*>(process.ToPointer())) {};
		
		//Image file
		property String^ Name
		{
			String^ get() {
				return Marshal::PtrToStringUni(
					IntPtr(reinterpret_cast<void*>(m_Process->m_Information->ImageName.Buffer)));
			}
		}

		//process id
		property Int64 Id
		{
			Int64 get() {
				return IntPtr(m_Process->m_Information->UniqueProcessId).ToInt64();
			}
		}
		
	private:
		NativeProcess *m_Process;
	};
	//owned processdata
	public ref class ProcessSet
	{
		
	private:
		static ProcessSet()
		{
			//GetDebugPrivilege
			SetDebugPrivilege();

			//GetDll and Function address
			DllFunction::init();
		}
	public:
		ProcessSet() :m_NativeProcess(new NativeProcessSet) {}

		//free unmanagedClass
		~ProcessSet()
		{
			this->!ProcessSet();
		}

		//Get New Process Information
		bool Refresh()
		{
			return m_NativeProcess->Refresh();
		}
		//return process 
		array<Process^>^ GetProcesses()
		{
			auto processCount = m_NativeProcess->GetProcesses().size();
			auto temp = gcnew array<Process^>(processCount);
			for (int i=0;i<temp->Length;++i)
			{
				temp[i]=gcnew Process(IntPtr(m_NativeProcess->GetProcesses().at(i).get()));
			}
			return temp;
		}
	protected:
		//free unmanagedClass
		!ProcessSet()
		{
			delete m_NativeProcess;
		}
	private:
		NativeProcessSet* m_NativeProcess;
	};
	
	
	
}
