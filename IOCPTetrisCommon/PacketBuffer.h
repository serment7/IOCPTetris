#pragma once

#include <algorithm>
namespace SBNet
{
	class PacketBuffer
	{
	public:
		void Init(int _bufferSize, int _packetHeaderSize)
		{
			bufferSize = _bufferSize;
			packetHeaderSize = _packetHeaderSize;
			readPos = 0;
			writePos = 0;

			buffer = new char[bufferSize];
			relocateBuffer = new char[bufferSize];
		}

		void Write(char* data, int size)
		{
			if (NeedRelocate(size))
			{
				RelocateBuffer();
			}

			if (data == nullptr || size + writePos > bufferSize - 1)
			{
				return;
			}

			std::copy(data, data + size, buffer + writePos);
			writePos += size;
		}

		char* Read()
		{
			char* result = nullptr;
			int canReadSize = writePos - readPos;
			if (canReadSize < packetHeaderSize)
			{
				return result;
			}

			uint16_t dataSize = buffer[readPos];
			uint16_t test = 0;
			std::copy(buffer + readPos, buffer + readPos + sizeof(uint16_t), reinterpret_cast<char*>(&test));

			if (canReadSize < dataSize)
			{
				return result;
			}

			result = new char[dataSize];
			std::copy(buffer + readPos, buffer + readPos + dataSize, result);

			readPos += dataSize;
			return result;
		}

		bool NeedRelocate(int needPacketSize)
		{
			auto remainWriteSize = bufferSize - writePos;

			// 읽을 수 있는 공간이 가장 큰 패킷보다 작다면
			if (remainWriteSize < needPacketSize)
			{
				return true;
			}

			return false;
		}

		void RelocateBuffer()
		{
			auto remainReadSize = writePos - readPos;
			std::copy(buffer + readPos, buffer + readPos + remainReadSize, relocateBuffer);
			std::copy(relocateBuffer, relocateBuffer + remainReadSize, buffer);

			readPos = 0;
			writePos = remainReadSize;
		}

	private:
		char* buffer = nullptr;
		char* relocateBuffer = nullptr;
		int bufferSize = 0;
		int packetHeaderSize = 0;
		int readPos = 0, writePos = 0;
	};
}