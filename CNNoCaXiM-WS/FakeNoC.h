#pragma once
#include "Links.h"

class FakeNoC : public Links
{
public:
	FakeNoC() = default;

	void runOneStep() override
	{
		for (auto& connection : m_connections)
		{
			while (!connection.m_connection.first->m_outFlitRegister.empty())
			{
				connection.m_connection.second->m_inFlitRegister.push_back(connection.m_connection.first->m_outFlitRegister.front());
				connection.m_connection.first->m_outFlitRegister.pop_front();
				logDebug(" Links: flit transferred (LHS -> RHS) ");
			}

			while (!connection.m_connection.first->m_outCreditRegister.empty())
			{
				connection.m_connection.second->m_inCreditRegister.push_back(connection.m_connection.first->m_outCreditRegister.front());
				connection.m_connection.first->m_outCreditRegister.pop_front();
				logDebug(" Links: credit transferred (LHS -> RHS) ");
			}

			while (!connection.m_connection.second->m_outFlitRegister.empty())
			{
				connection.m_connection.first->m_inFlitRegister.push_back(connection.m_connection.second->m_outFlitRegister.front());
				connection.m_connection.second->m_outFlitRegister.pop_front();
				logDebug(" Links: flit transferred (LHS <- RHS) ");
			}

			while (!connection.m_connection.second->m_outCreditRegister.empty())
			{
				connection.m_connection.first->m_inCreditRegister.push_back(connection.m_connection.second->m_outCreditRegister.front());
				connection.m_connection.second->m_outCreditRegister.pop_front();
				logDebug(" Links: credit transferred (LHS <- RHS) ");
			}
		}
	}

private:
};
