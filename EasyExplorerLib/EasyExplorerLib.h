#pragma once
#include"Tools.h"
using namespace System;
using namespace System::Runtime::InteropServices;
namespace EasyExplorerLib {
#pragma managed
	public ref class Process
	{
	public:
		Process() :m_Process(nullptr) {};
		
		//Image file
		property String^ Name
		{
			String^ get() {
				return Marshal::PtrToStringUni(
					IntPtr(reinterpret_cast<void*>(m_Process->ImageName.Buffer)));
			}
		}

		//process id
		property Int64 Id
		{
			Int64 get() {
				return IntPtr(m_Process->UniqueProcessId).ToInt64();
			}
		}
		//before use have to SetProcess Get Process Information
		//todo change
		void SetProcess(PSYSTEM_PROCESS_INFORMATION process)
		{
			m_Process = process;
		}
	private:
		PSYSTEM_PROCESS_INFORMATION m_Process;
	};
	//owned processdata
	public ref class ProcessSet
	{
		// TODO: 在此处为此类添加方法。
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
				temp[i] = gcnew Process();
				temp[i]->SetProcess(m_NativeProcess->GetProcesses().at(i));
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
