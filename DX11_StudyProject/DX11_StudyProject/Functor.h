#pragma once

template<typename T>
void SAFE_DELETE(T& p_data)
{
	if (nullptr != p_data)
	{
		delete p_data;
		p_data = nullptr;
	}
}

template<typename T>
void SAFE_DELETE_ARRAY(T& p_data)
{
	if (nullptr != p_data)
	{
		delete[] p_data;
		p_data = nullptr;
	}
}