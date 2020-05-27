#include "pch.h"
#include "Tools.h"
namespace EasyExplorerLib {
	


	bool NativeProcessSet::Refresh()
	{
		m_Buffer.reset();
		auto status = GetSystemInformation(m_Buffer,m_NtQuerySystemInformation,SystemProcessInformation);
		if (m_Buffer == nullptr || (!NT_SUCCESS(status)))
			return false;
		auto buffer = m_Buffer.get();
		m_Processes.clear();

		PSYSTEM_PROCESS_INFORMATION current=nullptr;
		do
		{
			current = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(buffer);
			m_Processes.push_back(current);
		} while ((current->NextEntryOffset) && (buffer += current->NextEntryOffset, true));

		return true;
	}


	bool NativeProcessSet::RefreshOwnedHandles()
	{
		std::unique_ptr<std::byte[]> tempHandlesBuffer;
		auto status = GetSystemInformation(tempHandlesBuffer,m_NtQueryInformationProcess,, );
		
		if (tempHandlesBuffer == nullptr||(!NT_SUCCESS(status)))
			return false;

		m_OwnedHandles = std::move(tempHandlesBuffer);
		
		

		return false;
	}

	//NativeProcess
	bool NativeProcess::RefreshOwnedHandles()
	{
		if(m_ProcessHandle==nullptr)

		return false;
	}
}